#include "controller/app_controller.hpp"

#include <common_ui/sdl_manager.hpp>
#include <common_ui/window.hpp>
#include <SDL2/SDL.h>

#include <iostream>
#include <random>

#include <controller/sdl_controller.hpp>

#include <views/sdl_config_view.hpp>
#include <views/sdl_view.hpp>

#include <models/game_model.hpp>
#include <models/model_factory.hpp>
#include <models/poke_specie.hpp>
#include <controller/controller_api.hpp>
#include <controller/settings_controller.hpp>
#include <controller/game_state_controller.hpp>
#include <controller/end_controller.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace aff::pk_high_low::controller {

AppController::AppController(sdl_utils::common::Window& window, const std::filesystem::path& assets_path)
    : assets_path_(assets_path), window_(&window) {}

void AppController::process_pending(MainLoopData* d)
{

    if (d->pending && d->pending->has_value()) {
        switch (d->pending->value()) {
            case Action::START_GAME: d->game->configure(d->settings->settings()); *d->cur = Screen::GAME; break;
            case Action::RESTART: d->game->configure(d->settings->settings()); *d->cur = Screen::GAME; break;
            case Action::SHOW_END: *d->cur = Screen::END; break;
            // TMP changed to just do the same as start// case Action::RESTART: d->game->restart(); *d->cur = Screen::GAME; break;
            case Action::BACK_TO_SETTINGS: d->game->reset(); *d->cur = Screen::SETTINGS; break;
            case Action::QUIT: *d->quit = true; break;
            default: break;
        }
        d->pending->reset();
    }
}

int AppController::run()
{
    using namespace aff::sdl_utils::common;

    try {
        Window* used_window = window_;

        // Construct controllers (alive concurrently)
        SettingsController settings_ctl(*used_window, assets_path_);
        GameControllerT game_ctl(*used_window, assets_path_);
        EndController end_ctl(*used_window, assets_path_);

        // App runtime context and API
        AppContext ctx; ctx.window = used_window; ctx.assets_path = assets_path_;
        std::optional<Action> pending_action;
        AppAPI api; api.request = [&](Action a){ pending_action = a; };

        Screen cur = Screen::SETTINGS;

        bool quit = false;

        MainLoopData data { used_window, &settings_ctl, &game_ctl, &end_ctl, &ctx, &api, &pending_action, &cur, &quit };

        const auto main_loop_tick = [](void* arg) {
            MainLoopData* d = static_cast<MainLoopData*>(arg);
            SDL_Event ev;
            // Process all pending SDL events
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_QUIT) { *d->quit = true; continue; }
                // dispatch event to current screen
                switch (*d->cur) {
                    case Screen::SETTINGS: d->settings->tick(&ev, *d->ctx, *d->api); break;
                    case Screen::GAME: d->game->tick(&ev, *d->ctx, *d->api); break;
                    case Screen::END: d->end->tick(&ev, *d->ctx, *d->api); break;
                }
            }

            // Per-frame update/render (pass nullptr event)
            switch (*d->cur) {
                case Screen::SETTINGS: d->settings->tick(nullptr, *d->ctx, *d->api); break;
                case Screen::GAME: d->game->tick(nullptr, *d->ctx, *d->api); break;
                case Screen::END: d->end->tick(nullptr, *d->ctx, *d->api); break;
            }

            // Check for pending actions requested by controllers
            AppController::process_pending(d);

#ifdef __EMSCRIPTEN__
            if (*d->quit) emscripten_cancel_main_loop();
#endif
        };

#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop_arg(main_loop_tick, &data, 0, true);
        // Under Emscripten, control does not return here until canceled
        return EXIT_SUCCESS;
#else
        while (!quit) {
            main_loop_tick(&data);
            SDL_Delay(16);
        }
        return EXIT_SUCCESS;
#endif

    } catch (const std::exception& ex) {
        std::cerr << "Fatal error in AppController: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}

/*
int AppController::run_dummy()
{
    try {
        // Use provided window if available, otherwise create local manager+window
        if (window_) {
            SDL_Renderer* ren = window_->renderer();
            SDL_Event ev;
            SDL_Color teal{0, 128, 128, 255};
            bool quit = false;
            int state = 0;

            struct MainLoopData {
                SDL_Renderer* renderer;
                SDL_Event* event;
                SDL_Color* color;
                bool* quit_flag;
                int* state;
            } mainLoopData { ren, &ev, &teal, &quit, &state };
            const auto main_loop_tick = [](void* arg) {
                MainLoopData* data = static_cast<MainLoopData*>(arg);
                SDL_Renderer* ren = data->renderer;
                SDL_Color* teal = data->color;
                bool* quit = data->quit_flag;
                SDL_Event* ev = data->event;

                SDL_SetRenderDrawColor(ren, teal->r * *data->state, teal->g * *data->state, teal->b * *data->state, teal->a);
                SDL_RenderClear(ren);
                SDL_RenderPresent(ren);

                if (SDL_WaitEvent(ev)) {
                    if (ev->type == SDL_QUIT) 
                        *quit = true;
                    if (ev->type == SDL_KEYDOWN) {
                        if (*data->state == 0) {
                            *data->state = 1; 
                            std::cout << "Showing teal screen.";
                            std::cout << " Press any key or close the window to exit." << std::endl;
                        }
                        else 
                        *quit = true;
                    }
                    if (ev->type == SDL_MOUSEBUTTONDOWN) {
                        if (*data->state == 0)
                            *data->state = 1; 
                        else 
                            *quit = true;
                    }
                }
                #ifdef __EMSCRIPTEN__
                    if (*quit) {
                        emscripten_cancel_main_loop(); // Stop the main loop immediately when quit is triggered
                    }
                #endif
                std::cout << "Main loop tick. State: " << *data->state << ", Quit: " << (*data->quit_flag ? "true" : "false") << std::endl;
            };
            #ifdef __EMSCRIPTEN__
                emscripten_set_main_loop_arg(main_loop_tick, &mainLoopData, 0, true);
            #else
                while (!quit) {
                    main_loop_tick(&mainLoopData);
                }
            #endif
        } 
        return EXIT_SUCCESS;
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error in AppController::run_dummy: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
*/

} // namespace aff::pk_high_low::controller
