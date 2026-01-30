#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_MODEL_CONCEPTS_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_MODEL_CONCEPTS_HPP

#include <concepts>
#include <string>

#include <controller/game_choice.hpp>

// --- Concepts ---

namespace aff::pk_high_low::models {

template <typename M>
concept Model =
    requires {
        typename M::entity_type;
        typename M::entity_score_type;
    } &&
    requires(M& m, const M& cm, long long score, long long round) {
        { cm.items() } -> std::convertible_to<std::pair<typename M::entity_type, typename M::entity_type>>;
        { cm.itemScores() } -> std::convertible_to<std::pair<typename M::entity_score_type, typename M::entity_score_type>>;
        { cm.getScore() } -> std::convertible_to<long long>;
        { cm.getRound() } -> std::convertible_to<long long>;
        { m.is_game_over() } -> std::convertible_to<bool>;
        { m.setScore(score) } -> std::same_as<void>;
        { m.setRound(round) } -> std::same_as<void>;
        // Behaviors
        { m.update_for_new_round() } -> std::same_as<void>;
        { m.process_player_choice(std::declval<controller::game_choice>()) } -> std::convertible_to<bool>;

    };

}  // namespace aff::pk_high_low::models

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_MODEL_CONCEPTS_HPP
