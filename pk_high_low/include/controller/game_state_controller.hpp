#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_STATE_CONTROLLER_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_STATE_CONTROLLER_HPP

#include <filesystem>
#include <string>
#include <common_ui/window.hpp>
#include <common_ui/font.hpp>
#include <controller/controller_api.hpp>
#include <controller/game_mode.hpp>

namespace aff::pk_high_low::controller {

class GameStateController {
public:
    GameStateController(aff::sdl_utils::common::Window& window, const std::filesystem::path& assets_path);
    void tick(const SDL_Event* ev, AppContext& ctx, AppAPI& api);
    void configure(const game_settings& settings);
    void reset();
    void restart();
private:
    aff::sdl_utils::common::Window* window_;
    std::filesystem::path assets_path_;
    aff::sdl_utils::common::Font font_;
    std::string mode_text_ {"Loading..."};
    std::string pokedex_text_ {"Loading..."};
    std::string difficulty_text_ {"Loading..."};
    std::string complete_mode_text_ {"Loading..."};
};

} // namespace

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_STATE_CONTROLLER_HPP
