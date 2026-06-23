#include "interactive.h"

#include "../parse.h"
#include "controller.h"
#include "render_model.h"
#include "sdl_view.h"

int interactiveRender(int argc, char** argv, Hitable* world) {
    RenderOptions opts;
    opts.samples = 1;
    switch (parseOptions(argc, argv, opts)) {
        case 0:
            break;
        case 2:
            return 0;
        default:
            return 1;
    }

    const int height = renderHeight(opts);
    RenderModel model(world, opts);
    auto view = makeSdlView(
        "pathtracer",
        opts.width,
        height,
        opts.displayWidth,
        opts.displayHeight,
        opts.fullscreen);
    RenderController controller(model, *view);
    controller.run();
    return 0;
}
