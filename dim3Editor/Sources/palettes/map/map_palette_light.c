/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Light

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
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

#define kLightPropertyOn					0
#define kLightPropertyLightMap				1
#define kLightPropertyName					2
#define kLightPropertyType					3
#define kLightPropertyDirection				4
#define kLightPropertyIntensity				5
#define kLightPropertyExponent				6
#define kLightPropertyColor					7
#define kLightPropertyHalo					8

extern map_type					map;
extern iface_type				iface;

extern list_palette_type		map_palette;

char							light_map_type_list[][name_str_len]={"Normal","Blink","Glow","Pulse","Flicker","Failing",""},
								light_map_direction_list[][name_str_len]={"All","-X","+X","-Y","+Y","-Z","+Z",""};

/* =======================================================

      Property Palette Fill Light
      
======================================================= */

void map_palette_fill_light(int light_idx)
{
	map_light_type		*light;

	light=&map.lights[light_idx];

	list_palette_set_title(&map_palette,"Light",light->name,NULL,NULL,NULL,NULL);

	list_palette_add_header(&map_palette,0,"Light Settings");
	list_palette_add_checkbox(&map_palette,kLightPropertyOn,"On",&light->setting.on,FALSE);
	list_palette_add_checkbox(&map_palette,kLightPropertyLightMap,"Used in Light Map",&light->setting.light_map,FALSE);

	list_palette_add_header(&map_palette,0,"Light Naming");
	list_palette_add_string(&map_palette,kLightPropertyName,"Name",light->name,name_str_len,FALSE);

	list_palette_add_header(&map_palette,0,"Light Display");
	list_palette_add_picker_list_int(&map_palette,kLightPropertyType,"Type",(char*)light_map_type_list,-1,name_str_len,0,FALSE,FALSE,&light->setting.type,FALSE);
	list_palette_add_picker_list_int(&map_palette,kLightPropertyDirection,"Direction",(char*)light_map_direction_list,-1,name_str_len,0,FALSE,FALSE,&light->setting.direction,FALSE);
	list_palette_add_int(&map_palette,kLightPropertyIntensity,"Intensity",&light->setting.intensity,FALSE);
	list_palette_add_float(&map_palette,kLightPropertyExponent,"Exponent",&light->setting.exponent,FALSE);
	list_palette_add_pick_color(&map_palette,kLightPropertyColor,"Color",&light->setting.col,FALSE);
	list_palette_add_picker_list_string(&map_palette,kLightPropertyHalo,"Halo",(char*)iface.halo_list.halos,iface.halo_list.nhalo,sizeof(iface_halo_type),(int)offsetof(iface_halo_type,name),TRUE,TRUE,light->setting.halo_name,FALSE);

	list_palette_add_header(&map_palette,0,"Light Info");
	list_palette_add_point(&map_palette,-1,"Position",&light->pnt,TRUE);
}

/* =======================================================

      Property Palette Click Light
      
======================================================= */

void map_palette_click_light(int light_idx,bool double_click)
{
}

