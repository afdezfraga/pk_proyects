#include <models/pokedex.hpp>

#include <fstream>

#include <nlohmann/json.hpp>

namespace aff::pk_high_low::models {

pokedex pokedex::from_list(const std::vector<specie_t>& species) {
    pokedex dex;
    for (const auto& specie : species) {
        dex.insert(specie);
    }
    return dex;
}

pokedex pokedex::from_file(const std::filesystem::path& filepath) {

    // Read file into json list
    nlohmann::json j;
    std::ifstream infile(filepath);
    infile >> j;

    std::vector<specie_t> species;
    for (const auto& item : j) {
        specie_t specie = specie_t::from_json(item);
        species.push_back(specie);
    }

    // Parse each entry
    return from_list(species);
}

bool pokedex::insert(const specie_t& specie) {
    auto [it, inserted] = id_map_.emplace(specie.id, storage_.size());
    storage_.push_back(specie);
    return inserted;
}

bool pokedex::erase(const id_t& id) {
    auto it = id_map_.find(id);
    if (it == id_map_.end()) return false;
    storage_.erase(storage_.begin() + it->second);
    id_map_.erase(it);
    return true;
}

bool pokedex::update(const id_t& id, const specie_t& updated) {
    auto it = id_map_.find(id);
    if (it == id_map_.end()) return false;
    storage_[it->second] = updated;
    return true;
}

const std::optional<pokedex::specie_t> pokedex::find_by_id(const id_t& id) const {
    auto it = id_map_.find(id);
    if (it == id_map_.end()) return std::nullopt;
    return storage_[it->second];
}

pokedex::index_t pokedex::size() const {
    return storage_.size();
}

pokedex::specie_t& pokedex::operator[](index_t index) {
    return storage_[index];
}

const pokedex::specie_t& pokedex::operator[](index_t index) const {
    return storage_[index];
}

pokedex::specie_t& pokedex::at(index_t index) {
    return storage_.at(index);
}

const pokedex::specie_t& pokedex::at(index_t index) const {
    return storage_.at(index);
}

} // namespace aff::pk_high_low::models
