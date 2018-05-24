/****************************************************************************
* NeoCD Redux
*
* GUI File Selector
****************************************************************************/

/*** SVN 
 * $LastChangedDate: 2007-03-27 13:54:15 +0100 (Tue, 27 Mar 2007) $
 * $LastChangedRevision: 50 $
 ***/

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef HW_RVL
#include <wiiuse/wpad.h>
#include <di/di.h>
#endif

#include "neocdredux.h"
#include "gxvideo.h"
#include "backdrop.h"
#include "banner.h"
#include "config.h"

/*** GC 2D Video ***/
extern u32 *xfb[2];
extern u32 whichfb;
extern GXRModeObj *vmode;

/*** libOGC Default Font ***/
extern u8 console_font_8x16[];

static u32 fgcolour = COLOR_WHITE;
static u32 bgcolour = COLOR_BLACK;
static char background[1280 * 480] ATTRIBUTE_ALIGN (32);
static char bannerunc[banner_WIDTH * banner_HEIGHT * 2] ATTRIBUTE_ALIGN (32);
static void unpack (void);
extern int defbutton;
/****************************************************************************
* plotpixel
****************************************************************************/
static void
plotpixel (int x, int y)
{
  u32 pixel;

  pixel = xfb[whichfb][(y * 320) + (x >> 1)];

  if (x & 1)
    xfb[whichfb][(y * 320) + (x >> 1)] =
      (pixel & 0xffff00ff) | (COLOR_WHITE & 0xff00);
  else
    xfb[whichfb][(y * 320) + (x >> 1)] =
      (COLOR_WHITE & 0xffff00ff) | (pixel & 0xff00);
}

/****************************************************************************
* roughcircle
****************************************************************************/
static void
roughcircle (int cx, int cy, int x, int y)
{
  if (x == 0)
    {
      plotpixel (cx, cy + y);    /*** Anti ***/
      plotpixel (cx, cy - y);
      plotpixel (cx + y, cy);
      plotpixel (cx - y, cy);
    }
  else
    {
      if (x == y)
  {
    plotpixel (cx + x, cy + y);      /*** Anti ***/
    plotpixel (cx - x, cy + y);
    plotpixel (cx + x, cy - y);
    plotpixel (cx - x, cy - y);
  }
      else
  {
    if (x < y)
      {
        plotpixel (cx + x, cy + y);      /*** Anti ***/
        plotpixel (cx - x, cy + y);
        plotpixel (cx + x, cy - y);
        plotpixel (cx - x, cy - y);
        plotpixel (cx + y, cy + x);
        plotpixel (cx - y, cy + x);
        plotpixel (cx + y, cy - x);
        plotpixel (cx - y, cy - x);
      }

  }
    }
}

/****************************************************************************
* circle
****************************************************************************/
void
circle (int cx, int cy, int radius)
{
  int x = 0;
  int y = radius;
  int p = (5 - radius * 4) / 4;

  roughcircle (cx, cy, x, y);

  while (x < y)
    {
      x++;
      if (p < 0)
  p += (x << 1) + 1;
      else
  {
    y--;
    p += ((x - y) << 1) + 1;
  }
      roughcircle (cx, cy, x, y);
    }
}

/****************************************************************************
* drawchar
****************************************************************************/
static void
drawchar (int x, int y, char c)
{
  int yy, xx;
  u32 colour[2];
  int offset;
  u8 bits;

  offset = (y * 320) + (x >> 1);

  for (yy = 0; yy < 16; yy++)
    {
      bits = console_font_8x16[((c << 4) + yy)];

      for (xx = 0; xx < 4; xx++)
  {
    if (bits & 0x80)
      colour[0] = fgcolour;
    else
      colour[0] = bgcolour;

    if (bits & 0x40)
      colour[1] = fgcolour;
    else
      colour[1] = bgcolour;

    xfb[whichfb][offset + xx] =
      (colour[0] & 0xffff00ff) | (colour[1] & 0xff00);

    bits <<= 2;
  }

      offset += 320;
    }
}

/****************************************************************************
* drawcharw
****************************************************************************/
static void
drawcharw (int x, int y, char c)
{
  int yy, xx;
  int offset;
  int bits;

  offset = (y * 320) + (x >> 1);

  for (yy = 0; yy < 16; yy++)
    {
      bits = console_font_8x16[((c << 4) + yy)];

      for (xx = 0; xx < 8; xx++)
  {
    if (bits & 0x80)
      xfb[whichfb][offset + xx] = xfb[whichfb][offset + 320 + xx] =
        fgcolour;
    else
      xfb[whichfb][offset + xx] = xfb[whichfb][offset + 320 + xx] =
        bgcolour;

    bits <<= 1;
  }

      offset += 640;
    }
}

/****************************************************************************
* gprint
****************************************************************************/
void
gprint (int x, int y, char *text, int mode)
{
  int n;
  int i;

  n = strlen (text);
  if (!n)
    return;

  if (mode != TXT_DOUBLE)
    {
	if (mode == 3)
		{
      for (i = 0; i < n; i++, x += 10)
	  drawchar (x, y, text[i]);
		}
	else
		{
	  for (i = 0; i < n; i++, x += 8)
	  drawchar (x, y, text[i]);
		}  
    }
  else
    {
      for (i = 0; i < n; i++, x += 16)
  drawcharw (x, y, text[i]);
    }
}

/****************************************************************************
* DrawScreen
****************************************************************************/
void
DrawScreen (void)
{
  static int inited = 0;

  if (!inited)
    {
      unpack ();
      inited = 1;
    }

  whichfb ^= 1;
  memcpy (xfb[whichfb], background, 1280 * 480);

}

/****************************************************************************
* ShowScreen
****************************************************************************/
void
ShowScreen (void)
{
  VIDEO_SetNextFramebuffer (xfb[whichfb]);
  VIDEO_Flush ();
  VIDEO_WaitVSync ();
}

/****************************************************************************
* setfgcolour
****************************************************************************/
void
setfgcolour (u32 colour)
{
  fgcolour = colour;
}

/****************************************************************************
* setbgcolour
****************************************************************************/
void
setbgcolour (u32 colour)
{
  bgcolour = colour;
}

/****************************************************************************
* WaitButtonA
****************************************************************************/
void
WaitButtonA (void)
{
#ifdef HW_RVL
  while (!(PAD_ButtonsHeld (0) & PAD_BUTTON_A) && !(WPAD_ButtonsHeld (0) & WPAD_BUTTON_2) && !(WPAD_ButtonsHeld (0) & WPAD_CLASSIC_BUTTON_A))
    VIDEO_WaitVSync ();
#else
  while (!(PAD_ButtonsHeld (0) & PAD_BUTTON_A))
    VIDEO_WaitVSync ();
#endif

#ifdef HW_RVL
  while ((PAD_ButtonsHeld (0) & PAD_BUTTON_A) || (WPAD_ButtonsHeld (0) & WPAD_BUTTON_2) || (WPAD_ButtonsHeld (0) & WPAD_CLASSIC_BUTTON_A))
    VIDEO_WaitVSync ();
#else
  while (PAD_ButtonsHeld (0) & PAD_BUTTON_A)
    VIDEO_WaitVSync ();
#endif
}

/****************************************************************************
* ActionScreen
****************************************************************************/
void
ActionScreen (char *msg)
{
  int n;
  char pressa[] = "Press A to continue";

  DrawScreen ();

  n = strlen (msg);
  fgcolour = COLOR_WHITE;
  bgcolour = BMPANE;

  gprint ((640 - (n * 16)) >> 1, 248, msg, TXT_DOUBLE);

  gprint (168, 288, pressa, TXT_DOUBLE);

  ShowScreen ();

  WaitButtonA ();
}

/****************************************************************************
* InfoScreen
****************************************************************************/
void
InfoScreen (char *msg)
{
  int n;

  DrawScreen ();

  n = strlen (msg);
  fgcolour = COLOR_WHITE;
  bgcolour = BMPANE;

  gprint ((640 - (n * 16)) >> 1, 264, msg, TXT_DOUBLE);

  ShowScreen ();
  
}

/****************************************************************************
* Credits
****************************************************************************/
void
Credits (void)
{
int quit = 0;
char Title[]   = "CREDITS";
char Coder[]   = "Coding: Wiimpathy, Jacobeian (1st version)";
char Thanks[]  ="Thanks to:";
char Softdev[] ="Softdev and his Neo-CD Redux"; 
char Devkit[]  ="DevkitPPC/libogc";
char L[]       ="L. for her patience and support.";
char Fun[]     ="Have fun !";
char iosVersion[20];
char appVersion[20]="NeoCD-Wii v0.5";

#ifdef HW_RVL
	sprintf(iosVersion, "IOS : %d", IOS_GetVersion());
#endif

  DrawScreen ();
  
  fgcolour = COLOR_WHITE;
  bgcolour = BMPANE;

  gprint (250, 150, Title, TXT_DOUBLE);
  gprint (60, 210, Coder, 3);
  gprint (60, 260, Thanks, 3);
  gprint (60, 290, Softdev, 3);
  gprint (60, 310, Devkit, 3);
  gprint (60, 330, L, 3);
  gprint (60, 350, Fun, 3);
  gprint (510, 390, iosVersion, 0);
  gprint (60, 390, appVersion, 0);

  ShowScreen ();
 while (PAD_ButtonsHeld (0))
    VIDEO_WaitVSync ();
#ifdef HW_RVL
  while (WPAD_ButtonsHeld (0))
    VIDEO_WaitVSync ();
#endif
while (!quit)
    {
      if (PAD_ButtonsHeld (0) & PAD_BUTTON_A)
  quit = 2;

      else if (PAD_ButtonsHeld (0) & PAD_BUTTON_B)
  quit = 1;
#ifdef HW_RVL
      else if (WPAD_ButtonsHeld (0) & (WPAD_BUTTON_2|WPAD_CLASSIC_BUTTON_A))
  quit = 2;

      else if (WPAD_ButtonsHeld (0) & (WPAD_BUTTON_1|WPAD_CLASSIC_BUTTON_B))
  quit = 1;
#endif
  }


}


/****************************************************************************
* unpack
****************************************************************************/
static void
unpack (void)
{
  unsigned long res, inbytes, outbytes;

  inbytes = backdrop_COMPRESSED;
  outbytes = backdrop_RAW;

  res = uncompress ((Bytef *)background, &outbytes, (Bytef *)backdrop, inbytes);

  inbytes = banner_COMPRESSED;
  outbytes = banner_RAW;

  res = uncompress ((Bytef *)bannerunc, &outbytes, (Bytef *)banner, inbytes);
}

/****************************************************************************
* loadnewgame
****************************************************************************/
int
loadnewgame (void)
{
  char titles[3][20] = { {"Load new game?\0"}, {"A - Yes\0"}, {"B - No \0"} };
  int i;
  int quit = 0;
	
   

  while (PAD_ButtonsHeld (0))
    VIDEO_WaitVSync ();
#ifdef HW_RVL
  while (WPAD_ButtonsHeld (0))
    VIDEO_WaitVSync ();
#endif
  reset_video(0);
  DrawScreen ();

  fgcolour = COLOR_WHITE;
  bgcolour = BMPANE;

  for (i = 0; i < 3; i++)
    gprint ((640 - (strlen (titles[i]) * 16)) >> 1, 232 + (i * 32), titles[i],
      TXT_DOUBLE);

  ShowScreen ();

  while (!quit)
    {
      if (PAD_ButtonsHeld (0) & PAD_BUTTON_A)
  quit = 2;

      else if (PAD_ButtonsHeld (0) & PAD_BUTTON_B)
  quit = 1;
#ifdef HW_RVL
      else if (WPAD_ButtonsHeld (0) & (WPAD_BUTTON_2|WPAD_CLASSIC_BUTTON_A))
  quit = 2;

      else if (WPAD_ButtonsHeld (0) & (WPAD_BUTTON_1|WPAD_CLASSIC_BUTTON_B))
  quit = 1;
#endif
  }

  while (PAD_ButtonsHeld (0))
    VIDEO_WaitVSync ();
#ifdef HW_RVL
  while (WPAD_ButtonsHeld (0))
    VIDEO_WaitVSync ();
#endif

  reset_video(config.Tvmode);

  if (quit == 1)
    return 0;

  return 1;
}


/****************************************************************************
* draw_options
****************************************************************************/
static void
draw_menu(  int currsel , int max , char items[][22] )
{
  int i;
  int j = 158;
  char inverse[34];

  DrawScreen ();  

  for( i = 0; i < max; i++ )
  {
    if ( i == currsel )
    {
      setfgcolour (BMPANE);
      setbgcolour (INVTEXT);
      memset(inverse, 32, 34);
      inverse[32] = 0;
      memcpy(inverse + 6, items[i], strlen(items[i]));
      gprint( 64, j, inverse, TXT_DOUBLE);
      
    }
    else
    {
      setfgcolour (COLOR_WHITE);
      setbgcolour (BMPANE);
      gprint( ( 640 - ( strlen(items[i]) << 4 )) >> 1  ,
        j, items[i], TXT_DOUBLE);
    }
    j += 32;
  }
  
  ShowScreen();
}



static char options[4][22] = {
    "Audio               ",
    "Controls            ",
    "Storage             ",
    "Video               " 
 };



//static int config.Tvmode = 1;


/****************************************************************************
* load_options
****************************************************************************/        
static void
load_options( void )
{
  int currsel = 0;
  int quit = 0;
  int redraw = 1;
  u16 joy;

#ifdef HW_RVL
  u32 wpad;
#endif


  signed char x,y;

  int MAXOPTS = 4;

  
  while ( !quit )
  {
    if ( redraw )
    {
      draw_menu( currsel , MAXOPTS , options);
      redraw = 0;
    }

    joy = PAD_ButtonsDown(0);
    x = PAD_StickX(0);
    y = PAD_StickY(0);

    if ( y > 70 )
      joy |= PAD_BUTTON_UP;
    else
      if ( y < -70 )
        joy |= PAD_BUTTON_DOWN;

#ifdef HW_RVL

    	wpad= WPAD_ButtonsDown (0);
		struct expansion_t ext;
		// Extension Wiimote
		WPAD_Expansion(WPAD_CHAN_0, &ext);

		 switch (ext.type)
		   {
			   case WPAD_EXP_NONE:     
					if (wpad & WPAD_BUTTON_LEFT)                joy |= PAD_BUTTON_DOWN;
					if (wpad & WPAD_BUTTON_RIGHT)          		joy |= PAD_BUTTON_UP;
					if (wpad & WPAD_BUTTON_2)                   joy |= PAD_BUTTON_A;
					if (wpad & WPAD_BUTTON_1)                   joy |= PAD_BUTTON_B;
					break;
				case WPAD_EXP_CLASSIC:  
					if (wpad & WPAD_CLASSIC_BUTTON_UP)     joy |= PAD_BUTTON_UP;
					if (wpad & WPAD_CLASSIC_BUTTON_DOWN)   joy |= PAD_BUTTON_DOWN;    
					if (wpad & WPAD_CLASSIC_BUTTON_A)      joy |= PAD_BUTTON_A;    
					if (wpad & WPAD_CLASSIC_BUTTON_B)      joy |= PAD_BUTTON_B;
					break;
			}

#endif

    if ( joy & PAD_BUTTON_DOWN )
    {
      currsel++;
      if ( currsel >= MAXOPTS )
        currsel = 0;

      redraw = 1;
    }

    if ( joy & PAD_BUTTON_UP )
    {
      currsel--;
      if ( currsel < 0 )
        currsel = 0;

      redraw = 1;
    }

    if ( joy & PAD_BUTTON_A )
    {
      switch ( currsel )
      {      
        case 0: /*** Audio  ***/
		  Menu_Audio();
		  redraw = 1;
          break;		
        case 1: /*** Controls  ***/
		  Menu_Controls();
		  redraw = 1;
          break;
	    case 2: /*** Storage  ***/
		  load_menu_device();
		  redraw = 1;
		  break;
	    case 3: /*** Video  ***/
		  Menu_Video();
		  redraw = 1;
		  break;
      }

    }
    
   if ( joy & PAD_BUTTON_B )  
      quit = 1;


  }


  /*** Clear joy buffer ***/
  while ( PAD_ButtonsHeld(0) )
    VIDEO_WaitVSync();

#ifdef HW_RVL
  while (WPAD_ButtonsHeld(0))
    VIDEO_WaitVSync ();
#endif
}



static char options_audio[6][22] = {
    "Back                ",
    "SFX Volume       1.0",
    "MP3 Volume       1.0", 
    "Low Gain         1.0", 
    "Mid Gain         1.0",
    "High Gain        1.0" 
 };

static float opts[5] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
/****************************************************************************
* AUDIO 
****************************************************************************/        
 void
Menu_Audio( void )
{
  int currsel = 0;
  int quit = 0;
  int redraw = 1;
  u16 joy;
#ifdef HW_RVL
  u32 wpad;
#endif

  char buf[22];
  signed char x,y;

  int MAXOPTS = 6;
  opts[0]=config.SoundVolume;
  opts[1]=config.Mp3Volume;
  opts[2]=config.Mp3Low;
  opts[3]=config.Mp3Mid;
  opts[4]=config.Mp3High;

  sprintf(options_audio[1],"SFX Volume       %1.1f",opts[0]);
  sprintf(options_audio[2],"MP3 Volume       %1.1f",opts[1]);
  sprintf(options_audio[3],"Low Gain         %1.1f",opts[2]);
  sprintf(options_audio[4],"Mid Gain         %1.1f",opts[3]);
  sprintf(options_audio[5],"High Gain        %1.1f",opts[4]);

  while ( !quit )
  {
    if ( redraw )
    {
      draw_menu( currsel , MAXOPTS , options_audio);
      redraw = 0;
    }

    joy = PAD_ButtonsDown(0);
    x = PAD_StickX(0);
    y = PAD_StickY(0);

    if ( y > 70 )
      joy |= PAD_BUTTON_UP;
    else
      if ( y < -70 )
        joy |= PAD_BUTTON_DOWN;

#ifdef HW_RVL

    	wpad= WPAD_ButtonsDown (0);
		struct expansion_t ext;
		// Extension Wiimote
		WPAD_Expansion(WPAD_CHAN_0, &ext);

		 switch (ext.type)
		   {
			   case WPAD_EXP_NONE:     
					if (wpad & WPAD_BUTTON_LEFT)                joy |= PAD_BUTTON_DOWN;
					if (wpad & WPAD_BUTTON_RIGHT)          		joy |= PAD_BUTTON_UP;
					if (wpad & WPAD_BUTTON_2)                   joy |= PAD_BUTTON_A;
					if (wpad & WPAD_BUTTON_1)                   joy |= PAD_BUTTON_B;
					break;
				case WPAD_EXP_CLASSIC:  
					if (wpad & WPAD_CLASSIC_BUTTON_UP)     joy |= PAD_BUTTON_UP;
					if (wpad & WPAD_CLASSIC_BUTTON_DOWN)   joy |= PAD_BUTTON_DOWN;    
					if (wpad & WPAD_CLASSIC_BUTTON_A)      joy |= PAD_BUTTON_A;    
					if (wpad & WPAD_CLASSIC_BUTTON_B)      joy |= PAD_BUTTON_B;
					break;
			}

#endif

    if ( joy & PAD_BUTTON_DOWN )
    {
      currsel++;
      if ( currsel >= MAXOPTS )
        currsel = 0;

      redraw = 1;
    }

    if ( joy & PAD_BUTTON_UP )
    {
      currsel--;
      if ( currsel < 0 )
        currsel = 0;

      redraw = 1;
    }

    if ( joy & PAD_BUTTON_A )
    {
      switch ( currsel )
      {      
        case 0: /***   ***/
		  quit = 1;
          break;		
      default:
          opts[currsel-1] += 0.1f;
          if ( opts[currsel-1] >= 2.0f )
            opts[currsel-1] = 2.0f;

          strcpy(buf, options_audio[currsel]);
          buf[17]=0;
          sprintf(options_audio[currsel],"%s%1.1f", buf, opts[currsel-1]);
          break;
      }
redraw = 1;
    }
    
   if ( joy & PAD_BUTTON_B )
    {
      if ( currsel > 0 )
      {
        opts[currsel-1] -= 0.1f;
        if ( opts[currsel-1] < 0.0f )
          opts[currsel-1] = 0.0f;
        
        strcpy(buf, options_audio[currsel]);
        buf[17]=0;
        sprintf(options_audio[currsel],"%s%1.1f", buf, opts[currsel-1]);
        
        redraw = 1;
      }
      else
      {
        quit = 1;
      }
    }

  }

/*** Update before we go ***/
  mixer_set( opts[0], opts[1], opts[2], opts[3], opts[4]);

  config.SoundVolume = opts[0];
  config.Mp3Volume = opts[1];
  config.Mp3Low = opts[2];
  config.Mp3Mid = opts[3];
  config.Mp3High = opts[4];
  Save_config();

  /*** Clear joy buffer ***/
  while ( PAD_ButtonsHeld(0) )
    VIDEO_WaitVSync();

#ifdef HW_RVL
  while (WPAD_ButtonsHeld(0))
    VIDEO_WaitVSync ();
#endif
}




static char options_controls[5][22] = {
    "Back                ",
    "Wiimote 1           ",
    "Wiimote 2           ",
	"Classic Controller 1",
	"Classic Controller 2" 
    //"Gamecube Pad 1      ",
	//"Gamecube Pad 2      " 
 };
/****************************************************************************
* PADS
****************************************************************************/        
void
Menu_Controls( void )
{
  int currsel = 0;
  int quit = 0;
  int redraw = 1;
  u16 joy;

#ifdef HW_RVL
  u32 wpad;
#endif


  signed char x,y;

  int MAXOPTS = 5;

  
  while ( !quit )
  {
    if ( redraw )
    {
      draw_menu( currsel , MAXOPTS , options_controls);
      redraw = 0;
    }

    joy = PAD_ButtonsDown(0);
    x = PAD_StickX(0);
    y = PAD_StickY(0);

    if ( y > 70 )
      joy |= PAD_BUTTON_UP;
    else
      if ( y < -70 )
        joy |= PAD_BUTTON_DOWN;

#ifdef HW_RVL

    	wpad= WPAD_ButtonsDown (0);
		struct expansion_t ext;
		// Extension Wiimote
		WPAD_Expansion(WPAD_CHAN_0, &ext);

		 switch (ext.type)
		   {
			   case WPAD_EXP_NONE:     
					if (wpad & WPAD_BUTTON_LEFT)                joy |= PAD_BUTTON_DOWN;
					if (wpad & WPAD_BUTTON_RIGHT)          		joy |= PAD_BUTTON_UP;
					if (wpad & WPAD_BUTTON_2)                   joy |= PAD_BUTTON_A;
					if (wpad & WPAD_BUTTON_1)                   joy |= PAD_BUTTON_B;
					break;
				case WPAD_EXP_CLASSIC:  
					if (wpad & WPAD_CLASSIC_BUTTON_UP)     joy |= PAD_BUTTON_UP;
					if (wpad & WPAD_CLASSIC_BUTTON_DOWN)   joy |= PAD_BUTTON_DOWN;    
					if (wpad & WPAD_CLASSIC_BUTTON_A)      joy |= PAD_BUTTON_A;    
					if (wpad & WPAD_CLASSIC_BUTTON_B)      joy |= PAD_BUTTON_B;
					break;
			}

#endif

    if ( joy & PAD_BUTTON_DOWN )
    {
      currsel++;
      if ( currsel >= MAXOPTS )
        currsel = 0;

      redraw = 1;
    }

    if ( joy & PAD_BUTTON_UP )
    {
      currsel--;
      if ( currsel < 0 )
        currsel = 0;

      redraw = 1;
    }

    if ( joy & PAD_BUTTON_A )
    {
      switch ( currsel )
      {      
         case 0: /*** Back  ***/
		  quit = 1;
          break;		
        case 1: /*** Wiimote 1 ***/
		  defbutton = 1;
		  gx_input_Config(0, 1, 6);
		  Save_config();
          break;
	    case 2: /*** Wiimote 2  ***/
		  defbutton = 1;
		  gx_input_Config(1, 1, 6);
		  Save_config();
		  break;
	    case 3: /*** Classic Controller 1  ***/
		  defbutton = 1;
		  gx_input_Config(0, 3, 6);
		  Save_config();
		  break;
	    case 4: /*** Classic Controller 2  ***/
		  defbutton = 1;
		  gx_input_Config(1, 3, 6);
		  Save_config();
		  break;
	    //case 5: /*** Gamecube Pad 1  ***/
		  /*defbutton = 1;
		  gx_input_Config(0, 0, 4);
		  break;*/
	    //case 6: /*** Gamecube Pad 2  ***/
		  /*defbutton = 1;
		  gx_input_Config(1, 0, 4);
		  break;*/
      }
	redraw = 1;
    }
    
   if ( joy & PAD_BUTTON_B )  
      quit = 1;


  }


  /*** Clear joy buffer ***/
  while ( PAD_ButtonsHeld(0) )
    VIDEO_WaitVSync();

#ifdef HW_RVL
  while (WPAD_ButtonsHeld(0))
    VIDEO_WaitVSync ();
#endif
}


static char options_video[3][22] = {
    "TV Mode         AUTO",
	"Filter           OFF",
    "Region              "	
 };
/****************************************************************************
* VIDEO
****************************************************************************/        
void Menu_Video( void )
{
  int currsel = 0;
  int quit = 0;
  int redraw = 1;
  u16 joy;

#ifdef HW_RVL
  u32 wpad;
#endif

  signed char x,y;

  int MAXOPTS = 3;

  if (config.Tvmode == 0) strcpy(options_video[0], "TV Mode         AUTO");	  	  
  if (config.Tvmode == 1) strcpy(options_video[0], "TV Mode   INTERLACED");
  if (config.Tvmode == 2) strcpy(options_video[0], "TV Mode  PROGRESSIVE");

  while ( !quit )
  {
    if ( redraw )
    {
      draw_menu( currsel , MAXOPTS , options_video);
      redraw = 0;
    }

    joy = PAD_ButtonsDown(0);
    x = PAD_StickX(0);
    y = PAD_StickY(0);

    if ( y > 70 )
      joy |= PAD_BUTTON_UP;
    else
      if ( y < -70 )
        joy |= PAD_BUTTON_DOWN;

#ifdef HW_RVL

    	wpad= WPAD_ButtonsDown (0);
		struct expansion_t ext;
		// Extension Wiimote
		WPAD_Expansion(WPAD_CHAN_0, &ext);

		 switch (ext.type)
		   {
			   case WPAD_EXP_NONE:     
					if (wpad & WPAD_BUTTON_LEFT)                joy |= PAD_BUTTON_DOWN;
					if (wpad & WPAD_BUTTON_RIGHT)          		joy |= PAD_BUTTON_UP;
					if (wpad & WPAD_BUTTON_2)                   joy |= PAD_BUTTON_A;
					if (wpad & WPAD_BUTTON_1)                   joy |= PAD_BUTTON_B;
					break;
				case WPAD_EXP_CLASSIC:  
					if (wpad & WPAD_CLASSIC_BUTTON_UP)     joy |= PAD_BUTTON_UP;
					if (wpad & WPAD_CLASSIC_BUTTON_DOWN)   joy |= PAD_BUTTON_DOWN;    
					if (wpad & WPAD_CLASSIC_BUTTON_A)      joy |= PAD_BUTTON_A;    
					if (wpad & WPAD_CLASSIC_BUTTON_B)      joy |= PAD_BUTTON_B;
					break;
			}
#endif

    if ( joy & PAD_BUTTON_DOWN )
    {
      currsel++;
      if ( currsel >= MAXOPTS )
        currsel = 0;

      redraw = 1;
    }

    if ( joy & PAD_BUTTON_UP )
    {
      currsel--;
      if ( currsel < 0 )
        currsel = 0;

      redraw = 1;
    }

    if ( joy & PAD_BUTTON_A )
    {
      switch ( currsel )
      {      
        case 0:
		  config.Tvmode++;
		  if ( config.Tvmode > 2 )
            config.Tvmode = 0;

			switch (config.Tvmode)
			{
			  case 0 : strcpy(options_video[0], "TV Mode         AUTO");
				break;
			  case 1 : strcpy(options_video[0], "TV Mode   INTERLACED");
				break;
			  case 2 : strcpy(options_video[0], "TV Mode  PROGRESSIVE");
				break;
		      default:
              	strcpy(options_video[0], "TV Mode         AUTO");
              	break;
			}
          break;

		  case 1:
		  config.TvFilter++;
		  if ( config.TvFilter > 1 )
            config.TvFilter = 0;

			switch (config.TvFilter)
			{
			  case 0 : strcpy(options_video[1], "Filter           OFF");
				break;
			  case 1 : strcpy(options_video[1], "Filter            ON");
				break;
		      default:
              	strcpy(options_video[1], "Filter           OFF");
              	break;
			}
          break;

        case 2: /*** Region ***/
          neogeo_region++;
          if ( neogeo_region > 2 )
            neogeo_region = 0;
          switch ( neogeo_region )
          {
            case 0: strcpy(options_video[2], "Region         JAPAN");
              break;
            
            case 1: strcpy(options_video[2], "Region           USA");
              break;
            
            default:
              strcpy(options_video[2], "Region        EUROPE");
              break;
          }
          break;
      }
      redraw = 1;
    }
    
      if ( joy & PAD_BUTTON_B )
      quit = 1;
  }

  Save_config();
  /*** Clear joy buffer ***/
  while ( PAD_ButtonsHeld(0) )
    VIDEO_WaitVSync();

#ifdef HW_RVL
  while (WPAD_ButtonsHeld(0))
    VIDEO_WaitVSync ();
#endif
}

/****************************************************************************
* load_main
****************************************************************************/        
int dvd_motor = 0;

int
load_menu( void )
{
  int currsel = 0;
  int quit = 0;
  int redraw = 1;
  u16 joy;

#ifdef HW_RVL
  u32 wpad;
#endif

  int ret = 0;
  signed char x,y;
  char iosVersion[20];

#ifdef HW_RVL
	sprintf(iosVersion, "IOS : %d", IOS_GetVersion());
#endif

  int MAXOPTS = 6;
  char menu[6][22] = {
    "Return to Game  ",
    "Load New Game   ",
    //"Load from SD/USB",
    "Reset Game      ", 
    "Settings        ",     
	"Exit            ",
    "Credits         "};

 reset_video(0);

  while ( !quit )
  {
    if ( redraw )
    {
      draw_menu( currsel , MAXOPTS, menu);
      redraw = 0;
    }

    joy = PAD_ButtonsDown(0);
    x = PAD_StickX(0);
    y = PAD_StickY(0);

    if ( y > 70 )
      joy |= PAD_BUTTON_UP;
    else
      if ( y < -70 )
        joy |= PAD_BUTTON_DOWN;

#ifdef HW_RVL

    wpad= WPAD_ButtonsDown (0);
	struct expansion_t ext;
	// Extension Wiimote
	WPAD_Expansion(WPAD_CHAN_0, &ext);

		 switch (ext.type)
		   {
			   case WPAD_EXP_NONE:     
					if (wpad & WPAD_BUTTON_LEFT)                joy |= PAD_BUTTON_DOWN;
					if (wpad & WPAD_BUTTON_RIGHT)          		joy |= PAD_BUTTON_UP;
					if (wpad & WPAD_BUTTON_2)                   joy |= PAD_BUTTON_A;
					if (wpad & WPAD_BUTTON_1)                   joy |= PAD_BUTTON_B;
					break;
				case WPAD_EXP_CLASSIC: 
					if (wpad & WPAD_CLASSIC_BUTTON_UP)     joy |= PAD_BUTTON_UP;
					if (wpad & WPAD_CLASSIC_BUTTON_DOWN)   joy |= PAD_BUTTON_DOWN;    
					if (wpad & WPAD_CLASSIC_BUTTON_A)      joy |= PAD_BUTTON_A;    
					if (wpad & WPAD_CLASSIC_BUTTON_B)      joy |= PAD_BUTTON_B;
					break;
			}
#endif

    if ( joy & PAD_BUTTON_DOWN )
    {
      currsel++;
      if ( currsel >= MAXOPTS )
        currsel = 0;

      redraw = 1;
    }
    
    if ( joy & PAD_BUTTON_UP )
    {
      currsel--;
      if ( currsel < 0 )
        currsel = 0;

      redraw = 1;
    }
    
    if ( joy & PAD_BUTTON_A )
    {
      switch ( currsel )
      {
        case 0: /*** Return to game ***/
          ret = 0;
          quit = 1;
          break;

        case 1:  /*** Load new game ***/
          ret = quit = 1;
          break;

       // case 2:  /*** Load device menu ***/
		/*  load_menu_device();
		  redraw = 1;
          break;*/
     
        case 2:  /*** Reset game ***/
		  neogeo_reset();
          YM2610_sh_reset();
          ret = 0;
          quit = 1;
          break;
          
        case 3:  /*** Settings ***/
		  load_options();
          redraw = 1;
          break;
         
		case 4:  /*** Exit ***/
          neogeocd_exit();
          break;

        case 5:  /*** Credits ***/         
		  Credits();
          redraw = 1;
          break;
      }
    }

    if ( joy & PAD_BUTTON_B )
    {
      ret = 0;
      quit = 1;
    }
  }

  /*** Clear joy buffer ***/
  while ( PAD_ButtonsHeld(0) )
    VIDEO_WaitVSync();

#ifdef HW_RVL
  while (WPAD_ButtonsHeld(0))
    VIDEO_WaitVSync ();
#endif

  if (ret == 1 && quit == 1)
	reset_video(0);
  else
    reset_video(config.Tvmode);

  return ret;
}


int
load_menu_device( void )
{
  int currsel = 0;
  int quit = 0;
  int redraw = 1;
  u16 joy;

#ifdef HW_RVL
  u32 wpad;
#endif
  
  int ret = 0;
  signed char x,y;
  char iosVersion[20];

#ifdef HW_RVL
	sprintf(iosVersion, "IOS : %d", IOS_GetVersion());
#endif
  
  int MAXOPTS = 4;
  char menu_device[4][22] = {
    "Current Device:   ",
    "Load from SD      ",
  	"Load from USB     ",
	"Back              "
    };

 if (config.StorageDevice == 0)
	strcpy(menu_device[0], "Current Device: SD");
 if (config.StorageDevice == 1)
	strcpy(menu_device[0], "Current Device: USB");

  while ( !quit )
  {
    if ( redraw )
    {
      draw_menu( currsel , 4, menu_device);
      redraw = 0;
    }

    joy = PAD_ButtonsDown(0);
    x = PAD_StickX(0);
    y = PAD_StickY(0);

    if ( y > 70 )
      joy |= PAD_BUTTON_UP;
    else
      if ( y < -70 )
        joy |= PAD_BUTTON_DOWN;

#ifdef HW_RVL

    wpad= WPAD_ButtonsDown (0);
	struct expansion_t ext;
	// Extension Wiimote
	WPAD_Expansion(WPAD_CHAN_0, &ext);

		 switch (ext.type)
		   {
			   case WPAD_EXP_NONE:     
					if (wpad & WPAD_BUTTON_LEFT)                joy |= PAD_BUTTON_DOWN;
					if (wpad & WPAD_BUTTON_RIGHT)          		joy |= PAD_BUTTON_UP;
					if (wpad & WPAD_BUTTON_2)                   joy |= PAD_BUTTON_A;
					if (wpad & WPAD_BUTTON_1)                   joy |= PAD_BUTTON_B;
					break;
				case WPAD_EXP_CLASSIC: 
					if (wpad & WPAD_CLASSIC_BUTTON_UP)     joy |= PAD_BUTTON_UP;
					if (wpad & WPAD_CLASSIC_BUTTON_DOWN)   joy |= PAD_BUTTON_DOWN;    
					if (wpad & WPAD_CLASSIC_BUTTON_A)      joy |= PAD_BUTTON_A;    
					if (wpad & WPAD_CLASSIC_BUTTON_B)      joy |= PAD_BUTTON_B;
					break;
			}
#endif

    if ( joy & PAD_BUTTON_DOWN )
    {
      currsel++;
      if ( currsel >= MAXOPTS )
        currsel = 0;

      redraw = 1;
    }
    
    if ( joy & PAD_BUTTON_UP )
    {
      currsel--;
      if ( currsel < 0 )
        currsel = 0;

      redraw = 1;
    }
    
    if ( joy & PAD_BUTTON_A )
    {
      switch ( currsel )
      {
        case 0: /*** Current Device ***/
         break;
                    
        case 1:  /*** SD ***/
			config.StorageDevice=0;
			strcpy(menu_device[0], "Current Device: SD");
          break;

		case 2:  /*** USB ***/         
			config.StorageDevice=1;
			strcpy(menu_device[0],"Current Device: USB");
          break;
      
		case 3:  /*** Back ***/
          ret = quit = 1;
          break;

      }
	redraw = 1;
    }

    if ( joy & PAD_BUTTON_B )
    {
     // ret = 0;
      quit = 1;
    }
  }

  /*** Clear joy buffer ***/
  while ( PAD_ButtonsHeld(0) )
    VIDEO_WaitVSync();

#ifdef HW_RVL
  while (WPAD_ButtonsHeld(0))
    VIDEO_WaitVSync ();
#endif
Save_config();
  return ret;
}



/****************************************************************************
* bannerscreen
****************************************************************************/
void
bannerscreen (void)
{
  int y, x, j;
  int offset;
  int *bb = (int *) bannerunc;

  whichfb ^= 1;
  offset = (200 * 320) + 40;
  VIDEO_ClearFrameBuffer (vmode, xfb[whichfb], COLOR_BLACK);

  for (y = 0, j = 0; y < banner_HEIGHT; y++)
    {
      for (x = 0; x < (banner_WIDTH >> 1); x++)
        xfb[whichfb][offset + x] = bb[j++];

      offset += 320;
    }

  VIDEO_SetNextFramebuffer (xfb[whichfb]);
  VIDEO_Flush ();
  VIDEO_WaitVSync ();

}
