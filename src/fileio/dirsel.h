/****************************************************************************
* NeoGeo Directory Selector
*
* As there is no 'ROM' to speak of, use the directory as the starting point
****************************************************************************/

/*** SVN
 * $LastChangedDate: 2007-03-14 00:46:07 +0000 (Wed, 14 Mar 2007) $
 * $LastChangedRevision: 36 $
 ***/

#ifndef __NEODIRSEL__
#define __NEODIRSEL__

extern char basedir[1024];
extern char dirbuffer[0x10000];
extern char scratchdir[1024];

extern void DirSelector (void);

#endif
