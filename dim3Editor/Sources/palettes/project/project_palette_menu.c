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

#define kMenuPropertyName						0

#define kMenuPropertyMenuItemAdd				100

#define kMenuPropertyMenuItemName				1000
#define kMenuPropertyMenuItemDelete				2000

extern iface_type				iface;
extern app_state_type			state;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Menu
      
======================================================= */

void project_palette_fill_menu(int menu_idx)
{
	int					n;
	iface_menu_type		*menu;

	menu=&iface.menu_list.menus[menu_idx];

	list_palette_set_title(&project_palette,"Menus",NULL,"Menu",menu->name,NULL,NULL);

		// settings

	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_string(&project_palette,kMenuPropertyName,"Name",menu->name,name_str_len,FALSE);

		// menu items
		// do NOT sort these as they have a natural order

	list_palette_add_header_button(&project_palette,kMenuPropertyMenuItemAdd,"Items",list_button_plus);
	
	for (n=0;n!=menu->nitem;n++) {
		list_palette_add_string_selectable_button(&project_palette,(kMenuPropertyMenuItemName+n),list_button_minus,(kMenuPropertyMenuItemDelete+n),menu->items[n].data,(state.proj.cur_menu_item_idx==n),FALSE);
	}
}

/* =======================================================

      Property Palette Click Menu
      
======================================================= */

void project_palette_click_menu(int menu_idx,bool double_click)
{
	int					id,idx,sz;
	iface_menu_type		*menu;

	menu=&iface.menu_list.menus[menu_idx];

	id=project_palette.item_pane.click.id;

		// select item

	if ((id>=kMenuPropertyMenuItemName) && (id<(kMenuPropertyMenuItemName+max_menu_item))) {
		state.proj.cur_menu_item_idx=id-kMenuPropertyMenuItemName;
		if (double_click) list_palette_set_level(&project_palette,3);
		return;
	}

		// delete menu

	if ((id>=kMenuPropertyMenuItemDelete) && (id<(kMenuPropertyMenuItemDelete+max_menu_item))) {
		state.proj.cur_menu_item_idx=-1;

		idx=id-kMenuPropertyMenuItemDelete;

		sz=(menu->nitem-idx)-1;
		if (sz>0) memmove(&menu->items[idx],&menu->items[idx+1],(sz*sizeof(iface_menu_item_type)));

		menu->nitem--;

		return;
	}

		// add menu

	if (id==kMenuPropertyMenuItemAdd) {
		state.proj.cur_menu_item_idx=-1;

		if (menu->nitem>=max_menu_item) {
			os_dialog_alert("Add Menu Item","Reached the maximum number of menu items");
			return;
		}

		idx=menu->nitem;
		menu->nitem++;

		dialog_property_string_run(list_string_value_string,(void*)menu->items[idx].data,max_menu_item_data_sz,0,0);

		menu->items[idx].item_id=-1;
		menu->items[idx].multiplayer_disable=FALSE;
		menu->items[idx].quit=FALSE;
		menu->items[idx].sub_menu[0]=0x0;

		state.proj.cur_menu_item_idx=idx;

		return;
	}
}

