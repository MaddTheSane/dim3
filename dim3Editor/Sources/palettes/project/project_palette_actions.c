/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Actions

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

#define kActionPropertyAdd						0
#define kActionProperyName						1000

extern iface_type				iface;
extern app_state_type			state;
extern list_palette_type		project_palette;

char							control_name_str[][32]=control_names;

/* =======================================================

      Property Palette Fill Actions
      
======================================================= */

void project_palette_fill_actions(void)
{
	int						n;

	list_palette_set_title(&project_palette,"Control Actions",NULL,NULL,NULL,NULL,NULL);

	list_palette_add_header(&project_palette,0,"Control Actions");
	
	for (n=0;n!=ncontrol;n++) {
		list_palette_add_string_selectable(&project_palette,(kActionProperyName+n),control_name_str[n],NULL,-1,(state.proj.cur_action_idx==n),FALSE);
	}
}

/* =======================================================

      Property Palette Click Actions
      
======================================================= */

void project_palette_click_actions(bool double_click)
{
		// edit action
		
	state.proj.cur_action_idx=project_palette.item_pane.click.id-kActionProperyName;
	if (double_click) list_palette_set_level(&project_palette,2);
}

