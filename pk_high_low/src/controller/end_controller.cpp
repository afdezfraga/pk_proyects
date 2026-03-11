#include "controller/end_controller.hpp"

#include <SDL.h>
#include <common_ui/text.hpp>

using namespace aff::sdl_utils::common;
using namespace aff::pk_high_low::controller;

static std::string find_font_end(const std::filesystem::path& assets_path) {
    std::vector<std::filesystem::path> candidates {
        assets_path / "fonts" / "Roboto-Black.ttf",
        assets_path / "fonts" / "DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    };
    for (const auto &p : candidates) if (std::filesystem::exists(p)) return p.string();
    return std::string();
}

EndController::EndController(Window& window, const std::filesystem::path& assets_path)
    : window_(&window), assets_path_(assets_path), font_((find_font_end(assets_path_).empty() ? std::string() : find_font_end(assets_path_)), 24)
{
    view_ = std::make_unique<aff::pk_high_low::views::EndView>(window, assets_path_);
}

void EndController::reset()
{
}

void EndController::tick(const SDL_Event* ev, AppContext& ctx, AppAPI& api)
{
    if (view_) {
        view_->tick(ev, ctx, api);
    }
}
