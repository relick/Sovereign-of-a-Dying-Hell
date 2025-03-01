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

#define get_influence() io_game.ReadVar<Variables::Influence>()
#define set_influence(AMOUNT) io_game.SetVar<Variables::Influence>(AMOUNT)
#define add_influence(AMOUNT) set_influence(get_influence() + AMOUNT);
#define zsay(FACE, TEXT) say_face(zanmu, FACE, TEXT)
#define zthink(FACE, TEXT) think_face(zanmu, FACE, TEXT)
#define suika_cost() (io_game.ReadVar<Variables::SuikaDissuaded>() ? 0 : 12)

#define TEST_SKIP_TO_VOTE 0

namespace Jam15
{

SCENE_RUN(FirstVoteProposal)
{
    SCENE_SETUP();

    scene(kishin_council);
    //play_music(faron, true);
    wait_for_tasks();

#if !TEST_SKIP_TO_VOTE
    desc("The musky chamber is packed with oni, of low and high rank alike.");
    desc("Those of the highest rank sit in the centre, in irreverent, informal seiza.");
    desc("The oni make up the Council, and the kishin lead its ferocious debates.");
    desc("It's once again your turn to speak.");

    show(zanmu, neutral);
    say(zanmu, "Oni Council, allow me my third proposal for today.");
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
    say(speaker, "That's enough back and forth. Let's not waste time.");
    say(speaker, "Council, declare your votes!");
#endif

    desc("You're about to start a vote.");
    desc("During votes, you will have a limited time to spend gathered influence to counteract the onslaught of debate from the opposing side.");
#if WEB_BUILD
    desc("Spend influence by mashing the space bar!");
#else
    desc("Spend influence by mashing A/B/C on the gamepad!");
#endif
    set_influence(64);

    // It's impossible to win this
    Game::VoteModeParams const vote = {
        .m_voteName = "Use the earth spirits to maintain the torture chambers",
        .m_votingTime = FIX16(5),
        .m_startingPlayerInfluence = get_influence(), // Fixed influence to start with
        .m_attackSize = 128, // More attack than influence + enough to win
        .m_attackPattern = Game::AttackPattern::FastBitty,
        .m_playerWantsToLose = false,
        .m_easyMode = script.IsMashlessMode(),
    };
    start_vote(vote);

    say(speaker, "A decisive defeat. No more proposals from you today, Nippaku, please.");

    show(zanmu, neutral);
    say(zanmu, "The lack of wisdom in this Council once again astounds.");
    say(zanmu, "Fine, argue over seating arrangements, or whatever it is you do, I will attend to real business.");

#if TEST_SKIP_TO_VOTE
    script.SetNextScene(Scenes::VotingForAnimalRights);
#else
    script.SetNextScene(Scenes::HatchingThePlan);
#endif
    end;
}

SCENE_RUN(HatchingThePlan)
{
    SCENE_SETUP();

    scene(zanmu_study);
    wait_for_tasks();

    desc("You return to your study. It's cold and minimally decorated.");
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
    zthink(neutral, "Then, I'll prove the process is charade with a sham vote.");
    zthink(gloat, "If all goes well, I'll finish with a vote to install me as sole ruler!");
    zthink(neutral, "Okay, for the first vote, I should talk to Hoshiguma. She's always been squeamish of the punishments wrought on the animals. That, and she has enough influence on the Council to sway it my way.");

    script.SetNextScene(Scenes::LobbyingYuugi);
    end;
}

SCENE_RUN(LobbyingYuugi)
{
    SCENE_SETUP();

    scene(hell_city);
    wait_for_tasks();

    zsay(pleasant, "Hoshiguma, thanks for the meet up.");
    show(yuugi, neutral);
    say(yuugi, "You're lucky I like you, Nippaku. How many times have you turned down my party invitations?");

    zsay(pained, "Ehe... Eighteen times... I've been busy?");

    say(yuugi, "Don't worry about it. Sometimes I feel bad seeing you work so hard whilst I merely drink and relax. Sometimes.");
    say(yuugi, "Anyway, work is why you wanted to meet, right?");

    zthink(pleasant, "Hoshiguma prefers people to be direct. But let's start with flattery.");

    zsay(pleasant, "Sure, but we can shoot the breeze first. I'm in no hurry.");

    show(yuugi, angry);
    say(yuugi, "Great! I miss this side of you.");
    say(yuugi, "Hear me out, then. The other day...");

    hide();

    s8 opinion = 0;
    desc("Yuugi spends a few hours regaling you with her feats of strength.");

    show(yuugi, neutral);

    zthink(pained, "This is going on too long.");
    zsay(pained, "Sorry Hoshiguma, could we talk about work now?");

    show(yuugi, dejected);
    say(yuugi, "Aww. Alright then. What did you need?");

    zsay(neutral, "I need help passing a Council vote. I thought it would be particularly up your alley.");
    zsay(neutral, "My subordinate has reminded me of the horribly harsh conditions of the spirits in the Animal Realm.");

    {
        choice(
            "\"You don't want them to suffer, right?\"",
            "\"And you've done nothing. Don't you care?\"",
            "\"Let's work together to ease the worst of it.\"",
        );
        auto const res = get_choice_result();
        switch(res)
        {
        case 0:
        {
            show(yuugi, angry);
            say(yuugi, "Of course I wouldn't, give me a break!");

            zthink(pained, "Oops. I need to remember not to pressure her.");
            break;
        }
        case 1:
        {
            opinion -= 1;
            show(yuugi, angry);
            say(yuugi, "What the hell? That better be a joke. I've done a far sight more than you!");

            zsay(pained, "I... uh, yeah, a bad joke. Hahaha!");
            zthink(unamused_sweat, "I can't afford to insult her again.");
            break;
        }
        case 2:
        {
            opinion += 1;
            show(yuugi, neutral);
            say(yuugi, "Huh. Yeah, we should!");
            break;
        }
        }
    }

    show(yuugi, neutral);
    say(yuugi, "What's with the change of heart? You've never brought it up before.");

    zsay(neutral, "I needn't pretend with you, Hoshiguma. I took note of how the level of misery was greatly impacting productivity.");
    zsay(neutral, "Hell's economy is totally anaemic! We're squeezing the life out of it by stamping the boot too hard.");

    say(yuugi, "Hahaha. Nothing quite like a managerial problem to get the old monk in you fired up.");
    say(yuugi, "It's true how much excessive torture damages the economy, even if it's not why I care. I'll mention that next time I talk to the other oni.");

    zsay(smirk, "Great! They will appreciate your more wholistic worldview, I am sure.");
    zsay(neutral, "In any case, let me be specific. My proposal will be to end solitary confinements and provide sustenance even to those undergoing torture.");

    say(yuugi, "You'd go that far..? That's a huge step! But good!");
    say(yuugi, "What can I do, though?");

    zsay(neutral, "I just need you to rally the oni in your circle. Combined with those I can account for myself, it should be enough.");

    say(yuugi, "Is that really enough, though? I just need to talk to a few oni?");

    zsay(smirk, "No, not just talk. I'm trying to *win* this vote, not make a moral statement.");

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
            opinion += 1;
            show(yuugi, angry);
            say(yuugi, "Oh, hell yeah! Let's do it!");
            say(yuugi, "We're coming to free you, little bunny spirits!");
            break;
        }
        case 1:
        {
            show(yuugi, dejected);
            say(yuugi, "...Really? I don't think that's going to help.");
            if (opinion >= 0)
            {
                say(yuugi, "Still...");
            }
            break;
        }
        case 2:
        {
            opinion -= 1;
            show(yuugi, neutral);
            say(yuugi, "Noted, Nippaku. But I only threaten my enemies.");
            say(yuugi, "If that's really how you think... I'd rather you didn't let it slip.");
            if (opinion >= 0)
            {
                say(yuugi, "Still...");
            }
            break;
        }
        }
    }

    show(yuugi, neutral);

    bool const hasYuugiInfluence = opinion >= 0;
    io_game.SetVar<Variables::HasYuugiInfluence>(hasYuugiInfluence);
    if (hasYuugiInfluence)
    {
        say(yuugi, "I think this has a chance to work.");

        zsay(gloat, "Of course it does, I'm involved.");

        say(yuugi, "Hahaha! How quick your arrogance returns now I'm on board!");
        say(yuugi, "If you're holding the vote tomorrow, I'll make the rounds today.");
        say(yuugi, "It was a nice chat. Later, Nippaku.");

        desc("You gained Yuugi's influence for the next vote.");
    }
    else
    {
        say(yuugi, "Ah...");
        say(yuugi, "You know, I'm having second thoughts. Call it an oni's hunch.");

        zsay(unamused_sweat, "Wait, why-");

        say(yuugi, "Nope, I'm wise to your trickery, Nippaku, I'm not going to hear you out.");
        say(yuugi, "Thanks for the chat, though, I'd genuinely like that again.");

        zsay(unamused, "Mhm. Alright then.");
        zthink(unamused_sweat, "Frustrating. I'll have to make do with the allies I have.");
    }

    script.SetNextScene(Scenes::VotingForAnimalRights);
    end;
}

SCENE_RUN(VotingForAnimalRights)
{
    SCENE_SETUP();

    scene(kishin_council);
    wait_for_tasks();

#if !TEST_SKIP_TO_VOTE
    show(zanmu, neutral);
    say(zanmu, "Wise Council, allow me to discuss something that's come to my attention.");
    say(zanmu, "We've not paid much mind to the Animal Realm recently, given the problems everywhere else have been greater.");
    say(zanmu, "But even there, since a few decades ago, productivity is way down. Income is far outstripped by the costs to keep the animal spirits in check.");
    say(zanmu, "This I have pinpointed to the usage of solitary confinements, primarily. The spirit of the animal spirits is broken, and they're deliberately avoiding work by seeking the punishments!");
    say(zanmu, "We should end the use of such an economically destructive punishment. But furthermore, we need to create motivation so they don't merely search for another trick. We should invest money in food for all spirits.");
    say(zanmu, "This may sound unorthodox, but let me tell you, every time we've increased punishments, the situation has become worse. Trying the opposite is surely worth a try?");

    say(speaker, "An interesting proposal. Any opinions?");
#else
    io_game.SetVar<Variables::HasYuugiInfluence>(true);
#endif

    // Start on 100 or 150 now
    set_influence(128);

    if (io_game.ReadVar<Variables::HasYuugiInfluence>())
    {
        show(yuugi, angry);
        say(yuugi, "We gotta do this! The animal spirits need some relief!");
        add_influence(32);
    }
    else
    {
        zthink(unamused_sweat, "I see Hoshiguma's staying quiet.");
    }

    //desc("From now on, your influence is carried over between votes. Spend it wisely!");
    desc("The vote is about to start.");
    Game::VoteModeParams const vote = {
        .m_voteName = "End solitary punishment, and feed the animals",
        .m_votingTime = FIX16(5),
        .m_startingPlayerInfluence = get_influence(),
        .m_attackSize = 96,
        .m_attackPattern = Game::AttackPattern::SlowChunky,
        .m_playerWantsToLose = false,
        .m_easyMode = script.IsMashlessMode(),
    };
    start_vote(vote);
    Game::VoteResult const result = get_vote_result();

    if (io_game.ReadVar<Variables::HasYuugiInfluence>())
    {
        add_influence(-32);
    }
    //set_influence(result.m_remainingInfluence);
    io_game.SetVar<Variables::PunishmentVotePasses>(result.m_playerWon);

    if (io_game.ReadVar<Variables::PunishmentVotePasses>())
    {
        say(speaker, "It seems that passed.");

        zthink(pleasant, "That went perfectly!");
        desc("Winning the vote has secured you lasting influence with the Council");
        add_influence(64);

        show(zanmu, neutral);
        say(zanmu, "Wonderful!");
        say(zanmu, "I'll take it into my hands and notify the guards of the Animal Realm right away. This decision is absolutely the right one.");
        zthink(gloat, "My plan is well under way now.");
        
        if (io_game.ReadVar<Variables::HasYuugiInfluence>())
        {
            show(yuugi, neutral);
            say(yuugi, "Thank you, Nippaku. I couldn't have done that by myself. I'm certain, this is the right thing to have done.");

            zthink(gloat, "The right thing to guarantee an uprising, sure!");
            say(zanmu, "It is as you say~");
        }
    }
    else
    {
        say(speaker, "Sorry, no dice.");

        show(zanmu, neutral);
        say(zanmu, "*sigh*, what a pity.");
        if (io_game.ReadVar<Variables::HasYuugiInfluence>())
        {
            show(yuugi, dejected);
            say(yuugi, "Damn it... I really thought we had a chance. Have you lost your touch, Nippaku?");
            
            zsay(pained, "An oni doesn't give up at the first loss. I'll get my way eventually.");
        }

        zthink(neutral, "Losing is a setback, but I can carry on with the plan.");
    }

#if TEST_SKIP_TO_VOTE
    script.SetNextScene(Scenes::VotingForPriceIncreases);
#else
    script.SetNextScene(Scenes::SuikaApproaches);
#endif
    end;
}

SCENE_RUN(SuikaApproaches)
{
    SCENE_SETUP();

    scene(hell_palace);
    wait_for_tasks();

    desc("Returning from the council meeting, you pass through the eery hallways of one of Hell's many palaces.");

    zthink(neutral, "With that out the way, I need to work on the next step.");
    zthink(neutral, "I'll speak to Hisami about it tomorrow-");

    desc("An oni with towering horns steps into your view.");

    show(suika, bigjoy);
    say(suika, "Zanmu!");

    zsay(pained, "Suika? What a pleasant surprise.");

    say(suika, "I'm glad you're glad to see me. Is the potted plant stalking us?");

    zsay(neutral, "She's on an errand.");

    show(suika, neutral);
    say(suika, "Good, 'cause I wanted to talk with you *extra* privately.");
    say(suika, "I was puzzling over your recent proposals in the Council meetings. I reckon you're up to something.");
    say(suika, "You don't *have* to tell me, but we're friends, right? Maybe I'll even support you.");

    zthink(neutral, "Suika always hides her true intentions under that playful mask.");
    zthink(neutral, "I need to think over carefully if she can be trusted.");

    zsay(smirk, "Straight to the point, huh?");

    // Default to false, is set to true if persuasion is successful
    io_game.SetVar<Variables::SuikaDissuaded>(false);

    {
        choice(
            "\"Fine, I'll tell you.\"",
            "\"I'm not scheming anything.\"",
        );
        auto const res = get_choice_result();
        if (res == 1)
        {
            zsay(smirk, "I'm not scheming anything. Has it become wrong to propose improvements to Hell?");

            show(suika, pout);
            say(suika, "Ehhhh...");
            say(suika, "Fine! But if I find out you just lied, I don't care what it is. I'll fight you down.");
            hide();

            zthink(unamused_sweat, "Oh dear. Suika has considerable rapport with the other kishin. Votes might be harder from now on...");

            script.SetNextScene(Scenes::DelegatingToHisami);
            end;
        }
    }

    zsay(smirk, "You're very observant. It's true, I'm scheming~");
    
    say(suika, "Woohoo! I knew it! What're you doing?");

    zsay(neutral, "I'm going to take over Hell, and make it run smoothly again.");

    show(suika, neutral);
    say(suika, "...Huh?");
    say(suika, "What do you mean you're going to take over?");

    zthink(pained, "Ack. Suika's horrified. I need to tone it way down.");
    
    {
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

            zsay(neutral, "It's a serious situation.");

            say(suika, "Damn! I thought you were just trying to poke fun at the Council. But this'll be going too far, Zanmu!");
            say(suika, "I need to warn the oni!");
            hide();

            zthink(unamused_sweat, "Hm. It's within tolerance, but I won't deny she's going to make votes harder from now on.");

            script.SetNextScene(Scenes::DelegatingToHisami);
            end;
        }
        }
    }
    
    say(suika, "I see. You've always been good at that, I suppose.");
    say(suika, "How're you gonna do it?");

    {
        choice(
            "\"With threats and violence.\"",
            "\"I'll put it to the vote.\"",
            );
        auto const res = get_choice_result();
        switch (res)
        {
        case 0:
        {
            show(suika, neutral);
            say(suika, "Nah, Zanmu, I can't condone that.");
            say(suika, "You're meant to be one of our smart ones! I really didn't take you for a meat-headed brute.");

            zthink(unamused_sweat, "Well, saying that got the unsurprising reaction. I was, uh, meant to tone it down. I suspect votes may be harder from now on.");
            break;
        }
        case 1:
        {
            show(suika, bigjoy);
            say(suika, "Good on ya, Zanmu! That's the way to do it.");
            say(suika, "Truth be told, I'm not planning on staying in Hell much longer anyway.");
            say(suika, "I don't like the atmosphere, and there's too many rules. I hear rumours of youkai trying to create some place on the surface, away from humans. Sounds kinda nice, right?");
            
            zsay(neutral, "Can't say I'm interested in such a place, but I understand wanting to leave. Take care, Suika.");
            zthink(pleasant, "I think that means she won't get in my way. That's good!");

            io_game.SetVar<Variables::SuikaDissuaded>(true);
            break;
        }
        }
    }

    show(suika, bigjoy);
    say(suika, "Well. Thanks for hearing me out! I'll leave you alone now.");

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

    zsay(neutral, "I'm going to propose that we raise the prices to cross the Sanzu.");
    zsay(neutral, "There's a number of things you could do to help.");
    zsay(neutral, "Perhaps you could lobby the Yama? If they voice support, it will be even harder to lose this proposal. But the psychological impact of such a surefire policy being rejected would be even greater on the Council.");
    zsay(neutral, "Or maybe, it's better to play it safe and spread doubt of the idea among the rest of the oni so it's more likely to fail from the get go.");
    if (io_game.ReadVar<Variables::SuikaDissuaded>())
    {
        zsay(neutral, "In fact, now that Suika's backed off, maybe one of her kishin friends would be open to an alliance. That could pay greater dividends in the long run.");
    }

    {
        if (io_game.ReadVar<Variables::SuikaDissuaded>())
        {
            choice(
                "\"Lobby the Yama. Get them to voice support.\"",
                "\"Spread doubt among the lesser oni.\""
            );
        }
        else
        {
            choice(
                "\"Lobby the Yama. Get them to voice support.\"",
                "\"Spread doubt among the lesser oni.\"",
                "\"Reach out to a senior kishin for an alliance.\""
            );
        }
        auto const res = get_choice_result();

        show(hisami, neutral);
        say(hisami, "As you command, Lady Zanmu~!");
        hide();

        io_game.SetVar<Variables::LobbiedTheYama>(res == 0);
        io_game.SetVar<Variables::SpreadSeedsOfDoubt>(res == 1);
        io_game.SetVar<Variables::KishinAlliance>(res == 2);

        zthink(neutral, "Good. We'll see how that plays out, then.");
    }

    script.SetNextScene(Scenes::VotingForPriceIncreases);
    end;
}

SCENE_RUN(VotingForPriceIncreases)
{
    SCENE_SETUP();

    scene(kishin_council);
    wait_for_tasks();

#if !TEST_SKIP_TO_VOTE
    show(zanmu, neutral);
    say(zanmu, "Thoughtful Council, I'd like your consideration on a small change.");
    say(zanmu, "I believe we should increase the prices for crossing the Sanzu a small amount, perhaps 10%.");
    say(zanmu, "Inflation on the surface has increased enough to make this affordable, and it would go a long way to making our balance sheets start to add up.");
    say(zanmu, "...");
    say(acouncil, "Is that the whole proposal?");
    say(zanmu, "Yes?");
    say(acouncil, "Hm. It sounds quite sensible - surprise to be sure to come from you, Nippaku!");
    say(bcouncil, "Yeah. I see nothing wrong with this.");
    say(speaker, "Seems opinion isn't divided, but let's vote anyway to be sure.");
#else
    io_game.SetVar<Variables::SuikaDissuaded>(true);
    io_game.SetVar<Variables::LobbiedTheYama>(false);
    io_game.SetVar<Variables::SpreadSeedsOfDoubt>(true);
    io_game.SetVar<Variables::KishinAlliance>(false);
#endif

    if (io_game.ReadVar<Variables::SpreadSeedsOfDoubt>())
    {
        desc("Hisami's work with the lesser oni has granted you extra influence for this vote only.");
        add_influence(32);
    }
    else if (io_game.ReadVar<Variables::KishinAlliance>())
    {
        desc("Hisami's work with a senior kishin has granted you lasting influence with the Council.");
        add_influence(32);
    }
    u16 attackSize = 128;
    if (io_game.ReadVar<Variables::LobbiedTheYama>())
    {
        desc("Word of the Yama's support has reached the oni, they will be harder to dissuade.");
        attackSize += 52;
    }
    desc("In this vote, you are trying to ensure it fails. That means you will spend influence to push the bar in the opposite direction to usual.");
    desc("The vote is about to start.");
    Game::VoteModeParams const vote = {
        .m_voteName = "Increase cost of crossing the Sanzu by 10%",
        .m_votingTime = FIX16(7),
        .m_startingPlayerInfluence = get_influence(),
        .m_attackSize = static_cast<u16>(attackSize + suika_cost()),
        .m_attackPattern = Game::AttackPattern::FastBitty,
        .m_playerWantsToLose = true,
        .m_easyMode = script.IsMashlessMode(),
    };
    start_vote(vote);
    Game::VoteResult const result = get_vote_result();

    if (io_game.ReadVar<Variables::SpreadSeedsOfDoubt>())
    {
        add_influence(-32);
    }
    // Min to account for the single vote bonus
    //set_influence(std::min<u16>(get_influence(), result.m_remainingInfluence));
    io_game.SetVar<Variables::PriceIncreaseVoteFails>(result.m_playerWon);

    if (io_game.ReadVar<Variables::PriceIncreaseVoteFails>())
    {
        say(speaker, "How surprising? I thought the sentiment was in your favour, Nippaku.");

        zthink(pleasant, "Oh, I think it went swimmingly!");
        if (io_game.ReadVar<Variables::LobbiedTheYama>())
        {
            desc("Losing the vote despite the Yama's support has secured you great and lasting influence with the Council.");
            add_influence(128);
        }
        else
        {
            desc("Losing the vote has secured you more lasting influence with the Council");
            add_influence(64);
        }

        show(zanmu, neutral);
        say(zanmu, "I'm sorry, Council, but I cannot fathom this. What the Hell poisoned your minds at the last moment?");
        say(zanmu, "If we can't even organise ourselves to pass the simplest, sanest changes, I have to wonder what the whole point of this Council is.");
        hide();

        zthink(smirk, "There, that should do nicely.");
    }
    else
    {
        say(speaker, "Passed, as expected.");

        zthink(unamused, "It's good to know the Council *can* pass some things, but that wasn't what I wanted this time.");

        show(zanmu, neutral);
        say(zanmu, "Thank you Council, I'll hand over to others now.");
        hide();
        
        if (!io_game.ReadVar<Variables::SuikaDissuaded>())
        {
            zthink(neutral, "Hmm. Suika was here today. She probably saw through my intentions...");
        }
    }

#if TEST_SKIP_TO_VOTE
    script.SetNextScene(Scenes::VotingForExecutive);
#else
    scene(zanmu_study);

    zthink(neutral, "Time to prepare for the big vote.");
    zthink(neutral, "I think I need to pay that visit to the beast in the Animal Realm.");
    zthink(gloat, "If I can make her do what I want, it's just the ticket I need to beat the Council.");

    //script.SetNextScene(Scenes::MeetingTheShadowyKishin);
    script.SetNextScene(Scenes::EngagingYuuma);
#endif
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

    show(yuuma, neutral);
    say_hidden(yuuma, "Nippaku...");

    u8 flattery = 0;
    if (io_game.ReadVar<Variables::PunishmentVotePasses>())
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
                zsay(gloat, "I've got greater visions, where the strong and weak both see their burdens eased.");
                say_hidden(yuuma, "Hah. Alright then.");
                break;
            }
        }
    }

    show(yuuma, neutral);
    say(yuuma, "I'm Yuuma. What do you want?");

    zsay(neutral, "I wanted to see who was acting as a leader to the eagle spirits. I'm keen to see them prosper.");

    zthink(pained, "A taotie is their leader? She might turn out to be a loose cannon. Still, I need to get on her good side...");
    zthink(neutral, "What I say next will be very important.");

    bool freeAnimalRealm = false;
    {
        choice(
            "\"Are you interested in a free Animal Realm?\"",
            "\"Why did you reject my help?\"",
        );
        auto const res = get_choice_result();
        freeAnimalRealm = res == 0;
    }
    
    if (!freeAnimalRealm)
    {
        zsay(pleasant, "You gave my subordinate some grief for helping you out.");
        zsay(neutral, "I'm not sure I understood why. Aren't you interested in growing your ranks?");

        show(yuuma, annoyed);
        say(yuuma, "Nah, not really. Some of the eagles just decided to start doing what I say after they saw my strength. Then they've spread the word among themselves.");
        say(yuuma, "Why do you care, though?");

        zsay(unamused, "In truth, I was hoping you could cause some havoc and threaten the oni. It sounds like I've had you pegged all wrong.");

        show(yuuma, pleased);
        say(yuuma, "Whoa whoa whoa, hold on. I didn't say I wouldn't start some trouble. Tell me more!");

        zsay(smirk, "Oho? Well then. My vision for Hell needs the Oni Council to surrender its power, somehow.");
        zsay(neutral, "I'm handling things up top, I just need a *big stick* to threaten them with.");
        zsay(gloat, "A powerful taotie and a legion of fired up eagle spirits ready to set the Animal Realm ablaze... that would be perfect.");

        show(yuuma, neutral);
        say(yuuma, "Can't say that doesn't sound fun, but you'll need to tell me more about that vision if I'm gonna go wrecking stuff for it.");
        
        choice(
            "\"I'll be the sole ruler of Hell, and fix it.\"",
            "\"Are you interested in a free Animal Realm?\"",
        );
        auto const res = get_choice_result();
        freeAnimalRealm = res == 1;
    }

    if (freeAnimalRealm)
    {
        // freeAnimalRealm

        show(yuuma, double);
        say(yuuma, "Huh?");

        zsay(neutral, "Let me explain. There's a bigger picture than the oni can see as they squabble over tiny details. The current system isn't working.");
        zsay(smirk, "That's especially true here! The enslaved animal spirits are supposed to be the backbone of Hell's economy...");
        zsay(smirk, "But with so little freedom, they barely do any work. They have no loyalty or morale.");

        show(yuuma, annoyed);
        say(yuuma, "You don't sound so different from the other oni if all you care about is the economy?");

        zthink(unamused, "Hmm... she's not throwing me any bones.");

        zsay(neutral, "Actually... I see the problem is the oni's involvement in the economy in the first place.");
        zsay(neutral, "They plan everything out, because they can't let go of even the smallest drop of their power.");
        zsay(neutral, "That's what makes them miss all the opportunities that only individuals can pursue.");

        show(yuuma, pleased);
        say(yuuma, "Hah! That is more my language. And so you want that for the Animal Realm?");

        zsay(smirk, "The Animal Realm understands the only rule that truly matters: 'the strong eat the weak'. I see no reason not to simply put that into practice.");
        zsay(gloat, "Set the whole Realm free, no hierarchy, no slavery from above. Let beasts and animals rule themselves.");

        show(yuuma, neutral);
        say(yuuma, "The eagle spirits follow my words because they've seen my strength. The rule is as you say...");
        say(yuuma, "I'm game for that idea. Tell me what you wanted of me, then?");

        zsay(neutral, "The threat of you and the eagles starting an uprising and taking territory. That's an essential bargaining chip with the oni.");
        zsay(smirk, "They will vote for my plans believing it will prevent your uprising, and in doing so I will deliver the free Animal Realm regardless.");

        show(yuuma, double);
        say(yuuma, "Hahahah!");
        say(yuuma, "I don't trust you one bit!");

        if (io_game.ReadVar<Variables::PriceIncreaseVoteFails>())
        {
            ++flattery;
        }
        else
        {
            show(yuuma, annoyed);
            say(yuuma, "For one, I heard you managed to get the Sanzu's prices raised on your own. Why do you need me for leverage?");
        }

        show(yuuma, pleased);
        say(yuuma, "Even if you don't deceive, there's no way you'd get that sort of power. Why wouldn't I just take control myself anyway, if a few eagles and a spork is all it takes!");

        zthink(unamused_sweat, "Damn! She's feeling threatened. I'll need to make her feel in control.");

        bool failedBothFlattery = true;
        // Flattery 1
        {
            choice(
                "\"The oni aren't weak. You'd be crushed.\"",
                "\"It's true, an uprising could work.\"",
                "\"This is the only way with long term success.\"",
            );
            auto const res = get_choice_result();
            switch (res)
            {
            case 1:
            {
                failedBothFlattery = false;
                zsay(smirk, "You could carve out your own niche of the Animal Realm, give it a name, and rule it as you like.");
                zsay(gloat, "But that's where it would end, there would be no more chances for more territory. The rest of Hell would resist fiercely and fight for it back.");

                show(yuuma, annoyed);
                say(yuuma, "Hmph. Maybe so. Defending it for eternity sounds exhausting.");

                zthink(smirk, "Good, she liked that.");
                break;
            }
            default:
            {
                zsay(smirk, "You'd be crushed if you tried. Only my plan will work.");

                say(yuuma, "Why do you think calling me weak or stupid would get you anywhere?");

                zthink(unamused_sweat, "Argh, she's more annoyed now.");
                break;
            }
            }
        }

        // Flattery 2
        {
            zthink(smirk, "Now to sweeten the deal.");

            choice(
                "\"If you don't help, I'll crush you myself.\"",
                "\"You'd be a leader in a free Animal Realm.\"",
            );
            auto const res = get_choice_result();
            switch (res)
            {
            case 0:
            {
                show(yuuma, annoyed);
                say(yuuma, "If you want to fight, just say it! I'll leave nothing left of you!");

                zsay(pained, "N-no, I'm not in the mood right now.");

                break;
            }
            case 1:
            {
                failedBothFlattery = false;

                show(yuuma, pleased);
                say(yuuma, "That's what I was thinking too. I'd have a headstart on any other beasts, with the eagles already with me. Heheheh.");
                
                zthink(neutral, "I hope that's enough to persuade her.");

                break;
            }
            }
        }
        
        if (flattery >= 2)
        {
            if (failedBothFlattery)
            {
                show(yuuma, annoyed);
                say(yuuma, "...");
                show(yuuma, neutral);
                say(yuuma, "I was hoping you'd be more convincing. But I'm a reasonable person, I see your ability at the votes.");
            }

            zsay(pleasant, "So, I can count on you to start a mess when it matters?");

            show(yuuma, pleased);
            say(yuuma, "Hehehe. I'll do it!");
            say(yuuma, "But you better follow through, or you're next on my dinner plate.");

            zsay(pained, "Yes, yes.");

            hide();

            desc("You've secured Yuuma's backing.");

            zthink(gloat, "Fantastic. This will be key to my victory.");
            io_game.SetVar<Variables::YuumaPromised>(true);

            script.SetNextScene(Scenes::VotingForExecutive);
            end;
        }

        // freeAnimalRealm
    }
    
    say(yuuma, "Look. Whatever you've got going on with the Council, I don't care. I handle my own things, my own way.");

    zsay(unamused_sweat, "Hm. If you insist.");

    hide();

    zthink(neutral, "That was disappointing. Without her backing, I'll need to be on top of my game.");

    script.SetNextScene(Scenes::VotingForExecutive);
    end;
}

SCENE_RUN(VotingForExecutive)
{
    SCENE_SETUP();

    scene(kishin_council);
    wait_for_tasks();

#if !TEST_SKIP_TO_VOTE
    show(zanmu, neutral);
    say(zanmu, "Illustrious Council! Before we attend to any further matters, I have an urgent proposal to make.");
    say(speaker, "Go ahead, Nippaku.");
    say(zanmu, "I have spent the last few days surveying the realms again, as I often do. I've seen decay, paralysis, and overpopulation. This Council has... struggled to respond over the years.");
    say(zanmu, "But it's reaching a breaking point. If this goes on much longer, the Yama will turn elsewhere to send damned souls.");
    say(zanmu, "I see no other choice but this: we must empower one of us as an executive that can make the decisions needed to lead Hell out of its rot.");
    say(zanmu, "They must be untethered by the debates of this Council.");
    say(zanmu, "They must have a singular strong vision, to end this slow death by committee.");

    say(acouncil, "What?");
    say(bcouncil, "Preposterous!");
    say(ccouncil, "She has a point, we barely achieve anything.");
    say(dcouncil, "None of us deserve to rule without restriction.");
    say(bcouncil, "Exactly! This is a power grab!");

    zthink(pleasant, "They're taking it about as well as I expected.");

    say(speaker, "...QUIET!");
    say(speaker, "Nippaku, your proposal is acknowledged. We'll hold a vote now, to put the matter to rest. Hopefully a *productive* meeting can follow.");
#endif

    if (io_game.ReadVar<Variables::KishinAlliance>())
    {
        desc("Your alliance with the senior kishin has paid off again in bonus influence for this vote.");
        add_influence(32);
    }

    desc("The vote is about to start.");
    // This vote is nearly impossible to win, requiring all influence across the game and good mashing
    Game::VoteModeParams const vote = {
        .m_voteName = "Create a singular ruler of Hell",
        .m_votingTime = FIX16(8),
        .m_startingPlayerInfluence = get_influence(),
        .m_attackSize = static_cast<u16>(304 + suika_cost()), // Possible influence until now totals 320
        .m_attackPattern = Game::AttackPattern::Variable,
        .m_playerWantsToLose = false,
        .m_easyMode = script.IsMashlessMode(),
    };
    start_vote(vote);
    Game::VoteResult const result = get_vote_result();

    //set_influence(result.m_remainingInfluence);

    if (result.m_playerWon)
    {
        script.GoToEnding(Scenes::Ending_RulingDecay);
        end;
    }

    show(zanmu, neutral);
    say(zanmu, "I accept the Council's decision, but I see it as a refusal of reality.");
    say(zanmu, "Nevertheless, I do acknowledge the risk of allocating supreme power. I still see another way forward, that avoids this risk.");
    say(zanmu, "I will take leave of the remainder of this meeting, and will raise a fresh idea in tomorrow's meeting.");
    hide();

    scene(zanmu_study);

    zthink(neutral, "That was no real surprise. It would have taken all my influence to win that vote.");
    zthink(neutral, "If I could have ruled this Hell... it would have had a chance.");
    zthink(pleasant, "But maybe it was always going to be easier to take a more drastic option!");
    zthink(neutral, "Tomorrow, I will propose a relocation, to a new Hell.");
    zthink(gloat, "...");
    /*zthink(smirk, "I should spend the rest of the day shoring up my support.");

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
    }*/

#if TEST_SKIP_TO_VOTE
    script.SetNextScene(Scenes::VotingToRelocateHell);
#else
    zthink(neutral, "I think I should take a little break first.");
    script.SetNextScene(Scenes::FinalHisami);
#endif
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

/*SCENE_RUN(FinalYuugi)
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
}*/

SCENE_RUN(VotingToRelocateHell)
{
    SCENE_SETUP();

    scene(kishin_council);
    wait_for_tasks();

#if !TEST_SKIP_TO_VOTE
    show(zanmu, neutral);
    say(zanmu, "Council, I said I had a fresh idea. I'll keep it short. We should move out of this Hell and start a new one.");
    say(zanmu, "Most of the problems we face are from overpopulation, overdevelopment, overspending. This would be a chance to solve all of these in one go.");
    zthink(gloat, "It'd also solve this good for nothing Council.");
    say(zanmu, "The vast nothingness of the underworld is ripe for use. The logistics make sense.");

    say(acouncil, "Well... compared to yesterday's absurdity, this isn't such a bad idea.");
    say(bcouncil, "I'd need time to think about it.");
    say(ccouncil, "Why do you need time? It's clearly nonsense. Again.");
    say(dcouncil, "Chair, can't we decide quickly with a vote again?");
    say(bcouncil, "I wish us oni would just fight to decide things rather than all this talking.");
    say(ccouncil, "Are we really willing to leave everything behind, just like that?");

    // TODO does yuugi speak up?

    say(speaker, "Our rule is to debate and vote on all proposals. Let this one be healthy.");
#else
    io_game.SetVar<Variables::YuumaPromised>(true);
#endif

    desc("The vote is about to start.");
    {
        Game::VoteModeParams const vote = {
            .m_voteName = "Relocate Hell",
            .m_votingTime = FIX16(7),
            .m_startingPlayerInfluence = get_influence(),
            .m_attackSize = static_cast<u16>(244 + suika_cost()), // Not impossible, but it can be fine if they lose this
            .m_attackPattern = Game::AttackPattern::Variable,
            .m_playerWantsToLose = false,
            .m_easyMode = script.IsMashlessMode(),
        };
        start_vote(vote);
        Game::VoteResult const result = get_vote_result();

        if (result.m_playerWon)
        {
            script.GoToEnding(Scenes::Ending_NewHell);
            end;
        }
    }

    if (io_game.ReadVar<Variables::YuumaPromised>())
    {
        zthink(gloat, "I still have cards to play. I'm not giving up now!");

        say(speaker, "Nippaku-");

        show(zanmu, neutral);
        say(zanmu, "Council! I demand we revote!");

        say(speaker, "No, Nippaku, that's enough-");

        say(zanmu, "I don't think so. A taotie in the animal realm is about to launch a rebellion.");

        say(ccouncil, "A *rebellion*? With what army?");
        say(bcouncil, "And what's the relevance?");

        zthink(gloat, "No point hiding anything now.");

        say(zanmu, "The eagle spirits, they've secretly banded together, and they follow the taotie.");
        say(zanmu, "I've organised with them that we'll move to a new Hell by force if necessary!");
        say(zanmu, "Utter chaos will be unleashed, and anyone who does not come willingly will face a grim fate in the new Hell!");
        say(zanmu, "So, let's hold a revote. Make this happen the orderly way.");

        show(yuugi, angry);
        say(yuugi, "Nippaku! I can't stand for this, no matter what threats you have!");

        if (io_game.ReadVar<Variables::SuikaDissuaded>())
        {
            show(suika, pout);
            say(suika, "I can't say it's a bad idea, all things told...");

            zsay(smirk, "Exactly, I believe most of you already understand me.");
            zsay(gloat, "Act like the oni you are! Have the strength to make bold change!");
        }
        else
        {
            show(suika, bigjoy);
            say(suika, "You're willing to go this far, Zanmu? I'll oppose you with all I have!");
        }
        hide();

        say(speaker, "... We'll have to hold the vote again, if those are the stakes.");

        desc("This is the final vote. Make it count!");

        Game::VoteModeParams const vote2 = {
            .m_voteName = "Relocate Hell",
            .m_votingTime = FIX16(5),
            .m_startingPlayerInfluence = get_influence(),
            .m_attackSize = static_cast<u16>(108 + suika_cost()),
            .m_attackPattern = Game::AttackPattern::Variable,
            .m_playerWantsToLose = false,
            .m_easyMode = script.IsMashlessMode(),
        };
        start_vote(vote2);
        Game::VoteResult const result2 = get_vote_result();

        //set_influence(result2.m_remainingInfluence);

        if (result2.m_playerWon)
        {
            script.GoToEnding(Scenes::Ending_NewHell);
            end;
        }
    }

    script.GoToEnding(Scenes::Ending_AdministratingCollapse);
    end;
}


SCENE_RUN(Ending_RulingDecay)
{
    SCENE_SETUP();

    desc("Zanmu Nippaku has managed to swindle her way to the top of this crumbling Hell.");
    desc("Through a massive and crushing force of oni might, she somehow brings Hell back to some semblance of stability.");
    desc("But, she can't help but be bothered by all the cracks that form, needing constant repair.");
    desc("Perhaps, getting everything she thought she wanted wasn't really the best outcome.");
    desc("Maybe there was another way?");

    desc("Ending 1: King of Decay\n\nThanks for playing! Try again for other endings!");

    io_game.RequestNextWorld(std::make_unique<Game::IntroWorld>());
    end;
}

SCENE_RUN(Ending_NewHell)
{
    SCENE_SETUP();

    desc("Zanmu Nippaku successfully negotiates with the oni and the Yama to move Hell.");

    desc("Yuugi and Suika flee to Gensokyo, although Yuugi soon returns to the Former Hell. It's hard to forget where home is.");

    desc("The logistics of relocation aren't easy, but the results speak for themselves.");

    io_vn.ClearMode(io_game);

    scene(ending);
    wait_for_tasks();
    io_vn.WaitUntilInput(); wait;

    desc("The New Hell is a thriving expanse, with realms of great wealth and great poverty alike.");
    desc("The likes of Yuuma Toutetsu and other gang leaders rule vast and growing metropolises as the oni power dwindles.");

    desc("What a wondrous world!");

    desc("Ending 2: Shin Jigoku\n\nThanks for playing! Try again for other endings!");

    io_vn.ClearMode(io_game);

    io_vn.WaitUntilInput(); wait;

    io_game.RequestNextWorld(std::make_unique<Game::IntroWorld>());
    end;
}

SCENE_RUN(Ending_AdministratingCollapse)
{
    SCENE_SETUP();

    desc("Despite her frantic efforts, Zanmu is unable to change anything major about Hell.");
    desc("It slips into further and further decay, and then total collapse...");
    desc("It's only a matter of time before the Yama do something drastic, with an ill fate awaiting all residents of Hell.");

    desc("Ending 3: Infernal Collapse\n\nThanks for playing! Try again for other endings!");

    io_game.RequestNextWorld(std::make_unique<Game::IntroWorld>());
    end;
}

SCENE_RUN(CharacterViewer)
{
    SCENE_SETUP();

    scene(zanmu_study);
    show(hisami, neutral);
    wait_for_tasks();
    io_vn.WaitUntilInput(); wait;

    scene(animal_realm);
    show(yuuma, annoyed);
    wait_for_tasks();
    io_vn.WaitUntilInput(); wait;
}

}