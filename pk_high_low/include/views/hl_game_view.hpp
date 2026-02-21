#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_HL_GAME_VIEW_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_HL_GAME_VIEW_HPP

#include <views/view_concepts.hpp>
#include <models/model_concepts.hpp>
#include <controller/game_choice.hpp>
#include <controller/hl_game_api.hpp>

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

#include <common_ui/window.hpp>
#include <common_ui/font.hpp>


namespace aff::pk_high_low::views {

template <aff::pk_high_low::models::Model M>
class hl_game_view {
public:
    using choice_t = aff::pk_high_low::controller::game_choice;
    using Window = aff::sdl_utils::common::Window;

private:
  // No members for now
  std::unordered_map<std::string, SDL_Texture*> tex_cache_;

public:
    // Rule of 0

    // The key function, commented as dev forward declaration for the controller to call
    // void tick(const SDL_Event* ev, controller::HLGameContext<M>& ctx, controller::HLGameAPI& api);

    // Satisfy the View concept
    void show(const std::string& message) {}
    std::string prompt(const std::string& message) { return ""; }
    void show_round(const M& model) {}
    choice_t get_player_choice(const M& model) { return choice_t::QUIT_GAME; }
    void show_round_results(const M& model, bool player_won) {}

private:
    // Commented as dev forward declaration for the controller to call
    /*
    void render_model(const M& model, bool show_results);
    void handle_event_current_round(const SDL_Event* ev, 
                                    controller::HLGameContext<M>& ctx, 
                                    controller::HLGameAPI& api);
    void handle_event_lost_round(const SDL_Event* ev, 
                                 controller::HLGameContext<M>& ctx, 
                                 controller::HLGameAPI& api);
    */

public: 

  void tick(const SDL_Event* ev, 
           controller::HLGameContext& ctx, 
           controller::HLGameAPI& api,
           const M& model) {
   if (ev == nullptr) {
      // No event, just render the current state
      bool reveal_score {ctx.game_state == controller::HLGameState::SHOWING_LOST_ROUND};
      render_model(ctx, model, reveal_score);
      return;
   }

   // handle change of size
   if (ev->type == SDL_WINDOWEVENT) {
    if (ev->window.event == SDL_WINDOWEVENT_RESIZED || ev->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
      // Re-render to adapt to new size
      render_model(ctx, model, false);
      return;
    }
   }

   // General stuff done,
   // now do the state-specific handling
   switch (ctx.game_state) {
    case controller::HLGameState::NOT_STARTED:
      // Do nothing
      break;
    case controller::HLGameState::SHOWING_CURRENT_ROUND:
      // Handle input for current round (e.g., player choice)
      handle_event_current_round(ev, ctx, api);
      break;
    case controller::HLGameState::SHOWING_LOST_ROUND:
      // Handle input for lost round (e.g., wait for replay or quit)
      handle_event_lost_round(ev, ctx, api);
      break;
    default:
      break;
   }

   int i = 0;

}

private:
  void render_model(controller::HLGameContext& ctx, const M& model, bool reveal_score) {
    SDL_SetRenderDrawColor(ctx.window->renderer(), 20, 20, 20, 255);
    SDL_RenderClear(ctx.window->renderer());

    auto items = model.items();
    auto scores = model.itemScores();

    int w, h;
    SDL_GetWindowSize(ctx.window->raw(), &w, &h);
    bool vertical_split = (w >= h);

    SDL_Rect left_rect, right_rect;
    if (vertical_split) {
      left_rect = {0, 0, w / 2, h};
      right_rect = {w / 2, 0, w - w / 2, h};
    } else {
      left_rect = {0, 0, w, h / 2};
      right_rect = {0, h / 2, w, h - h / 2};
    }

    render_item_in_rect(items.first, scores.first, ctx, left_rect, reveal_score);
    render_item_in_rect(items.second, scores.second, ctx, right_rect, reveal_score);

    SDL_RenderPresent(ctx.window->renderer());
  }

  void render_item_in_rect(const typename M::item_t &item,
                           const typename M::item_score_t &score,
                           controller::HLGameContext& ctx,
                           const SDL_Rect &rect,
                           bool reveal_score) {
    // background
    SDL_SetRenderDrawColor(ctx.window->renderer(), 35, 35, 35, 255);
    SDL_Rect bg = rect;
    SDL_RenderFillRect(ctx.window->renderer(), &bg);

    // image
    SDL_Texture *tex = load_texture_for_id(ctx, item.first.id);
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
      SDL_Rect dst{rect.x + (rect.w - dst_w) / 2, rect.y + 10, dst_w, dst_h};
      SDL_RenderCopy(ctx.window->renderer(), tex, nullptr, &dst);
    }

    // text (name and optional score)
    // Determine whether the score should be shown: either globally requested
    // via reveal_score or the item itself is marked revealed (item.second).
    bool show_score = reveal_score || item.second;

    render_text_at(ctx, item.first.name + (item.first.is_alt_form() ? (" (" + item.first.alt_name + ")") : ""),
                   rect.x + 10, rect.y + rect.h - 50, rect.w - 20);
    if (show_score) {
      render_text_at(ctx, std::string("Score: ") + std::to_string(score),
                     rect.x + 10, rect.y + rect.h - 25, rect.w - 20);
    }
  }

  void handle_event_current_round(const SDL_Event* ev, controller::HLGameContext& ctx, controller::HLGameAPI& api) {
    // If the key pressed is 1, 2, or t, 
    // we interpret that as a choice for the current round.
    if (ev->type == SDL_KEYDOWN) {
      switch (ev->key.keysym.sym) {
        case SDLK_1:
          ctx.last_player_choice = controller::game_choice::FIRST_CHOSEN;
          api.request(ctx, controller::HLGameAction::MAKE_CHOICE);
          break;
        case SDLK_2:
          ctx.last_player_choice = controller::game_choice::SECOND_CHOSEN;
          api.request(ctx, controller::HLGameAction::MAKE_CHOICE);
          break;
        case SDLK_t:
          ctx.last_player_choice = controller::game_choice::TIE_CHOSEN;
          api.request(ctx, controller::HLGameAction::MAKE_CHOICE);
          break;
        case SDLK_q:
          ctx.last_player_choice = controller::game_choice::QUIT_GAME;
          api.request(ctx, controller::HLGameAction::QUIT);
          break;
        default:
          break;
      }
    }

    // If the mouse is clicked, we can also interpret that as a choice.
    if (ev->type == SDL_MOUSEBUTTONDOWN) {
      int w, h;
      SDL_GetWindowSize(ctx.window->raw(), &w, &h);
      bool vertical_split = (w >= h);
      int x = ev->button.x;
      int y = ev->button.y;
      if (vertical_split) {
        if (x < w / 2) {
          ctx.last_player_choice = controller::game_choice::FIRST_CHOSEN;
          api.request(ctx, controller::HLGameAction::MAKE_CHOICE);
        } else {
          ctx.last_player_choice = controller::game_choice::SECOND_CHOSEN;
          api.request(ctx, controller::HLGameAction::MAKE_CHOICE);
        }
      } else {
        if (y < h / 2) {  
          ctx.last_player_choice = controller::game_choice::FIRST_CHOSEN;
          api.request(ctx, controller::HLGameAction::MAKE_CHOICE);
        } else {
          ctx.last_player_choice = controller::game_choice::SECOND_CHOSEN;
          api.request(ctx, controller::HLGameAction::MAKE_CHOICE);
        }
      }
    }
  }

  void handle_event_lost_round(const SDL_Event* ev, controller::HLGameContext& ctx, controller::HLGameAPI& api) {
    if (ev->type == SDL_KEYDOWN || ev->type == SDL_MOUSEBUTTONDOWN) {
      // For simplicity, any key press or mouse click will trigger exit.
      api.request(ctx, controller::HLGameAction::QUIT);
    }

  }

    void render_text_at(controller::HLGameContext& ctx, const std::string& txt, int x, int y, int max_w) {
        if (!ctx.font.raw()) return;
        SDL_Color col { 255, 255, 255, 255 };
        SDL_Surface* surf = TTF_RenderUTF8_Blended_Wrapped(ctx.font.raw(), txt.c_str(), col, max_w);
        if (!surf) return;
        SDL_Texture* tex = SDL_CreateTextureFromSurface(ctx.window->renderer(), surf);
        SDL_Rect dst { x, y, surf->w, surf->h };
        SDL_FreeSurface(surf);
        if (!tex) return;
        SDL_RenderCopy(ctx.window->renderer(), tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
    }

    // Render wrapped text centered horizontally at the given y position.
    void render_text_centered(controller::HLGameContext& ctx, const std::string& txt, int y, int max_w) {
       if (!ctx.font.raw()) return;
            SDL_Color col { 255, 255, 255, 255 };
            SDL_Surface* surf = TTF_RenderUTF8_Blended_Wrapped(ctx.font.raw(), txt.c_str(), col, max_w);
            if (!surf) return;
            SDL_Texture* tex = SDL_CreateTextureFromSurface(ctx.window->renderer(), surf);
            int surf_w = surf->w;
            int surf_h = surf->h;
            SDL_FreeSurface(surf);
            if (!tex) return;
            int win_w, win_h;
            SDL_GetWindowSize(ctx.window->raw(), &win_w, &win_h);
            SDL_Rect dst { (win_w - surf_w) / 2, y, surf_w, surf_h };
            SDL_RenderCopy(ctx.window->renderer(), tex, nullptr, &dst);
            SDL_DestroyTexture(tex);
    }

    SDL_Texture* load_texture_for_id(controller::HLGameContext& ctx, const std::string& id) {
        auto it = tex_cache_.find(id);
        if (it != tex_cache_.end()) return it->second;

        std::filesystem::path img_path = ctx.assets_path / "images" / (id + ".jpg");
        if (!std::filesystem::exists(img_path)) {
            // try png
            img_path = ctx.assets_path / "images" / (id + ".png");
            if (!std::filesystem::exists(img_path)) return nullptr;
        }

        SDL_Surface* surf = IMG_Load(img_path.string().c_str());
        if (!surf) {
            std::cerr << "IMG_Load failed for " << img_path << ": " << IMG_GetError() << std::endl;
            return nullptr;
        }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(ctx.window->renderer(), surf);
        SDL_FreeSurface(surf);
        if (!tex) {
            std::cerr << "CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
            return nullptr;
        }
        tex_cache_.emplace(id, tex);
        return tex;
    }

}; // class hl_game_view

} // namespace aff::pk_high_low::views

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_HL_GAME_VIEW_HPP