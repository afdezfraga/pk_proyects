#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_CONCEPTS_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_CONCEPTS_HPP

#include <concepts>

#include <models/model_concepts.hpp>
#include <views/view_concepts.hpp>

// --- Concepts ---

namespace aff::pk_high_low::controller {

// Game controller concept made from a Model and a View
template <typename C, typename M, typename V>
concept GameController =
    models::Model<M> &&
    views::View<V, M> &&
    requires(C& c, M& m, V& v) {
        { c.run() } -> std::same_as<void>;
        { c.quit() } -> std::same_as<void>;
    };

}

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_CONCEPTS_HPP