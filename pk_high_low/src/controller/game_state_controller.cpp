#include "controller/game_state_controller.hpp"

#include <SDL.h>
#include <cmath>
#include <common_ui/text.hpp>

#include <controller/game_mode.hpp>

using namespace aff::sdl_utils::common;
using namespace aff::pk_high_low::controller;

static std::string find_font_game(const std::filesystem::path& assets_path) {
    std::vector<std::filesystem::path> candidates {
        assets_path / "fonts" / "Roboto-Black.ttf",
        assets_path / "fonts" / "DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    };
    for (const auto &p : candidates) if (std::filesystem::exists(p)) return p.string();
    return std::string();
}

GameStateController::GameStateController(Window& window, const std::filesystem::path& assets_path)
    : window_(&window), assets_path_(assets_path), font_((find_font_game(assets_path_).empty() ? std::string() : find_font_game(assets_path_)), 24)
{}

void GameStateController::configure(const game_settings& settings)
{
    // no-op for dummy
    mode_text_ = " Mode=" + std::to_string(static_cast<int>(settings.mode));
    pokedex_text_ = " Pokedex=" + std::to_string(static_cast<int>(settings.pokedex));
    difficulty_text_ = " Difficulty=" + std::to_string(static_cast<int>(settings.difficulty));
    complete_mode_text_ = " Complete Mode=" + (settings.complete_mode 
                                                ? std::string("ON") 
                                                : std::string("OFF"));
}

void GameStateController::reset()
{
    // nothing to reset for dummy
}

void GameStateController::restart()
{
    // nothing to restart for dummy
}

void GameStateController::tick(const SDL_Event* ev, AppContext& ctx, AppAPI& api)
{
    // Any input advances to end screen
    if (ev) {
        if (ev->type == SDL_KEYDOWN || ev->type == SDL_MOUSEBUTTONDOWN) {
            api.request(Action::SHOW_END);
            return;
        }
    }

    // Render pulsing background
    if (!window_) return;
    auto* ren = window_->renderer();
    const uint32_t t = SDL_GetTicks();
    float phase = (t % 2000) / 2000.0f; // 0..1
    float v = 0.5f + 0.5f * std::sin(phase * 2.0f * 3.14159265f);
    uint8_t iv = static_cast<uint8_t>(50 + 180 * v);
    window_->clear(SDL_Color{ iv, uint8_t(100 + iv/3), uint8_t(150 + iv/4), 255 });

    auto tex = Text::render(font_, "Dummy Game — press any key or click to end", SDL_Color{240,240,240,255}, ren);
    if (tex.raw()) { int tw=0,th=0; SDL_QueryTexture(tex.raw(), nullptr, nullptr, &tw, &th); SDL_Rect dst{20, 20, tw, th}; SDL_RenderCopy(ren, tex.raw(), nullptr, &dst); }

    // Render game state text
    auto mode_tex = Text::render(font_, mode_text_, SDL_Color{240,240,240,255}, ren);
    if (mode_tex.raw()) { int tw=0,th=0; SDL_QueryTexture(mode_tex.raw(), nullptr, nullptr, &tw, &th); SDL_Rect dst{20, 60, tw, th}; SDL_RenderCopy(ren, mode_tex.raw(), nullptr, &dst); }

    auto pokedex_tex = Text::render(font_, pokedex_text_, SDL_Color{240,240,240,255}, ren);
    if (pokedex_tex.raw()) { int tw=0,th=0; SDL_QueryTexture(pokedex_tex.raw(), nullptr, nullptr, &tw, &th); SDL_Rect dst{20, 90, tw, th}; SDL_RenderCopy(ren, pokedex_tex.raw(), nullptr, &dst); }

    auto difficulty_tex = Text::render(font_, difficulty_text_, SDL_Color{240,240,240,255}, ren);
    if (difficulty_tex.raw()) { int tw=0,th=0; SDL_QueryTexture(difficulty_tex.raw(), nullptr, nullptr, &tw, &th); SDL_Rect dst{20, 120, tw, th}; SDL_RenderCopy(ren, difficulty_tex.raw(), nullptr, &dst); }

    auto complete_mode_tex = Text::render(font_, complete_mode_text_, SDL_Color{240,240,240,255}, ren);
    if (complete_mode_tex.raw()) { int tw=0,th=0; SDL_QueryTexture(complete_mode_tex.raw(), nullptr, nullptr, &tw,&th); SDL_Rect dst{20 ,150,tw ,th }; SDL_RenderCopy(ren ,complete_mode_tex.raw() ,nullptr ,&dst ); }

    window_->present();
}
