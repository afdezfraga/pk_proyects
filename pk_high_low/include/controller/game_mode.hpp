#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_MODE_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_MODE_HPP

namespace aff::pk_high_low::controller {

enum class game_mode {
    BTS,
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
    game_mode mode{game_mode::BTS};
    pokedex_mode pokedex{pokedex_mode::STANDARD};
    difficulty_mode difficulty{difficulty_mode::RANDOM};
    bool complete_mode{false};
};

} // namespace aff::pk_high_low::controller

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_GAME_MODE_HPP
