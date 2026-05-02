#include "widgets/EnumSelector.hpp"
#include <common_ui/Theme.hpp>
#include <common_ui/text.hpp>
#include <SDL2/SDL.h>

namespace aff::sdl_utils::widgets {

void EnumSelector::setOptions(const std::vector<std::string>& opts) {
    options_ = opts;
    if (value_idx_ >= options_.size()) value_idx_ = options_.empty() ? 0 : options_.size() - 1;
    invalidateTextures();
}

void EnumSelector::setValue(size_t idx) {
    if (options_.empty()) return;
    if (idx >= options_.size()) idx = options_.size() - 1;
    value_idx_ = idx;
    if (cb_) cb_(value_idx_);
    invalidateTextures();
}

void EnumSelector::invalidateTextures() {
    // textures are created lazily in render since we need a renderer
    label_tex_ = aff::sdl_utils::common::Texture(nullptr);
    left_tex_ = aff::sdl_utils::common::Texture(nullptr);
    right_tex_ = aff::sdl_utils::common::Texture(nullptr);
}

void EnumSelector::computeLayout(int parent_w, int parent_h) {
    aff::sdl_utils::common::Widget::computeLayout(parent_w, parent_h);
    // rectangles will be set in render based on actual texture sizes
}

void EnumSelector::render(SDL_Renderer* ren) {
    if (!visible()) return;
    aff::sdl_utils::common::Theme theme;
    SDL_Color fg{255,255,255,255};
    SDL_Color accent = theme.primary;

    // create textures lazily if needed
    if (font_) {
        if (!left_tex_.raw()) left_tex_ = aff::sdl_utils::common::Text::render(*font_, "<", fg, ren);
        if (!right_tex_.raw()) right_tex_ = aff::sdl_utils::common::Text::render(*font_, ">", fg, ren);
        if (!label_tex_.raw() && !options_.empty()) label_tex_ = aff::sdl_utils::common::Text::render(*font_, options_[value_idx_], accent, ren);
    }

    int lx=0,ly=0,lw=0,lh=0;
    int tx=0,ty=0,tw=0,th=0;
    int rx=0,ry=0,rw=0,rh=0;
    if (left_tex_.raw()) SDL_QueryTexture(left_tex_.raw(), nullptr, nullptr, &lw, &lh);
    if (label_tex_.raw()) SDL_QueryTexture(label_tex_.raw(), nullptr, nullptr, &tw, &th);
    if (right_tex_.raw()) SDL_QueryTexture(right_tex_.raw(), nullptr, nullptr, &rw, &rh);

    // layout: left 12%, right 12%, label centered in remaining space
    int padding = 8;
    int area_x = x();
    int area_y = y();
    int area_w = w();
    int area_h = h();

    int left_w = std::max(lw, area_w / 10);
    int right_w = std::max(rw, area_w / 10);
    int label_w = area_w - left_w - right_w - padding * 2;

    left_rect_.x = area_x + padding;
    left_rect_.y = area_y + (area_h - lh) / 2;
    left_rect_.w = left_w;
    left_rect_.h = lh;

    label_rect_.x = area_x + padding + left_w;
    label_rect_.y = area_y + (area_h - th) / 2;
    label_rect_.w = label_w;
    label_rect_.h = th;

    right_rect_.x = area_x + padding + left_w + label_w;
    right_rect_.y = area_y + (area_h - rh) / 2;
    right_rect_.w = right_w;
    right_rect_.h = rh;

    // render backgrounds/frames
    SDL_Rect bg = { area_x, area_y, area_w, area_h };
    SDL_SetRenderDrawColor(ren, 30, 30, 30, 255);
    SDL_RenderFillRect(ren, &bg);

    // render left arrow
    if (left_tex_.raw()) SDL_RenderCopy(ren, left_tex_.raw(), nullptr, &left_rect_);
    // render label (clip to label_rect_)
    if (label_tex_.raw()) SDL_RenderCopy(ren, label_tex_.raw(), nullptr, &label_rect_);
    // render right arrow
    if (right_tex_.raw()) SDL_RenderCopy(ren, right_tex_.raw(), nullptr, &right_rect_);
}

bool EnumSelector::handleEvent(const SDL_Event& ev) {
    if (!visible()) return false;
    if (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_LEFT) {
        int mx = ev.button.x;
        int my = ev.button.y;
        if (mx < x() || mx > x() + w() || my < y() || my > y() + h()) return false;
        // determine clicked zone
        int relx = mx - x();
        if (relx < w() / 4) {
            // left
            if (value_idx_ > 0) setValue(value_idx_ - 1);
            return true;
        } else if (relx > (w() * 3) / 4) {
            // right
            if (!options_.empty() && value_idx_ + 1 < options_.size()) setValue(value_idx_ + 1);
            return true;
        } else {
            // center: cycle
            if (!options_.empty()) {
                size_t next = (value_idx_ + 1) % options_.size();
                setValue(next);
            }
            return true;
        }
    }
    return false;
}

} // namespace aff::sdl_utils::widgets
