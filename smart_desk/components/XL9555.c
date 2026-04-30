#include "XL9555.h"

/*============================ ESP-IDF 头文件 ============================*/
#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "hal/gpio_types.h"
#include "hal/i2c_types.h"
#include "portmacro.h"
#include "soc/clk_tree_defs.h"


#define XL9555ADDR      0x20
#define XL9555_ISR_BIT  BIT0

static i2c_master_bus_handle_t XL9555_bus_Handle = NULL;
static i2c_master_dev_handle_t XL9555_dev_Handle = NULL;

static EventGroupHandle_t   XL9555_IO_Event = NULL;
static XL9555_Input_callback_t  XL9555_IO_CallBack = NULL;

static gpio_num_t XL9555_INT;


/**
 * @brief XL9555中断处理函数
 *
 * @param param 中断参数
 */
static void IRAM_ATTR XL9555_INT_Handle(void * param)
{
    BaseType_t taskWake;
    // 从中断发出事件
    xEventGroupSetBitsFromISR(XL9555_IO_Event, XL9555_ISR_BIT, &taskWake);

    // 触发任务唤醒
    portYIELD_FROM_ISR(taskWake);
}



/**
 * @brief XL9555中断任务处理函数
 *
 * @param param 任务参数
 */
void XL9555_Task(void * param)
{
    EventBits_t ev;
    uint16_t Last_Input;

    // 记录初始输入状态
    XL9555_Read_Word(0x00, &Last_Input);

    while (1)
    {
        // 等待中断事件
        ev = xEventGroupWaitBits(XL9555_IO_Event, XL9555_ISR_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

        if(ev & XL9555_ISR_BIT)        // 触发了指定事件
        {
            uint16_t Input = 0;

            // 延时减少外部毛刺干扰
            esp_rom_delay_us(1000);

            // 再次读取IO口判断是不是干扰
            if(gpio_get_level(XL9555_INT) != 0)
            {
                continue;
            }

            // 读取当前输入状态（这会清除XL9555的中断标志）
            esp_err_t ret = XL9555_Read_Word(0x00, &Input);
            if(ret == ESP_OK)
            {
                // 遍历所有引脚，检测变化
                for(int i = 0; i < 16; i++)
                {
                    uint8_t Last_value = Last_Input & (1 << i)?1:0;
                    uint8_t value = Input&(1<<i)?1:0;

                    // 如果引脚状态发生变化，调用传入的回调函数
                    if(value != Last_value)
                    {
                        XL9555_IO_CallBack((1<<i), value);      //将变化的引脚和电平传入回调函数
                    }
                }

                // 更新上次输入状态
                Last_Input = Input;
            }
        }
    }
}



/**
 * @brief 初始化XL9555 I2C扩展芯片
 *
 * @param SDA      I2C数据引脚
 * @param SCL      I2C时钟引脚
 * @param INT      中断引脚（不使用中断则传GPIO_NUM_NC）
 * @param f        输入引脚变化回调函数
 * @return         esp_err_t ESP_OK成功，其他失败
 */
esp_err_t XL9555_Init(gpio_num_t SDA,gpio_num_t SCL, gpio_num_t INT, XL9555_Input_callback_t f)
{
    XL9555_INT = INT;
    // 保存回调函数，给Xl9555使用
    XL9555_IO_CallBack = f;

    // I2C总线配置
    i2c_master_bus_config_t i2c_bus_config =
    {
        .clk_source = I2C_CLK_SRC_DEFAULT,    // I2C时钟源
        .sda_io_num = SDA,                    // SDA引脚
        .scl_io_num = SCL,                    // SCL引脚
        .glitch_ignore_cnt = 7,               // glitch消除计数
        .i2c_port = 1,                        // I2C端口号
    };

    // 创建I2C主机总线
    i2c_new_master_bus(&i2c_bus_config, &XL9555_bus_Handle);

    // 设备配置
    i2c_device_config_t dev_config =
    {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7, // 设备地址长度（7位）
        .device_address = XL9555ADDR,          // 设备I2C地址
        .scl_speed_hz = 100000,                // SCL时钟频率（100KHz）
    };

    // 添加设备到总线
    i2c_master_bus_add_device(XL9555_bus_Handle, &dev_config, &XL9555_dev_Handle);

    // 配置中断引脚（如果需要）
    if(INT != GPIO_NUM_NC)
    {
        XL9555_IO_Event = xEventGroupCreate();  // 创建事件组，如果引脚电平变换，就会进入中断中给事件组置位

        // 中断引脚配置
        gpio_config_t INT_Config =
        {
            .intr_type = GPIO_INTR_NEGEDGE,     // 下降沿中断，因为INT低电平时才是中断产生
            .mode = GPIO_MODE_INPUT,            // 输入模式
            .pull_up_en = GPIO_PULLUP_ENABLE,   // 使能上拉
            .pull_down_en = GPIO_PULLDOWN_DISABLE, // 禁用下拉
            .pin_bit_mask = (1 << INT),         // GPIO位掩码
        };
        gpio_config(&INT_Config);               // 配置GPIO

        // 使能全局中断服务
        gpio_install_isr_service(0);            // 参数代表中断优先级

        // 注册中断服务函数
        gpio_isr_handler_add(INT, XL9555_INT_Handle, (void*)INT);

        // 创建任务捕获ISR事件，并且读取内容
        xTaskCreatePinnedToCore(XL9555_Task, "XL9555_Task", 4096, NULL, 3, NULL, 1);
    }

    return ESP_OK;
}



/**
 * @brief 向XL9555写入Word数据（16位）
 *
 * @param Reg_ADDR  寄存器地址
 * @param Data      要写入的16位数据
 * @return          esp_err_t ESP_OK成功，其他失败
 */
esp_err_t XL9555_Write_Word(uint8_t Reg_ADDR, uint16_t Data)
{
    uint8_t write_buf[3];
    write_buf[0]= Reg_ADDR;
    write_buf[1]= Data & 0xff;          // 低八位
    write_buf[2]= (Data >> 8) & 0xff;   // 高八位

    return i2c_master_transmit(XL9555_dev_Handle,write_buf, 3, 500);
}



/**
 * @brief 从XL9555读取Word数据（16位）
 *
 * @param Reg_ADDR  寄存器地址
 * @param Data      读取到的16位数据指针
 * @return          esp_err_t ESP_OK成功，其他失败
 */
esp_err_t XL9555_Read_Word(uint8_t Reg_ADDR, uint16_t *Data)
{
    return i2c_master_transmit_receive(XL9555_dev_Handle, &Reg_ADDR, 1, (uint8_t*)Data, 2, 500);
}



/**
 * @brief 配置XL9555 IO方向
 *
 * @param config    配置数据，1为输入，0为输出
 * @return          esp_err_t ESP_OK成功，其他失败
 */
esp_err_t XL9555_IO_Cofig(uint16_t config)
{
    esp_err_t ret;
    do
    {
        ret = XL9555_Write_Word(0x06, config);
        vTaskDelay(pdMS_TO_TICKS(100));
    } while (ret != ESP_OK);        //等待写入完成
    return ret;
}



/**
 * @brief 读取XL9555引脚电平
 *
 * @param PIN       引脚宏（如IO0_0）
 * @return          int 1为高电平，0为低电平
 */
esp_err_t XL9555_Pin_Read(uint16_t PIN)
{
    uint16_t PIN_Data = 0;
    XL9555_Read_Word(0x00, &PIN_Data);

    return (PIN_Data & PIN)?1:0;
}



/**
 * @brief 写入XL9555引脚电平
 *
 * @param Pin       引脚宏（如IO0_0）
 * @param Level     电平值，1高电平，0低电平
 * @return          esp_err_t ESP_OK成功，其他失败
 */
esp_err_t XL9555_Pin_Write(uint16_t Pin, int Level)
{
    uint16_t Pin_Data = 0;

    // 先读取原有配置，再修改，再写入
    XL9555_Read_Word(0x02, &Pin_Data);

    if(Level)
    {
        Pin_Data |= Pin;
    }
    else
    {
        Pin_Data &= ~Pin;
    }

    return XL9555_Write_Word(0x02, Pin_Data);
}
