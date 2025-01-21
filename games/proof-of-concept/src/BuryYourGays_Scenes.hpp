#pragma once

#include "Scene.hpp"

namespace BuryYourGays
{

#define SCENE(scene_name) struct scene_name : public Game::Scene { Game::SceneRoutine Run(Game::Game &io_game, Game::DialoguePrinter2 &io_dp) override; }

SCENE(forestShed_runningThroughWoods);

}