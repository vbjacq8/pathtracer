#include "sdl_view.h"

#include "../renderer.h"
#include "view_events.h"

#include <SDL.h>

#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <vector>



class SdlView : public View {
public:
    SdlView(const std::string& title, int width, int height)
        : width_(width), height_(height), pixels_(width * height * 3) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
            std::exit(1);
        }

        //Initialize SDL window and position
        window_ = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width,
            height,
            SDL_WINDOW_SHOWN);
        if (!window_) {
            std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
            std::exit(1);
        }

        //Initialize SDL renderer, connecting it to SDL window
        renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer_) {
            std::fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
            std::exit(1);
        }

        //Initialize SDL texture, connecting it to SDL renderer
        texture_ = SDL_CreateTexture(
            renderer_,
            SDL_PIXELFORMAT_RGB24,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height);
        if (!texture_) {
            std::fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
            std::exit(1);
        }
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
                out.x = sdlEvent.button.x;
                out.y = sdlEvent.button.y;
                break;
            case SDL_MOUSEBUTTONUP:
                out.type = ViewEventType::MouseButtonUp;
                out.button = sdlEvent.button.button;
                out.x = sdlEvent.button.x;
                out.y = sdlEvent.button.y;
                break;
            case SDL_MOUSEMOTION:
                out.type = ViewEventType::MouseMotion;
                out.x = sdlEvent.motion.x;
                out.y = sdlEvent.motion.y;
                out.xrel = sdlEvent.motion.xrel;
                out.yrel = sdlEvent.motion.yrel;
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

        const int rowBytes = width_ * 3;
        for (int row = 0; row < height_; ++row) {
            std::memcpy(
                static_cast<char*>(pixels) + row * pitch,
                pixels_.data() + row * rowBytes,
                rowBytes);
        }

        SDL_UnlockTexture(texture_);
        SDL_RenderClear(renderer_);
        SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
        SDL_RenderPresent(renderer_);

        char title[128];
        std::snprintf(title, sizeof(title), "pathtracer — %d samples", samples);
        SDL_SetWindowTitle(window_, title);
    }

    bool shouldClose() const override {
        return shouldClose_;
    }

private:
    int width_;
    int height_;
    std::vector<uint8_t> pixels_;
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    SDL_Texture* texture_ = nullptr;
    bool shouldClose_ = false;
};

std::unique_ptr<View> makeSdlView(const std::string& title, int width, int height) {
    return std::make_unique<SdlView>(title, width, height);
}
