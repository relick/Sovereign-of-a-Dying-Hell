#include "VoteMode.hpp"

namespace Game
{

//------------------------------------------------------------------------------
void VoteMode::Start
(
	f16 i_votingTime,
	u16 i_startingPlayerInfluence,
	u16 i_attackSize,
	AttackPattern i_attackPattern,
	bool i_playerWantsToLose
)
{
	m_votingTime = i_votingTime;
	m_startingPlayerInfluence = i_startingPlayerInfluence;
	m_attackSize = i_attackSize;
	m_attackPattern = i_attackPattern;
	m_playerWantsToLose = i_playerWantsToLose;


}

//------------------------------------------------------------------------------
void VoteMode::Update
(
)
{

}

}