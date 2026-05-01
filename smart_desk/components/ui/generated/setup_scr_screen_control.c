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



void setup_scr_screen_control(lv_ui *ui)
{
    //Write codes screen_control
    ui->screen_control = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_control, 320, 240);
    lv_obj_set_scrollbar_mode(ui->screen_control, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_control, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->screen_control, &_screen_control_RGB565A8_320x240, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen_control, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_control, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_control_btn_light_switch
    ui->screen_control_btn_light_switch = lv_button_create(ui->screen_control);
    lv_obj_set_pos(ui->screen_control_btn_light_switch, 216, 73);
    lv_obj_set_size(ui->screen_control_btn_light_switch, 66, 32);
    ui->screen_control_btn_light_switch_label = lv_label_create(ui->screen_control_btn_light_switch);
    lv_label_set_text(ui->screen_control_btn_light_switch_label, "开/关");
    lv_label_set_long_mode(ui->screen_control_btn_light_switch_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_control_btn_light_switch_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_control_btn_light_switch, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_control_btn_light_switch_label, LV_PCT(100));

    //Write style for screen_control_btn_light_switch, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_btn_light_switch, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_btn_light_switch, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_btn_light_switch, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_btn_light_switch, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_control_btn_light_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_control_btn_light_switch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_control_btn_light_switch, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_control_btn_light_switch, &lv_font_ZiTiQuanWeiJunHeiW22_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_control_btn_light_switch, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_control_btn_light_switch, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_control_label_control_1
    ui->screen_control_label_control_1 = lv_label_create(ui->screen_control);
    lv_obj_set_pos(ui->screen_control_label_control_1, 25, 30);
    lv_obj_set_size(ui->screen_control_label_control_1, 83, 21);
    lv_label_set_text(ui->screen_control_label_control_1, "RGB控制");
    lv_label_set_long_mode(ui->screen_control_label_control_1, LV_LABEL_LONG_WRAP);

    //Write style for screen_control_label_control_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_label_control_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_label_control_1, lv_color_hex(0xdce0ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_label_control_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_label_control_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_control_label_control_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_control_label_control_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_control_label_control_1, lv_color_hex(0x2900ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_control_label_control_1, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_control_label_control_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_control_label_control_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_control_label_control_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_control_label_control_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_control_label_control_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_control_label_control_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_control_label_control_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_control_label_control_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_control_slider_RGB_red
    ui->screen_control_slider_RGB_red = lv_slider_create(ui->screen_control);
    lv_obj_set_pos(ui->screen_control_slider_RGB_red, 17, 129);
    lv_obj_set_size(ui->screen_control_slider_RGB_red, 100, 8);
    lv_slider_set_range(ui->screen_control_slider_RGB_red, 0, 100);
    lv_slider_set_mode(ui->screen_control_slider_RGB_red, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_control_slider_RGB_red, 50, LV_ANIM_OFF);

    //Write style for screen_control_slider_RGB_red, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_slider_RGB_red, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_slider_RGB_red, lv_color_hex(0xff0027), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_slider_RGB_red, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_slider_RGB_red, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_control_slider_RGB_red, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_control_slider_RGB_red, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_control_slider_RGB_red, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_slider_RGB_red, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_slider_RGB_red, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_slider_RGB_red, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_slider_RGB_red, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_control_slider_RGB_red, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_slider_RGB_red, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_slider_RGB_red, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_slider_RGB_red, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_slider_RGB_red, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_control_slider_RGB_green
    ui->screen_control_slider_RGB_green = lv_slider_create(ui->screen_control);
    lv_obj_set_pos(ui->screen_control_slider_RGB_green, 17, 166);
    lv_obj_set_size(ui->screen_control_slider_RGB_green, 100, 8);
    lv_slider_set_range(ui->screen_control_slider_RGB_green, 0, 100);
    lv_slider_set_mode(ui->screen_control_slider_RGB_green, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_control_slider_RGB_green, 50, LV_ANIM_OFF);

    //Write style for screen_control_slider_RGB_green, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_slider_RGB_green, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_slider_RGB_green, lv_color_hex(0x00ff16), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_slider_RGB_green, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_slider_RGB_green, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_control_slider_RGB_green, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_control_slider_RGB_green, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_control_slider_RGB_green, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_slider_RGB_green, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_slider_RGB_green, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_slider_RGB_green, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_slider_RGB_green, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_control_slider_RGB_green, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_slider_RGB_green, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_slider_RGB_green, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_slider_RGB_green, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_slider_RGB_green, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_control_slider_RGB_bri
    ui->screen_control_slider_RGB_bri = lv_slider_create(ui->screen_control);
    lv_obj_set_pos(ui->screen_control_slider_RGB_bri, 17, 92);
    lv_obj_set_size(ui->screen_control_slider_RGB_bri, 100, 8);
    lv_slider_set_range(ui->screen_control_slider_RGB_bri, 0, 100);
    lv_slider_set_mode(ui->screen_control_slider_RGB_bri, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_control_slider_RGB_bri, 50, LV_ANIM_OFF);

    //Write style for screen_control_slider_RGB_bri, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_slider_RGB_bri, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_slider_RGB_bri, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_slider_RGB_bri, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_slider_RGB_bri, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_control_slider_RGB_bri, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_control_slider_RGB_bri, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_control_slider_RGB_bri, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_slider_RGB_bri, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_slider_RGB_bri, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_slider_RGB_bri, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_slider_RGB_bri, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_control_slider_RGB_bri, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_slider_RGB_bri, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_slider_RGB_bri, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_slider_RGB_bri, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_slider_RGB_bri, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_control_slider_RGB_blue
    ui->screen_control_slider_RGB_blue = lv_slider_create(ui->screen_control);
    lv_obj_set_pos(ui->screen_control_slider_RGB_blue, 17, 204);
    lv_obj_set_size(ui->screen_control_slider_RGB_blue, 100, 8);
    lv_slider_set_range(ui->screen_control_slider_RGB_blue, 0, 100);
    lv_slider_set_mode(ui->screen_control_slider_RGB_blue, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_control_slider_RGB_blue, 50, LV_ANIM_OFF);

    //Write style for screen_control_slider_RGB_blue, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_slider_RGB_blue, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_slider_RGB_blue, lv_color_hex(0x0024ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_slider_RGB_blue, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_slider_RGB_blue, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_control_slider_RGB_blue, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_control_slider_RGB_blue, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_control_slider_RGB_blue, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_slider_RGB_blue, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_slider_RGB_blue, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_slider_RGB_blue, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_slider_RGB_blue, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_control_slider_RGB_blue, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_slider_RGB_blue, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_slider_RGB_blue, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_slider_RGB_blue, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_slider_RGB_blue, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_control_slider_light_bri
    ui->screen_control_slider_light_bri = lv_slider_create(ui->screen_control);
    lv_obj_set_pos(ui->screen_control_slider_light_bri, 205, 195);
    lv_obj_set_size(ui->screen_control_slider_light_bri, 100, 8);
    lv_slider_set_range(ui->screen_control_slider_light_bri, 0, 100);
    lv_slider_set_mode(ui->screen_control_slider_light_bri, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_control_slider_light_bri, 50, LV_ANIM_OFF);

    //Write style for screen_control_slider_light_bri, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_slider_light_bri, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_slider_light_bri, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_slider_light_bri, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_slider_light_bri, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_control_slider_light_bri, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_control_slider_light_bri, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_control_slider_light_bri, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_slider_light_bri, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_slider_light_bri, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_slider_light_bri, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_slider_light_bri, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_control_slider_light_bri, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_slider_light_bri, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_slider_light_bri, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_slider_light_bri, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_slider_light_bri, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_control_label_control_2
    ui->screen_control_label_control_2 = lv_label_create(ui->screen_control);
    lv_obj_set_pos(ui->screen_control_label_control_2, 210, 30);
    lv_obj_set_size(ui->screen_control_label_control_2, 83, 21);
    lv_label_set_text(ui->screen_control_label_control_2, "主灯开关");
    lv_label_set_long_mode(ui->screen_control_label_control_2, LV_LABEL_LONG_WRAP);

    //Write style for screen_control_label_control_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_label_control_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_label_control_2, lv_color_hex(0xdce0ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_label_control_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_label_control_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_control_label_control_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_control_label_control_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_control_label_control_2, lv_color_hex(0x2900ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_control_label_control_2, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_control_label_control_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_control_label_control_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_control_label_control_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_control_label_control_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_control_label_control_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_control_label_control_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_control_label_control_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_control_label_control_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_control_label_control_light_bri
    ui->screen_control_label_control_light_bri = lv_label_create(ui->screen_control);
    lv_obj_set_pos(ui->screen_control_label_control_light_bri, 210, 144);
    lv_obj_set_size(ui->screen_control_label_control_light_bri, 83, 21);
    lv_label_set_text(ui->screen_control_label_control_light_bri, "主灯亮度");
    lv_label_set_long_mode(ui->screen_control_label_control_light_bri, LV_LABEL_LONG_WRAP);

    //Write style for screen_control_label_control_light_bri, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_control_label_control_light_bri, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_control_label_control_light_bri, lv_color_hex(0xdce0ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_control_label_control_light_bri, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_control_label_control_light_bri, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_control_label_control_light_bri, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_control_label_control_light_bri, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_control_label_control_light_bri, lv_color_hex(0x2900ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_control_label_control_light_bri, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_control_label_control_light_bri, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_control_label_control_light_bri, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_control_label_control_light_bri, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_control_label_control_light_bri, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_control_label_control_light_bri, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_control_label_control_light_bri, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_control_label_control_light_bri, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_control_label_control_light_bri, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_control.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_control);

    //Init events for screen.
    events_init_screen_control(ui);
}
