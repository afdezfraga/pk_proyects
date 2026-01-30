#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_CONTROLLER_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_CONTROLLER_HPP

#include <controller/game_concepts.hpp>
#include <controller/game_choice.hpp>

// --- Game Controller ---

namespace aff::pk_high_low::controller {

template <models::Model M, views::View<M> V>
class high_low_game {

public:
    using choice_t = game_choice;
    high_low_game(M&& model, V&& view)
        : model_(std::forward<M>(model)), view_(std::forward<V>(view)) {}

    void run() {
        running_ = true;
        choice_t current_choice;

        while (running_) {
            // Game loop implementation
            current_choice = play_new_round();

            if (current_choice == choice_t::QUIT_GAME) {
                running_ = false;
            } else {
                // Process the round result
                running_ = process_round_result(current_choice);
            }

        }
    }

    auto play_new_round() -> choice_t {
      // Update the model for a new round
      model_.update_for_new_round();

      // Show the round via the view and get player choice
      view_.show_round(model_);

      // Get player choice via the view
      return view_.get_player_choice(model_);
    }

    auto process_round_result(const choice_t& choice) -> bool {
        // Process the player's choice and update the model accordingly
        bool is_correct = model_.process_player_choice(choice);

        // Show round results via the view
        view_.show_round_results(model_, is_correct);

        // Determine if the game should continue
        return !model_.is_game_over();
    }

private:
    M model_;
    V view_;
    bool running_ { false };
};

} // namespace aff::pk_high_low::controller

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_CONTROLLER_HPP