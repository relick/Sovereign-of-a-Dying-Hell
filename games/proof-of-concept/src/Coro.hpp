#pragma once

#include "Declare.hpp"

#include <coroutine>
#include <optional>

namespace Game
{

template<typename T_CoroTag, typename T_YieldReturnType>
class Coro
{
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type
    {
        std::optional<T_YieldReturnType> m_val;

        Coro get_return_object() { return { handle_type::from_promise(*this) }; }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(T_YieldReturnType&& i_val) { m_val = std::move(i_val); }
        void unhandled_exception() {}
        
        std::suspend_always yield_value(T_YieldReturnType&& i_val)
        {
            m_val = std::move(i_val); 
            return {};
        }
    };

    Coro() = default;
    Coro(handle_type i_h) : m_handle{ i_h } {}
    ~Coro() { if (m_handle) { m_handle->destroy(); } }

    Coro(Coro const&) = delete;
    Coro(Coro&& i_o)
    : m_handle{ std::move(i_o.m_handle) }
    {
        i_o.m_handle.reset();
    }
    Coro& operator=(Coro const&) = delete;
    Coro& operator=(Coro&& i_o)
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

    T_YieldReturnType const& operator()()
    {
        m_handle->resume();
        return *(m_handle->promise().m_val);
    }

private:
    std::optional<handle_type> m_handle;
};

}