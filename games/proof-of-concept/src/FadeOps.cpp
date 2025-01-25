#include "FadeOps.hpp"

#include <genesis.h>

#define PALETTEFADE_FRACBITS    8
#define PALETTEFADE_ROUND_VAL   ((1 << (PALETTEFADE_FRACBITS - 1)) - 1)

namespace System
{

FadeOp CreateFade
(
	u16* io_startDstPal,
	u16 const* i_endPal,
	u16 i_size,
	u16 i_numFrame
)
{
	FadeOp newFade;

	newFade.m_size = i_size;
	newFade.m_counter = i_numFrame;
	newFade.m_dstPal = io_startDstPal;

	s16* palR = newFade.m_palR.data();
	s16* palG = newFade.m_palG.data();
	s16* palB = newFade.m_palB.data();

	s16* stepR = newFade.m_stepR.data();
	s16* stepG = newFade.m_stepG.data();
	s16* stepB = newFade.m_stepB.data();

	u16 len = newFade.m_size;
	while (len--)
	{
		u16 const s = *io_startDstPal++;
		s16 const RS = ((s & VDPPALETTE_REDMASK) >> VDPPALETTE_REDSFT) << PALETTEFADE_FRACBITS;
		s16 const GS = ((s & VDPPALETTE_GREENMASK) >> VDPPALETTE_GREENSFT) << PALETTEFADE_FRACBITS;
		s16 const BS = ((s & VDPPALETTE_BLUEMASK) >> VDPPALETTE_BLUESFT) << PALETTEFADE_FRACBITS;

		*palR++ = RS + PALETTEFADE_ROUND_VAL;
		*palG++ = GS + PALETTEFADE_ROUND_VAL;
		*palB++ = BS + PALETTEFADE_ROUND_VAL;

		u16 const d = *i_endPal++;
		s16 const RD = ((d & VDPPALETTE_REDMASK) >> VDPPALETTE_REDSFT) << PALETTEFADE_FRACBITS;
		s16 const GD = ((d & VDPPALETTE_GREENMASK) >> VDPPALETTE_GREENSFT) << PALETTEFADE_FRACBITS;
		s16 const BD = ((d & VDPPALETTE_BLUEMASK) >> VDPPALETTE_BLUESFT) << PALETTEFADE_FRACBITS;

		*stepR++ = divs(RD - RS, i_numFrame);
		*stepG++ = divs(GD - GS, i_numFrame);
		*stepB++ = divs(BD - BS, i_numFrame);
	}

	return newFade;
}

bool FadeOp::DoFadeStep
(
)
{
	// not yet done ?
	if (m_counter > 0)
	{
		// then prepare fade palette for next frame
		s16* palR = m_palR.data();
		s16* palG = m_palG.data();
		s16* palB = m_palB.data();

		s16* stepR = m_stepR.data();
		s16* stepG = m_stepG.data();
		s16* stepB = m_stepB.data();
		u16* dst = m_dstPal;

		// compute the next fade palette
		u16 i = m_size;
		while (i--)
		{
			u16 col;

			const u16 R = *palR + *stepR++;
			const u16 G = *palG + *stepG++;
			const u16 B = *palB + *stepB++;

			*palR++ = R;
			*palG++ = G;
			*palB++ = B;

			col = ((R >> PALETTEFADE_FRACBITS) << VDPPALETTE_REDSFT) & VDPPALETTE_REDMASK;
			col |= ((G >> PALETTEFADE_FRACBITS) << VDPPALETTE_GREENSFT) & VDPPALETTE_GREENMASK;
			col |= ((B >> PALETTEFADE_FRACBITS) << VDPPALETTE_BLUESFT) & VDPPALETTE_BLUEMASK;

			*dst++ = col;
		}


		// not yet done
		--m_counter;
		return true;
	}
	else
	{
		// done
		return false;
	}
}

}