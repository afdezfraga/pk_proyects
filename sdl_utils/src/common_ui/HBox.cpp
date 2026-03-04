#include "common_ui/HBox.hpp"
#include <SDL.h>
#include <algorithm>

namespace aff::sdl_utils::common {

void HBox::computeLayout(int parent_w, int parent_h) {
    // first compute our own geometry using base behavior
    Widget::computeLayout(parent_w, parent_h);

    int inner_x = x() + padding_;
    int inner_y = y() + padding_;
    int inner_w = std::max(0, w() - 2*padding_);
    int inner_h = std::max(0, h() - 2*padding_);

    int total_fixed = 0;
    int flexible_count = 0;
    for (auto& cptr : children_) {
        if (cptr->w() > 0) total_fixed += cptr->w();
        else flexible_count += 1;
    }

    int available = inner_w - total_fixed - spacing_ * std::max(0, (int)children_.size()-1);
    int flex_w = flexible_count > 0 ? std::max(0, available / flexible_count) : 0;

    int cur_x = inner_x;
    for (auto& cptr : children_) {
        int cw = cptr->w() > 0 ? cptr->w() : flex_w;
        int ch = inner_h;
        cptr->setSize(cw, ch);
        cptr->setPosition(cur_x, inner_y);
        cptr->computeLayout(cw, ch);
        cur_x += cw + spacing_;
    }
}

} // namespace aff::sdl_utils::common
