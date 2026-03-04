#include "widgets/Button.hpp"
#include <SDL.h>
#include <SDL_ttf.h>
#include <common_ui/font.hpp>
#include <common_ui/text.hpp>

namespace aff::sdl_utils::widgets {

void Button::render(SDL_Renderer* renderer) {
    if (!visible()) return;
    SDL_Rect r{ x(), y(), w(), h() };
    // background
    if (pressed_) SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    else if (hover_) SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
    else SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &r);

    // border
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderDrawRect(renderer, &r);

    // text
    if (!text_.empty() && font_) {
        // render using existing Text helper (simple caching assumed)
        auto tex = aff::sdl_utils::common::Text::render(*font_, text_, SDL_Color{220,220,220,255}, renderer);
        if (tex.raw()) {
            SDL_Texture* t = tex.raw(); int tw=0,th=0; SDL_QueryTexture(t,nullptr,nullptr,&tw,&th);
            SDL_Rect dst{ r.x + (r.w - tw)/2, r.y + (r.h - th)/2, tw, th };
            SDL_RenderCopy(renderer, t, nullptr, &dst);
        }
    }
}

bool Button::handleEvent(const SDL_Event& ev) {
    if (!visible()) return false;
    if (ev.type == SDL_MOUSEMOTION) {
        int mx = ev.motion.x, my = ev.motion.y;
        hover_ = (mx >= x() && mx <= x()+w() && my >= y() && my <= y()+h());
    }
    if (ev.type == SDL_MOUSEBUTTONDOWN) {
        int mx = ev.button.x, my = ev.button.y;
        if (mx >= x() && mx <= x()+w() && my >= y() && my <= y()+h()) { pressed_ = true; return true; }
    }
    if (ev.type == SDL_MOUSEBUTTONUP) {
        int mx = ev.button.x, my = ev.button.y;
        if (pressed_) {
            pressed_ = false;
            if (mx >= x() && mx <= x()+w() && my >= y() && my <= y()+h()) {
                if (cb_) cb_();
                return true;
            }
        }
    }
    return false;
}

} // namespace aff::sdl_utils::widgets
