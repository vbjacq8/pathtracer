#pragma once

#include "../hitable.h"
#include "../ray.h"

#include <memory>

/**
 * \brief Interactive renderer entry point with progressive accumulation and camera controls.
 * \param world scene to trace (kept alive for the duration of the interactive session)
 * \param background miss-ray radiance strategy (default sky gradient)
 * \returns process exit code
 */
int interactiveRender(int argc, char** argv, HitablePtr world,
                      BackgroundFn background = colorBlueWhiteGradient);
