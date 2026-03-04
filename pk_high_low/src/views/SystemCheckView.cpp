#include "views/SystemCheckView.hpp"
#include <common_ui/font.hpp>
#include <common_ui/text.hpp>

namespace aff::pk_high_low::views {

void system_check_view::tick(const SDL_Event* ev,
    aff::pk_high_low::controller::SettingsContext& ctx,
    aff::pk_high_low::controller::SettingsAPI& api)
{
    using namespace aff::sdl_utils::common;
    using namespace aff::sdl_utils::widgets;

    // Render background
    bg_.render(ctx.window->renderer());

    // Update progress based on time for demo purposes
    Uint32 t = SDL_GetTicks() % 10000; // 0..10000
    float pct = static_cast<float>(t) / 10000.0f;

    // Responsive layout: progress bar centered horizontally, 22% down from top
    progress_.setSizePercent(0.8f, 0.02f); // 80% width, 2% height
    progress_.setAnchor(0.5f, 0.65f); // anchor center horizontally, 65% down vertically
    int win_w = 0, win_h = 0; SDL_GetRendererOutputSize(ctx.window->renderer(), &win_w, &win_h);
    // compute/update/render
    progress_.computeLayout(win_w, win_h);
    progress_.setValue(pct);
    progress_.update(1.0f/60.0f);
    progress_.render(ctx.window->renderer());

    // Launch button
    // Big action button near bottom, full-widthish
    launch_btn_.setSizePercent(0.85f, 0.12f);
    launch_btn_.setAnchor(0.5f, 0.82f);
    launch_btn_.computeLayout(win_w, win_h);
    launch_btn_.setText("LAUNCH POKÉDEX");
    launch_btn_.setCallback([&api, &ctx](){ api.request(ctx, aff::pk_high_low::controller::SettingsAction::START_GAME); });
    // ensure button has a font to render text
    launch_btn_.setFont(&ctx.font);
    // draw button
    launch_btn_.render(ctx.window->renderer());

    ctx.window->present();
}

} // namespace aff::pk_high_low::views
