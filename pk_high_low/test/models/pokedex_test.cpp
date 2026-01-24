#include <gtest/gtest.h>

#include <filesystem>

#include <models/pokedex.hpp>
#include <models/poke_specie.hpp>

#include <nlohmann/json.hpp>

using aff::pk_high_low::models::poke_specie;
using aff::pk_high_low::models::pokedex;

using std::filesystem::path;

const path ASSET_PATH { std::filesystem::absolute(__FILE__).parent_path().parent_path().parent_path() / "assets"};
const path TEST_DEX_FILE { ASSET_PATH / "test_dex.json" };

TEST(Test_Pokedex, Test_FromFile) {
    pokedex dex = pokedex::from_file(TEST_DEX_FILE);
    EXPECT_EQ(dex.size(), 1219);

    auto specie = dex.find_by_id("0001");
    ASSERT_TRUE(specie.has_value());
    EXPECT_EQ(specie->name, "Bulbasaur");

    specie = dex.find_by_id("0002");
    ASSERT_TRUE(specie.has_value());
    EXPECT_EQ(specie->name, "Ivysaur");

    specie = dex.find_by_id("0003");
    ASSERT_TRUE(specie.has_value());
    EXPECT_EQ(specie->name, "Venusaur");

    specie = dex.find_by_id("0003_a");;
    ASSERT_TRUE(specie.has_value());
    EXPECT_EQ(specie->alt_name, "Mega Venusaur");
}


class Test_Pokedex_Fixture : public ::testing::Test {
protected:
    void SetUp() override {
        dex_ = pokedex::from_file(TEST_DEX_FILE);
    }
    pokedex dex_;
};

TEST_F(Test_Pokedex_Fixture, Test_FindByStat_Exact) {
    // Find at least 5 pokemon around speed 100 +/- 10
    auto results = dex_.find_by_stat([](const poke_specie& p){ return p.speed; }, 200, 0, 0);
    EXPECT_GE(results.size(), 1u);
    // verify returned items are reasonable (closest-first not guaranteed here), at least one within range
    EXPECT_TRUE(results[0].speed == 200);
    EXPECT_TRUE(results[0].name == "Regieleki");
}

TEST_F(Test_Pokedex_Fixture, Test_FindByStat_Speed) {
    // Find at least 5 pokemon around speed 100 +/- 10
    auto results = dex_.find_by_stat([](const poke_specie& p){ return p.speed; }, 100, 10, 5);
    EXPECT_GE(results.size(), 5u);
    // verify returned items are reasonable (closest-first not guaranteed here), at least one within range
    bool any_in_range = false;
    for (const auto& s : results) {
        if (s.speed >= 90 && s.speed <= 110) { any_in_range = true; break; }
    }
    EXPECT_TRUE(any_in_range);
}

TEST_F(Test_Pokedex_Fixture, Test_FindByStat_HighestAtk) {
    // Query by the higher of atk or sp_atk
    auto results = dex_.find_by_stat([](const poke_specie& p){ return std::max(p.atk, p.sp_atk); }, 150, 20, 5);
    EXPECT_GE(results.size(), 5u);
    // ensure results are sorted roughly by closeness: first element should be no farther than last
    if (results.size() >= 2) {
        auto dist0 = std::abs(std::max(results[0].atk, results[0].sp_atk) - 150);
        auto distN = std::abs(std::max(results.back().atk, results.back().sp_atk) - 150);
        EXPECT_LE(dist0, distN);
    }
}

TEST_F(Test_Pokedex_Fixture, Test_FindByCondition_Custom) {
    // Find fast and strong pokemon
    auto results = dex_.find_by_condition([](const poke_specie& p){ return p.speed > 100 && p.atk > 120; });
    // At least one expected in a full dex
    EXPECT_GE(results.size(), 1u);
}

TEST_F(Test_Pokedex_Fixture, Test_Insert_Update_Erase) {
    poke_specie temp {
        .id {"zz999"},
        .name {"Testmon"},
        .type_1 {"Normal"},
        .type_2 {"None"},
        .hp {50}, .atk {60}, .def {50}, .sp_atk {70}, .sp_def {50}, .speed {40}
    };

    // Insert
    bool inserted = dex_.insert(temp);
    EXPECT_TRUE(inserted);
    auto found = dex_.find_by_id("zz999");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->name, "Testmon");

    // Update
    temp.name = "TestmonX";
    bool updated = dex_.update("zz999", temp);
    EXPECT_TRUE(updated);
    found = dex_.find_by_id("zz999");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->name, "TestmonX");

    // Erase
    bool erased = dex_.erase("zz999");
    EXPECT_TRUE(erased);
    found = dex_.find_by_id("zz999");
    EXPECT_FALSE(found.has_value());
}


