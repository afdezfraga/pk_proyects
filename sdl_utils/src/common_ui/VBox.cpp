#include "common_ui/VBox.hpp"
#include <SDL.h>
#include <algorithm>

namespace aff::sdl_utils::common {

void VBox::computeLayout(int parent_w, int parent_h) {
    // first compute our own geometry using base behavior
    Widget::computeLayout(parent_w, parent_h);

    int inner_x = x() + padding_;
    int inner_y = y() + padding_;
    int inner_w = std::max(0, w() - 2*padding_);
    int inner_h = std::max(0, h() - 2*padding_);

    // total fixed height and count of flexible children
    int total_fixed = 0;
    int flexible_count = 0;
    for (auto& cptr : children_) {
        // if child has percent height, allocate proportionally later
        // we cannot read private pct_h_, so rely on child's current h() being non-zero
        if (cptr->h() > 0) total_fixed += cptr->h();
        else flexible_count += 1;
    }

    int available = inner_h - total_fixed - spacing_ * std::max(0, (int)children_.size()-1);
    int flex_h = flexible_count > 0 ? std::max(0, available / flexible_count) : 0;

    int cur_y = inner_y;
    for (auto& cptr : children_) {
        int cw = inner_w;
        int ch = cptr->h() > 0 ? cptr->h() : flex_h;
        cptr->setSize(cw, ch);
        cptr->setPosition(inner_x, cur_y);
        // compute child's children layout recursively
        cptr->computeLayout(cw, ch);
        cur_y += ch + spacing_;
    }
}

} // namespace aff::sdl_utils::common
