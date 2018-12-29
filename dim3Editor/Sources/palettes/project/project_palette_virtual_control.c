/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Virtual Control

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

#define kVirutalControlStick			100
#define kVirutalControlButton			200

extern iface_type				iface;
extern app_state_type			state;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Virtual Control
      
======================================================= */

void project_palette_fill_virtual_control(void)
{
	int							n;
	char						str[256];
	
	list_palette_set_title(&project_palette,"Virtual Controls",NULL,NULL,NULL,NULL,NULL);
	
		// sticks

	list_palette_add_header(&project_palette,0,"Sticks");

	for (n=0;n!=max_virtual_stick;n++) {
		sprintf(str,"Stick %d",n);
		list_palette_add_string_selectable(&project_palette,(kVirutalControlStick+n),str,NULL,-1,(state.proj.cur_virtual_control_stick_idx==n),FALSE);
	}

		// buttons

	list_palette_add_header(&project_palette,0,"Buttons");

	for (n=0;n!=max_virtual_button;n++) {
		sprintf(str,"Button %d",n);
		list_palette_add_string_selectable(&project_palette,(kVirutalControlButton+n),str,NULL,-1,(state.proj.cur_virtual_control_button_idx==n),FALSE);
	}
}

/* =======================================================

      Property Palette Click Virtual Control
      
======================================================= */

void project_palette_click_virtual_control(bool double_click)
{
	int				id;

	id=project_palette.item_pane.click.id;

		// select sticks

	if ((id>=kVirutalControlStick) && (id<(kVirutalControlStick+max_virtual_stick))) {
		state.proj.cur_virtual_control_stick_idx=id-kVirutalControlStick;
		state.proj.cur_virtual_control_button_idx=-1;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}

		// select buttons

	if ((id>=kVirutalControlButton) && (id<(kVirutalControlButton+max_virtual_button))) {
		state.proj.cur_virtual_control_button_idx=id-kVirutalControlButton;
		state.proj.cur_virtual_control_stick_idx=-1;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}
}

