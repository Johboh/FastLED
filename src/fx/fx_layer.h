#pragma once

#include "crgb.h"
#include "fixed_vector.h"
#include "fx/fx.h"
#include "namespace.h"
#include "ptr.h"
#include <stdint.h>
#include <string.h>


FASTLED_NAMESPACE_BEGIN

struct Layer;
typedef RefPtr<Layer> LayerPtr;
struct Layer : public Referent {
    scoped_array<CRGB> surface;
    scoped_array<uint8_t> surface_alpha;
    RefPtr<Fx> fx;
    bool running = false;

    void setFx(RefPtr<Fx> newFx) {
        if (newFx != fx) {
            release();
            fx = newFx;
        }
    }

    void draw(uint32_t now) {
        if (fx) {
            if (!surface.get()) {
                surface.reset(new CRGB[fx->getNumLeds()]);
            }
            if (fx->hasAlphaChannel() && !surface_alpha.get()) {
                surface_alpha.reset(new uint8_t[fx->getNumLeds()]);
            }
            // mem clear the surface
            CRGB* surface_ptr = this->surface.get();
            uint8_t* surface_alpha_ptr = this->surface_alpha.get();
            if (surface_ptr) {
                memset(surface_ptr, 0, sizeof(CRGB) * fx->getNumLeds());
            }
            if (surface_alpha_ptr) {
                memset(surface_alpha_ptr, 0, sizeof(uint8_t) * fx->getNumLeds());
            }
            if (!running) {
                fx->resume();
                running = true;
            }
            Fx::DrawContext context = {now, surface.get()};
            if (fx->hasAlphaChannel()) {
                context.alpha_channel = surface_alpha.get();
            }
            fx->draw(context);
        }
    }

    void pause() {
        if (fx && running) {
            fx->pause();
            running = false;
        }
    }

    void release() {
        pause();
        fx.reset();
    }
};

FASTLED_NAMESPACE_END
