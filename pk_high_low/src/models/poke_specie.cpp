#include <models/poke_specie.hpp>

#include <iostream>

namespace aff::pk_high_low::models {

poke_specie::poke_stat_t poke_specie::bst() const noexcept {
    return hp + atk + def + sp_atk + sp_def + speed;
}

bool poke_specie::is_alt_form() const noexcept {
    return alt_name != "";
}

poke_specie::num_id_t poke_specie::num_id() const noexcept {
    if (is_alt_form()) {
        auto pos = id.find('_');
        if (pos != std::string::npos) {
            return static_cast<num_id_t>(std::stoi(id.substr(0, pos)));
        }
    } 
    return static_cast<num_id_t>(std::stoi(id));
}

poke_specie poke_specie::from_json(const nlohmann::json& j) {
    poke_specie specie;

    try {
        auto types = j.value("types", std::vector<poke_specie::poke_type_t>{ "None" });

        specie.id       = j.at("id").get<poke_specie::id_t>();
        specie.name     = j.at("name").get<std::string>();
        specie.alt_name = j.value("alt_name", "");
        specie.type_1   = types.size() > 0 ? types[0] : "None";
        specie.type_2   = types.size() > 1 ? types[1] : "None";
        specie.hp       = j.at("stats").at("hp").get<poke_specie::poke_stat_t>();
        specie.atk      = j.at("stats").at("attack").get<poke_specie::poke_stat_t>();
        specie.def      = j.at("stats").at("defense").get<poke_specie::poke_stat_t>();
        specie.sp_atk   = j.at("stats").at("sp.atk").get<poke_specie::poke_stat_t>();
        specie.sp_def   = j.at("stats").at("sp.def").get<poke_specie::poke_stat_t>();
        specie.speed    = j.at("stats").at("speed").get<poke_specie::poke_stat_t>();
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "Error parsing specie JSON: " << e.what() << "\n";
        throw;
    }

    return specie;
}



}  // namespace aff::pk_high_low::models