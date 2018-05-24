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

/************************
*** Z80 CPU Interface ***
************************/

//-- Include Files ----------------------------------------------------------
#include	<gccore.h>
#include	"neocdredux.h"
#include 	<stdio.h>
#include 	<stdlib.h>

//-- Exported Functions -----------------------------------------------------
unsigned char subcpu_memspace[65536];
int sound_code = 0;
int pending_command = 0;
int result_code = 0;
int nmi_over = 0;

//---------------------------------------------------------------------------
/*** 
* Whilst tracing the Z80, it appears that PORT0/8/C are always written
* with the same value after NMI.
*
* At this point, the M68K reads from address 0x32000 to determine the
* success or failure of the previous NMI.
*
* On NMI, sound_code == 3 is total reset. 
* sound_code == 1 is a partial reset.
* 
* So really, it should not be called sound_code, as it does this
* very important task too.
***/

void
PortWrite (UINT16 PortNo, UINT8 data)
{

  switch (PortNo & 0xff)
    {

    case 0x4:
      YM2610_control_port_0_A_w (0, data);
      break;

    case 0x5:
      YM2610_data_port_0_A_w (0, data);
      break;

    case 0x6:
      YM2610_control_port_0_B_w (0, data);
      break;

    case 0x7:
      YM2610_data_port_0_B_w (0, data);
      break;

    case 0x8:
      /* NMI enable / acknowledge? (the data written doesn't matter) */
      break;

    case 0xc:
      result_code = data;
      break;

    case 0x18:
      /* NMI disable? (the data written doesn't matter) */
      break;

    case 0x80:
      cdda_stop ();
      break;

    default:
      //printf("Unimplemented Z80 Write Port: %x data: %x\n",PortNo&0xff,data);
      break;
    }
}

//---------------------------------------------------------------------------
UINT8
PortRead (UINT16 PortNo)
{
  switch (PortNo & 0xff)
    {
    case 0x0:
      pending_command = 0;
      return sound_code;
      break;

    case 0x4:
      return YM2610_status_port_0_A_r (0);
      break;

    case 0x5:
      return YM2610_read_port_0_r (0);
      break;

    case 0x6:
      return YM2610_status_port_0_B_r (0);
      break;

    default:
      //printf("Unimplemented Z80 Read Port: %d\n",PortNo&0xff);
      break;
    };
  return 0;
}
