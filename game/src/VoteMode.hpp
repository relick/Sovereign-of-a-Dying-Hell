#pragma once

#include "Declare.hpp"
#include "GameRoutines.hpp"
#include "TileData.hpp"
#include "Worlds.hpp"

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
	char const* m_voteName{ nullptr };
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
	VNWorld* m_vnWorld{};

	// Initial data
	VoteModeParams m_params;

	// Running data
	bool m_votingStarted{ false };
	bool m_votingComplete{ false };
	s16 m_votePosition{ 0 };
	
	u16 m_framesLeft{};
	u16 m_remainingInfluence{};
	bool m_voteWon{ false };

	std::vector<AttackEvent> m_attackEvents;
	bool m_ABCpressed{ false };

	// Graphic data
	bool m_graphicsReady{ false };
	bool m_graphicsDone{ false }; // set to true when post-vote animation finishes

	std::array<SpriteID, 2> m_num{};
	SpriteID m_midline{};
	SpriteID m_cursor{};

	std::array<u16, 11> m_num_tileIndex{};
	u16 m_barCorners_tileIndex{};
	u16 m_barMids_tileIndex{};
	u16 m_barMidLine_tileIndex{};
	u16 m_cursor_tileIndex{};
	u16 m_passed_vote_tileIndex{};
	u16 m_passed_passed_tileIndex{};
	u16 m_failed_vote_tileIndex{};
	u16 m_failed_failed_tileIndex{};
	u16 m_influenceBar_tileIndex{};
	u16 m_influenceBarText_tileIndex{};

	// 2 rows of 32 tiles
	std::array<u16, 64> m_barTileMap{};
	Task m_updateBarTileMap;

	// 1 row of 40 tiles, mirrored from the middle
	std::array<u16, 40> m_influenceBarTileMap{};
	Task m_updateInfluenceBarMap;

	// Vote text, uses as many tiles/sprites as needed
	std::vector<Tiles::Tile> m_voteNameTextTiles;
	std::vector<SpriteID> m_voteNameSprites;

	u16 m_endTimer{};

public:
	VoteMode(Game& io_game, FontData const& i_fontData, VNWorld& io_vnWorld)
		: m_game{ &io_game }, m_fontData{ &i_fontData }, m_vnWorld{ &io_vnWorld }
	{}
	~VoteMode();

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

	bool VotingDone() const { return m_votingComplete && m_graphicsDone; }
	VoteResult VotingResult() const { return { m_voteWon == !m_params.m_playerWantsToLose, m_remainingInfluence, }; }

private:
	void GenerateAttackEvents();
	std::pair<u16, u16> FindNumTileIndicesForFrameTimer() const;
	void SetupGraphics();
	Task RenderText(u16 i_tileIndex);
	void UpdateGraphics();
	Task UpdateBarTileMap();
	Task UpdateInfluenceBarTileMap();
	u16 GetBarCornerAttr(bool i_hflip) const;
	u16 GetBarMidAttr(u16 i_tileI) const;
	void SetupEndGraphics();
	Task UpdateEndGraphics();
};

}