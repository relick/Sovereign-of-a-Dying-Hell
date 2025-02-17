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

SCENE_RUN(FirstVoteProposal)
{
    SCENE_SETUP();

    play_music(test, 1.0, true);

    end;
}

SCENE_RUN(HatchingThePlan)
{
    SCENE_SETUP();

    end;
}

SCENE_RUN(LobbyingYuugi)
{
    SCENE_SETUP();

    end;
}

SCENE_RUN(VotingForAnimalRights)
{
    SCENE_SETUP();

    end;
}

SCENE_RUN(SuikaApproaches)
{
    SCENE_SETUP();

    end;
}

SCENE_RUN(DelegatingToHisami)
{
    SCENE_SETUP();

    end;
}

SCENE_RUN(VotingForPriceIncreases)
{
    SCENE_SETUP();

    end;
}

SCENE_RUN(MeetingTheShadowyKishin)
{
    SCENE_SETUP();

    end;
}

SCENE_RUN(EngagingYuuma)
{
    SCENE_SETUP();

    end;
}

SCENE_RUN(VotingForExecutive)
{
    SCENE_SETUP();

    end;
}

SCENE_RUN(FinalHisami)
{
    SCENE_SETUP();

    end;
}

SCENE_RUN(FinalYuugi)
{
    SCENE_SETUP();

    end;
}

SCENE_RUN(FinalSuika)
{
    SCENE_SETUP();

    end;
}

SCENE_RUN(FinalYuuma)
{
    SCENE_SETUP();

    end;
}

SCENE_RUN(VotingToRelocateHell)
{
    SCENE_SETUP();

    end;
}


SCENE_RUN(Ending_RulingDecay)
{
    SCENE_SETUP();

    end;
}

SCENE_RUN(Ending_NewHell)
{
    SCENE_SETUP();

    end;
}

SCENE_RUN(Ending_AdministratingCollapse)
{
    SCENE_SETUP();

    end;
}

}