#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DATA_HUB_MAX_CHANNELS 16
#define DATA_HUB_NAME_LEN     16
#define DATA_HUB_UNIT_LEN     8

typedef struct {
    char name[DATA_HUB_NAME_LEN];
    char unit[DATA_HUB_UNIT_LEN];
    float latest_value;
    int64_t latest_timestamp_us;
} data_hub_channel_info_t;

void data_hub_init(void);

// Registers the channel on first sight and overwrites its latest value.
// Called only by uart_link. This project has no logger/on-device history
// consumer, so data_hub keeps only the latest sample per channel — not a
// history buffer.
void data_hub_publish(const char *name, float value, const char *unit);

// Copies up to max_out registered channels' latest state into out.
// Returns the number of channels copied.
size_t data_hub_list_channels(data_hub_channel_info_t *out, size_t max_out);

#ifdef __cplusplus
}
#endif
