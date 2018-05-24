#include <ogcsys.h>

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define APPPATH  "/apps/neocd"
#define MAX_KEYS 6

int saveSettings();
int load_settings();
int update_settings();

/****************************************************************************
 * Config Option 
 *
 ****************************************************************************/
typedef struct 
{
// Audio
  float SoundVolume;
  float Mp3Volume;
  float Mp3Low;
  float Mp3Mid;
  float Mp3High;
// Controller
  int PadPort;
  u16 pad_keymap[4][MAX_KEYS];
  #ifdef HW_RVL
  u32 wpad_keymap[4*3][MAX_KEYS];
  #endif
// Storage
  int StorageDevice;
  char StorageFolder[MAXPATHLEN];
// video
  int Tvmode;  
  int TvFilter;

} t_config;

/* Global data */
t_config config;

extern int Load_config ();
extern int Save_config ();
extern void Default_config ();
#endif /* _CONFIG_H_ */
