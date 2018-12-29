/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Project Palette

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

extern iface_type				iface;
extern file_path_setup_type		file_path_setup;
extern app_state_type			state;

extern char						control_name_str[][32];

list_palette_type				project_palette;

/* =======================================================

      Project Palette Setup
      
======================================================= */

void project_palette_initialize(void)
{
	list_palette_list_initialize(&project_palette,"No Properties",FALSE,FALSE,FALSE);

	project_palette.item_pane.click.id=0;
	project_palette.item_pane.click.idx=-1;
	project_palette.item_pane.click.item=NULL;

	state.proj.cur_item=item_interface_project;
	state.proj.cur_device=device_type_pc;
	state.proj.cur_intro_button_idx=-1;
	state.proj.cur_intro_model_idx=-1;
	state.proj.cur_hud_bitmap_idx=-1;
	state.proj.cur_hud_text_idx=-1;
	state.proj.cur_hud_bar_idx=-1;
	state.proj.cur_virtual_control_stick_idx=-1;
	state.proj.cur_virtual_control_button_idx=-1;
	state.proj.cur_radar_icon_idx=-1;
	state.proj.cur_menu_idx=-1;
	state.proj.cur_menu_item_idx=-1;
	state.proj.cur_chooser_idx=-1;
	state.proj.cur_chooser_piece_idx=-1;
	state.proj.cur_singleplayer_option_idx=-1;
	state.proj.cur_multiplayer_character_idx=-1;
	state.proj.cur_multiplayer_game_idx=-1;
	state.proj.cur_multiplayer_option_idx=-1;
	state.proj.cur_sound_idx=-1;
	state.proj.cur_particle_idx=-1;
	state.proj.cur_group_particle_idx=-1;
	state.proj.cur_ring_idx=-1;
	state.proj.cur_halo_idx=-1;
	state.proj.cur_mark_idx=-1;
	state.proj.cur_label_idx=-1;
	state.proj.cur_crosshair_idx=-1;
	state.proj.cur_action_idx=-1;
	state.proj.cur_shader_idx=-1;
}

void project_palette_shutdown(void)
{
	list_palette_list_shutdown(&project_palette);
}

/* =======================================================

      Project Palette Fill
      
======================================================= */

void project_palette_fill_level_0(void)
{
	project_palette_fill_main();
}

void project_palette_fill_level_1(void)
{
	switch (state.proj.cur_item) {

		case item_interface_project:
			project_palette_fill_project();
			return;

		case item_interface_color:
			project_palette_fill_color();
			return;

		case item_interface_device:
			project_palette_fill_devices();
			return;

		case item_interface_logo:
			project_palette_fill_logo();
			return;

		case item_interface_title_page:
			project_palette_fill_title_page();
			return;

		case item_interface_singleplayer:
			project_palette_fill_singleplayer();
			return;

		case item_interface_multiplayer:
			project_palette_fill_multiplayer();
			return;

		case item_interface_setup:
			project_palette_fill_setup();
			return;

		case item_interface_progress:
			project_palette_fill_progress();
			return;

		case item_interface_hud:
			project_palette_fill_hud();
			return;
			
		case item_interface_virtual_control:
			project_palette_fill_virtual_control();
			return;

		case item_interface_radar:
			project_palette_fill_radar();
			return;

		case item_interface_menu:
			project_palette_fill_menus();
			return;

		case item_interface_chooser:
			project_palette_fill_choosers();
			return;

		case item_interface_sound:
			project_palette_fill_sounds();
			return;

		case item_interface_particle:
			project_palette_fill_particles();
			return;

		case item_interface_ring:
			project_palette_fill_rings();
			return;

		case item_interface_halo:
			project_palette_fill_halos();
			return;

		case item_interface_mark:
			project_palette_fill_marks();
			return;
			
		case item_interface_label:
			project_palette_fill_labels();
			return;

		case item_interface_crosshair:
			project_palette_fill_crosshairs();
			return;

		case item_interface_action:
			project_palette_fill_actions();
			return;

		case item_interface_shader:
			project_palette_fill_shaders();
			return;
			
		case item_interface_preload_models:
			project_palette_fill_preload_models();
			return;

		case item_interface_preferences:
			project_palette_fill_preferences();
			return;

	}
}

void project_palette_fill_level_2(void)
{
	switch (state.proj.cur_item) {

		case item_interface_title_page:
			if (state.proj.cur_intro_button_idx!=-1) {
				project_palette_fill_title_page_button(state.proj.cur_intro_button_idx);
				return;
			}
			if (state.proj.cur_intro_model_idx!=-1) {
				project_palette_fill_title_page_model(state.proj.cur_intro_model_idx);
				return;
			}
			break;

		case item_interface_hud:
			if (state.proj.cur_hud_bitmap_idx!=-1) {
				project_palette_fill_hud_bitmap(state.proj.cur_hud_bitmap_idx);
				return;
			}
			if (state.proj.cur_hud_text_idx!=-1) {
				project_palette_fill_hud_text(state.proj.cur_hud_text_idx);
				return;
			}
			if (state.proj.cur_hud_bar_idx!=-1) {
				project_palette_fill_hud_bar(state.proj.cur_hud_bar_idx);
				return;
			}
			break;
			
		case item_interface_virtual_control:
			if (state.proj.cur_virtual_control_stick_idx!=-1) {
				project_palette_fill_virtual_control_stick(state.proj.cur_virtual_control_stick_idx);
				break;
			}
			if (state.proj.cur_virtual_control_button_idx!=-1) {
				project_palette_fill_virtual_control_button(state.proj.cur_virtual_control_button_idx);
				break;
			}
			break;

		case item_interface_radar:
			if (state.proj.cur_radar_icon_idx!=-1) {
				project_palette_fill_radar_icon(state.proj.cur_radar_icon_idx);
				return;
			}
			break;

		case item_interface_singleplayer:
			if (state.proj.cur_singleplayer_option_idx!=-1) {
				project_palette_fill_singleplayer_option(state.proj.cur_singleplayer_option_idx);
				return;
			}
			break;

		case item_interface_multiplayer:
			if (state.proj.cur_multiplayer_character_idx!=-1) {
				project_palette_fill_multiplayer_character(state.proj.cur_multiplayer_character_idx);
				return;
			}
			if (state.proj.cur_multiplayer_game_idx!=-1) {
				project_palette_fill_multiplayer_game(state.proj.cur_multiplayer_game_idx);
				return;
			}
			if (state.proj.cur_multiplayer_option_idx!=-1) {
				project_palette_fill_multiplayer_option(state.proj.cur_multiplayer_option_idx);
				return;
			}
			break;

		case item_interface_menu:
			if (state.proj.cur_menu_idx!=-1) {
				project_palette_fill_menu(state.proj.cur_menu_idx);
				return;
			}
			break;

		case item_interface_chooser:
			if (state.proj.cur_chooser_idx!=-1) {
				project_palette_fill_chooser(state.proj.cur_chooser_idx);
				return;
			}
			break;

		case item_interface_sound:
			if (state.proj.cur_sound_idx!=-1) {
				project_palette_fill_sound(state.proj.cur_sound_idx);
				return;
			}
			break;

		case item_interface_particle:
			if (state.proj.cur_particle_idx!=-1) {
				if (!iface.particle_list.particles[state.proj.cur_particle_idx].group.on) {
					project_palette_fill_particle(state.proj.cur_particle_idx);
				}
				else {
					project_palette_fill_particle_group(state.proj.cur_particle_idx);
				}
				return;
			}
			break;

		case item_interface_ring:
			if (state.proj.cur_ring_idx!=-1) {
				project_palette_fill_ring(state.proj.cur_ring_idx);
				return;
			}
			break;

		case item_interface_halo:
			if (state.proj.cur_halo_idx!=-1) {
				project_palette_fill_halo(state.proj.cur_halo_idx);
				return;
			}
			break;

		case item_interface_mark:
			if (state.proj.cur_mark_idx!=-1) {
				project_palette_fill_mark(state.proj.cur_mark_idx);
				return;
			}
			break;
			
		case item_interface_label:
			if (state.proj.cur_label_idx!=-1) {
				project_palette_fill_label(state.proj.cur_label_idx);
				return;
			}
			break;

		case item_interface_crosshair:
			if (state.proj.cur_crosshair_idx!=-1) {
				project_palette_fill_crosshair(state.proj.cur_crosshair_idx);
				return;
			}
			break;

		case item_interface_action:
			if (state.proj.cur_action_idx!=-1) {
				project_palette_fill_action(state.proj.cur_action_idx);
				return;
			}
			break;

		case item_interface_shader:
			if (state.proj.cur_shader_idx!=-1) {
				project_palette_fill_shader(state.proj.cur_shader_idx);
				return;
			}
			break;

	}
}

void project_palette_fill_level_3(void)
{
	switch (state.proj.cur_item) {

		case item_interface_menu:
			if ((state.proj.cur_menu_idx!=-1) && (state.proj.cur_menu_item_idx!=-1)) {
				project_palette_fill_menu_item(state.proj.cur_menu_idx,state.proj.cur_menu_item_idx);
				return;
			}
			break;

		case item_interface_chooser:
			if ((state.proj.cur_chooser_idx!=-1) && (state.proj.cur_chooser_piece_idx!=-1)) {
				project_palette_fill_chooser_piece(state.proj.cur_chooser_idx,state.proj.cur_chooser_piece_idx);
				return;
			}
			break;

		case item_interface_particle:
			if (state.proj.cur_particle_idx!=-1) {
				if (iface.particle_list.particles[state.proj.cur_particle_idx].group.on) {
					if (state.proj.cur_group_particle_idx!=-1) {
						project_palette_fill_group_particle(state.proj.cur_particle_idx,state.proj.cur_group_particle_idx);
						return;
					}
				}
			}
			break;

	}
}

/* =======================================================

      Property Palette Draw
      
======================================================= */

void project_palette_draw(void)
{
		// delete the properties

	list_palette_delete_all_items(&project_palette);

		// fill by level

	switch (list_palette_get_level(&project_palette)) {
		case 0:
			project_palette_fill_level_0();
			break;
		case 1:
			project_palette_fill_level_1();
			break;
		case 2:
			project_palette_fill_level_2();
			break;
		case 3:
			project_palette_fill_level_3();
			break;
	}

	list_palette_draw(&project_palette);
}

/* =======================================================

      Property Palette Scrolling
      
======================================================= */

void project_palette_reset(void)
{
	project_palette.item_pane.scroll_offset=0;
}

void project_palette_scroll_wheel(d3pnt *pnt,int move)
{
	if (list_palette_get_level(&project_palette)==1) list_palette_scroll_wheel(&project_palette,pnt,move);
}

/* =======================================================

      Item Palette Delete
      
======================================================= */

bool project_palette_delete(void)
{
		// can only delete on
		// first level

	if (list_palette_get_level(&project_palette)!=0) return(FALSE);

		// deletes

	return(FALSE);
}

/* =======================================================

      Property Palette Click Levels
      
======================================================= */

void project_palette_click_level_0(bool double_click)
{
	project_palette_click_main(double_click);
}

void project_palette_click_level_1(bool double_click)
{
	switch (state.proj.cur_item) {

		case item_interface_project:
			project_palette_click_project(double_click);
			break;

		case item_interface_color:
			project_palette_click_color(double_click);
			break;

		case item_interface_device:
			project_palette_click_devices(double_click);
			break;

		case item_interface_logo:
			project_palette_click_logo(double_click);
			break;

		case item_interface_title_page:
			project_palette_click_title_page(double_click);
			break;

		case item_interface_singleplayer:
			project_palette_click_singleplayer(double_click);
			break;

		case item_interface_multiplayer:
			project_palette_click_multiplayer(double_click);
			break;

		case item_interface_setup:
			project_palette_click_setup(double_click);
			break;

		case item_interface_progress:
			project_palette_click_progress(double_click);
			break;

		case item_interface_hud:
			project_palette_click_hud(double_click);
			break;
			
		case item_interface_virtual_control:
			project_palette_click_virtual_control(double_click);
			break;

		case item_interface_radar:
			project_palette_click_radar(double_click);
			break;

		case item_interface_menu:
			project_palette_click_menus(double_click);
			break;

		case item_interface_chooser:
			project_palette_click_choosers(double_click);
			break;

		case item_interface_sound:
			project_palette_click_sounds(double_click);
			break;

		case item_interface_particle:
			project_palette_click_particles(double_click);
			break;

		case item_interface_ring:
			project_palette_click_rings(double_click);
			break;

		case item_interface_halo:
			project_palette_click_halos(double_click);
			break;

		case item_interface_mark:
			project_palette_click_marks(double_click);
			break;
			
		case item_interface_label:
			project_palette_click_labels(double_click);
			break;

		case item_interface_crosshair:
			project_palette_click_crosshairs(double_click);
			break;

		case item_interface_action:
			project_palette_click_actions(double_click);
			break;

		case item_interface_shader:
			project_palette_click_shaders(double_click);
			break;
			
		case item_interface_preload_models:
			project_palette_click_preload_models(double_click);
			break;

		case item_interface_preferences:
			project_palette_click_preferences(double_click);
			break;

	}
}

void project_palette_click_level_2(bool double_click)
{
	switch (state.proj.cur_item) {

		case item_interface_title_page:
			if (state.proj.cur_intro_button_idx!=-1) {
				project_palette_click_title_page_button(state.proj.cur_intro_button_idx,double_click);
				break;
			}
			if (state.proj.cur_intro_model_idx!=-1) {
				project_palette_click_title_page_model(state.proj.cur_intro_model_idx,double_click);
				break;
			}
			break;

		case item_interface_hud:
			if (state.proj.cur_hud_bitmap_idx!=-1) {
				project_palette_click_hud_bitmap(state.proj.cur_hud_bitmap_idx,double_click);
				break;
			}
			if (state.proj.cur_hud_text_idx!=-1) {
				project_palette_click_hud_text(state.proj.cur_hud_text_idx,double_click);
				break;
			}
			if (state.proj.cur_hud_bar_idx!=-1) {
				project_palette_click_hud_bar(state.proj.cur_hud_bar_idx,double_click);
				break;
			}
			break;
			
		case item_interface_virtual_control:
			if (state.proj.cur_virtual_control_stick_idx!=-1) {
				project_palette_click_virtual_control_stick(state.proj.cur_virtual_control_stick_idx,double_click);
				break;
			}
			if (state.proj.cur_virtual_control_button_idx!=-1) {
				project_palette_click_virtual_control_button(state.proj.cur_virtual_control_button_idx,double_click);
				break;
			}
			break;

		case item_interface_radar:
			if (state.proj.cur_radar_icon_idx!=-1) {
				project_palette_click_radar_icon(state.proj.cur_radar_icon_idx,double_click);
				break;
			}
			break;

		case item_interface_singleplayer:
			if (state.proj.cur_singleplayer_option_idx!=-1) {
				project_palette_click_singleplayer_option(state.proj.cur_singleplayer_option_idx,double_click);
				break;
			}
			break;

		case item_interface_multiplayer:
			if (state.proj.cur_multiplayer_character_idx!=-1) {
				project_palette_click_multiplayer_character(state.proj.cur_multiplayer_character_idx,double_click);
				break;
			}
			if (state.proj.cur_multiplayer_game_idx!=-1) {
				project_palette_click_multiplayer_game(state.proj.cur_multiplayer_game_idx,double_click);
				break;
			}
			if (state.proj.cur_multiplayer_option_idx!=-1) {
				project_palette_click_multiplayer_option(state.proj.cur_multiplayer_option_idx,double_click);
				break;
			}
			break;

		case item_interface_menu:
			if (state.proj.cur_menu_idx!=-1) {
				project_palette_click_menu(state.proj.cur_menu_idx,double_click);
				break;
			}
			break;

		case item_interface_chooser:
			if (state.proj.cur_chooser_idx!=-1) {
				project_palette_click_chooser(state.proj.cur_chooser_idx,double_click);
				break;
			}
			break;

		case item_interface_sound:
			if (state.proj.cur_sound_idx!=-1) {
				project_palette_click_sound(state.proj.cur_sound_idx,double_click);
				break;
			}
			break;

		case item_interface_particle:
			if (state.proj.cur_particle_idx!=-1) {
				if (!iface.particle_list.particles[state.proj.cur_particle_idx].group.on) {
					project_palette_click_particle(state.proj.cur_particle_idx,double_click);
				}
				else {
					project_palette_click_particle_group(state.proj.cur_particle_idx,double_click);
				}
				break;
			}
			break;

		case item_interface_ring:
			if (state.proj.cur_ring_idx!=-1) {
				project_palette_click_ring(state.proj.cur_ring_idx,double_click);
				break;
			}
			break;

		case item_interface_halo:
			if (state.proj.cur_halo_idx!=-1) {
				project_palette_click_halo(state.proj.cur_halo_idx,double_click);
				break;
			}
			break;

		case item_interface_mark:
			if (state.proj.cur_mark_idx!=-1) {
				project_palette_click_mark(state.proj.cur_mark_idx,double_click);
				break;
			}
			break;
			
		case item_interface_label:
			if (state.proj.cur_label_idx!=-1) {
				project_palette_click_label(state.proj.cur_label_idx,double_click);
				break;
			}
			break;

		case item_interface_crosshair:
			if (state.proj.cur_crosshair_idx!=-1) {
				project_palette_click_crosshair(state.proj.cur_crosshair_idx,double_click);
				break;
			}
			break;

		case item_interface_action:
			if (state.proj.cur_action_idx!=-1) {
				project_palette_click_action(state.proj.cur_action_idx,double_click);
				break;
			}
			break;

		case item_interface_shader:
			if (state.proj.cur_shader_idx!=-1) {
				project_palette_click_shader(state.proj.cur_shader_idx,double_click);
				break;
			}
			break;

	}
}

void project_palette_click_level_3(bool double_click)
{
	switch (state.proj.cur_item) {

		case item_interface_menu:
			if ((state.proj.cur_menu_idx!=-1) && (state.proj.cur_menu_item_idx!=-1)) {
				project_palette_click_menu_item(state.proj.cur_menu_idx,state.proj.cur_menu_item_idx,double_click);
				break;
			}
			break;

		case item_interface_chooser:
			if ((state.proj.cur_chooser_idx!=-1) && (state.proj.cur_chooser_piece_idx!=-1)) {
				project_palette_click_chooser_piece(state.proj.cur_chooser_idx,state.proj.cur_chooser_piece_idx,double_click);
				break;
			}
			break;

		case item_interface_particle:
			if (state.proj.cur_particle_idx!=-1) {
				if (iface.particle_list.particles[state.proj.cur_particle_idx].group.on) {
					if (state.proj.cur_group_particle_idx!=-1) {
						project_palette_click_group_particle(state.proj.cur_particle_idx,state.proj.cur_group_particle_idx,double_click);
						break;
					}
				}
			}
			break;


	}
}

/* =======================================================

      Property Palette Click MainLine
      
======================================================= */

void project_palette_click(d3pnt *pnt,bool double_click)
{
		// click

	if (!list_palette_click(&project_palette,pnt,double_click)) return;

		// click by level

	switch (list_palette_get_level(&project_palette)) {
		case 0:
			project_palette_click_level_0(double_click);
			break;
		case 1:
			project_palette_click_level_1(double_click);
			break;
		case 2:
			project_palette_click_level_2(double_click);
			break;
		case 3:
			project_palette_click_level_3(double_click);
			break;
	}

	main_wind_draw();
}

