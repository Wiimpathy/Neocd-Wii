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
* NeoCD/SDL CPU Timer Interface
*
* Module to schedule CPU, sound timers and interrupts
****************************************************************************/
#ifndef __CPUINTF__
#define __CPUINTF__

#define Z80_USEC   ((1.0 / 4000000.0))
#define M68K_USEC ((1.0 / 12000000.0 ))

typedef struct
{
  int cycles_done;
  int cycles_frame;
  int cycles_scanline;
  int cycles_overrun;
  int irq_state;
  double total_cycles;
  double total_time_us;
  int boost;
} CPU;

typedef struct
{
  char gamename[20];
  int nowait_irqack;
  int raster_interrupt_enabled;
} GAMECONFIG;

void neogeo_runframe (void);
void neogeo_configure_game (char *gamename);

#endif
