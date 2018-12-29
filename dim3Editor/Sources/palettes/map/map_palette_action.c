/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Cinema Action

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

#define kCinemaActionPropertyTimeStart				0
#define kCinemaActionPropertyTimeEnd				1
#define kCinemaActionPropertyAction					2
#define kCinemaActionPropertyActorType				3
#define kCinemaActionPropertyActorName				4
#define kCinemaActionPropertyAnimation				5
#define kCinemaActionPropertyNextAnimation			6
#define kCinemaActionPropertyNode					7
#define kCinemaActionPropertyText					8
#define kCinemaActionPropertyReverseMove			9

extern map_type					map;
extern file_path_setup_type		file_path_setup;
extern iface_type				iface;

extern list_palette_type		map_palette;

extern char						action_actor_type_str[][32],action_action_type_str[][32];

/* =======================================================

      Property Palette Fill Cinema Action
      
======================================================= */

void map_palette_fill_cinema_action(int cinema_idx,int action_idx)
{
	bool					has_actor,has_animation,has_node,is_fade;
	map_cinema_type			*cinema;
	map_cinema_action_type	*action;

	if (action_idx==-1) return;

	cinema=&map.cinema.cinemas[cinema_idx];
	action=&cinema->actions[action_idx];

	list_palette_set_title(&map_palette,"Cinema",cinema->name,"Action",action_actor_type_str[action->actor_type],NULL,NULL);

		// setup some editing enables
	
	has_actor=((action->actor_type!=cinema_actor_camera) && (action->actor_type!=cinema_actor_player));
	has_animation=((action->actor_type==cinema_actor_player) || (action->actor_type==cinema_actor_object));
	has_node=((action->actor_type==cinema_actor_camera) || (action->actor_type==cinema_actor_player) || (action->actor_type==cinema_actor_object) || (action->actor_type==cinema_actor_particle) || (action->actor_type==cinema_actor_sound));
	is_fade=((action->action==cinema_action_fade_in) || (action->action==cinema_action_fade_out));

		// the properties

	list_palette_add_header(&map_palette,0,"Cinema Action Timing");
	list_palette_add_int(&map_palette,kCinemaActionPropertyTimeStart,"Time Start",&action->start_msec,FALSE);
	list_palette_add_int(&map_palette,kCinemaActionPropertyTimeEnd,"Time End",&action->end_msec,FALSE);

	list_palette_add_header(&map_palette,0,"Cinema Action Actor");
	list_palette_add_picker_list_int(&map_palette,kCinemaActionPropertyAction,"Action",(char*)action_action_type_str,-1,name_str_len,0,FALSE,FALSE,&action->action,FALSE);
	if (!is_fade) {
		list_palette_add_picker_list_int(&map_palette,kCinemaActionPropertyActorType,"Type",(char*)action_actor_type_str,-1,name_str_len,0,FALSE,FALSE,&action->actor_type,FALSE);
	}
	else {
		list_palette_add_na_blank(&map_palette,kCinemaActionPropertyActorName,"Type");
	}

		// actors

	if (has_actor) {

		switch (action->actor_type) {

			case cinema_actor_object:
				list_palette_add_picker_list_string(&map_palette,kCinemaActionPropertyActorName,"Spot",(char*)map.spots,map.nspot,sizeof(spot_type),(int)offsetof(spot_type,name),TRUE,TRUE,action->actor_name,FALSE);
				break;

			case cinema_actor_movement:
				list_palette_add_picker_list_string(&map_palette,kCinemaActionPropertyActorName,"Movement",(char*)map.movement.movements,map.movement.nmovement,sizeof(movement_type),(int)offsetof(movement_type,name),TRUE,TRUE,action->actor_name,FALSE);
				break;

			case cinema_actor_particle:
				list_palette_add_picker_list_string(&map_palette,kCinemaActionPropertyActorName,"Particle",(char*)iface.particle_list.particles,iface.particle_list.nparticle,sizeof(iface_particle_type),(int)offsetof(iface_particle_type,name),TRUE,TRUE,action->actor_name,FALSE);
				break;

			case cinema_actor_sound:
				list_palette_add_picker_list_string(&map_palette,kCinemaActionPropertyActorName,"Sound",(char*)iface.sound_list.sounds,iface.sound_list.nsound,sizeof(iface_sound_type),(int)offsetof(iface_sound_type,name),TRUE,TRUE,action->actor_name,FALSE);
				break;

			case cinema_actor_hud_text:
				list_palette_add_picker_list_string(&map_palette,kCinemaActionPropertyActorName,"HUD Text",(char*)iface.text_list.texts,iface.text_list.ntext,sizeof(iface_text_type),(int)offsetof(iface_text_type,name),TRUE,TRUE,action->actor_name,FALSE);
				break;

			case cinema_actor_hud_bitmap:
				list_palette_add_picker_list_string(&map_palette,kCinemaActionPropertyActorName,"HUD Bitmap",(char*)iface.bitmap_list.bitmaps,iface.bitmap_list.nbitmap,sizeof(iface_bitmap_type),(int)offsetof(iface_bitmap_type,name),TRUE,TRUE,action->actor_name,FALSE);
				break;

		}

	}
	else {
		list_palette_add_na_blank(&map_palette,kCinemaActionPropertyActorName,"Name");
	}

		// animations

	if (has_animation) {
		list_palette_add_string(&map_palette,kCinemaActionPropertyAnimation,"Animation",action->animation_name,name_str_len,FALSE);
		list_palette_add_string(&map_palette,kCinemaActionPropertyNextAnimation,"Next Animation",action->next_animation_name,name_str_len,FALSE);
	}
	else {
		list_palette_add_na_blank(&map_palette,kCinemaActionPropertyAnimation,"Animation");
		list_palette_add_na_blank(&map_palette,kCinemaActionPropertyNextAnimation,"Next Animation");
	}

		// nodes

	if ((has_node) && (!is_fade)) {
		list_palette_add_picker_list_string(&map_palette,kCinemaActionPropertyNode,"Node",(char*)map.nodes,map.nnode,sizeof(node_type),(int)offsetof(node_type,name),TRUE,TRUE,action->node_name,FALSE);
	}
	else {
		list_palette_add_na_blank(&map_palette,kCinemaActionPropertyNode,"Node");
	}

	list_palette_add_header(&map_palette,0,"Cinema Action Options");
	if (action->actor_type==cinema_actor_movement) {
		list_palette_add_checkbox(&map_palette,kCinemaActionPropertyReverseMove,"Reverse Move",&action->move_reverse,FALSE);
	}
	else {
		list_palette_add_na_blank(&map_palette,kCinemaActionPropertyReverseMove,"Reverse Move");
	}
	if (action->actor_type==cinema_actor_hud_text) {
		list_palette_add_string(&map_palette,kCinemaActionPropertyText,"Display Text",action->text_str,256,FALSE);
	}
	else {
		list_palette_add_na_blank(&map_palette,kCinemaActionPropertyText,"Display Text");
	}
}

/* =======================================================

      Property Palette Click Cinema Action
      
======================================================= */

void map_palette_click_cinema_action(int cinema_idx,int action_idx,bool double_click)
{
}
