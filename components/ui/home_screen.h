#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Default screen: a UTC clock (from web_server_get_wall_clock();
// "Time not set" until NTP lands) over one tile per data_hub channel,
// created on first sight and refreshed on a timer. Nothing here assumes
// a fixed set of channels — the same screen works whether the companion
// MCU exposes one reading or ten.
void home_screen_create(void);

#ifdef __cplusplus
}
#endif
