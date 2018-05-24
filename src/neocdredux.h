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
*
* Acknowledgements
*
*	NGCD 0.8 "The Kick Ass NeoGeo CD Emulator" - Martinez Fabrice
*	NeoCD/SDL 0.3.1				   - Fosters
*	NeogeoCDZ				   - NJ
*	Musashi M68000 C Core			   - Karl Stenerud
*	Mame Z80 C Core				   - Juergen Buchmueller
*	Sound core and miscellaneous		   - Mame Team
*
****************************************************************************/

/*** SVN 
 * $LastChangedDate: 2007-03-20 14:08:16 +0000 (Tue, 20 Mar 2007) $
 * $LastChangedRevision: 44 $
 ***/

#ifndef __NEOGEOREDUX__
#define __NEOGEOREDUX__

#define APPTITLE "NeoCD-Redux"
#define VERSION "0.1.52"
#define SAMPLE_RATE 48000

/*** SDL Replacement Types ***/
typedef char Sint8;
typedef unsigned char Uint8;
typedef short Sint16;
typedef unsigned short Uint16;
typedef int Sint32;
typedef unsigned int Uint32;

/*** Header files ***/
#include <gccore.h>
#include <ogcsys.h>
#include <zlib.h>
#include "m68k.h"
#include "z80intrf.h"
#include "fileio.h"
#include "smbfileio.h"
#include "memory.h"
#include "cpuintf.h"
#include "cdrom.h"
#include "cdaudio.h"
#include "patches.h"
#include "video.h"
#include "gxvideo.h"
#include "pd4990a.h"
#include "input.h"
#include "timer.h"
#include "streams.h"
#include "ay8910.h"
#include "2610intf.h"
#include "sound.h"
#include "gcaudio.h"
#include "mcard.h"
#include "gui.h"
#include "iso9660.h"
#include "dirsel.h"
#include "dvdfileio.h"
#include "sdfileio.h"

/*** Functions ***/
void neogeo_swab(const void *src1, const void *src2, int isize);
int neogeo_redux(void);
void neogeo_decode_spr(unsigned char *mem, unsigned int offset,
		       unsigned int length);
void neogeo_decode_fix(unsigned char *mem, unsigned int offset,
		       unsigned int length);
void neogeo_cdda_control(void);
void neogeo_prio_switch(void);
void neogeo_exit(void);
void neogeo_exit_cdplayer(void);
void neogeo_new_game(void);
void neogeo_trace(void);
void neogeocd_exit(void);
void neogeo_reset(void);

// bool SupportedIOS(u32 ios);
// bool SaneIOS(u32 ios);

/*** Globals ***/
extern unsigned char *neogeo_rom_memory;
extern unsigned char *neogeo_prg_memory;
extern unsigned char *neogeo_fix_memory;
extern unsigned char *neogeo_ipl_memory;
extern unsigned char *neogeo_spr_memory;
extern unsigned char *neogeo_pcm_memory;
extern unsigned char neogeo_memorycard[8192];
extern char neogeo_game_vectors[0x100];
extern char neogeo_region;
extern int patch_ssrpg;

#endif
