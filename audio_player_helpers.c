/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "esp_codec_dev_defaults.h"
#include "bsp/esp-bsp.h"
#include "bsp/audio_player_helpers.h"

static const char *TAG = "bsp_audio_helper";

static esp_codec_dev_handle_t play_dev_handle;
static esp_codec_dev_handle_t record_dev_handle;

static bool is_audio_init = false;
static bool is_player_init = false;
static int volume_intensity = BSP_AUDIO_HELPER_DEFAULT_VOLUME;

static audio_player_cb_t audio_idle_callback = NULL;
static void *audio_idle_cb_user_data = NULL;
static char audio_file_path[128];

static esp_err_t audio_mute_function(AUDIO_PLAYER_MUTE_SETTING setting)
{
    bsp_audio_player_codec_mute_set(setting == AUDIO_PLAYER_MUTE ? true : false);
    if (setting == AUDIO_PLAYER_UNMUTE) {
        ESP_RETURN_ON_ERROR(esp_codec_dev_set_out_vol(play_dev_handle, volume_intensity), TAG, "Set Codec volume failed");
    }
    return ESP_OK;
}

static void audio_callback(audio_player_cb_ctx_t *ctx)
{
    if (audio_idle_callback) {
        ctx->user_ctx = audio_idle_cb_user_data;
        audio_idle_callback(ctx);
    }
}

esp_err_t bsp_audio_player_i2s_read(void *audio_buffer, size_t len, size_t *bytes_read, uint32_t timeout_ms)
{
    (void) timeout_ms;
    esp_err_t ret = esp_codec_dev_read(record_dev_handle, audio_buffer, len);
    if (bytes_read) {
        *bytes_read = len;
    }
    return ret;
}

esp_err_t bsp_audio_player_i2s_write(void *audio_buffer, size_t len, size_t *bytes_written, uint32_t timeout_ms)
{
    (void) timeout_ms;
    esp_err_t ret = esp_codec_dev_write(play_dev_handle, audio_buffer, len);
    if (bytes_written) {
        *bytes_written = len;
    }
    return ret;
}

esp_err_t bsp_audio_player_codec_set_fs(uint32_t rate, uint32_t bits_cfg, i2s_slot_mode_t ch)
{
    esp_err_t ret = ESP_OK;

    esp_codec_dev_sample_info_t fs = {
        .bits_per_sample = (uint8_t)bits_cfg,
        .channel = (uint8_t)ch,
        .channel_mask = 0,
        .sample_rate = rate,
        .mclk_multiple = 0,
    };

    if (play_dev_handle) {
        ret = esp_codec_dev_close(play_dev_handle);
    }
    if (record_dev_handle) {
        ret |= esp_codec_dev_close(record_dev_handle);
        ret |= esp_codec_dev_set_in_gain(record_dev_handle, BSP_AUDIO_HELPER_DEFAULT_ADC_VOLUME);
    }

    if (play_dev_handle) {
        ret |= esp_codec_dev_open(play_dev_handle, &fs);
    }
    if (record_dev_handle) {
        ret |= esp_codec_dev_open(record_dev_handle, &fs);
    }
    return ret;
}

esp_err_t bsp_audio_player_codec_volume_set(int volume, int *volume_set)
{
    ESP_RETURN_ON_ERROR(esp_codec_dev_set_out_vol(play_dev_handle, volume), TAG, "Set Codec volume failed");
    volume_intensity = volume;
    if (volume_set) {
        *volume_set = volume_intensity;
    }
    return ESP_OK;
}

int bsp_audio_player_codec_volume_get(void)
{
    return volume_intensity;
}

esp_err_t bsp_audio_player_codec_mute_set(bool enable)
{
    return esp_codec_dev_set_out_mute(play_dev_handle, enable);
}

esp_err_t bsp_audio_player_codec_dev_stop(void)
{
    esp_err_t ret = ESP_OK;
    if (play_dev_handle) {
        ret = esp_codec_dev_close(play_dev_handle);
    }
    if (record_dev_handle) {
        ret |= esp_codec_dev_close(record_dev_handle);
    }
    return ret;
}

esp_err_t bsp_audio_player_codec_dev_resume(void)
{
    return bsp_audio_player_codec_set_fs(BSP_AUDIO_HELPER_DEFAULT_SAMPLE_RATE,
                                         BSP_AUDIO_HELPER_DEFAULT_BIT_WIDTH,
                                         BSP_AUDIO_HELPER_DEFAULT_CHANNEL);
}

esp_err_t bsp_audio_player_codec_init(void)
{
    if (is_audio_init) {
        return ESP_OK;
    }

    play_dev_handle = bsp_audio_codec_speaker_init();
    ESP_RETURN_ON_FALSE(play_dev_handle, ESP_FAIL, TAG, "speaker codec init failed");

    record_dev_handle = bsp_audio_codec_microphone_init();
    ESP_RETURN_ON_FALSE(record_dev_handle, ESP_FAIL, TAG, "mic codec init failed");

    ESP_RETURN_ON_ERROR(bsp_audio_player_codec_set_fs(BSP_AUDIO_HELPER_DEFAULT_SAMPLE_RATE,
                                                      BSP_AUDIO_HELPER_DEFAULT_BIT_WIDTH,
                                                      BSP_AUDIO_HELPER_DEFAULT_CHANNEL),
                        TAG, "set codec fs failed");

    is_audio_init = true;
    return ESP_OK;
}

esp_err_t bsp_audio_player_init(void)
{
    if (is_player_init) {
        return ESP_OK;
    }

    audio_player_config_t config = {
        .mute_fn = audio_mute_function,
        .write_fn = bsp_audio_player_i2s_write,
        .clk_set_fn = bsp_audio_player_codec_set_fs,
        .priority = 5,
    };

    ESP_RETURN_ON_ERROR(audio_player_new(config), TAG, "audio_player_init failed");
    audio_player_callback_register(audio_callback, NULL);

    is_player_init = true;
    return ESP_OK;
}

esp_err_t bsp_audio_player_deinit(void)
{
    is_player_init = false;
    return audio_player_delete();
}

esp_err_t bsp_audio_player_file_instance_init(const char *path, file_iterator_instance_t **ret_instance)
{
    ESP_RETURN_ON_FALSE(path, ESP_FAIL, TAG, "path is NULL");
    ESP_RETURN_ON_FALSE(ret_instance, ESP_FAIL, TAG, "ret_instance is NULL");

    file_iterator_instance_t *file_iterator = file_iterator_new(path);
    ESP_RETURN_ON_FALSE(file_iterator, ESP_FAIL, TAG, "file_iterator_new failed, %s", path);

    *ret_instance = file_iterator;
    return ESP_OK;
}

esp_err_t bsp_audio_player_play_index(file_iterator_instance_t *instance, int index)
{
    ESP_RETURN_ON_FALSE(instance, ESP_FAIL, TAG, "instance is NULL");

    char filename[128];
    int retval = file_iterator_get_full_path_from_index(instance, index, filename, sizeof(filename));
    ESP_RETURN_ON_FALSE(retval != 0, ESP_FAIL, TAG, "file_iterator_get_full_path_from_index failed");

    FILE *fp = fopen(filename, "rb");
    ESP_RETURN_ON_FALSE(fp, ESP_FAIL, TAG, "unable to open file");

    ESP_RETURN_ON_ERROR(audio_player_play(fp), TAG, "audio_player_play failed");
    memcpy(audio_file_path, filename, sizeof(audio_file_path));

    return ESP_OK;
}

esp_err_t bsp_audio_player_play_file(const char *file_path)
{
    FILE *fp = fopen(file_path, "rb");
    ESP_RETURN_ON_FALSE(fp, ESP_FAIL, TAG, "unable to open file");

    ESP_RETURN_ON_ERROR(audio_player_play(fp), TAG, "audio_player_play failed");
    memcpy(audio_file_path, file_path, sizeof(audio_file_path));

    return ESP_OK;
}

void bsp_audio_player_register_callback(audio_player_cb_t cb, void *user_data)
{
    audio_idle_callback = cb;
    audio_idle_cb_user_data = user_data;
}

bool bsp_audio_player_is_playing_by_path(const char *file_path)
{
    return (strcmp(audio_file_path, file_path) == 0);
}

bool bsp_audio_player_is_playing_by_index(file_iterator_instance_t *instance, int index)
{
    return (index == file_iterator_get_index(instance));
}
