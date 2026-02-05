#include <array>
#include <iostream>
#include <filesystem>
#include <utility>
#include <random>

#include <models/poke_specie.hpp>
#include <models/pokedex.hpp>

#include <controller/game_choice.hpp>

#include <models/game_model.hpp>

#include <views/console_view.hpp>
#include <controller/game_controller.hpp>

// SDL-based view/controller (new)
#include <views/sdl_view.hpp>
#include <controller/sdl_controller.hpp>

// void play_high_low(const aff::pk_high_low::models::pokedex& dex);

int main() {
    using namespace aff::pk_high_low::models;
    using namespace aff::pk_high_low::views;
    using namespace aff::pk_high_low::controller;
    using std::filesystem::path;
    using std::vector;

    // Dex file path
#ifdef __EMSCRIPTEN__
    // When running in the browser we expect assets to be preloaded into
    // the virtual filesystem under '/assets' or the relative 'assets' path.
    const path assets_path { "assets" };
#else
    const path assets_path { std::filesystem::absolute(__FILE__).parent_path().parent_path() / "assets"};
#endif
    const path dex_path  { assets_path / "dex.json" };

    // Parse each entry
    const pokedex dex { pokedex::from_file(dex_path) };

    assert(dex.size() > static_cast<pokedex::index_t>(2) && 
           "Pokedex should not be empty");

    // Make utils for game model
    using item_t = std::pair<poke_specie, bool>; // bool indicates if revealed
    using item_score_t = poke_specie::poke_stat_t;
    const auto get_bts = [](const item_t& item) -> item_score_t {
        return item.first.bts();
    };

    const auto get_bts_speed = [](const item_t& item) -> item_score_t {
        return item.first.speed;
    };

    // Things for item update
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist_dex(0, dex.size() - 1); // distribution in range [0, dex.size()-1]
    std::array<pokedex::index_t, 2> previous_indices {
        static_cast<pokedex::index_t>(-1),
        static_cast<pokedex::index_t>(-1)
    };
    uint8_t replace_index = 0;
    const auto next_different_index = [&](pokedex::index_t index1, pokedex::index_t index2) {
        pokedex::index_t next_index = index1;
        while (next_index == index1 || next_index == index2) {
            next_index = dist_dex(rng);
        }
        return next_index;
    };

    // Item update function
    const auto item_update = [&](std::pair<item_t, item_t>& items){

        // Update visibility of current items
        // Only if its not the first round
        bool reveal = items.first.first.id != item_t().first.id && items.second.first.id != item_t().first.id;
        items.first.second = reveal;
        items.second.second = reveal;

        // Get a new specie for the current index
        auto new_index = next_different_index(previous_indices[0], previous_indices[1]);
        previous_indices[replace_index] = new_index;
        if (replace_index == 0) {
            items.second = item_t { dex.at(new_index), false };
        } else { // replace_index == 1
            items.first = item_t { dex.at(new_index), false };
        }
        replace_index ^= 1; // alternate between 0 and 1
    };


    const auto auto_win_ties = [](const std::pair<item_score_t, item_score_t>& item_scores, const aff::pk_high_low::controller::game_choice& choice) -> bool {
        switch (choice) {
            case aff::pk_high_low::controller::game_choice::FIRST_CHOSEN:
                return item_scores.first >= item_scores.second;
            case aff::pk_high_low::controller::game_choice::SECOND_CHOSEN:
                return item_scores.second >= item_scores.first;
            case aff::pk_high_low::controller::game_choice::TIE_CHOSEN:
                return item_scores.first == item_scores.second;
            default:
                return false;
        }
    };

    using hl_model_t = game_model<item_t, item_score_t, 
                                decltype(get_bts), 
                                decltype(item_update), 
                                decltype(auto_win_ties)>;
    auto hl_model = hl_model_t(
        std::move(get_bts),
        std::move(item_update),
        std::move(auto_win_ties)
    );

    const auto print_pokemon = [](const hl_model_t::entity_type& item, 
                            const hl_model_t::item_score_t& score, bool force_reveal) -> std::string {

        const poke_specie& poke = item.first;
        const bool revealed = item.second;  

        return std::string (
            poke.name + 
            (poke.is_alt_form() ? " (" + poke.alt_name + ")" : "") + 
            (revealed || force_reveal ? 
                " (Total Base Stats: " + std::to_string(score) + ")" 
                : "")
        );
    };

    // Original console view usage (commented out to keep code):
    // using view_t = console_view<hl_model_t, decltype(print_pokemon)>;
    // auto hl_view = view_t(std::move(print_pokemon));
    // auto hl_game = high_low_game(std::move(hl_model), std::move(hl_view));
    // hl_game.run();

    // SDL view/controller usage
    using sdl_view_t = aff::pk_high_low::views::sdl_view<hl_model_t>;
    auto sdl_view_inst = sdl_view_t(assets_path);
    auto sdl_ctrl = aff::pk_high_low::controller::sdl_controller<hl_model_t, decltype(sdl_view_inst)>(
        std::move(hl_model), std::move(sdl_view_inst));
    sdl_ctrl.run();

    // play_high_low(dex);
    return EXIT_SUCCESS;
}

/*
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
        //return item.first.bts();
        //return item.first.speed;
        return std::max(item.first.atk, item.first.sp_atk);
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
*/