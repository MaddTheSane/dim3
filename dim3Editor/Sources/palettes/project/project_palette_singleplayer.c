/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Singleplayer

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

#define kSPPropertySkill						0
#define kSPPropertyMapPick						1
#define kSPPropertyMapPickSSRevealId			2

#define kSPPropertyOptionAdd					10

#define kSPPropertyOptionName					100
#define kSPPropertyOptionDelete					200

extern iface_type				iface;
extern app_state_type			state;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Singleplayer
      
======================================================= */

void project_palette_fill_singleplayer(void)
{
	int					n;

	list_palette_set_title(&project_palette,"Singleplayer",NULL,NULL,NULL,NULL,NULL);

		// settings

	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_checkbox(&project_palette,kSPPropertySkill,"Skill",&iface.singleplayer.skill,FALSE);
	list_palette_add_checkbox(&project_palette,kSPPropertyMapPick,"Map Pick",&iface.singleplayer.map_pick,FALSE);
	list_palette_add_int(&project_palette,kSPPropertyMapPickSSRevealId,"Simple Save Map Pick Reveal ID",&iface.singleplayer.map_pick_ss_reveal_id,FALSE);

		// options

	list_palette_add_header_button(&project_palette,kSPPropertyOptionAdd,"Options",list_button_plus);

	list_palette_sort_mark_start(&project_palette,&project_palette.item_pane);
	
	for (n=0;n!=iface.singleplayer.option_list.noption;n++) {
		list_palette_add_string_selectable_button(&project_palette,(kSPPropertyOptionName+n),list_button_minus,(kSPPropertyOptionDelete+n),iface.singleplayer.option_list.options[n].name,(state.proj.cur_singleplayer_option_idx==n),FALSE);
	}

	list_palette_sort(&project_palette,&project_palette.item_pane);
}

/* =======================================================

      Property Palette Click Singleplayer
      
======================================================= */

void project_palette_click_singleplayer(bool double_click)
{
	int					id,idx,sz;

	id=project_palette.item_pane.click.id;

		// selects

	if ((id>=kSPPropertyOptionName) && (id<(kSPPropertyOptionName+max_net_option))) {
		state.proj.cur_singleplayer_option_idx=id-kSPPropertyOptionName;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}

		// delete

	if ((id>=kSPPropertyOptionDelete) && (id<(kSPPropertyOptionDelete+max_net_option))) {
		state.proj.cur_singleplayer_option_idx=-1;

		idx=id-kSPPropertyOptionDelete;

		sz=(iface.singleplayer.option_list.noption-idx)-1;
		if (sz>0) memmove(&iface.singleplayer.option_list.options[idx],&iface.singleplayer.option_list.options[idx+1],(sz*sizeof(iface_sp_option_type)));

		iface.singleplayer.option_list.noption--;

		return;
	}

		// add

	if (id==kSPPropertyOptionAdd) {
		state.proj.cur_singleplayer_option_idx=-1;

		if (iface.singleplayer.option_list.noption>=max_net_option) {
			os_dialog_alert("Add Singleplayer Option","Reached the maximum number of singleplayer options");
			return;
		}

		idx=iface.singleplayer.option_list.noption;
		iface.singleplayer.option_list.noption++;
		
		iface.singleplayer.option_list.options[idx].name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)iface.singleplayer.option_list.options[idx].name,name_str_len,0,0);

		iface.singleplayer.option_list.options[idx].descript[0]=0x0;

		state.proj.cur_singleplayer_option_idx=idx;

		return;
	}
}

