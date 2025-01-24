#include "BuryYourGays_Scenes.hpp"
#include "Worlds.hpp"

#include "SceneDefines.hpp"

#include <genesis.h>
#include "res_music.h"
#include "res_bg.h"

#define SCENE_RUN(scene_name) Game::SceneRoutine scene_name::Run(Game::Game& io_game, Game::VNWorld& io_vn)

namespace BuryYourGays
{

SCENE_RUN(forestShed_runningThroughWoods)
{
    scene(forest);

    play_music(hallowbean_nature, 2.0, true);

    wait_for_tasks();

    portrait(beans, tired);
    think("I'm so tired... and my feet hurt...");
    think("It's getting dark so fast...");
    portrait(beans, shout);
    think("Glad I grabbed that flashlight from Gerald's survival kit.");

    portrait(beans, sad);
    say(beans, "Poor Gerald...");
    portrait(beans, tired);
    say(beans, "I wonder where the others are?");
    hide_portrait();

    show(stacey, sigh);
    say(stacey, "I'm sure the road was this way.");

    show(stacey, annoyed);
    say(stacey, "Can't you keep up, Beans?");

    portrait(beans, stressed);
    say(beans, "I'm going as fast as I can!");
    think("How am I meant to keep up with a champion cheerleader?");
    hide();

    wait_for_tasks();

    portrait(beans, tired);
    think("I can barely see where I'm putting my feet!");
    hide_portrait();

    io_game.RequestNextWorld(std::make_unique<Game::IntroWorld>());

    end;
}

}