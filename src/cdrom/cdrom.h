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

/****************************************************************************
* CDROM Data interface
*
* Simulate all CDROM reading using standard files.
****************************************************************************/
#ifndef __NEOCDROM__
#define __NEOCDROM__

/*** Globals ***/
extern char cdpath[1024];
extern int img_display;
extern int ipl_in_progress;

/*** Prototypes ***/
int cdrom_process_ipl(void);
int recon_filetype(char *ext);
int cdrom_mount(char *mount);
void cdrom_load_files(void);
void neogeo_upload(void);
void cdrom_load_title(void);
int cdrom_process_ipl(void);
void neogeo_end_upload(void);
void neogeo_start_upload(void);
void neogeo_ipl(void);

typedef struct
  {
    char fname[16];
    short bank;
    int offset;
    int unk1;
    short unk2;
    short image;
    short unk3;
  }
__attribute__ ((__packed__)) LOADFILE;

#endif
