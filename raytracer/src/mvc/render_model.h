#pragma once

#include "../camera.h"
#include "../color.h"
#include "../framebuffer.h"
#include "../hitable.h"
#include "../parse.h"
#include "../renderer.h"
#include "camera_controls.h"

/**
 * \brief Rendering state: scene, camera, options, and progressive framebuffer.
 */
class RenderModel {
public:
    RenderModel(Hitable* world, const RenderOptions& opts, Camera::objectColor colorFn = metalColor)
        : opts_(opts),
          world_(world),
          colorFn_(colorFn),
          fb_(opts.width, renderHeight(opts)),
          cam_(makeCamera(opts)) {}

    /** \brief Rebuilds the camera from the current RenderOptions. */
    void rebuildCamera() {
        cam_ = makeCamera(opts_);
    }

    /** \brief Discards accumulated samples without changing the camera. */
    void resetAccumulation() {
        fb_.reset();
    }

    /** \brief Adds one sample per pixel. */
    void accumulatePass() {
        renderPass(cam_, world_, fb_, opts_.depth, colorFn_);
    }

    const Framebuffer& framebuffer() const {
        return fb_;
    }

    /** \returns Sample count at the top-left pixel (representative of the frame). */
    int sampleCount() const {
        return fb_.samplesAt(0, 0);
    }

    RenderOptions& options() {
        return opts_;
    }

    const RenderOptions& options() const {
        return opts_;
    }

    /** \brief Marks the camera as changed so the next frame resets accumulation. */
    void markCameraDirty() {
        cameraDirty_ = true;
    }

    bool isCameraDirty() const {
        return cameraDirty_;
    }

    void clearCameraDirty() {
        cameraDirty_ = false;
    }

private:
    RenderOptions opts_;
    Hitable* world_;
    Camera::objectColor colorFn_;
    Framebuffer fb_;
    Camera cam_;
    bool cameraDirty_ = false;
};
