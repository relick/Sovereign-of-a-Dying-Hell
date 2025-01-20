#include "Scene.hpp"
#include "Game.hpp"
#include "Worlds.hpp"
#include "DialoguePrinter2.hpp"

#include "SceneDefines.hpp"

namespace Game
{

SceneCoro Scene::Run
(
    Game& io_game,
    DialoguePrinter2& io_dp
)
{
    io_dp.SetName("BOY", true);
    io_dp.SetText("Hi!\nHow are you?\nWant to play?\nI have mahjong.");
    wait;

    io_dp.SetName("GIRL", false);
    io_dp.SetText("Ewwwww.\nGet away, creep.");
    wait;

    io_game.RequestNextWorld(std::make_unique<IntroWorld>());
}

}