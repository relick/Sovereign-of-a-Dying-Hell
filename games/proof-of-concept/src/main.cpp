#include "Game.hpp"
#include "Worlds.hpp"
#include "Version.hpp"

int main( bool hardReset )
{
	System::StaticInit_Version();

	Game::Game game;
	game.RequestNextWorld(std::make_unique<Game::VNWorld>());
	game.Run();

	return 0;
}
