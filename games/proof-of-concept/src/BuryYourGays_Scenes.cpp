#include "BuryYourGays_Scenes.hpp"
#include "Worlds.hpp"
#include "BuryYourGays_Script.hpp"

#include "SceneDefines.hpp"

#include <genesis.h>
#include "res_music.h"
#include "res_bg.h"

#define SCENE_RUN(scene_name) Game::SceneRoutine scene_name::Run(Game::Game& io_game, Game::VNWorld& io_vn, Game::Script const& i_script)
#define SCENE_SETUP() BuryYourGays::Script const& script = static_cast<BuryYourGays::Script const&>(i_script)

namespace BuryYourGays
{

SCENE_RUN(forestShed_runningThroughWoods)
{
    SCENE_SETUP();

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

    static const std::array tripChoices = {
        "Trip over a tree root!",
        "Fall over a log!",
        "Avoid the obstacles!",
        "Stumble on a fallen branch!",
    };
    std::optional<u16> const tripResult = timed_choice(3.0, tripChoices);

#define trip(object) portrait(beans, shout); vpunch(); say(beans, "OOF-"); think("That " object " came out of nowhere!"); hide_portrait()

    if (tripResult)
    {
        switch (*tripResult)
        {
        case 0: trip("tree root"); break;
        case 1: trip("log"); break;
        case 2:
        {
            portrait(beans, kewl);
            think("Sure footed as a mountain goat!");
            portrait(beans, shout);
            vpunch();
            say(beans, "OOF-");
            say(beans, "Ohh...");
            say(beans, "I was so busy avoiding the branches I just ran face first into that tree.");
            hide_portrait();
            break;
        }
        case 3: trip("branch"); break;
        }
    }
    else
    {
        trip("tree root");
    }

#undef trip

    io_game.RequestNextWorld(std::make_unique<Game::IntroWorld>());

    end;
}

}