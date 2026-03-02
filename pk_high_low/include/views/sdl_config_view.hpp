#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_SDL_CONFIG_VIEW_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_SDL_CONFIG_VIEW_HPP

#include <filesystem>
#include <controller/game_mode.hpp>
#include <common_ui/window.hpp>
#include <controller/settings_api.hpp>

namespace aff::pk_high_low::views {

class sdl_config_view {

private:
    int field_idx_ {0}; // 0: mode, 1: pokedex, 2: difficulty, 3: complete_mode

public:
    // Rule of 0

    // Main tick for event handling and rendering
    void tick(const SDL_Event* ev, 
              aff::pk_high_low::controller::SettingsContext& ctx, 
              aff::pk_high_low::controller::SettingsAPI& api);
};

} // namespace aff::pk_high_low::views

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_SDL_CONFIG_VIEW_HPP
