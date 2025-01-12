#pragma once

#include "Declare.hpp"

namespace Game
{

class DialoguePrinter
{
	u16 m_xPos{0};
	u16 m_yPos{0};
	u16 m_lineWidth{40};
	u16 m_lineCount{28};

	char const* m_curLine{nullptr};
	u16 m_lineLen{0};
	u16 m_lineI{MAX_U16};
	bool m_lastWasSpace{true};
	u16 m_x{0};
	u16 m_y{0};
	char m_lineBuf[41]{};

public:
	DialoguePrinter(u16 i_xPos, u16 i_yPos, u16 i_lineWidth, u16 i_lineCount);

	// Set the next line to be displayed
	void PushLine(char const* i_line);

	// Progress line display to the next step (e.g. when button pressed)
	void Next();

	// Call every time a char should display (i.e. dialogue speed)
	// Returns true when line has been fully shown
	bool Update();

private:
	// Returns false when no more can be pushed to the screen until progressed
	bool PushChar(bool i_displayPerChar);

	// Draws m_lineBuf
	void FlushBuf();
};
}