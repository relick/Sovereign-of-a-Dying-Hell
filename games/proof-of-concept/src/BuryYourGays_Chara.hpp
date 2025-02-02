#pragma once

#include "Declare.hpp"
#include "CharacterData.hpp"

#include <genesis.h>
#include "res_chara.h"

namespace BuryYourGays
{

inline constexpr Game::AnimFrames<1> c_stacey_annoyed_anim = { {
	{ &stacey_annoyed_tilemap, 0, },
} };

inline constexpr Game::AnimFrames<1> c_stacey_excited_anim = { {
	{ &stacey_excited_tilemap, 0, },
} };

inline constexpr Game::AnimFrames<1> c_stacey_sigh_anim = { {
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

}