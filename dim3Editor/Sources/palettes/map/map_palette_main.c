/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Map Camera

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

#define kPropertyAddSpot			100		// this numbers need to be greater than the regular piece numbers
#define kPropertyAddLight			101
#define kPropertyAddSound			102
#define kPropertyAddParticle		103
#define kPropertyAddScenery			104
#define kPropertyAddNode			105
#define kPropertyAddGroup			106
#define kPropertyAddMovement		107
#define kPropertyAddCinema			108

extern map_type					map;
extern app_state_type			state;

extern list_palette_type		map_palette;

/* =======================================================

      Property Palette Fill Main
      
======================================================= */

void map_palette_fill_main(void)
{
	int			n;

		// title
		
	list_palette_set_title(&map_palette,"Map",NULL,NULL,NULL,NULL,NULL);

		// map

	list_palette_add_header(&map_palette,0,"Settings");
	list_palette_add_item(&map_palette,item_map_setting,0,"General",(map_palette.item_pane.click.id==item_map_setting),FALSE);
	list_palette_add_item(&map_palette,item_map_camera,0,"Camera",(map_palette.item_pane.click.id==item_map_camera),FALSE);
	list_palette_add_item(&map_palette,item_map_light_media,0,"Light & Media",(map_palette.item_pane.click.id==item_map_light_media),FALSE);
	list_palette_add_item(&map_palette,item_map_sky_weather,0,"Sky & Weather",(map_palette.item_pane.click.id==item_map_sky_weather),FALSE);

		// spots

	list_palette_add_header_button(&map_palette,kPropertyAddSpot,"Spots",list_button_plus);
	list_palette_sort_mark_start(&map_palette,&map_palette.item_pane);

	for (n=0;n!=map.nspot;n++) {
		if (map.spots[n].name[0]!=0x0) {
			list_palette_add_item(&map_palette,item_map_spot,n,map.spots[n].name,((map_palette.item_pane.click.id==item_map_spot)&&(n==map_palette.item_pane.click.idx)),FALSE);
		}
		else {
			list_palette_add_item(&map_palette,item_map_spot,n,"[none]",((map_palette.item_pane.click.id==item_map_spot)&&(n==map_palette.item_pane.click.idx)),FALSE);
		}
	}

	list_palette_sort(&map_palette,&map_palette.item_pane);

		// lights

	list_palette_add_header_button(&map_palette,kPropertyAddLight,"Lights",list_button_plus);

	for (n=0;n!=map.nlight;n++) {
		list_palette_add_color(&map_palette,item_map_light,n,&map.lights[n].setting.col,((map_palette.item_pane.click.id==item_map_light)&&(n==map_palette.item_pane.click.idx)),FALSE);
	}

		// sounds

	list_palette_add_header_button(&map_palette,kPropertyAddSound,"Sounds",list_button_plus);
	list_palette_sort_mark_start(&map_palette,&map_palette.item_pane);

	for (n=0;n!=map.nsound;n++) {
		list_palette_add_item(&map_palette,item_map_sound,n,map.sounds[n].name,((map_palette.item_pane.click.id==item_map_sound)&&(n==map_palette.item_pane.click.idx)),FALSE);
	}

	list_palette_sort(&map_palette,&map_palette.item_pane);

		// particles

	list_palette_add_header_button(&map_palette,kPropertyAddParticle,"Particles",list_button_plus);
	list_palette_sort_mark_start(&map_palette,&map_palette.item_pane);

	for (n=0;n!=map.nparticle;n++) {
		list_palette_add_item(&map_palette,item_map_particle,n,map.particles[n].name,((map_palette.item_pane.click.id==item_map_particle)&&(n==map_palette.item_pane.click.idx)),FALSE);
	}

	list_palette_sort(&map_palette,&map_palette.item_pane);

		// sceneries

	list_palette_add_header_button(&map_palette,kPropertyAddScenery,"Scenery",list_button_plus);
	list_palette_sort_mark_start(&map_palette,&map_palette.item_pane);

	for (n=0;n!=map.nscenery;n++) {
		list_palette_add_item(&map_palette,item_map_scenery,n,map.sceneries[n].model_name,((map_palette.item_pane.click.id==item_map_scenery)&&(n==map_palette.item_pane.click.idx)),FALSE);
	}

	list_palette_sort(&map_palette,&map_palette.item_pane);

		// nodes

	list_palette_add_header_button(&map_palette,kPropertyAddNode,"Nodes",list_button_plus);
	list_palette_sort_mark_start(&map_palette,&map_palette.item_pane);

	for (n=0;n!=map.nnode;n++) {
		if (map.nodes[n].name[0]!=0x0) list_palette_add_item(&map_palette,item_map_node,n,map.nodes[n].name,((map_palette.item_pane.click.id==item_map_node)&&(n==map_palette.item_pane.click.idx)),FALSE);
	}

	list_palette_sort(&map_palette,&map_palette.item_pane);

		// groups

	list_palette_add_header_button(&map_palette,kPropertyAddGroup,"Groups",list_button_plus);
	list_palette_sort_mark_start(&map_palette,&map_palette.item_pane);

	for (n=0;n!=map.group.ngroup;n++) {
		list_palette_add_item(&map_palette,item_map_group,n,map.group.groups[n].name,((map_palette.item_pane.click.id==item_map_group)&&(n==map_palette.item_pane.click.idx)),FALSE);
	}

	list_palette_sort(&map_palette,&map_palette.item_pane);

		// movements

	list_palette_add_header_button(&map_palette,kPropertyAddMovement,"Movements",list_button_plus);
	list_palette_sort_mark_start(&map_palette,&map_palette.item_pane);

	for (n=0;n!=map.movement.nmovement;n++) {
		list_palette_add_item(&map_palette,item_map_movement,n,map.movement.movements[n].name,((map_palette.item_pane.click.id==item_map_movement)&&(n==map_palette.item_pane.click.idx)),FALSE);
	}

	list_palette_sort(&map_palette,&map_palette.item_pane);

		// cinemas

	list_palette_add_header_button(&map_palette,kPropertyAddCinema,"Cinemas",list_button_plus);
	list_palette_sort_mark_start(&map_palette,&map_palette.item_pane);

	for (n=0;n!=map.cinema.ncinema;n++) {
		list_palette_add_item(&map_palette,item_map_cinema,n,map.cinema.cinemas[n].name,((map_palette.item_pane.click.id==item_map_cinema)&&(n==map_palette.item_pane.click.idx)),FALSE);
	}

	list_palette_sort(&map_palette,&map_palette.item_pane);
}

/* =======================================================

      Property Palette Click Main
      
======================================================= */

void map_palette_click_main(bool double_click)
{
		// add buttons

	switch (map_palette.item_pane.click.id) {

		case kPropertyAddSpot:
			piece_create_spot();
			return;

		case kPropertyAddLight:
			piece_create_light();
			return;

		case kPropertyAddSound:
			piece_create_sound();
			return;

		case kPropertyAddParticle:
			piece_create_particle();
			return;

		case kPropertyAddScenery:
			piece_create_scenery();
			return;

		case kPropertyAddNode:
			piece_create_node();
			return;

		case kPropertyAddGroup:
			piece_create_group();
			return;

		case kPropertyAddMovement:
			piece_create_movement();
			return;

		case kPropertyAddCinema:
			piece_create_cinema();
			return;
	}

	if (map_palette.item_pane.click.idx==-1) return;

		// no selection states
	
	state.map.cur_no_sel_piece_type=map_palette.item_pane.click.id;

	state.map.cur_group_idx=-1;
	state.map.cur_movement_idx=-1;
	state.map.cur_movement_move_idx=-1;
	state.map.cur_cinema_idx=-1;
	state.map.cur_cinema_action_idx=-1;

		// special check for non-selection
		// type double-clicks

	if (double_click) {
		switch (state.map.cur_no_sel_piece_type) {

			case item_map_setting:
			case item_map_camera:
			case item_map_light_media:
			case item_map_sky_weather:
				list_palette_set_level(&map_palette,1);
				main_wind_draw();
				return;

		}
	}

		// handle click

	select_clear();

	switch (state.map.cur_no_sel_piece_type) {

		case item_map_group:
			state.map.cur_group_idx=map_palette.item_pane.click.idx;
			select_add_group(state.map.cur_group_idx);
			break;

		case item_map_movement:
			state.map.cur_movement_idx=map_palette.item_pane.click.idx;
			state.map.cur_movement_move_idx=-1;
			select_add_movement(state.map.cur_movement_idx);
			break;

		case item_map_cinema:
			state.map.cur_cinema_idx=map_palette.item_pane.click.idx;
			state.map.cur_cinema_action_idx=-1;
			select_add_cinema(state.map.cur_cinema_idx);
			break;

		default:
			select_add(map_palette.item_pane.click.id,map_palette.item_pane.click.idx,-1);
			break;

	}

	if (double_click) map_view_goto_select();

		// turn on any hidden items

	switch (state.map.cur_no_sel_piece_type) {

		case item_map_spot:
		case item_map_scenery:
			state.map.show_object=TRUE;
			break;

		case item_map_light:
		case item_map_sound:
		case item_map_particle:
			state.map.show_lightsoundparticle=TRUE;
			break;

		case item_map_node:
			state.map.show_node=TRUE;
			break;
	}
	
		// open the property window for double-click
		
	if (double_click) list_palette_set_level(&map_palette,1);

	main_wind_draw();
}

