#include "DialoguePrinter.hpp"

#include <genesis.h>

#include <cstring>

namespace Game
{

//------------------------------------------------------------------------------
DialoguePrinter::DialoguePrinter
(
	u16 i_xPos,
	u16 i_yPos,
	u16 i_lineWidth,
	u16 i_lineCount
)
	: m_xPos{ i_xPos }
	, m_yPos{ i_yPos }
	, m_lineWidth{ i_lineWidth }
	, m_lineCount{ i_lineCount }
{
	m_clearBuf.resize(m_lineWidth * m_lineCount);
	std::fill(m_clearBuf.begin(), m_clearBuf.end(), TILE_ATTR_FULL(PAL3, 0, 0, 0, TILE_FONT_INDEX));
}

//------------------------------------------------------------------------------
void DialoguePrinter::PushLine
(
	char const* i_line
)
{
	m_curLine = i_line;
	m_lineLen = std::strlen(i_line);
	m_lineI = 0;
	m_lastWasSpace = true;
	m_x = 0;
	m_y = 0;
	m_lineBuf = {};
}

//------------------------------------------------------------------------------
void DialoguePrinter::Next
(
)
{
	if(!m_curLine)
	{
		return;
	}

	if(!PushChar(false))
	{
		if (m_lineI == m_lineLen)
		{
			// End line
			m_curLine = nullptr;
			return;
		}

		// Can't print any more already, so move to next
		//VDP_fillTileMapRect(VDP_getTextPlane(), TILE_ATTR_FULL(PAL3, 0, 0, 0, TILE_FONT_INDEX), m_xPos, m_yPos, m_lineWidth, m_lineCount);
		VDP_setTileMapDataRect(VDP_getTextPlane(), m_clearBuf.data(), m_xPos, m_yPos, m_lineWidth, m_lineCount, m_lineWidth, DMA_QUEUE);
		m_x = 0;
		m_y = 0;
		m_lineBuf = {};
		m_lastWasSpace = true;
	}
	else
	{
		// Could print more so print to end of current display
		while(PushChar(false))
		{
		}
		FlushBuf();
	}
}

//------------------------------------------------------------------------------
bool DialoguePrinter::Update
(
)
{
	if (m_curLine)
	{
		bool const noMoreThisDisp = PushChar(true);
		return noMoreThisDisp && m_lineI == m_lineLen;
	}
	return true;
}


//------------------------------------------------------------------------------
bool DialoguePrinter::PushChar
(
	bool i_displayPerChar
)
{
	if (m_y >= m_lineCount || m_lineI == m_lineLen)
	{
		return false; // Need to progress or we're at the end of the line
	}

	// Main logic for stepping through and deciding how our display progresses
	if(m_lineI == 0)
	{
		//VDP_clearTextArea(m_xPos, m_yPos, m_lineWidth, m_lineCount);
		//VDP_fillTileMapRect(VDP_getTextPlane(), TILE_ATTR_FULL(PAL3, 0, 0, 0, TILE_FONT_INDEX), m_xPos, m_yPos, m_lineWidth, m_lineCount);
		VDP_setTileMapDataRect(VDP_getTextPlane(), m_clearBuf.data(), m_xPos, m_yPos, m_lineWidth, m_lineCount, m_lineWidth, DMA_QUEUE);
		m_lineBuf = {};
	}

	// Word wrapping handling, run on first character of each word
	if(m_lastWasSpace)
	{
		char const* word = m_curLine + m_lineI;
		u16 wordLen = 0;
		while (*word != ' ' && *word != '\n' && *word != '\0')
		{
			// Handle £ and :) by ignoring length from one of the characters
			if ((*word == ':' && *(word + 1) == ')') || (*word == '\xC2' && *(word + 1) == '\xA3'))
			{
				--wordLen;
			}
			++wordLen;
			++word;
		}

		if(m_x + wordLen >= m_lineWidth)
		{
			if(m_y + 1 >= m_lineCount)
			{
				return false;
			}
			else
			{
				FlushBuf();
				m_lineBuf = {};
				m_x = 0;
				m_y++;
			}
		}
	}

	m_lineBuf[m_x] = m_curLine[m_lineI];

	m_lastWasSpace = m_lineBuf[m_x] == ' ';

	// Minor processing for font
	if (m_lineBuf[m_x] == '\xC2')
	{
		if (m_curLine[m_lineI + 1] == '\xA3')
		{
			m_lineBuf[m_x] = '{'; // renders as £ in vn_font
			m_lineI++;
		}
	}
	else if (m_lineBuf[m_x] == ':')
	{
		if(m_curLine[m_lineI+1] == ')')
		{
			m_lineBuf[m_x] = '`'; // renders as :) in vn_font
			m_lineI++;
		}
	}
	else if (m_lineBuf[m_x] == '\n')
	{
		m_lineBuf[m_x] = '\0';
		FlushBuf();
		m_lineBuf = {};
		m_lastWasSpace = true;
		m_x = 0;
		m_y++;
		m_lineI++;
		return m_y < m_lineCount;
	}

	// Update counters
	m_lineI++;

	if (i_displayPerChar)
	{
		FlushBuf();
	}

	m_x++;
	if(m_x >= m_lineWidth)
	{
		FlushBuf();
		m_lineBuf = {};
		m_x = 0;
		m_y++;
		if(m_y >= m_lineCount)
		{
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
void DialoguePrinter::FlushBuf()
{
	if(m_lineBuf[0] == '\0')
	{
		return;
	}
	VDP_drawTextEx(VDP_getTextPlane(), m_lineBuf.data(), TILE_ATTR_FULL(PAL3, 0, 0, 0, 0), m_xPos, m_y + m_yPos, DMA_QUEUE);
}
}