/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Crosshair

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

#define kCrosshairSettingsName					0
#define kCrosshairSettingsFileName				1

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Crosshair
      
======================================================= */

void project_palette_fill_crosshair(int crosshair_idx)
{
	iface_crosshair_type		*crosshair;

	crosshair=&iface.crosshair_list.crosshairs[crosshair_idx];

	list_palette_set_title(&project_palette,"Crosshairs",NULL,"Crosshair",crosshair->name,NULL,NULL);

		// settings

	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_string(&project_palette,kCrosshairSettingsName,"Name",crosshair->name,name_str_len,FALSE);
	list_palette_add_picker_file(&project_palette,kCrosshairSettingsFileName,list_button_none,0,"Bitmap","Bitmaps/Crosshairs","png","",crosshair->bitmap_name,FALSE);
}

/* =======================================================

      Property Palette Click Crosshair
      
======================================================= */

void project_palette_click_crosshair(int crosshair_idx,bool double_click)
{
}

