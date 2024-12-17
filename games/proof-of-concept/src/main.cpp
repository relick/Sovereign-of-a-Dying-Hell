#include <genesis.h>
#include <memory>

class World
{
public:
	virtual ~World() = default;

	virtual void Init() = 0;
	virtual void Shutdown() = 0;
	virtual void Run() = 0;
};

class IntroWorld
	: public World
{
	int y = 0;

	void Init() override {}
	void Shutdown() override {}
	void Run() override
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
	void Init() override {}
	void Shutdown() override {}
	void Run() override
	{
	}
};

class GameWorld
	: public World
{
	void Init() override {}
	void Shutdown() override {}
	void Run() override
	{
	}
};

class Game
{
	std::unique_ptr<IntroWorld> m_intro;
	std::unique_ptr<TitleWorld> m_title;
	std::unique_ptr<GameWorld> m_game;

	World* m_curWorld{nullptr};

public:
	Game()
	{
		m_intro = std::make_unique<IntroWorld>();
		m_title = std::make_unique<TitleWorld>();
		m_game = std::make_unique<GameWorld>();

		m_curWorld = m_intro.get();

		m_curWorld->Init();
	}

	int Run()
	{
		while (true)
		{
			m_curWorld->Run();
			SYS_doVBlankProcess();
		}

		return 0;
	}
};

int main( bool hardReset )
{
	Game game;
	return game.Run();
}
