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
* SMB File I/O
****************************************************************************/
#ifdef USE_SMB
#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <network.h>
#include <smb.h>
#include "fileio.h"

/*** QUICK START
       SMB requires the following be correct for your environment
 ***/
#define GC_IP "192.168.1.32"		/*** IP to assign the GameCube ***/
#define GW_IP "192.168.1.100"	/*** Your gateway IP ***/
#define MASK "255.255.255.0"	/*** Your subnet mask ***/
#define SMB_USER "Guest"		/*** Your share user ***/
#define SMB_PWD "password"		/*** Your share user password ***/
#define SMB_GCID "gamecube"	/*** Machine Name of GameCube ***/
#define SMB_SVID "samsungv20"	/*** Machine Name of Server(Share) ***/
#define SMB_SHARE "gcshare"	/*** Share name on server ***/
#define SMB_IP "192.168.1.100"	/*** IP Address of share server ***/

/*** END OF QUICK START ***/
static int connected = 0;
static int netinited = 0;
int SMBTimer = 0;

#define SMBTIMEOUT ( 6000 )	/*** Some implementations timeout in 10 minutes ***/
SMBINFO smbinfo = { GC_IP, GW_IP, MASK, SMB_IP,
                    SMB_USER, SMB_PWD, SMB_GCID, SMB_SVID, SMB_SHARE
                  };

static SMBCONN smbconn = 0;
static GENHANDLER smbhandler;

/*** Pseudo file handles ***/
#define MAXFILES 20
GENFILEINFO fileinfo[MAXFILES];

/****************************************************************************
* SMBConnect
****************************************************************************/
static int
SMBConnect (void)
{
  int ret;

  if (SMBTimer > SMBTIMEOUT)
    connected = SMBTimer = 0;

  if (!connected)
    {
      if (!netinited)
        {
          ret = if_config (smbinfo.gcip, smbinfo.gwip, smbinfo.mask, 0);
          netinited = 1;
        }

      if (smbconn)
        SMB_Close (smbconn);

      if (SMB_Connect (&smbconn, smbinfo.smbuser, smbinfo.smbpwd,
                       smbinfo.smbgcid, smbinfo.smbsvid, smbinfo.smbshare,
                       smbinfo.smbip) != SMB_SUCCESS)
        {
          connected = 0;
          return 0;
        }
    }

  connected = 1;
  return 1;
}

/****************************************************************************
* SMBFindFree
*
* Find the first free fileinfo
****************************************************************************/
static int
SMBFindFree (void)
{
  int i = 0;

  while (i < MAXFILES)
    {
      if (fileinfo[i].handle == -1)
        return i;
      i++;
    }

  return -1;
}

/****************************************************************************
* SMBfopen
****************************************************************************/
static u32
SMBfopen (const char *filename, const char *mode)
{
  int handle;
  SMBDIRENTRY smbdir;
  SMBFILE smbfile;
  char fname[1024];
  int i;

  strcpy (fname, filename);
  for (i = 0; i < strlen (fname); i++)
    {
      if (fname[i] == '/')
        fname[i] = '\\';
    }

  handle = SMBFindFree ();

  if (handle < 0)
    return 0;			/*** Signal NULL handle ***/

  if (!SMBConnect ())
    return 0;

  fileinfo[handle].mode = GEN_MODE_READ;	/*** Default ***/
  fileinfo[handle].currpos = fileinfo[handle].length = 0;

  /*** Finally, try to open ***/
  if (mode[0] == 'w')
    fileinfo[handle].mode = GEN_MODE_WRITE;

  if (fileinfo[handle].mode == GEN_MODE_READ)
    {
      /*** Try to find the file first ***/
      if (SMB_FindFirst
          (fname, SMB_SRCH_READONLY | SMB_SRCH_SYSTEM | SMB_SRCH_HIDDEN,
           &smbdir, smbconn) != SMB_SUCCESS)
        return 0;

      SMB_FindClose (smbconn);

      /*** Set fileinfo ***/
      fileinfo[handle].length = smbdir.size_low;
    }

  switch (fileinfo[handle].mode)
    {
    case GEN_MODE_READ:/*** Open for reading ***/
      smbfile =
        SMB_OpenFile (fname, SMB_OPEN_READING | SMB_DENY_NONE, SMB_OF_OPEN,
                      smbconn);
      if (!smbfile)
        return 0;

      fileinfo[handle].handle = (u32) smbfile;
      break;

    case GEN_MODE_WRITE:		/*** Open for writing ***/
      smbfile = SMB_OpenFile (fname, SMB_OPEN_WRITING | SMB_DENY_NONE,
                              SMB_OF_CREATE | SMB_OF_TRUNCATE, smbconn);
      if (!smbfile)
        return 0;

      fileinfo[handle].handle = (u32) smbfile;
      break;
    }

  /*** Occasionally, smbfile array is set to 0 ***/
  return handle | 0x8000;
}

/****************************************************************************
* SMBfclose
****************************************************************************/
static int
SMBfclose (u32 fp)
{
  if (fileinfo[fp & 0x7fff].handle != -1)
    {
      SMB_CloseFile ((SMBFILE) fileinfo[fp & 0x7fff].handle);
      fileinfo[fp & 0x7fff].handle = -1;
      return 1;
    }

  return 0;
}

/****************************************************************************
* SMBfread
****************************************************************************/
static u32
SMBfread (char *buffer, int block, int length, u32 fp)
{
  int readsize;
  int handle;
  int readbytes = 0;
  int ret;
  int blocksize = 2856;
  char tmp[4096];

  handle = fp & 0x7fff;

  if (fileinfo[handle].handle == -1)
    return 0;

  if (fileinfo[handle].mode != GEN_MODE_READ)
    return 0;

  readsize = block * length;
  if (readsize <= 0)
    return 0;

  if (readsize < 2856)
    blocksize = readsize;

  if ((fileinfo[handle].currpos + readsize) > fileinfo[handle].length)
    readsize = fileinfo[handle].length - fileinfo[handle].currpos;

  while (readsize > 0)
    {
      ret = SMB_ReadFile (tmp, blocksize, fileinfo[handle].currpos,
                          (SMBFILE) fileinfo[handle].handle);

      if (ret > 0)
        {
          memcpy (buffer + readbytes, tmp, ret);
          fileinfo[handle].currpos += ret;
          readbytes += ret;
          readsize -= ret;
        }
      else
        readsize = 0;

      if (readsize < 2856)
        blocksize = readsize;
    }

  return readbytes;
}

/****************************************************************************
* SMBfwrite
*****************************************************************************/
static u32
SMBfwrite (char *buffer, int block, int length, u32 fp)
{
  int writesize;
  int handle;
  int writebytes = 0;
  int ret;
  int blocksize = 1024;

  handle = fp & 0x7fff;

  if (fileinfo[handle].handle == -1)
    return 0;

  if (fileinfo[handle].mode != GEN_MODE_WRITE)
    return 0;

  writesize = block * length;
  if (writesize <= 0)
    return 0;

  if (writesize < 1024)
    blocksize = writesize;

  while (writesize > 0)
    {
      ret =
        SMB_WriteFile (buffer + writebytes, blocksize,
                       fileinfo[handle].currpos,
                       (SMBFILE) fileinfo[handle].handle);

      if (ret > 0)
        {
          fileinfo[handle].currpos += ret;
          fileinfo[handle].length = fileinfo[handle].currpos;
          writebytes += ret;
          writesize -= ret;
        }
      else
        writesize = 0;

      if (writesize < blocksize)
        blocksize = writesize;
    }

  return writebytes;
}

/****************************************************************************
* SMBfseek
****************************************************************************/
static int
SMBfseek (u32 fp, int where, int whence)
{
  int handle;

  handle = fp & 0x7fff;

  if (fileinfo[handle].handle == -1)
    return -1;

  switch (whence)
    {
    case SEEK_END:
      if (where > 0)
        return 0;				/*** Fail ***/

      fileinfo[handle].currpos = fileinfo[handle].length + where;
      return 1;

    case SEEK_CUR:
      if ((where + fileinfo[handle].currpos) > fileinfo[handle].length)
        return 0;

      fileinfo[handle].currpos += where;
      return 1;

    case SEEK_SET:
      if (where < 0)
        return 0;

      fileinfo[handle].currpos = where;
      return 1;
    }

  return 0;
}

/****************************************************************************
* SMBftell
****************************************************************************/
static int
SMBftell (u32 fp)
{
  int handle = fp & 0x7fff;

  if (fileinfo[handle].handle == -1)
    return -1;

  return fileinfo[handle].currpos;
}

/****************************************************************************
* Generic File Interface
****************************************************************************/
void
SMB_SetHandler (void)
{
  /*** Clear ***/
  memset (&smbhandler, 0, sizeof (GENHANDLER));
  memset (&fileinfo, 0xff, sizeof (GENFILEINFO) * MAXFILES);

  /*** Set generic handlers ***/
  smbhandler.gen_fopen = SMBfopen;
  smbhandler.gen_fclose = SMBfclose;
  smbhandler.gen_fread = SMBfread;
  smbhandler.gen_fseek = SMBfseek;
  smbhandler.gen_ftell = SMBftell;
  smbhandler.gen_fwrite = SMBfwrite;
  smbhandler.gen_fcloseall = 0;
  smbhandler.gen_getdir = 0;
  smbhandler.gen_mount = 0;
  smbhandler.gen_unmount = 0;

  GEN_SetHandler (&smbhandler);
}
#endif
