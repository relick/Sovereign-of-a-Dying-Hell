#pragma once

#include "Declare.hpp"
#include "DialoguePrinter.hpp"

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
	s16 y = 0;

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
	DialoguePrinter m_printer{1, 25, 38, 2};

	void Init(Game &io_game) override;
	void Shutdown(Game &io_game) override;
	void Run(Game &io_game) override;
};

}