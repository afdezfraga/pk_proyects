#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_GAME_MODEL_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_GAME_MODEL_HPP

#include <cstdint>
#include <utility>

#include <controller/game_choice.hpp>

namespace aff::pk_high_low::models {

// A class that takes a lambda to calculate the score of a Pokemon
template <
typename Item, 
typename ItemScore,
typename ScoreFunc, 
typename ItemUpdateFunc, 
typename ChoiceFunc
>
class game_model{
public:
    using item_score_f = ScoreFunc;
    using item_update_f = ItemUpdateFunc;
    using is_choice_correct_f = ChoiceFunc;
    using score_t = int64_t;
    using round_t = int64_t;
    using choice_t = aff::pk_high_low::controller::game_choice;
    using entity_type = Item;
    using item_t = entity_type;
    using items_t = std::pair<item_t, item_t>;
    using entity_score_type = ItemScore;
    using item_score_t = entity_score_type;
    using item_scores_t = std::pair<item_score_t, item_score_t>;

    explicit game_model(item_score_f&& score_func, 
                        item_update_f&& update_func,  
                        is_choice_correct_f&& choice_func)
        : item_score_function_(std::forward<item_score_f>(score_func)),
          item_update_function_(std::forward<item_update_f>(update_func)),
          is_choice_correct_function_(std::forward<is_choice_correct_f>(choice_func)) {}

    auto update_for_new_round() -> void {
        // Any round
        current_round_++;
        // Update items
        item_update_function_(current_items_);

        // If its the first round, we need to update both items
        if (current_round_ == 1) item_update_function_(current_items_);
    }

    auto process_player_choice(const choice_t& choice) -> bool {
        bool is_correct = is_choice_correct_function_(itemScores(), choice);
        if (is_correct) {
            current_score_++;
        } else {
            game_over_ = true;
        }
        return is_correct;
    }

    // Getters
    auto getScore() const -> score_t { return current_score_; }
    auto getRound() const -> round_t { return current_round_; }
    auto items() const -> items_t { return current_items_; }
    auto is_game_over() const -> bool { return game_over_; }
    auto itemScores() const -> item_scores_t {
        return item_scores_t { item_score_function_(current_items_.first), 
                               item_score_function_(current_items_.second) };
    }

    // Setters
    auto setScore(score_t score) -> void { current_score_ = score; }
    auto setRound(round_t round) -> void { current_round_ = round; }

private:
    item_score_f item_score_function_;
    item_update_f item_update_function_;
    is_choice_correct_f is_choice_correct_function_;
    score_t current_score_ {0};
    round_t current_round_ {0};
    items_t current_items_ { item_t{}, item_t{} };
    bool game_over_ { false };

};

}  // namespace aff::pk_high_low::models

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_GAME_MODEL_HPP