#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_POKEDEX_FILTER_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_POKEDEX_FILTER_HPP

#include <filesystem>
#include <vector>
#include <fstream>

#include <nlohmann/json.hpp>

#include <models/poke_specie.hpp>
#include <models/pokedex.hpp>

namespace aff::pk_high_low::models {

class pokedex_filter {
public:
    static pokedex filter_by_list(const pokedex& dex, const std::vector<poke_specie::id_t>& ids) {
        pokedex result;
        for (const auto& id : ids) {
            auto specie_opt = dex.find_by_id(id);
            if (specie_opt) {
                result.insert(*specie_opt);
            }
        }
        return result;
    }

    static pokedex filter_by_file(const pokedex& dex, const std::filesystem::path& filepath) {
      // Read file into json list
      nlohmann::json j;
      std::ifstream infile(filepath);
      infile >> j;

      // j is an object with a key "ids" that is a list of strings
      std::vector<poke_specie::id_t> ids = j["ids"].get<std::vector<poke_specie::id_t>>();
      return filter_by_list(dex, ids);
    }

};

} // namespace aff::pk_high_low::models

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_MODELS_POKEDEX_FILTER_HPP