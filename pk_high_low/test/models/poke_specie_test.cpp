#include <gtest/gtest.h>

#include <models/poke_specie.hpp>

#include <nlohmann/json.hpp>

using aff::pk_high_low::models::poke_specie;

TEST(Test_PokeSpecie, Test_IsAltForm) {
    poke_specie ps1 {
      .id {"0003"},
      .name {"Venusaur"},
      .type_1 {"Grass"},
      .type_2 {"Poison"},
      .hp {80},
      .atk {82},
      .def {83},
      .sp_atk {100},
      .sp_def {100},
      .speed {80}
    };
    EXPECT_FALSE(ps1.is_alt_form());

    poke_specie ps2 {
      .id {"0003_a"},
      .name {"Venusaur"},
      .alt_name {"Mega Venusaur"},
      .type_1 {"Grass"},
      .type_2 {"Poison"},
      .hp {80},
      .atk {100},
      .def {123},
      .sp_atk {122},
      .sp_def {120},
      .speed {80}
    };
    EXPECT_TRUE(ps2.is_alt_form());
}

TEST(Test_PokeSpecie, Test_BST) {
    poke_specie ps {
      .id {"0001"},
      .name {"Bulbasaur"},
      .type_1 {"Grass"},
      .type_2 {"Poison"},
      .hp {45},
      .atk {49},
      .def {49},
      .sp_atk {65},
      .sp_def {65},
      .speed {45}
    };
    EXPECT_EQ(ps.bst(), 318);
}

TEST(Test_PokeSpecie, Test_NumID) {
    poke_specie ps1 {
      .id { "0003" },
      .name { "Venusaur" },
      .type_1 { "Grass" },
      .type_2 { "Poison" },
      .hp { 80 },
      .atk { 82 },
      .def { 83 },
      .sp_atk { 100 },
      .sp_def { 100 },
      .speed { 80 }
    };
    EXPECT_EQ(ps1.num_id(), 3);

    poke_specie ps2 {
      .id { "0003_a" },
      .name { "Venusaur" },
      .alt_name { "Mega Venusaur" },
      .type_1 { "Grass" },
      .type_2 { "Poison" },
      .hp { 80 },
      .atk { 100 },
      .def { 123 },
      .sp_atk { 122 },
      .sp_def { 120 },
      .speed { 80 }
    };
    EXPECT_EQ(ps2.num_id(), 3);
}

TEST(Test_PokeSpecie, Test_FromJson) {

    nlohmann::json j_item = {
        {"id", "0001"},
        {"name", "Bulbasaur"},
        {"types", {"Grass", "Poison"}},
        {"stats", {
            {"hp", 45},
            {"attack", 49},
            {"defense", 49},
            {"sp.atk", 65},
            {"sp.def", 65},
            {"speed", 45}
        }}
    };

    poke_specie ps = poke_specie::from_json(j_item);

    EXPECT_EQ(ps.id, "0001");
    EXPECT_EQ(ps.name, "Bulbasaur");
    EXPECT_EQ(ps.type_1, "Grass");
    EXPECT_EQ(ps.type_2, "Poison");
    EXPECT_EQ(ps.hp, 45);
    EXPECT_EQ(ps.atk, 49);
    EXPECT_EQ(ps.def, 49);
    EXPECT_EQ(ps.sp_atk, 65);
    EXPECT_EQ(ps.sp_def, 65);
    EXPECT_EQ(ps.speed, 45);
}
