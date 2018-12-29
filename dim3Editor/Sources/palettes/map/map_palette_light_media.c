/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Map

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

#define kMapPropertyAmbientColor			0
#define kMapPropertyAmbientSound			1
#define kMapPropertyAmbientSoundPitch		2

#define kMapPropertyLightMapQuality			10
#define kMapPropertyLightMapSize			11
#define kMapPropertyLightMapUseNormals		12
#define kMapPropertyLightMapSkipGlows		13
#define kMapPropertyLightMapDiffuseBoost	14

#define kMapPropertyMediaType				20
#define kMapPropertyMediaEventId			21
#define kMapPropertyMediaName				22
#define kMapPropertyMediaTitleSound			23

#define kMapPropertyMusicName				30
#define kMapPropertyMusicFadeTime			31
#define kMapPropertyMusicPreloadName		32

extern map_type					map;
extern iface_type				iface;

extern list_palette_type		map_palette;

char							map_map_light_map_size_list[][name_str_len]={"256","512","1024",""},
								map_map_media_type_list[][name_str_len]={"None","Chooser","Title","Cinema",""},
								map_map_light_map_quality_list[][name_str_len]={"Very Low","Low","Medium","High","Very High",""};

/* =======================================================

      Property Palette Fill Light Media
      
======================================================= */

void map_palette_fill_light_media(void)
{
	int				n;
	char			name[256];

	list_palette_set_title(&map_palette,"Map Light Media Settings",NULL,NULL,NULL,NULL,NULL);

		// ambient

	list_palette_add_header(&map_palette,0,"Map Ambient");
	list_palette_add_pick_color(&map_palette,kMapPropertyAmbientColor,"Color",&map.ambient.light_color,FALSE);
	list_palette_add_picker_list_string(&map_palette,kMapPropertyAmbientSound,"Sound",(char*)iface.sound_list.sounds,iface.sound_list.nsound,sizeof(iface_sound_type),(int)offsetof(iface_sound_type,name),TRUE,TRUE,map.ambient.sound_name,FALSE);
	list_palette_add_float(&map_palette,kMapPropertyAmbientSoundPitch,"Sound Pitch",&map.ambient.sound_pitch,FALSE);

		// light map

	list_palette_add_header(&map_palette,0,"Map Light Map");
	list_palette_add_picker_list_int(&map_palette,kMapPropertyLightMapQuality,"Quality",(char*)map_map_light_map_quality_list,-1,name_str_len,0,FALSE,FALSE,&map.light_map.quality,FALSE);
	list_palette_add_picker_list_int(&map_palette,kMapPropertyLightMapSize,"Texture Size",(char*)map_map_light_map_size_list,-1,name_str_len,0,FALSE,FALSE,&map.light_map.size,FALSE);
	list_palette_add_checkbox(&map_palette,kMapPropertyLightMapUseNormals,"Use Normals (Hard Edges)",&map.light_map.use_normals,FALSE);
	list_palette_add_checkbox(&map_palette,kMapPropertyLightMapSkipGlows,"Skip Glow Mapped Polys",&map.light_map.skip_glows,FALSE);
	list_palette_add_float(&map_palette,kMapPropertyLightMapDiffuseBoost,"Diffuse Boost",&map.light_map.diffuse_boost,FALSE);

		// media

	list_palette_add_header(&map_palette,0,"Map Media");
	list_palette_add_picker_list_int(&map_palette,kMapPropertyMediaType,"Startup Type",(char*)map_map_media_type_list,-1,name_str_len,0,FALSE,FALSE,&map.media.type,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyMediaEventId,"Event Id",&map.media.event_id,FALSE);
	list_palette_add_string(&map_palette,kMapPropertyMediaName,"Startup Name",map.media.name,name_str_len,FALSE);
	list_palette_add_picker_list_string(&map_palette,kMapPropertyMediaTitleSound,"Startup Title Sound",(char*)iface.sound_list.sounds,iface.sound_list.nsound,sizeof(iface_sound_type),(int)offsetof(iface_sound_type,name),TRUE,TRUE,map.media.title_sound_name,FALSE);

		// music

	list_palette_add_header(&map_palette,0,"Map Music");
	list_palette_add_picker_file(&map_palette,kMapPropertyMusicName,list_button_none,0,"Name","Music","mp3","",map.music.name,FALSE);

	list_palette_add_int(&map_palette,kMapPropertyMusicFadeTime,"Fade In Time",&map.music.fade_msec,FALSE);
	for (n=0;n!=max_music_preload;n++) {
		sprintf(name,"Preload Name %d",n);
		list_palette_add_picker_file(&map_palette,(kMapPropertyMusicPreloadName+n),list_button_none,0,name,"Music","mp3","",map.music.preload_name[n],FALSE);
	}
}

/* =======================================================

      Property Palette Click Light Media
      
======================================================= */

void map_palette_click_light_media(bool double_click)
{
}

