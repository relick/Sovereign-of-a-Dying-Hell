#include "Jam15_Scenes.hpp"
#include "Jam15_Script.hpp"
#include "Jam15_Chara.hpp"
#include "IntroWorld.hpp"
#include "VoteMode.hpp"

#include "SceneDefines.hpp"

#include "Jam15/res_music.h"
#include "Jam15/res_bg.h"

#define SCENE_RUN(scene_name) Game::SceneRoutine scene_name::Run(Game::Game& io_game, Game::VNWorld& io_vn, Game::Script& io_script)
#define SCENE_SETUP() [[maybe_unused]] Jam15::Script& script = static_cast<Jam15::Script&>(io_script)

#define zsay(FACE, TEXT) say_face(zanmu, FACE, TEXT)
#define zthink(FACE, TEXT) think_face(zanmu, FACE, TEXT)

namespace Jam15
{

SCENE_RUN(FirstVoteProposal)
{
    SCENE_SETUP();

    scene(kishin_council);
    //play_music(faron, 0.0, true);
    wait_for_tasks();

    desc("The musky chamber is packed with oni, of low and high rank alike.");
    desc("Those of the highest rank sit in the centre, in irreverent, informal seiza.");
    desc("The oni make up the Council, and the kishin lead its ferocious debates.");
    desc("It's once again your turn to speak.");

    show(zanmu, neutral);
    say(zanmu, "Council, allow me my third proposal for today.");
    say(zanmu, "This one, I consider of critical importance.");
    say(acouncil, "Nippaku, you always say that.");
    say(zanmu, "It's always true~");
    say(acouncil, "Get on with it, then.");
    say(zanmu, "The earth spirits are idle, and have been since the completion of the most recent oni palace.");
    say(zanmu, "They must be given work, and Hell has plenty of it to give out.");
    say(zanmu, "We've been short on labour for maintaining the torture chambers for decades now. My suggestion is to put them all to work there.");
    say(bcouncil, "Nippaku, you persist with the earth spirits? You know they've been earmarked for the blood pools development.");
    say(acouncil, "That's right, we really shouldn't disrupt a plan in progress. Finding others to do the work would delay it by decades.");
    say(zanmu, "Ridiculous! 27.9\% of the torture chambers have already been forced to close due to malfunction, and our income is suffering for it.");
    say(zanmu, "There's no money for the vanity projects if we refuse to grease the wheels of the economy.");
    say(ccouncil, "We'll simply need to find others to do the maintenance...");
    play_sfx(gavel);
    say(speaker, "That's enough back and forth. Let's not waste time here.");
    say(speaker, "Council, declare your votes!");

    io_game.SetVar<Variables::Influence>(128);

    // It should be impossible to win this
    Game::VoteModeParams const vote = {
        .m_votingTime = FIX16(7),
        .m_startingPlayerInfluence = io_game.ReadVar<Variables::Influence>(),
        .m_attackSize = 256,
        .m_attackPattern = Game::AttackPattern::FastBitty,
        .m_playerWantsToLose = false,
    };
    start_vote(vote);
    Game::VoteResult const result = get_vote_result();

    if (result.m_playerWon)
    {
        show(zanmu, neutral);
        say(zanmu, "I won!");
    }
    else
    {
        show(zanmu, neutral);
        say(zanmu, "I lost!");
    }

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

    desc("Your study is cold and devoid of decoration.");
    desc("The tension makes it the perfect place to think.");

    zthink(unamused, "Tch. The oni are always so weak willed. I'll need to try again another time.");
    zthink(unamused_sweat, "This rate of change isn't fast enough, though. Hell's institutions are already faltering.");
    zthink(neutral, "Perhaps working within the Council isn't the play any more...");

    show(hisami, joy);
    say(hisami, "Lady Zanmu~~! I have news~! You're sure to like it~!");
    zsay(neutral, "...");
    say(hisami, "... ...");
    zsay(neutral, "... ... ...");
    show(hisami, neutral);
    say(hisami, "Lady Zanmu?");
    
    zsay(neutral, "Oh. Hisami, I didn't notice you.");
    zsay(neutral, "You've been away for quite a while. What requires my attention?");
    
    say(hisami, "... Lady Zanmu, I have news. I thought you may like to hear it.");
    say(hisami, "I was luring more spirits into the Animal Realm, and I handed them off to a surprisingly organised band of eagles who promised to take care of them.");
    say(hisami, "Given the animal spirits are enslaved with no time to help others, I couldn't help but find that odd!");
    say(hisami, "I found a stalking spot, and listened for eleven and a half days. They mostly talked about how much they'd love to eat turtles some time again...");

    zsay(pained, "Please get to the point!");

    say(hisami, "Um, yes! The eagle spirits are following a beast. They think she's more powerful than even the oni. Clearly they exaggerate, no-one is stronger than you, Lady Zanmu~!");

    zthink(neutral, "I can't always trust Hisami, but she wouldn't lie about something like this.");
    zthink(gloat, "..Hah! Is this the chance I've been waiting for?");
    zsay(pleasant, "That is wonderful news. You've done well, Hisami.");

    say(hisami, "Ehehe~");

    zsay(gloat, "It's time to make a mockery of the Council, and rule Hell myself.");
    zsay(smirk, "Go back to the Animal Realm, and spread more rumours of that beast's power. Add fuel to the fire! Return to me in two days.");
    
    show(hisami, joy);
    say(hisami, "As you wish, Lady Zanmu~!");
    hide();

    zthink(smirk, "I need to prepare the waters...");
    zthink(smirk, "First, I'll make the Council weaken their own grip on the Animal Realm. That'll serve well to make them cower.");
    zthink(neutral, "I should talk to Hoshiguma. She's always been squeamish of the punishments wrought on the animals. That, and she has enough influence on the Council to sway it my way.");

    script.SetNextScene(Scenes::LobbyingYuugi);
    end;
}

SCENE_RUN(LobbyingYuugi)
{
    SCENE_SETUP();

    scene(hell_city);
    wait_for_tasks();

    say(zanmu, "Hoshiguma, thanks for the meet up.");
    show(yuugi, neutral);
    say(yuugi, "You're lucky I like you, Nippaku. How many times have you turned down my party invitations?");

    say(zanmu, "Ehe... Eighteen times... I've been busy?");

    say(yuugi, "Don't worry about it. Sometimes I feel bad seeing you work so hard whilst I merely drink and relax. Sometimes.");
    say(yuugi, "Anyway, work is why you wanted to meet, right?");

    think("Hoshiguma prefers people to be direct. But let's start with flattery.");

    say(zanmu, "Sure, but we can shoot the breeze first. I'm in no hurry.");

    say(yuugi, "Great! I miss this side of you.");
    say(yuugi, "Hear me out on this, then. The other day, ");

    s8 opinion = 0;
    // TODO: part one for opinion gaining

    say(zanmu, "Yes. I need help passing a Council vote. I thought it would be particularly up your alley.");
    say(zanmu, "My subordinate has reminded me of the horribly harsh conditions of the spirits in the Animal Realm.");

    {
        choice(
            "\"You wouldn't want them to suffer much longer, right?\"",
            "\"And you've done nothing, don't you care about them?\"",
            "\"We should work together to ease the worst of it.\"",
        );
        auto const res = get_choice_result();
        switch(res)
        {
        case 0:
        {
            opinion -= 1;
            say(yuugi, "Of course I wouldn't, give me a break!");

            think("Oops. I need to remember not to pressure her.");
            break;
        }
        case 1:
        {
            opinion -= 2;
            say(yuugi, "What the hell? That better be a joke. I've done a far sight more than you!");

            say(zanmu, "I... uh, yeah, a bad joke. Hahaha!");
            think("I can't afford to insult her again.");
            break;
        }
        case 2:
        {
            opinion += 1;
            say(yuugi, "Huh. Yeah, we should!");
            break;
        }
        }
    }

    say(yuugi, "What's with the change of heart? You've never brought it up before.");

    say(zanmu, "Well, I needn't pretend. I took note of how the level of misery was greatly impacting productivity.");
    say(zanmu, "Hell's economy is totally anaemic! We're squeezing the life out of it by stamping the boot too hard.");

    say(yuugi, "Hahaha. Nothing quite like a managerial problem to get the old monk in you fired up.");
    say(yuugi, "It's true how much excessive torture damages the economy, even if it's not why I care. I'll mention that next time I talk to the other oni.");

    say(zanmu, "Great! They will appreciate your more wholistic worldview, I am sure.");
    say(zanmu, "In any case, let me be specific. My proposal will be to end solitary confinements and provide sustenance even to those undergoing torture.");

    say(yuugi, "You'd go that far..? That's a huge step! But good!");
    say(yuugi, "What can I do, though?");

    say(zanmu, "I just need you to rally the oni in your circle. Combined with those I can account for myself, it should be enough.");

    say(yuugi, "Is that really enough, though? I just need to talk to a few oni?");

    say(zanmu, "No, not just talk. I'm trying to *win* this vote, not make a moral statement.");

    {
        choice(
            "\"You know them best. Get their hearts riled up!\"",
            "\"You need to guilt trip them.\"",
            "\"If it were me, I'd threaten them!\"",
            );
        auto const res = get_choice_result();
        switch(res)
        {
        case 0:
        {
            opinion += 3;
            say(yuugi, "Oh, hell yeah! Let's do it!");
            say(yuugi, "We're coming to free you, little bunny spirits!");
            break;
        }
        case 1:
        {
            opinion -= 1;
            say(yuugi, "...Really? I don't think that's going to help.");
            if (opinion > 0)
            {
                say(yuugi, "Still...");
            }
            break;
        }
        case 2:
        {
            opinion -= 2;
            say(yuugi, "Noted, Nippaku. But I only threaten my enemies.");
            say(yuugi, "If that's really how you think... I'd rather you didn't let it slip.");
            if (opinion > 0)
            {
                say(yuugi, "Still...");
            }
            break;
        }
        }
    }

    bool const hasYuugiInfluence = opinion > 0;
    io_game.SetVar<Variables::HasYuugiInfluence>(hasYuugiInfluence);
    if (hasYuugiInfluence)
    {
        say(yuugi, "I think this has a chance to work.");

        say(zanmu, "Of course it does, I'm involved.");

        say(yuugi, "Hahaha! How quick your arrogance returns now I'm on board!");
        say(yuugi, "If you're holding the vote tomorrow, I'll make the rounds today.");
        say(yuugi, "It was a nice chat. Later, Nippaku.");
        // TODO-INFLUENCE
    }
    else
    {
        say(yuugi, "Ah...");
        say(yuugi, "You know, I'm having second thoughts. Call it an oni's hunch.");

        say(zanmu, "Wait, why-");

        say(yuugi, "Nope, I'm wise to your trickery, Nippaku, I'm not going to hear you out.");
        say(yuugi, "Thanks for the chat, though, I'd genuinely like that again.");

        say(zanmu, "Mhm. Alright then.");
        think("Frustrating. I'll have to make do with the allies I have.");
    }

    script.SetNextScene(Scenes::VotingForAnimalRights);
    end;
}

SCENE_RUN(VotingForAnimalRights)
{
    SCENE_SETUP();

    scene(kishin_council);
    wait_for_tasks();

    show(zanmu, neutral);
    say(zanmu, "Wise Council, allow me to discuss something that's come to my attention.");
    say(zanmu, "We've not paid much mind to the Animal Realm recently, given the problems everywhere else have been greater.");
    say(zanmu, "But even there, since a few decades ago, productivity is way down. Income is far outstripped by the costs to keep the animal spirits in check.");
    say(zanmu, "This I have pinpointed to the usage of solitary confinements, primarily. The spirit of the animal spirits is broken, and they're deliberately avoiding work by seeking the punishments!");
    say(zanmu, "We should end the use of such a destructive punishment. But furthermore, we need to add some motivation so they don't find another trick. We should invest money in food for all spirits.");
    say(zanmu, "This may sound unorthodox, but let me tell you, every time we've increased punishments, the situation has become worse. Trying the opposite is surely worth a try?");

    say(speaker, "An interesting proposal. Any opinions?");

    // TODO: has yuugi's influence
    if (io_game.ReadVar<Variables::HasYuugiInfluence>())
    {
        show(yuugi, angry);
        say(yuugi, "We gotta do this! The animal spirits need some relief!");
    }
    else
    {
        think("I see Hoshiguma's staying quiet.");
    }

    // TODO-VOTE: with influence from yuugi

    show(zanmu, neutral);
    if (io_game.ReadVar<Variables::PunishmentVotePasses>())
    {
        say(zanmu, "Wonderful!");
        say(zanmu, "I'll take this into my hands and notify the guards of the Animal Realm right away. I promise, this won't be a mistake.");
        think("This is perfect. My plan is well under way now.");
        // TODO-INFLUENCE: further influence boost
        
        if (io_game.ReadVar<Variables::HasYuugiInfluence>())
        {
            show(yuugi, neutral);
            say(yuugi, "Thank you, Nippaku. I couldn't have done that by myself. I'm certain, this is the right thing to have done.");

            think("The right thing to guarantee an uprising, sure!");
            say(zanmu, "It is as you say~");
        }
    }
    else
    {
        say(zanmu, "*sigh*, what a pity.");
        if (io_game.ReadVar<Variables::HasYuugiInfluence>())
        {
            show(yuugi, dejected);
            say(yuugi, "Damn it... I really thought we had a chance. Have you lost your touch, Nippaku?");
            
            say(zanmu, "An oni doesn't give up at the first loss. I'll get my way eventually.");
        }
    }

    script.SetNextScene(Scenes::SuikaApproaches);
    end;
}

// TODO: maybe an intermission where Hisami spreads fervour with the eagle spirits?

SCENE_RUN(SuikaApproaches)
{
    SCENE_SETUP();

    scene(zanmu_study);
    wait_for_tasks();

    desc("Your study feels sterile, yet stale, at the same time.");

    think("With that out the way, I need to work on the next step.");
    think("I'll speak to Hisami about it tomorrow-");

    desc("An oni with towering horns firmly pushes aside the shoji door.");

    say(zanmu, "Suika? What a pleasant surprise.");

    show(suika, bigjoy);
    say(suika, "Zanmu!");
    say(suika, "I'm glad you're glad to see me. Is the potted plant around?");

    say(zanmu, "She's on an errand.");

    show(suika, neutral);
    say(suika, "Good, 'cause I wanted to talk with you *extra* privately.");
    say(suika, "I was puzzling over your recent proposals in the council meetings. I reckon you're up to something.");
    say(suika, "You don't *have* to tell me, but we're friends, right? Maybe I'll even support you.");

    think("Suika always hides her true intentions under that playful mask.");
    think("I need to think over carefully if she can be trusted.");

    say(zanmu, "Straight to the point, huh?");

    // Default to false, is set to true if persuasion is successful
    io_game.SetVar<Variables::SuikaDissuaded>(false);

    {
        choice(
            "Share your plans with Suika",
            "Keep the plans secret",
        );
        auto const res = get_choice_result();
        if (res == 1)
        {
            say(zanmu, "I'm not scheming anything. Has it become wrong to propose improvements to Hell?");

            show(suika, pout);
            say(suika, "Ehhhh...");
            say(suika, "Fine! But if I find out you just lied, I don't care what it is. I'll fight you down.");

            think("Oh dear. Suika has considerable rapport with the other kishin. Votes might be harder to secure...");

            script.SetNextScene(Scenes::DelegatingToHisami);
            end;
        }
    }

    say(zanmu, "You're very observant. It's true, I'm scheming~");
    
    say(suika, "Woohoo! I knew it! What're you doing?");

    say(zanmu, "I'm going to take over Hell, and make it run smoothly again.");

    show(suika, neutral);
    say(suika, "...Huh?");
    say(suika, "What do you mean you're going to take over?");

    think("Ack. Suika's horrified. I need to ");
    
    {
        // TODO
        choice(
            "\"I'll be a true administrator.\"",
            "\"I'll be Hell's king.\"",
            "\"I'll control the Council as my puppets.\"",
        );
        auto const res = get_choice_result();
        switch(res)
        {
        case 0:
        {
            break;
        }
        case 1:
        case 2:
        {
            say(suika, "You can't be serious!");

            say(zanmu, "It's a serious situation.");

            say(suika, "Damn! I thought you were just trying to poke fun at the Council. But this'll be going too far, Zanmu!");
            say(suika, "I need to warn the oni!");
            hide();

            think("Hm. It's within tolerance, but I won't deny she's going to make things harder.");
            end;
        }
        }
    }
    
    say(suika, "I see. You've always been good at that, I suppose.");
    say(suika, "How're you gonna do it?");

    script.SetNextScene(Scenes::DelegatingToHisami);
    end;
}

SCENE_RUN(DelegatingToHisami)
{
    SCENE_SETUP();

    scene(zanmu_study);
    wait_for_tasks();

    desc("It's been two and a half days since the plan began.");

    show(hisami, joy);
    say(hisami, "I'm baaaaaaaack~!");

    zsay(pained, "You're 11 hours late. What were you even doing?");

    show(hisami, double);
    say(hisami, "I was doing exactly as you asked, and telling every animal spirit to follow the beast.");
    say(hisami, "Then that beast found me and beat me up...");

    zsay(pained, "Huh?! Why would they oppose your actions?");

    show(hisami, neutral);
    say(hisami, "She said she wasn't interested in growing her ranks.");
    say(hisami, "An arrogant type indeed!");

    zthink(gloat, "Now that's interesting... I'll need to meet with this so-called leader myself.");
    zthink(neutral, "But before I get round to that, I need to prepare for another step of the plan.");
    zthink(neutral, "I've tallied the Council members, and perhaps half would be willing to side with me if given reason.");
    zthink(smirk, "So, it's time to create that reason.");

    zsay(neutral, "I'll deal with the beast later. First, we need to lose a vote.");

    show(hisami, double);
    say(hisami, "...Lose?");
    
    zsay(smirk, "Yep. I'll make the proposal, and it'll be the easiest proposal to pass.");
    zsay(smirk, "And by losing it on purpose, the Council will feel for themselves the frustration of the system.");
    zsay(gloat, "Maybe they'll even have a little empathy for me!");

    show(hisami, joy);
    say(hisami, "I see~! In that case, how can I be of service?");

    zsay(neutral, "I'm going to propose that we raise the prices to cross the Sanzu by 10%.");
    zsay(neutral, "I want you to...");

    {
        choice(
            "\"Lobby the Yama. Get them to voice support.\"",
            "\"Spread doubt among the lesser oni.\"",
            "\"Reach out to a senior kishin for an alliance.\""
        );
        auto const res = get_choice_result();

        show(hisami, neutral);
        say(hisami, "As you command, Lady Zanmu~!");
        hide();

        io_game.SetVar<Variables::LobbiedTheYama>(res == 0);
        io_game.SetVar<Variables::SpreadSeedsOfDoubt>(res == 1);
        io_game.SetVar<Variables::KishinAlliance>(res == 2);
        switch(res)
        {
        case 0:
        {
            zthink(neutral, "That will make the vote harder to lose.");
            zthink(gloat, "But if it does, the effect on the Council will be twice as strong!");
            break;
        }
        case 1:
        {
            zthink(neutral, "That should make this vote easier to lose.");
            zthink(pleasant, "It's good to save influence for later.");
            break;
        }
        case 2:
        {
            zthink(neutral, "That probably won't change anything about this next vote, but I will do well to have that alliance in the future.");
            break;
        }
        }
    }

    script.SetNextScene(Scenes::VotingForPriceIncreases);
    end;
}

SCENE_RUN(VotingForPriceIncreases)
{
    SCENE_SETUP();

    scene(kishin_council);
    wait_for_tasks();

    // TODO

    //script.SetNextScene(Scenes::MeetingTheShadowyKishin);
    script.SetNextScene(Scenes::EngagingYuuma);
    end;
}

/*SCENE_RUN(MeetingTheShadowyKishin)
{
    SCENE_SETUP();

    scene(hell_palace);
    wait_for_tasks();

    // TODO

    script.SetNextScene(Scenes::EngagingYuuma);
    end;
}*/

SCENE_RUN(EngagingYuuma)
{
    SCENE_SETUP();

    scene(animal_realm);
    wait_for_tasks();

    // music(shining_law_of_the_strong_eating_the_weak, 0.0, true);

    zthink(neutral, "It should be around here.");
    zthink(pained, "Ah, I've been spotted.");

    say_hidden(yuuma, "Oi. Scram, oni.");

    zsay(smirk, "It's Nippaku.");

    bool const punishmentVotePassed = io_game.ReadVar<Variables::PunishmentVotePasses>();
    bool const priceIncreaseVoteFailed = io_game.ReadVar<Variables::PriceIncreaseVoteFails>();

    show(yuuma, neutral);
    say_hidden(yuuma, "Nippaku...");

    u8 flattery = 0;
    if (punishmentVotePassed)
    {
        ++flattery;
        show(yuuma, pleased);
        say_hidden(yuuma, "Ah! You're the one who cut back our punishments.");
        say_hidden(yuuma, "Hehehe. You must be scheming something good.");
    }
    else
    {
        show(yuuma, annoyed);
        say_hidden(yuuma, "The bleeding heart making symbolic votes over our conditions?");

        choice(
            "\"I'm sorry it didn't pass.\"",
            "\"The oni are shortsighted.\""
        );
        auto const res = get_choice_result();
        switch(res)
        {
            case 0:
            {
                say_hidden(yuuma, "Whatever.");
                zthink(pained, "...");
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

    show(yuuma, neutral);
    say(yuuma, "I'm Yuuma. What do you want?");

    zsay(neutral, "I wanted to see who was acting as a leader to the eagle spirits. I'm keen to see them prosper.");

    think("A taotie is their leader? She might turn out to be a loose cannon. Still, I need to get on her good side...");
    
    // TODO: a bit more chatter to get to know Yuuma?
    
    think("What I say next will be very important.");

    bool freeAnimalRealm = false;
    {
        choice(
            "Raise the idea of a liberated Animal Realm",
            "Offer advice on growing her organisation",
        );
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
            show(yuuma, annoyed);
            say(yuuma, "For one, you managed to get the Sanzu's prices raised on your own. Why do you need me for leverage?");
        }

        show(yuuma, pleased);
        say(yuuma, "Even if you don't deceive, there's no way you'd get that sort of power. Why wouldn't I just take control myself anyway, if a few eagles and a spork is all it takes!");

        think("Damn! She's feeling threatened. I'll need to make her feel in control.");

        bool failedBothFlattery = true;
        // Flattery 1
        {
            choice(
                "\"The oni aren't weak. An uprising alone isn't enough.\"",
                "\"It's true, an uprising could work.\"",
                "\"This is the only way with long term success.\"",
            );
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
            choice(
                "\"\"",
                "\"I'll tell everyone of your plans if you don't help.\"",
                "\"\"",
            );
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
    say(speaker, "Go ahead, Nippaku.");
    say(zanmu, "I have spent the last few days surveying the realms again, as I often do. I've seen decay, paralysis, and overpopulation. This council has... struggled to respond over the years.");
    say(zanmu, "But it's reaching a breaking point. If this goes on much longer, the Yama will turn elsewhere to send damned souls.");
    say(zanmu, "I see no other choice but this: we must empower one of us as an executive that can lead Hell out of its rot.");
    say(zanmu, "They must be untethered by the debates of this council.");
    say(zanmu, "They must have a singular strong vision, to end this slow death by committee.");

    say(acouncil, "What?");
    say(bcouncil, "Preposterous!");
    say(ccouncil, "She has a point, we barely achieve anything.");
    say(dcouncil, "None of us deserve to rule without restriction.");
    say(bcouncil, "Exactly! This is a power move!");

    think("They're taking it about as well as I expected.");

    say(speaker, "...QUIET!");
    say(speaker, "Nippaku, your proposal is acknowledged, let's get it out of the way. Hopefully a *productive* meeting can follow.");

    // TODO-VOTE
    bool const voteSucceeded = {};

    if (voteSucceeded)
    {
        script.SetNextScene(Scenes::Ending_RulingDecay);
        end;
    }
    
    // TODO eh could be better
    say(zanmu, "I accept the council's decision, but I see it as a refusal of reality.");
    say(zanmu, "Nevertheless, I do acknowledge the risk of allocating supreme power. I still see another way forward, that avoids this risk.");
    say(zanmu, "I will take leave of the remainder of this meeting, and will raise a fresh idea in tomorrow's meeting.");

    hide();
    scene(zanmu_study);

    think("That was no surprise. It would have taken all my influence to win that vote.");
    think("If I could have ruled this Hell... it would have had a chance.");
    think("But maybe it was always going to be easier to take the more drastic option.");
    think("Tomorrow, I will propose a relocation, to a new Hell.");
    think("...");
    think("I should spend the rest of the day shoring up my support.");

    {
        choice(
            "Stay with Hisami",
            "Revisit Yuugi",
            "Revisit Suika",
            "Revisit Yuuma",
        );
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

    desc("Somehow, the same old study seems a little warmer, a little cozier.");

    show(hisami, neutral);
    say(hisami, "Lady Zanmu~! What do you need~?");

    zsay(pleasant, "Actually, Hisami, I thought it best to spend today relaxing.");

    say(hisami, "A-and you thought to invite me?");

    zsay(pained, "You're the only one who wouldn't get on my nerves right now!");

    show(hisami, flush);
    say(hisami, "Lady Zanmu...");

    zsay(neutral, "Let's chat a while.");
    zsay(neutral, "As you know, I chose to come to Hell. I've always been curious of what it's really like internally for those that don't choose.");
    zsay(neutral, "Tell me about someone interesting you dragged to Hell.");

    show(hisami, neutral);
    say(hisami, "Oho, Lady Zanmu's interested in my work~~");
    say(hisami, "... ... Oh! There was once a girl who seemed so confused of the situation.");
    say(hisami, "Each time I used one of my lines, she said she didn't care. Yet, she kept following me!");
    say(hisami, "I don't like people acting strange like that, so I told her where I was going.");
    show(hisami, joy);
    say(hisami, "And she said that she knew, and that she was trying to go there too!");
    say(hisami, "She even kept saying things like \"Don't you know who I am?\" and \"Have you never seen me before?\"");

    zthink(unamused_sweat, "...");

    show(hisami, neutral);
    say(hisami, "But why would I know that? I meet so many, it is hard to remember all their faces.");
    say(hisami, "Just as we got to the gate, she stopped following. I had to ask, \"Are you not coming into Hell after all?\"");
    say(hisami, "But she only mumbled some nonsense about having arrived to do her job!");

    zsay(pleasant, "Hisami. Perhaps this girl was a chicken?");

    show(hisami, joy);
    say(hisami, "Yes~ Many are too chicken to enter hell.");

    zsay(pained, "No, I meant a literal chicken.");

    show(hisami, neutral);
    say(hisami, "Oh~! Perhaps she was.");

    zsay(pleasant, "Poor Niwatari. Forgotten and considered a weirdo by the one who sees her every day.");

    say(hisami, "... Oho~!");

    zsay(pained, "Well, it wasn't quite what I was looking for, but at least we passed the time. Thanks, Hisami.");

    show(hisami, flush);
    say(hisami, "Of course, Lady Zanmu~");

    zthink(neutral, "Okay, that's enough time wasting. I should get ready for tomorrow.");

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

    // TODO

    end;
}


SCENE_RUN(Ending_RulingDecay)
{
    SCENE_SETUP();

    // TODO

    end;
}

SCENE_RUN(Ending_NewHell)
{
    SCENE_SETUP();

    // TODO

    end;
}

SCENE_RUN(Ending_AdministratingCollapse)
{
    SCENE_SETUP();

    // TODO

    end;
}

}