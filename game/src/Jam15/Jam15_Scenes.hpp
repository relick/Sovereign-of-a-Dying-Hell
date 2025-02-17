#pragma once

#include "Scene.hpp"

namespace Jam15
{

enum class Scenes : u8
{
	scene1,
};


#define SCENE(scene_name) struct scene_name : public Game::Scene { Game::SceneRoutine Run(Game::Game& io_game, Game::VNWorld& io_vn, Game::Script const& i_script) override; }

SCENE(scene1);

}