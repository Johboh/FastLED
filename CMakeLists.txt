# FastLED
# https://github.com/FastLED/FastLED
# MIT License

cmake_minimum_required(VERSION 3.5)

# Collect all source files recursively
file(GLOB_RECURSE FastLED_SRCS
    "src/*.cpp"
    "src/fl/*.*"
    "src/fx/*.*"
    "src/lib8tion/*.*"
    "src/sensors/*.*"
    "src/platforms/esp/32/*.*"
    "src/third_party/*.*"
)

set(required_components arduino-esp32 esp_driver_rmt driver)

idf_component_register(COMPONENT_NAME "fastled"
                        SRCS ${FastLED_SRCS} ${ESP32_LED_STRIP_COMPONENT_SRCS}
                        INCLUDE_DIRS "./src/"
                        REQUIRES ${required_components})

# Workaround (almost) for flickering ESP32S2 (for WS2812B)
#target_compile_options(${COMPONENT_LIB} PRIVATE -DLED_STRIP_RMT_DEFAULT_MEM_BLOCK_SYMBOLS=256)
#target_compile_options(${COMPONENT_LIB} PRIVATE -DFASTLED_ALL_PINS_HARDWARE_SPI)
