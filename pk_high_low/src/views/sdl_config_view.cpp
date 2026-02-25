#include "views/sdl_config_view.hpp"

#include <common_ui/font.hpp>
#include <common_ui/text.hpp>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>

#include <iostream>
#include <vector>
#include <filesystem>

namespace aff::pk_high_low::views {

sdl_config_view::sdl_config_view(sdl_utils::common::Window& window, const std::filesystem::path& assets_path)
    : window_(&window), assets_path_(assets_path)
{
    result_ = {};
}

// Helper: pick a font from assets or fallback system path
static std::string find_font(const std::filesystem::path& assets_path) {
    std::vector<std::filesystem::path> candidates {
        assets_path / "fonts" / "Roboto-Black.ttf",
        assets_path / "fonts" / "DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
    };
    for (const auto &p : candidates) if (std::filesystem::exists(p)) return p.string();
    return std::string();
}

aff::pk_high_low::controller::game_settings sdl_config_view::run_blocking()
{
    using namespace aff::sdl_utils::common;
    if (!window_) return result_;

    SDL_Renderer* renderer = window_->renderer();
    const int win_w = 800; // assume window size is adequate; window may be resized by user

    std::string font_path = find_font(assets_path_);
    Font font((font_path.empty() ? "" : font_path), 20);

    // UI state
    int field_idx = 0; // 0: score,1:pokedex,2:difficulty,3:complete
    bool done = false;

    auto& settings = result_;

    while (!done) {
        // handle events
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) { done = true; break; }
            if (ev.type == SDL_KEYDOWN) {
                switch (ev.key.keysym.sym) {
                    case SDLK_UP: field_idx = (field_idx + 3) % 4; break;
                    case SDLK_DOWN: field_idx = (field_idx + 1) % 4; break;
                    case SDLK_LEFT:
                    case SDLK_RIGHT: {
                        // cycle values depending on field
                        if (field_idx == 0) {
                            // score mode
                            using gm = aff::pk_high_low::controller::game_mode;
                            if (ev.key.keysym.sym == SDLK_LEFT)
                                settings.mode = static_cast<gm>((static_cast<int>(settings.mode) + 3) % 3);
                            else
                                settings.mode = static_cast<gm>((static_cast<int>(settings.mode) + 1) % 3);
                        } else if (field_idx == 1) {
                            using pm = aff::pk_high_low::controller::pokedex_mode;
                            if (ev.key.keysym.sym == SDLK_LEFT)
                                settings.pokedex = static_cast<pm>((static_cast<int>(settings.pokedex) + 2) % 3);
                            else
                                settings.pokedex = static_cast<pm>((static_cast<int>(settings.pokedex) + 1) % 3);
                        } else if (field_idx == 2) {
                            using dm = aff::pk_high_low::controller::difficulty_mode;
                            if (ev.key.keysym.sym == SDLK_LEFT)
                                settings.difficulty = static_cast<dm>((static_cast<int>(settings.difficulty) + 3) % 4);
                            else
                                settings.difficulty = static_cast<dm>((static_cast<int>(settings.difficulty) + 1) % 4);
                        } else if (field_idx == 3) {
                            settings.complete_mode = !settings.complete_mode;
                        }
                    } break;
                    case SDLK_RETURN:
                    case SDLK_SPACE:
                        done = true; break;
                    case SDLK_q:
                        // cancel - set complete_mode false and exit
                        done = true; break;
                }
            }
            if (ev.type == SDL_MOUSEBUTTONDOWN) {
                // treat as accept
                done = true; break;
            }
        }

        // render
        window_->clear(SDL_Color{10,10,10,255});

        // Title
        {
            std::string title = "PK High-Low — Configuration";
            auto tex = Text::render(font, title, SDL_Color{200,200,200,255}, renderer);
            if (tex.raw()) {
                SDL_Texture* t = tex.raw();
                int tw = 0, th = 0; SDL_QueryTexture(t, nullptr, nullptr, &tw, &th);
                SDL_Rect dst{ 20, 20, tw, th };
                SDL_RenderCopy(renderer, t, nullptr, &dst);
            }
        }

        // Lines
        auto draw_line = [&](int idy, const std::string& label, const std::string& value, bool highlight){
            int y = 80 + idy * 48;
            SDL_Color col = highlight ? SDL_Color{100,200,180,255} : SDL_Color{220,220,220,255};
            auto lbl_tex = Text::render(font, label, col, renderer);
            if (lbl_tex.raw()) {
                SDL_Texture* t = lbl_tex.raw(); int tw=0,th=0; SDL_QueryTexture(t,nullptr,nullptr,&tw,&th);
                SDL_Rect dst{20, y, tw, th}; SDL_RenderCopy(renderer, t, nullptr, &dst);
            }
            auto val_tex = Text::render(font, value, SDL_Color{200,200,200,255}, renderer);
            if (val_tex.raw()) {
                SDL_Texture* t = val_tex.raw(); int tw=0,th=0; SDL_QueryTexture(t,nullptr,nullptr,&tw,&th);
                SDL_Rect dst{400, y, tw, th}; SDL_RenderCopy(renderer, t, nullptr, &dst);
            }
        };

        // Prepare value strings
        auto score_str = [&](){ using gm = aff::pk_high_low::controller::game_mode; switch(settings.mode){case gm::BST: return std::string("BST"); case gm::SPEED: return std::string("SPEED"); default: return std::string("ATTACK");}};
        auto pokedex_str = [&](){ using pm = aff::pk_high_low::controller::pokedex_mode; switch(settings.pokedex){case pm::STANDARD: return std::string("STANDARD"); case pm::GEN1_ONLY: return std::string("GEN1_ONLY"); default: return std::string("VGC");}};
        auto diff_str = [&](){ using dm = aff::pk_high_low::controller::difficulty_mode; switch(settings.difficulty){case dm::RANDOM: return std::string("RANDOM"); case dm::MATCHED: return std::string("MATCHED"); case dm::HARD: return std::string("HARD"); default: return std::string("CUSTOM");}};

        draw_line(0, "Score Mode:", score_str(), field_idx==0);
        draw_line(1, "Pokedex:", pokedex_str(), field_idx==1);
        draw_line(2, "Difficulty:", diff_str(), field_idx==2);
        draw_line(3, "Complete Mode:", settings.complete_mode ? "ON" : "OFF", field_idx==3);

        // Footer
        auto footer = Text::render(font, "Use Up/Down to move, Left/Right to change, Enter to start", SDL_Color{160,160,160,255}, renderer);
        if (footer.raw()) { SDL_Texture* t = footer.raw(); int tw=0,th=0; SDL_QueryTexture(t,nullptr,nullptr,&tw,&th); SDL_Rect dst{20, 80+4*48, tw, th}; SDL_RenderCopy(renderer,t,nullptr,&dst); }

        window_->present();

        SDL_Delay(16);
    }

    return result_;
}

bool sdl_config_view::update()
{
    // Non-blocking update not implemented yet.
    return false;
}

aff::pk_high_low::controller::game_settings sdl_config_view::get_result() const
{
    return result_;
}

} // namespace aff::pk_high_low::views
