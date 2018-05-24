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

/****************************************************************************
* NeoCD-Redux
*
* GX Video
****************************************************************************/
#ifndef __GXVIDEO__
#define __GXVIDEO__

void StartGX (void);
void update_video (int width, int height, char *vbuffer);
void reset_video (int mode);

#endif
