#include "controller/hl_game_controller.hpp"

#include <SDL.h>
#include <cmath>
#include <common_ui/text.hpp>

#include <controller/game_mode.hpp>

using namespace aff::sdl_utils::common;
using namespace aff::pk_high_low::controller;

static std::string find_font_hl_game(const std::filesystem::path& assets_path) {
    std::vector<std::filesystem::path> candidates {
      assets_path / "fonts" / "Roboto-Black.ttf",
      assets_path / "fonts" / "DejaVuSans.ttf",
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    };
    for (const auto &p : candidates) if (std::filesystem::exists(p)) return p.string();
    return std::string();
}


HLGameController::HLGameController(Window& window, const std::filesystem::path& assets_path)
    : window_(&window), 
      assets_path_(assets_path), 
      font_((find_font_hl_game(assets_path_).empty() 
                ? std::string() 
                : find_font_hl_game(assets_path_)), 
            24)
{}

void HLGameController::configure(const game_settings& settings)
{
    // TMP ignore config and just use default BST game mode for now

    // Pokedex
    model_utils_.dex_ = hl_model_utils_t::dex_t::from_file(assets_path_ / "dex.json");

    // RNG utils
    model_utils_.rng_utils_.rng = std::mt19937(std::random_device{}());
    model_utils_.rng_utils_.dist_dex = 
            std::uniform_int_distribution<std::mt19937::result_type>(0, 
                                                                     model_utils_.dex_.size() - 1);

    // Index stuff
    model_utils_.previous_indices_ = { static_cast<hl_model_utils_t::dex_t::index_t>(-1), 
                                       static_cast<hl_model_utils_t::dex_t::index_t>(-1) };
    model_utils_.replace_index_ = 0;

    // Game model functions
    switch (settings.mode) {
        case game_mode::SPEED:
            model_utils_.get_bst_ = [](const hl_model_utils_t::item_t& item) -> hl_model_utils_t::item_score_t {
                return item.first.speed;
            };
            break;
        case game_mode::ATTACK:
            model_utils_.get_bst_ = [](const hl_model_utils_t::item_t& item) -> hl_model_utils_t::item_score_t {
                return std::max(item.first.atk, item.first.sp_atk);
            };
            break;
        case game_mode::BST:
        default:
            model_utils_.get_bst_ = [](const hl_model_utils_t::item_t& item) -> hl_model_utils_t::item_score_t {
                return item.first.bst();
            };
            break;
    }
    
    switch (settings.difficulty) {
        // TO DO: Implement smarter index selection for MATCHED, HARD and CUSTOM difficulties
        case difficulty_mode::CUSTOM:
        case difficulty_mode::HARD:
        case difficulty_mode::MATCHED:

        // Random is default for now
        case difficulty_mode::RANDOM:
        default:
            model_utils_.next_different_index_ = [&](hl_model_utils_t::dex_t::index_t index1, 
                                                    hl_model_utils_t::dex_t::index_t index2) {
                hl_model_utils_t::dex_t::index_t next_index = index1;
                while (next_index == index1 || next_index == index2) {
                    next_index = model_utils_.rng_utils_.dist_dex(model_utils_.rng_utils_.rng);
                }
                return next_index;
            };
            break;
    }

    model_utils_.item_update_ = [&](std::pair<hl_model_utils_t::item_t, hl_model_utils_t::item_t>& items){

        // Update visibility of current items
        // Only if its not the first round
        bool reveal = items.first.first.id != hl_model_utils_t::item_t().first.id 
                      && items.second.first.id != hl_model_utils_t::item_t().first.id;
        items.first.second = reveal;
        items.second.second = reveal;

        // Get a new specie for the current index
        auto new_index = model_utils_.next_different_index_(model_utils_.previous_indices_[0], 
                                                            model_utils_.previous_indices_[1]);
        model_utils_.previous_indices_[model_utils_.replace_index_] = new_index;
        if (model_utils_.replace_index_ == 0) {
            items.second = hl_model_utils_t::item_t { model_utils_.dex_.at(new_index), false };
        } else { // replace_index == 1
            items.first = hl_model_utils_t::item_t { model_utils_.dex_.at(new_index), false };
        }
        model_utils_.replace_index_ ^= 1; // alternate between 0 and 1
    };
    model_utils_.auto_win_ties_ = [](const std::pair<hl_model_utils_t::item_score_t,
                                                     hl_model_utils_t::item_score_t>& item_scores, 
                                     const aff::pk_high_low::controller::game_choice& choice) -> bool {
        switch (choice) {
            case aff::pk_high_low::controller::game_choice::FIRST_CHOSEN:
                return item_scores.first >= item_scores.second;
            case aff::pk_high_low::controller::game_choice::SECOND_CHOSEN:
                return item_scores.second >= item_scores.first;
            case aff::pk_high_low::controller::game_choice::TIE_CHOSEN:
                return item_scores.first == item_scores.second;
            default:
                return false;
        }
    };

    // --------------------------------------------------------------------------------------------
    // All utils configured
    // Now lets make the model, view and controller for the game logic
    // --------------------------------------------------------------------------------------------

    hl_model_utils_t::model_t model = hl_model_utils_t::model_t(
        std::move(model_utils_.get_bst_),
        std::move(model_utils_.item_update_),
        std::move(model_utils_.auto_win_ties_)
    );
    hl_model_utils_t::view_t view {};

    hl_controller_ = hl_model_utils_t::controller_t(std::move(model), 
                                        std::move(view), 
                                        window_, 
                                        assets_path_);
}

void HLGameController::reset()
{
    // hl_controller_.reset();
}

void HLGameController::restart()
{
    // TO DO
}

void HLGameController::tick(const SDL_Event* ev, AppContext& ctx, AppAPI& api)
{
    if (hl_controller_) {
        hl_controller_->tick(ev, ctx, api);
    }
}
