#include "sdl_view.h"

#include "../renderer.h"
#include "view_events.h"

#include <SDL.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <vector>



class SdlView : public View {
public:
    SdlView(
        const std::string& title,
        int renderWidth,
        int renderHeight,
        int displayWidth,
        int displayHeight,
        bool fullscreen)
        : renderWidth_(renderWidth),
          renderHeight_(renderHeight),
          displayWidth_(displayWidth),
          displayHeight_(displayHeight),
          pixels_(renderWidth * renderHeight * 3) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
            std::exit(1);
        }

        Uint32 windowFlags = SDL_WINDOW_SHOWN;
        if (fullscreen) {
            windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }

        const int windowWidth = fullscreen ? renderWidth : displayWidth;
        const int windowHeight = fullscreen ? renderHeight : displayHeight;
        window_ = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            windowWidth,
            windowHeight,
            windowFlags);
        if (!window_) {
            std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
            std::exit(1);
        }

        if (fullscreen) {
            SDL_GetWindowSize(window_, &displayWidth_, &displayHeight_);
        }

        renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer_) {
            std::fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
            std::exit(1);
        }

        texture_ = SDL_CreateTexture(
            renderer_,
            SDL_PIXELFORMAT_RGB24,
            SDL_TEXTUREACCESS_STREAMING,
            renderWidth,
            renderHeight);
        if (!texture_) {
            std::fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
            std::exit(1);
        }

        SDL_SetTextureScaleMode(texture_, SDL_ScaleModeNearest);
    }

    ~SdlView() override {
        if (texture_) {
            SDL_DestroyTexture(texture_);
        }
        if (renderer_) {
            SDL_DestroyRenderer(renderer_);
        }
        if (window_) {
            SDL_DestroyWindow(window_);
        }
        SDL_Quit();
    }

    /**
    *\brief Observer that prepares the event
    *\param out ViewEvent that is to be handled
    *\returns boolean; \sa RenderController::run() (used in while loop)
    */
    bool pollEvent(ViewEvent& out) override {
        SDL_Event sdlEvent;
        if (!SDL_PollEvent(&sdlEvent)) {
            return false;
        }

        out = ViewEvent{};
        switch (sdlEvent.type) {
            case SDL_QUIT:
                out.type = ViewEventType::Quit;
                shouldClose_ = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                out.type = ViewEventType::MouseButtonDown;
                out.button = sdlEvent.button.button;
                out.x = mapToRenderX(sdlEvent.button.x);
                out.y = mapToRenderY(sdlEvent.button.y);
                break;
            case SDL_MOUSEBUTTONUP:
                out.type = ViewEventType::MouseButtonUp;
                out.button = sdlEvent.button.button;
                out.x = mapToRenderX(sdlEvent.button.x);
                out.y = mapToRenderY(sdlEvent.button.y);
                break;
            case SDL_MOUSEMOTION:
                out.type = ViewEventType::MouseMotion;
                out.x = mapToRenderX(sdlEvent.motion.x);
                out.y = mapToRenderY(sdlEvent.motion.y);
                out.xrel = mapDeltaToRenderX(sdlEvent.motion.xrel);
                out.yrel = mapDeltaToRenderY(sdlEvent.motion.yrel);
                break;
            case SDL_MOUSEWHEEL:
                out.type = ViewEventType::MouseWheel;
                out.wheelY = static_cast<float>(sdlEvent.wheel.y);
                break;
            case SDL_KEYDOWN:
                out.type = ViewEventType::KeyDown;
                if (sdlEvent.key.keysym.sym >= SDLK_a && sdlEvent.key.keysym.sym <= SDLK_z) {
                    out.key = 'a' + (sdlEvent.key.keysym.sym - SDLK_a);
                } else {
                    out.key = sdlEvent.key.keysym.sym;
                }
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                    shouldClose_ = true;
                }
                break;
            default:
                out.type = ViewEventType::None;
                break;
        }
        return out.type != ViewEventType::None;
    }

    MovementKeyState movementKeyState() const override {
        const Uint8* state = SDL_GetKeyboardState(nullptr);
        return MovementKeyState{
            state[SDL_SCANCODE_W] != 0,
            state[SDL_SCANCODE_S] != 0,
            state[SDL_SCANCODE_A] != 0,
            state[SDL_SCANCODE_D] != 0,
            state[SDL_SCANCODE_SPACE] != 0,
            state[SDL_SCANCODE_LSHIFT] != 0 || state[SDL_SCANCODE_RSHIFT] != 0,
        };
    }

    /**
    *\brief converts framebuffer data into a rendered SDL window that is displayed
    *\param samples \copydoc RenderModel::sampleCount
    */
    void present(const Framebuffer& fb, int samples) override {
        framebufferToRgb(fb, pixels_);

        void* pixels = nullptr;
        int pitch = 0;
        if (SDL_LockTexture(texture_, nullptr, &pixels, &pitch) != 0) {
            std::fprintf(stderr, "SDL_LockTexture failed: %s\n", SDL_GetError());
            return;
        }

        const int rowBytes = renderWidth_ * 3;
        for (int row = 0; row < renderHeight_; ++row) {
            std::memcpy(
                static_cast<char*>(pixels) + row * pitch,
                pixels_.data() + row * rowBytes,
                rowBytes);
        }

        SDL_UnlockTexture(texture_);
        SDL_RenderClear(renderer_);

        SDL_Rect dst{0, 0, displayWidth_, displayHeight_};
        SDL_RenderCopy(renderer_, texture_, nullptr, &dst);
        SDL_RenderPresent(renderer_);

        char title[128];
        std::snprintf(
            title,
            sizeof(title),
            "pathtracer — %d samples (%dx%d -> %dx%d)",
            samples,
            renderWidth_,
            renderHeight_,
            displayWidth_,
            displayHeight_);
        SDL_SetWindowTitle(window_, title);
    }

    bool shouldClose() const override {
        return shouldClose_;
    }

private:
    /**
     * \brief Maps a window-space X coordinate to render-buffer space.
     *
     * Mouse events from SDL are in display/window pixels, but the path tracer
     * framebuffer may be smaller when upscaling (e.g. render 640 wide, window
     * 1920 wide). This scales windowX by renderWidth / displayWidth so orbit
     * and pan deltas align with traced pixels.
     *
     * \param windowX horizontal position in window pixels (origin top-left)
     * \returns corresponding X in [0, renderWidth_ - 1]
     */
    int mapToRenderX(int windowX) const {
        const int x = static_cast<int>(
            windowX * static_cast<double>(renderWidth_) / displayWidth_);
        return std::clamp(x, 0, renderWidth_ - 1);
    }

    /**
     * \brief Maps a window-space Y coordinate to render-buffer space.
     *
     * Same scaling as mapToRenderX, using renderHeight / displayHeight.
     *
     * \param windowY vertical position in window pixels (origin top-left)
     * \returns corresponding Y in [0, renderHeight_ - 1]
     */
    int mapToRenderY(int windowY) const {
        const int y = static_cast<int>(
            windowY * static_cast<double>(renderHeight_) / displayHeight_);
        return std::clamp(y, 0, renderHeight_ - 1);
    }

    /** \brief Scales a horizontal mouse delta from window space to render space. */
    int mapDeltaToRenderX(int delta) const {
        return static_cast<int>(
            delta * static_cast<double>(renderWidth_) / displayWidth_);
    }

    /** \brief Scales a vertical mouse delta from window space to render space. */
    int mapDeltaToRenderY(int delta) const {
        return static_cast<int>(
            delta * static_cast<double>(renderHeight_) / displayHeight_);
    }

    int renderWidth_;
    int renderHeight_;
    int displayWidth_;
    int displayHeight_;
    std::vector<uint8_t> pixels_;
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    SDL_Texture* texture_ = nullptr;
    bool shouldClose_ = false;
};

std::unique_ptr<View> makeSdlView(
    const std::string& title,
    int renderWidth,
    int renderHeight,
    int displayWidth,
    int displayHeight,
    bool fullscreen) {
    return std::make_unique<SdlView>(
        title,
        renderWidth,
        renderHeight,
        displayWidth,
        displayHeight,
        fullscreen);
}
