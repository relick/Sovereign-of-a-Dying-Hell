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

	if(!PushChar())
	{
		if (m_lineI == m_lineLen)
		{
			// End line
			m_curLine = nullptr;
			return;
		}

		// Can't print any more already, so move to next
		VDP_clearTextArea(m_xPos, m_yPos, m_lineWidth, m_lineCount);
		m_x = 0;
		m_y = 0;
		m_lastWasSpace = true;
	}
	else
	{
		// Could print more so print to end of current display
		while(PushChar())
		{}
	}
}

//------------------------------------------------------------------------------
bool DialoguePrinter::Update
(
)
{
	if (m_curLine)
	{
		bool const noMoreThisDisp = PushChar();
		return noMoreThisDisp && m_lineI == m_lineLen;
	}
	return true;
}


//------------------------------------------------------------------------------
bool DialoguePrinter::PushChar
(
)
{
	if (m_y >= m_lineCount || m_lineI == m_lineLen)
	{
		return false; // Need to progress or we're at the end of the line
	}

	// Main logic for stepping through and deciding how our display progresses
	if(m_lineI == 0)
	{
		VDP_clearTextArea(m_xPos, m_yPos, m_lineWidth, m_lineCount);
	}

	// Word wrapping handling, run on first character of each word
	if(m_lastWasSpace)
	{
		char const* word = m_curLine + m_lineI;
		u16 wordLen = 0;
		while (*word != ' ' && *word != '\n' && *word != '\0')
		{
			// Handle :) by ignoring length from the colon
			if (*word == ':' && *(word + 1) == ')')
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
				m_x = 0;
				m_y++;
			}
		}
	}

	char str[2] = { m_curLine[m_lineI], '\0'};

	m_lastWasSpace = str[0] == ' ';

	// Minor processing for font
	if(str[0] == '\xA3')
	{
		str[0] = '{'; // renders as £ in vn_font
	}
	else if(str[0] == ':')
	{
		if(m_curLine[m_lineI+1] == ')')
		{
			str[0] = '`'; // renders as :) in vn_font
			m_lineI++;
		}
	}
	else if(str[0] == '\n')
	{
		m_lastWasSpace = true;
		m_x = 0;
		m_y++;
		m_lineI++;
		return m_y < m_lineCount;
	}

	VDP_drawText(str, m_x + m_xPos, m_y + m_yPos);

	// Update counters
	m_lineI++;

	m_x++;
	if(m_x >= m_lineWidth)
	{
		m_x = 0;
		m_y++;
		if(m_y >= m_lineCount)
		{
			return false;
		}
	}

	return true;
}
}