#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_CONSOLE_VIEW_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_CONSOLE_VIEW_HPP

#include <views/view_concepts.hpp>
#include <models/model_concepts.hpp>
#include <controller/game_choice.hpp>

#include <iostream>
#include <string>

namespace aff::pk_high_low::views {

    template <aff::pk_high_low::models::Model M, typename DisplayFunc>
    class console_view {
    public:
        using item_display_f = DisplayFunc;
        using choice_t = aff::pk_high_low::controller::game_choice;

        explicit console_view(item_display_f&& display_func)
            : item_display_function_(std::forward<item_display_f>(display_func)) {}

        void show(const std::string& message) {
            std::cout << message << std::endl;
        }

        std::string prompt(const std::string& message) {
            std::cout << message;
            std::string input;
            std::getline(std::cin, input);
            return input;
        }

        void show_round(const M& model) {
            using std::cout;
            cout << "\n--- Round " << model.getRound() << " ---\n";
            display_items(model);
        }

        auto get_player_choice(const M& model) -> choice_t {
            using std::cout;
            using std::cin;
            using std::string;

            string user_input;
            bool still_no_input = true;
            choice_t result {choice_t::QUIT_GAME};
            while (still_no_input) {
            
                // Get user choice
                cout << "Which Pokemon has higher total base stats? Enter '1', '2', 'T' for Tie, or 'q' to quit: ";
                cin >> user_input;

                if (user_input == "q") {
                    result = choice_t::QUIT_GAME;
                    return result;
                }

                if (user_input != "1" && user_input != "2" && user_input != "T") {
                    cout << "Invalid input. Please enter '1', '2', 'T' for Tie or 'q' to quit.\n";
                    continue;
                }
                still_no_input = false;

                // Parse user choice
                if (user_input == "1") result = choice_t::FIRST_CHOSEN;
                if (user_input == "2") result = choice_t::SECOND_CHOSEN;
                if (user_input == "T") result = choice_t::TIE_CHOSEN;
                
            }
            return result;
        }

        void show_round_results(const M& model, bool is_correct) {
            using std::cout;
            cout << "\nRound Results:\n";
            display_items(model, true);
            if (is_correct) {
                cout << "Correct! Your current score is: " << model.getScore() << "\n";
            } else {
                cout << "Wrong choice! Game Over. Your final score is: " << model.getScore() << "\n";
            }
        }



    private:
        item_display_f item_display_function_;

        void display_items(const M& model, bool force_reveal = false) {
            const auto items = model.items();
            const auto scores = model.itemScores();
            std::cout << "1: " << item_display_function_(items.first, scores.first, force_reveal) << "\n";
            std::cout << "2: " << item_display_function_(items.second, scores.second, force_reveal) << "\n";
        }

    };

}  // namespace aff::pk_high_low::views

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_CONSOLE_VIEW_HPP