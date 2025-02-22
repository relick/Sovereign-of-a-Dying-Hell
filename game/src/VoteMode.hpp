#pragma once

#include "Declare.hpp"

#include <vector>

namespace Game
{

enum class AttackPattern
{
	SlowChunky, // Attacks hit irregularly, but have big impacts
	FastBitty, // Attacks hit regularly, with small impacts
	Variable, // Attacks hit with a mix of big and small, with varying delays
};

struct VoteModeParams
{
	f16 m_votingTime{};
	u16 m_startingPlayerInfluence{};
	u16 m_attackSize{};
	AttackPattern m_attackPattern{};
	bool m_playerWantsToLose{};
};

struct AttackEvent
{
	u16 m_frame{};
	u16 m_size{};
};

struct VoteResult
{
	bool m_playerWon{ false };
	u16 m_remainingInfluence{ 0 };
};

class VoteMode
{
	Game* m_game{};
	FontData const* m_fontData{};

	// Initial data
	VoteModeParams m_params;

	// Running data
	bool m_votingStarted{ false };
	bool m_votingComplete{ false };
	s16 m_votePosition{ 0 };
	
	u16 m_framesLeft{};
	u16 m_remainingInfluence{};
	bool m_playerWon{ false };

	std::vector<AttackEvent> m_attackEvents;
	bool m_ABCpressed{ false };

	// Graphic data
	

public:
	VoteMode(Game& io_game, FontData const& i_fontData)
		: m_game{ &io_game }, m_fontData{ &i_fontData }
	{}

	// The idea will be, for the duration of i_votingTime,
	// the player can mash buttons, which spends their influence, to move the bar to the right.
	// Over time in (to the player) random intervals, the bar will drain to the left in steps.
	// The total it will move to the left by the end of voting time is i_attackSize
	// i_attackPattern determines how the attacks are delivered through the time.
	// i_playerWantsToLose flips left and right graphically but otherwise is treated the same
	void Start(VoteModeParams i_params);

	// The voting mode takes over from VNWorld entirely (besides using the background)
	// Check VotingDone() to know when everything's done and results can be extracted.
	void Update();

	bool VotingDone() const { return m_votingComplete; }
	VoteResult VotingResult() const { return { m_playerWon, m_remainingInfluence, }; }

private:
	void GenerateAttackEvents();
};

}