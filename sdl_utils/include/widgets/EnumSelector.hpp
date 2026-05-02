#ifndef AFF_SDL_UTILS_WIDGETS_ENUMSELECTOR_HPP
#define AFF_SDL_UTILS_WIDGETS_ENUMSELECTOR_HPP

#include <common_ui/Widget.hpp>
#include <common_ui/font.hpp>
#include <common_ui/text.hpp>
#include <common_ui/Theme.hpp>
#include <common_ui/texture.hpp>
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <functional>
#include <optional>

namespace aff::sdl_utils::widgets {

class EnumSelector : public aff::sdl_utils::common::Widget {
public:
    EnumSelector() = default;
    ~EnumSelector() = default;

    void setOptions(const std::vector<std::string>& opts);
    void setValue(size_t idx);
    size_t value() const { return value_idx_; }
    void setCallback(std::function<void(size_t)> cb) { cb_ = std::move(cb); }
    void setFont(const aff::sdl_utils::common::Font* f) { font_ = f; invalidateTextures(); }

    // Widget overrides
    void computeLayout(int parent_w, int parent_h) override;
    void render(SDL_Renderer* ren) override;
    void update(float dt) override {}
    bool handleEvent(const SDL_Event& ev) override;

private:
    void invalidateTextures();

    std::vector<std::string> options_;
    size_t value_idx_ {0};
    const aff::sdl_utils::common::Font* font_ {nullptr};
    aff::sdl_utils::common::Texture label_tex_;
    aff::sdl_utils::common::Texture left_tex_;
    aff::sdl_utils::common::Texture right_tex_;
    SDL_Rect left_rect_ {0,0,0,0};
    SDL_Rect label_rect_ {0,0,0,0};
    SDL_Rect right_rect_ {0,0,0,0};
    std::function<void(size_t)> cb_;
};

} // namespace aff::sdl_utils::widgets

#endif // AFF_SDL_UTILS_WIDGETS_ENUMSELECTOR_HPP
