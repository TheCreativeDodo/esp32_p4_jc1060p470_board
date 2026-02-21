#include <cmath>
#include "sdkconfig.h"
#include "esp_log.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"

#if CONFIG_EXAMPLE_USE_LVGL_MUSIC_DEMO
#include "lv_demos.h"
#endif

#if CONFIG_EXAMPLE_STARTUP_SPEAKER_BEEP
static const char *TAG = "jc1060_example";

static void speaker_demo_beep(void)
{
    esp_codec_dev_handle_t speaker = bsp_audio_codec_speaker_init();
    if (speaker == nullptr) {
        ESP_LOGE(TAG, "Speaker codec init failed");
        return;
    }

    esp_codec_dev_sample_info_t fs = {
        .bits_per_sample = 16,
        .channel = 1,
        .channel_mask = 0,
        .sample_rate = 16000,
        .mclk_multiple = 0,
    };

    if (esp_codec_dev_open(speaker, &fs) != ESP_CODEC_DEV_OK) {
        ESP_LOGE(TAG, "Speaker codec open failed");
        return;
    }

    esp_codec_dev_set_out_vol(speaker, 60);

    constexpr int sample_rate = 16000;
    constexpr int tone_hz = 1000;
    constexpr int duration_ms = 500;
    constexpr int chunk_samples = 256;
    constexpr double amplitude = 12000.0;
    int16_t pcm[chunk_samples];

    int total_samples = (sample_rate * duration_ms) / 1000;
    int sample_index = 0;

    while (sample_index < total_samples) {
        int n = total_samples - sample_index;
        if (n > chunk_samples) {
            n = chunk_samples;
        }

        for (int i = 0; i < n; ++i) {
            const double t = static_cast<double>(sample_index + i) / static_cast<double>(sample_rate);
            pcm[i] = static_cast<int16_t>(amplitude * sin(2.0 * M_PI * tone_hz * t));
        }

        if (esp_codec_dev_write(speaker, pcm, n * static_cast<int>(sizeof(int16_t))) != ESP_CODEC_DEV_OK) {
            ESP_LOGE(TAG, "Speaker write failed");
            break;
        }

        sample_index += n;
    }

    esp_codec_dev_close(speaker);
}
#endif

extern "C" void app_main(void)
{
    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_DRAW_BUFF_SIZE,
        .double_buffer = BSP_LCD_DRAW_BUFF_DOUBLE,
        .hw_cfg = {
            .hdmi_resolution = BSP_HDMI_RES_NONE,
            .dsi_bus = {
                .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
                .lane_bit_rate_mbps = BSP_LCD_MIPI_DSI_LANE_BITRATE_MBPS,
            },
        },
        .flags = {
            .buff_dma = true,
            .buff_spiram = false,
            .sw_rotate = true,
        },
    };

    bsp_display_start_with_config(&cfg);
    bsp_display_backlight_on();

#if CONFIG_EXAMPLE_STARTUP_SPEAKER_BEEP
    speaker_demo_beep();
#endif

    bsp_display_lock(0);
#if CONFIG_EXAMPLE_USE_LVGL_MUSIC_DEMO
    lv_demo_music();
#else
    lv_obj_t *screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *label = lv_label_create(screen);
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
    lv_label_set_text(label, "JC1060P470 BSP example ready");
    lv_obj_center(label);
#endif
    bsp_display_unlock();
}
