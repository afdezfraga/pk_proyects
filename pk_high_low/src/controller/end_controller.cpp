#include "controller/end_controller.hpp"

#include <SDL.h>
#include <common_ui/text.hpp>

using namespace aff::sdl_utils::common;
using namespace aff::pk_high_low::controller;

static std::string find_font_end(const std::filesystem::path& assets_path) {
    std::vector<std::filesystem::path> candidates {
        assets_path / "fonts" / "Roboto-Black.ttf",
        assets_path / "fonts" / "DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    };
    for (const auto &p : candidates) if (std::filesystem::exists(p)) return p.string();
    return std::string();
}

EndController::EndController(Window& window, const std::filesystem::path& assets_path)
    : window_(&window), assets_path_(assets_path), font_((find_font_end(assets_path_).empty() ? std::string() : find_font_end(assets_path_)), 24)
{}

void EndController::reset()
{
}

void EndController::tick(const SDL_Event* ev, AppContext& ctx, AppAPI& api)
{
    if (ev && ev->type == SDL_KEYDOWN) {
        switch (ev->key.keysym.sym) {
            case SDLK_RETURN:
                api.request(Action::RESTART);
                return;
            case SDLK_BACKSPACE:
                api.request(Action::BACK_TO_SETTINGS);
                return;
            case SDLK_ESCAPE:
                api.request(Action::QUIT);
                return;
            default: break;
        }
    }

    if (!window_) return;
    auto* ren = window_->renderer();
    window_->clear(SDL_Color{20,20,20,255});

    auto title = Text::render(font_, "Game Over", SDL_Color{240,240,240,255}, ren);
    if (title.raw()) { int tw=0,th=0; SDL_QueryTexture(title.raw(), nullptr, nullptr, &tw, &th); SDL_Rect dst{20,20,tw,th}; SDL_RenderCopy(ren, title.raw(), nullptr, &dst); }

    auto msg = Text::render(font_, "Enter = Restart   Backspace = Settings   Esc = Quit", SDL_Color{200,200,200,255}, ren);
    if (msg.raw()) { int tw=0,th=0; SDL_QueryTexture(msg.raw(), nullptr, nullptr, &tw, &th); SDL_Rect dst{20,80,tw,th}; SDL_RenderCopy(ren, msg.raw(), nullptr, &dst); }

    window_->present();
}
