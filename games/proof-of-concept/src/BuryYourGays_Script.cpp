#include "BuryYourGays_Script.hpp"

#include "CharacterData.hpp"

#include <genesis.h>
#include "res_chara.h"

namespace BuryYourGays
{

inline constexpr Game::Animation<1> c_stacey_annoyed_anim = { {
	{ &stacey_annoyed_tilemap, 0, },
} };

inline constexpr Game::Animation<1> c_stacey_excited_anim = { {
	{ &stacey_excited_tilemap, 0, },
} };

inline constexpr Game::Animation<1> c_stacey_sigh_anim = { {
	{ &stacey_sigh_tilemap, 0, },
} };

inline constexpr Game::Pose c_stacey_annoyed_pose = {
	&stacey_annoyed_tileset,
	&stacey_annoyed_pal,
	c_stacey_annoyed_anim
};

inline constexpr Game::Pose c_stacey_excited_pose = {
	&stacey_excited_tileset,
	&stacey_excited_pal,
	c_stacey_excited_anim
};

inline constexpr Game::Pose c_stacey_sigh_pose = {
	&stacey_sigh_tileset,
	&stacey_sigh_pal,
	c_stacey_sigh_anim
};

inline constexpr Game::Poses<3> c_stacey_poses = {
	c_stacey_annoyed_pose,
	c_stacey_excited_pose,
	c_stacey_sigh_pose,
};

inline constexpr Game::Character c_stacey = {
	"STACEY", true,
	c_stacey_poses,
};


inline constexpr Game::Character c_beans = {
	"BEANS", true,
	{},
};

void Script::Init
(
	Game::Game& io_game,
	Game::VNWorld& io_vn,
	Game::CharacterData& io_cd
)
{
	// Set up characters
	beans = io_cd.AddCharacter(&c_beans);
	stacey = io_cd.AddCharacter(&c_stacey);

	// Set up poses
	stacey_annoyed = 0;
	stacey_excited = 1;
	stacey_sigh = 2;

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