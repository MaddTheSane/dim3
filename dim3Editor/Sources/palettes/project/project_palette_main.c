/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Main

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

#define kShaderPropertyAdd						0

#define kShaderProperyName						1000
#define kShaderProperyDelete					2000

extern iface_type				iface;
extern app_state_type			state;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Main
      
======================================================= */

void project_palette_fill_main(void)
{
	list_palette_set_title(&project_palette,"Project",NULL,NULL,NULL,NULL,NULL);

	list_palette_add_header(&project_palette,item_interface,"General");
	list_palette_add_item(&project_palette,item_interface_project,0,"Project",(state.proj.cur_item==item_interface_project),FALSE);
	list_palette_add_item(&project_palette,item_interface_color,0,"Color",(state.proj.cur_item==item_interface_color),FALSE);
	list_palette_add_item(&project_palette,item_interface_device,0,"Devices",(state.proj.cur_item==item_interface_device),FALSE);

	list_palette_add_header(&project_palette,item_interface,"Titles");
	list_palette_add_item(&project_palette,item_interface_logo,0,"Logo",(state.proj.cur_item==item_interface_logo),FALSE);
	list_palette_add_item(&project_palette,item_interface_title_page,0,"Title Page",(state.proj.cur_item==item_interface_title_page),FALSE);
	list_palette_add_item(&project_palette,item_interface_setup,0,"Setup UI",(state.proj.cur_item==item_interface_setup),FALSE);
	list_palette_add_item(&project_palette,item_interface_progress,0,"Progress",(state.proj.cur_item==item_interface_progress),FALSE);

	list_palette_add_header(&project_palette,item_interface,"Game Play");
	list_palette_add_item(&project_palette,item_interface_singleplayer,0,"Singleplayer",(state.proj.cur_item==item_interface_singleplayer),FALSE);
	list_palette_add_item(&project_palette,item_interface_multiplayer,0,"Multiplayer",(state.proj.cur_item==item_interface_multiplayer),FALSE);

	list_palette_add_header(&project_palette,item_interface,"Displays");
	list_palette_add_item(&project_palette,item_interface_hud,0,"HUD",(state.proj.cur_item==item_interface_hud),FALSE);
	list_palette_add_item(&project_palette,item_interface_radar,0,"Radar",(state.proj.cur_item==item_interface_radar),FALSE);
	list_palette_add_item(&project_palette,item_interface_menu,0,"Menus",(state.proj.cur_item==item_interface_menu),FALSE);
	list_palette_add_item(&project_palette,item_interface_chooser,0,"Choosers",(state.proj.cur_item==item_interface_chooser),FALSE);

	list_palette_add_header(&project_palette,item_interface,"Graphics, Sounds, Effects");
	list_palette_add_item(&project_palette,item_interface_sound,0,"Sounds",(state.proj.cur_item==item_interface_sound),FALSE);
	list_palette_add_item(&project_palette,item_interface_particle,0,"Particles",(state.proj.cur_item==item_interface_particle),FALSE);
	list_palette_add_item(&project_palette,item_interface_ring,0,"Rings",(state.proj.cur_item==item_interface_ring),FALSE);
	list_palette_add_item(&project_palette,item_interface_halo,0,"Halos",(state.proj.cur_item==item_interface_halo),FALSE);
	list_palette_add_item(&project_palette,item_interface_mark,0,"Marks",(state.proj.cur_item==item_interface_mark),FALSE);
	list_palette_add_item(&project_palette,item_interface_label,0,"Labels",(state.proj.cur_item==item_interface_label),FALSE);
	list_palette_add_item(&project_palette,item_interface_crosshair,0,"Crosshairs",(state.proj.cur_item==item_interface_crosshair),FALSE);
	list_palette_add_item(&project_palette,item_interface_shader,0,"Shaders",(state.proj.cur_item==item_interface_shader),FALSE);

	list_palette_add_header(&project_palette,item_interface,"Controls");
	list_palette_add_item(&project_palette,item_interface_action,0,"Control Actions",(state.proj.cur_item==item_interface_action),FALSE);
	list_palette_add_item(&project_palette,item_interface_virtual_control,0,"Virtual Controls",(state.proj.cur_item==item_interface_virtual_control),FALSE);

	list_palette_add_header(&project_palette,item_interface,"Caching");
	list_palette_add_item(&project_palette,item_interface_preload_models,0,"Preload Models",(state.proj.cur_item==item_interface_preload_models),FALSE);

	list_palette_add_header(&project_palette,item_interface,"Editor");
	list_palette_add_item(&project_palette,item_interface_preferences,0,"Preferences",(state.proj.cur_item==item_interface_preferences),FALSE);
}

/* =======================================================

      Property Palette Click Shaders
      
======================================================= */

void project_palette_click_main(bool double_click)
{
		// select item

	state.proj.cur_item=project_palette.item_pane.click.id;

		// if double click, edit

	if (double_click) list_palette_set_level(&project_palette,1);
}

