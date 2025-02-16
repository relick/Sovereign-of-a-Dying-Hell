#include "Game.hpp"
#include "IntroWorld.hpp"
#include "Version.hpp"

int main( bool hardReset )
{
	System::StaticInit_Version();

	Game::Game game;
	game.RequestNextWorld(std::make_unique<Game::IntroWorld>());
	game.Run();

	return 0;
}
