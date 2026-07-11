#pragma once

#include "view.h"

#include <memory>
#include <string>

/**
 * \brief Creates an SDL-backed View.
 * \param title initial window title
 * \param renderWidth internal render width in pixels
 * \param renderHeight internal render height in pixels
 * \param displayWidth window width (ignored when \p fullscreen; resolved at runtime)
 * \param displayHeight window height (ignored when \p fullscreen; resolved at runtime)
 * \param gamma display gamma for tonemapped output
 * \param showFps update window title with frame rate (off by default)
 * \param fullscreen use borderless fullscreen desktop mode
 */
std::unique_ptr<View> makeSdlView(
    const std::string& title,
    int renderWidth,
    int renderHeight,
    int displayWidth,
    int displayHeight,
    double gamma,
    bool showFps,
    bool fullscreen);
