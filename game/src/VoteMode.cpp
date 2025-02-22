#include "VoteMode.hpp"

#include "Version.hpp"

#include <cstdlib>

namespace Game
{

inline constexpr u16 c_influencePerMash = 2;
inline constexpr s16 c_maxVote = 128;
inline constexpr s16 c_minVote = -128;

//------------------------------------------------------------------------------
void VoteMode::Start
(
	VoteModeParams i_params
)
{
	// Cache params
	m_params = std::move(i_params);

	// Set up data
	m_framesLeft = fix16Mul(std::max<f16>(m_params.m_votingTime, FIX16(5)), FramesPerSecond()); // Min 5-seconds
	m_remainingInfluence = m_params.m_startingPlayerInfluence;
	m_playerWon = false;

	GenerateAttackEvents();

	m_votingStarted = true;
}

//------------------------------------------------------------------------------
void VoteMode::Update
(
)
{
	if (!m_votingStarted || m_votingComplete)
	{
		return;
	}

	u16 const buttons = JOY_readJoypad(JOY_1);
	bool const ABCpressedThisFrame = [this, buttons] {
		if ((buttons & (BUTTON_A | BUTTON_B | BUTTON_C)) != 0)
		{
			if (!m_ABCpressed)
			{
				m_ABCpressed = true;
				return true;
			}
		}
		else
		{
			m_ABCpressed = false;
		}
		return false;
	}();

	if (ABCpressedThisFrame && m_remainingInfluence >= c_influencePerMash)
	{
		m_votePosition += c_influencePerMash;
		m_remainingInfluence -= c_influencePerMash;
	}

	if (m_votePosition >= c_maxVote)
	{
		m_votingComplete = true;
		m_playerWon = true;
		return;
	}
	else if (m_votePosition <= c_minVote)
	{
		m_votingComplete = true;
		m_playerWon = false;
		return;
	}

	if (!m_attackEvents.empty())
	{
		if (m_attackEvents.back().m_frame >= m_framesLeft)
		{
			m_votePosition -= static_cast<s16>(m_attackEvents.back().m_size);
			m_attackEvents.pop_back();
		}
	}
	
	if (m_framesLeft > 0)
	{
		--m_framesLeft;
	}
	else
	{
		m_votingComplete = true;
		m_playerWon = m_votePosition >= 0;
		return;
	}
}

//------------------------------------------------------------------------------
void VoteMode::GenerateAttackEvents
(
)
{
	auto fnGenerateAttacksWithParams = [this](u16 i_attackSize, f16 i_attackSizeVar, f16 i_attackFramesVar)
	{
		f16 const attackSizeVarHalf = i_attackSizeVar / 2;
		f16 const attackFramesVarHalf = i_attackFramesVar / 2;
		u16 remainingAttack = m_params.m_attackSize;

		u16 const possibleAttacks = m_params.m_attackSize / i_attackSize;

		m_attackEvents.reserve(possibleAttacks);

		for (u16 i = 0; i < possibleAttacks; ++i)
		{
			f16 const r = rand() & FIX16_FRAC_MASK;
			f16 const variation = fix16Mul(r, i_attackSizeVar) - attackSizeVarHalf;
			u16 const attackSize = fix16ToInt(variation) + i_attackSize;

			if (remainingAttack < attackSize)
			{
				m_attackEvents.push_back(AttackEvent{ 0, remainingAttack });
				break;
			}
			remainingAttack -= attackSize;

			if (remainingAttack < (i_attackSize - attackSizeVarHalf))
			{
				m_attackEvents.push_back(AttackEvent{ 0, static_cast<u16>(attackSize + remainingAttack) });
				break;
			}
			else
			{
				m_attackEvents.push_back(AttackEvent{ 0, attackSize });
			}
		}

		u16 const averageFramesPerAttack = m_framesLeft / (m_attackEvents.size() + 1); // Add 1 to avoid attacking immediately at the end
		u16 const earliestAllowedAttack = m_framesLeft - fix16ToInt(attackFramesVarHalf);

		u16 regularFrames = averageFramesPerAttack;
		for (u16 i = 0; i < m_attackEvents.size(); ++i)
		{
			f16 const r = rand() & FIX16_FRAC_MASK;
			f16 const variation = fix16Mul(r, i_attackFramesVar) - attackFramesVarHalf;
			u16 const attackTime = std::min<u16>(fix16ToInt(variation < 0 ? attackFramesVarHalf : variation) + regularFrames, earliestAllowedAttack);

			m_attackEvents[i].m_frame = attackTime;

			regularFrames += averageFramesPerAttack;
		}

	};

	switch (m_params.m_attackPattern)
	{
	case AttackPattern::SlowChunky:
	{
		constexpr u16 c_attackSize = 20;
		constexpr f16 c_attackSizeVariation = FIX16(5);
		constexpr f16 c_attackFramesVariation = FIX16(40);
		fnGenerateAttacksWithParams(c_attackSize, c_attackSizeVariation, c_attackFramesVariation);
		break;
	}
	case AttackPattern::FastBitty:
	{
		constexpr u16 c_attackSize = 5;
		constexpr f16 c_attackSizeVariation = FIX16(2);
		constexpr f16 c_attackFramesVariation = FIX16(20);
		fnGenerateAttacksWithParams(c_attackSize, c_attackSizeVariation, c_attackFramesVariation);
		break;
	}
	case AttackPattern::Variable:
	{
		constexpr u16 c_attackSize = 20;
		constexpr f16 c_attackSizeVariation = FIX16(20);
		constexpr f16 c_attackFramesVariation = FIX16(60);
		fnGenerateAttacksWithParams(c_attackSize, c_attackSizeVariation, c_attackFramesVariation);
		break;
	}
	}
}

}