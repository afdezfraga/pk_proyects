#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_HL_GAME_API_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_HL_GAME_API_HPP

#include <functional>
#include <filesystem>
#include <optional>

#include <models/model_concepts.hpp>
#include <controller/game_choice.hpp>

#include <common_ui/window.hpp>
#include <common_ui/font.hpp>

namespace aff::pk_high_low::controller {

enum class HLGameAction {
    NONE = 0,
    MAKE_CHOICE,
    QUIT
};

enum class HLGameState {
    NOT_STARTED = 0,
    SHOWING_CURRENT_ROUND,
    SHOWING_LOST_ROUND
};

// Context provided to controllers each tick.
struct HLGameContext {
    aff::sdl_utils::common::Window* window = nullptr;
    std::filesystem::path assets_path;
    aff::sdl_utils::common::Font font;
    HLGameState game_state { HLGameState::NOT_STARTED };
    std::optional<HLGameAction> still_pending_action { std::nullopt };
    game_choice last_player_choice { game_choice::QUIT_GAME };    
};

// Lightweight API controllers use to request app-level actions.
struct HLGameAPI {
    std::function<void(HLGameContext&, HLGameAction)> request;
};

} // namespace aff::pk_high_low::controller

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_HL_GAME_API_HPP