#pragma once

#include <algorithm>
#include <array>

template<size_t N>
using RawLiteralRef = char const(&)[N];

// N excludes null terminator
template<size_t N = 0>
class StringLiteral
{
	std::array<char, N + 1> m_str;

public:
	template<size_t M>
	consteval StringLiteral(RawLiteralRef<M> i_str) requires (M == N + 1)
	{
		std::copy(i_str, i_str + N, m_str.begin());
		m_str[N] = 0;
	}

	template<size_t M>
	consteval StringLiteral(std::array<char, M> i_str) requires (M == N + 1)
	{
		std::copy(i_str.begin(), i_str.end(), m_str.begin());
	}

	consteval StringLiteral()
	{
		m_str[N] = 0;
	}

	constexpr char const* c_str() const { return m_str.data(); }

	template<size_t A, size_t B>
	friend consteval StringLiteral<A + B> operator+(StringLiteral<A> i_a, StringLiteral<B> i_b);
	template<size_t A, size_t B>
	friend consteval StringLiteral<A - 1 + B> operator+(RawLiteralRef<A> i_a, StringLiteral<B> i_b);
	template<size_t A, size_t B>
	friend consteval StringLiteral<A + B - 1> operator+(StringLiteral<A> i_a, RawLiteralRef<B> i_b);

	struct iterator
	{
		char const* m_ptr{};

		constexpr iterator& operator+=(ptrdiff_t n) { m_ptr += n; return *this; }
		friend inline constexpr iterator operator+(iterator a, ptrdiff_t n) { return { a.m_ptr + n }; }
		friend inline constexpr iterator operator+(ptrdiff_t n, iterator a) { return { a.m_ptr + n }; }
		constexpr iterator& operator-=(ptrdiff_t n) { m_ptr -= n; return *this; }
		friend inline constexpr iterator operator-(iterator a, ptrdiff_t n) { return { a.m_ptr - n }; }
		friend inline constexpr ptrdiff_t operator-(iterator a, iterator b) { return a.m_ptr - b.m_ptr; }
		constexpr char const& operator[](size_t n) { return m_ptr[n]; }
		friend inline constexpr bool operator<(iterator a, iterator b) { return a.m_ptr < b.m_ptr; }
		friend inline constexpr bool operator>(iterator a, iterator b) { return a.m_ptr > b.m_ptr; }
		friend inline constexpr bool operator>=(iterator a, iterator b) { return a.m_ptr >= b.m_ptr; }
		friend inline constexpr bool operator<=(iterator a, iterator b) { return a.m_ptr >= b.m_ptr; }
	};
	using const_iterator = iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	constexpr iterator begin() const { return cbegin(); }
	constexpr const_iterator cbegin() const { return { m_str.data() }; }
	constexpr iterator end() const { return cend(); }
	constexpr const_iterator cend() const { return { m_str.data() + N + 1 }; }
	constexpr reverse_iterator rbegin() const { return crbegin(); }
	constexpr const_reverse_iterator crbegin() const { return { m_str.data() + N + 1 }; }
	constexpr reverse_iterator rend() const { return crend(); }
	constexpr const_reverse_iterator crend() const { return { m_str.data() }; }

	constexpr bool empty() const { return N > 0; }
	constexpr size_t size() const { return N; }
	constexpr size_t length() const { return size(); }
};

template<size_t A, size_t B>
consteval StringLiteral<A + B> operator+(StringLiteral<A> i_a, StringLiteral<B> i_b)
{
	StringLiteral<A + B> concat;
	std::copy(i_a.m_str.begin(), i_a.m_str.begin() + A, concat.m_str.begin());
	std::copy(i_b.m_str.begin(), i_b.m_str.begin() + B, concat.m_str.begin() + A);
	return concat;
}

template<size_t A, size_t B>
consteval StringLiteral<A - 1 + B> operator+(RawLiteralRef<A> i_a, StringLiteral<B> i_b)
{
	StringLiteral<A - 1 + B> concat;
	std::copy(i_a, i_a + A - 1, concat.m_str.begin());
	std::copy(i_b.m_str.begin(), i_b.m_str.begin() + B, concat.m_str.begin() + A - 1);
	return concat;
}

template<size_t A, size_t B>
consteval StringLiteral<A + B - 1> operator+(StringLiteral<A> i_a, RawLiteralRef<B> i_b)
{
	StringLiteral<A + B - 1> concat;
	std::copy(i_a.m_str.begin(), i_a.m_str.begin() + A, concat.m_str.begin());
	std::copy(i_b, i_b + B - 1, concat.m_str.begin() + A);
	return concat;
}

template<size_t N>
StringLiteral(RawLiteralRef<N>) -> StringLiteral<N - 1>;

template<size_t N>
struct OperatorStringLiteral { std::array<char, N> m_str; constexpr OperatorStringLiteral(char const(&i_str)[N]) { std::copy(i_str, i_str + N, m_str.begin()); } };

template<OperatorStringLiteral t_StrLit>
inline constexpr auto operator ""_sl()
{
	return StringLiteral<std::size(t_StrLit.m_str) - 1>{ t_StrLit.m_str };
}

struct ConstantString
{
	char const* m_str{};

	template<size_t N>
	ConstantString(StringLiteral<N> i_strLit) : m_str{ i_strLit.c_str() } {}
	ConstantString(char const* i_str) : m_str{ i_str } {}

	operator char const*() const { return m_str; }
};