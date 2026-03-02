#include "controller/settings_controller.hpp"

#include <SDL.h>
#include <common_ui/text.hpp>

using namespace aff::sdl_utils::common;
using namespace aff::pk_high_low::controller;

static std::string find_font(const std::filesystem::path& assets_path) {
    std::vector<std::filesystem::path> candidates {
        assets_path / "fonts" / "Roboto-Black.ttf",
        assets_path / "fonts" / "DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    };
    for (const auto &p : candidates) if (std::filesystem::exists(p)) return p.string();
    return std::string();
}

SettingsController::SettingsController(Window& window, const std::filesystem::path& assets_path)
    : window_(&window), 
      assets_path_(assets_path),
      settings_(),
      ctx_ {
        .window = window_,
        .assets_path = assets_path_,
        .font = Font(find_font(assets_path), 24),
        .settings = settings_,
      }
{}

void SettingsController::reset()
{
    settings_ = {};
}

void SettingsController::tick(const SDL_Event* ev, AppContext& ctx, AppAPI& api)
{
    // Render the config view
    config_view_.tick(ev, ctx_, settings_api_);

    // Process input event (if any)
    if (ctx_.pending_action.has_value()) {
        switch (ctx_.pending_action.value()) {
            case SettingsAction::NONE:
                break;
            case SettingsAction::UPDATE_SETTINGS:
                // Just update the context, the view will read from it on the next tick
                settings_ = ctx_.settings;
                break;
            case SettingsAction::START_GAME:
                api.request(Action::START_GAME);
                break;
            case SettingsAction::QUIT:
                api.request(Action::QUIT);
                break;
        }
        ctx_.pending_action.reset();
    }
}
