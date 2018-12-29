/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Progress

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


#define kSettingsProgressX						0
#define kSettingsProgressY						1
#define kSettingsProgressWid					2
#define kSettingsProgressHigh					3
	
#define kSettingsProgressBitmapName				4

#define kSettingsProgressImageCount				5
#define kSettingsProgressImageMillisecond		6
#define kSettingsProgressImageLoop				7
#define kSettingsProgressImageLoopBack			8

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Settings
      
======================================================= */

void project_palette_fill_progress(void)
{
	list_palette_set_title(&project_palette,"Project Progress",NULL,NULL,NULL,NULL,NULL);

		// progress
		
	list_palette_add_header(&project_palette,0,"Position");
	list_palette_add_int(&project_palette,kSettingsProgressX,"X",&iface.progress.x,FALSE);
	list_palette_add_int(&project_palette,kSettingsProgressY,"Y",&iface.progress.y,FALSE);
	list_palette_add_int(&project_palette,kSettingsProgressWid,"Width",&iface.progress.wid,FALSE);
	list_palette_add_int(&project_palette,kSettingsProgressHigh,"Height",&iface.progress.high,FALSE);
	
		// settings

	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_picker_file(&project_palette,kSettingsProgressBitmapName,list_button_none,0,"Bitmap","Bitmaps/Interface","png","",iface.progress.bitmap_name,FALSE);

		// animation

	list_palette_add_header(&project_palette,0,"Animation");
	list_palette_add_int(&project_palette,kSettingsProgressImageCount,"Count",&iface.progress.animate.image_count,FALSE);
	list_palette_add_int(&project_palette,kSettingsProgressImageMillisecond,"Display Milliseconds",&iface.progress.animate.msec,FALSE);
	list_palette_add_checkbox(&project_palette,kSettingsProgressImageLoop,"Loop",&iface.progress.animate.loop,FALSE);
	list_palette_add_checkbox(&project_palette,kSettingsProgressImageLoopBack,"Loop Back",&iface.progress.animate.loop_back,FALSE);
}

/* =======================================================

      Property Palette Click Progress
      
======================================================= */

void project_palette_click_progress(bool double_click)
{
}

