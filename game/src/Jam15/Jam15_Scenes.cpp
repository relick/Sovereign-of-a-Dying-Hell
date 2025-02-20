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
    wait_for_tasks();

    desc("The musky chamber is packed with oni, of low and high rank alike.");
    desc("Those of the highest rank sit in the centre, in irreverent, informal seiza.");
    desc("The oni make up the Council, and the kishin lead its ferocious debates.");

    show(zanmu, neutral);
    say(zanmu, "Council, allow me my third proposal for today.");
    say(zanmu, "This one, I consider of critical importance.");
    say(councilR, "Nippaku, you always say that.");
    say(zanmu, "It's always true~");
    say(councilR, "Get on with it, then.");
    say(zanmu, "The earth spirits are idle, and have been since the completion of the most recent oni palace.");
    say(zanmu, "They must be given work, and Hell has plenty of it to give out.");
    say(zanmu, "We've been short on labour for maintaining the torture chambers for decades now. My suggestion is to put them all to work there.");
    say(councilR, "Nippaku, you persist with the earth spirits? You know they've been earmarked for the blood pools development.");
    say(councilL, "That's right, we really shouldn't disrupt a plan in progress. Finding others to do the work would delay it by decades.");
    say(zanmu, "Ridiculous! 27.9\% of the torture chambers have already been forced to close due to malfunction, and our income is suffering for it.");
    say(zanmu, "There's no money for the vanity projects if we refuse to grease the wheels of the economy.");
    say(councilR, "We'll just need to find others to do the work...");
    play_sfx(gavel);
    say(speaker, "That's enough back and forth. Let's not waste time here.");
    say(speaker, "Council, declare your votes!");

    // TODO VOTE (always lost)

    say(speaker, "A decisive defeat. No more proposals from you today, Nippaku, please.");
    say(zanmu, "The lack of wisdom in this Council once again astounds.");
    say(zanmu, "Fine, argue over seating arrangements, or whatever it is you do, I will attend to real business.");

    script.SetNextScene(Scenes::HatchingThePlan);
    end;
}

SCENE_RUN(HatchingThePlan)
{
    SCENE_SETUP();

    scene(zanmu_study);
    wait_for_tasks();

    desc("Zanmu's study is cold and devoid of decoration.");
    desc("This tension makes it a perfect area to think, and make deals.");

    think("Tch. The oni are fickle, I'll just need to try again another time.");
    think("This rate of change isn't fast enough, though. Hell's institutions are already faltering.");
    think("Perhaps working within the Council isn't the play any more...");

    show(hisami, neutral);
    say(hisami, "Lady Zanmu~~! I have news~!");
    say(hisami, "...");
    say(zanmu, "... ... ...");
    say(hisami, "Lady Zanmu?");
    
    say(zanmu, "Oh. Hisami, I didn't notice you. You were gone a while.");
    say(zanmu, "What requires my attention?");
    
    say(hisami, "Lady Zanmu, I have news. I thought you may like to hear it.");
    say(hisami, "I was luring more spirits into the Animal Realm, and I handed them off to a surprisingly organised band of eagles who promised to take care of them.");
    say(hisami, "Given the animal spirits are enslaved with no time to help others, I couldn't help but find that odd!");
    say(hisami, "I found a stalking spot, and listened for eleven and a half days. They mostly talked about how much they'd love to eat turtles some time again...");

    say(zanmu, "Hisami, the point?");

    say(hisami, "Um, yes! The eagle spirits are following a beast. They say she's more powerful than the oni. It may well be most of the eagles are under her wing by now.");

    think("I can't always trust Hisami, but she wouldn't lie about something like this.");
    think("Hah! Is this the chance I've been waiting for?");
    say(zanmu, "That is wonderful news. You've done well, Hisami.");

    say(hisami, "Ehehe~");

    say(zanmu, "Go back to the Animal Realm, and spread rumours of that beast's power. Return to me in two days.");

    // TODO

    script.SetNextScene(Scenes::LobbyingYuugi);
    end;
}

SCENE_RUN(LobbyingYuugi)
{
    SCENE_SETUP();

    scene(hell_city);
    wait_for_tasks();

    // TODO

    script.SetNextScene(Scenes::VotingForAnimalRights);
    end;
}

SCENE_RUN(VotingForAnimalRights)
{
    SCENE_SETUP();

    scene(kishin_council);
    wait_for_tasks();

    // TODO

    script.SetNextScene(Scenes::SuikaApproaches);
    end;
}

SCENE_RUN(SuikaApproaches)
{
    SCENE_SETUP();

    scene(zanmu_study);
    wait_for_tasks();

    desc("The study is cold and sterile. Just how Zanmu likes it.");
    desc("An oni with towering horns gently pushes open the shoji door.");

    say(zanmu, "Suika? What a pleasant surprise.");

    show(suika, neutral);
    face(suika, happy);
    say(suika, "Zanmu!");
    say(suika, "I'm glad you're glad to see me. Is the potted plant around?");

    say(zanmu, "She's on an errand.");

    face(suika, neutral);
    say(suika, "Good, 'cause I wanted to talk with you *extra* privately.");
    say(suika, "I was puzzling over your recent proposals in the council meetings. I reckon you're up to something.");
    say(suika, "You don't *have* to tell me, but we're friends, right? Maybe I'll even support you.");

    think("Suika always hides her true intentions under that playful mask.");
    think("I need to think over carefully if she can be trusted.");

    say(zanmu, "Straight to the point, huh?");
    
    {
        static constexpr std::array choices = {
            "Share your plans with her",
            "Keep the plans secret",
        };
        choice(choices);
        auto const res = get_choice_result();
        switch(res)
        {
        case 0:
        {
            // TODO
            break;
        }
        case 1:
        {
            say(zanmu, "I'm not scheming anything. Has it become wrong to propose improvements to Hell?");

            face(suika, pout);
            say(suika, "Ehhhh...");
            say(suika, "Fine! But if I find out you just lied, I don't care what it is. I'll fight it down.");

            think("Oh dear. Suika has considerable influence with the other kishin. This could become a problem...");
            break;
        }
        }
    }

    script.SetNextScene(Scenes::DelegatingToHisami);
    end;
}

SCENE_RUN(DelegatingToHisami)
{
    SCENE_SETUP();

    scene(zanmu_study);
    wait_for_tasks();

    // TODO

    script.SetNextScene(Scenes::VotingForPriceIncreases);
    end;
}

SCENE_RUN(VotingForPriceIncreases)
{
    SCENE_SETUP();

    scene(kishin_council);
    wait_for_tasks();

    // TODO

    script.SetNextScene(Scenes::MeetingTheShadowyKishin);
    end;
}

SCENE_RUN(MeetingTheShadowyKishin)
{
    SCENE_SETUP();

    scene(hell_palace);
    wait_for_tasks();

    // TODO

    script.SetNextScene(Scenes::EngagingYuuma);
    end;
}

SCENE_RUN(EngagingYuuma)
{
    SCENE_SETUP();

    scene(animal_realm);
    wait_for_tasks();

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
    
    think("What I say next will be very important.");

    bool freeAnimalRealm = false;
    {
        static constexpr std::array choices = {
            "Raise the idea of a liberated Animal Realm",
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
                "\"I'll tell everyone of your plans if you don't help.\"",
                "\"\"",
            };
            choice(choices);
            auto const res = get_choice_result();
            switch (res)
            {
            case 2:
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

    scene(kishin_council);
    wait_for_tasks();

    show(zanmu, neutral);
    say(zanmu, "Illustrious Council! Before we attend to any further matters, I have an urgent proposal to make.");
    say(councilR, "Go ahead, Nippaku.");
    say(zanmu, "I have spent the last few days surveying the realms again, as I often do. I've seen decay, paralysis, and overpopulation. This council has... struggled to respond over the years.");
    say(zanmu, "But it's reaching a breaking point. If this goes on much longer, the Yama will turn elsewhere to send damned souls.");
    say(zanmu, "I see no other choice but this: we must empower one of us as an executive that can lead Hell out of its rot.");
    say(zanmu, "They must be untethered by the debates of this council.");
    say(zanmu, "They must have a singular strong vision, to end this slow death by committee.");

    say(councilL, "What?");
    say(councilR, "Preposterous!");
    say(councilL, "She has a point, we barely achieve anything.");
    say(councilR, "None of us deserve to rule without restriction.");
    say(councilL, "Exactly! This is a power move!");

    think("They're taking it about as well as I expected.");

    say(councilR, "...QUIET!");
    say(councilR, "Nippaku, your proposal is acknolwedged, let's get it out the way first. Hopefully a productive meeting can follow.");

    // TODO-VOTE
    bool const voteSucceeded = {};

    if (voteSucceeded)
    {
        // TODO
        script.SetNextScene(Scenes::Ending_RulingDecay);
        end;
    }
    
    // TODO eh could be better
    say(zanmu, "I accept the council's decision, but I see it as a refusal of reality.");
    say(zanmu, "Nevertheless, I do acknowledge the risk of allocating supreme power. I still see another way forward, that avoids this risk.");
    say(zanmu, "I will take leave of the remainder of this meeting, and will raise a fresh idea in tomorrow's meeting.");

    hide();
    io_vn.ClearMode(io_game);
    scene(zanmu_study);

    think("That was no surprise. It would have taken all my influence to win that vote.");
    think("If I could have ruled this Hell... it would have had a chance.");
    think("But maybe it was always going to be easier to take the more drastic option.");
    think("Tomorrow, I will propose a relocation, to a new Hell.");
    think("...");
    think("I should spend the rest of the day shoring up my support.");

    {
        static constexpr std::array choices = {
            "Stay with Hisami",
            "Revisit Yuugi",
            "Revisit Suika",
            "Revisit Yuuma",
        };
        choice(choices);
        auto const res = get_choice_result();
        switch (res)
        {
        case 0:
        {
            script.SetNextScene(Scenes::FinalHisami);
            break;
        }
        case 1:
        {
            script.SetNextScene(Scenes::FinalYuugi);
            break;
        }
        case 2:
        {
            script.SetNextScene(Scenes::FinalSuika);
            break;
        }
        case 3:
        {
            script.SetNextScene(Scenes::FinalYuuma);
            break;
        }
        }
    }

    end;
}

SCENE_RUN(FinalHisami)
{
    SCENE_SETUP();

    scene(zanmu_study);
    wait_for_tasks();

    // TODO

    script.SetNextScene(Scenes::VotingToRelocateHell);
    end;
}

SCENE_RUN(FinalYuugi)
{
    SCENE_SETUP();

    scene(hell_city);
    wait_for_tasks();

    // TODO

    script.SetNextScene(Scenes::VotingToRelocateHell);
    end;
}

SCENE_RUN(FinalSuika)
{
    SCENE_SETUP();

    scene(hell_palace);
    wait_for_tasks();

    // TODO

    script.SetNextScene(Scenes::VotingToRelocateHell);
    end;
}

SCENE_RUN(FinalYuuma)
{
    SCENE_SETUP();

    scene(animal_realm);
    wait_for_tasks();

    // TODO

    script.SetNextScene(Scenes::VotingToRelocateHell);
    end;
}

SCENE_RUN(VotingToRelocateHell)
{
    SCENE_SETUP();

    scene(kishin_council);
    wait_for_tasks();

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