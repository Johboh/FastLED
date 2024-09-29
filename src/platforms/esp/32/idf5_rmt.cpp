#include "led_strip/enabled.h"

#if FASTLED_RMT51

#warning "Work in progress: ESP32 ClocklessController for IDF 5.1 - this is still a prototype"

#include "idf5_rmt.h"
#include "led_strip/led_strip.h"
#include "esp_log.h"
#include "led_strip/demo.h"
#include "led_strip/configure_led.h"


USING_NAMESPACE_LED_STRIP

#define TAG "idf5_rmt.cpp"

void to_esp_modes(LedStripMode mode, led_model_t* out_chipset, led_pixel_format_t* out_rgbw) {
    switch (mode) {
        case kWS2812:
            *out_rgbw = LED_PIXEL_FORMAT_GRB;
            *out_chipset = LED_MODEL_WS2812;
            break;
        case kSK6812:
            *out_rgbw = LED_PIXEL_FORMAT_GRB;
            *out_chipset = LED_MODEL_SK6812;
            break;
        case kWS2812_RGBW:
            *out_rgbw = LED_PIXEL_FORMAT_GRBW;
            *out_chipset = LED_MODEL_WS2812;
            break;
        case kSK6812_RGBW:
            *out_rgbw = LED_PIXEL_FORMAT_GRBW;
            *out_chipset = LED_MODEL_SK6812;
            break;
        default:
            ESP_LOGE(TAG, "Invalid LedStripMode");
            break;
    }
}

RmtController5::RmtController5(int DATA_PIN, int T1, int T2, int T3) {
    // Stub implementation
    ESP_LOGI(TAG, "RmtController5 constructor called");
    mPin = DATA_PIN;
}

RmtController5::~RmtController5() {
    // Stub implementation
    ESP_LOGI(TAG, "RmtController5 destructor called");
}


class RmtLedStrip {
public:
    RmtLedStrip(int pin, uint32_t max_leds, LedStripMode mode) {
        mMaxLeds = max_leds;
        led_model_t chipset;
        led_pixel_format_t rgbw_mode;
        to_esp_modes(mode, &chipset, &rgbw_mode);

        config_led_t config = {
            .pin = pin,
            .max_leds = max_leds,
            .chipset = chipset,
            .rgbw = rgbw_mode
        };
        led_strip = construct_new_led_strip(config);
        is_rgbw = (mode == kWS2812_RGBW || mode == kSK6812_RGBW);
    }

    ~RmtLedStrip() {
        bool release_pixel_buffer = true;
        led_strip_del(led_strip, release_pixel_buffer);
    }

    void show_pixels(PixelIterator &pixels) {
        assert(pixels.size() <= mMaxLeds);
        if (is_rgbw) {
            uint8_t r, g, b, w;
            for (uint16_t i = 0; pixels.has(1); i++) {
                pixels.loadAndScaleRGBW(&r, &g, &b, &w);
                ESP_ERROR_CHECK(led_strip_set_pixel_rgbw(led_strip, i, g, r, b, w));
                pixels.advanceData();
                pixels.stepDithering();
            }
        } else {
            uint8_t r, g, b;
            for (uint16_t i = 0; pixels.has(1); i++) {
                pixels.loadAndScaleRGB(&r, &g, &b);
                ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, g, r, b));
                pixels.advanceData();
                pixels.stepDithering();
            }
        }
        ESP_ERROR_CHECK(led_strip_refresh(led_strip));
    }

private:
    led_strip_handle_t led_strip;
    bool is_rgbw;
    uint32_t mMaxLeds;
};


void RmtController5::showPixels(PixelIterator &pixels) {
    ESP_LOGI(TAG, "showPixels called");
    uint32_t max_leds = pixels.size();
    Rgbw rgbw = pixels.get_rgbw();
    LedStripMode mode = rgbw.active() ? kWS2812_RGBW : kWS2812;

    RmtLedStrip led_strip(mPin, max_leds, mode);
    led_strip.show_pixels(pixels);
}

#endif  // FASTLED_RMT51

