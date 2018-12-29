/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Rings

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

#define kRingPropertyAdd						0

#define kRingProperyName						1000
#define kRingProperyDelete						2000

extern iface_type				iface;
extern app_state_type			state;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Rings
      
======================================================= */

void project_palette_fill_rings(void)
{
	int						n;

	list_palette_set_title(&project_palette,"Rings",NULL,NULL,NULL,NULL,NULL);

	list_palette_add_header_button(&project_palette,kRingPropertyAdd,"Rings",list_button_plus);

	list_palette_sort_mark_start(&project_palette,&project_palette.item_pane);
	
	for (n=0;n!=iface.ring_list.nring;n++) {
		list_palette_add_string_selectable_button(&project_palette,(kRingProperyName+n),list_button_minus,(kRingProperyDelete+n),iface.ring_list.rings[n].name,(state.proj.cur_ring_idx==n),FALSE);
	}

	list_palette_sort(&project_palette,&project_palette.item_pane);
}

/* =======================================================

      Property Palette Click Rings
      
======================================================= */

void project_palette_click_rings(bool double_click)
{
	int					id,idx,sz;

	id=project_palette.item_pane.click.id;

		// ring edit
		
	if ((id>=kRingProperyName) && (id<(kRingProperyName+max_iface_ring))) {
		state.proj.cur_ring_idx=id-kRingProperyName;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}
	
		// ring delete
		
	if ((id>=kRingProperyDelete) && (id<(kRingProperyDelete+max_iface_ring))) {
		state.proj.cur_ring_idx=-1;

		idx=id-kRingProperyDelete;

		sz=(iface.ring_list.nring-idx)-1;
		if (sz>0) memmove(&iface.ring_list.rings[idx],&iface.ring_list.rings[idx+1],(sz*sizeof(iface_ring_type)));

		iface.ring_list.nring--;

		return;
	}
	
		// ring add

	if (id==kRingPropertyAdd) {
		state.proj.cur_ring_idx=-1;

		if (iface.ring_list.nring>=max_iface_ring) {
			os_dialog_alert("Add Ring","Reached the maximum number of rings");
			return;
		}

		idx=iface.ring_list.nring;
		iface.ring_list.nring++;
		
		iface.ring_list.rings[idx].name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)iface.ring_list.rings[idx].name,name_str_len,0,0);
	
		iface.ring_list.rings[idx].life_msec=1000;
		iface.ring_list.rings[idx].start_outer_size=2000;
		iface.ring_list.rings[idx].end_outer_size=2000;
		iface.ring_list.rings[idx].start_inner_size=1000;
		iface.ring_list.rings[idx].end_inner_size=1000;

		iface.ring_list.rings[idx].start_alpha=1.0f;
		iface.ring_list.rings[idx].end_alpha=1.0f;

		iface.ring_list.rings[idx].bitmap_name[0]=0x0;

		iface.ring_list.rings[idx].blend_add=TRUE;

		iface.ring_list.rings[idx].start_color.r=1.0f;
		iface.ring_list.rings[idx].start_color.g=1.0f;
		iface.ring_list.rings[idx].start_color.b=1.0f;
		iface.ring_list.rings[idx].end_color.r=1.0f;
		iface.ring_list.rings[idx].end_color.g=1.0f;
		iface.ring_list.rings[idx].end_color.b=1.0f;

		iface.ring_list.rings[idx].ang.x=0.0f;
		iface.ring_list.rings[idx].ang.y=0.0f;
		iface.ring_list.rings[idx].ang.z=0.0f;

		iface.ring_list.rings[idx].vct.x=100.0f;
		iface.ring_list.rings[idx].vct.y=100.0f;
		iface.ring_list.rings[idx].vct.z=100.0f;

		iface.ring_list.rings[idx].rot.x=0.0f;
		iface.ring_list.rings[idx].rot.y=0.0f;
		iface.ring_list.rings[idx].rot.z=0.0f;

		iface.ring_list.rings[idx].rot_accel.x=0.0f;
		iface.ring_list.rings[idx].rot_accel.y=0.0f;
		iface.ring_list.rings[idx].rot_accel.z=0.0f;

		iface.ring_list.rings[idx].animate.image_count=1;
		iface.ring_list.rings[idx].animate.msec=0;
		iface.ring_list.rings[idx].animate.loop=FALSE;
		iface.ring_list.rings[idx].animate.loop_back=FALSE;

		state.proj.cur_ring_idx=idx;

		return;
	}

}

