/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_screen_wifi(lv_ui *ui)
{
    //Write codes screen_wifi
    ui->screen_wifi = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_wifi, 320, 240);
    lv_obj_set_scrollbar_mode(ui->screen_wifi, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->screen_wifi, &_screen_wifi_RGB565A8_320x240, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_label_wifi_1
    ui->screen_wifi_label_wifi_1 = lv_label_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_label_wifi_1, 12, 67);
    lv_obj_set_size(ui->screen_wifi_label_wifi_1, 94, 20);
    lv_label_set_text(ui->screen_wifi_label_wifi_1, "当前WIFI：");
    lv_label_set_long_mode(ui->screen_wifi_label_wifi_1, LV_LABEL_LONG_WRAP);

    //Write style for screen_wifi_label_wifi_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wifi_label_wifi_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_label_wifi_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_label_wifi_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_label_wifi_1, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wifi_label_wifi_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wifi_label_wifi_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wifi_label_wifi_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wifi_label_wifi_1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wifi_label_wifi_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wifi_label_wifi_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wifi_label_wifi_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wifi_label_wifi_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wifi_label_wifi_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_label_wifi_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_label_current_ssid
    ui->screen_wifi_label_current_ssid = lv_label_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_label_current_ssid, 121, 67);
    lv_obj_set_size(ui->screen_wifi_label_current_ssid, 175, 20);
    lv_label_set_text(ui->screen_wifi_label_current_ssid, "none");
    lv_label_set_long_mode(ui->screen_wifi_label_current_ssid, LV_LABEL_LONG_WRAP);

    //Write style for screen_wifi_label_current_ssid, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wifi_label_current_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_label_current_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_label_current_ssid, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_label_current_ssid, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wifi_label_current_ssid, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wifi_label_current_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wifi_label_current_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wifi_label_current_ssid, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wifi_label_current_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wifi_label_current_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wifi_label_current_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wifi_label_current_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wifi_label_current_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_label_current_ssid, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_btn_AP
    ui->screen_wifi_btn_AP = lv_button_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_btn_AP, 109, 107);
    lv_obj_set_size(ui->screen_wifi_btn_AP, 92, 35);
    ui->screen_wifi_btn_AP_label = lv_label_create(ui->screen_wifi_btn_AP);
    lv_label_set_text(ui->screen_wifi_btn_AP_label, "进行AP配网");
    lv_label_set_long_mode(ui->screen_wifi_btn_AP_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wifi_btn_AP_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wifi_btn_AP, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wifi_btn_AP_label, LV_PCT(100));

    //Write style for screen_wifi_btn_AP, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wifi_btn_AP, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_btn_AP, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wifi_btn_AP, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wifi_btn_AP, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_btn_AP, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_btn_AP, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_btn_AP, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_btn_AP, &lv_font_ZiTiQuanWeiJunHeiW22_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wifi_btn_AP, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wifi_btn_AP, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_label_wifi_2
    ui->screen_wifi_label_wifi_2 = lv_label_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_label_wifi_2, 12, 162);
    lv_obj_set_size(ui->screen_wifi_label_wifi_2, 109, 21);
    lv_label_set_text(ui->screen_wifi_label_wifi_2, "请连接WIFI：");
    lv_label_set_long_mode(ui->screen_wifi_label_wifi_2, LV_LABEL_LONG_WRAP);

    //Write style for screen_wifi_label_wifi_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wifi_label_wifi_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_label_wifi_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_label_wifi_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_label_wifi_2, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wifi_label_wifi_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wifi_label_wifi_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wifi_label_wifi_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wifi_label_wifi_2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wifi_label_wifi_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wifi_label_wifi_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wifi_label_wifi_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wifi_label_wifi_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wifi_label_wifi_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_label_wifi_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_label_wifi_AP
    ui->screen_wifi_label_wifi_AP = lv_label_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_label_wifi_AP, 121, 163);
    lv_obj_set_size(ui->screen_wifi_label_wifi_AP, 157, 20);
    lv_label_set_text(ui->screen_wifi_label_wifi_AP, "ESP32-AP");
    lv_label_set_long_mode(ui->screen_wifi_label_wifi_AP, LV_LABEL_LONG_WRAP);

    //Write style for screen_wifi_label_wifi_AP, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wifi_label_wifi_AP, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_label_wifi_AP, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_label_wifi_AP, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_label_wifi_AP, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wifi_label_wifi_AP, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wifi_label_wifi_AP, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wifi_label_wifi_AP, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wifi_label_wifi_AP, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wifi_label_wifi_AP, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wifi_label_wifi_AP, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wifi_label_wifi_AP, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wifi_label_wifi_AP, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wifi_label_wifi_AP, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_label_wifi_AP, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_label_wifi_3
    ui->screen_wifi_label_wifi_3 = lv_label_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_label_wifi_3, 12, 200);
    lv_obj_set_size(ui->screen_wifi_label_wifi_3, 94, 19);
    lv_label_set_text(ui->screen_wifi_label_wifi_3, "打开网址：");
    lv_label_set_long_mode(ui->screen_wifi_label_wifi_3, LV_LABEL_LONG_WRAP);

    //Write style for screen_wifi_label_wifi_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wifi_label_wifi_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_label_wifi_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_label_wifi_3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_label_wifi_3, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wifi_label_wifi_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wifi_label_wifi_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wifi_label_wifi_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wifi_label_wifi_3, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wifi_label_wifi_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wifi_label_wifi_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wifi_label_wifi_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wifi_label_wifi_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wifi_label_wifi_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_label_wifi_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_label_wifi_ip
    ui->screen_wifi_label_wifi_ip = lv_label_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_label_wifi_ip, 121, 199);
    lv_obj_set_size(ui->screen_wifi_label_wifi_ip, 164, 22);
    lv_label_set_text(ui->screen_wifi_label_wifi_ip, "192.268.100.1");
    lv_label_set_long_mode(ui->screen_wifi_label_wifi_ip, LV_LABEL_LONG_WRAP);

    //Write style for screen_wifi_label_wifi_ip, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wifi_label_wifi_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_label_wifi_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_label_wifi_ip, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_label_wifi_ip, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wifi_label_wifi_ip, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wifi_label_wifi_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wifi_label_wifi_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wifi_label_wifi_ip, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wifi_label_wifi_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wifi_label_wifi_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wifi_label_wifi_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wifi_label_wifi_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wifi_label_wifi_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_label_wifi_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_img_wifi_title
    ui->screen_wifi_img_wifi_title = lv_image_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_img_wifi_title, 60, 10);
    lv_obj_set_size(ui->screen_wifi_img_wifi_title, 52, 37);
    lv_obj_add_flag(ui->screen_wifi_img_wifi_title, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_wifi_img_wifi_title, &_wifi_RGB565A8_52x37);
    lv_image_set_pivot(ui->screen_wifi_img_wifi_title, 50,50);
    lv_image_set_rotation(ui->screen_wifi_img_wifi_title, 0);

    //Write style for screen_wifi_img_wifi_title, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_wifi_img_wifi_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_wifi_img_wifi_title, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_label_wifi_title
    ui->screen_wifi_label_wifi_title = lv_label_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_label_wifi_title, 127, 20);
    lv_obj_set_size(ui->screen_wifi_label_wifi_title, 121, 20);
    lv_label_set_text(ui->screen_wifi_label_wifi_title, "WIFI配置页面");
    lv_label_set_long_mode(ui->screen_wifi_label_wifi_title, LV_LABEL_LONG_WRAP);

    //Write style for screen_wifi_label_wifi_title, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wifi_label_wifi_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_label_wifi_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_label_wifi_title, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_label_wifi_title, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wifi_label_wifi_title, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wifi_label_wifi_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wifi_label_wifi_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wifi_label_wifi_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wifi_label_wifi_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wifi_label_wifi_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wifi_label_wifi_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wifi_label_wifi_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wifi_label_wifi_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_label_wifi_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_wifi.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_wifi);

    //Init events for screen.
    events_init_screen_wifi(ui);
}
