/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Radar

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

#define kRadarPropertySettingsOn				0
#define kRadarPropertySettingsBackground		1
#define kRadarPropertySettingsDisplayRadius		2
#define kRadarPropertySettingsViewRadius		3
#define kRadarPropertySettingsNoMotionFade		4
#define kRadarPropertySettingsRot				5

#define kRadarPropertyPositionX					7
#define kRadarPropertyPositionY					8

#define kRadarPropertyIconAdd					100
#define kRadarPropertyIconName					1000
#define kRadarPropertyIconDelete				2000

extern iface_type				iface;
extern app_state_type			state;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Radar
      
======================================================= */

void project_palette_fill_radar(void)
{
	int				n;

	list_palette_set_title(&project_palette,"Radar",NULL,NULL,NULL,NULL,NULL);

		// settings

	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_checkbox(&project_palette,kRadarPropertySettingsOn,"On",&iface.radar.on,FALSE);
	list_palette_add_picker_file(&project_palette,kRadarPropertySettingsBackground,list_button_none,0,"Background","Bitmaps/Radar","png","",iface.radar.background_bitmap_name,FALSE);
	list_palette_add_int(&project_palette,kRadarPropertySettingsDisplayRadius,"Display Radius",&iface.radar.display_radius,FALSE);
	list_palette_add_int(&project_palette,kRadarPropertySettingsViewRadius,"View Radius",&iface.radar.view_radius,FALSE);
	list_palette_add_int(&project_palette,kRadarPropertySettingsNoMotionFade,"No Motion Fade Milliseconds",&iface.radar.no_motion_fade,FALSE);
	list_palette_add_checkbox(&project_palette,kRadarPropertySettingsRot,"Rotate",&iface.radar.rot,FALSE);

		// position

	list_palette_add_header(&project_palette,0,"Position");
	list_palette_add_int(&project_palette,kRadarPropertyPositionX,"X",&iface.radar.pnt.x,FALSE);
	list_palette_add_int(&project_palette,kRadarPropertyPositionY,"Y",&iface.radar.pnt.y,FALSE);

		// icons

	list_palette_add_header_button(&project_palette,kRadarPropertyIconAdd,"Icons",list_button_plus);

	list_palette_sort_mark_start(&project_palette,&project_palette.item_pane);
	
	for (n=0;n!=iface.radar.nicon;n++) {
		list_palette_add_string_selectable_button(&project_palette,(kRadarPropertyIconName+n),list_button_minus,(kRadarPropertyIconDelete+n),iface.radar.icons[n].name,(state.proj.cur_radar_icon_idx==n),FALSE);
	}

	list_palette_sort(&project_palette,&project_palette.item_pane);
}

/* =======================================================

      Property Palette Click Radar
      
======================================================= */

void project_palette_click_radar(bool double_click)
{
	int					id,idx,sz;

	id=project_palette.item_pane.click.id;

		// select icon

	if ((id>=kRadarPropertyIconName) && (id<(kRadarPropertyIconName+max_radar_icon))) {
		state.proj.cur_radar_icon_idx=id-kRadarPropertyIconName;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}

		// delete icon

	if ((id>=kRadarPropertyIconDelete) && (id<(kRadarPropertyIconDelete+max_radar_icon))) {
		state.proj.cur_radar_icon_idx=-1;

		idx=id-kRadarPropertyIconDelete;

		sz=(iface.radar.nicon-idx)-1;
		if (sz>0) memmove(&iface.radar.icons[idx],&iface.radar.icons[idx+1],(sz*sizeof(iface_radar_icon_type)));

		iface.radar.nicon--;

		return;
	}

		// add model

	if (id==kRadarPropertyIconAdd) {
		state.proj.cur_radar_icon_idx=-1;

		if (iface.radar.nicon>=max_radar_icon) {
			os_dialog_alert("Add Radar Icon","Reached the maximum number of radar icons");
			return;
		}

		idx=iface.radar.nicon;
		iface.radar.nicon++;

		iface.radar.icons[idx].name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)iface.radar.icons[idx].name,name_str_len,0,0);

		iface.radar.icons[idx].size=5;
		iface.radar.icons[idx].rot=FALSE;
		iface.radar.icons[idx].bitmap_name[0]=0x0;

		state.proj.cur_radar_icon_idx=idx;

		return;
	}

		// regular picks, always
		// disable selection

	if (!double_click) return;

	state.proj.cur_radar_icon_idx=-1;
}

