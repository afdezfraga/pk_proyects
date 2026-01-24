#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_POKE_SPECIE_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_POKE_SPECIE_HPP

#include <string>

#include <nlohmann/json.hpp>

namespace aff::pk_high_low::models {


/*************************************************************************//**
 * @brief Lightweight POD describing a Pokémon species.
 *
 * @details
 * This structure holds identification, naming, typing and base-stat information
 * for a single Pokémon species used throughout the model layer. It is a
 * simple aggregate type that is cheap to copy and store in containers.
 *
 * @section typedefs Typedefs
 * - @c id_t        : external unique identifier (std::string)
 * - @c num_id_t    : numeric identifier (int32_t)
 * - @c poke_type_t : textual type name
 * - @c poke_stat_t : integer type used for stats
 *
 * @section members Members
 * - @var id_t poke_specie::id
 *   Unique identifier for the species.
 * - @var std::string poke_specie::name
 *   Primary display name.
 * - @var std::string poke_specie::alt_name
 *   Optional alternate form name.
 * - @var poke_type_t poke_specie::type_1
 *   Primary type ("None" if absent).
 * - @var poke_type_t poke_specie::type_2
 *   Secondary type ("None" if absent).
 * - @var poke_stat_t poke_specie::hp, atk, def, sp_atk, sp_def, speed
 *   Individual base stats.
 *
 * @section functions Member functions
 * - @fn poke_stat_t poke_specie::bts() const noexcept
 *   Returns the base total stats (sum of the individual stat fields).
 * - @fn bool poke_specie::is_alt_form() const noexcept
 *   Returns true when this entry represents an alternate form.
 * - @fn num_id_t poke_specie::num_id() const noexcept
 *   Returns the numeric identifier for indexing/sorting.
 *
 ****************************************************************************/
struct poke_specie {

    using id_t = std::string;
    using num_id_t = int32_t;
    using poke_type_t = std::string;
    using poke_stat_t = int32_t;
    
    id_t id {};
    std::string name {};
    std::string alt_name { "" };
    poke_type_t type_1 { "None" };
    poke_type_t type_2 { "None" };
    poke_stat_t hp {};
    poke_stat_t atk {};
    poke_stat_t def {};
    poke_stat_t sp_atk {};
    poke_stat_t sp_def {};
    poke_stat_t speed {};


    poke_stat_t bts() const noexcept;
    bool is_alt_form() const noexcept;
    num_id_t num_id() const noexcept;

    static poke_specie from_json(const nlohmann::json& j);
};

}  // namespace aff::pk_high_low::models

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_POKE_SPECIE_HPP