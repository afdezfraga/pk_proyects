#include "models/model_factory.hpp"
#include <stdexcept>

namespace aff::pk_high_low::models {

std::unique_ptr<ModelInterface> make_model(const aff::pk_high_low::controller::game_settings& /*settings*/, const pokedex& /*dex*/) {
    // Placeholder implementation: real factory will construct concrete game_model instances
    // For now return nullptr to indicate not implemented.
    return nullptr;
}

} // namespace aff::pk_high_low::models
