/* Screenmap maps strip indexes to x,y coordinates. This is used for FastLED.js
 * to map the 1D strip to a 2D grid. Note that the strip can have arbitrary
 * size. this was first motivated during the (attempted? Oct. 19th 2024) port of
 * the Chromancer project to FastLED.js.
 */

#include "screenmap.h"

#include "str.h"
#include "fixed_map.h"
#include "json.h"
#include "namespace.h"
#include "fixed_vector.h"

FASTLED_NAMESPACE_BEGIN



void ScreenMap::ParseJson(const char *jsonStrOfMapFile,
                          FixedMap<Str, ScreenMap, 16> *segmentMaps) {
    ArduinoJson::JsonDocument doc;
    ArduinoJson::deserializeJson(doc, jsonStrOfMapFile);
    auto map = doc["map"];
    for (auto kv : map.as<ArduinoJson::JsonObject>()) {
        auto segment = kv.value();
        auto x = segment["x"];
        auto y = segment["y"];
        auto obj = segment["diameter"];
        float diameter = 1.0f;
        if (obj.is<float>()) {
            diameter = obj.as<float>();
        }
        auto n = x.size();
        ScreenMap segment_map(n, diameter);
        for (uint16_t j = 0; j < n; j++) {
            segment_map.set(j, pair_xy_float{x[j], y[j]});
        }
        segmentMaps->insert(kv.key().c_str(), segment_map);
    }
}

void ScreenMap::toJson(const FixedMap<Str, ScreenMap, 16>& segmentMaps, ArduinoJson::JsonDocument* _doc) {
    auto& doc = *_doc;
    auto map = doc["map"].to<ArduinoJson::JsonObject>();
    for (auto kv : segmentMaps) {
        auto segment = map[kv.first].to<ArduinoJson::JsonObject>();
        auto x_array = segment["x"].to<ArduinoJson::JsonArray>();
        auto y_array = segment["y"].to<ArduinoJson::JsonArray>();
        for (uint16_t i = 0; i < kv.second.getLength(); i++) {
            const pair_xy_float& xy = kv.second[i];
            x_array.add(xy.x);
            y_array.add(xy.y);
        }
        segment["diameter"] = kv.second.getDiameter();
    }
}

void ScreenMap::toJsonStr(const FixedMap<Str, ScreenMap, 16>& segmentMaps, Str* jsonBuffer) {
    ArduinoJson::JsonDocument doc;
    toJson(segmentMaps, &doc);
    ArduinoJson::serializeJson(doc, *jsonBuffer);
}

FASTLED_NAMESPACE_END
