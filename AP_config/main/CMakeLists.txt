idf_component_register(SRCS "button.c" "XL9555.c" "AP_WIFI.c" "WS_Serve.c" "main.c" "WIFI_manager.c"
                    INCLUDE_DIRS ".")


spiffs_create_partition_image(html ../html FLASH_IN_PROJECT)