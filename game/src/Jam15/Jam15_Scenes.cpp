#include "Jam15_Scenes.hpp"
#include "Jam15_Script.hpp"
#include "Jam15_Chara.hpp"
#include "IntroWorld.hpp"

#include "SceneDefines.hpp"

#include "Jam15/res_music.h"
#include "Jam15/res_bg.h"

#define SCENE_RUN(scene_name) Game::SceneRoutine scene_name::Run(Game::Game& io_game, Game::VNWorld& io_vn, Game::Script const& i_script)
#define SCENE_SETUP() [[maybe_unused]] Jam15::Script const& script = static_cast<Jam15::Script const&>(i_script)

namespace Jam15
{

SCENE_RUN(scene1)
{
    SCENE_SETUP();

    play_music(test, 1.0, true);

    end;
}

}