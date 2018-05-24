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
 * $LastChangedDate: 2007-03-19 10:55:55 +0000 (Mon, 19 Mar 2007) $
 * $LastChangedRevision: 42 $
 ***/

#ifndef	CDAUDIO_H
#define CDAUDIO_H

/*** NeoCD/SDL CDaudio - modified for MP3 playback ***/

//-- Exported Variables ------------------------------------------------------
extern int cdda_first_drive;
extern int cdda_nb_of_drives;
extern int cdda_current_drive;
extern int cdda_current_track;
extern int cdda_playing;
extern char drive_list[32];
extern int nb_of_drives;
extern int cdda_autoloop;
extern char cddapath[1024];

//-- Exported Functions ------------------------------------------------------
int cdda_init(void);
int cdda_play(int);
void cdda_pause(void);
void cdda_stop(void);
void cdda_resume(void);
void cdda_shutdown(void);
void cdda_loop_check(void);
int cdda_get_disk_info(void);
void cdda_build_drive_list(void);
int cdda_get_volume(void);
void cdda_set_volume(int volume);
void audio_setup(void);

//-- libMP3 -----------------------------------------------------------------
int mp3_decoder(int len, char *outbuffer);

#define MP3PLAYING 1
#define MP3NOTPLAYING 2
#define MP3PAUSED 3

#endif				/* CDAUDIO_H */
