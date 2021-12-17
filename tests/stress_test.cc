#include <random>
#include <functional>
#include <gtest/gtest.h>
#include "env.h"

TEST(EnvTest, DoesntExportBulletsOutsideOfArena) {
  // GIVEN
  Env env{10, 32};
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
      for (auto tank : tanks) {
        actions[tank->GetId()] = Action{rand(), rand(), rand(), true};
      }

      // WHEN
      auto step = env.Step(actions);

      // THEN
      unsigned int aliveTanksCount = 0;
      for (auto tank : tanks) {
        if (tank->IsAlive()) {
          aliveTanksCount++;
        }
      }
      done = aliveTanksCount <= halfOfTanks;
    } while(!done);
  }
}
