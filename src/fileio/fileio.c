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
 * $LastChangedDate: 2007-03-26 11:50:27 +0100 (Mon, 26 Mar 2007) $
 * $LastChangedRevision: 49 $
 ***/

/****************************************************************************
* Generic File I/O
*
* This module attempts to provide a single interface for file I/O.
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include "fileio.h"

static GENHANDLER genhandler;

/****************************************************************************
* GEN_SetHandler
*
* Call BEFORE using any of the other functions
****************************************************************************/
void
GEN_SetHandler (GENHANDLER * g)
{
  memcpy (&genhandler, g, sizeof (GENHANDLER));
}

/****************************************************************************
* GEN_fopen
*
* Passthrough fopen
****************************************************************************/
u32
GEN_fopen (const char *filename, const char *mode)
{
  if (genhandler.gen_fopen == NULL)
    return 0;			/*** NULL - no file or handler ***/

  return (genhandler.gen_fopen) (filename, mode);
}

/****************************************************************************
* GEN_fread
*
* Passthrough fread
****************************************************************************/
u32
GEN_fread (char *buffer, int block, int length, u32 fp)
{
  if (genhandler.gen_fread == NULL)
    return 0;

  return (genhandler.gen_fread) (buffer, block, length, fp);
}

/****************************************************************************
* GEN_fwrite
*
* Passthrough fwrite
****************************************************************************/
u32
GEN_fwrite (char *buffer, int block, int length, u32 fp)
{
  if (genhandler.gen_fwrite == NULL)
    return 0;

  return (genhandler.gen_fwrite) (buffer, block, length, fp);
}

/****************************************************************************
* GEN_fclose
*
* Passthrough fclose
****************************************************************************/
int
GEN_fclose (u32 fp)
{
  if (genhandler.gen_fclose == NULL)
    return 0;

  return (genhandler.gen_fclose) (fp);
}

/****************************************************************************
* GEN_fseek
*
* Passthrough fseek
****************************************************************************/
int
GEN_fseek (u32 fp, int where, int whence)
{
  if (genhandler.gen_fseek == NULL)
    return 0;

  return (genhandler.gen_fseek) (fp, where, whence);
}

/****************************************************************************
* GEN_ftell
*
* Passthrough ftell
****************************************************************************/
int
GEN_ftell (u32 fp)
{
  if (genhandler.gen_ftell == NULL)
    return -1;

  return (genhandler.gen_ftell) (fp);
}

/****************************************************************************
* GEN_fcloseall
***************************************************************************/
void
GEN_fcloseall (void)
{
  if (genhandler.gen_fcloseall == NULL)
    return;

  (genhandler.gen_fcloseall) ();
}


/****************************************************************************
* GEN_fcloseall
***************************************************************************/
int
GEN_getdir (char *dir)
{
  if (genhandler.gen_getdir == NULL)
    return 0;

  return (genhandler.gen_getdir) (dir);
}

/****************************************************************************
* GEN_mount
***************************************************************************/
void
GEN_mount (void)
{
  if (genhandler.gen_mount == NULL)
    return;

  (genhandler.gen_mount) ();
}

