#ifndef AFF_PK_PROJECTS_SDL_UTILS_WIDGETS_BUTTON_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_WIDGETS_BUTTON_HPP

#include <SDL.h>
#include <functional>
#include <string>
#include <common_ui/Widget.hpp>
#include <common_ui/font.hpp>

namespace aff::sdl_utils::widgets {

class Button : public aff::sdl_utils::common::Widget {
public:
    Button() = default;
    ~Button() override = default;

    void setText(const std::string& t) { text_ = t; }
    void setCallback(std::function<void()> cb) { cb_ = std::move(cb); }
    void setFont(const aff::sdl_utils::common::Font* f) { font_ = f; }

    void render(SDL_Renderer* renderer) override;
    bool handleEvent(const SDL_Event& ev) override;

private:
    std::string text_;
    bool hover_ = false;
    bool pressed_ = false;
    std::function<void()> cb_;
    const aff::sdl_utils::common::Font* font_ = nullptr;
};

} // namespace aff::sdl_utils::widgets

#endif // AFF_PK_PROJECTS_SDL_UTILS_WIDGETS_BUTTON_HPP
