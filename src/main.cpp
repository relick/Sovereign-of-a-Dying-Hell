#include "adventure-md/Game.hpp"
#include "adventure-md/IntroWorld.hpp"
#include "adventure-md/Version.hpp"

#include "Jam15_Script.hpp"

main( bool hardReset )
{
	System::StaticInit_Version();

	Game::Game game;
	game.RequestNextWorld(std::make_unique<Game::IntroWorld>(std::make_unique<Jam15::Script>()));
	game.Run();
}
