#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Registers every screen (home, settings, wifi_settings) and shows the
// home screen. Call once from app_main(), after the LVGL display/touch
// bring-up (lv_scr_act() must already be valid) and after data_hub_init()
// — the only other hard prerequisite, since data_hub_list_channels()
// takes a mutex that's NULL until then. Safe to call before
// uart_link/web_server bring-up: home_screen and wifi_settings_screen
// read those through their own refresh timers via plain static-default
// reads (no channels yet, WiFi "Connecting...") that are safe to see
// before that subsystem's own _init() has run, so the screen shows up
// immediately rather than waiting behind whichever of those is slowest
// or fails outright — web_server's WiFi connect alone can take up to 20s
// before falling back to the setup AP.
void ui_init(void);

#ifdef __cplusplus
}
#endif
