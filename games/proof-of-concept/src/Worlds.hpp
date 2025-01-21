#pragma once

#include "Declare.hpp"
#include "DialoguePrinter2.hpp"
#include "Constants.hpp"
#include "Scene.hpp"
#include "GameRoutines.hpp"

namespace Game
{

class World
{
public:
	virtual ~World() = default;

	virtual WorldRoutine Init(Game &io_game) = 0;
	virtual WorldRoutine Shutdown(Game &io_game) = 0;
	virtual void Run(Game &io_game) = 0;
};

class IntroWorld
	: public World
{
	std::array<s16, c_screenHeightPx> m_lineTable{};
	u16 m_sineScroll{0};
	VBlankCallbackID m_fxScrollID{};

	bool m_joyUnpressed = false;

	f16 m_timer = 0;
	bool m_fadeInStarted = false;
	bool m_fadeOutStarted = false;

	WorldRoutine Init(Game &io_game) override;
	WorldRoutine Shutdown(Game &io_game) override;
	void Run(Game &io_game) override;

private:
	void DMAScrollData();
};

class TitleWorld
	: public World
{
	WorldRoutine Init(Game &io_game) override { co_return; }
	WorldRoutine Shutdown(Game &io_game) override { co_return; }
	void Run(Game &io_game) override {}
};

class VNWorld
	: public World
{
	DialoguePrinter2 m_printer;
	Scene m_scene;
	SceneRoutine m_sceneRun;
	bool m_readyForNext{false};

	WorldRoutine Init(Game &io_game) override;
	WorldRoutine Shutdown(Game &io_game) override;
	void Run(Game &io_game) override;
};

}