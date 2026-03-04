#include <controller/controller_api.hpp>
#include "views/sdl_config_view.hpp"

#include <common_ui/font.hpp>
#include <common_ui/text.hpp>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>

#include <iostream>
#include <vector>
#include <filesystem>

namespace aff::pk_high_low::views {

    void sdl_config_view::tick(const SDL_Event* ev, 
        aff::pk_high_low::controller::SettingsContext& ctx, 
        aff::pk_high_low::controller::SettingsAPI& api)
{
    using namespace aff::sdl_utils::common;
    using namespace aff::pk_high_low::controller;
    // UI state (persisted across ticks)
    auto& settings = ctx.settings;

    // Handle events
    if (ev) {
        if (ev->type == SDL_QUIT) 
           { api.request(ctx, SettingsAction::QUIT); return; }
        if (ev->type == SDL_KEYDOWN) {
            switch (ev->key.keysym.sym) {
                case SDLK_UP: field_idx_ = (field_idx_ + 3) % 4; break;
                case SDLK_DOWN: field_idx_ = (field_idx_ + 1) % 4; break;
                case SDLK_LEFT:
                case SDLK_RIGHT: {
                    if (field_idx_ == 0) {
                        using gm = aff::pk_high_low::controller::game_mode;
                        if (ev->key.keysym.sym == SDLK_LEFT)
                            settings.mode = static_cast<gm>((static_cast<int>(settings.mode) + 2) % 3);
                        else
                            settings.mode = static_cast<gm>((static_cast<int>(settings.mode) + 1) % 3);
                    } else if (field_idx_ == 1) {
                        using pm = aff::pk_high_low::controller::pokedex_mode;
                        if (ev->key.keysym.sym == SDLK_LEFT)
                            settings.pokedex = static_cast<pm>((static_cast<int>(settings.pokedex) + 2) % 3);
                        else
                            settings.pokedex = static_cast<pm>((static_cast<int>(settings.pokedex) + 1) % 3);
                    } else if (field_idx_ == 2) {
                        using dm = aff::pk_high_low::controller::difficulty_mode;
                        if (ev->key.keysym.sym == SDLK_LEFT)
                            settings.difficulty = static_cast<dm>((static_cast<int>(settings.difficulty) + 3) % 4);
                        else
                            settings.difficulty = static_cast<dm>((static_cast<int>(settings.difficulty) + 1) % 4);
                    } else if (field_idx_ == 3) {
                        settings.complete_mode = !settings.complete_mode;
                    }
                    api.request(ctx, SettingsAction::UPDATE_SETTINGS);
                } break;
                case SDLK_RETURN:
                case SDLK_SPACE:
                    api.request(ctx, SettingsAction::START_GAME); return;
                case SDLK_q:
                case SDLK_ESCAPE:
                    api.request(ctx, SettingsAction::QUIT); return;
            }
        }
        if (ev->type == SDL_MOUSEBUTTONDOWN) {
            api.request(ctx, SettingsAction::START_GAME); return;
        }
    }

    // Animated red background with diagonal black lines
    bg_.render(ctx.window->renderer());

    // Title
    {
        std::string title = "PK High-Low --- Configuration";
        auto tex = aff::sdl_utils::common::Text::render(ctx.font, title, SDL_Color{200,200,200,255}, ctx.window->renderer());
        if (tex.raw()) {
            SDL_Texture* t = tex.raw();
            int tw = 0, th = 0; SDL_QueryTexture(t, nullptr, nullptr, &tw, &th);
            SDL_Rect dst{ 20, 20, tw, th };
            SDL_RenderCopy(ctx.window->renderer(), t, nullptr, &dst);
        }
    }

    // Lines
    auto draw_line = [&](int idy, const std::string& label, const std::string& value, bool highlight){
        int y = 80 + idy * 48;
        SDL_Color col = highlight ? SDL_Color{100,200,180,255} : SDL_Color{220,220,220,255};
        auto lbl_tex = aff::sdl_utils::common::Text::render(ctx.font, label, col, ctx.window->renderer());
        if (lbl_tex.raw()) {
            SDL_Texture* t = lbl_tex.raw(); int tw=0,th=0; SDL_QueryTexture(t,nullptr,nullptr,&tw,&th);
            SDL_Rect dst{20, y, tw, th}; SDL_RenderCopy(ctx.window->renderer(), t, nullptr, &dst);
        }
        auto val_tex = aff::sdl_utils::common::Text::render(ctx.font, value, SDL_Color{200,200,200,255}, ctx.window->renderer());
        if (val_tex.raw()) {
            SDL_Texture* t = val_tex.raw(); int tw=0,th=0; SDL_QueryTexture(t,nullptr,nullptr,&tw,&th);
            SDL_Rect dst{400, y, tw, th}; SDL_RenderCopy(ctx.window->renderer(), t, nullptr, &dst);
        }
    };

    auto score_str = [&](){ using gm = aff::pk_high_low::controller::game_mode; switch(settings.mode){case gm::BST: return std::string("BST"); case gm::SPEED: return std::string("SPEED"); default: return std::string("ATTACK");}};
    auto pokedex_str = [&](){ using pm = aff::pk_high_low::controller::pokedex_mode; switch(settings.pokedex){case pm::STANDARD: return std::string("STANDARD"); case pm::GEN1_ONLY: return std::string("GEN1_ONLY"); default: return std::string("VGC");}};
    auto diff_str = [&](){ using dm = aff::pk_high_low::controller::difficulty_mode; switch(settings.difficulty){case dm::RANDOM: return std::string("RANDOM"); case dm::MATCHED: return std::string("MATCHED"); case dm::HARD: return std::string("HARD"); default: return std::string("CUSTOM");}};

    draw_line(0, "Score Mode:", score_str(), field_idx_==0);
    draw_line(1, "Pokedex:", pokedex_str(), field_idx_==1);
    draw_line(2, "Difficulty:", diff_str(), field_idx_==2);
    draw_line(3, "Complete Mode:", settings.complete_mode ? "ON" : "OFF", field_idx_==3);

    // Footer
    auto footer = aff::sdl_utils::common::Text::render(ctx.font, "Use Up/Down to move, Left/Right to change, Enter to start", SDL_Color{160,160,160,255}, ctx.window->renderer());
    if (footer.raw()) { SDL_Texture* t = footer.raw(); int tw=0,th=0; SDL_QueryTexture(t,nullptr,nullptr,&tw,&th); SDL_Rect dst{20, 80+4*48, tw, th}; SDL_RenderCopy(ctx.window->renderer(), t,nullptr,&dst); }

    ctx.window->present();
}

} // namespace aff::pk_high_low::views
