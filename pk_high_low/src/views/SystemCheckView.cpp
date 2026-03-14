#include "views/SystemCheckView.hpp"
#include <common_ui/font.hpp>
#include <common_ui/text.hpp>
#include <common_ui/Theme.hpp>
#include <filesystem>

namespace aff::pk_high_low::views {

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
        if (!is_set_up_ || title_needs_update_) {
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
                title_font_.emplace(fp, 72);
            }

            const aff::sdl_utils::common::Font& tf = *title_font_;
            title_left_tex_ = aff::sdl_utils::common::Text::render(tf, "StatDex", SDL_Color{255,255,255,255}, ren);
            title_right_tex_ = aff::sdl_utils::common::Text::render(tf, ".Check", theme.primary, ren);
            int w1=0,h1=0,w2=0,h2=0; if (title_left_tex_.raw()) SDL_QueryTexture(title_left_tex_.raw(), nullptr, nullptr, &w1, &h1);
            if (title_right_tex_.raw()) SDL_QueryTexture(title_right_tex_.raw(), nullptr, nullptr, &w2, &h2);
            int total_w = w1 + w2;
            int win_w = 0, win_h = 0; SDL_GetRendererOutputSize(ren, &win_w, &win_h);
            int x = (win_w - total_w) / 2;
            int y = 20;
            title_left_rect_ = { x, y, w1, h1 };
            title_right_rect_ = { x + w1, y, w2, h2 };
            title_needs_update_ = false;
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
        title_needs_update_ = true;
    } else {
        eye_.update(dt);
        eye_.render_animated(ctx.window->renderer());
    }


    // Responsive layout: progress bar centered horizontally, 65% down from top
    progress_.setSizePercent(0.8f, 0.02f); // 80% width, 2% height
    progress_.setAnchor(0.5f, 0.65f); // anchor center horizontally, 65% down vertically
    // compute/update/render
    progress_.computeLayout(win_w, win_h);
    progress_.setValue(pct);
    progress_.update(dt);
    progress_.render(ctx.window->renderer());

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
