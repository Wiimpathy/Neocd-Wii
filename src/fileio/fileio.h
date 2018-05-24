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
 * $LastChangedDate: 2007-04-08 05:35:23 +0100 (Sun, 08 Apr 2007) $
 * $LastChangedRevision: 51 $
 ***/

/****************************************************************************
* Generic File I/O
*
* This module attempts to provide a single interface for file I/O.
****************************************************************************/
#ifndef __GENFILEIO__
#define __GENFILEIO__

#define GEN_MODE_READ 1
#define GEN_MODE_WRITE 2

typedef struct
  {
    u32 handle;
    int length;
    int currpos;
    long long int offset_on_media64;
    int mode;
  }
GENFILEINFO;

typedef struct
  {
    u32 (*gen_fopen) (const char *filename, const char *mode);
    u32 (*gen_fread) (char *buffer, int block, int length, u32 fp);
    u32 (*gen_fwrite) (char *buffer, int block, int length, u32 fp);
    int (*gen_fclose) (u32 fp);
    int (*gen_fseek) (u32 fp, int where, int whence);
    int (*gen_ftell) (u32 fp);
    int (*gen_getdir)(char *dir);
    void (*gen_mount) (void);
    void (*gen_fcloseall) (void);
  }
GENHANDLER;

typedef u32 GENFILE;

extern u32 GEN_fopen (const char *filename, const char *mode);
extern u32 GEN_fread (char *buffer, int block, int length, u32 fp);
extern u32 GEN_fwrite (char *buffer, int block, int length, u32 fp);
extern int GEN_fclose (u32 fp);
extern int GEN_fseek (u32 fp, int where, int whence);
extern int GEN_ftell (u32 fp);
extern int GEN_getdir(char *dir);
extern void GEN_mount ( void );
extern void GEN_fcloseall (void);

extern void GEN_SetHandler (GENHANDLER * g);

#endif
