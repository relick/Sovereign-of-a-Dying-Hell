#pragma once

#include "Declare.hpp"
#include "Coro.hpp"

namespace Game
{

using SceneCoro = Coro<struct SceneCoroTag, int>;

class Scene
{
public:
    SceneCoro Run(Game& io_game, DialoguePrinter2& io_dp);

};

}