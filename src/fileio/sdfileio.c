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
* SD FileIO
*
* Uses old 8.3 filenames only
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ogc/mutex.h>
#include <sys/dir.h>
#include <fat.h>
#include "neocdredux.h"
#include "fileio.h"
#include "config.h"

/* Generic File I/O */
#define MAXFILES	32
static FILE *sdfsfiles[MAXFILES];
static GENHANDLER sdhandler;
static u32 sdmutex = 0;

#define MAXDIRENTRIES 0x4000
static char *direntries[MAXDIRENTRIES];

char msg[128];

/****************************************************************************
* SDFindFree
****************************************************************************/
int SDFindFree( void )
{
  int i;

  for( i = 0; i < MAXFILES; i++ )
    {
      if ( sdfsfiles[i] == NULL )
        return i;
    }

  return -1;
}

/****************************************************************************
* SDfopen
****************************************************************************/
static u32
SDfopen (const char *filename, const char *mode)
{
  /* No writing allowed */
  if ( strstr(mode,"w") )
    {
      return 0;
    }

  /* Open for reading */
  int handle = SDFindFree();
  if ( handle == -1 )
    {
      sprintf(msg,"OUT OF HANDLES!");
      ActionScreen(msg);
      return 0;
    }

  while ( LWP_MutexLock( sdmutex ) );

  sdfsfiles[handle] =  fopen(filename, mode);
  if ( sdfsfiles[handle] == NULL )
    {
      LWP_MutexUnlock( sdmutex );
      return 0;
    }

  LWP_MutexUnlock( sdmutex );
  return handle | 0x8000;
}

/****************************************************************************
* SDfclose
****************************************************************************/
static int
SDfclose (u32 fp)
{
  while ( LWP_MutexLock( sdmutex ) );

  if( sdfsfiles[fp & 0x7FFF] != NULL )
    {
      fclose(sdfsfiles[fp & 0x7FFF]);
      sdfsfiles[fp & 0x7FFF] = NULL;
      LWP_MutexUnlock( sdmutex );
      return 1;
    }

  LWP_MutexUnlock( sdmutex );
  return 0;
}

/****************************************************************************
* SDfread
****************************************************************************/
static u32
SDfread (char *buf, int block, int len, u32 fp)
{
  int handle = ( fp & 0x7FFF );

  if( sdfsfiles[handle] == NULL )
    return 0;

  while ( LWP_MutexLock ( sdmutex ) );

  int bytesdone = fread(buf, block, len, sdfsfiles[handle]);

  LWP_MutexUnlock( sdmutex );

  return bytesdone;

}

/****************************************************************************
* SDSeek
****************************************************************************/
static int
SDfseek (u32 fp, int where, int whence)
{
  int handle = ( fp & 0x7FFF );

  if ( sdfsfiles[handle] == NULL )
    {
      sprintf(msg,"SEEK : Invalid Handle %d", handle);
      ActionScreen(msg);
      return -1;
    }

  return fseek(sdfsfiles[handle], where, whence );

}

/****************************************************************************
* SDftell
*
* Return current position
****************************************************************************/
static int
SDftell (u32 fp)
{
  int handle = ( fp & 0x7FFF );

  if ( sdfsfiles[handle] == NULL )
    {
      sprintf(msg,"FTELL : Invalid Handle %d", handle);
      ActionScreen(msg);
      return -1;
    }

  return ftell(sdfsfiles[handle]);
}

static void
SDfcloseall (void)
{
  int i;

  while ( LWP_MutexLock( sdmutex ) );

  for( i = 0; i < MAXFILES; i++ )
    {
      if ( sdfsfiles[i] != NULL )
        fclose(sdfsfiles[i]);
    }

  LWP_MutexUnlock( sdmutex );
}

/****************************************************************************
 * SDgetdir
 ****************************************************************************/
static int SDgetdir( char *thisdir )
{
  char filename[MAXPATHLEN];
  struct stat filestat;
  static int count = 0;
  int i;
  unsigned int *p;

  for( i = 0; i < count; i++ )
    free(direntries[i]);

  count = 0;

  /* open directory */
  DIR_ITER *dir = diropen (thisdir);
  if (dir) 
  {
    while ((dirnext(dir, filename, &filestat) == 0) && filename[0])
    {
      /* Only get subdirectories */
      if (filestat.st_mode & S_IFDIR)
      {
        direntries[count++] = strdup(filename);
        if ( count == MAXDIRENTRIES ) break;
      }
    }

    dirclose(dir);
  }

  if ( count > 1 )
  {
    int top,seek;
    char *t;

    for( top = 0; top < count - 1; top++ )
    {
      for( seek = top + 1; seek < count; seek++ )
      {
        if ( stricmp(direntries[top], direntries[seek]) > 0 )
        {
          t = direntries[top];
          direntries[top] = direntries[seek];
          direntries[seek] = t;
        }
      }
    }
  }

  memcpy(dirbuffer, &count, 4);
  p = (unsigned int *)(dirbuffer + 4);
  for ( i = 0; i < count; i++ )
    {
      memcpy(&p[i], &direntries[i], 4);
    }

  return count;
}


/****************************************************************************
* SDmount
****************************************************************************/
static void SDmount( void )
{
  memset (basedir, 0, 1024);
  memset (scratchdir, 0, 1024);
  memset (dirbuffer, 0, 0x10000);

  if (config.StorageDevice == 0)
  //strcpy(basedir,"sd:/neocd/");
sprintf(basedir, "sd:/%s", config.StorageFolder);
  else if (config.StorageDevice == 1)
  //strcpy(basedir,"usb:/neocd/");
sprintf(basedir, "usb:/%s", config.StorageFolder);

 else
 strcpy(basedir,"/");
	
  if ( !SDgetdir(basedir) )
    return;

  DirSelector();
  bannerscreen();
}

void
SD_SetHandler (void)
{
  /* Clear */
  memset(&sdhandler, 0, sizeof(GENHANDLER));
  memset(&sdfsfiles, 0, MAXFILES * 4);
  memset(&direntries, 0, MAXDIRENTRIES * 4);

  sdhandler.gen_fopen = SDfopen;
  sdhandler.gen_fclose = SDfclose;
  sdhandler.gen_fread = SDfread;
  sdhandler.gen_fseek = SDfseek;
  sdhandler.gen_ftell = SDftell;
  sdhandler.gen_fcloseall = SDfcloseall;
  sdhandler.gen_getdir = SDgetdir;
  sdhandler.gen_mount = SDmount;
 
  GEN_SetHandler (&sdhandler);

  /* Set mutex */
  LWP_MutexInit(&sdmutex, FALSE);

}



