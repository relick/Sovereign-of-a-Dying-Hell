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
	beans = io_cd.AddCharacter("BEANS", true);
	stacey = io_cd.AddCharacter("STACEY", true);

	// Set up poses
	stacey_annoyed = io_cd.AddPose(stacey, &stacey_annoyed_img, &stacey_annoyed_name_pal, &stacey_annoyed_text_pal);
	stacey_excited = io_cd.AddPose(stacey, &stacey_excited_img, &stacey_excited_name_pal, &stacey_excited_text_pal);
	stacey_sigh = io_cd.AddPose(stacey, &stacey_sigh_img, stacey_sigh_img.palette, stacey_sigh_img.palette);

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
		m_currentSceneRoutine = m_currentScene->Run(io_game, io_vn, *this);
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