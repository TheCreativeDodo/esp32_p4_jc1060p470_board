/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "driver/i2s_std.h"
#include "audio_player.h"
#include "file_iterator.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_AUDIO_HELPER_DEFAULT_SAMPLE_RATE   (16000)
#define BSP_AUDIO_HELPER_DEFAULT_BIT_WIDTH     (16)
#define BSP_AUDIO_HELPER_DEFAULT_ADC_VOLUME    (24.0)
#define BSP_AUDIO_HELPER_DEFAULT_CHANNEL       (2)
#define BSP_AUDIO_HELPER_DEFAULT_VOLUME        (60)

esp_err_t bsp_audio_player_codec_mute_set(bool enable);
esp_err_t bsp_audio_player_codec_volume_set(int volume, int *volume_set);
int bsp_audio_player_codec_volume_get(void);
esp_err_t bsp_audio_player_codec_dev_stop(void);
esp_err_t bsp_audio_player_codec_dev_resume(void);
esp_err_t bsp_audio_player_codec_set_fs(uint32_t rate, uint32_t bits_cfg, i2s_slot_mode_t ch);
esp_err_t bsp_audio_player_i2s_read(void *audio_buffer, size_t len, size_t *bytes_read, uint32_t timeout_ms);
esp_err_t bsp_audio_player_i2s_write(void *audio_buffer, size_t len, size_t *bytes_written, uint32_t timeout_ms);
esp_err_t bsp_audio_player_codec_init(void);
esp_err_t bsp_audio_player_init(void);
esp_err_t bsp_audio_player_deinit(void);
esp_err_t bsp_audio_player_file_instance_init(const char *path, file_iterator_instance_t **ret_instance);
esp_err_t bsp_audio_player_play_index(file_iterator_instance_t *instance, int index);
esp_err_t bsp_audio_player_play_file(const char *file_path);
void bsp_audio_player_register_callback(audio_player_cb_t cb, void *user_data);
bool bsp_audio_player_is_playing_by_path(const char *file_path);
bool bsp_audio_player_is_playing_by_index(file_iterator_instance_t *instance, int index);

#ifdef __cplusplus
}
#endif
