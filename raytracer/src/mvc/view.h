#pragma once

#include "../framebuffer.h"
#include "view_events.h"

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

    /**
     * \brief Shows the current accumulated image.
     * \param fb framebuffer to display
     * \param samples sample count shown in the window title
     */
    virtual void present(const Framebuffer& fb, int samples) = 0;

    /** \returns true when the application should exit. */
    virtual bool shouldClose() const = 0;
};
