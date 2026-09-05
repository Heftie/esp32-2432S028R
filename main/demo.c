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
#include "rgb_led.h"
#include "web_server.h"
#include "ui.h"

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

    // Shown as early as possible, before any of the slower/fallible
    // hardware bring-up below — web_server's WiFi negotiation alone can
    // take up to 20s falling back to the setup AP. web_server is polled
    // through wifi_settings_screen's own refresh timer via a plain
    // static-default read that's safe to see before web_server_init()
    // has even run, so the screen just shows "Connecting..." until it
    // catches up.
    ui_init();

    const rgb_led_config_t rgb_led_cfg = {
        .red_gpio = RGB_LED_RED,
        .green_gpio = RGB_LED_GREEN,
        .blue_gpio = RGB_LED_BLUE,
        .active_low = RGB_LED_ACTIVE_LOW,
    };
    if (rgb_led_init(&rgb_led_cfg) != ESP_OK) {
        ESP_LOGE(TAG, "rgb_led_init failed");
    }

    const web_server_config_t web_cfg = {
        .http_port = 0, // default 80
    };
    if (web_server_init(&web_cfg) != ESP_OK) {
        ESP_LOGE(TAG, "web_server_init failed");
    }

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
