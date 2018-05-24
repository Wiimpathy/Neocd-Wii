/****************************************************************************
*   NeoCD Redux 0.1
*   NeoGeo CD Emulator
*   Copyright (C) 2007 softdev
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License along
*   with this program; if not, write to the Free Software Foundation, Inc.,
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
****************************************************************************/

/*** SVN 
 * $LastChangedDate: 2007-03-14 18:19:52 +0000 (Wed, 14 Mar 2007) $
 * $LastChangedRevision: 38 $
 ***/

/*** Dither algorithm adapted from Madplay 0.15.2b ***/
#include <gccore.h>
#include <mad/mad.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "madfilter.h"

enum {
	MADMIN = -MAD_F_ONE,
	MADMAX = MAD_F_ONE -1
};

s32 audio_linear_dither( mad_fixed_t sample, struct audio_dither *dither )
{
	u32 scalebits;
	mad_fixed_t output, mask, random;
	
	  /* noise shape */
	sample += dither->error[0] - dither->error[1] + dither->error[2];
	dither->error[2] = dither->error[1];
	dither->error[1] = dither->error[0] / 2;
	
	/* bias */
	output = sample + (1L << (MAD_F_FRACBITS + 1 - 16 - 1));

	scalebits = MAD_F_FRACBITS + 1 - 16;
	mask = (1L << scalebits) - 1;

	/* dither */
	random  = (dither->random * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;;
	output += (random & mask) - (dither->random & mask);
	dither->random = random;
	
	/* clip */
	if (output >= dither->peak_sample) {
		if (output > MADMAX) {
			++dither->clipped_samples;
			if (output - MADMAX > dither->peak_clipping)
				dither->peak_clipping = output - MADMAX;

			output = MADMAX;

			if (sample > MADMAX)
				sample = MADMAX;
		}
		dither->peak_sample = output;
	}
	else if (output < -dither->peak_sample) {
		if (output < MADMIN) {
			++dither->clipped_samples;
			if (MADMIN - output > dither->peak_clipping)
				dither->peak_clipping = MADMIN - output;

			output = MADMIN;

			if (sample < MADMIN)
				sample = MADMIN;
		}
		dither->peak_sample = -output;
	}
	
	/* quantize */
	output &= ~mask;

	/* error feedback */
	dither->error[0] = sample - output;

	/* scale */
	return (output >> scalebits);
}
