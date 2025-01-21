#pragma once

#include "Declare.hpp"
#include "GameRoutines.hpp"

namespace Game
{

class Scene
{
public:
    SceneRoutine Run(Game &io_game, DialoguePrinter2 &io_dp);
};

}