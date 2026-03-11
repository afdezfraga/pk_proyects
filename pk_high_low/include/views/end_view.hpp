#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_END_VIEW_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_END_VIEW_HPP

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <common_ui/window.hpp>
#include <common_ui/font.hpp>
#include <common_ui/text.hpp>

#include <controller/controller_api.hpp>

#include <string>
#include <filesystem>

namespace aff::pk_high_low::views {

class EndView {
public:
    EndView(aff::sdl_utils::common::Window& window, const std::filesystem::path& assets_path)
        : window_(&window), assets_path_(assets_path)
    {
        auto fp = find_font(assets_path_);
        std::string p = fp.empty() ? std::string() : fp;
        font_ = aff::sdl_utils::common::Font(p, 24);
        title_font_ = aff::sdl_utils::common::Font(p, 56);
        big_font_ = aff::sdl_utils::common::Font(p, 120);
        small_font_ = aff::sdl_utils::common::Font(p, 16);

        std::string title_str = "SYSTEM OVERLOAD";
        title_texture_ = aff::sdl_utils::common::Text::render(title_font_, title_str, SDL_Color{20,220,230,255}, window.renderer());
        subtitle_texture_ = aff::sdl_utils::common::Text::render(small_font_, "Memory leak detected in Rotom-Dex core interface. Fatal kernel panic.", SDL_Color{200,170,170,255}, window.renderer());
    }

    void tick(const SDL_Event* ev, controller::AppContext& ctx, controller::AppAPI& api) {
        // input handling
        if (ev) {
            if (ev->type == SDL_KEYDOWN) {
                switch (ev->key.keysym.sym) {
                    case SDLK_RETURN: api.request(controller::Action::RESTART); return;
                    case SDLK_BACKSPACE: api.request(controller::Action::BACK_TO_SETTINGS); return;
                    case SDLK_ESCAPE: api.request(controller::Action::QUIT); return;
                    case SDLK_r: api.request(controller::Action::RESTART); return;
                    default: break;
                }
            }
            if (ev->type == SDL_MOUSEBUTTONDOWN) {
                int mx = ev->button.x; int my = ev->button.y;
                update_layout();
                if (point_in_rect(mx, my, retry_btn_)) { api.request(controller::Action::RESTART); return; }
                if (point_in_rect(mx, my, reboot_btn_)) { api.request(controller::Action::BACK_TO_SETTINGS); return; }
                if (point_in_rect(mx, my, shutdown_btn_)) { api.request(controller::Action::QUIT); return; }
            }
        }

        // render
        if (!window_) return;
        auto* ren = window_->renderer();
        // deep red background like the reference
        window_->clear(SDL_Color{36,6,6,255});

        update_layout();

        // compute win size for centering
        int win_w, win_h; SDL_GetWindowSize(window_->raw(), &win_w, &win_h);

        // Top warning bar
        SDL_SetRenderDrawColor(ren, 80, 10, 10, 255);
        SDL_RenderFillRect(ren, &top_bar_);
        // simple icon (round) + 'CRITICAL ERROR' label
        SDL_SetRenderDrawColor(ren, 200, 40, 40, 255);
        SDL_RenderFillRect(ren, &warning_icon_);
        auto crit_t = aff::sdl_utils::common::Text::render(font_, "CRITICAL ERROR", SDL_Color{240,240,240,255}, ren);
        if (crit_t.raw()) { int tw,th; SDL_QueryTexture(crit_t.raw(), nullptr, nullptr, &tw, &th); SDL_Rect dst{ (win_w - tw)/2, top_bar_.y + (top_bar_.h - th)/2, tw, th }; SDL_RenderCopy(ren, crit_t.raw(), nullptr, &dst); }

        // Large circular emblem (approx as filled rect with ring)
        SDL_SetRenderDrawColor(ren, 80, 20, 20, 160);
        SDL_RenderFillRect(ren, &emblem_outer_);
        SDL_SetRenderDrawColor(ren, 200, 80, 80, 200);
        SDL_RenderDrawRect(ren, &emblem_outer_);

        // Neon title: render glow by drawing multiple alpha layers
        {
            if (title_texture_.raw()) {
                // glow layers
                for (int a = 40; a <= 200; a += 40) {
                    SDL_SetTextureColorMod(title_texture_.raw(), 20, 200, 220);
                    SDL_SetTextureAlphaMod(title_texture_.raw(), (Uint8)a);
                    SDL_RenderCopy(ren, title_texture_.raw(), nullptr, &title_rect_);
                }
                // main layer full
                SDL_SetTextureAlphaMod(title_texture_.raw(), 255);
                SDL_RenderCopy(ren, title_texture_.raw(), nullptr, &title_rect_);
            }
        }

        // subtitle
        if (subtitle_texture_.raw()) {
            SDL_RenderCopy(ren, subtitle_texture_.raw(), nullptr, &subtitle_rect_);
        }

        // Large score box (centered under subtitle)
        SDL_SetRenderDrawColor(ren, 60, 20, 20, 200);
        SDL_RenderFillRect(ren, &score_box_);
        SDL_SetRenderDrawColor(ren, 140, 40, 40, 200);
        SDL_RenderDrawRect(ren, &score_box_);

        // Score label + number
        auto lbl = aff::sdl_utils::common::Text::render(font_, "FINAL STREAK", SDL_Color{220,160,160,255}, ren);
        if (lbl.raw()) { int tw,th; SDL_QueryTexture(lbl.raw(), nullptr, nullptr, &tw, &th); SDL_Rect dst{ score_box_.x + (score_box_.w - tw)/2, score_box_.y + 16, tw, th }; SDL_RenderCopy(ren, lbl.raw(), nullptr, &dst); }

        std::string score_str = std::to_string(ctx.last_game_score);
        auto score_tex = aff::sdl_utils::common::Text::render(big_font_, score_str, SDL_Color{250,250,250,255}, ren);
        if (score_tex.raw()) { int tw,th; SDL_QueryTexture(score_tex.raw(), nullptr, nullptr, &tw, &th); SDL_Rect dst{ score_box_.x + (score_box_.w - tw)/2, score_box_.y + 48, tw, th }; SDL_RenderCopy(ren, score_tex.raw(), nullptr, &dst); }

        // Buttons
        // retry (bright cyan small)
        SDL_SetRenderDrawColor(ren, 6, 200, 200, 220);
        SDL_RenderFillRect(ren, &retry_btn_);
        SDL_SetRenderDrawColor(ren, 6, 120, 120, 200);
        SDL_RenderDrawRect(ren, &retry_btn_);
        auto retry_t = aff::sdl_utils::common::Text::render(font_, "RETRY", SDL_Color{12,12,12,255}, ren);
        if (retry_t.raw()) { int tw,th; SDL_QueryTexture(retry_t.raw(), nullptr, nullptr, &tw, &th); SDL_Rect dst{ retry_btn_.x + (retry_btn_.w - tw)/2, retry_btn_.y + (retry_btn_.h - th)/2, tw, th }; SDL_RenderCopy(ren, retry_t.raw(), nullptr, &dst); }

        // reboot (primary orange)
        SDL_SetRenderDrawColor(ren, 232, 110, 22, 220);
        SDL_RenderFillRect(ren, &reboot_btn_);
        SDL_SetRenderDrawColor(ren, 180, 80, 30, 200);
        SDL_RenderDrawRect(ren, &reboot_btn_);
        auto reboot_t = aff::sdl_utils::common::Text::render(font_, "REBOOT SYSTEM", SDL_Color{14,14,14,255}, ren);
        if (reboot_t.raw()) { int tw,th; SDL_QueryTexture(reboot_t.raw(), nullptr, nullptr, &tw, &th); SDL_Rect dst{ reboot_btn_.x + (reboot_btn_.w - tw)/2, reboot_btn_.y + (reboot_btn_.h - th)/2, tw, th }; SDL_RenderCopy(ren, reboot_t.raw(), nullptr, &dst); }

        // shutdown (muted)
        SDL_SetRenderDrawColor(ren, 40, 20, 20, 220);
        SDL_RenderFillRect(ren, &shutdown_btn_);
        SDL_SetRenderDrawColor(ren, 90, 40, 40, 200);
        SDL_RenderDrawRect(ren, &shutdown_btn_);
        auto shut_t = aff::sdl_utils::common::Text::render(font_, "SHUT DOWN", SDL_Color{180,140,140,255}, ren);
        if (shut_t.raw()) { int tw,th; SDL_QueryTexture(shut_t.raw(), nullptr, nullptr, &tw, &th); SDL_Rect dst{ shutdown_btn_.x + (shutdown_btn_.w - tw)/2, shutdown_btn_.y + (shutdown_btn_.h - th)/2, tw, th }; SDL_RenderCopy(ren, shut_t.raw(), nullptr, &dst); }

        window_->present();
    }

private:
    aff::sdl_utils::common::Window* window_ = nullptr;
    std::filesystem::path assets_path_;
    aff::sdl_utils::common::Font font_ { find_font(assets_path_), 24 };
    aff::sdl_utils::common::Font title_font_ { find_font(assets_path_), 56 };
    aff::sdl_utils::common::Font big_font_ { find_font(assets_path_), 120 };
    aff::sdl_utils::common::Font small_font_ { find_font(assets_path_), 16 };

    SDL_Rect top_bar_ { 0, 0, 800, 64 };
    SDL_Rect emblem_outer_ { 520, 20, 240, 240 };
    SDL_Rect warning_icon_ { 16, 8, 48, 48 };

    // Title / subtitle textures and layout (made members so other code can query them)
    aff::sdl_utils::common::Texture title_texture_;
    aff::sdl_utils::common::Texture subtitle_texture_;
    SDL_Rect title_rect_ { 0, 0, 0, 0 };
    SDL_Rect subtitle_rect_ { 0, 0, 0, 0 };

    SDL_Rect score_box_ { 40, 160, 520, 180 };
    SDL_Rect retry_btn_ { 60, 360, 480, 64 };
    SDL_Rect reboot_btn_ { 60, 440, 480, 64 };
    SDL_Rect shutdown_btn_ { 60, 520, 480, 64 };

    static std::string find_font(const std::filesystem::path& assets_path) {
        std::vector<std::filesystem::path> candidates {
            assets_path / "fonts" / "Roboto-Black.ttf",
            assets_path / "fonts" / "DejaVuSans.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        };
        for (const auto &p : candidates) if (std::filesystem::exists(p)) return p.string();
        return std::string();
    }

    void update_layout() {
        if (!window_) return;
        int win_w, win_h; SDL_GetWindowSize(window_->raw(), &win_w, &win_h);
        int margin = 40;
        int small_margin = 16;
        
        // top bar and emblem positions
        top_bar_ = { 0, 0, win_w, 64 };
        emblem_outer_ = { win_w - 260, margin, 220, 220 };
        warning_icon_ = { 16, 8, 48, 48 };

        // Start with buttons bottom up
        int bw = win_w - margin * 2;
        int bh = 180;

        int btn_w = bw;
        int btn_h = 64;

        shutdown_btn_ = { margin, win_h - margin - btn_h, btn_w, btn_h };
        reboot_btn_ = { margin, shutdown_btn_.y - small_margin - btn_h, btn_w, btn_h };
        retry_btn_ = { margin, reboot_btn_.y - small_margin - btn_h, btn_w, btn_h };
        
        score_box_ = { margin, retry_btn_.y - margin - bh, bw, bh };

        // Title centered in upper half, subtitle below it
        int tw = title_texture_.width();
        int th = title_texture_.height();

        int stw = subtitle_texture_.width();
        int sth = subtitle_texture_.height();

        int tx = (win_w - tw)/2;
        int stx = (win_w - stw)/2;

        int mid_y = (score_box_.y + top_bar_.h) / 2;
        title_rect_ = { tx, mid_y, tw, th };
        subtitle_rect_ = { stx, title_rect_.y + title_rect_.h + small_margin, stw, sth };;


        



        
    }

    static bool point_in_rect(int x, int y, const SDL_Rect& r) {
        return x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h;
    }
};

} // namespace aff::pk_high_low::views

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_END_VIEW_HPP
