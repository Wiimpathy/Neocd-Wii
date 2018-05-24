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
 * $LastChangedDate: 2007-03-23 13:41:34 +0000 (Fri, 23 Mar 2007) $
 * $LastChangedRevision: 48 $
 ***/

#include <stdio.h>
#include <debug.h>
#include "utils/usb2storage.h"
#include "utils/mload.h"
#include <fat.h>

#ifdef HW_RVL
#include <di/di.h>
#include <wiiuse/wpad.h>
#endif

#include "neocdredux.h"
#include "config.h"
/*#include "pal60.h"*/

/*** 2D Video Globals ***/
GXRModeObj *vmode;    /*** Graphics Mode Object ***/
u32 *xfb[2] = { NULL, NULL };  /*** Framebuffers ***/
int whichfb = 0;    /*** Frame buffer toggle ***/

extern int FrameTicker;

#ifdef USE_SMB
extern int SMBTimer;
#endif

#define DEBUGON 0

#if DEBUGON
const char *dbg_local_ip = "192.168.1.32";
const char *dbg_netmask = "255.255.255.0";
const char *dbg_gw = "192.168.1.100";
#endif


/****************************************************************************
 * IOS support
 ***************************************************************************/
static bool FindIOS(u32 ios)
{
	s32 ret;
	u32 n;
	
  u64 *titles = NULL;
	u32 num_titles=0;
	
	ret = ES_GetNumTitles(&num_titles);
	if (ret < 0)
		return false;
	
	if(num_titles < 1) 
		return false;
	
	titles = (u64 *)memalign(32, num_titles * sizeof(u64) + 32);
	if (!titles)
		return false;
	
	ret = ES_GetTitles(titles, num_titles);
	if (ret < 0)
	{
		free(titles);
		return false;
	}
	
	for(n=0; n < num_titles; n++)
	{
		if((titles[n] & 0xFFFFFFFF)==ios) 
		{
			free(titles); 
			return true;
		}
	}
  free(titles); 
	return false;
}

/****************************************************************************
* Frameticker and SMB timer
****************************************************************************/
static void framestart(u32 arg)
{
#ifdef USE_SMB
    SMBTimer++;
#endif
    FrameTicker++;
}

/****************************************************************************
* PAD update
****************************************************************************/
static void pad_update(u32 arg)
{
  PAD_ScanPads();
#ifdef HW_RVL
  WPAD_ScanPads();
#endif
}

/****************************************************************************
* Initialise Video
*
* Before doing anything in libogc, it's recommended to configure a video
* output.
****************************************************************************/
static void Initialise(void)
{

    VIDEO_Init();    /*** ALWAYS CALL FIRST IN ANY LIBOGC PROJECT!
             Not only does it initialise the video 
             subsystem, but also sets up the ogc os
        ***/

	if(IOS_GetVersion() != 202 && FindIOS(202))
		IOS_ReloadIOS(202);

	if(IOS_GetVersion() == 202)
	{
		/* load EHCI module & enable USB2 driver */
		if(mload_init() >= 0 && load_ehci_module())
			USB2Enable(true);
	}
#if DEBUGON
    DEBUG_Init(2424);
    _break();
#endif
    PAD_Init();      /*** Initialise pads for input ***/
#ifdef HW_RVL
    WPAD_Init();
#endif

  /*** Configure NTSC 
      When looking on a real Wii, NTSC and PALM 
      are identical modes. All that's left
      is to update the OTR and all is good and happy :D
  ***/
//vmode=&TVEurgb60Hz480Int;
    vmode = VIDEO_GetPreferredMode(NULL);

  /*** Let libogc configure the mode ***/
    VIDEO_Configure(vmode);


  /*** Now configure the framebuffer. 
       Really a framebuffer is just a chunk of memory
       to hold the display line by line.
  ***/

    xfb[0] = (u32 *) MEM_K0_TO_K1((u32 *) SYS_AllocateFramebuffer(vmode));

  /*** I prefer also to have a second buffer for double-buffering.
       This is not needed for the console demo.
  ***/
    xfb[1] = (u32 *) MEM_K0_TO_K1((u32 *) SYS_AllocateFramebuffer(vmode));

  /*** Define a console ***/
    console_init(xfb[0], 20, 64, vmode->fbWidth, vmode->xfbHeight,
     vmode->fbWidth * 2);

  /*** Clear framebuffer to black ***/
    VIDEO_ClearFrameBuffer(vmode, xfb[0], COLOR_BLACK);
    VIDEO_ClearFrameBuffer(vmode, xfb[1], COLOR_BLACK);

  /*** Correct OTR etc ***/
/***    u32 *v = (u32 *)0xCC002000;
    int i;
    for( i = 0; i < 64; i++ )
      *v++ = vpal60[i];
***/

  /*** Set the framebuffer to be displayed at next VBlank ***/
    VIDEO_SetNextFramebuffer(xfb[0]);

  /*** Increment frameticker and timer ***/
    VIDEO_SetPreRetraceCallback(framestart);

  /*** Get the PAD status updated by libogc ***/
    VIDEO_SetPostRetraceCallback(pad_update);
    VIDEO_SetBlack(0);

  /*** Update the video for next vblank ***/
    VIDEO_Flush();
    VIDEO_WaitVSync();    /*** Wait for VBL ***/
    if (vmode->viTVMode & VI_NON_INTERLACE)
      VIDEO_WaitVSync();

#ifndef HW_RVL
    DVD_Init ();
#endif
	fatInitDefault();

}

/****************************************************************************
* Main
*
* hello.c - a very basic hello world application
****************************************************************************/
int main(void)
{
    Initialise();  /*** Setup video ***/
	Load_config();
#ifdef USEDVD
    DVD_SetHandler();
#else
#ifdef USESD
    SD_SetHandler(); 
#else
    SMB_SetHandler();
#endif
#endif

    neogeo_redux();  /*** Run emulator ***/

    while (1);

    return 0;    /*** Keep gcc happy ***/
}
