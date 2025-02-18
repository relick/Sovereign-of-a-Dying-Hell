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

    scene(kishin_council);

    play_music(test, 1.0, true);

    end;
}

SCENE_RUN(HatchingThePlan)
{
    SCENE_SETUP();

    scene(zanmu_study);

    end;
}

SCENE_RUN(LobbyingYuugi)
{
    SCENE_SETUP();

    scene(hell_city);

    end;
}

SCENE_RUN(VotingForAnimalRights)
{
    SCENE_SETUP();

    scene(kishin_council);

    end;
}

SCENE_RUN(SuikaApproaches)
{
    SCENE_SETUP();

    scene(zanmu_study);

    end;
}

SCENE_RUN(DelegatingToHisami)
{
    SCENE_SETUP();

    scene(zanmu_study);

    end;
}

SCENE_RUN(VotingForPriceIncreases)
{
    SCENE_SETUP();

    scene(kishin_council);

    end;
}

SCENE_RUN(MeetingTheShadowyKishin)
{
    SCENE_SETUP();

    scene(hell_palace);

    end;
}

SCENE_RUN(EngagingYuuma)
{
    SCENE_SETUP();

    scene(animal_realm);

    think("It should be around here.");
    think("Ah, I've been spotted.");

    say_hidden(yuuma, "Oi. Scram, oni.");

    say(zanmu, "It's Nippaku.");

    bool const punishmentVotePassed = io_game.ReadVar<bool>(Variables::PunishmentVotePasses);
    bool const priceIncreaseVoteFailed = io_game.ReadVar<bool>(Variables::PriceIncreaseVoteFails);

    show(yuuma, neutral);
    say_hidden(yuuma, "Nippaku...");

    bool onGoodTerms = false;
    if (punishmentVotePassed)
    {
        onGoodTerms = true;
        face(yuuma, pleased);
        say_hidden(yuuma, "Ah! You're the one who cut back our punishments.");
        say_hidden(yuuma, "Hehehe. You must be scheming something good.");
    }
    else
    {
        face(yuuma, annoyed);
        say_hidden(yuuma, "The bleeding heart making symbolic votes over our conditions?");

        {
            static constexpr std::array choices = {
                "I'm sorry it wasn't passed.",
                "The oni are shortsighted."
            };
            choice(choices);
            auto const res = get_choice_result();
            switch(res)
            {
                case 0:
                {
                    say_hidden(yuuma, "Whatever.");
                    break;
                }
                case 1:
                {
                    onGoodTerms = true;
                    say_hidden(yuuma, "They are. And you're different?");
                    say(zanmu, "Certainly.");
                    say_hidden(yuuma, "Hehe. Alright then.");
                    break;
                }
            }
        }
    }

    face(yuuma, neutral);
    say(yuuma, "I'm Yuuma. What do you want?");
    
    say(zanmu, "");

    end;
}

SCENE_RUN(VotingForExecutive)
{
    SCENE_SETUP();

    scene(kishin_council);

    end;
}

SCENE_RUN(FinalHisami)
{
    SCENE_SETUP();

    scene(zanmu_study);

    end;
}

SCENE_RUN(FinalYuugi)
{
    SCENE_SETUP();

    scene(hell_city);

    end;
}

SCENE_RUN(FinalSuika)
{
    SCENE_SETUP();

    scene(hell_palace);

    end;
}

SCENE_RUN(FinalYuuma)
{
    SCENE_SETUP();

    scene(animal_realm);

    end;
}

SCENE_RUN(VotingToRelocateHell)
{
    SCENE_SETUP();

    scene(kishin_council);

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