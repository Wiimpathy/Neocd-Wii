
Neo-CD Redux Wii v0.5 (wiimpathy 21 october 2011)

- fixed artefacts on left screen border.

- Video modes options : interlaced, progressive and filter on/off.

- Configurable buttons.

- Games default folder : neocd.

- Ability to save settings.

- Browse games by page. 


////////////////////////////////////////////////////////////////////////////////////////////////////////////

Neo-CD Redux Wii v0.4 (wiimpathy september 2011)

- fixed all black screens.

- MP3 audio now works.

- some changes in menus.

////////////////////////////////////////////////////////////////////////////////////////////////////////////

Neo-CD Redux Wii v3 (wiimpathy september 2011)

- fixed some black screens. Now almost all games should start. Still some black sreens with some games and gfx bugs.

- fixed classic controller support.


////////////////////////////////////////////////////////////////////////////////////////////////////////////
Neo-CD Redux Wii port v0.2 (Jacobeian - July 2011)

A very basic port, many things not tested, sourcecode provided for you to improve it and fix bugs !!

- libfat compatible

- added internal SD slot support (can be recompiled to use DVD, not tested)

- added Wiimote + Classic Controller support





Requirements:

(1) put NEOGEO CD BIOS at sd:/bios/NeoCD.bin (see .pdf for supported BIOS dump)

(2) create a directory for each game, at the root of SDCARD (ex: sd:/AOF)

(3) put IPL.TXT in game directory (not provided, usually comes with NEOGEO CD dump):
Game cannot run without IPL.TXT (it contains informations about data mapping).
If it is not included in your game copy, google for it !

(4) put all data files (.bin) in game directory
Name of data files must match the ones listed in IPL.TXT file

(5) put all audio files (.MP3 only !) in /mp3 subdirectory (ex: sd:/AOF/mp3/track02.mp3, etc)
Audio files must be renamed if necessary to track02.mp3, track03.mp3, etc


Usage:

Read the original .pdf
