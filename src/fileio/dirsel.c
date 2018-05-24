/****************************************************************************
* NeoGeo Directory Selector
*
* As there is no 'ROM' to speak of, use the directory as the starting point
****************************************************************************/

/*** SVN
 * $LastChangedDate: 2007-03-19 08:08:55 +0000 (Mon, 19 Mar 2007) $
 * $LastChangedRevision: 41 $
 ***/
#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HW_RVL
#include <wiiuse/wpad.h>
#endif

#include "neocdredux.h"

#define PAGE_SIZE 8

char basedir[1024];
char scratchdir[1024];
char dirbuffer[0x10000] ATTRIBUTE_ALIGN (32);


/****************************************************************************
* DrawDirSelector
****************************************************************************/
static void
DrawDirSelector (int maxfile, int menupos, int currsel)
{
  int i;
  int j = 158;
  int *p = (int *) dirbuffer;
  char *m;
  char display[40];
  char inverse[40];

  DrawScreen ();

  for (i = menupos; i < (menupos + PAGE_SIZE) && (i < maxfile); i++)
    {
      m = (char *) p[i + 1];
      memset (display, 0, 40);
      memcpy (display, m, 32);

      if (i == currsel)
        {
          setfgcolour (BMPANE);
          setbgcolour (INVTEXT);
          memset (inverse, 32, 40);
          inverse[32] = 0;
          memcpy (inverse, display, strlen (display));
          gprint (64, j, inverse, TXT_DOUBLE);
        }
      else
        {
          setfgcolour (COLOR_WHITE);
          setbgcolour (BMPANE);
          gprint (64, j, display, TXT_DOUBLE);
        }

      j += 32;
    }

  ShowScreen ();
}

/****************************************************************************
* DirSelector
*
* A == Enter directory
* B == Parent directory
* X == Set directory
****************************************************************************/
void
DirSelector (void)
{
  int *p = (int *) dirbuffer;
  char *m;
  int maxfile, i;
  int currsel = 0;
  int menupos = 0;
  int redraw = 1;
  unsigned short joy;
#ifdef HW_RVL
  u32 wpad;
  
#endif

  int quit = 0;

  maxfile = p[0];

  while (!quit)
    {
      if (redraw)
        {
          DrawDirSelector (maxfile, menupos, currsel);
          redraw = 0;
        }

      joy = PAD_ButtonsDown (0);

#ifdef HW_RVL

      wpad= WPAD_ButtonsDown (0);
	  struct expansion_t ext;
	  // Extension Wiimote
	  WPAD_Expansion(WPAD_CHAN_0, &ext);

     switch (ext.type)
	   {
		   case WPAD_EXP_NONE:     
				/* directions */
				if ( wpad & WPAD_BUTTON_DOWN)           joy |= PAD_BUTTON_RIGHT;
				if ( wpad & WPAD_BUTTON_UP)				joy |= PAD_BUTTON_LEFT;
				if ( wpad & WPAD_BUTTON_RIGHT)          joy |= PAD_BUTTON_UP; 
				if ( wpad & WPAD_BUTTON_LEFT)           joy |= PAD_BUTTON_DOWN;
				/*  buttons */
				/*  buttons */
				  /* default mapping should match buttons original position */
				if ( wpad & WPAD_BUTTON_1)              joy |= PAD_BUTTON_A;
				if ( wpad & WPAD_BUTTON_2)              joy |= PAD_BUTTON_B;
				if ( wpad & WPAD_BUTTON_A)              joy |= PAD_BUTTON_X;
				if (wpad & WPAD_BUTTON_B)               joy |= PAD_BUTTON_Y;
				if (wpad & WPAD_BUTTON_MINUS)           joy |= PAD_TRIGGER_L;
				if ( wpad & WPAD_BUTTON_PLUS)           joy |= PAD_TRIGGER_R;
				break;
		  
		   case WPAD_EXP_CLASSIC:   
				/* directions */
				if ( wpad & WPAD_CLASSIC_BUTTON_DOWN)                joy |= PAD_BUTTON_DOWN;
				if ( wpad & WPAD_CLASSIC_BUTTON_UP)         		 joy |= PAD_BUTTON_UP;
				if ( wpad & WPAD_CLASSIC_BUTTON_RIGHT)               joy |= PAD_BUTTON_RIGHT; 
				if ( wpad & WPAD_CLASSIC_BUTTON_LEFT)                joy |= PAD_BUTTON_LEFT;
				/*  buttons */
				  /* default mapping should match buttons original position */
				if ( wpad & WPAD_CLASSIC_BUTTON_A)                   joy |= PAD_BUTTON_A;
				if ( wpad & WPAD_CLASSIC_BUTTON_B)                   joy |= PAD_BUTTON_B;
				if ( wpad & WPAD_CLASSIC_BUTTON_X)                   joy |= PAD_BUTTON_X;
				if ( wpad & WPAD_CLASSIC_BUTTON_Y)                   joy |= PAD_BUTTON_Y;
				if (wpad & WPAD_CLASSIC_BUTTON_MINUS)  				 joy |= PAD_TRIGGER_L;
				if ( wpad & WPAD_CLASSIC_BUTTON_PLUS)                joy |= PAD_TRIGGER_R;
				break;		   
	   }

#endif

      if (joy & PAD_BUTTON_DOWN)
        {
          currsel++;
          if (currsel == maxfile)
            currsel = menupos = 0;
          if ((currsel - menupos) >= PAGE_SIZE)
            menupos += PAGE_SIZE;

          redraw = 1;
        }

      if (joy & PAD_BUTTON_UP)
        {
          currsel--;
          if (currsel < 0)
            {
              currsel = maxfile - 1;
              menupos = currsel - PAGE_SIZE + 1;
            }

          if (currsel < menupos)
            menupos -= PAGE_SIZE;

          if (menupos < 0)
            menupos = 0;

          redraw = 1;
        }

	  if (joy & PAD_TRIGGER_L)
        {
          menupos -= PAGE_SIZE;
		  currsel = menupos;
          if (currsel < 0)
            {
              currsel = maxfile - 1;
              menupos = currsel - PAGE_SIZE + 1;
            }

          if (menupos < 0)
            menupos = 0;

          redraw = 1;
        }

      if (joy & PAD_TRIGGER_R)
        {
		  menupos += PAGE_SIZE;
		  currsel = menupos;
		  if (currsel > maxfile)
            currsel = menupos = 0;

          redraw = 1;
        }

      if (joy & PAD_BUTTON_A)
        {
          strcpy (scratchdir, basedir);

          if (scratchdir[strlen (scratchdir) - 1] != '/')
            strcat (scratchdir, "/");

          m = (char *) p[currsel + 1];

          strcat (scratchdir, m);

          if (GEN_getdir (scratchdir))
            {
              maxfile = p[0];
              currsel = menupos = 0;
              strcpy (basedir, scratchdir);
            }
          else
            GEN_getdir (basedir);

          redraw = 1;
        }

      if (joy & PAD_BUTTON_B)
        {
          if (strcmp (basedir, "/"))
            {
              strcpy (scratchdir, basedir);
              for (i = strlen (scratchdir) - 1; i >= 0; i--)
                {
                  if (scratchdir[i] == '/')
                    {
                      if (i == 0)
                        strcpy (scratchdir, "/");
                      else
                        scratchdir[i] = 0;

                      if (GEN_getdir (scratchdir))
                        {
                          maxfile = p[0];
                          currsel = menupos = 0;
                          strcpy (basedir, scratchdir);
                        }
                      break;
                    }
                }
            }
          redraw = 1;
        }

      if (joy & PAD_BUTTON_X)
        {
          /*** Set basedir to mount point ***/
          if (basedir[strlen (basedir) - 1] != '/')
            strcat (basedir, "/");

          m = (char *) p[currsel + 1];
          strcat (basedir, m);
          quit = 1;
        }
    }
          /*** Remove any still held buttons ***/
      while (PAD_ButtonsHeld (0))
        VIDEO_WaitVSync ();
#ifdef HW_RVL
      while (WPAD_ButtonsHeld(0))
        VIDEO_WaitVSync ();
#endif

}

