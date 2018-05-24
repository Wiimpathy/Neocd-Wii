/****************************************************************************
* NeoCD Redux
*
* GUI File Selector
****************************************************************************/

/*** SVN 
 * $LastChangedDate: 2007-03-16 12:29:33 +0000 (Fri, 16 Mar 2007) $
 * $LastChangedRevision: 40 $
 ***/

#ifndef __NEOGUI__
#define __NEOGUI__

#define TXT_NORMAL 0
#define TXT_INVERSE 1
#define TXT_DOUBLE 2

#define BGCOLOUR 0x74b3745a
#define BGBANNER 0x962a96ca
#define BGPANE 0x63b3635b
#define BMPANE 0xa685a67b
#define INVTEXT 0x628a6277

extern void gprint (int x, int y, char *text, int mode);
extern void ShowScreen (void);
extern void DrawScreen (void);
extern void setbgcolour (u32 colour);
extern void setfgcolour (u32 colour);
extern void WaitButtonA (void);
extern void ActionScreen (char *msg);
extern void InfoScreen (char *msg);
extern void Credits (void);
extern int loadnewgame (void);
extern void bannerscreen (void);
extern int load_menu (void);
extern int load_menu_device (void);
extern void Menu_Audio (void);
extern void Menu_Controls (void);
extern void Menu_Video (void);
extern int dvd_motor;
extern int FatDevice;
#endif
