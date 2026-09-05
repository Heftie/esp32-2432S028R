#include <stdio.h>

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
#include "hardware.h"
#include "data_hub.h"
#include "uart_link.h"
#include "rgb_led.h"

static const char *TAG="demo";

void app_main(void)
{
    esp_lcd_panel_io_handle_t lcd_io;
    esp_lcd_panel_handle_t lcd_panel;
    esp_lcd_touch_handle_t tp;
    lvgl_port_touch_cfg_t touch_cfg;
    lv_display_t *lvgl_display = NULL;

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

    data_hub_init();

    const uart_link_config_t uart_cfg = {
        .uart_num = UART_NUM_2,
        .txd_gpio = UART_LINK_TXD,
        .rxd_gpio = UART_LINK_RXD,
        .baud_rate = UART_LINK_BAUD,
    };
    if (uart_link_init(&uart_cfg) != ESP_OK) {
        ESP_LOGE(TAG, "uart_link_init failed");
    }

    const rgb_led_config_t rgb_led_cfg = {
        .red_gpio = RGB_LED_RED,
        .green_gpio = RGB_LED_GREEN,
        .blue_gpio = RGB_LED_BLUE,
        .active_low = RGB_LED_ACTIVE_LOW,
    };
    if (rgb_led_init(&rgb_led_cfg) != ESP_OK) {
        ESP_LOGE(TAG, "rgb_led_init failed");
    }

    lvgl_port_lock(0);

    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);

    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "Hello LVGL 9 and esp_lvgl_port!");
    lv_obj_set_style_text_color(label, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_center(label);

    lvgl_port_unlock();

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
