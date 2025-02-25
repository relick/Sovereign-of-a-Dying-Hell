#pragma once

#include "Scene.hpp"

namespace Jam15
{

enum class Scenes : u8
{
	FirstVoteProposal,
	HatchingThePlan,
	LobbyingYuugi,
	VotingForAnimalRights,
	SuikaApproaches,
	DelegatingToHisami,
	VotingForPriceIncreases,
	//MeetingTheShadowyKishin,
	EngagingYuuma,
	VotingForExecutive,
	FinalHisami,
	//FinalYuugi,
	//FinalSuika,
	//FinalYuuma,
	VotingToRelocateHell,

	Ending_RulingDecay,
	Ending_NewHell,
	Ending_AdministratingCollapse,

	CharacterViewer,

	InitialScene = FirstVoteProposal,
};


#define SCENE(scene_name) struct scene_name : public Game::Scene { Game::SceneRoutine Run(Game::Game& io_game, Game::VNWorld& io_vn, Game::Script& io_script) override; }

SCENE(FirstVoteProposal);
SCENE(HatchingThePlan);
SCENE(LobbyingYuugi);
SCENE(VotingForAnimalRights);
SCENE(SuikaApproaches);
SCENE(DelegatingToHisami);
SCENE(VotingForPriceIncreases);
//SCENE(MeetingTheShadowyKishin);
SCENE(EngagingYuuma);
SCENE(VotingForExecutive);
SCENE(FinalHisami);
//SCENE(FinalYuugi);
//SCENE(FinalSuika);
//SCENE(FinalYuuma);
SCENE(VotingToRelocateHell);

SCENE(Ending_RulingDecay);
SCENE(Ending_NewHell);
SCENE(Ending_AdministratingCollapse);

SCENE(CharacterViewer);

}