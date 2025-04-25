#pragma once

#include <genesis.h>
#include <limits>

template<typename, typename T_Core = u16>
class TypeSafeID final
{
public:
	using Core = T_Core;

	TypeSafeID() = default;
	explicit TypeSafeID(Core i_id) : m_id{ i_id } {}
	
	Core Get() const { return m_id; }
	bool Valid() const { return m_id != std::numeric_limits<Core>::max(); }

private:
	Core m_id{ std::numeric_limits<Core>::max() };
};