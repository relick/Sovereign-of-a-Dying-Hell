#pragma once

#include "adventure-md/Scene.hpp"

namespace TestZanmu
{

enum class Scenes : u8
{
	BlinkingInHell,
};


#define SCENE(scene_name) struct scene_name : public Game::Scene { Game::SceneRoutine Run(Game::Game& io_game, Game::VNWorld& io_vn, Game::Script const& i_script) override; }

SCENE(BlinkingInHell);

}