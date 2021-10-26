#include <random>
#include <functional>
#include <gtest/gtest.h>
#include "env.h"

TEST(EnvTest, DoesntExportBulletsOutsideOfArena) {
  // GIVEN
  Env env{10};
  const unsigned int halfOfTanks = 5;
  const float arenaSize = env.GetArenaSize();
  const auto tanks = env.GetTanks();

  std::mt19937 randomEngine;
  auto rand = std::bind(
      std::uniform_real_distribution<float> {-1, 1},
      randomEngine
  );

  for (int episode=0; episode < 100; episode++) {
    env.Reset();
    bool done = false;
    do {
      std::map<int, Action> actions;
      for (tank : tanks) {
        actions[tank->GetId()] = Action{rand(), rand(), rand(), true};
      }

      // WHEN
      std::tuple<
        std::vector<Observation>,
        std::vector<float>,
        std::vector<char>
      > step = env.Step(actions);

      // THEN
      auto obs = std::get<0>(step);

      for (const Observation ob : obs) {
        for (const Bullet* bullet : ob.bullets) {
          const auto pos = bullet->GetPosition();
          ASSERT_TRUE(pos.x <= arenaSize);
          ASSERT_TRUE(pos.x >= -arenaSize);
          ASSERT_TRUE(pos.y <= arenaSize);
          ASSERT_TRUE(pos.y >= -arenaSize);
        }
      }

      unsigned int aliveTanksCount = 0;
      for (tank : tanks) {
        if (tank->IsAlive()) {
          aliveTanksCount++;
        }
      }
      done = aliveTanksCount <= halfOfTanks;
    } while(!done);
  }
}
