#pragma once

#include "Declare.hpp"
#include "SFXManager.hpp"
#include "SpriteManager.hpp"
#include "GameRoutines.hpp"

#include <algorithm>
#include <bit>
#include <functional>
#include <memory>
#include <vector>

namespace Game
{

#define PROFILER (0)

template<typename T>
concept NotTaskCoroutine = !std::convertible_to<T, Task>;

struct Erasure
{
	virtual ~Erasure() = default;
};

template<NotTaskCoroutine T_Lambda>
struct LambdaHolder
	: Erasure
{
	LambdaHolder(T_Lambda&& i_lambda) : m_lambda(std::move(i_lambda)) {}
	T_Lambda m_lambda;
};

class Game
{
	std::unique_ptr<World> m_curWorld;
	std::unique_ptr<World> m_nextWorld;
	WorldRoutine m_currentWorldRoutine;

	SFXManager m_sfx;
	SpriteManager m_sprites;

	struct TaskData
	{
		Task m_routine;
		std::unique_ptr<Erasure> m_lambda;
		TaskPriority m_priority{};
		u16 m_monotonic{};

		bool operator<(TaskData const& i_o) const
		{
			// monotonic number is > as later tasks should be 'less' than earlier tasks
			return m_priority < i_o.m_priority || (m_priority == i_o.m_priority && m_monotonic > i_o.m_monotonic);
		}
	};
	std::vector<TaskData> m_tasks;
	static inline u16 s_taskMonotonic = 0;

	static inline VBlankCallbackID s_callbackID = 0;
	static inline std::vector<std::pair<VBlankCallbackID, std::function<void()>>> s_vBlankCallbacks;

	// TODO: improve beyond just being a set of numbers.
	bool m_loadedData{ false };
	std::vector<u16> m_gameVariables;

public:
	Game();

	void RequestNextWorld(std::unique_ptr<World> &&i_nextWorld);

	void Run();

#if PROFILER
	static u32 GetVCount();
#endif

	VBlankCallbackID AddVBlankCallback(std::function<void()>&& i_callback);
	void RemoveVBlankCallback(VBlankCallbackID i_callbackID);

	SFXManager& SFX() { return m_sfx; }
	SpriteManager& Sprites() { return m_sprites; }

	void QueueFunctionTask(Task&& i_task);
	void QueueFunctionTask(Task&& i_task, TaskPriority i_priority);
	template<NotTaskCoroutine T_Lambda, typename... T_Args>
	void QueueLambdaTask(T_Lambda&& i_lambda, T_Args&&... i_args);
	template<NotTaskCoroutine T_Lambda, typename... T_Args>
	void QueueLambdaTask(T_Lambda&& i_lambda, TaskPriority i_priority, T_Args&&... i_args);
	bool TasksInProgress() const;

	void SetVariableCount(u16 i_count) { m_gameVariables.resize(i_count); }
	template<typename T, typename T_Index = u16>
	void SetVar(T_Index i_varIndex, T i_value) requires(sizeof(T) == sizeof(u16)) { m_gameVariables[static_cast<u16>(i_varIndex)] = std::bit_cast<u16>(i_value); }
	template<typename T, typename T_Index = u8>
	void SetVar(T_Index i_varIndex, T i_value) requires(sizeof(T) == sizeof(u8)) { m_gameVariables[static_cast<u16>(i_varIndex)] = std::bit_cast<u8>(i_value); }
	template<typename T_Index = u16>
	void SetVar(T_Index i_varIndex, bool i_value) { m_gameVariables[static_cast<u16>(i_varIndex)] = i_value ? 1 : 0; }
	template<typename T, typename T_Index = u16>
	T ReadVar(T_Index i_varIndex) const requires(sizeof(T) == sizeof(u16)) { return std::bit_cast<T>(m_gameVariables[static_cast<u16>(i_varIndex)]); }
	template<typename T, typename T_Index = u8>
	T ReadVar(T_Index i_varIndex) const requires(sizeof(T) == sizeof(u8)) { return std::bit_cast<T>(static_cast<u8>(m_gameVariables[static_cast<u16>(i_varIndex)])); }
	template<typename T_Index = u16>
	bool ReadVar(T_Index i_varIndex) const { return m_gameVariables[static_cast<u16>(i_varIndex)] == 1; }

	void SaveVariables();
	bool LoadVariables();
	bool HasLoadedData() const { return m_loadedData; }

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

template<NotTaskCoroutine T_Lambda, typename... T_Args>
void Game::QueueLambdaTask
(
	T_Lambda&& i_lambda,
	T_Args&&... i_args
)
{
	QueueLambdaTask(std::forward<T_Lambda>(i_lambda), TaskPriority::Normal, std::forward<T_Args>(i_args)...);
}

template<NotTaskCoroutine T_Lambda, typename... T_Args>
void Game::QueueLambdaTask
(
	T_Lambda&& i_lambda,
	TaskPriority i_priority,
	T_Args&&... i_args
)
{
	LambdaHolder<T_Lambda>* holder = new LambdaHolder<T_Lambda>{ std::move(i_lambda) };
	m_tasks.push_back({
		holder->m_lambda(std::forward<T_Args>(i_args)...),
		std::unique_ptr<Erasure>(holder),
		i_priority,
		s_taskMonotonic++,
		});
	std::push_heap(m_tasks.begin(), m_tasks.end());
}

}