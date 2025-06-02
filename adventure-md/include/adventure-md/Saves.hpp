#pragma once

#include "Declare.hpp"
#include "Debug.hpp"

#include <SGDK/sram.h>

#include <cstring>
#include <span>
#include <type_traits>
#include <vector>

namespace Saves
{

namespace Detail
{

inline constexpr u16 SRAMSize = 32 * 1024; // 32 KiB
inline constexpr u16 UsableSRAMSize = SRAMSize / 2; // Make a mirror for data integrity.

template<u16 t_SlotCount>
inline constexpr u16 SlotDataSize = UsableSRAMSize / ( t_SlotCount );

template<u16 t_SlotCount>
inline constexpr u16 SRAMOffset( u16 i_slotNum ) { return i_slotNum * SlotDataSize<t_SlotCount>; }
inline constexpr u16 Mirror( u16 i_offset ) { return i_offset + UsableSRAMSize; }

template<u16 t_SlotCount> struct SlotCountHelper { static constexpr u16 SlotCount = t_SlotCount; };

template<typename T>
concept ValidSlotCount = requires {
	{T::SlotCount} -> std::convertible_to<u16>;
	requires( ( ( SlotDataSize<T::SlotCount> ) * T::SlotCount ) == UsableSRAMSize ); // Must be exactly divisible into SRAM size
};

template<typename T>
concept VersionEnum = std::is_enum_v<T> && requires{ requires( sizeof( T ) == 1 ); };

[[nodiscard]] inline bool TrueOnFirstCallOnly_Global()
{
	static bool s_seenBefore = false;
	if ( s_seenBefore )
	{
		return false;
	}
	s_seenBefore = true;
	return true;
}

template<typename T>
inline constexpr std::span<std::byte const, sizeof( T )> AsByteSpan( T const& i_data )
{
	return std::span<std::byte const, sizeof( T )>{ reinterpret_cast< std::byte const* >( &i_data ), sizeof( T ) };
}

template<typename T>
inline constexpr std::span<std::byte, sizeof( T )> AsByteSpan( T& i_data )
{
	return std::span<std::byte, sizeof( T )>{ reinterpret_cast< std::byte* >( &i_data ), sizeof( T ) };
}

// Writes in words to both main and mirror at same time
class SRAMSaver
{
	u32 m_cursor{ 0 };
	u32 m_cursor_mirror{ 0 };

	void Write2Bytes( std::byte a, std::byte b )
	{
		u16 word = ( ( u16 )a ) | ( ( ( u16 )b ) << 8 );
		SRAM_writeWord( m_cursor, word );
		m_cursor += 2;
		SRAM_writeWord( m_cursor_mirror, word );
		m_cursor_mirror += 2;
	}

public:
	explicit constexpr SRAMSaver( u16 i_startOffset )
		: m_cursor{ i_startOffset }
		, m_cursor_mirror{ Mirror( i_startOffset ) }
	{
		Assert( m_cursor < UsableSRAMSize, "SRAM offset too high" );
		Assert( UsableSRAMSize >= m_cursor_mirror && m_cursor_mirror < SRAMSize, "SRAM offset too high" );
	}

	template<typename T>
	constexpr SRAMSaver& operator<<( T const& i_data )
		requires( std::is_trivially_copyable_v<T> && !std::is_pointer_v<T> )
	{
		return operator<<( AsByteSpan( i_data ) );
	}

	template<size_t t_Extent>
	constexpr SRAMSaver& operator<<( std::span<std::byte const, t_Extent> i_data )
	{
		// Write all but last 1 byte if odd
		u16 const loopEnd = i_data.size() - ( i_data.size() % 2 );
		for ( u16 i = 0; i < loopEnd; i += 2 )
		{
			Write2Bytes( i_data[ i ], i_data[ i + 1 ] );
		}

		// Handle final byte if odd
		if ( loopEnd < i_data.size() )
		{
			Write2Bytes( i_data[ loopEnd ], std::byte() );
		}

		return *this;
	}
};

// Reads in words from either main or mirror
class SRAMLoader
{
	u32 m_cursor{ 0 };

	void Read2Bytes( std::byte& a, std::byte& b )
	{
		u16 word = SRAM_readWord( m_cursor );
		m_cursor += 2;
		a = ( std::byte )word;
		b = ( std::byte )( word >> 8 );
	}

public:
	explicit constexpr SRAMLoader( u16 i_startOffset, bool i_readMirror )
		: m_cursor{ i_readMirror ? Mirror( i_startOffset ) : i_startOffset }
	{
		Assert( i_startOffset < UsableSRAMSize, "SRAM offset too high" );
	}

	template<typename T>
	SRAMLoader& operator>>( T& o_data )
		requires( std::is_trivially_copyable_v<T> && !std::is_pointer_v<T> )
	{
		return operator>>( AsByteSpan( o_data ) );
	}

	template<size_t t_Extent>
	SRAMLoader& operator>>( std::span<std::byte, t_Extent> o_data )
	{
		// Read all but last 1 byte if odd
		u16 const loopEnd = o_data.size() - ( o_data.size() % 2 );
		for ( u16 i = 0; i < loopEnd; i += 2 )
		{
			Read2Bytes( o_data[ i ], o_data[ i + 1 ] );
		}

		// Handle final byte if odd
		if ( loopEnd < o_data.size() )
		{
			std::byte dummy;
			Read2Bytes( o_data[ loopEnd ], dummy );
		}

		return *this;
	}
};

inline constexpr u32 c_hashBasis = 0x811C'9DC5;
inline constexpr u32 c_hashPrime = 0x0100'0193;
inline u32 Hash( auto /*std::span<std::byte const, ...>*/ i_data, u32 i_startHash = c_hashBasis)
{
	u32 hash = i_startHash;
	for ( std::byte b : i_data )
	{
		hash ^= ( u32 )b;
		hash *= c_hashPrime;
	}

	return hash;
}

}

struct MagicString
{
	u16 a = 0x5F83;
	u16 b = 0xC823;
	u16 c = 0x27FE;
	u16 d = 0x7580;

	[[nodiscard]] constexpr bool operator==( MagicString const& o ) const
	{
		return std::memcmp( this, &o, sizeof( MagicString ) ) == 0;
	}
};

struct SRAMSlotHeader
{
	MagicString m_magicString;
	bool m_saved;
	u8 m_gameVersion;
	u8 m_reserved0[ 2 ];
	u32 m_fnvHash;
	u8 m_reserved1[ 4 ];

	SRAMSlotHeader()
		: m_saved{ false }
		, m_gameVersion{ UINT8_MAX }
		, m_fnvHash{ 0 }
	{}

	template<Detail::VersionEnum T_Version>
	SRAMSlotHeader( T_Version i_version, auto /*std::span<std::byte const, ...>*/ i_data)
		: m_saved{ true }
		, m_gameVersion{ ( u8 )i_version }
		, m_fnvHash{
			Detail::Hash(
				Detail::AsByteSpan( m_saved ),
				Detail::Hash(
					Detail::AsByteSpan( m_gameVersion ),
					Detail::Hash( i_data )
				)
			)
		}
	{}
};

template<u16 t_SlotUserDataSize>
class Serialiser
{
	std::array<std::byte, t_SlotUserDataSize> m_userBuffer;
	u16 m_cursor{ 0 };

public:
	template<typename T>
	Serialiser& operator<<( T const& i_value )
		requires( std::is_trivially_copyable_v<T> && !std::is_pointer_v<T> )
	{
		Assert( m_cursor + sizeof( T ) < t_SlotUserDataSize, "Too much data written to save!" );

		std::memcpy( m_userBuffer.data() + m_cursor, &i_value, sizeof( T ) );
		m_cursor += sizeof( T );
		return *this;
	}

	std::span<std::byte const, t_SlotUserDataSize> Data() const
	{
		return m_userBuffer;
	}
};

template<u16 t_SlotUserDataSize>
class Deserialiser
{
	std::array<std::byte, t_SlotUserDataSize> m_userBuffer;
	u16 m_cursor{ 0 };

public:
	template<typename T>
	Deserialiser& operator>>( T& o_value )
		requires( std::is_trivially_copyable_v<T> && !std::is_pointer_v<T> )
	{
		Assert( m_cursor + sizeof( T ) < t_SlotUserDataSize, "Too much data read from save!" );

		std::memcpy( &o_value, m_userBuffer.data() + m_cursor, sizeof( T ) );
		m_cursor += sizeof( T );
		return *this;
	}

	// Filled by a loader only
	std::span<std::byte, t_SlotUserDataSize> Data()
	{
		return m_userBuffer;
	}
};

template<u16 t_SlotCount, Detail::ValidSlotCount = Detail::SlotCountHelper<t_SlotCount>>
class SaveMemory
{
	SaveMemory()
	{
		if ( !IsMemoryInitialised() )
		{
			[[maybe_unused]] bool const success = ClearAllSlots();
			Assert( success, "Failed to clear all slots when initialising SRAM" );
		}
	}

	[[nodiscard]] bool IsMemoryInitialised() const
	{
		SRAMSlotHeader checkHeader;
		auto fnTest =
			[ &checkHeader ]( u16 i_slotNum, bool i_mirror )
			{
				// Load just the header.
				SRAMSlotHeader header;
				Detail::SRAMLoader loader( Detail::SRAMOffset<SlotCount>( i_slotNum ), i_mirror );

				loader >> header;

				// See how it compares in terms of string and save status/version
				// Note: No hash check because the hash determines the validity of a given save
				// and not the validity of the entire save memory.
				return header.m_magicString == checkHeader.m_magicString
					&& ( header.m_saved || header.m_gameVersion == checkHeader.m_gameVersion );
			};

		// Check all slots and mirrors
		u8 failCount = 0;
		for ( u16 i = 0; i < SlotCount; ++i )
		{
			failCount += fnTest( i, false ) ? 0 : 1;
			failCount += fnTest( i, true ) ? 0 : 1;
		}

		// If more than half are bad then memory should be initialised. If it's
		// less than half, that's not great, but suggests that it was initialised before,
		// and is probably recoverable on a per-slot basis.
		return failCount > SlotCount;
	}

public:
	static constexpr u16 SlotCount = t_SlotCount;
	static constexpr u16 SlotUserDataSize = Detail::SlotDataSize<SlotCount> - sizeof( SRAMSlotHeader );

	static SaveMemory& Get()
	{
		static SaveMemory s_saveMemory;
#if DEBUG
		static bool s_seenBefore = false;
		if ( !s_seenBefore )
		{
			Assert( Detail::TrueOnFirstCallOnly_Global(), "Only one SaveMemory singleton should exist." );
		}
		s_seenBefore = true;
#endif
		return s_saveMemory;
	}

	[[nodiscard]] bool ClearAllSlots()
	{
		SRAMSlotHeader header;

		for ( u16 i = 0; i < SlotCount; ++i )
		{
			Detail::SRAMSaver saver( Detail::SRAMOffset<SlotCount>( i ) );
			saver << header;
		}

		return true;
	}

	[[nodiscard]] bool ClearSlot( u16 i_slotNum )
	{
		SRAMSlotHeader header;
		Detail::SRAMSaver saver( Detail::SRAMOffset<SlotCount>( i_slotNum ) );
		saver << header;

		return true;
	}

	Serialiser<SlotUserDataSize> MakeSerialiser() const { return {}; }

	template<Detail::VersionEnum T_Version>
	[[nodiscard]] bool WriteSlotData
	(
		u16 i_slotNum,
		Serialiser<SlotUserDataSize> const& i_serialisedData,
		T_Version i_version
	)
	{
		Detail::SRAMSaver saver( Detail::SRAMOffset<SlotCount>( i_slotNum ) );

		SRAMSlotHeader header( i_version, i_serialisedData.Data() );
		saver << header;
		saver << i_serialisedData.Data();

		return true;
	}

	Deserialiser<SlotUserDataSize> MakeDeserialiser() const { return {}; }

	template<Detail::VersionEnum T_Version>
	[[nodiscard]] bool ReadSlotData
	(
		u16 i_slotNum,
		Deserialiser<SlotUserDataSize>& o_serialisedData,
		T_Version i_oldestSupportedVersion
	) const
	{
		SRAMSlotHeader header;
		SRAMSlotHeader checkHeader;

		auto fnLoad =
			[ i_slotNum, &o_serialisedData, i_oldestSupportedVersion, &header, &checkHeader ]( bool i_mirror )
			{
				Detail::SRAMLoader loader( Detail::SRAMOffset<SlotCount>( i_slotNum ), i_mirror );

				loader >> header;

				if ( header.m_magicString != checkHeader.m_magicString )
				{
					return false;
				}

				if ( header.m_saved && i_oldestSupportedVersion <= ( T_Version )header.m_gameVersion )
				{
					loader >> o_serialisedData.Data();
					checkHeader = SRAMSlotHeader{ ( T_Version )header.m_gameVersion, o_serialisedData.Data() };
					return header.m_fnvHash == checkHeader.m_fnvHash;
				}
				else
				{
					return false;
				}
			};

		// First try main
		if ( fnLoad( false ) )
		{
			return true;
		}

		// If fell through then main was bad. Try mirror
		return fnLoad( true );
	}
};

}