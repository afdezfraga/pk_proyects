#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_VIEW_CONCEPTS_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_VIEW_CONCEPTS_HPP

#include <concepts>
#include <string>
#include <utility>

#include <models/model_concepts.hpp>
#include <controller/game_choice.hpp>

// --- Concepts ---

namespace aff::pk_high_low::views {

template <typename V, typename M>
concept View =
    aff::pk_high_low::models::Model<M> &&
    requires(V& v, const std::string& s, const M& m) {
        { v.show(s) } -> std::same_as<void>;
        { v.prompt(s) } -> std::convertible_to<std::string>;
        { v.show_round(m) } -> std::same_as<void>;
        { v.get_player_choice(m) } -> std::convertible_to<controller::game_choice>;
        { v.show_round_results(m, std::declval<bool>()) } -> std::same_as<void>;
    };

}  // namespace aff::pk_high_low::views

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_VIEW_CONCEPTS_HPP