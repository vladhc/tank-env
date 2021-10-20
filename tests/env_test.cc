#include <iostream>
#include <gtest/gtest.h>
#include "tank.h"
#include "env.h"

TEST(EnvTest, EpisodeCompleteWhenSecondTeamIsDead) {
  // GIVEN
  Env env;
  for (auto tank : env.GetTanks()) {
    if (tank->GetTeamId() == 1) {
      int hp = tank->GetHitpoints();
      env.DamageTank(tank->GetId(), hp);
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
    for (auto tank : env.GetTanks()) {
      if (tank->GetTeamId() == teamId) {
        int hp = tank->GetHitpoints();
        env.DamageTank(tank->GetId(), hp);
        break;
      }
    }
  }

  // WHEN
  bool done = env.EpisodeComplete();

  // THEN
  EXPECT_FALSE(done);
}

TEST(EnvTest, DeadTankIsReturnedOnce) {
  // GIVEN
  Env env;
  // Kill 1 tank
  const int tankId = 0;
  for (auto tank : env.GetTanks()) {
    if (tank->GetId() == tankId) {
      int hp = tank->GetHitpoints();
      env.DamageTank(tank->GetId(), hp);
      break;
    }
  }
  std::map<int, Action> actions;

  // WHEN
  auto step1 = env.Step(actions);
  auto step2 = env.Step(actions);

  // THEN
  bool tankExported = false;
  for (Observation obs : std::get<0>(step1)) {
    if (obs.heroId == tankId) {
      tankExported = true;
      break;
    }
  }
  EXPECT_TRUE(tankExported);

  tankExported = false;
  for (Observation obs : std::get<0>(step2)) {
    if (obs.heroId == tankId) {
      tankExported = true;
      break;
    }
  }
  EXPECT_FALSE(tankExported);
}

TEST(EnvTest, DeadTankIsDoneAndPunished) {
  // GIVEN
  Env env;
  // Kill 1 tank of each team
  // int teamIds[] = {0, 1};
  int tankId = 0;
  for (auto tank : env.GetTanks()) {
    if (tank->GetId() == tankId) {
      int hp = tank->GetHitpoints();
      env.DamageTank(tank->GetId(), hp);
      break;
    }
  }
  std::map<int, Action> actions;

  // WHEN
  auto step = env.Step(actions);

  // THEN
  auto observations = std::get<0>(step);
  auto rewards = std::get<1>(step);
  auto dones = std::get<2>(step);

  float reward = 0;
  bool done = false;

  for (int i=0; i < observations.size(); i++) {
    Observation obs = observations[i];
    if (obs.heroId == tankId) {
      reward = rewards[i];
      done = dones[i];
      break;
    }
  }

  ASSERT_TRUE(reward < 0);
  ASSERT_TRUE(done);
}

TEST(EnvTest, ResetRemovesBullets) {
  // GIVEN
  Env env;
  std::map<int, Action> actions;
  for (auto tank : env.GetTanks()) {
    actions[tank->GetId()] = Action{0, 0, true};
  }
  env.Step(actions);

  // WHEN
  env.Reset();

  // THEN
  ASSERT_TRUE(env.GetBullets().empty());
}
