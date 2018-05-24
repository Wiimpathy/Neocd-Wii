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
***    Header File    ***
************************/

#ifndef	Z80INTRF_H
#define Z80INRTF_H

#include "z80.h"

void z80_init (void);
int mz80exec (int cycles);
void mz80nmi (void);
void mz80int (int irq);
void z80_exit (void);

extern UINT8 subcpu_memspace[65536];
extern int sound_code;
extern int pending_command;
extern int result_code;
extern int z80_cycles;

#endif /* Z80INTRF_H */
