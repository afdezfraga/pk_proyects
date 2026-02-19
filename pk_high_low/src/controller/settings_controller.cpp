#include "controller/settings_controller.hpp"

#include <SDL.h>
#include <common_ui/text.hpp>

using namespace aff::sdl_utils::common;
using namespace aff::pk_high_low::controller;

static std::string find_font(const std::filesystem::path& assets_path) {
    std::vector<std::filesystem::path> candidates {
        assets_path / "fonts" / "Roboto-Black.ttf",
        assets_path / "fonts" / "DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    };
    for (const auto &p : candidates) if (std::filesystem::exists(p)) return p.string();
    return std::string();
}

SettingsController::SettingsController(Window& window, const std::filesystem::path& assets_path)
    : window_(&window), assets_path_(assets_path),
      font_((find_font(assets_path_).empty() ? std::string() : find_font(assets_path_)), 20)
{
    settings_ = {};
}

void SettingsController::reset()
{
    settings_ = {};
    field_idx_ = 0;
}

void SettingsController::tick(const SDL_Event* ev, AppContext& ctx, AppAPI& api)
{
    // Process input event (if any)
    if (ev) {
        if (ev->type == SDL_KEYDOWN) {
            switch (ev->key.keysym.sym) {
                case SDLK_UP: field_idx_ = (field_idx_ + 3) % 4; break;
                case SDLK_DOWN: field_idx_ = (field_idx_ + 1) % 4; break;
                case SDLK_LEFT: {
                    if (field_idx_ == 0) settings_.mode = static_cast<game_mode>((static_cast<int>(settings_.mode) + 2) % 3);
                    else if (field_idx_ == 1) settings_.pokedex = static_cast<pokedex_mode>((static_cast<int>(settings_.pokedex) + 2) % 3);
                    else if (field_idx_ == 2) settings_.difficulty = static_cast<difficulty_mode>((static_cast<int>(settings_.difficulty) + 3) % 4);
                    else if (field_idx_ == 3) settings_.complete_mode = !settings_.complete_mode;
                } break;
                case SDLK_RIGHT: {
                    if (field_idx_ == 0) settings_.mode = static_cast<game_mode>((static_cast<int>(settings_.mode) + 1) % 3);
                    else if (field_idx_ == 1) settings_.pokedex = static_cast<pokedex_mode>((static_cast<int>(settings_.pokedex) + 1) % 3);
                    else if (field_idx_ == 2) settings_.difficulty = static_cast<difficulty_mode>((static_cast<int>(settings_.difficulty) + 1) % 4);
                    else if (field_idx_ == 3) settings_.complete_mode = !settings_.complete_mode;
                } break;
                case SDLK_RETURN:
                case SDLK_SPACE:
                    api.request(Action::START_GAME);
                    break;
                case SDLK_ESCAPE:
                    api.request(Action::QUIT);
                    break;
                default: break;
            }
        }
        if (ev->type == SDL_MOUSEBUTTONDOWN) {
            // treat as accept
            api.request(Action::START_GAME);
        }
    }

    // Render UI
    if (!window_) return;
    auto* renderer = window_->renderer();
    window_->clear(SDL_Color{10,10,10,255});

    // Title
    {
        auto tex = Text::render(font_, "PK High-Low — Configuration", SDL_Color{200,200,200,255}, renderer);
        if (tex.raw()) { int tw=0,th=0; SDL_QueryTexture(tex.raw(), nullptr, nullptr, &tw, &th); SDL_Rect dst{20,20,tw,th}; SDL_RenderCopy(renderer, tex.raw(), nullptr, &dst); }
    }

    auto draw_line = [&](int idy, const std::string& label, const std::string& value, bool highlight){
        int y = 80 + idy * 48;
        SDL_Color col = highlight ? SDL_Color{100,200,180,255} : SDL_Color{220,220,220,255};
        auto lbl_tex = Text::render(font_, label, col, renderer);
        if (lbl_tex.raw()) { int tw=0,th=0; SDL_QueryTexture(lbl_tex.raw(), nullptr, nullptr, &tw, &th); SDL_Rect dst{20, y, tw, th}; SDL_RenderCopy(renderer, lbl_tex.raw(), nullptr, &dst); }
        auto val_tex = Text::render(font_, value, SDL_Color{200,200,200,255}, renderer);
        if (val_tex.raw()) { int tw=0,th=0; SDL_QueryTexture(val_tex.raw(), nullptr, nullptr, &tw, &th); SDL_Rect dst{400, y, tw, th}; SDL_RenderCopy(renderer, val_tex.raw(), nullptr, &dst); }
    };

    auto score_str = [&](){ using gm = game_mode; switch(settings_.mode){case gm::BTS: return std::string("BTS"); case gm::SPEED: return std::string("SPEED"); default: return std::string("ATTACK");}};
    auto pokedex_str = [&](){ using pm = pokedex_mode; switch(settings_.pokedex){case pm::STANDARD: return std::string("STANDARD"); case pm::GEN1_ONLY: return std::string("GEN1_ONLY"); default: return std::string("VGC");}};
    auto diff_str = [&](){ using dm = difficulty_mode; switch(settings_.difficulty){case dm::RANDOM: return std::string("RANDOM"); case dm::MATCHED: return std::string("MATCHED"); case dm::HARD: return std::string("HARD"); default: return std::string("CUSTOM");}};

    draw_line(0, "Score Mode:", score_str(), field_idx_==0);
    draw_line(1, "Pokedex:", pokedex_str(), field_idx_==1);
    draw_line(2, "Difficulty:", diff_str(), field_idx_==2);
    draw_line(3, "Complete Mode:", settings_.complete_mode ? "ON" : "OFF", field_idx_==3);

    auto footer = Text::render(font_, "Use Up/Down to move, Left/Right to change, Enter to start", SDL_Color{160,160,160,255}, renderer);
    if (footer.raw()) { int tw=0,th=0; SDL_QueryTexture(footer.raw(), nullptr, nullptr, &tw, &th); SDL_Rect dst{20, 80+4*48, tw, th}; SDL_RenderCopy(renderer, footer.raw(), nullptr, &dst); }

    window_->present();
}
