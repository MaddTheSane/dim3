/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Ring

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

#define kRingSettingsName						0
#define kRingSettingsBitmapName					1
#define kRingSettingsLife						2

#define kRingRingStartOuterSize					3
#define kRingRingEndOuterSize					4
#define kRingRingStartInnerSize					5
#define kRingRingEndInnerSize					6
#define kRingRingStartAlpha						7
#define kRingRingEndAlpha						8
#define kRingRingStartColor						9
#define kRingRingEndColor						10

#define kRingMotionMove							11
#define kRingMotionRot							12
#define kRingMotionRotAdd						13
#define kRingMotionRotAccel						14

#define kRingOptionBlendAdd						15

#define kRingImageCount							17
#define kRingImageMillisecond					18
#define kRingImageLoop							19
#define kRingImageLoopBack						20

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Ring
      
======================================================= */

void project_palette_fill_ring(int ring_idx)
{
	iface_ring_type			*ring;

	ring=&iface.ring_list.rings[ring_idx];

	list_palette_set_title(&project_palette,"Rings",NULL,"Ring",ring->name,NULL,NULL);

		// settings

	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_string(&project_palette,kRingSettingsName,"Name",ring->name,name_str_len,FALSE);
	list_palette_add_picker_file(&project_palette,kRingSettingsBitmapName,list_button_none,0,"Bitmap","Bitmaps/Rings","png","",ring->bitmap_name,FALSE);
	list_palette_add_int(&project_palette,kRingSettingsLife,"Life Milliseconds",&ring->life_msec,FALSE);

		// ring

	list_palette_add_header(&project_palette,0,"Ring");
	list_palette_add_int(&project_palette,kRingRingStartOuterSize,"Start Outer Size",&ring->start_outer_size,FALSE);
	list_palette_add_int(&project_palette,kRingRingEndOuterSize,"End Outer Size",&ring->end_outer_size,FALSE);
	list_palette_add_int(&project_palette,kRingRingStartInnerSize,"Start Inner Size",&ring->start_inner_size,FALSE);
	list_palette_add_int(&project_palette,kRingRingEndInnerSize,"End Inner Size",&ring->end_inner_size,FALSE);
	list_palette_add_float(&project_palette,kRingRingStartAlpha,"Start Alpha",&ring->start_alpha,FALSE);
	list_palette_add_float(&project_palette,kRingRingEndAlpha,"End Alpha",&ring->end_alpha,FALSE);
	list_palette_add_pick_color(&project_palette,kRingRingStartColor,"Start Color",&ring->start_color,FALSE);
	list_palette_add_pick_color(&project_palette,kRingRingEndColor,"End Color",&ring->end_color,FALSE);

		// motion

	list_palette_add_header(&project_palette,0,"Motion");
	list_palette_add_vector(&project_palette,kRingMotionMove,"Move",&ring->vct,FALSE);
	list_palette_add_angle(&project_palette,kRingMotionRot,"Rotation",&ring->ang,FALSE);
	list_palette_add_angle(&project_palette,kRingMotionRotAdd,"Rotation Add",&ring->rot,FALSE);
	list_palette_add_vector(&project_palette,kRingMotionRotAccel,"Rotation Accel",&ring->rot_accel,FALSE);

		// options

	list_palette_add_header(&project_palette,0,"Options");
	list_palette_add_checkbox(&project_palette,kRingOptionBlendAdd,"Additive Blend",&ring->blend_add,FALSE);

		// animation

	list_palette_add_header(&project_palette,0,"Animation");
	list_palette_add_int(&project_palette,kRingImageCount,"Count",&ring->animate.image_count,FALSE);
	list_palette_add_int(&project_palette,kRingImageMillisecond,"Display Milliseconds",&ring->animate.msec,FALSE);
	list_palette_add_checkbox(&project_palette,kRingImageLoop,"Loop",&ring->animate.loop,FALSE);
	list_palette_add_checkbox(&project_palette,kRingImageLoopBack,"Loop Back",&ring->animate.loop_back,FALSE);
}

/* =======================================================

      Property Palette Click Ring
      
======================================================= */

void project_palette_click_ring(int ring_idx,bool double_click)
{
}

