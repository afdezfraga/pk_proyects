#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_SDL_VIEW_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_SDL_VIEW_HPP

#include <views/view_concepts.hpp>
#include <models/model_concepts.hpp>
#include <controller/game_choice.hpp>

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <filesystem>
#include <optional>
#include <string>
#include <iostream>
#include <unordered_map>
#include <common_ui/window.hpp>

namespace aff::pk_high_low::views {

template <aff::pk_high_low::models::Model M>
class sdl_view {
public:
    using choice_t = aff::pk_high_low::controller::game_choice;

    explicit sdl_view(const std::filesystem::path& assets_path,
                      int width = 1024, int height = 768)
        : assets_path_(assets_path), width_(width), height_(height) {
        init_sdl();
        create_window();
    }

    // Construct from an existing window (uses its renderer). The underlying
    // SDL init is still performed if required (SDLManager may have already
    // initialized SDL/IMG/TTF).
    explicit sdl_view(sdl_utils::common::Window& window, const std::filesystem::path& assets_path,
                      int width = 1024, int height = 768)
        : assets_path_(assets_path), width_(width), height_(height) {
        // Ensure TTF/IMG are initialized (init_sdl checks WasInit)
        init_sdl();
        window_ = window.raw();
        renderer_ = window.renderer();
    }

    ~sdl_view() {
        cleanup();
    }

    void show(const std::string& message) {
        std::cout << message << std::endl;
    }

    std::string prompt(const std::string& message) {
        std::cout << message;
        std::string s;
        std::getline(std::cin, s);
        return s;
    }

    void show_round(const M& model) {
        render_model(model, false);
    }

    choice_t get_player_choice(const M& model) {
        // Wait for an SDL event that maps to a choice.
        SDL_Event event;
        // Ensure current model is rendered so user sees it while waiting
        render_model(model, false);

        while (true) {
            if (SDL_WaitEvent(&event) == 0) continue;

            if (event.type == SDL_QUIT) return choice_t::QUIT_GAME;

            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    // Re-render to adapt to new size
                    render_model(model, false);
                    continue;
                }
            }

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_1: return choice_t::FIRST_CHOSEN;
                    case SDLK_2: return choice_t::SECOND_CHOSEN;
                    case SDLK_t: return choice_t::TIE_CHOSEN;
                    case SDLK_q: return choice_t::QUIT_GAME;
                    default: break;
                }
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int w, h;
                SDL_GetWindowSize(window_, &w, &h);
                bool vertical_split = (w >= h);
                int x = event.button.x;
                int y = event.button.y;
                if (vertical_split) {
                    if (x < w/2) return choice_t::FIRST_CHOSEN;
                    else return choice_t::SECOND_CHOSEN;
                } else {
                    if (y < h/2) return choice_t::FIRST_CHOSEN;
                    else return choice_t::SECOND_CHOSEN;
                }
            }
        }
    }

    void show_round_results(const M& model, bool /*is_correct*/) {
        render_model(model, true);
    }

private:
    std::filesystem::path assets_path_;
    SDL_Window* window_ { nullptr };
    SDL_Renderer* renderer_ { nullptr };
    TTF_Font* font_ { nullptr };
    int width_ {1024};
    int height_ {768};

    std::unordered_map<std::string, SDL_Texture*> tex_cache_;
    bool img_inited_ { false };
    bool ttf_inited_ { false };

    void init_sdl() {
        if (SDL_WasInit(SDL_INIT_VIDEO) == 0) {
            if (SDL_Init(SDL_INIT_VIDEO) != 0) {
                throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
            }
        }
        int img_flags = IMG_INIT_JPG | IMG_INIT_PNG;
        int inited = IMG_Init(img_flags);
        img_inited_ = (inited & img_flags) != 0;
        if (!img_inited_) {
            std::cerr << "Warning: IMG_Init failed or some formats unavailable: " << IMG_GetError() << std::endl;
        }
        if (TTF_WasInit() == 0) {
            if (TTF_Init() == 0) {
                ttf_inited_ = true;
            } else {
                std::cerr << "Warning: TTF_Init failed: " << TTF_GetError() << std::endl;
            }
        } else {
            ttf_inited_ = true;
        }
        // Try a few common system fonts
        const std::vector<std::string> try_fonts {
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
            "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
        };
        for (const auto& p : try_fonts) {
            if (!font_ && std::filesystem::exists(p)) {
                font_ = TTF_OpenFont(p.c_str(), 18);
            }
        }
        if (!font_) {
            // Not fatal; names will be printed to stdout instead
            std::cerr << "No TTF font found; names will be printed to console instead of rendered." << std::endl;
        }
    }

public:
    // Move constructor - transfer ownership of SDL resources
    sdl_view(sdl_view&& other) noexcept
        : assets_path_(std::move(other.assets_path_)), window_(other.window_), renderer_(other.renderer_),
          font_(other.font_), width_(other.width_), height_(other.height_),
          tex_cache_(std::move(other.tex_cache_)), img_inited_(other.img_inited_), ttf_inited_(other.ttf_inited_) {
        other.window_ = nullptr;
        other.renderer_ = nullptr;
        other.font_ = nullptr;
        other.img_inited_ = false;
        other.ttf_inited_ = false;
        other.tex_cache_.clear();
    }

    // Move assignment - transfer ownership
    sdl_view& operator=(sdl_view&& other) noexcept {
        if (this != &other) {
            cleanup();
            assets_path_ = std::move(other.assets_path_);
            window_ = other.window_;
            renderer_ = other.renderer_;
            font_ = other.font_;
            width_ = other.width_;
            height_ = other.height_;
            tex_cache_ = std::move(other.tex_cache_);
            img_inited_ = other.img_inited_;
            ttf_inited_ = other.ttf_inited_;

            other.window_ = nullptr;
            other.renderer_ = nullptr;
            other.font_ = nullptr;
            other.img_inited_ = false;
            other.ttf_inited_ = false;
            other.tex_cache_.clear();
        }
        return *this;
    }

    // Disable copy
    sdl_view(const sdl_view&) = delete;
    sdl_view& operator=(const sdl_view&) = delete;

private:
    void create_window() {
        window_ = SDL_CreateWindow("PK High-Low",
                                   SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   width_, height_, SDL_WINDOW_RESIZABLE);
        if (!window_) throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
        renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer_) throw std::runtime_error(std::string("SDL_CreateRenderer failed: ") + SDL_GetError());
    }

    SDL_Texture* load_texture_for_id(const std::string& id) {
        auto it = tex_cache_.find(id);
        if (it != tex_cache_.end()) return it->second;

        std::filesystem::path img_path = assets_path_ / "images" / (id + ".jpg");
        if (!std::filesystem::exists(img_path)) {
            // try png
            img_path = assets_path_ / "images" / (id + ".png");
            if (!std::filesystem::exists(img_path)) return nullptr;
        }

        SDL_Surface* surf = IMG_Load(img_path.string().c_str());
        if (!surf) {
            std::cerr << "IMG_Load failed for " << img_path << ": " << IMG_GetError() << std::endl;
            return nullptr;
        }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer_, surf);
        SDL_FreeSurface(surf);
        if (!tex) {
            std::cerr << "CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
            return nullptr;
        }
        tex_cache_.emplace(id, tex);
        return tex;
    }

    void render_model(const M& model, bool reveal_score) {
        if (!renderer_) return;
        SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 255);
        SDL_RenderClear(renderer_);

        auto items = model.items();
        auto scores = model.itemScores();

        int w, h;
        SDL_GetWindowSize(window_, &w, &h);
        bool vertical_split = (w >= h);

        SDL_Rect left_rect, right_rect;
        if (vertical_split) {
            left_rect = {0, 0, w/2, h};
            right_rect = {w/2, 0, w - w/2, h};
        } else {
            left_rect = {0, 0, w, h/2};
            right_rect = {0, h/2, w, h - h/2};
        }

        render_item_in_rect(items.first, scores.first, left_rect, reveal_score);
        render_item_in_rect(items.second, scores.second, right_rect, reveal_score);

        SDL_RenderPresent(renderer_);
    }

    void render_item_in_rect(const typename M::item_t& item,
                             const typename M::item_score_t& score,
                             const SDL_Rect& rect,
                             bool reveal_score) {
        // background
        SDL_SetRenderDrawColor(renderer_, 35, 35, 35, 255);
        SDL_Rect bg = rect;
        SDL_RenderFillRect(renderer_, &bg);

        // image
        SDL_Texture* tex = load_texture_for_id(item.first.id);
        if (tex) {
            int tw, th;
            SDL_QueryTexture(tex, nullptr, nullptr, &tw, &th);
            // compute dest rect preserving aspect and leaving space at bottom for text
            int text_area = 60;
            int avail_w = rect.w - 20;
            int avail_h = rect.h - text_area - 20;
            float img_aspect = (float)tw / (float)th;
            int dst_w = avail_w;
            int dst_h = (int)(dst_w / img_aspect);
            if (dst_h > avail_h) {
                dst_h = avail_h;
                dst_w = (int)(dst_h * img_aspect);
            }
            SDL_Rect dst { rect.x + (rect.w - dst_w)/2, rect.y + 10, dst_w, dst_h };
            SDL_RenderCopy(renderer_, tex, nullptr, &dst);
        }

        // text (name and optional score)
        // Determine whether the score should be shown: either globally requested
        // via reveal_score or the item itself is marked revealed (item.second).
        bool show_score = reveal_score || item.second;

        if (font_) {
            render_text_at(item.first.name + (item.first.is_alt_form() ? (" (" + item.first.alt_name + ")") : ""),
                           rect.x + 10, rect.y + rect.h - 50, rect.w - 20);
            if (show_score) {
                render_text_at(std::string("Score: ") + std::to_string(score),
                               rect.x + 10, rect.y + rect.h - 25, rect.w - 20);
            }
        } else {
            // fallback: print to stdout
            std::cout << "Display: " << item.first.id << " - " << item.first.name;
            if (item.first.is_alt_form()) std::cout << " (" << item.first.alt_name << ")";
            if (show_score) std::cout << " | Score: " << score;
            std::cout << std::endl;
        }
    }

    void render_text_at(const std::string& txt, int x, int y, int max_w) {
        if (!font_) return;
        SDL_Color col { 255, 255, 255, 255 };
        SDL_Surface* surf = TTF_RenderUTF8_Blended_Wrapped(font_, txt.c_str(), col, max_w);
        if (!surf) return;
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer_, surf);
        SDL_Rect dst { x, y, surf->w, surf->h };
        SDL_FreeSurface(surf);
        if (!tex) return;
        SDL_RenderCopy(renderer_, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
    }

    // Render wrapped text centered horizontally at the given y position.
    void render_text_centered(const std::string& txt, int y, int max_w) {
       if (!font_) return;
            SDL_Color col { 255, 255, 255, 255 };
            SDL_Surface* surf = TTF_RenderUTF8_Blended_Wrapped(font_, txt.c_str(), col, max_w);
            if (!surf) return;
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer_, surf);
            int surf_w = surf->w;
            int surf_h = surf->h;
            SDL_FreeSurface(surf);
            if (!tex) return;
            int win_w, win_h;
            SDL_GetWindowSize(window_, &win_w, &win_h);
            SDL_Rect dst { (win_w - surf_w) / 2, y, surf_w, surf_h };
            SDL_RenderCopy(renderer_, tex, nullptr, &dst);
            SDL_DestroyTexture(tex);
    }

    void cleanup() {
        for (auto& kv : tex_cache_) {
            SDL_DestroyTexture(kv.second);
        }
        tex_cache_.clear();
        if (font_) TTF_CloseFont(font_);
        if (renderer_) SDL_DestroyRenderer(renderer_);
        if (window_) SDL_DestroyWindow(window_);
        if (img_inited_) IMG_Quit();
        if (ttf_inited_) TTF_Quit();
        // Do not call SDL_Quit() globally as other parts may rely on it.
    }

public:
    // Show a final message with Exit and Replay buttons. Returns true if the user
    // chose to replay, false to exit. Blocks until a selection is made.
    bool wait_for_exit(const std::string& message) {
        if (!renderer_ || !window_) return false;

        bool selection_made = false;
        bool want_replay = false;
        while (!selection_made) {
            // render background
            SDL_SetRenderDrawColor(renderer_, 10, 10, 10, 255);
            SDL_RenderClear(renderer_);

            int w, h;
            SDL_GetWindowSize(window_, &w, &h);

            // draw message centered
            if (font_) {
                int max_w = w - 40;
                int msg_y = h/3;
                render_text_centered(message, msg_y, max_w);
            } else {
                std::cout << message << std::endl;
            }

            // draw Replay and Exit buttons
            SDL_Rect replay_btn { w/2 - 160, h*2/3 - 20, 120, 40 };
            SDL_Rect exit_btn   { w/2 + 40,  h*2/3 - 20, 120, 40 };

            // Replay button
            SDL_SetRenderDrawColor(renderer_, 70, 180, 90, 255);
            SDL_RenderFillRect(renderer_, &replay_btn);
            if (font_) {
                render_text_at(std::string("Replay"), replay_btn.x + 16, replay_btn.y + 10, replay_btn.w - 32);
            }

            // Exit button
            SDL_SetRenderDrawColor(renderer_, 200, 90, 90, 255);
            SDL_RenderFillRect(renderer_, &exit_btn);
            if (font_) {
                render_text_at(std::string("Exit"), exit_btn.x + 32, exit_btn.y + 10, exit_btn.w - 64);
            }

            SDL_RenderPresent(renderer_);

            SDL_Event event;
            while (SDL_WaitEvent(&event)) {
                if (event.type == SDL_QUIT) { selection_made = true; want_replay = false; break; }
                if (event.type == SDL_WINDOWEVENT) {
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        // Break to outer loop which will re-render the message/buttons with new size
                        break;
                    }
                }
                if (event.type == SDL_KEYDOWN) {
                    // 'r'/'enter' = replay, 'q'/'backspace'/'esc' = exit
                    if (event.key.keysym.sym == SDLK_r || event.key.keysym.sym == SDLK_RETURN) { selection_made = true; want_replay = true; break; }
                    if (event.key.keysym.sym == SDLK_q || event.key.keysym.sym == SDLK_BACKSPACE || event.key.keysym.sym == SDLK_ESCAPE) { selection_made = true; want_replay = false; break; }
                }
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int mx = event.button.x;
                    int my = event.button.y;
                    if (mx >= replay_btn.x && mx <= replay_btn.x + replay_btn.w && my >= replay_btn.y && my <= replay_btn.y + replay_btn.h) { selection_made = true; want_replay = true; break; }
                    if (mx >= exit_btn.x && mx <= exit_btn.x + exit_btn.w && my >= exit_btn.y && my <= exit_btn.y + exit_btn.h) { selection_made = true; want_replay = false; break; }
                }
            }
        }
        return want_replay;
    }

    // Block until any key is pressed or mouse is clicked (or window closed).
    void wait_for_any_input() {
        if (!renderer_ || !window_) return;
        SDL_Event event;
        while (true) {
            if (SDL_WaitEvent(&event) == 0) continue;
            if (event.type == SDL_QUIT) break;
            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    // re-render the last shown model to adapt to new size
                    if (renderer_) {
                        SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 255);
                        SDL_RenderClear(renderer_);
                        SDL_RenderPresent(renderer_);
                    }
                    continue;
                }
            }
            if (event.type == SDL_KEYDOWN) break;
            if (event.type == SDL_MOUSEBUTTONDOWN) break;
        }
    }
};

} // namespace aff::pk_high_low::views

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_SDL_VIEW_HPP
