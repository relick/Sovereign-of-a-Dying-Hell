#pragma once

#include "Declare.hpp"

#include <coroutine>
#include <optional>

namespace Game
{

class SceneCoro
{
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type
    {
        SceneCoro get_return_object() { return { handle_type::from_promise(*this) }; }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(int) {}
        void unhandled_exception() {}
        
        std::suspend_always yield_value(int)
        {
            return {};
        }
    };

    SceneCoro() = default;
    SceneCoro(handle_type i_h) : m_handle{ i_h } {}
    ~SceneCoro() { if (m_handle) { m_handle->destroy(); } }

    SceneCoro(SceneCoro const&) = delete;
    SceneCoro(SceneCoro&& i_o)
    : m_handle{ std::move(i_o.m_handle) }
    {
        i_o.m_handle.reset();
    }
    SceneCoro& operator=(SceneCoro const&) = delete;
    SceneCoro& operator=(SceneCoro&& i_o)
    {
        if (m_handle) { m_handle->destroy(); }
        m_handle = std::move(i_o.m_handle);
        i_o.m_handle.reset();
        return *this;
    }

    explicit operator bool()
    {
        return !m_handle->done();
    }

    void operator()()
    {
        m_handle->resume();
    }

private:
    std::optional<handle_type> m_handle;
};

class Scene
{
public:
    SceneCoro Run(Game& io_game, DialoguePrinter2& io_dp);

};

}