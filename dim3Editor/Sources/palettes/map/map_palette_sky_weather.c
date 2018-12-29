/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Map Sky Weather

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

#define kMapPropertySkyOn						1
#define kMapPropertySkyType						2
#define kMapPropertySkyRadius					3
#define kMapPropertyDomeY						4
#define kMapPropertyDomeMirror					5
#define kMapPropertyTextureRepeat				6
#define kMapPropertyTextureFill					7
#define kMapPropertyTextureBottomFill			8
#define kMapPropertyTextureNorthFill			9
#define kMapPropertyTextureSouthFill			10
#define kMapPropertyTextureEastFill				11
#define kMapPropertyTextureWestFill				12

#define kMapPropertyBackgroundOn				20
#define kMapPropertyBackgroundFrontFill			21
#define kMapPropertyBackgroundFrontStamp		22
#define kMapPropertyBackgroundFrontShift		23
#define kMapPropertyBackgroundFrontClip			24

#define kMapPropertyBackgroundMiddleFill		30
#define kMapPropertyBackgroundMiddleStamp		31
#define kMapPropertyBackgroundMiddleShift		32
#define kMapPropertyBackgroundMiddleClip		33

#define kMapPropertyBackgroundBackFill			40
#define kMapPropertyBackgroundBackStamp			41
#define kMapPropertyBackgroundBackShift			42
#define kMapPropertyBackgroundBackClip			43

#define kMapPropertyFogOn						50
#define kMapPropertyFogInnerRadius				51
#define kMapPropertyFogOuterRadius				52
#define kMapPropertyFogUseSolidColor			53
#define kMapPropertyFogColor					54
#define kMapPropertyFogTextureIndex				55
#define kMapPropertyFogCount					56
#define kMapPropertyFogHigh						57
#define kMapPropertyFogDrop						58
#define kMapPropertyFogAlpha					59
#define kMapPropertyFogTextureSpeed				60
#define kMapPropertyFogTextureFact				61

#define kMapPropertyRainOn						70
#define kMapPropertyRainDensity					71
#define kMapPropertyRainRadius					72
#define kMapPropertyRainHeight					73
#define kMapPropertyRainSpeed					74
#define kMapPropertyRainWidth					75
#define kMapPropertyRainLength					76
#define kMapPropertyRainAlpha					77
#define kMapPropertyRainStartColor				78
#define kMapPropertyRainEndColor				79
#define kMapPropertyRainSlantAdd				80
#define kMapPropertyRainSlantMaxTime			81
#define kMapPropertyRainSlantChangeTime			82

extern map_type					map;

extern list_palette_type		map_palette;

char							map_map_sky_type_list[][name_str_len]={"Panoramic","Dome Hemisphere","Cube",""};

/* =======================================================

      Property Palette Fill Map Sky Weather
      
======================================================= */

void map_palette_fill_sky_weather(void)
{
	list_palette_set_title(&map_palette,"Map Sky & Weather",NULL,NULL,NULL,NULL,NULL);

		// sky

	list_palette_add_header(&map_palette,0,"Map Sky");
	list_palette_add_checkbox(&map_palette,kMapPropertySkyOn,"On",&map.sky.on,FALSE);
	list_palette_add_picker_list_int(&map_palette,kMapPropertySkyType,"Type",(char*)map_map_sky_type_list,-1,name_str_len,0,FALSE,FALSE,&map.sky.type,FALSE);
	list_palette_add_int(&map_palette,kMapPropertySkyRadius,"Radius",&map.sky.radius,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyDomeY,"Dome Height",&map.sky.dome_y,FALSE);
	list_palette_add_checkbox(&map_palette,kMapPropertyDomeMirror,"Mirror Dome",&map.sky.dome_mirror,FALSE);
	list_palette_add_float(&map_palette,kMapPropertyTextureRepeat,"Texture Repeat",&map.sky.txt_fact,FALSE);
	
	list_palette_add_header(&map_palette,0,"Map Sky Textures");
	list_palette_add_picker_list_int(&map_palette,kMapPropertyTextureFill,"Fill/Cube Top Fill",(char*)map.textures,max_map_texture,sizeof(texture_type),(int)offsetof(texture_type,frames[0].name),TRUE,FALSE,&map.sky.fill,FALSE);
	list_palette_add_picker_list_int(&map_palette,kMapPropertyTextureBottomFill,"Cube Bottom Fill",(char*)map.textures,max_map_texture,sizeof(texture_type),(int)offsetof(texture_type,frames[0].name),TRUE,FALSE,&map.sky.bottom_fill,FALSE);
	list_palette_add_picker_list_int(&map_palette,kMapPropertyTextureNorthFill,"Cube North Fill",(char*)map.textures,max_map_texture,sizeof(texture_type),(int)offsetof(texture_type,frames[0].name),TRUE,FALSE,&map.sky.north_fill,FALSE);
	list_palette_add_picker_list_int(&map_palette,kMapPropertyTextureSouthFill,"Cube South Fill",(char*)map.textures,max_map_texture,sizeof(texture_type),(int)offsetof(texture_type,frames[0].name),TRUE,FALSE,&map.sky.south_fill,FALSE);
	list_palette_add_picker_list_int(&map_palette,kMapPropertyTextureEastFill,"Cube East Fill",(char*)map.textures,max_map_texture,sizeof(texture_type),(int)offsetof(texture_type,frames[0].name),TRUE,FALSE,&map.sky.east_fill,FALSE);
	list_palette_add_picker_list_int(&map_palette,kMapPropertyTextureWestFill,"Cube West Fill",(char*)map.textures,max_map_texture,sizeof(texture_type),(int)offsetof(texture_type,frames[0].name),TRUE,FALSE,&map.sky.west_fill,FALSE);

		// background

	list_palette_add_header(&map_palette,0,"Map Background");
	
	list_palette_add_checkbox(&map_palette,kMapPropertyBackgroundOn,"On",&map.background.on,FALSE);
	list_palette_add_uv(&map_palette,kMapPropertyBackgroundFrontStamp,"Front Stamp",&map.background.front.size,FALSE);
	list_palette_add_uv(&map_palette,kMapPropertyBackgroundFrontShift,"Front Scroll",&map.background.front.scroll_factor,FALSE);
	list_palette_add_uv(&map_palette,kMapPropertyBackgroundFrontClip,"Front Y Clip",&map.background.front.clip,FALSE);

	list_palette_add_picker_list_int(&map_palette,kMapPropertyBackgroundFrontFill,"Front Fill",(char*)map.textures,max_map_texture,sizeof(texture_type),(int)offsetof(texture_type,frames[0].name),TRUE,FALSE,&map.background.front.fill,FALSE);
	list_palette_add_uv(&map_palette,kMapPropertyBackgroundMiddleStamp,"Middle Stamp",&map.background.middle.size,FALSE);
	list_palette_add_uv(&map_palette,kMapPropertyBackgroundMiddleShift,"Middle Scroll",&map.background.middle.scroll_factor,FALSE);
	list_palette_add_uv(&map_palette,kMapPropertyBackgroundMiddleClip,"Middle Y Clip",&map.background.middle.clip,FALSE);

	list_palette_add_picker_list_int(&map_palette,kMapPropertyBackgroundMiddleFill,"Middle Fill",(char*)map.textures,max_map_texture,sizeof(texture_type),(int)offsetof(texture_type,frames[0].name),TRUE,FALSE,&map.background.middle.fill,FALSE);
	list_palette_add_uv(&map_palette,kMapPropertyBackgroundBackStamp,"Back Stamp",&map.background.back.size,FALSE);
	list_palette_add_uv(&map_palette,kMapPropertyBackgroundBackShift,"Back Scroll",&map.background.back.scroll_factor,FALSE);
	list_palette_add_picker_list_int(&map_palette,kMapPropertyBackgroundBackFill,"Back Fill",(char*)map.textures,max_map_texture,sizeof(texture_type),(int)offsetof(texture_type,frames[0].name),TRUE,FALSE,&map.background.back.fill,FALSE);
	list_palette_add_uv(&map_palette,kMapPropertyBackgroundBackClip,"Back Y Clip",&map.background.back.clip,FALSE);

		// fog general

	list_palette_add_header(&map_palette,0,"Map Fog General");
	list_palette_add_checkbox(&map_palette,kMapPropertyFogOn,"On",&map.fog.on,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyFogInnerRadius,"Inner Radius",&map.fog.inner_radius,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyFogOuterRadius,"Outer Radius",&map.fog.outer_radius,FALSE);

		// fog obscure

	list_palette_add_header(&map_palette,0,"Map Fog Obscure");
	list_palette_add_checkbox(&map_palette,kMapPropertyFogUseSolidColor,"Use Obscure (GL) Type",&map.fog.use_solid_color,FALSE);
	list_palette_add_pick_color(&map_palette,kMapPropertyFogColor,"Color",&map.fog.col,FALSE);

		// fog textured

	list_palette_add_header(&map_palette,0,"Map Fog Textured");
	list_palette_add_picker_list_int(&map_palette,kMapPropertyFogTextureIndex,"Fill",(char*)map.textures,max_map_texture,sizeof(texture_type),(int)offsetof(texture_type,frames[0].name),TRUE,FALSE,&map.fog.texture_idx,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyFogCount,"Layer Count",&map.fog.count,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyFogHigh,"Height",&map.fog.high,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyFogDrop,"Y Drop",&map.fog.drop,FALSE);
	list_palette_add_float(&map_palette,kMapPropertyFogAlpha,"Alpha",&map.fog.alpha,FALSE);
	list_palette_add_float(&map_palette,kMapPropertyFogTextureSpeed,"Flow Speed",&map.fog.speed,FALSE);
	list_palette_add_uv(&map_palette,kMapPropertyFogTextureFact,"Texture Stamp",&map.fog.txt_fact,FALSE);

		// rain

	list_palette_add_header(&map_palette,0,"Map Rain General");
	list_palette_add_checkbox(&map_palette,kMapPropertyRainOn,"On",&map.rain.on,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyRainDensity,"Density",&map.rain.density,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyRainRadius,"Radius",&map.rain.radius,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyRainHeight,"Height",&map.rain.height,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyRainSpeed,"Speed",&map.rain.speed,FALSE);

	list_palette_add_header(&map_palette,0,"Map Rain Look");
	list_palette_add_int(&map_palette,kMapPropertyRainWidth,"Line Width",&map.rain.line_width,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyRainLength,"Line Length",&map.rain.line_length,FALSE);
	list_palette_add_float(&map_palette,kMapPropertyRainAlpha,"Line Alpha",&map.rain.alpha,FALSE);
	list_palette_add_pick_color(&map_palette,kMapPropertyRainStartColor,"Line Start Color",&map.rain.start_color,FALSE);
	list_palette_add_pick_color(&map_palette,kMapPropertyRainEndColor,"Line End Color",&map.rain.end_color,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyRainSlantAdd,"Slant Add",&map.rain.slant_add,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyRainSlantMaxTime,"Slant Max Time",&map.rain.slant_time_msec,FALSE);
	list_palette_add_int(&map_palette,kMapPropertyRainSlantChangeTime,"Slant Change Time",&map.rain.slant_change_msec,FALSE);
}

/* =======================================================

      Property Palette Click Map Sky Weather
      
======================================================= */

void map_palette_click_sky_weather(bool double_click)
{
}

