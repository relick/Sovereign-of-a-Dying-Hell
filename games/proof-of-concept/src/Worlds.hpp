#pragma once

#include "Declare.hpp"
#include "DialoguePrinter.hpp"
#include "Constants.hpp"

namespace Game
{

class World
{
public:
	virtual ~World() = default;

	virtual void Init(Game &io_game) = 0;
	virtual void Shutdown(Game &io_game) = 0;
	virtual void Run(Game &io_game) = 0;
};

class IntroWorld
	: public World
{
	f16 m_timer = 0;

	void Init(Game &io_game) override;
	void Shutdown(Game &io_game) override;
	void Run(Game &io_game) override;
};

class TitleWorld
	: public World
{
	void Init(Game &io_game) override {}
	void Shutdown(Game &io_game) override {}
	void Run(Game &io_game) override {}
};

class GameWorld
	: public World
{
	DialoguePrinter m_printer{2, c_textFramePos + 1, 36, 3};

	void Init(Game &io_game) override;
	void Shutdown(Game &io_game) override;
	void Run(Game &io_game) override;
};

}