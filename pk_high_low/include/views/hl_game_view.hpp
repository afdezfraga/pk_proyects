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

    render_item_in_rect_fancy("1", items.first, scores.first, ctx, left_rect, reveal_score);
    render_item_in_rect_fancy("2", items.second, scores.second, ctx, right_rect, reveal_score);

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

  // A fancier renderer that draws a framed card with shadow, cyan frame,
  // score badge and centered name — inspired by the provided mockup.
  void render_item_in_rect_fancy(const std::string& key,
                                 const typename M::item_t &item,
                                 const typename M::item_score_t &score,
                                 controller::HLGameContext& ctx,
                                 const SDL_Rect &rect,
                                 bool reveal_score) {
    // enable blending for semi-transparent shapes
    SDL_BlendMode old_mode;
    SDL_GetRenderDrawBlendMode(ctx.window->renderer(), &old_mode); // query current mode
    SDL_SetRenderDrawBlendMode(ctx.window->renderer(), SDL_BLENDMODE_BLEND);

    // Drop shadow (slightly offset dark rectangle)
    SDL_Rect shadow = rect;
    shadow.x += 6; shadow.y += 6;
    SDL_SetRenderDrawColor(ctx.window->renderer(), 0, 0, 0, 120);
    SDL_RenderFillRect(ctx.window->renderer(), &shadow);

    // Card background (dark, slightly inset)
    SDL_Rect card = rect;
    card.x += 6; card.y += 6; card.w -= 12; card.h -= 12;
    SDL_SetRenderDrawColor(ctx.window->renderer(), 28, 30, 33, 220);
    SDL_RenderFillRect(ctx.window->renderer(), &card);

    // Inner darker panel for the image area
    SDL_Rect image_panel = card;
    image_panel.x += 12; image_panel.y += 12;
    image_panel.w -= 24; image_panel.h = (int)(card.h * 0.62f);
    SDL_SetRenderDrawColor(ctx.window->renderer(), 24, 26, 28, 200);
    SDL_RenderFillRect(ctx.window->renderer(), &image_panel);

    // Cyan frame glow: draw a thin bright outline and an outer darker stroke
    SDL_Rect frame = card;
    SDL_SetRenderDrawColor(ctx.window->renderer(), 12, 160, 160, 200);
    SDL_RenderDrawRect(ctx.window->renderer(), &frame);
    // additional subtle outer stroke
    SDL_SetRenderDrawColor(ctx.window->renderer(), 6, 80, 80, 80);
    SDL_Rect outer = { frame.x - 2, frame.y - 2, frame.w + 4, frame.h + 4 };
    SDL_RenderDrawRect(ctx.window->renderer(), &outer);

    // Load and draw image centered in image_panel, preserving aspect ratio
    SDL_Texture *tex = load_texture_for_id(ctx, item.first.id);
    if (tex) {
      int tw, th;
      SDL_QueryTexture(tex, nullptr, nullptr, &tw, &th);
      int padding = 8;
      int avail_w = image_panel.w - padding * 2;
      int avail_h = image_panel.h - padding * 2;
      float img_aspect = (float)tw / (float)th;
      int dst_w = avail_w;
      int dst_h = (int)(dst_w / img_aspect);
      if (dst_h > avail_h) {
        dst_h = avail_h;
        dst_w = (int)(dst_h * img_aspect);
      }
      SDL_Rect dst{ image_panel.x + (image_panel.w - dst_w) / 2,
                    image_panel.y + (image_panel.h - dst_h) / 2,
                    dst_w, dst_h };

      // subtle vignette: draw a semi-transparent dark overlay around edges
      SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
      SDL_RenderCopy(ctx.window->renderer(), tex, nullptr, &dst);

      // optional small inner highlight (a translucent white rect near top)
      SDL_SetRenderDrawColor(ctx.window->renderer(), 255, 255, 255, 18);
      SDL_Rect highlight = dst;
      highlight.h = (int)(dst.h * 0.18f);
      SDL_RenderFillRect(ctx.window->renderer(), &highlight);
    }

    // Name area: centered text near bottom of the card
    int name_y = card.y + card.h - 56;
    std::string name_text = item.first.name + (item.first.is_alt_form() ? (" (" + item.first.alt_name + ")") : "");
    render_text_centered_in_rect(ctx, name_text, name_y, card);

    // small indicator dots under the name (like in mockup)
    int dots_y = name_y + 28;
    int center_x = card.x + card.w / 2;
    int dot_r = 4;
    SDL_SetRenderDrawColor(ctx.window->renderer(), 12, 160, 160, 200);
    for (int i = -1; i <= 1; ++i) {
      SDL_Rect d{ center_x + i * 14 - dot_r, dots_y - dot_r, dot_r * 2, dot_r * 2 };
      SDL_RenderFillRect(ctx.window->renderer(), &d);
    }

    // Score badge at top-right of the card
    int badge_w = 74;
    int badge_h = 48;
    SDL_Rect badge{ card.x + card.w - badge_w - 12, card.y + 12, badge_w, badge_h };
    // badge background (semi-transparent dark circle-like rect)
    SDL_SetRenderDrawColor(ctx.window->renderer(), 6, 80, 80, 160);
    SDL_RenderFillRect(ctx.window->renderer(), &badge);
    // badge outline
    SDL_SetRenderDrawColor(ctx.window->renderer(), 12, 160, 160, 220);
    SDL_RenderDrawRect(ctx.window->renderer(), &badge);

    // Badge text (score or ???)
    std::string badge_top = reveal_score || item.second ? std::to_string(score) : "???";
    //std::string badge_bot = "SPD";
    // center top text in badge
    render_text_centered_in_rect(ctx, badge_top, std::nullopt, badge);
    //render_text_centered_in_rect(ctx, badge_bot, badge.y + 24, badge);

    // If score should be highlighted (revealed), add a faint glow behind badge
    if (reveal_score || item.second) {
      SDL_SetRenderDrawColor(ctx.window->renderer(), 12, 160, 160, 40);
      SDL_Rect glow = badge; glow.x -= 6; glow.y -= 6; glow.w += 12; glow.h += 12;
      SDL_RenderFillRect(ctx.window->renderer(), &glow);
    }

    // Finally, small bottom-left action circle (decorative)
    SDL_Rect action{ card.x + 12, card.y + card.h - 36, 28, 28 };
    SDL_SetRenderDrawColor(ctx.window->renderer(), 12, 160, 160, 60);
    SDL_RenderFillRect(ctx.window->renderer(), &action);
    SDL_SetRenderDrawBlendMode(ctx.window->renderer(), old_mode); // restore previous blend mode

    // Render the key (1 or 2) at top-left corner of the card for player reference
    std::string key_text = key;
    render_text_centered_in_rect(ctx, key_text, std::nullopt, action);
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
    void render_text_centered_in_rect(controller::HLGameContext& ctx, const std::string& txt, std::optional<int> y, SDL_Rect rect) {
        if (!ctx.font.raw()) return;
              SDL_Color col { 255, 255, 255, 255 };
              SDL_Surface* surf = TTF_RenderUTF8_Blended_Wrapped(ctx.font.raw(), txt.c_str(), col, rect.w);
              if (!surf) return;
              SDL_Texture* tex = SDL_CreateTextureFromSurface(ctx.window->renderer(), surf);
              int surf_w = surf->w;
              int surf_h = surf->h;
              SDL_FreeSurface(surf);
              if (!tex) return;
              SDL_Rect dst { rect.x + (rect.w - surf_w) / 2, y.value_or(rect.y + (rect.h - surf_h) / 2), surf_w, surf_h };
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