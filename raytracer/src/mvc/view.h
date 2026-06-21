#pragma once

#include "../framebuffer.h"
#include "view_events.h"

/** \brief WASD movement keys currently held down. */
struct MovementKeyState {
    bool forward = false;
    bool back = false;
    bool left = false;
    bool right = false;
};

/**
 * \brief Abstract display and input surface for the interactive renderer.
 */
class View {
public:
    virtual ~View() = default;

    /**
     * \brief Dequeues one pending input event, if any.
     * \returns false when the event queue is empty
     */
    virtual bool pollEvent(ViewEvent& out) = 0;

    /** \returns Which movement keys are currently held (polled each frame). */
    virtual MovementKeyState movementKeyState() const = 0;

    /**
     * \brief Shows the current accumulated image.
     * \param fb framebuffer to display
     * \param samples sample count shown in the window title
     */
    virtual void present(const Framebuffer& fb, int samples) = 0;

    /** \returns true when the application should exit. */
    virtual bool shouldClose() const = 0;
};
