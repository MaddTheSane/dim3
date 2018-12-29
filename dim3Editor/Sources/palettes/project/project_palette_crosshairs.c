/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Crosshairs

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

#define kCrosshairPropertyAdd					0

#define kCrosshairProperyName					1000
#define kCrosshairProperyDelete					2000

extern iface_type				iface;
extern app_state_type			state;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Crosshairs
      
======================================================= */

void project_palette_fill_crosshairs(void)
{
	int						n;

	list_palette_set_title(&project_palette,"Crosshairs",NULL,NULL,NULL,NULL,NULL);

	list_palette_add_header_button(&project_palette,kCrosshairPropertyAdd,"Crosshairs",list_button_plus);

	list_palette_sort_mark_start(&project_palette,&project_palette.item_pane);
	
	for (n=0;n!=iface.crosshair_list.ncrosshair;n++) {
		list_palette_add_string_selectable_button(&project_palette,(kCrosshairProperyName+n),list_button_minus,(kCrosshairProperyDelete+n),iface.crosshair_list.crosshairs[n].name,(state.proj.cur_crosshair_idx==n),FALSE);
	}

	list_palette_sort(&project_palette,&project_palette.item_pane);
}

/* =======================================================

      Property Palette Click Crosshairs
      
======================================================= */

void project_palette_click_crosshairs(bool double_click)
{
	int				id,idx,sz;

	id=project_palette.item_pane.click.id;

		// crosshair edit
		
	if ((id>=kCrosshairProperyName) && (id<(kCrosshairProperyName+max_iface_crosshair))) {
		state.proj.cur_crosshair_idx=id-kCrosshairProperyName;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}
	
		// crosshair delete
		
	if ((id>=kCrosshairProperyDelete) && (id<(kCrosshairProperyDelete+max_iface_crosshair))) {
		state.proj.cur_crosshair_idx=-1;

		idx=id-kCrosshairProperyDelete;

		sz=(iface.crosshair_list.ncrosshair-idx)-1;
		if (sz>0) memmove(&iface.crosshair_list.crosshairs[idx],&iface.crosshair_list.crosshairs[idx+1],(sz*sizeof(iface_crosshair_type)));

		iface.crosshair_list.ncrosshair--;

		return;
	}
	
		// crosshair add

	if (id==kCrosshairPropertyAdd) {
		state.proj.cur_crosshair_idx=-1;

		if (iface.crosshair_list.ncrosshair>=max_iface_crosshair) {
			os_dialog_alert("Add Crosshair","Reached the maximum number of crosshairs");
			return;
		}

		idx=iface.crosshair_list.ncrosshair;
		iface.crosshair_list.ncrosshair++;
		
		iface.crosshair_list.crosshairs[idx].name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)iface.crosshair_list.crosshairs[idx].name,name_str_len,0,0);
	
		iface.crosshair_list.crosshairs[idx].bitmap_name[0]=0x0;

		state.proj.cur_crosshair_idx=idx;

		return;
	}
}

