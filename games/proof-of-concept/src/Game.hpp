#pragma once

#include "Declare.hpp"
#include "SpriteManager.hpp"
#include "GameRoutines.hpp"

#include <deque>
#include <functional>
#include <memory>
#include <vector>

namespace Game
{

#define PROFILER (0)

class Game
{
	std::unique_ptr<World> m_curWorld;
	std::unique_ptr<World> m_nextWorld;
	WorldRoutine m_currentWorldRoutine;

	SpriteManager m_sprites;

	std::deque<DMARoutine> m_dmaRoutines;

	static inline VBlankCallbackID s_callbackID = 0;
	static inline std::vector<std::pair<VBlankCallbackID, std::function<void()>>> s_vBlankCallbacks;

public:
	Game();

	void RequestNextWorld(std::unique_ptr<World> &&i_nextWorld);

	void Run();

#if PROFILER
	static u32 GetVCount();
#endif

	VBlankCallbackID AddVBlankCallback(std::function<void()>&& i_callback);
	void RemoveVBlankCallback(VBlankCallbackID i_callbackID);

	SpriteManager& Sprites() { return m_sprites; }

	void AddDMARoutine(DMARoutine&& i_routine);
	bool DMAsInProgress() const;

private:
	static void VIntCallback();
	static void VBlankCallback();

	void PostWorldFrame();
	void PreWorldInit();
};

#if PROFILER

struct AutoProfileScope
{
	char const* m_fmt;
	u32 m_before;
	AutoProfileScope(char const* i_fmt) : m_fmt(i_fmt), m_before(Game::GetVCount()) {}
	~AutoProfileScope() {
		u32 after = Game::GetVCount();
		kprintf(m_fmt, after - m_before);
	}
};

#else

struct AutoProfileScope
{
	AutoProfileScope(char const* i_fmt) {}
};

#endif

}