/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Halos

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

#define kHaloPropertyAdd						0

#define kHaloProperyName						1000
#define kHaloProperyDelete						2000

extern iface_type				iface;
extern app_state_type			state;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Halos
      
======================================================= */

void project_palette_fill_halos(void)
{
	int						n;

	list_palette_set_title(&project_palette,"Halos",NULL,NULL,NULL,NULL,NULL);

	list_palette_add_header_button(&project_palette,kHaloPropertyAdd,"Halos",list_button_plus);

	list_palette_sort_mark_start(&project_palette,&project_palette.item_pane);
	
	for (n=0;n!=iface.halo_list.nhalo;n++) {
		list_palette_add_string_selectable_button(&project_palette,(kHaloProperyName+n),list_button_minus,(kHaloProperyDelete+n),iface.halo_list.halos[n].name,(state.proj.cur_halo_idx==n),FALSE);
	}

	list_palette_sort(&project_palette,&project_palette.item_pane);
}

/* =======================================================

      Property Palette Click Halos
      
======================================================= */

void project_palette_click_halos(bool double_click)
{
	int				id,idx,sz;

	id=project_palette.item_pane.click.id;

		// halo edit
		
	if ((id>=kHaloProperyName) && (id<(kHaloProperyName+max_iface_halo))) {
		state.proj.cur_halo_idx=id-kHaloProperyName;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}
	
		// halo delete
		
	if ((id>=kHaloProperyDelete) && (id<(kHaloProperyDelete+max_iface_halo))) {
		state.proj.cur_halo_idx=-1;

		idx=id-kHaloProperyDelete;

		sz=(iface.halo_list.nhalo-idx)-1;
		if (sz>0) memmove(&iface.halo_list.halos[idx],&iface.halo_list.halos[idx+1],(sz*sizeof(iface_halo_type)));

		iface.halo_list.nhalo--;

		return;
	}
	
		// halo add

	if (id==kHaloPropertyAdd) {
		state.proj.cur_halo_idx=-1;

		if (iface.halo_list.nhalo>=max_iface_halo) {
			os_dialog_alert("Add Halo","Reached the maximum number of halos");
			return;
		}

		idx=iface.halo_list.nhalo;
		iface.halo_list.nhalo++;
		
		iface.halo_list.halos[idx].name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)iface.halo_list.halos[idx].name,name_str_len,0,0);
	
		iface.halo_list.halos[idx].bitmap_name[0]=0x0;

		iface.halo_list.halos[idx].min_dist=3000;
		iface.halo_list.halos[idx].max_dist=50000;
		iface.halo_list.halos[idx].min_size=500;
		iface.halo_list.halos[idx].max_size=200;
		iface.halo_list.halos[idx].min_alpha=0.8f;
		iface.halo_list.halos[idx].max_alpha=0.05f;
		iface.halo_list.halos[idx].no_clip_object=FALSE;
		iface.halo_list.halos[idx].no_clip_self=TRUE;

		state.proj.cur_halo_idx=idx;

		return;
	}
}

