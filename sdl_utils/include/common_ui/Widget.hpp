#ifndef AFF_PK_PROJECTS_SDL_UTILS_COMMON_WIDGET_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_COMMON_WIDGET_HPP

#include <SDL.h>

namespace aff::sdl_utils::common {

class Widget {
public:
    Widget() = default;
    virtual ~Widget() = default;

    virtual void render(SDL_Renderer* renderer) {}
    virtual void update(float /*dt*/) {}
    virtual bool handleEvent(const SDL_Event& /*ev*/) { return false; }

    void setPosition(int x, int y) { x_ = x; y_ = y; }
    void setSize(int w, int h) { w_ = w; h_ = h; }
    // Responsive helpers: anchor (0..1) and percent sizes (0..1). If percent is 0, absolute size used.
    void setAnchor(float ax, float ay) { anchor_x_ = ax; anchor_y_ = ay; }
    void setSizePercent(float pw, float ph) { pct_w_ = pw; pct_h_ = ph; }
    void setOffset(int ox, int oy) { offset_x_ = ox; offset_y_ = oy; }

    // Compute absolute layout based on parent size. Container will call this before render.
    virtual void computeLayout(int parent_w, int parent_h) {
        // Support percent both as 0..1 and 0..100.0 for convenience.
        if (pct_w_ > 0.0f) {
            float pw = pct_w_ > 1.0f ? (pct_w_ / 100.0f) : pct_w_;
            w_ = static_cast<int>(parent_w * pw);
        }
        if (pct_h_ > 0.0f) {
            float ph = pct_h_ > 1.0f ? (pct_h_ / 100.0f) : pct_h_;
            h_ = static_cast<int>(parent_h * ph);
        }
        // anchor gives the normalized point inside parent where widget's top-left is placed with offset
        int ax = static_cast<int>(anchor_x_ * (parent_w - w_));
        int ay = static_cast<int>(anchor_y_ * (parent_h - h_));
        x_ = ax + offset_x_;
        y_ = ay + offset_y_;
    }
    int x() const { return x_; }
    int y() const { return y_; }
    int w() const { return w_; }
    int h() const { return h_; }

    void setVisible(bool v) { visible_ = v; }
    bool visible() const { return visible_; }

private:
    int x_ = 0, y_ = 0;
    int w_ = 0, h_ = 0;
    bool visible_ = true;
    // responsive fields
    float anchor_x_ = 0.0f, anchor_y_ = 0.0f; // 0..1
    float pct_w_ = 0.0f, pct_h_ = 0.0f; // percent of parent (0 means use absolute)
    int offset_x_ = 0, offset_y_ = 0; // pixel offsets applied after anchor
};

} // namespace aff::sdl_utils::common

#endif // AFF_PK_PROJECTS_SDL_UTILS_COMMON_WIDGET_HPP