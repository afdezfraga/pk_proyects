#ifndef AFF_PK_PROJECTS_SDL_UTILS_COMMON_VBOX_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_COMMON_VBOX_HPP

#include "Container.hpp"

namespace aff::sdl_utils::common {

class VBox : public Container {
public:
    VBox() = default;
    ~VBox() override = default;

    void setSpacing(int s) { spacing_ = s; }
    void setPadding(int p) { padding_ = p; }

    // compute layout for this container and arrange children vertically
    void computeLayout(int parent_w, int parent_h) override;

private:
    int spacing_ = 8;
    int padding_ = 8;
};

} // namespace aff::sdl_utils::common

#endif // AFF_PK_PROJECTS_SDL_UTILS_COMMON_VBOX_HPP
