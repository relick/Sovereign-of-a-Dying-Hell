#pragma once

#include "Declare.hpp"

namespace Game
{

class Script
{
public:
	virtual ~Script() = default;
	
	virtual void InitVN(Game& io_game, VNWorld& io_vn) = 0;
	virtual void UpdateVN(Game& io_game, VNWorld& io_vn) = 0;
};

}