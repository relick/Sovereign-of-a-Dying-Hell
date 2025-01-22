#pragma once

#include "Declare.hpp"

namespace Game
{

class Script
{
public:
	virtual ~Script() = default;
	
	virtual void Init(Game& io_game, VNWorld& io_vn, CharacterData& io_cd) = 0;
	virtual void Update(Game& io_game, VNWorld& io_vn) = 0;
};

}