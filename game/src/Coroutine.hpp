#pragma once

#include "Declare.hpp"

#include <concepts>
#include <coroutine>
#include <optional>

namespace Game
{

template<typename T>
concept Suspension = std::same_as<T, std::suspend_always> || std::same_as<T, std::suspend_never>;

template<typename T_CoroTag, Suspension T_InitialSuspend, typename T_YieldReturnType = void>
class Coroutine
{
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type
    {
        std::optional<T_YieldReturnType> m_val;

        Coroutine get_return_object() { return { handle_type::from_promise(*this) }; }
        T_InitialSuspend initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(T_YieldReturnType&& i_val) { m_val = std::move(i_val); }
        void unhandled_exception() {}
        
        std::suspend_always yield_value(T_YieldReturnType&& i_val)
        {
            m_val = std::move(i_val); 
            return {};
        }
    };

    Coroutine() = default;
    Coroutine(handle_type i_h) : m_handle{ i_h } {}
    ~Coroutine()
    {
        if (m_handle) { m_handle->destroy(); }
    }

    Coroutine(Coroutine const&) = delete;
    Coroutine(Coroutine&& i_o)
    : m_handle{ std::move(i_o.m_handle) }
    {
        i_o.m_handle.reset();
    }
    Coroutine& operator=(Coroutine const&) = delete;
    Coroutine& operator=(Coroutine&& i_o)
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

template<typename T_CoroTag, Suspension T_InitialSuspend>
class Coroutine<T_CoroTag, T_InitialSuspend, void>
{
public:
    struct empty {};

    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type
    {
        Coroutine get_return_object() { return { handle_type::from_promise(*this) }; }
        T_InitialSuspend initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}

        std::suspend_always yield_value(empty)
        {
            return {};
        }
    };

    Coroutine() = default;
    Coroutine(handle_type i_h) : m_handle{ i_h } {}
    ~Coroutine()
    {
        if (m_handle) { m_handle->destroy(); }
    }

    Coroutine(Coroutine const&) = delete;
    Coroutine(Coroutine&& i_o)
    : m_handle{ std::move(i_o.m_handle) }
    {
        i_o.m_handle.reset();
    }
    Coroutine& operator=(Coroutine const&) = delete;
    Coroutine& operator=(Coroutine&& i_o)
    {
        if (m_handle) { m_handle->destroy(); }
        m_handle = std::move(i_o.m_handle);
        i_o.m_handle.reset();
        return *this;
    }

    explicit operator bool() const
    {
        return !m_handle->done();
    }

    // Simply returns the status of the coroutine
    bool operator()()
    {
        m_handle->resume();
        return static_cast<bool>(*m_handle);
    }

private:
    std::optional<handle_type> m_handle;
};

}