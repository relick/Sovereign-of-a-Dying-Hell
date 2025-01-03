#include "Game.hpp"
#include "Worlds.hpp"

int main( bool hardReset )
{
	Game::Game game;
	game.RequestNextWorld(std::make_unique<Game::IntroWorld>());
	game.Run();

	return 0;
}
