/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Mark

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

#define kMarkSettingsName						0
#define kMarkSettingsFileName					1

#define kMarkOptionNoRotate						2
#define kMarkOptionNoTransparent				3
#define kMarkOptionNoOpaque						4
#define kMarkOptionHilite						5
#define kMarkOptionBlendAdd						6
#define kMarkOptionColor						7

#define kMarkFadeInTick							9
#define kMarkFadeLifeTick						10
#define kMarkFadeOutTick						11

#define kMarkImageCount							12
#define kMarkImageMillisecond					13
#define kMarkImageLoop							14
#define kMarkImageLoopBack						15

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Mark
      
======================================================= */

void project_palette_fill_mark(int mark_idx)
{
	iface_mark_type			*mark;

	mark=&iface.mark_list.marks[mark_idx];

	list_palette_set_title(&project_palette,"Marks",NULL,"Mark",mark->name,NULL,NULL);

		// settings

	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_string(&project_palette,kMarkSettingsName,"Name",mark->name,name_str_len,FALSE);
	list_palette_add_picker_file(&project_palette,kMarkSettingsFileName,list_button_none,0,"Bitmap","Bitmaps/Marks","png","",mark->bitmap_name,FALSE);

		// options

	list_palette_add_header(&project_palette,0,"Options");
	list_palette_add_checkbox(&project_palette,kMarkOptionNoRotate,"No Rotate",&mark->no_rotate,FALSE);
	list_palette_add_checkbox(&project_palette,kMarkOptionNoTransparent,"Ignore Transaparent Polys",&mark->no_transparent,FALSE);
	list_palette_add_checkbox(&project_palette,kMarkOptionNoOpaque,"Ignore Opaque Polys",&mark->no_opaque,FALSE);
	list_palette_add_checkbox(&project_palette,kMarkOptionHilite,"High Light",&mark->hilite,FALSE);
	list_palette_add_checkbox(&project_palette,kMarkOptionBlendAdd,"Additive Blending",&mark->blend_add,FALSE);
	list_palette_add_pick_color(&project_palette,kMarkOptionColor,"Color",&mark->color,FALSE);

		// fade

	list_palette_add_header(&project_palette,0,"Fade");
	list_palette_add_int(&project_palette,kMarkFadeInTick,"Fade In Milliseconds",&mark->fade_in_msec,FALSE);
	list_palette_add_int(&project_palette,kMarkFadeLifeTick,"Life Milliseconds",&mark->life_msec,FALSE);
	list_palette_add_int(&project_palette,kMarkFadeOutTick,"Fade Out Milliseconds",&mark->fade_out_msec,FALSE);

		// animation

	list_palette_add_header(&project_palette,0,"Animation");
	list_palette_add_int(&project_palette,kMarkImageCount,"Count",&mark->animate.image_count,FALSE);
	list_palette_add_int(&project_palette,kMarkImageMillisecond,"Display Milliseconds",&mark->animate.msec,FALSE);
	list_palette_add_checkbox(&project_palette,kMarkImageLoop,"Loop",&mark->animate.loop,FALSE);
	list_palette_add_checkbox(&project_palette,kMarkImageLoopBack,"Loop Back",&mark->animate.loop_back,FALSE);
}

/* =======================================================

      Property Palette Click Mark
      
======================================================= */

void project_palette_click_mark(int mark_idx,bool double_click)
{
}

