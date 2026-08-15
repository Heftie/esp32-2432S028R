#include <stdio.h>
#include <math.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <freertos/semphr.h>

#include <esp_system.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_check.h>

#include <lvgl.h>
#include <esp_lvgl_port.h>

#include "lcd.h"
#include "touch.h"

static const char *TAG="demo";

lv_obj_t *lbl_counter;


typedef struct {
    lv_obj_t *label_title;
    lv_obj_t *label_value;
    lv_obj_t *label_unit;
} multimeter_ui_t;

multimeter_ui_t ui;

void multimeter_create_ui(void)
{
    lv_obj_t *scr = lv_scr_act();

    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);

    // Titel
    ui.label_title = lv_label_create(scr);
    lv_label_set_text(ui.label_title, "DC Voltage");
    lv_obj_set_style_text_color(ui.label_title, lv_color_white(), 0);
    lv_obj_set_style_text_font(ui.label_title, &lv_font_montserrat_20, 0);
    lv_obj_align(ui.label_title, LV_ALIGN_TOP_MID, 0, 10);

    // Wert
    ui.label_value = lv_label_create(scr);
    lv_label_set_text(ui.label_value, "0.00");
    lv_obj_set_style_text_color(ui.label_value, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_font(ui.label_value, &lv_font_montserrat_28, 0);
    lv_obj_align(ui.label_value, LV_ALIGN_CENTER, -20, 20);

    // Einheit
    ui.label_unit = lv_label_create(scr);
    lv_label_set_text(ui.label_unit, "V");
    lv_obj_set_style_text_color(ui.label_unit, lv_color_white(), 0);
    lv_obj_set_style_text_font(ui.label_unit, &lv_font_montserrat_28, 0);
    lv_obj_align_to(ui.label_unit, ui.label_value, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
}

void multimeter_update(float value, const char *unit)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%.2f", value);

    lv_label_set_text(ui.label_value, buf);
    lv_label_set_text(ui.label_unit, unit);

    // Startfarbe = grün
    lv_color_t color = lv_color_hex(0x00FF00);

    if (value > 10.0) {
        color = lv_color_hex(0xFFFF00); // gelb
    }

    if (value > 20.0) {
        color = lv_color_hex(0xFF0000); // rot
    }

    lv_obj_set_style_text_color(ui.label_value, color, 0);
}

void ui_event_Screen(lv_event_t *e)
{
static uint8_t pos=1;

    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *btn = (lv_obj_t *)lv_event_get_user_data(e);

    if (event_code == LV_EVENT_CLICKED)
    {
        lv_obj_align(btn, pos++, 0, 0);
        if (pos > 9) pos=1;
    }
}


static esp_err_t app_lvgl_main(void)
{
    lv_obj_t *scr = lv_scr_act();

    lvgl_port_lock(0);

    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "Hello LVGL 9 and esp_lvgl_port!");
    lv_obj_set_style_text_color(label, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -48);

    lv_obj_t *labelR = lv_label_create(scr);
    lv_label_set_text(labelR, "Red");
    lv_obj_set_style_text_color(labelR, lv_color_make(0xff, 0, 0), LV_STATE_DEFAULT);
    lv_obj_align(labelR, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *labelG = lv_label_create(scr);
    lv_label_set_text(labelG, "Green");
    lv_obj_set_style_text_color(labelG, lv_color_make(0, 0xff, 0), LV_STATE_DEFAULT);
    lv_obj_align(labelG, LV_ALIGN_TOP_MID, 0, 32);

    lv_obj_t *labelB = lv_label_create(scr);
    lv_label_set_text(labelB, "Blue");
    lv_obj_set_style_text_color(labelB, lv_color_make(0, 0, 0xff), LV_STATE_DEFAULT);
    lv_obj_align(labelB, LV_ALIGN_TOP_MID, 0, 64);

    lv_obj_t *btn_counter = lv_button_create(scr);
    lv_obj_align(btn_counter, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(btn_counter, 120, 50);
    lv_obj_add_event_cb(btn_counter, ui_event_Screen, LV_EVENT_ALL, btn_counter);

    lbl_counter = lv_label_create(btn_counter);
    lv_label_set_text(lbl_counter, "testing");
    lv_obj_set_style_text_color(lbl_counter, lv_color_make(248, 11, 181), LV_STATE_DEFAULT);
    lv_obj_align(lbl_counter, LV_ALIGN_CENTER, 0, 0);

    lvgl_port_unlock();

    return ESP_OK;
}


void app_main(void)
{
    esp_lcd_panel_io_handle_t lcd_io;
    esp_lcd_panel_handle_t lcd_panel;
    esp_lcd_touch_handle_t tp;
    lvgl_port_touch_cfg_t touch_cfg;
    lv_display_t *lvgl_display = NULL;
    char buf[16];
    uint16_t n = 0;

    ESP_ERROR_CHECK(lcd_display_brightness_init());

    ESP_ERROR_CHECK(app_lcd_init(&lcd_io, &lcd_panel));
    lvgl_display = app_lvgl_init(lcd_io, lcd_panel);
    if (lvgl_display == NULL)
    {
        ESP_LOGI(TAG, "fatal error in app_lvgl_init");
        esp_restart();
    }
    
    ESP_ERROR_CHECK(touch_init(&tp));
    touch_cfg.disp = lvgl_display;
    touch_cfg.handle = tp;
    touch_cfg.scale.x = 0;
    touch_cfg.scale.y = 0;
    lvgl_port_add_touch(&touch_cfg);

    ESP_ERROR_CHECK(lcd_display_brightness_set(75));
    ESP_ERROR_CHECK(lcd_display_rotate(lvgl_display, LV_DISPLAY_ROTATION_90));
    //ESP_ERROR_CHECK(app_lvgl_main());

    multimeter_create_ui();
     float v = 0.0;
     while (1)
    {
        v += 0.5;
        if (v > 25) v = 0.0;

        multimeter_update(v, "V");

        lv_timer_handler();   // wichtig!
        vTaskDelay(pdMS_TO_TICKS(100));
    }   
    /*
    while(42)
    {
        sprintf(buf, "%04d", n++);
        
        if (lvgl_port_lock(0))
        {
            lv_label_set_text(lbl_counter, buf);
            
            lvgl_port_unlock();
        }
        
        vTaskDelay(125 / portTICK_PERIOD_MS);
    }
    */
    vTaskDelay(portMAX_DELAY);
}
