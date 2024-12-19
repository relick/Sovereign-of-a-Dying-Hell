#include <genesis.h>
#include <memory>

class Game;

class World
{
public:
	virtual ~World() = default;

	virtual void Init(Game& io_game) = 0;
	virtual void Shutdown(Game& io_game) = 0;
	virtual void Run(Game& io_game) = 0;
};

class IntroWorld
	: public World
{
	int y = 0;

	void Init(Game& io_game) override {}
	void Shutdown(Game& io_game) override {}
	void Run(Game& io_game) override
	{
		VDP_clearText(12, y, 11);
		y++;
		y%=28;
		VDP_drawText("Hello world", 12, y);
	}
};

class TitleWorld
	: public World
{
	void Init(Game& io_game) override {}
	void Shutdown(Game& io_game) override {}
	void Run(Game& io_game) override
	{
	}
};

class GameWorld
	: public World
{
	void Init(Game& io_game) override {}
	void Shutdown(Game& io_game) override {}
	void Run(Game& io_game) override
	{
	}
};

class Game
{
	std::unique_ptr<World> m_curWorld;
	std::unique_ptr<World> m_nextWorld;

public:
	void RequestNextWorld(std::unique_ptr<World>&& i_nextWorld)
	{
		m_nextWorld = std::move(i_nextWorld);
	}

	void Run()
	{
		while (true)
		{
			if (m_nextWorld)
			{
				if (m_curWorld)
				{
					m_curWorld->Shutdown(*this);
				}
				m_nextWorld->Init(*this);
				std::swap(m_curWorld, m_nextWorld);
				m_nextWorld = nullptr;
			}
			m_curWorld->Run(*this);
			SYS_doVBlankProcess();
		}
	}
};

int main( bool hardReset )
{
	Game game;
	game.RequestNextWorld(std::make_unique<IntroWorld>());
	game.Run();

	return 0;
}
