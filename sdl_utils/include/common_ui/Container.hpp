#ifndef AFF_PK_PROJECTS_SDL_UTILS_COMMON_CONTAINER_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_COMMON_CONTAINER_HPP

#include "Widget.hpp"
#include <vector>
#include <memory>

namespace aff::sdl_utils::common {

class Container : public Widget {
public:
    Container() = default;
    ~Container() override = default;

    void addChild(std::unique_ptr<Widget> child) {
        children_.push_back(std::move(child));
    }

    void render(SDL_Renderer* renderer) override {
        for (auto& c : children_) {
            if (c->visible()) c->render(renderer);
        }
    }

    void update(float dt) override {
        for (auto& c : children_) c->update(dt);
    }

    // compute layout for this container and propagate to children
    void computeLayout(int parent_w, int parent_h) override {
        // default behavior: size/position based on parent then propagate
        Widget::computeLayout(parent_w, parent_h);
        for (auto& c : children_) c->computeLayout(w(), h());
    }

    bool handleEvent(const SDL_Event& ev) override {
        for (auto& c : children_) if (c->handleEvent(ev)) return true;
        return false;
    }

protected:
    std::vector<std::unique_ptr<Widget>> children_;

public:
    // access for layout helpers
    std::vector<std::unique_ptr<Widget>>& children() { return children_; }
};

} // namespace aff::sdl_utils::common

#endif // AFF_PK_PROJECTS_SDL_UTILS_COMMON_CONTAINER_HPP