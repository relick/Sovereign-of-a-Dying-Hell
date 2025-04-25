#pragma once

#include <genesis.h>
#include <limits>

template<typename, typename T_Core>
class TypeSafeID final
{
public:
	using Core = T_Core;

	constexpr TypeSafeID() = default;
	explicit constexpr TypeSafeID(Core i_id) : m_id{ i_id } {}
	
	constexpr Core Get() const { return m_id; }
	constexpr bool Valid() const { return m_id != std::numeric_limits<Core>::max(); }

	constexpr bool operator==(TypeSafeID const& o) const { return m_id == o.m_id; }

private:
	Core m_id{ std::numeric_limits<Core>::max() };
};