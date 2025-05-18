#pragma once

#include "adventure-md/Scene.hpp"

namespace BuryYourGays
{

enum class Scenes : u8
{
	forestShed_runningThroughWoods,
};


#define SCENE(scene_name) struct scene_name : public Game::Scene { Game::SceneRoutine Run(Game::Game& io_game, Game::VNWorld& io_vn, Game::Script const& i_script) override; }

SCENE(forestShed_runningThroughWoods);

}