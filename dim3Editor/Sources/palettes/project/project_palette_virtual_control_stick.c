/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Virtual Control Sticks

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

#define kStickOn								0
#define kStickUseBitmap							1
#define kStickOuterBitmap						2
#define kStickInnerBitmap						3
#define kStickPositionX							4
#define kStickPositionY							5
#define kStickPositionWid						6
#define kStickPositionHigh						7
#define kStickFlipX								8
#define kStickFlipY								9
#define kStickColor								10
#define kStickControl							11

extern iface_type				iface;
extern list_palette_type		project_palette;

extern char						control_name_str[][32];

/* =======================================================

      Property Palette Fill Virtual Control
      
======================================================= */

void project_palette_fill_virtual_control_stick(int virtual_control_stick_idx)
{
	char						str[256];
	iface_virtual_stick_type	*stick;
	
	sprintf(str,"%d",virtual_control_stick_idx);
	list_palette_set_title(&project_palette,"Virtual Control",NULL,"Stick",str,NULL,NULL);

	stick=&iface.virtual_control.sticks[virtual_control_stick_idx];
		
	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_checkbox(&project_palette,kStickOn,"On",&stick->on,FALSE);
	list_palette_add_checkbox(&project_palette,kStickUseBitmap,"Use Bitmap",&stick->use_bitmap,FALSE);
	list_palette_add_picker_file(&project_palette,kStickOuterBitmap,list_button_none,0,"Outer Bitmap","Bitmaps/Virtual","png","",stick->outer_bitmap_name,FALSE);
	list_palette_add_picker_file(&project_palette,kStickInnerBitmap,list_button_none,0,"Inner Bitmap","Bitmaps/Virtual","png","",stick->inner_bitmap_name,FALSE);
	list_palette_add_pick_color(&project_palette,kStickColor,"Color",&stick->color,FALSE);
	list_palette_add_picker_list_int(&project_palette,kStickControl,"Click Action",(char*)control_name_str,-1,32,0,TRUE,FALSE,&stick->click_control_idx,FALSE);

	
	list_palette_add_header(&project_palette,0,"Position");
	list_palette_add_int(&project_palette,kStickPositionX,"X",&stick->pnt.x,FALSE);
	list_palette_add_int(&project_palette,kStickPositionY,"Y",&stick->pnt.y,FALSE);
	list_palette_add_int(&project_palette,kStickPositionWid,"Width",&stick->size.x,FALSE);
	list_palette_add_int(&project_palette,kStickPositionHigh,"Height",&stick->size.y,FALSE);

	list_palette_add_header(&project_palette,0,"Axis");
	list_palette_add_checkbox(&project_palette,kStickFlipX,"Flip X",&stick->flip_x,FALSE);
	list_palette_add_checkbox(&project_palette,kStickFlipY,"Flip Y",&stick->flip_y,FALSE);
}

/* =======================================================

      Property Palette Click Virtual Control
      
======================================================= */

void project_palette_click_virtual_control_stick(int virtual_control_stick_idx,bool double_click)
{
}

