#include <tuple>
#include <vector>
#include <iostream>
#include <functional>
#include <gtest/gtest.h>
#include "tank.h"
#include "env.h"

TEST(EnvTest, EpisodeCompleteWhenSecondTeamIsDead) {
  // GIVEN
  Env env{10};
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
  Env env{10};
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
  Env env{10};
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
  Env env{10};
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
  Env env{10};
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

TEST(EnvTest, RandomplyPlacesTanksOnTheArena) {
  // GIVEN
  Env env{10};
  auto tanks = env.GetTanks();
  env.Reset();
  std::vector<float> angles(tanks.size());
  std::vector<b2Vec2> positions(tanks.size());

  for (int i=0; i < tanks.size(); i++) {
    auto tank = tanks[i];
    angles[i] = tank->GetAngle();
    positions[i] = tank->GetPosition();
  }

  // WHEN
  env.Reset();

  // THEN
  for (int i=0; i < tanks.size(); i++) {
    auto tank = tanks[i];
    EXPECT_TRUE(tank->GetAngle() != angles[i]);
    EXPECT_TRUE(tank->GetPosition() != positions[i]);
  }
}

TEST(EnvTest, ResetDoesntMakeTanksOverlap) {
  // GIVEN
  Env env{10};
  const float arenaSize = env.GetArenaSize();
  auto tanks = env.GetTanks();

  for (int iteration=0; iteration < 1000; iteration++) {

    // WHEN
    env.Reset();

    // THEN
    for (int i=0; i < tanks.size(); i++) {
      auto posA = tanks[i]->GetPosition();
      float sizeA = tanks[i]->GetSize();

      // check doesn't collide with the arena
      ASSERT_TRUE(posA.x <= (arenaSize - sizeA));
      ASSERT_TRUE(posA.x >= (-arenaSize + sizeA));

      // Check doesn't collide with other tanks
      for (int j=i+1; j < tanks.size(); j++) {
        auto posB = tanks[j]->GetPosition();
        float sizeB = tanks[j]->GetSize();
        float distance = (posA - posB).Length();
        float expectedDistance = sizeA + sizeB;
        ASSERT_TRUE(distance >= expectedDistance);
      }
    }
  }
}
