#pragma once

#include "view.h"

#include <memory>
#include <string>

/**
 * \brief Creates an SDL-backed View.
 * \param title initial window title
 * \param width window width in pixels
 * \param height window height in pixels
 */
std::unique_ptr<View> makeSdlView(const std::string& title, int width, int height);
