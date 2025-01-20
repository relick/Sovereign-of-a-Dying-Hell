#pragma once

#include "Declare.hpp"

#include <coroutine>

namespace Game
{

struct ScenePromise;

struct SceneCoro
    : std::coroutine_handle<ScenePromise>
{
    using promise_type = ScenePromise;

    
};

struct ScenePromise
{
    SceneCoro get_return_object() { return { SceneCoro::from_promise(*this) }; }
    std::suspend_always initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
    
    std::suspend_always yield_value(int)
    {
        return {};
    }
};

class Scene
{
public:
    SceneCoro Run(Game& io_game, DialoguePrinter2& io_dp);

};

}