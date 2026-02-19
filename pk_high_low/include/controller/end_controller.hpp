#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_END_CONTROLLER_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_END_CONTROLLER_HPP

#include <filesystem>
#include <common_ui/window.hpp>
#include <common_ui/font.hpp>
#include <controller/controller_api.hpp>

namespace aff::pk_high_low::controller {

class EndController {
public:
    EndController(aff::sdl_utils::common::Window& window, const std::filesystem::path& assets_path);
    void tick(const SDL_Event* ev, AppContext& ctx, AppAPI& api);
    void reset();
private:
    aff::sdl_utils::common::Window* window_;
    std::filesystem::path assets_path_;
    aff::sdl_utils::common::Font font_;
};

} // namespace

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_END_CONTROLLER_HPP
