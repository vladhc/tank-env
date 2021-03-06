#include <iostream>
#include <map>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <cmath>
#include "env.h"
#include "lidar.h"
#include "geom.h"
#include "renderer.h"
#include "chunk.h"

namespace py = pybind11;

py::dict encodeObservation(const Observation &obs) {
    // Hero
    const Tank* hero = obs.tanks[obs.heroId];
    unsigned int heroChunkSize = static_cast<unsigned int>(HeroChunk::Size);
    float* heroArr = new float[heroChunkSize];
    writeHeroChunk(hero, heroArr);
    py::array_t<float> heroObs{heroChunkSize, heroArr};
    delete heroArr;

    // Lidar
    std::vector<Ray> rays = hero->GetLidar()->CastRays();
    long int raysSize = static_cast<long int>(rays.size());
    float* lidarDistanceArr = new float[raysSize];
    float* lidarTankLocatedArr = new float[raysSize];
    float* lidarEnemyLocatedArr = new float[raysSize];
    for (unsigned int i=0; i < raysSize; i++) {
      const auto ray = rays[i];
      lidarDistanceArr[i] = ray.pt.Length();
      if (ray.obj != nullptr && ray.obj->type == GameObjectType::TANK) {
        lidarTankLocatedArr[i] = 1;
        Tank* t = (Tank*)ray.obj;
        if(t->GetTeamId() != hero->GetTeamId()) {
          lidarEnemyLocatedArr[i] = 1;
        } else {
          lidarEnemyLocatedArr[i] = 0;
        }
      } else {
        lidarTankLocatedArr[i] = 0;
        lidarEnemyLocatedArr[i] = 0;
      }
    }
    py::array_t<float> lidarDistanceObs{raysSize, lidarDistanceArr};
    py::array_t<float> lidarTankLocatedObs{raysSize, lidarTankLocatedArr};
    py::array_t<float> lidarEnemyLocatedObs{raysSize, lidarEnemyLocatedArr};
    delete lidarDistanceArr;
    delete lidarTankLocatedArr;
    delete lidarEnemyLocatedArr;

    // Other tanks
    py::list tanksObs{obs.tanks.size() - 1};
    unsigned idx = 0;
    unsigned int tankChunkSize = static_cast<unsigned int>(TankChunk::Size);
    for (const Tank* tank : obs.tanks) {
      if (tank->GetId() == obs.heroId) {
        continue;
      }
      float* tankArr = new float[tankChunkSize];
      writeTankChunk(tank, hero, tankArr);
      tanksObs[idx] = py::array_t<float>(tankChunkSize, tankArr);
      delete tankArr;
      idx++;
    }

    unsigned int bulletChunkSize = static_cast<unsigned int>(BulletChunk::Size);
    py::list bulletsObs{obs.bullets.size()};
    for (unsigned int i=0; i < obs.bullets.size(); i++) {
      const Bullet* bullet = obs.bullets[i];
      float* bulletArr = new float[bulletChunkSize];
      writeBulletChunk(bullet, hero, bulletArr);
      bulletsObs[i] = py::array_t<float>(bulletChunkSize, bulletArr);
      delete bulletArr;
    }

    unsigned int obstacleChunkSize = static_cast<unsigned int>(ObstacleChunk::Size);
    py::list obstaclesObs{obs.obstacles.size()};
    for (unsigned int i=0; i < obs.obstacles.size(); i++) {
      const b2Body* obstacle = obs.obstacles[i];
      float* obstacleArr = new float[obstacleChunkSize];
      writeObstacleChunk(obstacle, hero, obstacleArr);
      obstaclesObs[i] = py::array_t<float>(obstacleChunkSize, obstacleArr);
      delete obstacleArr;
    }

    using namespace pybind11::literals; // to bring in the `_a` literal
    py::dict obsMap(
        "hero"_a=heroObs,
        "lidar_dist"_a=lidarDistanceObs,
        "lidar_is_tank"_a=lidarTankLocatedObs,
        "lidar_is_enemy"_a=lidarEnemyLocatedObs,
        "tanks"_a=tanksObs,
        "bullets"_a=bulletsObs,
        "obstacles"_a=obstaclesObs
    );

    return obsMap;
}

PYBIND11_MODULE(tanks, m) {
    py::class_<Renderer>(m, "Renderer")
        .def(py::init<>())
        .def("render",
            [](Renderer &renderer, Env &env) {
              renderer.Render(env);
            });
    py::enum_<TankChunk>(m, "TankChunk")
      .value("IS_ENEMY", TankChunk::IS_ENEMY)
      .value("HITPOINTS", TankChunk::HITPOINTS)
      .value("IS_ALIVE", TankChunk::IS_ALIVE)
      .value("FIRE_COOLDOWN", TankChunk::FIRE_COOLDOWN)
      .value("POSITION_DISTANCE", TankChunk::POSITION_DISTANCE)
      .value("POSITION_ANGLE", TankChunk::POSITION_ANGLE)
      .value("POSITION_X", TankChunk::POSITION_X)
      .value("POSITION_Y", TankChunk::POSITION_Y)
      .value("BODY_ANGLE", TankChunk::BODY_ANGLE)
      .value("TURRET_ANGLE_RELATIVE_TO_BODY", TankChunk::TURRET_ANGLE_RELATIVE_TO_BODY)
      .value("VELOCITY_LENGTH", TankChunk::VELOCITY_LENGTH)
      .value("VELOCITY_ANGLE_RELATIVE_TO_BODY", TankChunk::VELOCITY_ANGLE_RELATIVE_TO_BODY)
      .value("BODY_ANGULAR_VELOCITY", TankChunk::BODY_ANGULAR_VELOCITY)
      .value("TURRET_ANGULAR_VELOCITY", TankChunk::TURRET_ANGULAR_VELOCITY)
      .value("AIM_ANGLE", TankChunk::AIM_ANGLE)
      .value("Size", TankChunk::Size);
    py::enum_<HeroChunk>(m, "HeroChunk")
      .value("HITPOINTS", HeroChunk::HITPOINTS)
      .value("FIRE_COOLDOWN", HeroChunk::FIRE_COOLDOWN)
      .value("POSITION_X", HeroChunk::POSITION_X)
      .value("POSITION_Y", HeroChunk::POSITION_Y)
      .value("BODY_ANGLE", HeroChunk::BODY_ANGLE)
      .value("TURRET_ANGLE_RELATIVE_TO_BODY", HeroChunk::TURRET_ANGLE_RELATIVE_TO_BODY)
      .value("VELOCITY_ANGLE_RELATIVE_TO_BODY", HeroChunk::VELOCITY_ANGLE_RELATIVE_TO_BODY)
      .value("VELOCITY_LENGTH", HeroChunk::VELOCITY_LENGTH)
      .value("BODY_ANGULAR_VELOCITY", HeroChunk::BODY_ANGULAR_VELOCITY)
      .value("TURRET_ANGULAR_VELOCITY_RELATIVE_TO_BODY", HeroChunk::TURRET_ANGULAR_VELOCITY_RELATIVE_TO_BODY)
      .value("Size", HeroChunk::Size);
    py::enum_<BulletChunk>(m, "BulletChunk")
      .value("POSITION_X", BulletChunk::POSITION_X)
      .value("POSITION_Y", BulletChunk::POSITION_Y)
      .value("POSITION_DISTANCE", BulletChunk::POSITION_DISTANCE)
      .value("POSITION_ANGLE", BulletChunk::POSITION_ANGLE)
      .value("VELOCITY_ANGLE", BulletChunk::VELOCITY_ANGLE)
      .value("Size", BulletChunk::Size);
    py::enum_<ObstacleChunk>(m, "ObstacleChunk")
      .value("POSITION_DISTANCE", ObstacleChunk::POSITION_DISTANCE)
      .value("POSITION_X", ObstacleChunk::POSITION_X)
      .value("POSITION_Y", ObstacleChunk::POSITION_Y)
      .value("POSITION_ANGLE", ObstacleChunk::POSITION_ANGLE)
      .value("ANGLE", ObstacleChunk::ANGLE)
      .value("WIDTH", ObstacleChunk::WIDTH)
      .value("HEIGHT", ObstacleChunk::HEIGHT)
      .value("Size", ObstacleChunk::Size);
    py::class_<Env>(m, "Env")
        .def(py::init<unsigned int, unsigned int, size_t>())
        .def("damage_tank",
            [](Env &env, int tankId, unsigned int damage) {
              env.DamageTank(tankId, damage);
            }
        )
        .def("reset",
            [](Env &env) {
              std::vector<Observation> obs = env.Reset();

              py::dict obsMap;
              for (unsigned int i=0; i < obs.size(); i++) {
                const int tankId = obs[i].heroId;
                py::int_ idx = py::int_(tankId);
                obsMap[idx] = encodeObservation(obs[i]);
              }

              return obsMap;
            }
        )
        .def("step",
            [](Env &env, std::map<int, py::array_t<float>> actionsMap) {
              std::map<int, Action> actions;
              for (auto item : actionsMap) {
                py::buffer_info actionArrInfo = item.second.request();
                if (actionArrInfo.ndim != 1) {
                  throw std::runtime_error("Number of dimensions must be 1");
                }
                if (actionArrInfo.shape[0] != 4) {
                  throw std::runtime_error("Expected dimension[0] to be of size 4");
                }
                float* action = (float*)(actionArrInfo.ptr);
                actions[item.first] = Action{action[0], action[1], action[2], action[3] > 0.};
              }

              auto t = env.Step(actions);
              std::vector<Observation> obs = t.observations;
              std::vector<float> rewards = t.rewards;
              std::vector<char> dones = t.dones;

              py::dict obsMap;
              py::dict rewardsMap;
              py::dict donesMap;

              for (unsigned int i=0; i < obs.size(); i++) {
                bool done = (bool)dones[i];
                float reward = rewards[i];

                if (!done || reward != 0) {
                  const int tankId = obs[i].heroId;
                  py::int_ idx = py::int_(tankId);

                  obsMap[idx] = encodeObservation(obs[i]);
                  rewardsMap[idx] = reward;
                  donesMap[idx] = done;
                }
              }
              donesMap["__all__"] = env.EpisodeComplete();

              return std::make_tuple(
                  obsMap,
                  rewardsMap,
                  donesMap,
                  py::dict()
              );
            }
        );
}
