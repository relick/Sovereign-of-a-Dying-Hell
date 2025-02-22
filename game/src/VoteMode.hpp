#pragma once

#include "Declare.hpp"

namespace Game
{

enum class AttackPattern
{
	SlowChunky, // Attacks hit irregularly, but have big impacts
	FastBitty, // Attacks hit regularly, with small impacts
	Variable, // Attacks hit with a mix of big and small, with varying delays
};

class VoteMode
{
	Game* m_game{};
	FontData const* m_fontData{};

	// Initial data
	f16 m_votingTime{};
	u16 m_startingPlayerInfluence{};
	u16 m_attackSize{};
	AttackPattern m_attackPattern{};
	bool m_playerWantsToLose{};

	// Running data
	bool m_votingComplete{ false };
	
	f16 m_timeLeft{};
	u16 m_remainingInfluence{};
	u16 m_remainingAttack{};
	bool m_playerWon{};

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
	void Start(
		f16 i_votingTime,
		u16 i_startingPlayerInfluence,
		u16 i_attackSize,
		AttackPattern i_attackPattern,
		bool i_playerWantsToLose
	);

	// The voting mode takes over from VNWorld entirely (besides using the background)
	// Check VotingDone() to know when everything's done and results can be extracted.
	void Update();

	bool VotingDone() const { return m_votingComplete; }
	bool GetVoteWonByPlayer() const { return m_playerWon; }
	u16 GetRemainingInfluence() const { return m_remainingInfluence; }
};

}