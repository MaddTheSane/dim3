/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Marks

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

#define kMarkPropertyAdd						0

#define kMarkProperyName						1000
#define kMarkProperyDelete						2000

extern iface_type				iface;
extern app_state_type			state;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Marks
      
======================================================= */

void project_palette_fill_marks(void)
{
	int						n;

	list_palette_set_title(&project_palette,"Marks",NULL,NULL,NULL,NULL,NULL);

	list_palette_add_header_button(&project_palette,kMarkPropertyAdd,"Marks",list_button_plus);

	list_palette_sort_mark_start(&project_palette,&project_palette.item_pane);
	
	for (n=0;n!=iface.mark_list.nmark;n++) {
		list_palette_add_string_selectable_button(&project_palette,(kMarkProperyName+n),list_button_minus,(kMarkProperyDelete+n),iface.mark_list.marks[n].name,(state.proj.cur_mark_idx==n),FALSE);
	}

	list_palette_sort(&project_palette,&project_palette.item_pane);
}

/* =======================================================

      Property Palette Click Marks
      
======================================================= */

void project_palette_click_marks(bool double_click)
{
	int				id,idx,sz;

	id=project_palette.item_pane.click.id;

		// mark edit
		
	if ((id>=kMarkProperyName) && (id<(kMarkProperyName+max_iface_mark))) {
		state.proj.cur_mark_idx=id-kMarkProperyName;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}
	
		// mark delete
		
	if ((id>=kMarkProperyDelete) && (id<(kMarkProperyDelete+max_iface_mark))) {
		state.proj.cur_mark_idx=-1;

		idx=id-kMarkProperyDelete;

		sz=(iface.mark_list.nmark-idx)-1;
		if (sz>0) memmove(&iface.mark_list.marks[idx],&iface.mark_list.marks[idx+1],(sz*sizeof(iface_mark_type)));

		iface.mark_list.nmark--;

		return;
	}
	
		// mark add

	if (id==kMarkPropertyAdd) {
		state.proj.cur_mark_idx=-1;

		if (iface.mark_list.nmark>=max_iface_mark) {
			os_dialog_alert("Add Mark","Reached the maximum number of marks");
			return;
		}

		idx=iface.mark_list.nmark;
		iface.mark_list.nmark++;
		
		iface.mark_list.marks[idx].name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)iface.mark_list.marks[idx].name,name_str_len,0,0);
	
		iface.mark_list.marks[idx].bitmap_name[0]=0x0;

		iface.mark_list.marks[idx].fade_in_msec=1000;
		iface.mark_list.marks[idx].life_msec=10000;
		iface.mark_list.marks[idx].fade_out_msec=1000;

		iface.mark_list.marks[idx].no_rotate=FALSE;
		iface.mark_list.marks[idx].no_transparent=FALSE;
		iface.mark_list.marks[idx].no_opaque=FALSE;
		iface.mark_list.marks[idx].hilite=FALSE;
		iface.mark_list.marks[idx].blend_add=FALSE;

		iface.mark_list.marks[idx].animate.image_count=1;
		iface.mark_list.marks[idx].animate.msec=0;
		iface.mark_list.marks[idx].animate.loop=FALSE;
		iface.mark_list.marks[idx].animate.loop_back=FALSE;

		iface.mark_list.marks[idx].color.r=1.0f;
		iface.mark_list.marks[idx].color.g=1.0f;
		iface.mark_list.marks[idx].color.b=1.0f;

		state.proj.cur_mark_idx=idx;

		return;
	}
}

