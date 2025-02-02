#include "TestZanmu_Scenes.hpp"
#include "TestZanmu_Script.hpp"
#include "TestZanmu_Chara.hpp"

#include "SceneDefines.hpp"

#include "res_music.h"
#include "res_bg.h"

#define SCENE_RUN(scene_name) Game::SceneRoutine scene_name::Run(Game::Game& io_game, Game::VNWorld& io_vn, Game::Script const& i_script)
#define SCENE_SETUP() [[maybe_unused]] TestZanmu::Script const& script = static_cast<TestZanmu::Script const&>(i_script)

namespace TestZanmu
{

SCENE_RUN(BlinkingInHell)
{
    SCENE_SETUP();

    scene(beach2);

    //play_music(spacey, 2.0, true);

    wait_for_tasks();

    show(zanmu, blink);
    say(zanmu, "This is some long text dialogue\nThat covers multiple lines.\nAnd is said whilst an animation of the character plays.");

    end;
}

}