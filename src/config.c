/****************************************************************************
*   NeoCD-Wii v0.5
*   NeoGeo CD Emulator
*   2011 wiimpathy (code based essentially based on eke-eke and tantric emulators, thanks to them!)
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ogc/mutex.h>
#include <sys/dir.h>
#include <fat.h>
#include "neocdredux.h"
#include "fileio.h"
#include "input.h"
#ifdef HW_RVL
#include <wiiuse/wpad.h>
#endif
#include <mxml.h>
#include "config.h"
    
static mxml_node_t *xml = NULL;
static mxml_node_t *data = NULL;
static mxml_node_t *section = NULL;
static mxml_node_t *item = NULL;
static mxml_node_t *elem = NULL;

static char temp[200];

int wpad_device[4] = { 0, 3, 2, 5 } ;
char pad_name[6][21] = { "Wiimote 1", "Wiimote 2", "Classic Controller 1", "Classic Controller 2", "Gamecube Pad 1", "Gamecube Pad 2"} ;
int pad=0;
int ButtonNbr=0;
char buttons[6][10]={"a", "b", "c", "d","start","select"}; 



static const char * toStr(int i)
{
	sprintf(temp, "%d", i);
	return temp;
}

static const char * FtoStr(float i)
{
	sprintf(temp, "%1.1f", i);
	return temp;
}


static void createXMLSection(const char * name, const char * description)
{
	section = mxmlNewElement(data, "section");
	mxmlElementSetAttr(section, "name", name);
	mxmlElementSetAttr(section, "description", description);
}

static void createXMLSectionPad(const char * name, const char * description)
{
	/*section = mxmlNewElement(data, "controller");
	mxmlElementSetAttr(section, "name", name);
	mxmlElementSetAttr(section, "description", description);*/

item = mxmlNewElement(section, "controller");
	mxmlElementSetAttr(item, "name", name);
	mxmlElementSetAttr(item, "description", description);
}


static void createXMLSetting(const char * name, const char * description, const char * value)
{
	item = mxmlNewElement(section, "setting");
	mxmlElementSetAttr(item, "name", name);
	mxmlElementSetAttr(item, "value", value);
	mxmlElementSetAttr(item, "description", description);
}

static void createXMLButtons(const char * name, const char * value)
{
	elem = mxmlNewElement(item, "button");
	mxmlElementSetAttr(elem, "name", name);
	mxmlElementSetAttr(elem, "value", value);
}





static const char * XMLSaveCallback(mxml_node_t *node, int where)
{
	const char *name;

	name = node->value.element.name;

	if(where == MXML_WS_BEFORE_CLOSE)
	{
		if(!strcmp(name, "file") || !strcmp(name, "section"))
			return ("\n");
		else if(!strcmp(name, "controller"))
			return ("\n\t");
	}
	if (where == MXML_WS_BEFORE_OPEN)
	{
		if(!strcmp(name, "file"))
			return ("\n");
		else if(!strcmp(name, "section"))
			return ("\n\n");
		else if(!strcmp(name, "setting") || !strcmp(name, "controller"))
			return ("\n\t");
		else if(!strcmp(name, "button"))
			return ("\n\t\t");
	}
	return (NULL);
}

static void loadXMLSettingfolder(char * var, const char * name, int maxsize)
{
	item = mxmlFindElement(xml, xml, "setting", "name", name, MXML_DESCEND);
	if(item)
	{
		const char * tmp = mxmlElementGetAttr(item, "value");
		if(tmp)
			snprintf(var, maxsize, "%s", tmp);
	}
}

static void loadXMLSettingint(int * var, const char * name)
{
	item = mxmlFindElement(xml, xml, "setting", "name", name, MXML_DESCEND);
	if(item)
	{
		const char * tmp = mxmlElementGetAttr(item, "value");
		if(tmp)
			*var = atoi(tmp);
	}
}

static void loadXMLSetting(float * var, const char * name)
{
	item = mxmlFindElement(xml, xml, "setting", "name", name, MXML_DESCEND);
	if(item)
	{
		const char * tmp = mxmlElementGetAttr(item, "value");
		if(tmp)
			*var = atof(tmp);
	}
}



int
Load_config ()
{
char *temp="rrrrrrrrr";
int i;
//int j;
char filepath[MAXPATHLEN];


FILE *file;
sprintf(filepath, "%s/neocd.xml", APPPATH);
file = fopen(filepath, "rb");


 if (file == NULL) 
        {
                fclose(file);
                Default_config();
				Save_config();
        } 
        else 
        {
                fseek (file , 0, SEEK_END);
                long settings_size = ftell (file);
                rewind (file);
                
                if (settings_size > 0) 
                {
                        xml = mxmlLoadFile(NULL, file, MXML_NO_CALLBACK);
                        fclose(file);

                    // Audio Settings    
					loadXMLSetting(&config.SoundVolume, "soundvolume");
					loadXMLSetting(&config.Mp3Volume, "mp3volume");
					loadXMLSetting(&config.Mp3Low, "mp3low");
					loadXMLSetting(&config.Mp3Mid, "mp3mid");
					loadXMLSetting(&config.Mp3High, "mp3high");

					// Controller Settings : Wiimote, Classic
					for(i=0;i<4;i++)
					{
						item = mxmlFindElement(xml, xml, "controller", "name", pad_name[i],MXML_DESCEND);
						if(item)
						{   
							pad=wpad_device[i];                             
							for(ButtonNbr=0;ButtonNbr<6;ButtonNbr++)
							{
		                    	elem = mxmlFindElement(item, xml, "button", "name", buttons[ButtonNbr], MXML_DESCEND);
		                    	if(elem)
		                    	{
		                        sprintf(temp, "%s", mxmlElementGetAttr(elem,"value"));
							    config.wpad_keymap[pad][ButtonNbr]=atoi(temp);
		                    	}
							}
						}
					}


				/*	for(i=0, j=4; i<2; i++, j++)
					{
						item = mxmlFindElement(xml, xml, "controller", "name", pad_name[j],MXML_DESCEND);
						if(item)
						{   
		              
							for(ButtonNbr=0;ButtonNbr<4;ButtonNbr++)
							{
		                    	elem = mxmlFindElement(item, xml, "button", "name", buttons[ButtonNbr], MXML_DESCEND);
		                    	if(elem)
		                    	{
		                        sprintf(temp, "%s", mxmlElementGetAttr(elem,"value"));
							    config.wpad_keymap[i][ButtonNbr]=atoi(temp);
		                    	}
						    }
						}

					}*/

					// Storage Settings    
					loadXMLSettingint(&config.StorageDevice, "storagedevice");
					loadXMLSettingfolder(config.StorageFolder, "storagefolder", sizeof(config.StorageFolder));

					// Video Settings 
					loadXMLSettingint(&config.Tvmode, "videomode");
					loadXMLSettingint(&config.TvFilter, "videofilter");

					mxmlDelete(data);
					mxmlDelete(item); 
 					mxmlDelete(xml);
 
 					return 1;



				}
}

        return 0;

}



int
Save_config ()
{
	int i;
	//int j;
	char filepath[MAXPATHLEN];

	xml = mxmlNewXML("1.0");
	mxmlSetWrapMargin(0); // disable line wrapping
	data = mxmlNewElement(xml, "file");

	// Audio Settings
	createXMLSection("Audio", "Audio Settings");

	createXMLSetting("soundvolume", "Sound Volume", FtoStr(config.SoundVolume));
	createXMLSetting("mp3volume", "MP3 Volume", FtoStr(config.Mp3Volume));    
	createXMLSetting("mp3low", "MP3 Low Gain", FtoStr(config.Mp3Low));
	createXMLSetting("mp3mid", "MP3 Mid Gain", FtoStr(config.Mp3Mid));
	createXMLSetting("mp3high", "MP3 High Gain", FtoStr(config.Mp3High));
    
	// Controller Settings
	createXMLSection("Controller", "Controller Settings");
	createXMLSetting("Controller", "Controller", "wiimotes");

	for(i=0;i<4;i++)
	{
		createXMLSectionPad(pad_name[i], pad_name[i]);
		pad=wpad_device[i];

		for(ButtonNbr=0;ButtonNbr<6;ButtonNbr++)
			createXMLButtons(buttons[ButtonNbr], toStr(config.wpad_keymap[pad][ButtonNbr]));
	}

	/*for(i=0, j=4; i<2; i++, j++)
	{
		createXMLSectionPad(pad_name[j], pad_name[j]);

		for(ButtonNbr=0;ButtonNbr<4;ButtonNbr++)
			createXMLButtons(buttons[ButtonNbr], toStr(config.pad_keymap[i][ButtonNbr]));
	}*/


	// Storage Settings
	createXMLSection("Storage", "Storage Settings");

	createXMLSetting("storagedevice", "Storage Device", toStr(config.StorageDevice));
	createXMLSetting("storagefolder", "Storage Folder", config.StorageFolder);

	// Storage Settings
	createXMLSection("Video", "Video Settings");
	createXMLSetting("videomode", "Video Mode", toStr(config.Tvmode));
	createXMLSetting("videofilter", "Video Filter", toStr(config.TvFilter));

	sprintf(filepath, "%s/neocd.xml", APPPATH);
	FILE *file;
	file = fopen(filepath, "wb");
        
    if(file == NULL) {

    	fclose(file);
    	return 0;
    } 
    else 
    {
		mxmlSaveFile(xml, file, XMLSaveCallback);
		fclose(file);
		mxmlDelete(data);
		mxmlDelete(xml);
		return 1;
    }

    return 0;

}



void Default_config(void)
{
// AUDIO
config.SoundVolume = 1.0f;
config.Mp3Volume = 1.0f;
config.Mp3Low = 1.0f;
config.Mp3Mid = 1.0f;
config.Mp3High = 1.0f;
// CONTROLLER
gx_input_SetDefault();
// STORAGE
config.StorageDevice = 0;
sprintf(config.StorageFolder, "neocd/");
// VIDEO
config.Tvmode = 0;
config.TvFilter = 0;
}
