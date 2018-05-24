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

#ifndef __MADPLAYFILTER__
#define __MADPLAYFILTER__

struct audio_dither {
	mad_fixed_t error[3];
	mad_fixed_t random;
	unsigned long clipped_samples;
	mad_fixed_t peak_clipping;
	mad_fixed_t peak_sample;	
};

s32 audio_linear_dither( mad_fixed_t sample, struct audio_dither *dither );
	
#endif
