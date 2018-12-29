/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette

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

extern map_type					map;
extern app_state_type			state;
extern iface_type				iface;
extern file_path_setup_type		file_path_setup;

char							list_texture_names[max_map_texture][name_str_len];

list_palette_type				map_palette;

/* =======================================================

      Map Palette Setup
      
======================================================= */

void map_palette_initialize(void)
{
	list_palette_list_initialize(&map_palette,"No Properties",FALSE,FALSE,FALSE);

	map_palette.item_pane.click.id=0;
	map_palette.item_pane.click.idx=-1;
	map_palette.item_pane.click.item=NULL;

	map_palette.open=FALSE;
}

void map_palette_shutdown(void)
{
	list_palette_list_shutdown(&map_palette);
}

/* =======================================================

      Property Palette Fills
      
======================================================= */

void map_palette_fill_level_0(void)
{
	map_palette_fill_main();
}

void map_palette_fill_level_1(void)
{
	int					sel_type,main_idx,sub_idx;

		// special check for non-selection property lists
	
	switch (state.map.cur_no_sel_piece_type) {

		case item_map_setting:
			map_palette_fill_map();
			return;

		case item_map_camera:
			map_palette_fill_camera();
			return;

		case item_map_light_media:
			map_palette_fill_light_media();
			return;

		case item_map_sky_weather:
			map_palette_fill_sky_weather();
			return;

		case item_map_group:
			map_palette_fill_group(state.map.cur_group_idx);
			return;
	
		case item_map_movement:
			map_palette_fill_movement(state.map.cur_movement_idx);
			return;

		case item_map_cinema:
			map_palette_fill_cinema(state.map.cur_cinema_idx);
			return;
	}

		// check selection

	main_idx=-1;
	
	if (select_count()!=0)  select_get(0,&sel_type,&main_idx,&sub_idx);
	if (main_idx==-1) return;

		// selection properties

	switch (sel_type) {

		case item_map_mesh:
			if (state.map.select_mode!=select_mode_polygon) sub_idx=-1;
			map_palette_fill_mesh(main_idx,sub_idx);
			break;

		case item_map_liquid:
			map_palette_fill_liquid(main_idx);
			break;

		case item_map_spot:
			map_palette_fill_spot(main_idx);
			break;

		case item_map_node:
			map_palette_fill_node(main_idx);
			break;

		case item_map_light:
			map_palette_fill_light(main_idx);
			break;

		case item_map_sound:
			map_palette_fill_sound(main_idx);
			break;

		case item_map_particle:
			map_palette_fill_particle(main_idx);
			break;

		case item_map_scenery:
			map_palette_fill_scenery(main_idx);
			break;

	}
}

void map_palette_fill_level_2(void)
{
		// selection properties

	switch (state.map.cur_no_sel_piece_type) {

		case item_map_movement:
			map_palette_fill_movement_move(state.map.cur_movement_idx,state.map.cur_movement_move_idx);
			break;

		case item_map_cinema:
			map_palette_fill_cinema_action(state.map.cur_cinema_idx,state.map.cur_cinema_action_idx);
			break;
	}
}

/* =======================================================

      Property Palette Draw
      
======================================================= */

void map_palette_draw(void)
{
		// delete the properties

	list_palette_delete_all_items(&map_palette);

		// if no map, do nothing

	if (!state.map.map_open) return;

		// if texture window is up,
		// put in texture properties

	if (state.map.texture_edit_idx!=-1) {
		map_palette_fill_texture(state.map.texture_edit_idx);
		list_palette_draw(&map_palette);
		return;
	}

		// fill the property list
		
	switch (list_palette_get_level(&map_palette)) {
		case 0:
			map_palette_fill_level_0();
			break;
		case 1:
			map_palette_fill_level_1();
			break;
		case 2:
			map_palette_fill_level_2();
			break;
	}
	
	list_palette_draw(&map_palette);
}

/* =======================================================

      Property Palette Scrolling
      
======================================================= */

void map_palette_reset(void)
{
	int				sel_type,main_idx,sub_idx;
	
		// clear out non-selection indexes

	state.map.cur_group_idx=-1;
	state.map.cur_movement_idx=-1;
	state.map.cur_movement_move_idx=-1;
	state.map.cur_cinema_idx=-1;
	state.map.cur_cinema_action_idx=-1;

		// if no select, change nothing

	if (select_count()==0) {
		state.map.cur_no_sel_piece_type=-1;
		list_palette_set_level(&map_palette,0);
		map_palette_scroll_into_view(item_map_setting,0);
		return;
	}
	
		// set the type

	select_get(0,&sel_type,&main_idx,&sub_idx);
	state.map.cur_no_sel_piece_type=sel_type;
	
	list_palette_set_level(&map_palette,1);
	
	if ((sel_type!=item_map_mesh) && (sel_type!=item_map_liquid)) map_palette_scroll_into_view(sel_type,main_idx);
}

void map_palette_scroll_into_view(int item_id,int item_idx)
{
		// can only scroll into view
		// on map items page

	if (!list_palette_get_level(&map_palette)!=0) return;

	map_palette_fill_level_0();
	list_palette_scroll_item_into_view(&map_palette,item_id,item_idx);

	main_wind_draw();
}

void map_palette_scroll_wheel(d3pnt *pnt,int move)
{
	list_palette_scroll_wheel(&map_palette,pnt,move);
}

/* =======================================================

      Property Palette Delete
      
======================================================= */

bool map_palette_delete(void)
{
	int					n,k;
	bool				del_ok;
	
		// can only delete on map
		// item list

	if (list_palette_get_level(&map_palette)!=0) return(FALSE);

		// anything to delete?

	switch (state.map.cur_no_sel_piece_type) {

			// delete group
			
		case item_map_group:
			
				// check if this group is used within a movement
				
			del_ok=TRUE;
			
			for (n=0;n!=map.movement.nmovement;n++) {
				if ((map.movement.movements[n].group_idx==state.map.cur_group_idx) || (map.movement.movements[n].reverse_group_idx==state.map.cur_group_idx)) {
					del_ok=FALSE;
					break;
				}
			}
			
			if (!del_ok) {
				os_dialog_alert("Delete Group","This group is being used in a movement, it can not be deleted");
				return(TRUE);
			}
				
				// delete group
				
			if (os_dialog_confirm("Delete Group","Is it okay to delete this group?",FALSE)!=0) return(FALSE);
			map_group_delete(&map,state.map.cur_group_idx);
			map_palette_reset();
			return(TRUE);

			// delete movement
			
		case item_map_movement:
		
				// check if movement is used within a cinema
				
			del_ok=TRUE;
			
			for (n=0;n!=map.cinema.ncinema;n++) {
				for (k=0;k!=map.cinema.cinemas[n].naction;k++) {
					if (map.cinema.cinemas[n].actions[k].actor_type==cinema_actor_movement) {
						if (strcasecmp(map.cinema.cinemas[n].actions[k].actor_name,map.movement.movements[state.map.cur_movement_idx].name)==0) {
							del_ok=FALSE;
							break;
						}
					}
				}
			}
			
			if (!del_ok) {
				os_dialog_alert("Delete Movement","This movement is being used in a cinema, it can not be deleted");
				return(TRUE);
			}
			
				// delete movement
				
			if (os_dialog_confirm("Delete Movement","Is it okay to delete this movement?",FALSE)!=0) return(FALSE);
			map_movement_delete(&map,state.map.cur_movement_idx);
			map_palette_reset();
			return(TRUE);

			// delete cinema
			
		case item_map_cinema:
			if (os_dialog_confirm("Delete Cinema","Is it okay to delete this cinema?",FALSE)!=0) return(FALSE);
			map_cinema_delete(&map,state.map.cur_cinema_idx);
			map_palette_reset();
			return(TRUE);

	}

	return(FALSE);
}

/* =======================================================

      Property Palette Click For Levels
      
======================================================= */

void map_palette_click_level_0(bool double_click)
{
	map_palette_click_main(double_click);
}
	
void map_palette_click_level_1(bool double_click)
{
	int					sel_type,main_idx,sub_idx;
	
		// special check for non-selection property lists
		// and check state changes

	switch (state.map.cur_no_sel_piece_type) {

		case item_map_group:
			map_palette_click_group(double_click);
			return;

		case item_map_movement:
			map_palette_click_movement(double_click);
			return;

		case item_map_cinema:
			map_palette_click_cinema(double_click);
			return;
	}

		// get the selection

	main_idx=-1;
	
	if (select_count()!=0) select_get(0,&sel_type,&main_idx,&sub_idx);

		// no selection, map properties

	if (main_idx==-1) {

		switch (state.map.cur_no_sel_piece_type) {
			
			case item_map_setting:
				map_palette_click_map(double_click);
				break;

			case item_map_camera:
				map_palette_click_camera(double_click);
				break;

			case item_map_light_media:
				map_palette_click_light_media(double_click);
				break;

			case item_map_sky_weather:
				map_palette_click_sky_weather(double_click);
				break;

		}

		return;
	}

		// selection properties

	switch (sel_type) {

		case item_map_mesh:
			if (state.map.select_mode!=select_mode_polygon) sub_idx=-1;
			map_palette_click_mesh(main_idx,sub_idx,double_click);
			break;

		case item_map_liquid:
			map_palette_click_liquid(main_idx,double_click);
			break;

		case item_map_spot:
			map_palette_click_spot(main_idx,double_click);
			break;

		case item_map_node:
			map_palette_click_node(main_idx,double_click);
			break;

		case item_map_light:
			map_palette_click_light(main_idx,double_click);
			break;

		case item_map_sound:
			map_palette_click_sound(main_idx,double_click);
			break;

		case item_map_particle:
			map_palette_click_particle(main_idx,double_click);
			break;

		case item_map_scenery:
			map_palette_click_scenery(main_idx,double_click);
			break;

	}
}

void map_palette_click_level_2(bool double_click)
{
		// selection properties

	switch (state.map.cur_no_sel_piece_type) {

		case item_map_movement:
			map_palette_click_movement_move(state.map.cur_movement_idx,state.map.cur_movement_move_idx,double_click);
			return;

		case item_map_cinema:
			map_palette_click_cinema_action(state.map.cur_cinema_idx,state.map.cur_cinema_action_idx,double_click);
			break;
	}
}

/* =======================================================

      Property Palette Click MainLine
      
======================================================= */

void map_palette_click(d3pnt *pnt,bool double_click)
{
	bool				old_open;
	
		// check if open changes
	
	old_open=list_palette_is_open(&map_palette);

		// click

	if (!list_palette_click(&map_palette,pnt,double_click)) {
		if (old_open!=list_palette_is_open(&map_palette)) main_wind_draw();
		return;
	}

		// can't do anything if no map

	if (!state.map.map_open) return;
		
		// if texture window is up, texture properties

	if (state.map.texture_edit_idx!=-1) {
		map_palette_click_texture(state.map.texture_edit_idx,map_palette.item_pane.click.id,double_click);
		main_wind_draw();
		return;
	}
	
		// click by level
		
	switch (list_palette_get_level(&map_palette)) {
		case 0:
			map_palette_click_level_0(double_click);
			break;
		case 1:
			map_palette_click_level_1(double_click);
			break;
		case 2:
			map_palette_click_level_2(double_click);
			break;
	}
	
	main_wind_draw();
}

