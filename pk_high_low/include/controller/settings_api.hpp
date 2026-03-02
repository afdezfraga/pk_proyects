#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_SETTINGS_API_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_SETTINGS_API_HPP

#include <functional>
#include <filesystem>
#include <optional>

#include <controller/game_mode.hpp>

#include <common_ui/window.hpp>
#include <common_ui/font.hpp>

namespace aff::pk_high_low::controller {

enum class SettingsAction {
    NONE = 0,
    UPDATE_SETTINGS,
    START_GAME,
    QUIT
};

// Context provided to controllers each tick.
struct SettingsContext {
    aff::sdl_utils::common::Window* window = nullptr;
    std::filesystem::path assets_path;
    aff::sdl_utils::common::Font font;
    game_settings settings { };
    std::optional<SettingsAction> pending_action { std::nullopt };  
};

// Lightweight API controllers use to request app-level actions.
struct SettingsAPI {
    std::function<void(SettingsContext&, SettingsAction)> request;
};

} // namespace aff::pk_high_low::controller

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_SETTINGS_API_HPP