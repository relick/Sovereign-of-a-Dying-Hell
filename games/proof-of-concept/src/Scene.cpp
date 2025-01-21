#include "Scene.hpp"
#include "Game.hpp"
#include "Worlds.hpp"
#include "DialoguePrinter2.hpp"

#include "SceneDefines.hpp"

namespace Game
{

SceneRoutine Scene::Run
(
    Game& io_game,
    DialoguePrinter2& io_dp
)
{
    say("BOY", "Hi!\nHow are you?\nWant to play?\nI have mahjong.");

    say("GIRL", "Ewwwww.\nGet away, creep.");

    io_game.RequestNextWorld(std::make_unique<IntroWorld>());

    end;
}

}