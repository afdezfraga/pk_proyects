#include <iostream>
#include <filesystem>

#include <models/poke_specie.hpp>
#include <models/pokedex.hpp>


int main() {
    using namespace aff::pk_high_low::models;
    using std::filesystem::path;
    using std::vector;

    // Dex file path
    const path assets_path { std::filesystem::absolute(__FILE__).parent_path().parent_path() / "assets"};
    const path dex_path  { assets_path / "dex.json" };

    // Parse each entry
    const pokedex dex { pokedex::from_file(dex_path) };

    std::cout << "Pokedex loaded with " << dex.size() << " species.\n";

    for (int i = 0; i < 5; ++i) {
        const poke_specie& specie = dex.at(i);
        std::cout << "Specie " << i << ": (" << specie.id << ") " << specie.name 
                    << (specie.is_alt_form() ? 
                            std::string(" (Alt Form: " + specie.alt_name + ")") : 
                            "") 
                    << " (Type 1: " << specie.type_1 
                    << ", Type 2: " << specie.type_2 << ")"
                    << " (HP: " << specie.hp 
                    << ", Atk: " << specie.atk
                    << ", Def: " << specie.def
                    << ", Sp.Atk: " << specie.sp_atk
                    << ", Sp.Def: " << specie.sp_def
                    << ", Speed: " << specie.speed << ")\n";
    }

    return EXIT_SUCCESS;
}