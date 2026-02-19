#pragma once

#include <filesystem>
#include <controller/game_mode.hpp>
#include <common_ui/window.hpp>

namespace aff::pk_high_low::views {

class sdl_config_view {
public:
    sdl_config_view(sdl_utils::common::Window& window, const std::filesystem::path& assets_path);

    // Blocking UI for native builds. Returns selected settings.
    aff::pk_high_low::controller::game_settings run_blocking();

    // Non-blocking update for Emscripten builds (optional)
    bool update();
    aff::pk_high_low::controller::game_settings get_result() const;

private:
    sdl_utils::common::Window* window_;
    std::filesystem::path assets_path_;
    aff::pk_high_low::controller::game_settings result_;
};

} // namespace aff::pk_high_low::views
