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

#include "neocdredux.h"
#include "streams.h"
#include "eq.h"

/**
 * Nintendo Gamecube Audio Interface 
 */
static Uint8 soundbuffer[2][3200] ATTRIBUTE_ALIGN(32);
static int whichab = 0;
static int IsPlaying = 0;
/****************************************************************************
 * AudioSwitchBuffers
 *
 * Genesis Plus only provides sound data on completion of each frame.
 * To try to make the audio less choppy, this function is called from both the
 * DMA completion and update_audio.
 *
 * Testing for data in the buffer ensures that there are no clashes.
 ****************************************************************************/
static void AudioSwitchBuffers(void)
{
    static int len[2] = { 3200, 3200 };

    AUDIO_StopDMA();

    IsPlaying = 1;
    AUDIO_InitDMA((u32) soundbuffer[whichab], len[whichab]);
    DCFlushRange(soundbuffer[whichab], len[whichab]);
    AUDIO_StartDMA();
	
    whichab ^= 1;
    len[whichab] = mixer_getaudio(soundbuffer[whichab], 3200);
}


/****************************************************************************
 * InitGCAudio
 *
 * Stock code to set the DSP at 48Khz
 ****************************************************************************/
void InitGCAudio(void)
{
    AUDIO_Init(NULL);
    AUDIO_SetDSPSampleRate(AI_SAMPLERATE_48KHZ);
    AUDIO_RegisterDMACallback(AudioSwitchBuffers);
    memset(soundbuffer, 0, 3200);
    mixer_init();
}

/****************************************************************************
 * NeoCD Audio Update
 *
 * This is called on each VBL to get the next frame of audio.
 *****************************************************************************/
void update_audio(void)
{
    mixer_update_audio();

    if (IsPlaying == 0) {
	AudioSwitchBuffers();
    }
}
