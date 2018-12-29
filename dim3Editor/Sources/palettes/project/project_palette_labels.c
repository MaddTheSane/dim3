/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Labels

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

#define kLabelPropertyAdd						0

#define kLabelProperyName						1000
#define kLabelProperyDelete						2000

extern iface_type				iface;
extern app_state_type			state;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Labels
      
======================================================= */

void project_palette_fill_labels(void)
{
	int						n;

	list_palette_set_title(&project_palette,"Labels",NULL,NULL,NULL,NULL,NULL);

	list_palette_add_header_button(&project_palette,kLabelPropertyAdd,"Labels",list_button_plus);

	list_palette_sort_mark_start(&project_palette,&project_palette.item_pane);
	
	for (n=0;n!=iface.label_list.nlabel;n++) {
		list_palette_add_string_selectable_button(&project_palette,(kLabelProperyName+n),list_button_minus,(kLabelProperyDelete+n),iface.label_list.labels[n].name,(state.proj.cur_label_idx==n),FALSE);
	}

	list_palette_sort(&project_palette,&project_palette.item_pane);
}

/* =======================================================

      Property Palette Click Labels
      
======================================================= */

void project_palette_click_labels(bool double_click)
{
	int				id,idx,sz;

	id=project_palette.item_pane.click.id;

		// label edit
		
	if ((id>=kLabelProperyName) && (id<(kLabelProperyName+max_iface_label))) {
		state.proj.cur_label_idx=id-kLabelProperyName;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}
	
		// label delete
		
	if ((id>=kLabelProperyDelete) && (id<(kLabelProperyDelete+max_iface_label))) {
		state.proj.cur_mark_idx=-1;

		idx=id-kLabelProperyDelete;

		sz=(iface.label_list.nlabel-idx)-1;
		if (sz>0) memmove(&iface.label_list.labels[idx],&iface.label_list.labels[idx+1],(sz*sizeof(iface_label_type)));

		iface.label_list.nlabel--;

		return;
	}
	
		// label add

	if (id==kLabelPropertyAdd) {
		state.proj.cur_label_idx=-1;

		if (iface.label_list.nlabel>=max_iface_label) {
			os_dialog_alert("Add Label","Reached the maximum number of labels");
			return;
		}

		idx=iface.label_list.nlabel;
		iface.label_list.nlabel++;
		
		iface.label_list.labels[idx].name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)iface.label_list.labels[idx].name,name_str_len,0,0);
	
		iface.label_list.labels[idx].bitmap_name[0]=0x0;

		iface.label_list.labels[idx].sz=50;

		state.proj.cur_label_idx=idx;

		return;
	}
}

