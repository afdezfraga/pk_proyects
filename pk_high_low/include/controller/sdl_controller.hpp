#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_SDL_CONTROLLER_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_SDL_CONTROLLER_HPP

#include <controller/game_concepts.hpp>
#include <controller/game_choice.hpp>
#include <string>
#include <concepts>

namespace aff::pk_high_low::controller {

// A simple SDL-aware controller with the same interface as high_low_game.
template <models::Model M, views::View<M> V>
class sdl_controller {
public:
    using choice_t = game_choice;

    sdl_controller(M&& model, V&& view)
        : model_(std::forward<M>(model)), view_(std::forward<V>(view)) {}

    void run() {
        bool replay = true;
        do {
            running_ = true;
            while (running_) {
                model_.update_for_new_round();
                view_.show_round(model_);
                choice_t choice = view_.get_player_choice(model_);
                if (choice == choice_t::QUIT_GAME) { running_ = false; replay = false; break; }
                bool cont = model_.process_player_choice(choice);
                view_.show_round_results(model_, cont);
                // If the player was wrong, show points and wait for any input before proceeding to end-screen
                if (!cont) {
                    try {
                        if constexpr (requires { view_.wait_for_any_input(); }) {
                            view_.wait_for_any_input();
                        }
                    } catch (...) { /* swallow */ }
                }
                running_ = !model_.is_game_over();
            }

            if (replay){
                // When the game ends, show a final message and ask whether to exit or replay.
                try {
                    std::string msg = model_.is_game_over() 
                                        ? std::string("Game Over. Final score: ") 
                                            + std::to_string(model_.getScore())
                                        : std::string("Thanks for playing. Final score: ") 
                                            + std::to_string(model_.getScore());

                    if constexpr (requires { { view_.wait_for_exit(msg) } -> std::convertible_to<bool>; }) {
                        // view returns a boolean: true => replay, false => exit
                        replay = view_.wait_for_exit(msg);
                        if (replay) {
                            model_.reset();
                        }
                    }
                } catch (...) {
                    // swallow exceptions during shutdown to avoid crashes
                    replay = false;
                }
            }

        } while (replay);
    }

    void quit() { running_ = false; }

private:
    M model_;
    V view_;
    bool running_{ false };
};

} // namespace aff::pk_high_low::controller

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_SDL_CONTROLLER_HPP
