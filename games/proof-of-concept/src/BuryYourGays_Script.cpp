#include "BuryYourGays_Script.hpp"

#include "CharacterData.hpp"

#include <genesis.h>
#include "res_chara.h"

namespace BuryYourGays
{

void Script::Init
(
	Game::Game& io_game,
	Game::VNWorld& io_vn,
	Game::CharacterData& io_cd
)
{
	// Set up characters
	io_cd.AddCharacter("beans", "BEANS", true);
	io_cd.AddCharacter("stacey", "STACEY", true);

	// Set up poses
	io_cd.AddPose("stacey", "annoyed", &stacey_annoyed, &stacey_annoyed_name_pal, &stacey_annoyed_text_pal);
	io_cd.AddPose("stacey", "excited", &stacey_excited, &stacey_excited_name_pal, &stacey_excited_text_pal);
	io_cd.AddPose("stacey", "sigh", &stacey_sigh, stacey_sigh.palette, stacey_sigh.palette);

	// First scene
	m_nextScene = CreateScene(Scenes::forestShed_runningThroughWoods);
	Update(io_game, io_vn);
}

void Script::Update
(
	Game::Game& io_game,
	Game::VNWorld& io_vn
)
{
	if (m_nextScene)
	{
		m_currentScene = std::move(m_nextScene);
		m_currentSceneRoutine = m_currentScene->Run(io_game, io_vn);
	}
	else if (m_currentSceneRoutine)
	{
		m_currentSceneRoutine();
	}
}

std::unique_ptr<Game::Scene> Script::CreateScene
(
	Scenes i_scene
)
{
	switch(i_scene)
	{
		case Scenes::forestShed_runningThroughWoods: return std::make_unique<forestShed_runningThroughWoods>();
	}
	
	return {};
}

}