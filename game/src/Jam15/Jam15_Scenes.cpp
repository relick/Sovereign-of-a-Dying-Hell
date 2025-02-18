#include "Jam15_Scenes.hpp"
#include "Jam15_Script.hpp"
#include "Jam15_Chara.hpp"
#include "IntroWorld.hpp"

#include "SceneDefines.hpp"

#include "Jam15/res_music.h"
#include "Jam15/res_bg.h"

#define SCENE_RUN(scene_name) Game::SceneRoutine scene_name::Run(Game::Game& io_game, Game::VNWorld& io_vn, Game::Script& io_script)
#define SCENE_SETUP() [[maybe_unused]] Jam15::Script& script = static_cast<Jam15::Script&>(io_script)

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

    // music(shining_law_of_the_strong_eating_the_weak, 0.0, true);

    portrait(zanmu, focus);
    think("It should be around here.");
    think("Ah, I've been spotted.");

    say_hidden(yuuma, "Oi. Scram, oni.");

    say(zanmu, "It's Nippaku.");

    bool const punishmentVotePassed = io_game.ReadVar<bool>(Variables::PunishmentVotePasses);
    bool const priceIncreaseVoteFailed = io_game.ReadVar<bool>(Variables::PriceIncreaseVoteFails);

    show(yuuma, neutral);
    say_hidden(yuuma, "Nippaku...");

    u8 flattery = 0;
    if (punishmentVotePassed)
    {
        ++flattery;
        face(yuuma, pleased);
        say_hidden(yuuma, "Ah! You're the one who cut back our punishments.");
        say_hidden(yuuma, "Hehehe. You must be scheming something good.");
    }
    else
    {
        face(yuuma, annoyed);
        say_hidden(yuuma, "The bleeding heart making symbolic votes over our conditions?");

        static constexpr std::array choices = {
            "\"I'm sorry it didn't pass.\"",
            "\"The oni are shortsighted.\""
        };
        choice(choices);
        auto const res = get_choice_result();
        switch(res)
        {
            case 0:
            {
                portrait(zanmu, pained);
                say_hidden(yuuma, "Whatever.");
                break;
            }
            case 1:
            {
                ++flattery;
                say_hidden(yuuma, "They are. And you're different?");
                say(zanmu, "I see things how they really are.");
                say_hidden(yuuma, "Hehe. Alright then.");
                break;
            }
        }
    }

    face(yuuma, neutral);
    say(yuuma, "I'm Yuuma. What do you want?");

    portrait(zanmu, neutral);
    say(zanmu, "I wanted to see who was acting as a leader to the eagle spirits. I'm keen to see them prosper.");

    think("A taotie is their leader? She might turn out to be a loose cannon. Still, I need to get on her good side...");

    // TODO: a bit more chatter to get to know Yuuma?

    bool freeAnimalRealm = false;
    {
        static constexpr std::array choices = {
            "Raise the idea of a completely free Animal Realm",
            "Offer advice on growing her organisation",
        };
        choice(choices);
        auto const res = get_choice_result();
        freeAnimalRealm = res == 0;
    }

    if (freeAnimalRealm)
    {
        // freeAnimalRealm

        say(zanmu, "There's a bigger picture than the oni can see as they squabble over tiny details. The current system isn't working.");
        say(zanmu, "That's especially true here! The enslaved animal spirits are supposed to be the backbone of Hell's economy, but with so little freedom, they barely do any work. They have no loyalty or morale.");

        say(yuuma, "You don't sound so different from the other oni if all you care about is the economy?");

        think("Hmm... she's not throwing me any bones.");

        say(zanmu, "Actually... I see the problem is the oni's involvement in the economy in the first place.");
        say(zanmu, "They plan everything out, because they can't let go of even the smallest drop of their power.");
        say(zanmu, "That's what makes them miss all the opportunities that only individuals can pursue.");

        say(yuuma, "Hah! That is more my language. What do you see as the alternative?");

        say(zanmu, "The Animal Realm understands the only rule that truly matters: 'the strong eat the weak'. I see no reason not to simply put that into practice. Set the whole Realm free, no hierarchy, no slavery from above. Let beasts rule themselves.");

        say(yuuma, "The eagle spirits follow my words because they've seen my strength. The rule is as you say...");
        say(yuuma, "But tell me how you're going to convince the oni to give up their grip?");

        say(zanmu, "Well, that's what I need to ask of you. I'm aware of your ambitions to carve your own territory. That's an essential bargaining chip.");
        say(zanmu, "They will vote for my plans believing it will prevent your uprising, and in doing so I will deliver the free Animal Realm regardless.");

        say(yuuma, "Hahahah!");
        say(yuuma, "I don't trust you one bit!");

        if (priceIncreaseVoteFailed)
        {
            ++flattery;
        }
        else
        {
            face(yuuma, annoyed);
            say(yuuma, "For one, you managed to get the Sanzu's prices raised on your own. Why do you need me for leverage?");
        }

        face(yuuma, pleased);
        say(yuuma, "Even if you don't deceive, there's no way you'd get that sort of power. Why wouldn't I just take control myself anyway, if a few eagles and a spork is all it takes!");

        think("Damn! She's feeling threatened. I'll need to make her feel in control.");

        bool failedBothFlattery = true;
        // Flattery 1
        {
            static constexpr std::array choices = {
                "\"The oni aren't weak. An uprising alone isn't enough.\"",
                "\"It's true, an uprising could work.\"",
                "\"This is the only way with long term success.\"",
            };
            choice(choices);
            auto const res = get_choice_result();
            switch (res)
            {
            case 1:
            {
                failedBothFlattery = false;
                say(zanmu, "You could carve out your own niche of the Animal Realm, give it a name, and rule it as you like.");
                say(zanmu, "But that's where it would end, there would be no more chances for more territory. The rest of Hell would resist fiercely and fight for it back.");

                say(yuuma, "Hmph. Maybe so. Defending it for eternity sounds exhausting.");

                think("Good, she liked that.");
                break;
            }
            default:
            {
                say(zanmu, "You'd be crushed if you tried. Only my plan will work.");

                say(yuuma, "Why do you think calling me stupid would get you anywhere?");

                think("Argh, she's more annoyed now.");
                break;
            }
            }
        }

        // Flattery 2
        if (flattery > 0)
        {
            think("Now to land the finishing blow.");

            // TODO
            static constexpr std::array choices = {
                "\"\"",
                "\"\"",
                "\"\"",
            };
            choice(choices);
            auto const res = get_choice_result();
            switch (res)
            {
            case 1:
            {
                failedBothFlattery = false;
                break;
            }
            default:
            {
                break;
            }
            }
        }
        
        if (flattery >= 2)
        {
            // TODO
            if (failedBothFlattery)
            {

            }
            else
            {

            }
        }
        else
        {
            say(yuuma, "Look. If you want to help the Animal Realm then do as you please. But I'm handling things my own way.");

            say(zanmu, "Hm. If you insist.");

            hide();

            think("That was disappointing. Without her backing, I'll need to be on top of my game.");
        }

        // freeAnimalRealm
    }
    else
    {
        // !freeAnimalRealm

        // TODO

        // !freeAnimalRealm
    }

    script.SetNextScene(Scenes::VotingForExecutive);

    end;
}

SCENE_RUN(VotingForExecutive)
{
    SCENE_SETUP();

    scene(zanmu_study);

    scene(kishin_council);

    scene(zanmu_study);


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