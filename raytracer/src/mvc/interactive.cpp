#include "interactive.h"

#include "../parse.h"
#include "controller.h"
#include "render_model.h"
#include "sdl_view.h"

int interactiveRender(int argc, char** argv, HitablePtr world, BackgroundFn background) {
    RenderOptions opts;
    //opts.samples = 1;
    switch (parseOptions(argc, argv, opts)) {
        case 0:
            break;
        case 2:
            return 0;
        default:
            return 1;
    }
    opts.background = background;

    const int height = renderHeight(opts);
    RenderModel model(world, opts);
    auto view = makeSdlView(
        "pathtracer",
        opts.width,
        height,
        opts.displayWidth,
        opts.displayHeight,
        opts.gamma,
        opts.showFps,
        opts.fullscreen);
    RenderController controller(model, *view);
    controller.run();
    return 0;
}
