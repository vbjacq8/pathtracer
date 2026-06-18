#pragma once

#include "../hitable.h"

/**
 * \brief Interactive renderer entry point with progressive accumulation and camera controls.
 * \param world scene to trace (caller retains ownership)
 * \returns process exit code
 */
int interactiveRender(int argc, char** argv, Hitable* world);
