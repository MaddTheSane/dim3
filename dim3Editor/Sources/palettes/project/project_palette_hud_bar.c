/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette HUD Bar

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

#define kBarSettingsName						0
#define kBarSettingsType						1
#define kBarSettingsShow						2

#define kBarPositionX							3
#define kBarPositionY							4
#define kBarPositionWid							5
#define kBarPositionHigh						6

#define kBarFillStartColor						7
#define kBarFillEndColor						8
#define kBarFillAlpha							9

#define kBarSettingsOutline						10
#define kBarOutlineColor						11
#define kBarOutlineAlpha						12

#define kBarSettingsBackground					20
#define kBarBackgroundColor						21
#define kBarBackgroundAlpha						22

extern iface_type				iface;
extern list_palette_type		project_palette;

char							hud_bar_type_str[][32]=bar_type_list_def;

/* =======================================================

      Property Palette Fill HUD Bar
      
======================================================= */

void project_palette_fill_hud_bar(int hud_bar_idx)
{
	iface_bar_type				*bar;

	bar=&iface.bar_list.bars[hud_bar_idx];

	list_palette_set_title(&project_palette,"HUD",NULL,"Bar",bar->name,NULL,NULL);

		// settings

	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_string(&project_palette,kBarSettingsName,"Name",bar->name,name_str_len,FALSE);
	list_palette_add_picker_list_int(&project_palette,kBarSettingsType,"Type",(char*)hud_bar_type_str,-1,name_str_len,0,FALSE,FALSE,&bar->type,FALSE);
	list_palette_add_checkbox(&project_palette,kBarSettingsShow,"Show",&bar->show,FALSE);

		// position

	list_palette_add_header(&project_palette,0,"Position");
	list_palette_add_int(&project_palette,kBarPositionX,"X",&bar->pnt.x,FALSE);
	list_palette_add_int(&project_palette,kBarPositionY,"Y",&bar->pnt.y,FALSE);
	list_palette_add_int(&project_palette,kBarPositionWid,"Width",&bar->size.x,FALSE);
	list_palette_add_int(&project_palette,kBarPositionHigh,"Height",&bar->size.y,FALSE);

		// fill

	list_palette_add_header(&project_palette,0,"Fill");
	list_palette_add_pick_color(&project_palette,kBarFillStartColor,"Start Color",&bar->fill_start_color,FALSE);
	list_palette_add_pick_color(&project_palette,kBarFillEndColor,"End Color",&bar->fill_end_color,FALSE);
	list_palette_add_float(&project_palette,kBarFillAlpha,"Alpha",&bar->fill_alpha,FALSE);

		// outline

	list_palette_add_header(&project_palette,0,"Outline");
	list_palette_add_checkbox(&project_palette,kBarSettingsOutline,"On",&bar->outline,FALSE);
	list_palette_add_pick_color(&project_palette,kBarOutlineColor,"Color",&bar->outline_color,FALSE);
	list_palette_add_float(&project_palette,kBarOutlineAlpha,"Alpha",&bar->outline_alpha,FALSE);

		// background

	list_palette_add_header(&project_palette,0,"Background");
	list_palette_add_checkbox(&project_palette,kBarSettingsBackground,"On",&bar->background,FALSE);
	list_palette_add_pick_color(&project_palette,kBarBackgroundColor,"Color",&bar->background_color,FALSE);
	list_palette_add_float(&project_palette,kBarBackgroundAlpha,"Alpha",&bar->background_alpha,FALSE);
}

/* =======================================================

      Property Palette Click HUD Bar
      
======================================================= */

void project_palette_click_hud_bar(int hud_bar_idx,bool double_click)
{
}

