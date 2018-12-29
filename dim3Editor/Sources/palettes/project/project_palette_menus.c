/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Menu

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

#define kMenuPropertyMenuAdd					0

#define kMenuPropertyMenuName					1000
#define kMenuPropertyMenuDelete					2000

extern iface_type				iface;
extern app_state_type			state;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Menus
      
======================================================= */

void project_palette_fill_menus(void)
{
	int				n;
	char			str[256];

	list_palette_set_title(&project_palette,"Menus",NULL,NULL,NULL,NULL,NULL);

	list_palette_add_header_button(&project_palette,kMenuPropertyMenuAdd,"Menus",list_button_plus);

	list_palette_sort_mark_start(&project_palette,&project_palette.item_pane);
	
	for (n=0;n!=iface.menu_list.nmenu;n++) {
		strcpy(str,iface.menu_list.menus[n].name);
		if (n==0) strcat(str," (root)");
		list_palette_add_string_selectable_button(&project_palette,(kMenuPropertyMenuName+n),list_button_minus,(kMenuPropertyMenuDelete+n),str,(state.proj.cur_menu_idx==n),FALSE);
	}

	list_palette_sort(&project_palette,&project_palette.item_pane);
}

/* =======================================================

      Property Palette Click Menu
      
======================================================= */

void project_palette_click_menus(bool double_click)
{
	int					id,idx,sz;

	id=project_palette.item_pane.click.id;

		// select menu

	if ((id>=kMenuPropertyMenuName) && (id<(kMenuPropertyMenuName+max_iface_menu))) {
		state.proj.cur_menu_idx=id-kMenuPropertyMenuName;
		state.proj.cur_menu_item_idx=-1;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}

		// delete item

	if ((id>=kMenuPropertyMenuDelete) && (id<(kMenuPropertyMenuDelete+max_iface_menu))) {
		state.proj.cur_menu_idx=-1;
		state.proj.cur_menu_item_idx=-1;

		idx=id-kMenuPropertyMenuDelete;

		sz=(iface.menu_list.nmenu-idx)-1;
		if (sz>0) memmove(&iface.menu_list.menus[idx],&iface.menu_list.menus[idx+1],(sz*sizeof(iface_menu_type)));

		iface.menu_list.nmenu--;

		return;
	}

		// add item

	if (id==kMenuPropertyMenuAdd) {
		state.proj.cur_menu_idx=-1;
		state.proj.cur_menu_item_idx=-1;

		if (iface.menu_list.nmenu>=max_iface_menu) {
			os_dialog_alert("Add Menu","Reached the maximum number of menus");
			return;
		}

		idx=iface.menu_list.nmenu;
		iface.menu_list.nmenu++;
		
		iface.menu_list.menus[idx].name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)iface.menu_list.menus[idx].name,name_str_len,0,0);
	
		iface.menu_list.menus[idx].nitem=0;

		state.proj.cur_menu_idx=idx;

		return;
	}
}

