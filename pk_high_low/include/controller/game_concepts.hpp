#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_CONCEPTS_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_CONCEPTS_HPP

#include <concepts>

#include <models/model_concepts.hpp>
#include <views/view_concepts.hpp>

#include <controller/controller_api.hpp>
#include <controller/game_mode.hpp>

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

// SDL-specific game controller concept 
// (for future use if we want to have multiple SDL-based controllers)
// Satisfies the GameController concept 
// and adds a tick method that takes SDL events 
// and an API/context for app-level interactions.
template <typename C, typename M, typename V>
concept SDLGameController =
    GameController<C, M, V> &&
    requires(C& c, const SDL_Event* ev, AppContext& ctx, AppAPI& api) {
        { c.tick(ev, ctx, api) } -> std::same_as<void>;
    };

template <typename C>
concept SDLGameControllerAgregator = 
    requires(C& c, const SDL_Event* ev, AppContext& ctx, AppAPI& api) {
        { c.tick(ev, ctx, api) } -> std::same_as<void>;
        { c.configure(std::declval<game_settings>()) } -> std::same_as<void>;
        { c.reset() } -> std::same_as<void>;
        { c.restart() } -> std::same_as<void>;
    };

}

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_CONCEPTS_HPP