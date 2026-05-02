#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_MODE_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_MODE_HPP

namespace aff::pk_high_low::controller {

enum class game_mode {
    BST,
    SPEED,
    ATTACK
};

// Additional configuration enums for future extensions
enum class pokedex_mode {
    STANDARD,
    GEN1_ONLY,
    VGC
};

enum class difficulty_mode {
    RANDOM,
    MATCHED,
    HARD,
    CUSTOM
};

struct game_settings {
    game_mode mode{game_mode::BST};
    pokedex_mode pokedex{pokedex_mode::STANDARD};
    difficulty_mode difficulty{difficulty_mode::RANDOM};
    bool complete_mode{false};
};

} // namespace aff::pk_high_low::controller

// Helper functions for converting enums to display strings
namespace aff::pk_high_low::controller {

inline const char* to_string(game_mode m) {
    switch (m) {
        case game_mode::BST: return "BST";
        case game_mode::SPEED: return "Speed";
        case game_mode::ATTACK: return "Attack";
    }
    return "Unknown";
}

inline const char* to_string(pokedex_mode p) {
    switch (p) {
        case pokedex_mode::STANDARD: return "Standard";
        case pokedex_mode::GEN1_ONLY: return "Gen I";
        case pokedex_mode::VGC: return "VGC";
    }
    return "Unknown";
}

inline const char* to_string(difficulty_mode d) {
    switch (d) {
        case difficulty_mode::RANDOM: return "Random";
        case difficulty_mode::MATCHED: return "Matched";
        case difficulty_mode::HARD: return "Hard";
        case difficulty_mode::CUSTOM: return "Custom";
    }
    return "Unknown";
}

} // namespace aff::pk_high_low::controller

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_MODE_HPP
