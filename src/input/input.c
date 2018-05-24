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

#include <math.h>

#ifdef HW_RVL
#include <wiiuse/wpad.h>
#endif

#include "neocdredux.h"
#include "config.h"

#define P1UP    0x00000001
#define P1DOWN  0x00000002
#define P1LEFT  0x00000004
#define P1RIGHT 0x00000008
#define P1A     0x00000010
#define P1B     0x00000020
#define P1C     0x00000040
#define P1D     0x00000080

#define P2UP    0x00000100
#define P2DOWN  0x00000200
#define P2LEFT  0x00000400
#define P2RIGHT 0x00000800
#define P2A     0x00001000
#define P2B     0x00002000
#define P2C     0x00004000
#define P2D     0x00008000

#define P1START 0x00010000
#define P1SEL   0x00020000
#define P2START 0x00040000
#define P2SEL   0x00080000

#define SPECIAL 0x01000000

#define MAX_KEYS 6

static u32 keys = 0;
static int padcal = 80;
int accept_input = 0;


static unsigned int neopadmap[] =
{
  P1UP, P1DOWN, P1LEFT, P1RIGHT, P1A, P1B, P1C, P1D
};

/* default mapping should match buttons original position */
static unsigned short gcpadmap[] =
{
  PAD_BUTTON_UP, PAD_BUTTON_DOWN, PAD_BUTTON_LEFT, PAD_BUTTON_RIGHT,
  PAD_BUTTON_B, PAD_BUTTON_A, PAD_BUTTON_Y, PAD_BUTTON_X
};

static const char *keys_name[MAX_KEYS] =
{
  "Button A",
  "Button B",
  "Button C",
  "Button D",
  "Button START",
  "Button SELECT"
};


int defbutton = 1;

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


/***************************************************************************************/
/*   Gamecube PAD Config Not used                                                      */
/***************************************************************************************/
static void pad_config(int chan, int max_keys)
{
  int i;
  u16 p,key;
  char msg[64];

  /* reset VSYNC callback */
  VIDEO_SetPostRetraceCallback(NULL);
  VIDEO_Flush();

  /* Check if PAD is connected */
  if (!(PAD_ScanPads() & (1<<chan)))
  {
    /* restore inputs update callback */
    VIDEO_SetPostRetraceCallback(pad_update);
    VIDEO_Flush();
    sprintf(msg, "PAD #%d is not connected !", chan+1);
    ActionScreen(msg);
    return;
  }

  /* Configure each keys */
  for (i=0; i<4; i++)
  {
    /* remove any pending keys */
    while (PAD_ButtonsHeld(chan))
    {
      VIDEO_WaitVSync();
      PAD_ScanPads();
    }

    /* wait for user input */
    sprintf(msg,"Press key for %s\n(Z to return)",keys_name[i]);
    InfoScreen(msg);

    key = 0;
    while (!key)
    {
      /* update PAD status */
      VIDEO_WaitVSync();
      PAD_ScanPads();
      p = PAD_ButtonsDown(chan);

      /* find pressed key */
      if (p & PAD_TRIGGER_Z) key = 0xff;
      else if (p & PAD_BUTTON_A) key = PAD_BUTTON_A;
      else if (p & PAD_BUTTON_B) key = PAD_BUTTON_B;
      else if (p & PAD_BUTTON_X) key = PAD_BUTTON_X;
      else if (p & PAD_BUTTON_Y) key = PAD_BUTTON_Y;
      /*else if (p & PAD_TRIGGER_R) key = PAD_TRIGGER_R;
      else if (p & PAD_TRIGGER_L) key = PAD_TRIGGER_L;
      else if (p & PAD_BUTTON_START) key = PAD_BUTTON_START;*/
    }

    /* update key mapping */
    if (key !=0xff) config.pad_keymap[chan][i] = key;
    else break;
  }

  /* remove any pending keys */
  while (PAD_ButtonsHeld(chan))
  {
    VIDEO_WaitVSync();
    PAD_ScanPads();
  }

  /* restore inputs update callback */
  VIDEO_SetPostRetraceCallback(pad_update);
  VIDEO_Flush();
}


/***************************************************************************************/
/*   Wiimotes Classic Controllers Config                                               */
/***************************************************************************************/
#ifdef HW_RVL

static void wpad_config(u8 chan, u8 exp, u8 max_keys)
{
  int i;
  char msg[64];
  u32 key,p = 255;

  /* remove inputs update callback */
  VIDEO_SetPostRetraceCallback(NULL);
  VIDEO_Flush();

  /* Check if device is connected */
  WPAD_Probe(chan, &p);
  if (((exp > WPAD_EXP_NONE) && (p != exp)) || (p == 255))
  {
    /* restore inputs update callback */
    VIDEO_SetPostRetraceCallback(pad_update);
    VIDEO_Flush();

   if (exp == WPAD_EXP_NONE)     sprintf(msg, "WIIMOTE #%d is not connected !", chan+1);
   //if (exp == WPAD_EXP_NUNCHUK)  sprintf(msg, "NUNCHUK #%d is not connected !", chan+1);
   if (exp == WPAD_EXP_CLASSIC)  sprintf(msg, "CLASSIC #%d is not connected !", chan+1);
   ActionScreen(msg);
   return;
  }

  /* loop on each mapped keys */
  for (i=0; i<max_keys; i++)
  {
    /* remove any pending buttons */
    while (WPAD_ButtonsHeld(chan))
    {
      WPAD_ScanPads();
      VIDEO_WaitVSync();
    }

    /* wait for user input */
    sprintf(msg,"Press key for %s\n(HOME to return)",keys_name[i]);
	InfoScreen(msg);

    /* wait for input */
    key = 0;
    while (!key)
    {
      VIDEO_WaitVSync();
      WPAD_ScanPads();
      p = WPAD_ButtonsDown(chan);

      switch (exp)
      {
        /* Wiimote  */
        case WPAD_EXP_NONE:
          if (p & WPAD_BUTTON_HOME) key = 0xff;
          else if (p & WPAD_BUTTON_2) key = WPAD_BUTTON_2;
          else if (p & WPAD_BUTTON_1) key = WPAD_BUTTON_1;
          else if (p & WPAD_BUTTON_B) key = WPAD_BUTTON_B;
          else if (p & WPAD_BUTTON_A) key = WPAD_BUTTON_A;
          else if (p & WPAD_BUTTON_PLUS) key = WPAD_BUTTON_PLUS;
          else if (p & WPAD_BUTTON_MINUS) key = WPAD_BUTTON_MINUS;
          break;
        
        /* Wiimote + Nunchuk Not Used*/
        case WPAD_EXP_NUNCHUK:
          if (p & WPAD_BUTTON_HOME) key = 0xff;
          else if (p & WPAD_BUTTON_2) key = WPAD_BUTTON_2;
          else if (p & WPAD_BUTTON_1) key = WPAD_BUTTON_1;
          else if (p & WPAD_BUTTON_B) key = WPAD_BUTTON_B;
          else if (p & WPAD_BUTTON_A) key = WPAD_BUTTON_A;
          else if (p & WPAD_BUTTON_PLUS) key = WPAD_BUTTON_PLUS;
          else if (p & WPAD_BUTTON_MINUS) key = WPAD_BUTTON_MINUS;
          else if (p & WPAD_NUNCHUK_BUTTON_Z) key = WPAD_NUNCHUK_BUTTON_Z;
          else if (p & WPAD_NUNCHUK_BUTTON_C) key = WPAD_NUNCHUK_BUTTON_C;
          break;

        /* Classic Controller */
        case WPAD_EXP_CLASSIC:
          if (p & WPAD_CLASSIC_BUTTON_HOME) key = 0xff;
          else if (p & WPAD_CLASSIC_BUTTON_X) key = WPAD_CLASSIC_BUTTON_X;
          else if (p & WPAD_CLASSIC_BUTTON_A) key = WPAD_CLASSIC_BUTTON_A;
          else if (p & WPAD_CLASSIC_BUTTON_Y) key = WPAD_CLASSIC_BUTTON_Y;
          else if (p & WPAD_CLASSIC_BUTTON_B) key = WPAD_CLASSIC_BUTTON_B;
          else if (p & WPAD_CLASSIC_BUTTON_ZL) key = WPAD_CLASSIC_BUTTON_ZL;
          else if (p & WPAD_CLASSIC_BUTTON_ZR) key = WPAD_CLASSIC_BUTTON_ZR;
          else if (p & WPAD_CLASSIC_BUTTON_PLUS) key = WPAD_CLASSIC_BUTTON_PLUS;
          else if (p & WPAD_CLASSIC_BUTTON_MINUS) key = WPAD_CLASSIC_BUTTON_MINUS;
          else if (p & WPAD_CLASSIC_BUTTON_FULL_L) key = WPAD_CLASSIC_BUTTON_FULL_L;
          else if (p & WPAD_CLASSIC_BUTTON_FULL_R) key = WPAD_CLASSIC_BUTTON_FULL_R;
          break;

        default:
          key = 0xff;
          break;
      }
    }

    /* update key mapping */
    if (key != 0xff) { config.wpad_keymap[exp + (chan * 3)][i] = key;
}
    else break;
  }

  /* remove any pending buttons */
  while (WPAD_ButtonsHeld(chan))
  {
    WPAD_ScanPads();
    VIDEO_WaitVSync();
  }


  /* restore inputs update callback */
  VIDEO_SetPostRetraceCallback(pad_update);
  VIDEO_Flush();
}


void gx_input_Config(u8 chan, u8 type, u8 max)
{
  switch (type)
  {
     case 0:
      pad_config(chan, max);
      break;
    default:
#ifdef HW_RVL
      wpad_config(chan,type-1, max);
#endif
      break;
  }
}

void gx_input_SetDefault(void)
{
  int i;
  //int j;
  //u32 exp;

  /* Default buttons */ 
#ifdef HW_RVL
  for (i=0; i<4; i++)
  {
    /* Wiimote */
    config.wpad_keymap[i*3 + WPAD_EXP_NONE][0] = WPAD_BUTTON_1;
    config.wpad_keymap[i*3 + WPAD_EXP_NONE][1] = WPAD_BUTTON_2;
    config.wpad_keymap[i*3 + WPAD_EXP_NONE][2] = WPAD_BUTTON_A;
    config.wpad_keymap[i*3 + WPAD_EXP_NONE][3]   = WPAD_BUTTON_B;
	config.wpad_keymap[i*3 + WPAD_EXP_NONE][4] = WPAD_BUTTON_MINUS;
    config.wpad_keymap[i*3 + WPAD_EXP_NONE][5]   = WPAD_BUTTON_PLUS;

    /* Classic Controller */
    config.wpad_keymap[i*3 + WPAD_EXP_CLASSIC][0] = WPAD_CLASSIC_BUTTON_B;
    config.wpad_keymap[i*3 + WPAD_EXP_CLASSIC][1] = WPAD_CLASSIC_BUTTON_A;
    config.wpad_keymap[i*3 + WPAD_EXP_CLASSIC][2] = WPAD_CLASSIC_BUTTON_X;
    config.wpad_keymap[i*3 + WPAD_EXP_CLASSIC][3]   = WPAD_CLASSIC_BUTTON_Y;
    config.wpad_keymap[i*3 + WPAD_EXP_CLASSIC][4] = WPAD_CLASSIC_BUTTON_MINUS;
    config.wpad_keymap[i*3 + WPAD_EXP_CLASSIC][5] = WPAD_CLASSIC_BUTTON_PLUS;    
  }
#endif


}
/***************************************************************************************/
/*   Wii WPAD support                                                                  */
/***************************************************************************************/
static u16 
wpad_update( int chan )
{

  unsigned short p = 0;
  u32 wpad= WPAD_ButtonsHeld (chan);
  struct expansion_t ext;
  // Extension Wiimote
  WPAD_Expansion(WPAD_CHAN_0, &ext);

  /* Buttons mapping */
  u32 *wpad_keymap = config.wpad_keymap[ext.type + (3 * chan)];
	   
	   switch (ext.type)
	   {
		   case WPAD_EXP_NONE:       
				/* directions */
				if ( wpad & WPAD_BUTTON_DOWN)           p |= PAD_BUTTON_RIGHT;
				if ( wpad & WPAD_BUTTON_UP)				p |= PAD_BUTTON_LEFT;
				if ( wpad & WPAD_BUTTON_RIGHT)          p |= PAD_BUTTON_UP; 
				if ( wpad & WPAD_BUTTON_LEFT)           p |= PAD_BUTTON_DOWN;
				
				/* buttons */
  			    if (wpad & wpad_keymap[0])				p |= PAD_BUTTON_B;    
			    if (wpad & wpad_keymap[1])				p |= PAD_BUTTON_A;   
			    if (wpad & wpad_keymap[2])				p |= PAD_BUTTON_X;    
			    if (wpad & wpad_keymap[3])				p |= PAD_BUTTON_Y;    
			    if (wpad & wpad_keymap[4])				p |= PAD_TRIGGER_Z;   
			    if (wpad & wpad_keymap[5])				p |= PAD_BUTTON_START;
    
				/* Menu Key */
				if (wpad & WPAD_BUTTON_HOME)            p |= PAD_TRIGGER_R;
				/* Memory Saving Combo */
				if ((wpad & WPAD_BUTTON_PLUS) && (wpad & WPAD_BUTTON_MINUS))  p |= PAD_TRIGGER_L;
				break;
		  
		   case WPAD_EXP_CLASSIC:   
				/* directions */
				if ( wpad & WPAD_CLASSIC_BUTTON_DOWN)           p |= PAD_BUTTON_DOWN;
				if ( wpad & WPAD_CLASSIC_BUTTON_UP)         	p |= PAD_BUTTON_UP;
				if ( wpad & WPAD_CLASSIC_BUTTON_RIGHT)          p |= PAD_BUTTON_RIGHT; 
				if ( wpad & WPAD_CLASSIC_BUTTON_LEFT)           p |= PAD_BUTTON_LEFT;
				/*  buttons */

				if (wpad & wpad_keymap[0])						p |= PAD_BUTTON_B;				
				if (wpad & wpad_keymap[1])						p |= PAD_BUTTON_A;				
				if (wpad & wpad_keymap[2])						p |= PAD_BUTTON_X;				
				if (wpad & wpad_keymap[3])						p |= PAD_BUTTON_Y;				
				if (wpad & wpad_keymap[4])						p |= PAD_TRIGGER_Z;				
				if (wpad & wpad_keymap[5])						p |= PAD_BUTTON_START;
				/* Menu Key */
				if (wpad & WPAD_CLASSIC_BUTTON_HOME)            p |= PAD_TRIGGER_R;
				break;		   
	   }

  return p;
}

static s8 wpad_stickx(int chan, int right)
{
  float mag = 0.0;
  float ang = 0.0;

  WPADData *data = WPAD_Data(chan);
  switch (data->exp.type)
  {
    case WPAD_EXP_NUNCHUK:
      if (right == 0)
      {
        mag = data->exp.nunchuk.js.mag;
        ang = data->exp.nunchuk.js.ang;
      }
      break;

    case WPAD_EXP_CLASSIC:
      if (right == 0)
      {
        mag = data->exp.classic.ljs.mag;
        ang = data->exp.classic.ljs.ang;
      }
      else
      {
        mag = data->exp.classic.rjs.mag;
        ang = data->exp.classic.rjs.ang;
      }
      break;

    default:
      break;
  }

  /* calculate X value (angle need to be converted into radian) */
  if (mag > 1.0) mag = 1.0;
  else if (mag < -1.0) mag = -1.0;
  double val = mag * sin(M_PI * ang/180.0f);
 
  return (s8)(val * 128.0f);
}


static s8 wpad_sticky(int chan, int right)
{
  float mag = 0.0;
  float ang = 0.0;

  WPADData *data = WPAD_Data(chan);

  switch (data->exp.type)
  {
    case WPAD_EXP_NUNCHUK:
      if (right == 0)
      {
        mag = data->exp.nunchuk.js.mag;
        ang = data->exp.nunchuk.js.ang;
      }
      break;

    case WPAD_EXP_CLASSIC:
      if (right == 0)
      {
        mag = data->exp.classic.ljs.mag;
        ang = data->exp.classic.ljs.ang;
      }
      else
      {
        mag = data->exp.classic.rjs.mag;
        ang = data->exp.classic.rjs.ang;
      }
      break;

    default:
      break;
  }

  /* calculate X value (angle need to be converted into radian) */
  if (mag > 1.0) mag = 1.0;
  else if (mag < -1.0) mag = -1.0;
  double val = mag * cos(M_PI * ang/180.0f);
 
  return (s8)(val * 128.0f);
}
#endif


/****************************************************************************
 * DecodeJoy
 ****************************************************************************/
static unsigned int
DecodeJoy (unsigned short p)
{
  unsigned int J = 0;
  int i;
  for (i = 0; i < 8; i++)

    {
      if (p & gcpadmap[i])
        J |= neopadmap[i];
    }
  return J;
}


/****************************************************************************
 * GetAnalog
 ****************************************************************************/
static unsigned int
GetAnalog (int Joy)
{
  float t;
  unsigned int i = 0;
  s8 x = PAD_StickX (Joy);
  s8 y = PAD_StickY (Joy);

#ifdef HW_RVL
  x +=wpad_stickx(Joy,0);
  y +=wpad_sticky(Joy,0);
#endif

  if ((x * x + y * y) > (padcal * padcal))
    {
      if (x > 0 && y == 0)
  i |= P1RIGHT;
      if (x < 0 && y == 0)
  i |= P1LEFT;
      if (x == 0 && y > 0)
  i |= P1UP;
      if (x == 0 && y < 0)
  i |= P1DOWN;

      if (x != 0 && y != 0)
  {

      /*** Recalc left / right ***/
    t = (float) y / x;
    if (t >= -2.41421356237 && t < 2.41421356237)
      {
        if (x >= 0)
    i |= P1RIGHT;
        else
    i |= P1LEFT;
      }

  /*** Recalc up / down ***/
    t = (float) x / y;
    if (t >= -2.41421356237 && t < 2.41421356237)
      {
        if (y >= 0)
    i |= P1UP;
        else
    i |= P1DOWN;
      }
  }
    }

  return i;

}


/****************************************************************************
 * StartSel
 ****************************************************************************/
static unsigned int
startsel (unsigned short p)
{
  int J = 0;
  if (p & PAD_BUTTON_START)
    J |= 1;
  if (p & PAD_TRIGGER_Z)
    J |= 2;
  return J;
}


/****************************************************************************
 * update_input
 ****************************************************************************/
void
update_input (void)
{
  unsigned short p;
  unsigned int t;

  if (!accept_input)
    return;

  /*** Do player one ***/
  p = PAD_ButtonsHeld (0);

#ifdef HW_RVL
  p |= wpad_update(0);
#endif

  if (p & PAD_TRIGGER_L)
    {
      if (mcard_written)
        {
          if (neogeo_set_memorycard ())
            mcard_written = 0;
        }
    }

  if (p & PAD_TRIGGER_R)
    neogeo_new_game ();

  keys = DecodeJoy (p);
  keys |= GetAnalog (0);
  t = startsel (p);
  keys |= (t << 16);

  /*** Do player two ***/
  p = PAD_ButtonsHeld (1);

#ifdef HW_RVL
  p |= wpad_update(1);
#endif

  keys |= (DecodeJoy (p) << 8);
  keys |= (GetAnalog (1) << 8);
  t = startsel (p);
  keys |= (t << 18);
}

/*--------------------------------------------------------------------------*/
unsigned char
read_player1 (void)
{
  return ~keys & 0xff;
}


/*--------------------------------------------------------------------------*/
unsigned char
read_player2 (void)
{
  return ~(keys >> 8) & 0xff;
}


/*--------------------------------------------------------------------------*/
unsigned char
read_pl12_startsel (void)
{
  return ~(keys >> 16) & 0x0f;
}
