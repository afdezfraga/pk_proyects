#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_POKEDEX_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_POKEDEX_HPP

#include <models/poke_specie.hpp>

#include <algorithm>
#include <filesystem>
#include <optional>
#include <vector>
#include <unordered_map>


namespace aff::pk_high_low::models {


/**
 * @brief Container for managing a collection of poke_specie objects with efficient CRUD and stat-based queries.
 *
 * @details
 * The pokedex class is optimized for fast retrieval by id and stat-based queries. It supports efficient creation,
 * reading, updating, and deletion of poke_specie objects. The main use case is to create the pokedex once and perform
 * fast lookups and range queries, such as finding all Pokémon with a base total stat (bst) within a given range, or
 * retrieving the N closest Pokémon to a target stat if not enough match the range.
 *
 * @note
 * - Efficient id-based lookup is provided via an unordered_map.
 * - Stat-based queries are optimized via a sorted vector of pointers for fast range and closest-N retrieval.
 * - Insertions and deletions update both structures; extra object creation/modification is possible but not optimal.
 *
 * @section Example
 * @code
 * pokedex dex;
 * dex.insert(specie);
 * auto found = dex.find_by_id("bulbasaur");
 * auto range = dex.find_by_bst_range(500, 30, 10); // bst in [470,530], at least 10 results
 * @endcode
 */
class pokedex {
public:
	using specie_t = poke_specie;
	using id_t = specie_t::id_t;
	using num_id_t = specie_t::num_id_t;
	using stat_t = specie_t::poke_stat_t;

  using index_t = std::size_t;

  /**
   * @brief Create a pokedex from a JSON file.
   * @param filepath Path to the JSON file.
   * @return pokedex object populated from the file.
   */
  static pokedex from_file(const std::filesystem::path& filepath);

  /**
   * @brief Create a pokedex from a list of poke_specie objects.
   * @param species Vector of poke_specie objects.
   * @return pokedex object populated from the list.
   */  
  static pokedex from_list(const std::vector<specie_t>& species);

	/**
	 * @brief Insert a new poke_specie into the pokedex.
	 * @param specie The poke_specie object to insert.
	 * @return true if inserted, false if id already exists.
	 */
	bool insert(const specie_t& specie);

	/**
	 * @brief Remove a poke_specie by id.
	 * @param id The id of the specie to remove.
	 * @return true if removed, false if not found.
	 */
	bool erase(const id_t& id);

	/**
	 * @brief Update an existing poke_specie by id.
	 * @param id The id of the specie to update.
	 * @param updated The updated poke_specie object.
	 * @return true if updated, false if not found.
	 */
	bool update(const id_t& id, const specie_t& updated);

	/**
	 * @brief Retrieve a reference to a poke_specie by id.
	 * @param id The id to look up.
	 * @return Reference to poke_specie if found, throws std::out_of_range otherwise.
	 */
	const std::optional<specie_t> find_by_id(const id_t& id) const;



	/**
	 * @brief Retrieve all poke_specie with a stat in [target-range, target+range].
	 *        If fewer than min_count match, returns the closest min_count by that stat.
	 * @param stat_selector Function or lambda to select the stat from poke_specie (e.g., &poke_specie::speed).
	 * @param target Target stat value.
	 * @param range Range around target.
	 * @param min_count Minimum number of results to return.
	 * @return Vector of references to matching poke_specie.
	 *
	 * @note Example usage:
	 *   find_by_stat([](const poke_specie& p){ return p.speed; }, 120, 10, 5); // speed in [110,130], at least 5 results
	 *   find_by_stat([](const poke_specie& p){ return std::max(p.atk, p.sp_atk); }, 150, 20, 8); // highest atk in [130,170]
	 */
	template<typename StatSelector>
	std::vector<specie_t> find_by_stat(StatSelector stat_selector, stat_t target, stat_t range, size_t min_count) const;

	/**
	 * @brief Retrieve all poke_specie matching a custom condition.
	 * @param predicate Function or lambda taking (const poke_specie&) and returning bool.
	 * @return Vector of references to matching poke_specie.
	 *
	 * @note Example usage:
	 *   find_by_condition([](const poke_specie& p){ return p.speed > 100 && p.atk > 120; });
	 */
	template<typename Predicate>
	std::vector<specie_t> find_by_condition(Predicate predicate) const;

	/**
	 * @brief Get the total number of species in the pokedex.
	 */
	index_t size() const;

  /**
   * @brief Indexed access to poke_specie objects.
   */
  specie_t& operator[](index_t index);

  /**
   * @brief Indexed access to poke_specie objects.
   */
  const specie_t& operator[](index_t index) const;

  /**
   * @brief Indexed access to poke_specie objects with bounds checking.
   */
  specie_t& at(index_t index);

  /**
   * @brief Indexed access to poke_specie objects with bounds checking.
   */
  const specie_t& at(index_t index) const;

private:
  // Storage for poke_specie objects
  std::vector<specie_t> storage_ {};

	// Efficient id lookup - to position in storage
	std::unordered_map<id_t, index_t> id_map_ {};
};

// Template method definitions
// Generic stat-based retrieval
template<typename StatSelector>
std::vector<pokedex::specie_t> pokedex::find_by_stat(StatSelector stat_selector, stat_t target, stat_t range, size_t min_count) const {
    std::vector<specie_t> matches;
    // Sort all species by closeness to target stat
    std::vector<specie_t> all_sorted { storage_ };
    std::sort(all_sorted.begin(), all_sorted.end(),
              [stat_selector, target](const specie_t& a, const specie_t& b) {
                  return std::abs(stat_selector(a) - target) < std::abs(stat_selector(b) - target);
              });
    
    // Get the closest species, ensuring at least min_count are returned
    int missing_species = static_cast<int>(min_count);
    for (auto& specie : all_sorted) {
        stat_t stat_value = stat_selector(specie);
        if (stat_value >= target - range && stat_value <= target + range) { // Within range
            matches.push_back(specie);
            --missing_species;
        } else if (missing_species > 0) { // Not on range but still need more
            matches.push_back(specie);
            --missing_species;
        } else { // Enough species collected
            break;
        }
    }
    return matches;
}

// Predicate-based retrieval
template<typename Predicate>
std::vector<pokedex::specie_t> pokedex::find_by_condition(Predicate predicate) const {
    std::vector<specie_t> result;
    for (auto& specie : storage_) {
        if (predicate(specie)) {
            result.push_back(specie);
        }
    }
    return result;
}

}  // namespace aff::pk_high_low::models

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_POKEDEX_HPP