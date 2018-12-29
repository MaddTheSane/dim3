/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Radar Icon

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

#define kRadarIconSettingsName					0
#define kRadarIconSettingsBitmapName			1

#define kRadarIconOptionSize					2
#define kRadarIconOptionRotate					3

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Radar Icon
      
======================================================= */

void project_palette_fill_radar_icon(int radar_icon_idx)
{
	iface_radar_icon_type		*icon;

	icon=&iface.radar.icons[radar_icon_idx];

	list_palette_set_title(&project_palette,"Radar",NULL,"Icon",icon->name,NULL,NULL);

		// settings

	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_string(&project_palette,kRadarIconSettingsName,"Name",icon->name,name_str_len,FALSE);
	list_palette_add_picker_file(&project_palette,kRadarIconSettingsBitmapName,list_button_none,0,"Bitmap","Bitmaps/Radar","png","",icon->bitmap_name,FALSE);

		// options

	list_palette_add_header(&project_palette,0,"Options");
	list_palette_add_int(&project_palette,kRadarIconOptionSize,"Size",&icon->size,FALSE);
	list_palette_add_checkbox(&project_palette,kRadarIconOptionRotate,"Rotate",&icon->rot,FALSE);
}

/* =======================================================

      Property Palette Click Radar Icon
      
======================================================= */

void project_palette_click_radar_icon(int radar_icon_idx,bool double_click)
{
}

