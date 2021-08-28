#include <iostream>
#include <gtest/gtest.h>
#include "tank.h"
#include "env.h"

TEST(EnvTest, EpisodeCompleteWhenSecondTeamIsDead) {
  // GIVEN
  Env env;
  for (Tank* tank : env.GetTanks()) {
    if (tank->GetTeamId() == 1) {
      int hp = tank->GetHitpoints();
      tank->TakeDamage(hp);
    }
  }

  // WHEN
  bool done = env.EpisodeComplete();

  // THEN
  EXPECT_TRUE(done);
}

TEST(EnvTest, EpisodeNoCompleteWhenTeamsAlive) {
  // GIVEN
  Env env;
  // Kill 1 tank of each team
  // int teamIds[] = {0, 1};
  for (int teamId : {0, 1}) {
    for (Tank* tank : env.GetTanks()) {
      if (tank->GetTeamId() == teamId) {
        int hp = tank->GetHitpoints();
        tank->TakeDamage(hp);
        break;
      }
    }
  }

  // WHEN
  bool done = env.EpisodeComplete();

  // THEN
  EXPECT_FALSE(done);
}
