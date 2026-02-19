#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_CONTROLLER_API_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_CONTROLLER_API_HPP

#include <functional>
#include <filesystem>

#include <common_ui/window.hpp>

namespace aff::pk_high_low::controller {

enum class Action {
    NONE = 0,
    START_GAME,
    SHOW_END,
    RESTART,
    BACK_TO_SETTINGS,
    QUIT
};

// Lightweight API controllers use to request app-level actions.
struct AppAPI {
    std::function<void(Action)> request;
};

// Context provided to controllers each tick.
struct AppContext {
    aff::sdl_utils::common::Window* window = nullptr;
    std::filesystem::path assets_path;
};

} // namespace aff::pk_high_low::controller

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_CONTROLLER_API_HPP
