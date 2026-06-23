#pragma once

#include "camera_controls.h"
#include "render_model.h"
#include "view.h"
#include "view_events.h"

#include <cmath>

/**
 * \brief Connects input events to the model and drives the render loop.
 */
class RenderController {
public:
    RenderController(RenderModel& model, View& view)
        : model_(model), view_(view) {}

    /** \brief Runs until the view requests shutdown. */
    void run() {
        while (!view_.shouldClose()) {
            ViewEvent event;
            while (view_.pollEvent(event)) {
                handleEvent(event);
            }

            updateHeldMovement();

            if (model_.isCameraDirty()) {
                model_.rebuildCamera();
                model_.resetAccumulation();
                model_.clearCameraDirty();
            }

            model_.accumulateFrame();
            view_.present(model_.framebuffer(), model_.sampleCount());
        }
    }

private:
    static constexpr double kOrbitSensitivity = 0.005;
    static constexpr double kPanSensitivity = 0.01;
    static constexpr double kDollySensitivity = 0.5;
    static constexpr double kFlySpeed = 0.2;

    void handleEvent(const ViewEvent& event) {
        switch (event.type) {
            case ViewEventType::Quit:
                return;
            case ViewEventType::MouseButtonDown:
                if (event.button == 1) {
                    orbiting_ = true;
                } else if (event.button == 3) {
                    panning_ = true;
                }
                lastX_ = event.x;
                lastY_ = event.y;
                break;
            case ViewEventType::MouseButtonUp:
                if (event.button == 1) {
                    orbiting_ = false;
                } else if (event.button == 3) {
                    panning_ = false;
                }
                break;
            case ViewEventType::MouseMotion:
                if (orbiting_ || panning_) {
                    const double dx = event.x - lastX_;
                    const double dy = event.y - lastY_;
                    if (orbiting_) {
                        orbitCamera(model_.options(), dx * kOrbitSensitivity, dy * kOrbitSensitivity);
                    } else {
                        panCamera(model_.options(), dx * kPanSensitivity, dy * kPanSensitivity);
                    }
                    model_.markCameraDirty();
                    lastX_ = event.x;
                    lastY_ = event.y;
                }
                break;
            case ViewEventType::MouseWheel:
                dollyCamera(model_.options(), -event.wheelY * kDollySensitivity);
                model_.markCameraDirty();
                break;
            case ViewEventType::KeyDown:
                handleKey(event.key);
                break;
            default:
                break;
        }
    }

    void updateHeldMovement() {
        const MovementKeyState keys = view_.movementKeyState();
        double forward = 0.0;
        double right = 0.0;
        double panVertical = 0.0;
        if (keys.forward) {
            forward += kFlySpeed;
        }
        if (keys.back) {
            forward -= kFlySpeed;
        }
        if (keys.left) {
            right -= kFlySpeed;
        }
        if (keys.right) {
            right += kFlySpeed;
        }
        if (keys.panUp) {
            panVertical += kFlySpeed;
        }
        if (keys.panDown) {
            panVertical -= kFlySpeed;
        }

        bool moved = false;
        if (forward != 0.0 || right != 0.0) {
            flyCamera(model_.options(), forward, right);
            moved = true;
        }
        if (panVertical != 0.0) {
            panCamera(model_.options(), 0.0, panVertical);
            moved = true;
        }
        if (moved) {
            model_.markCameraDirty();
        }
    }

    void handleKey(int key) {
        if (key == 'r') {
            model_.resetAccumulation();
        }
    }

    RenderModel& model_;
    View& view_;
    bool orbiting_ = false;
    bool panning_ = false;
    int lastX_ = 0;
    int lastY_ = 0;
};
