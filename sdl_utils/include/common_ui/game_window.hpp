#ifndef AFF_PK_PROJECTS_SDL_UTILS_COMMON_GAME_WINDOW_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_COMMON_GAME_WINDOW_HPP

#include <SDL2/SDL.h>
#include <SDL_image.h>

#include <string>
#include <stdexcept>

#include <common_ui/layout.hpp>

// em_script compatibility
#ifdef _EMSCRIPTEN
#include <emscripten.h>
#endif

using std::runtime_error;
using std::string;

namespace aff::sdl_utils::common {

    class game_window {
    private:
        SDL_Window* window {nullptr};
        SDL_Renderer* renderer {nullptr};
        layout* content {nullptr};
        bool running {false};

        void main_loop();

    public:
        game_window(const std::string& title, int width, int height, layout* content);
        // Rule of five
        game_window();
        // Copy constructor
        game_window(const game_window& other);
        // Move constructor
        game_window(game_window&& other) noexcept;
        // Copy assignment operator
        game_window& operator=(const game_window& other);
        // Move assignment operator
        game_window& operator=(game_window&& other) noexcept;
        // Destructor
        ~game_window();

        SDL_Renderer* getRenderer() const;
        SDL_Window* getWindow() const;

        void run();

        void clear();

        void render();

        void handleEvent(const SDL_Event& event);

        void display();

    };

    game_window::game_window(const string& title, int width, int height, layout* content) : content(content) {
        // Create a resizable window
        window = SDL_CreateWindow(title.c_str(),
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  width,
                                  height,
                                  SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

        if (!window)
        {
            std::string msg = "Window could not be created! SDL_Error: ";
            msg += SDL_GetError();
            throw std::runtime_error(msg);
        }

        // Create a renderer for the window
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer)
        {
            std::string msg = "Renderer could not be created! SDL_Error: ";
            msg += SDL_GetError();
            SDL_DestroyWindow(window);
            throw std::runtime_error(msg);
        }

        // Initialize the layout with the renderer
        if (content)
        {
            content->initialize(renderer);
        }
    }

    // Rule of five
    game_window::game_window() = default;
    
    // Copy constructor
    game_window::game_window(const game_window &other) : content(other.content) {
        if (other.window == nullptr)
        {
            this->window = nullptr;
            this->renderer = nullptr;
            return;
        }

        int width{0}, height{0};
        SDL_GetWindowSize(other.window, &width, &height);
        window = SDL_CreateWindow(SDL_GetWindowTitle(other.window),
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  width, height,
                                  SDL_WINDOW_RESIZABLE);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer)
        {
            std::string msg = "Renderer could not be created! SDL_Error: ";
            msg += SDL_GetError();
            SDL_DestroyWindow(window);
            throw std::runtime_error(msg);
        }
        if (content)
        {
            content->initialize(renderer);
        }
    }

    // Move constructor
    game_window::game_window(game_window &&other) noexcept : window(other.window), renderer(other.renderer), content(other.content) {
        other.window = nullptr;
        other.renderer = nullptr;
        other.content = nullptr;
    }

    // Copy assignment operator
    game_window& game_window::operator=(const game_window &other) {
        if (this != &other) {
                // Cleanup existing resources
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);

                // Copy resources from the other object
                this->content = other.content;
                if (other.window == nullptr) {
                    this->window = nullptr;
                    this->renderer = nullptr;
                    
                    return *this;
                }
                int width {0}, height {0};
                SDL_GetWindowSize(other.window, &width, &height);
                window = SDL_CreateWindow(SDL_GetWindowTitle(other.window),
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           width, height,
                                           SDL_WINDOW_RESIZABLE);
                renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
                if (content) {
                    content->initialize(renderer);
                }
            }
            return *this;
    }

    // Move assignment operator
    game_window& game_window::operator=(game_window &&other) noexcept {
        if (this != &other) {
            // Cleanup existing resources
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            
            // Move resources from the other object
            window = other.window;
            renderer = other.renderer;
            content = other.content;

            // Nullify the other object's pointers
            other.window = nullptr;
            other.renderer = nullptr;
            other.content = nullptr;
        }
        return *this;
    }

    // Destructor
    game_window::~game_window() {
        // Cleanup
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }

    SDL_Renderer* game_window::getRenderer() const {
        return renderer;
    }

    SDL_Window* game_window::getWindow() const {
        return window;
    }

    void game_window::main_loop() {
        SDL_Event event;
        // Handle events
        while (SDL_PollEvent(&event)) {
            handleEvent(event);
        }

        // Clear the renderer
        clear();

        // Render the layout
        render();

        // Present the renderer
        display();
    }

    void game_window::run() {
        running = true;

#ifdef _EMSCRIPTEN
        // For Emscripten, set the main loop using emscripten_set_main_loop
        emscripten_set_main_loop_arg(
            [](void* arg) {
                static_cast<game_window*>(arg)->main_loop();
            },
            this,
            0, // fps = 0 means to use the browser's requestAnimationFrame
            1  // simulate_infinite_loop = 1
        );
#else
        // Standard loop for non-Emscripten environments
        while (running) {
            main_loop();
        }
#endif
    }

    void game_window::clear() {
        SDL_RenderClear(renderer);
    }

    void game_window::render() {
        if (content) {
            content->render(renderer);
        }
    }

    void game_window::handleEvent(const SDL_Event& event) {
        if (event.type == SDL_QUIT) {
            running = false;
#ifdef _EMSCRIPTEN
            emscripten_cancel_main_loop();
#endif
            return;
        }
        if (content) {
            content->handleEvent(event);
        }
    }

    void game_window::display() {
        SDL_RenderPresent(renderer);
    }
} // namespace pkgac::ui

#endif // AFF_PK_PROJECTS_SDL_UTILS_COMMON_GAME_WINDOW_HPP