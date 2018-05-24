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
 * $LastChangedDate: 2007-03-14 00:46:07 +0000 (Wed, 14 Mar 2007) $
 * $LastChangedRevision: 36 $
 ***/

/****************************************************************************
* NeoGeo CD
*
* VRAM patches from NeoGeoCDZ
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "neocdredux.h"

/****************************************************************************
* Patch Fatal Fury 2
****************************************************************************/
void
patch_vram_rbff2 (void)
{
  UINT16 offs;
  UINT16 *neogeo_vidram16 = (UINT16 *) video_vidram;

  for (offs = 0; offs < ((0x300 >> 1) << 6); offs += 2)
    {
      UINT16 tileno = neogeo_vidram16[offs];
      UINT16 tileatr = neogeo_vidram16[offs + 1];

      if (tileno == 0x7a00 && (tileatr == 0x4b00 || tileatr == 0x1400))
	{
	  neogeo_vidram16[offs] = 0x7ae9;
	  return;
	}
    }
}


/****************************************************************************
* Patch ADK world
****************************************************************************/
void
patch_vram_adkworld (void)
{
  UINT16 offs;
  UINT16 *neogeo_vidram16 = (UINT16 *) video_vidram;

  for (offs = 0; offs < ((0x300 >> 1) << 6); offs += 2)
    {
      UINT16 tileno = neogeo_vidram16[offs];
      UINT16 tileatr = neogeo_vidram16[offs + 1];

      if ((tileno == 0x14c0 || tileno == 0x1520) && tileatr == 0x0000)
	neogeo_vidram16[offs] = 0x0000;
    }
}

/****************************************************************************
* Patch CrossSwords 2
****************************************************************************/
void
patch_vram_crsword2 (void)
{
  UINT16 offs;
  UINT16 *neogeo_vidram16 = (UINT16 *) video_vidram;

  for (offs = 0; offs < ((0x300 >> 1) << 6); offs += 2)
    {
      UINT16 tileno = neogeo_vidram16[offs];
      UINT16 tileatr = neogeo_vidram16[offs + 1];

      if (tileno == 0x52a0 && tileatr == 0x0000)
	neogeo_vidram16[offs] = 0x0000;
    }
}
