#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_MODEL_FACTORY_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_MODEL_FACTORY_HPP

#include <memory>
#include <controller/game_mode.hpp>
#include <controller/game_choice.hpp>
#include <models/pokedex.hpp>

namespace aff::pk_high_low::models {

// Minimal runtime-polymorphic model interface used by higher-level app glue.
struct ModelInterface {
    virtual ~ModelInterface() = default;
    virtual void update_for_new_round() = 0;
    virtual bool process_player_choice(const aff::pk_high_low::controller::game_choice&) = 0;
    virtual bool is_game_over() const = 0;
    virtual uint64_t getScore() const = 0;
    virtual void reset() = 0;
};

// Factory that will construct a configured model based on settings and a pokedex.
std::unique_ptr<ModelInterface> make_model(const aff::pk_high_low::controller::game_settings& settings, const pokedex& dex);

} // namespace aff::pk_high_low::models

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_MODEL_FACTORY_HPP