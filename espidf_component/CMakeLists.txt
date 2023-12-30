
FILE(GLOB_RECURSE lib_sources "../src/*.*")

if(IDF_VERSION_MAJOR GREATER_EQUAL 5)
set(hal_includes "./hal/v5")
FILE(GLOB_RECURSE hal_sources "./hal/v5/*.*")
set(required_components driver esp_timer esp_adc nvs_flash)
else()
set(hal_includes "./hal/v4")
FILE(GLOB_RECURSE hal_sources "./hal/v4/*.*")
set(required_components driver esp_adc_cal nvs_flash)
endif()


idf_component_register(COMPONENT_NAME "FastLED"
                        SRCS ${lib_sources} ${hal_sources}
                        INCLUDE_DIRS "../src/" ${hal_includes}
												REQUIRES ${required_components})

target_compile_definitions(${COMPONENT_LIB} PUBLIC ESP32)

target_compile_definitions(${COMPONENT_LIB} PUBLIC FASTLED_NO_PINMAP)
