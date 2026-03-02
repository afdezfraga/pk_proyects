#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_HLGAME_CONTROLLER_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_HLGAME_CONTROLLER_HPP

#include <array>
#include <filesystem>
#include <functional>
#include <optional>
#include <utility>
#include <string>
#include <random>
#include <utility>

#include <common_ui/window.hpp>
#include <common_ui/font.hpp>
#include <controller/controller_api.hpp>
#include <controller/game_mode.hpp>

#include <controller/game_concepts.hpp>
#include <views/view_concepts.hpp>
#include <models/model_concepts.hpp>
#include <controller/game_choice.hpp>

#include <models/pokedex.hpp>
#include <models/poke_specie.hpp>

#include <models/game_model.hpp>
#include <views/hl_game_view.hpp>

namespace aff::pk_high_low::controller {

template <models::Model M, views::SDLGameView<M> V>
class sdl_hl_game_controller {
private:
    M model_;
    V view_;
    HLGameContext ctx_;
    // Init api_.request with a lambda { [&](HLGameAction a){ pending_action_ = a; }
    HLGameAPI api_ { .request = [](HLGameContext& ctx, HLGameAction a){ ctx.still_pending_action = a; } };

    static std::string find_font_hl_game(const std::filesystem::path& assets_path) {
        std::vector<std::filesystem::path> candidates {
          assets_path / "fonts" / "Roboto-Black.ttf",
          assets_path / "fonts" / "DejaVuSans.ttf",
          "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        };
        for (const auto &p : candidates) if (std::filesystem::exists(p)) return p.string();
        return std::string();
    }


public:
    using choice_t = game_choice;

    sdl_hl_game_controller(M&& model, V&& view, aff::sdl_utils::common::Window* window, const std::filesystem::path& assets_path)
        : model_(std::forward<M>(model)), 
          view_(std::forward<V>(view)),
          ctx_{.window = window, 
               .assets_path = assets_path, 
               .font = aff::sdl_utils::common::Font(find_font_hl_game(assets_path), 24)}
    {
        // Init model on first round
        model_.update_for_new_round();
        ctx_.game_state = HLGameState::SHOWING_CURRENT_ROUND;
    }

    void run() { /* Do not really care, just for compatibility */ }

    void quit() { /* Do not really care, just for compatibility */ }

    void tick(const SDL_Event* ev, AppContext& ctx, AppAPI& api) {
        view_.tick(ev, ctx_, api_, model_);

        if (ctx_.still_pending_action.has_value()) {
            switch (ctx_.still_pending_action.value()) {
                case HLGameAction::MAKE_CHOICE: {
                    // Process the player's choice and update the model accordingly
                    bool is_correct = model_.process_player_choice(ctx_.last_player_choice);

                    // Determine if the game should continue
                    if (model_.is_game_over()) {
                        ctx_.game_state = HLGameState::SHOWING_LOST_ROUND;
                        ctx.last_game_score = model_.getScore();
                    } else {
                        ctx_.game_state = HLGameState::SHOWING_CURRENT_ROUND;
                        model_.update_for_new_round();
                    }
                    break;
                }
                case HLGameAction::QUIT: {
                    api.request(Action::SHOW_END);
                    break;
                }
                default:
                    break;
            }
            ctx_.still_pending_action.reset();
        }

    }



};

class HLGameController {
public:
    HLGameController(aff::sdl_utils::common::Window& window, const std::filesystem::path& assets_path);
    void tick(const SDL_Event* ev, AppContext& ctx, AppAPI& api);
    void configure(const game_settings& settings);
    void reset();
    void restart();
private:
    aff::sdl_utils::common::Window* window_;
    std::filesystem::path assets_path_;
    aff::sdl_utils::common::Font font_;

private:
    struct rng_utils_t {
        std::mt19937 rng {};
        std::uniform_int_distribution<std::mt19937::result_type> dist_dex {};
    };
    struct hl_model_utils_t {
        using item_t = std::pair<aff::pk_high_low::models::poke_specie, bool>; // bool indicates if revealed
        using item_score_t = aff::pk_high_low::models::poke_specie::poke_stat_t;
        using dex_t = aff::pk_high_low::models::pokedex;
        dex_t dex_;
        rng_utils_t rng_utils_;
        std::array<dex_t::index_t, 2> previous_indices_ { static_cast<dex_t::index_t>(-1), 
                                                          static_cast<dex_t::index_t>(-1) };
        uint8_t replace_index_ { 0 };

        std::function<item_score_t(const item_t&)> get_bst_;
        std::function<dex_t::index_t(dex_t::index_t, dex_t::index_t)> next_different_index_;
        std::function<void(std::pair<item_t, item_t>&)> item_update_;
        std::function<bool(const std::pair<item_score_t, item_score_t>&, const aff::pk_high_low::controller::game_choice&)> auto_win_ties_;
        using model_t = aff::pk_high_low::models::game_model<item_t, item_score_t, 
                                                            decltype(get_bst_), 
                                                            decltype(item_update_), 
                                                            decltype(auto_win_ties_)>;
        using view_t = aff::pk_high_low::views::hl_game_view<model_t>;
        using controller_t = sdl_hl_game_controller<model_t, view_t>;
    };
    hl_model_utils_t model_utils_;
    std::optional<hl_model_utils_t::controller_t> hl_controller_ { std::nullopt };
    
};

} // namespace

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_HLGAME_CONTROLLER_HPP
