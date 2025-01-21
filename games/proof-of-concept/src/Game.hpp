#pragma once

#include "Declare.hpp"
#include "SpriteManager.hpp"
#include "GameRoutines.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace Game
{

class Game
{
	std::unique_ptr<World> m_curWorld;
	std::unique_ptr<World> m_nextWorld;
	WorldRoutine m_currentWorldRoutine;

	SpriteManager m_sprites;

	static inline VBlankCallbackID s_callbackID = 0;
	static inline std::vector<std::pair<VBlankCallbackID, std::function<void()>>> s_vBlankCallbacks;

public:
	Game();

	void RequestNextWorld(std::unique_ptr<World> &&i_nextWorld);

	void Run();

	VBlankCallbackID AddVBlankCallback(std::function<void()>&& i_callback);
	void RemoveVBlankCallback(VBlankCallbackID i_callbackID);

	SpriteManager& Sprites() { return m_sprites; }

private:
	static void VBlankCallback();

	void PostWorldFrame();
	void PreWorldInit();
};

}