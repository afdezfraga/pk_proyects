#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_APP_CONTROLLER_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_APP_CONTROLLER_HPP

#include <filesystem>
#include <optional>

#include <common_ui/window.hpp>
#include <controller/game_mode.hpp>
#include <controller/controller_api.hpp>

#include <controller/hl_game_controller.hpp>

// Forward declare controllers
namespace aff::pk_high_low::controller {
    class SettingsController;
    class GameStateController;
    class EndController;
}

namespace aff::pk_high_low::controller {

class AppController {
public: 
    // using GameControllerT = GameStateController; // alias for easy swapping of game controller implementations
    using GameControllerT = HLGameController;
private:
    enum class Screen { SETTINGS, GAME, END };
    struct MainLoopData {
        sdl_utils::common::Window* window;
        SettingsController* settings;
        GameControllerT* game;
        EndController* end;
        AppContext* ctx;
        AppAPI* api;
        std::optional<Action>* pending;
        Screen* cur;
        bool* quit;
    };

private:
    static void process_pending(MainLoopData* d);

public:
    // Construct with an existing SDL window (caller is responsible for SDL init)
    AppController(sdl_utils::common::Window& window, const std::filesystem::path& assets_path);

    // Run the application - returns process exit code
    int run();
    // Simple dummy runner that shows a teal screen until any key or window close
    // int run_dummy();

private:
    std::filesystem::path assets_path_;
    sdl_utils::common::Window* window_ = nullptr; // optional reference to externally-owned window
    // Note: controllers are constructed at runtime inside run() so they share the window lifetime.
};

} // namespace aff::pk_high_low::controller

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_APP_CONTROLLER_HPP