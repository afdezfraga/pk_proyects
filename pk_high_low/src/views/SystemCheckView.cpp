#include "views/SystemCheckView.hpp"
#include <common_ui/font.hpp>
#include <common_ui/text.hpp>
#include <common_ui/Theme.hpp>
#include <widgets/EnumSelector.hpp>
#include <filesystem>

namespace aff::pk_high_low::views {

int system_check_view::get_title_font_size(int win_w) {
    using namespace aff::sdl_utils::common;
    // Determine font size based on window width for responsive design
    if (win_w >= Window::BIG_W_BREAK_POINT) return lg_title_font_size_; // large screens
    else if (win_w >= Window::SMALL_W_BREAK_POINT) return md_title_font_size_; // medium screens
    else return sm_title_font_size_; // small screens
}

void system_check_view::tick(const SDL_Event* ev,
    aff::pk_high_low::controller::SettingsContext& ctx,
    aff::pk_high_low::controller::SettingsAPI& api)
{
    using namespace aff::sdl_utils::common;
    using namespace aff::sdl_utils::widgets;

    // Render background
    bg_.render(ctx.window->renderer());

    // Title: "StatDex.Check" with two colors (StatDex = white, .Check = theme primary)
    {
        aff::sdl_utils::common::Theme theme;
        SDL_Renderer* ren = ctx.window->renderer();
        // create/update textures when layout changes
        if (!is_set_up_ || (ev && ev->type == SDL_WINDOWEVENT && ev->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
            int win_w = 0, win_h = 0; SDL_GetRendererOutputSize(ren, &win_w, &win_h);
            int desired_font_size = get_title_font_size(win_w);
            if (desired_font_size != title_font_size_) {
                title_font_.reset(); // force re-creation of font and textures if font size changes
                title_font_size_ = desired_font_size;
            }
            // lazy-init title font: prefer SpaceGrotesk-Bold from assets, fallback to other candidates
            if (!title_font_) {
                std::string fp;
                std::filesystem::path p1 = ctx.assets_path / "fonts" / "SpaceGrotesk-Bold.ttf";
                if (std::filesystem::exists(p1)) fp = p1.string();
                else {
                    std::vector<std::filesystem::path> candidates {
                        ctx.assets_path / "fonts" / "SpaceGrotesk-Bold.ttf",
                        ctx.assets_path / "fonts" / "SpaceGrotesk-Regular.ttf",
                        ctx.assets_path / "fonts" / "Roboto-Black.ttf",
                        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
                    };
                    for (auto &pp : candidates) if (std::filesystem::exists(pp)) { fp = pp.string(); break; }
                }
                title_font_.emplace(fp, get_title_font_size(win_w));
            }

            const aff::sdl_utils::common::Font& tf = *title_font_;
            title_left_tex_ = aff::sdl_utils::common::Text::render(tf, "StatDex", SDL_Color{255,255,255,255}, ren);
            title_right_tex_ = aff::sdl_utils::common::Text::render(tf, ".Check", theme.primary, ren);
            int w1=0,h1=0,w2=0,h2=0; if (title_left_tex_.raw()) SDL_QueryTexture(title_left_tex_.raw(), nullptr, nullptr, &w1, &h1);
            if (title_right_tex_.raw()) SDL_QueryTexture(title_right_tex_.raw(), nullptr, nullptr, &w2, &h2);
            int total_w = w1 + w2;
            int x = (win_w - total_w) / 2;
            int y = 20;
            title_left_rect_ = { x, y, w1, h1 };
            title_right_rect_ = { x + w1, y, w2, h2 };
        }
        if (title_left_tex_.raw()) SDL_RenderCopy(ren, title_left_tex_.raw(), nullptr, &title_left_rect_);
        if (title_right_tex_.raw()) SDL_RenderCopy(ren, title_right_tex_.raw(), nullptr, &title_right_rect_);
    }

    // Update progress based on time for demo purposes
    Uint32 new_t = SDL_GetTicks();
    Uint32 dt = new_t - prev_t_;
    Uint32 t = new_t % 10000; // 0..10000
    float pct = static_cast<float>(t) / 10000.0f;
    prev_t_ = new_t;

    // get window size for responsive layout
    int win_w = 0, win_h = 0; SDL_GetRendererOutputSize(ctx.window->renderer(), &win_w, &win_h);

    // Circular eye centered above the progress bar
    if (!is_set_up_ || (ev && ev->type == SDL_WINDOWEVENT && ev->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
        eye_.setSizePercent(0.25f, 0.25f); // 25% of width/height
        eye_.setAnchor(0.5f, 0.35f);
        eye_.computeLayout(win_w, win_h);
        eye_.update(dt); // period of 2 seconds for full rotation
        eye_.render(ctx.window->renderer());
    } else {
        eye_.update(dt);
        eye_.render_animated(ctx.window->renderer());
    }


    // Responsive layout: replace progress bar with three selectors stacked vertically
    // Each selector will be full-width-ish and spaced vertically
    float sel_w = 0.8f;
    float sel_h = 0.07f;
    float start_anchor_y = 0.60f; // start ~60% down
    float gap = 0.02f;

    // Setup selectors on first layout or resize
    if (!is_set_up_ || (ev && ev->type == SDL_WINDOWEVENT && ev->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
        // build option lists from enums
        using namespace aff::pk_high_low::controller;
        std::vector<std::string> mode_opts{ to_string(game_mode::BST), to_string(game_mode::SPEED), to_string(game_mode::ATTACK) };
        std::vector<std::string> pokedex_opts{ to_string(pokedex_mode::STANDARD), to_string(pokedex_mode::GEN1_ONLY), to_string(pokedex_mode::VGC) };
        std::vector<std::string> diff_opts{ to_string(difficulty_mode::RANDOM), to_string(difficulty_mode::MATCHED), to_string(difficulty_mode::HARD), to_string(difficulty_mode::CUSTOM) };

        mode_selector_.setSizePercent(sel_w, sel_h);
        mode_selector_.setAnchor(0.5f, start_anchor_y);
        mode_selector_.setOptions(mode_opts);
        mode_selector_.setFont(&ctx.font);
        mode_selector_.setValue(static_cast<size_t>(ctx.settings.mode));
        mode_selector_.setCallback([&api, &ctx](size_t idx){ ctx.settings.mode = static_cast<aff::pk_high_low::controller::game_mode>(idx); api.request(ctx, aff::pk_high_low::controller::SettingsAction::UPDATE_SETTINGS); });

        pokedex_selector_.setSizePercent(sel_w, sel_h);
        pokedex_selector_.setAnchor(0.5f, start_anchor_y + sel_h + gap);
        pokedex_selector_.setOptions(pokedex_opts);
        pokedex_selector_.setFont(&ctx.font);
        pokedex_selector_.setValue(static_cast<size_t>(ctx.settings.pokedex));
        pokedex_selector_.setCallback([&api, &ctx](size_t idx){ ctx.settings.pokedex = static_cast<aff::pk_high_low::controller::pokedex_mode>(idx); api.request(ctx, aff::pk_high_low::controller::SettingsAction::UPDATE_SETTINGS); });

        difficulty_selector_.setSizePercent(sel_w, sel_h);
        difficulty_selector_.setAnchor(0.5f, start_anchor_y + 2*(sel_h + gap));
        difficulty_selector_.setOptions(diff_opts);
        difficulty_selector_.setFont(&ctx.font);
        difficulty_selector_.setValue(static_cast<size_t>(ctx.settings.difficulty));
        difficulty_selector_.setCallback([&api, &ctx](size_t idx){ ctx.settings.difficulty = static_cast<aff::pk_high_low::controller::difficulty_mode>(idx); api.request(ctx, aff::pk_high_low::controller::SettingsAction::UPDATE_SETTINGS); });
    }

    // compute/update/render selectors
    mode_selector_.computeLayout(win_w, win_h);
    pokedex_selector_.computeLayout(win_w, win_h);
    difficulty_selector_.computeLayout(win_w, win_h);

    if (ev) {
        mode_selector_.handleEvent(*ev);
        pokedex_selector_.handleEvent(*ev);
        difficulty_selector_.handleEvent(*ev);
    }

    mode_selector_.render(ctx.window->renderer());
    pokedex_selector_.render(ctx.window->renderer());
    difficulty_selector_.render(ctx.window->renderer());

    // Launch button
    // Big action button near bottom, full-widthish
    if (!is_set_up_) {
        launch_btn_.setSizePercent(0.85f, 0.12f);
        launch_btn_.setAnchor(0.5f, 0.82f);
        launch_btn_.setText("LAUNCH POKÉDEX");
        launch_btn_.setCallback([&api, &ctx](){ api.request(ctx, aff::pk_high_low::controller::SettingsAction::START_GAME); });
        launch_btn_.setFont(&ctx.font);
    }
    launch_btn_.computeLayout(win_w, win_h);
    if (ev) launch_btn_.handleEvent(*ev);
    launch_btn_.render(ctx.window->renderer());
    

    is_set_up_ = true;
    ctx.window->present();
}

} // namespace aff::pk_high_low::views
