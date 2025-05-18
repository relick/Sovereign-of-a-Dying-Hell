#pragma once

#include "adventure-md/Declare.hpp"
#include "adventure-md/GameRoutines.hpp"

namespace Game
{

class Scene
{
public:
    virtual ~Scene() = default;

    virtual SceneRoutine Run(Game& io_game, VNWorld& io_vn, Script& io_script) = 0;
};

}