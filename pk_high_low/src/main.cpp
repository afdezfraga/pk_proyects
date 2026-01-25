#include <array>
#include <iostream>
#include <filesystem>
#include <utility>
#include <random>

#include <models/poke_specie.hpp>
#include <models/pokedex.hpp>

void play_high_low(const aff::pk_high_low::models::pokedex& dex);

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

    play_high_low(dex);
    return EXIT_SUCCESS;
}

void play_high_low(const aff::pk_high_low::models::pokedex& dex) {
    using namespace aff::pk_high_low::models;
    using std::cout;
    using std::cin;
    using std::string;
    using std::pair;
    using std::array;

    struct replace_model_t { size_t replace_index; pokedex::index_t keep_index; };
    struct game_choice_t { size_t chosen_index; bool is_correct; };
    // Concreting things
    using item_t = pair<poke_specie, bool>; // bool indicates if revealed
    using score_t = poke_specie::poke_stat_t;
    using repo_t = pokedex;
    using replace_model_t = replace_model_t;
    using choice_t = game_choice_t;
    struct big_model_t { size_t score {0}; size_t round {1}; array<item_t, 2> items; };
    using model_t = big_model_t;

    // Extra utils
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist_dex(0, dex.size() - 1); // distribution in range [0, dex.size()-1]

    const auto next_pk_index = [&](pokedex::index_t current_index) {
        pokedex::index_t next_index = current_index;
        while (next_index == current_index) {
            next_index = dist_dex(rng);
        }
        return next_index;
    };


    // Game logic things - Controller things to have
    const auto to_score = [](const item_t& item) -> score_t {
        return item.first.bts();
    };

    const auto refresh_model = [&](model_t& model, const repo_t& dex, replace_model_t& replace) {
        // Get a new specie for the replace_index
        replace.keep_index = next_pk_index(replace.keep_index);
        model.items[replace.replace_index] = item_t { dex.at(replace.keep_index), false };
        
        // Update replace model for next time
        replace.replace_index ^= 1; // alternate between 0 and 1

        model.round++;
        model.score += 1;
    };

    // View things / Controller things to show
    const auto print_pokemons = [](const model_t& model, const auto& to_score) {
        using std::cout;

        const poke_specie& poke_left = model.items[0].first;
        const poke_specie& poke_right = model.items[1].first;
        const bool left_revealed = model.items[0].second;
        const bool right_revealed = model.items[1].second;  

        cout << "\nPokemon #1: " << poke_left.name << (poke_left.is_alt_form() ? " (" + poke_left.alt_name + ")" : "") << (left_revealed ? 
                " (Total Base Stats: " + std::to_string(to_score(model.items[0])) + ")" : "") << "\n";
        cout << "Pokemon #2: " << poke_right.name << (poke_right.is_alt_form() ? " (" + poke_right.alt_name + ")" : "") << (right_revealed ? 
                " (Total Base Stats: " + std::to_string(to_score(model.items[1])) + ")" : "") << "\n";
    };

    const auto show_round = [ &to_score, &print_pokemons ] (const model_t& model) {
        using std::cout;
        cout << "\n--- Round " << model.round << " ---\n";
        print_pokemons(model, to_score);
    };

    const auto play_round = [&to_score] (const model_t& model) -> choice_t {
        string user_input;
        bool still_no_input = true;
        choice_t result {};
        while (still_no_input) {
        
            // Get user choice
            cout << "Which Pokemon has higher total base stats? Enter '1', '2', or 'q' to quit: ";
            cin >> user_input;

            if (user_input == "q") {
                auto max_index = std::numeric_limits<decltype(choice_t::chosen_index)>::max();
                return choice_t { .chosen_index = max_index, .is_correct = false };
            }

            if (user_input != "1" && user_input != "2") {
                cout << "Invalid input. Please enter '1', '2', or 'q' to quit.\n";
                continue;
            }

            // Parse user choice and determine correctness
            result.chosen_index = (user_input == "1") ? 0 : 1;
            result.is_correct = to_score(model.items[result.chosen_index]) >= 
                                to_score(model.items[result.chosen_index ^ 1]);
            still_no_input = false;
            
        }
        return result;
    };

    const auto show_round_results = [ &to_score, &print_pokemons ] (model_t& model) {
        using std::cout;
        
        model.items[0].second = true;
        model.items[1].second = true;
        print_pokemons(model, to_score);
    };


    cout << "\nWelcome to the Pokemon High-Low Game!\n";
    cout << "Try to guess if the next Pokemon's total base stats will be higher or lower than the current one.\n";

    if (dex.size() < 2) {
        cout << "Not enough species in the pokedex to play the game.\n";
        return;
    }

    bool playing = true;
    pokedex::index_t tmp_index = next_pk_index(-1);
    replace_model_t replace { .replace_index = 1, .keep_index = tmp_index }; 
    model_t model { .score = 0, .round = 1, .items = 
        {
        item_t { dex.at(replace.keep_index), false },
        item_t { dex.at(tmp_index = next_pk_index(replace.keep_index)), false }
        }
    };
    replace.replace_index = to_score(model.items[0]) < to_score(model.items[1]) ? 0 : 1;
    replace.keep_index = to_score(model.items[0]) < to_score(model.items[1]) ? tmp_index : replace.keep_index;

    while (playing) {
        // Start round - display choice
        show_round(model);

        auto result = play_round(model);

        // Show again with stats revealed
        show_round_results(model);

        if (result.is_correct) {
            // Replace the pokemon that should go next
            refresh_model(model, dex, replace);
            cout << "Correct! Your current score is: " << model.score << "\n";
        } else {
            if (result.chosen_index == std::numeric_limits<decltype(choice_t::chosen_index)>::max()) {
                cout << "You chose to quit. Your final score is: " << model.score << "\n";
            } else {
                cout << "Wrong!" 
                    << " Your final score is: " << model.score << "\n";
            }
            playing = false;
        }
    }
    return;    
}