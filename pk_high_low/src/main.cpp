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

#include <common_ui/sdl_manager.hpp>
#include <common_ui/window.hpp>
#include <controller/app_controller.hpp>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// void play_high_low(const aff::pk_high_low::models::pokedex& dex);

void play_high_low_sdl();

void test_sdl();

int main() {

    using namespace aff::sdl_utils::common;

    // Determine assets path (works for Emscripten and native)
#ifdef __EMSCRIPTEN__
    const std::filesystem::path assets_path { "assets" };
#else
    const std::filesystem::path assets_path { std::filesystem::absolute(__FILE__).parent_path().parent_path() / "assets" };
#endif

    try {
        // Initialize SDL subsystems via SDLManager and create a Window to pass to AppController
        SDLManager manager(SDL_INIT_VIDEO);
        Window window("PK High-Low", 1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        aff::pk_high_low::controller::AppController app(window, assets_path);
        return app.run();
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void test_sdl() {
    // Init sdl 
    // create a window 
    // and make it change color between red, blue and green every 5 seconds, 
    // while also printing to the console which color it is showing.
    // Make it compatible with Emscripten so it can run in the browser
    // , and also make sure to clean up SDL resources on exit.

    // 1. Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }

    // 2. Create a window
    SDL_Window* win = SDL_CreateWindow("SDL Test", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (win == nullptr) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    // 3. Create a renderer
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == nullptr) {
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(win);
        SDL_Quit();
        return;
    }

    // 4. Main loop variables
    bool quit = false;
    SDL_Event e;
    uint64_t lastColorChange = UINT64_MAX; // Max value to ensure immediate change on first tick
    std::array<SDL_Color, 3> colors = { SDL_Color{255, 0, 0, 255}, 
                                        SDL_Color{0, 255, 0, 255}, 
                                        SDL_Color{0, 0, 255, 255} };
    std::array<std::string, 3> colorNames = { "Red", "Green", "Blue" };
    int colorIndex = colors.size() - 1; // Start with the last color so the first change shows the first color
    // Determine assets path (works for Emscripten and native)
#ifdef __EMSCRIPTEN__
    const std::filesystem::path assets_path { "assets" };
#else
    const std::filesystem::path assets_path { std::filesystem::absolute(__FILE__).parent_path().parent_path() / "assets" };
#endif

    // Initialize SDL_image for JPG support
    if ((IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG) == 0) {
        std::cout << "IMG_Init Error: " << IMG_GetError() << std::endl;
        // continue without images
    }

    // Initialize SDL_ttf
    TTF_Font* font = nullptr;
    bool ttf_inited = false;
    if (TTF_Init() != 0) {
        std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
    } else {
        ttf_inited = true;
    }

    // Filenames in the requested order
    std::array<std::string, 3> image_files = { "0004.jpg", "0001.jpg", "0007.jpg" };
    std::array<std::string, 3> names = { "Charmander", "Bulbasaur", "Squirtle" };

    // Build full image paths and Preload textures
    std::vector<std::string> image_paths;
    image_paths.reserve(image_files.size());
    for (const auto &fname : image_files) {
        image_paths.push_back((assets_path / "images" / fname).string());
    }

    // Try to open a font: prefer an asset bundled font, fall back to common system fonts
    if (ttf_inited) {
        std::vector<std::filesystem::path> font_candidates {
            assets_path / "fonts" / "Roboto-Black.ttf",
            assets_path / "fonts" / "DejaVuSans.ttf",
            assets_path / "DejaVuSans.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
            "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
        };
        for (const auto &p : font_candidates) {
            if (std::filesystem::exists(p)) {
                font = TTF_OpenFont(p.string().c_str(), 28);
                if (font) break;
            }
        }
        if (!font) {
            std::cout << "Unable to find or open a TTF font. Text labels will not be shown." << std::endl;
        }
    }

    std::vector<SDL_Texture*> textures;
    textures.reserve(image_paths.size());
    for (const auto &img_path : image_paths) {
        SDL_Surface* surf = IMG_Load(img_path.c_str());
        if (!surf) {
            std::cout << "IMG_Load Error for " << img_path << ": " << IMG_GetError() << std::endl;
            textures.push_back(nullptr);
            continue;
        }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
        SDL_FreeSurface(surf);
        if (!tex) {
            std::cout << "SDL_CreateTextureFromSurface Error for " << img_path << ": " << SDL_GetError() << std::endl;
            textures.push_back(nullptr);
            continue;
        }
        textures.push_back(tex);
    }

    // Prepare name textures (rendered via TTF) if font is available
    std::vector<SDL_Texture*> name_textures;
    name_textures.reserve(names.size());
    if (font) {
        SDL_Color text_color { 255, 255, 255, 255 };
        for (const auto &nm : names) {
            SDL_Surface* txt_surf = TTF_RenderUTF8_Blended(font, nm.c_str(), text_color);
            if (!txt_surf) {
                std::cout << "TTF_RenderUTF8_Blended Error for '" << nm << "': " << TTF_GetError() << std::endl;
                name_textures.push_back(nullptr);
                continue;
            }
            SDL_Texture* txt_tex = SDL_CreateTextureFromSurface(ren, txt_surf);
            SDL_FreeSurface(txt_surf);
            if (!txt_tex) {
                std::cout << "SDL_CreateTextureFromSurface(Error) for text '" << nm << "': " << SDL_GetError() << std::endl;
                name_textures.push_back(nullptr);
                continue;
            }
            SDL_SetTextureBlendMode(txt_tex, SDL_BLENDMODE_BLEND);
            name_textures.push_back(txt_tex);
        }
    } else {
        // push nulls to keep indices aligned
        for (size_t i = 0; i < names.size(); ++i) name_textures.push_back(nullptr);
    }

    struct MainLoopData {
        SDL_Renderer* renderer;
        std::array<SDL_Color, 3> colors;
        std::array<std::string, 3> colorNames;
        int colorIndex;
        uint64_t lastColorChange; // Max value to ensure immediate change on first tick
        bool quit;
        std::vector<SDL_Texture*>* textures;
        std::vector<std::string>* image_paths;
        std::vector<SDL_Texture*>* name_textures;
    } mainLoopData { ren, colors, colorNames, colorIndex, lastColorChange, quit, &textures, &image_paths, &name_textures };

    const auto main_loop_tick = [] (void* arg) -> void {
        // This function will be called by Emscripten's main loop
        // We can use it to handle events and update the screen
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
#ifdef __EMSCRIPTEN__
                emscripten_cancel_main_loop();
#endif
                static_cast<MainLoopData*>(arg)->quit = true; // Signal to exit main loop
            }
        }
        auto *data = static_cast<MainLoopData*>(arg);
        uint64_t currentTime = SDL_GetTicks64();
        if (currentTime < data->lastColorChange || currentTime - data->lastColorChange >= 5000) {
            data->colorIndex = (data->colorIndex + 1) % data->colors.size();
            data->lastColorChange = currentTime;

            std::cout << "Current color: " << data->colorNames[data->colorIndex] << std::endl;

            // Set the renderer color and clear
            SDL_SetRenderDrawColor(data->renderer, data->colors[data->colorIndex].r, data->colors[data->colorIndex].g, data->colors[data->colorIndex].b, data->colors[data->colorIndex].a);
            SDL_RenderClear(data->renderer);

                // If we have a texture for this index, render it centered
                if (data->textures && static_cast<size_t>(data->colorIndex) < data->textures->size()) {
                    std::cout << "Attempting to render image for color: " << data->colorNames[data->colorIndex] << std::endl;
                    SDL_Texture* tex = (*(data->textures))[data->colorIndex];
                    if (tex) {
                    int tex_w = 0, tex_h = 0;
                    SDL_QueryTexture(tex, NULL, NULL, &tex_w, &tex_h);
                    // Fit texture if larger than window
                    int win_w = 640, win_h = 480;
                    SDL_Rect dst;
                    float scale = std::min( (float)win_w / tex_w, (float)win_h / tex_h );
                    if (scale > 1.0f) scale = 1.0f;
                    dst.w = static_cast<int>(tex_w * scale);
                    dst.h = static_cast<int>(tex_h * scale);
                    dst.x = (win_w - dst.w) / 2;
                    dst.y = (win_h - dst.h) / 2;
                    SDL_RenderCopy(data->renderer, tex, NULL, &dst);

                    // Render the name texture centered on the image if present
                    if (data->name_textures && static_cast<size_t>(data->colorIndex) < data->name_textures->size()) {
                        SDL_Texture* name_tex = (*(data->name_textures))[data->colorIndex];
                        if (name_tex) {
                            int n_w = 0, n_h = 0;
                            SDL_QueryTexture(name_tex, NULL, NULL, &n_w, &n_h);
                            SDL_Rect ndst;
                            ndst.w = n_w;
                            ndst.h = n_h;
                            ndst.x = dst.x + (dst.w - ndst.w) / 2;
                            ndst.y = dst.y + (dst.h - ndst.h) / 2;
                            SDL_RenderCopy(data->renderer, name_tex, NULL, &ndst);
                        } else {
                            if (data->image_paths && static_cast<size_t>(data->colorIndex) < data->image_paths->size()) {
                                std::cout << "Name texture missing for image: " << (*(data->image_paths))[data->colorIndex] << std::endl;
                            }
                        }
                    }
                }
                else {
                    std::cout << "Current color: " << data->colorNames[data->colorIndex] 
                              << " has no image!!!" << std::endl;
                    if (data->image_paths && static_cast<size_t>(data->colorIndex) < data->image_paths->size()) {
                        std::cout << "Image not loaded: " << (*(data->image_paths))[data->colorIndex] << std::endl;
                    } else {
                        std::cout << "Image not available for index " << data->colorIndex << std::endl;
                    }
                }
            }

            SDL_RenderPresent(data->renderer);
        }
    };

    // 5. Main loop
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(main_loop_tick, &mainLoopData, 0, true);
#else
    while (!mainLoopData.quit) {
        main_loop_tick(&mainLoopData);
    }
#endif

    // Cleanup textures
    for (auto tex : textures) {
        if (tex) SDL_DestroyTexture(tex);
    }
    // Cleanup name textures and TTF
    for (auto ntx : name_textures) {
        if (ntx) SDL_DestroyTexture(ntx);
    }
    if (font) {
        TTF_CloseFont(font);
    }
    if (ttf_inited) {
        TTF_Quit();
    }
    // Quit SDL_image
    IMG_Quit();

    // Cleanup SDL resources
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return;

}

// void play_high_low_sdl() {}


void play_high_low_sdl() {
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
    const auto get_bst = [](const item_t& item) -> item_score_t {
        return item.first.bst();
    };

    const auto get_bst_speed = [](const item_t& item) -> item_score_t {
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
                                decltype(get_bst), 
                                decltype(item_update), 
                                decltype(auto_win_ties)>;
    auto hl_model = hl_model_t(
        std::move(get_bst),
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
        //return item.first.bst();
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