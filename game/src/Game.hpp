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

template<typename T>
concept IsEnum = std::is_enum_v<T>;

// Specialise with using Tuple = std::tuple<types, for, variables>;
template<IsEnum T>
struct VariablesTypeTuple {};

template<typename T>
concept VariablesEnum = IsEnum<T> && requires
{
	typename VariablesTypeTuple<T>::Tuple;
	{ T::Count } -> std::same_as<T>;
	// Implicitly requires std::tuple
	requires std::tuple_size_v<typename VariablesTypeTuple<T>::Tuple> == static_cast<size_t>(T::Count);
};

template<typename T, T t_Index>
concept ValidVarIndex = VariablesEnum<T> && (t_Index >= static_cast<T>(0)) && (t_Index < T::Count);

template<auto t_Index> requires ValidVarIndex<decltype(t_Index), t_Index>
using TypeOfVar = std::tuple_element_t<static_cast<size_t>(t_Index), typename VariablesTypeTuple<decltype(t_Index)>::Tuple>;

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

	static inline VBlankCallbackID::Core s_callbackID = 0;
	static inline std::vector<std::pair<VBlankCallbackID, std::function<void()>>> s_vBlankCallbacks;

	// TODO: improve beyond just being a set of numbers.
	bool m_loadedData{ false };
	std::vector<u16> m_gameVariables;

public:
	Game();

	void RequestNextWorld(std::unique_ptr<World> i_nextWorld);

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

	template<VariablesEnum T>
	void ResetVariables() { m_loadedData = false; m_gameVariables.clear(); m_gameVariables.resize(static_cast<u16>(T::Count)); }
	template<auto t_Index>
	void SetVar(TypeOfVar<t_Index> i_value)
		requires(sizeof(TypeOfVar<t_Index>) == sizeof(u16))
	{ m_gameVariables[static_cast<u16>(t_Index)] = std::bit_cast<u16>(i_value); }
	template<auto t_Index>
	void SetVar(TypeOfVar<t_Index> i_value)
		requires(sizeof(TypeOfVar<t_Index>) == sizeof(u8)) && (!std::same_as<TypeOfVar<t_Index>, bool>)
	{ m_gameVariables[static_cast<u16>(t_Index)] = std::bit_cast<u8>(i_value); }
	template<auto t_Index>
	void SetVar(bool i_value)
		requires std::same_as<TypeOfVar<t_Index>, bool>
	{ m_gameVariables[static_cast<u16>(t_Index)] = i_value ? 1 : 0; }
	template<auto t_Index>
	TypeOfVar<t_Index> ReadVar() const
		requires(sizeof(TypeOfVar<t_Index>) == sizeof(u16))
	{ return std::bit_cast<TypeOfVar<t_Index>>(m_gameVariables[static_cast<u16>(t_Index)]); }
	template<auto t_Index>
	TypeOfVar<t_Index> ReadVar() const
		requires(sizeof(TypeOfVar<t_Index>) == sizeof(u8)) && (!std::same_as<TypeOfVar<t_Index>, bool>)
	{ return std::bit_cast<TypeOfVar<t_Index>>(static_cast<u8>(m_gameVariables[static_cast<u16>(t_Index)])); }
	template<auto t_Index>
	bool ReadVar() const
		requires std::same_as<TypeOfVar<t_Index>, bool>
	{ return m_gameVariables[static_cast<u16>(t_Index)] == 1; }

	void SaveVariables(u16 i_currentSaveVersion);
	bool LoadVariables(u16 i_expectedSaveVersion);
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