/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Cinemas

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
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "scripts.h"
#include "objects.h"

extern bool map_movements_cinema_start(int movement_idx,bool reverse,char *err_str);

extern app_type					app;
extern server_type				server;
extern view_type				view;
extern map_type					map;
extern iface_type				iface;
extern js_type					js;
extern network_setup_type		net_setup;

extern camera_type				camera;

/* =======================================================

      Cinema Operations
      
======================================================= */

bool cinema_start(char *name,int event_id,char *err_str)
{
	int				n;
	obj_type		*player_obj;

		// get the cinema

	view.cinema.idx=-1;

	for (n=0;n!=map.cinema.ncinema;n++) {
		if (strcmp(name,map.cinema.cinemas[n].name)==0) {
			view.cinema.idx=n;
			break;
		}
	}

	if (view.cinema.idx==-1) {
		sprintf(err_str,"No cinema exists with this name: %s",name);
		return(FALSE);
	}

		// start the timing and events

	view.cinema.on=TRUE;

	view.cinema.start_tick=game_time_get();
	view.cinema.last_tick=0;

	view.cinema.event_id=event_id;

		// backup the camera

	memmove(&view.cinema.camera_state,&camera,sizeof(camera_type));
	
		// backup the HUD states and hid if necessary
		
	if (!map.cinema.cinemas[view.cinema.idx].show_hud) {
	
		for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
			iface.bitmap_list.bitmaps[n].old_show=iface.bitmap_list.bitmaps[n].show;
			iface.bitmap_list.bitmaps[n].show=FALSE;
		}
		for (n=0;n!=iface.text_list.ntext;n++) {
			iface.text_list.texts[n].old_show=iface.text_list.texts[n].show;
			iface.text_list.texts[n].show=FALSE;
		}
		for (n=0;n!=iface.bar_list.nbar;n++) {
			iface.bar_list.bars[n].old_show=iface.bar_list.bars[n].show;
			iface.bar_list.bars[n].show=FALSE;
		}
	
	}

		// run any input freeze

	if (map.cinema.cinemas[view.cinema.idx].freeze_input) {
		player_obj=server.obj_list.objs[server.player_obj_idx];
		object_input_freeze(player_obj,TRUE);
	}

	return(TRUE);
}

void cinema_end(void)
{
	int				n;
	obj_type		*player_obj;
	
		// restore HUD states
		
	if (!map.cinema.cinemas[view.cinema.idx].show_hud) {
	
		for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
			iface.bitmap_list.bitmaps[n].show=iface.bitmap_list.bitmaps[n].old_show;
		}
		for (n=0;n!=iface.text_list.ntext;n++) {
			iface.text_list.texts[n].show=iface.text_list.texts[n].old_show;
		}
		for (n=0;n!=iface.bar_list.nbar;n++) {
			iface.bar_list.bars[n].show=iface.bar_list.bars[n].old_show;
		}
	
	}

		// turn off input freeze

	if (map.cinema.cinemas[view.cinema.idx].freeze_input) {
		player_obj=server.obj_list.objs[server.player_obj_idx];
		object_input_freeze(player_obj,FALSE);
	}

		// restore camera

	memmove(&camera,&view.cinema.camera_state,sizeof(camera_type));
	
		// clear any input
		// and stop any long running fades
		
	input_clear();
	view_fade_cancel();
	
		// turn off cinema
		
	view.cinema.on=FALSE;
}

/* =======================================================

      Cinema Actions Camera
      
======================================================= */

void cinema_action_run_camera(map_cinema_action_type *action)
{
	int			node_idx,start_node_idx;
	char		err_str[256];
	node_type	*node;

		// any camera action sets camera to static

	map.camera.camera_mode=cv_static;
	map.camera.c_static.follow=FALSE;

		// camera placement

	if (action->action==cinema_action_place) {

		node_idx=map_find_node(&map,action->node_name);
		if (node_idx==-1) return;
		
		node=&map.nodes[node_idx];

		memmove(&camera.cur_pos.static_pnt,&node->pnt,sizeof(d3pnt));
		memmove(&camera.cur_pos.static_ang,&node->ang,sizeof(d3ang));
		
		return;
	}

		// camera walking

	if (action->action==cinema_action_move) {

		node_idx=map_find_node(&map,action->node_name);
		if (node_idx==-1) return;

		start_node_idx=map_find_nearest_node_by_point(&map,&camera.cur_pos.pnt);
		if (start_node_idx==-1) return;

		if (!camera_walk_to_node_by_index_setup(start_node_idx,node_idx,(action->end_msec-action->start_msec),-1,FALSE,FALSE,err_str)) console_add_error(err_str);
	}
}

/* =======================================================

      Cinema Actions Objects
      
======================================================= */

void cinema_action_run_object(map_cinema_action_type *action)
{
	int					node_idx,start_node_idx,dist;
	char				err_str[256];
	node_type			*node;
	obj_type			*obj;

		// get object

	if (action->actor_type==cinema_actor_player) {
		obj=server.obj_list.objs[server.player_obj_idx];
	}
	else {
		obj=object_find_name(action->actor_name);
		if (obj==NULL) {
			sprintf(err_str,"Unknown object in cinema: %s",action->actor_name);
			console_add_error(err_str);
			return;
		}
	}

		// start any animations

	if (action->animation_name[0]!=0x0) {
		model_start_animation(&obj->draw,action->animation_name,game_time_get());
	}

	if (action->next_animation_name[0]!=0x0) {
		model_change_animation(&obj->draw,action->next_animation_name,game_time_get());
	}

		// object placement

	if (action->action==cinema_action_place) {

		node_idx=map_find_node(&map,action->node_name);
		if (node_idx==-1) return;

		node=&map.nodes[node_idx];

		obj->pnt.x=node->pnt.x;
		obj->pnt.y=node->pnt.y;
		obj->pnt.z=node->pnt.z;

		obj->ang.x=obj->motion.ang.x=node->ang.x;
		obj->ang.y=obj->motion.ang.y=node->ang.y;
		obj->ang.z=obj->motion.ang.z=node->ang.z;

		return;
	}

		// object walking

	if (action->action==cinema_action_move) {
		
		node_idx=map_find_node(&map,action->node_name);
		if (node_idx==-1) return;

		start_node_idx=map_find_nearest_node_by_point(&map,&obj->pnt);
		if (start_node_idx==-1) return;

			// find speed

		dist=map_node_to_node_distance(&map,start_node_idx,node_idx);
		obj->forward_move.speed=((float)dist)/((float)((action->end_msec-action->start_msec)/10));

			// throw an error if forward speed is
			// greater than object max speed

		if (obj->forward_move.speed>obj->forward_move.max_walk_speed) {
			sprintf(err_str,"%s max speed is too low to reach the node",obj->name);
			console_add_error(err_str);
			return;
		}

			// start walk

		if (!object_auto_walk_node_setup(obj,start_node_idx,node_idx,TRUE,-1,err_str)) console_add_error(err_str);
		
		return;
	}
	
		// object stopping
		
	if (action->action==cinema_action_stop) {
		object_move_stop(obj);
		return;
	}

		// show and hide

	if ((action->action==cinema_action_show) || (action->action==cinema_action_show_fade)) {
		obj->hidden=FALSE;
		return;
	}

	if (action->action==cinema_action_hide) {
		obj->hidden=TRUE;
		return;
	}
}

/* =======================================================

      Cinema Actions Movement, Particle, HUD
      
======================================================= */

void cinema_action_run_movement(map_cinema_action_type *action)
{
	int				movement_idx;
	char			err_str[256];

	movement_idx=map_movement_find(&map,action->actor_name);
	if (movement_idx==-1) {
		sprintf(err_str,"Unknown movement in cinema: %s",action->actor_name);
		console_add_error(err_str);
		return;
	}

	if (!map_movements_cinema_start(movement_idx,action->move_reverse,err_str)) console_add_error(err_str);
}

void cinema_action_run_particle(map_cinema_action_type *action)
{
	int				particle_idx,node_idx;
	char			err_str[256];

	particle_idx=particle_find_index(action->actor_name);
	if (particle_idx==-1) {
		sprintf(err_str,"Unknown particle in cinema: %s",action->actor_name);
		console_add_error(err_str);
		return;
	}

	node_idx=map_find_node(&map,action->node_name);
	if (node_idx==-1) {
		sprintf(err_str,"Need node to launch cinema particle: %s",action->actor_name);
		console_add_error(err_str);
		return;
	}
	
	particle_spawn(particle_idx,-1,&map.nodes[node_idx].pnt,NULL,NULL);
}

void cinema_action_run_sound(map_cinema_action_type *action)
{
	int				buffer_idx,node_idx;
	char			err_str[256];

	if (app.dedicated_host) return;
	
	buffer_idx=al_find_buffer(action->actor_name);
	if (buffer_idx==-1) {
		sprintf(err_str,"Unknown sound in cinema: %s",action->actor_name);
		console_add_error(err_str);
		return;
	}

	node_idx=map_find_node(&map,action->node_name);
	if (node_idx==-1) {
		sprintf(err_str,"Need node to launch cinema sound: %s",action->actor_name);
		console_add_error(err_str);
		return;
	}

	al_play_source(buffer_idx,&map.nodes[node_idx].pnt,1.0f,FALSE,FALSE,FALSE,FALSE);
}

void cinema_action_run_hud_bitmap(map_cinema_action_type *action)
{
	iface_bitmap_type		*bitmap;
	
	bitmap=iface_bitmaps_find(&iface,action->actor_name);
	if (bitmap==NULL) return;
	
	switch (action->action) {
		case cinema_action_show:
			bitmap->show=TRUE;
			break;
		case cinema_action_show_fade:
			bitmap->show=TRUE;
			bitmap->fade.on=TRUE;
			bitmap->fade.start_tick=game_time_get();
			break;
		case cinema_action_hide:
			bitmap->show=FALSE;
			break;
	}
}

void cinema_action_run_hud_text(map_cinema_action_type *action)
{
	iface_text_type			*text;

	text=iface_texts_find(&iface,action->actor_name);
	if (text==NULL) return;

		// do hide show

	switch (action->action) {
		case cinema_action_show:
			text->show=TRUE;
			break;
		case cinema_action_show_fade:
			text->show=TRUE;
			text->fade.on=TRUE;
			text->fade.start_tick=game_time_get();
			break;
		case cinema_action_hide:
			text->show=FALSE;
			break;
	}

		// do text replacement
		
	iface_text_set(text,action->text_str);
}

/* =======================================================

      Cinema Actions
      
======================================================= */

void cinema_action_run_generic(map_cinema_action_type *action)
{
		// fade in/out have no actors

	if (action->action==cinema_action_fade_in) {
		view_fade_cinema_fade_in_start(action->end_msec-action->start_msec);
		return;
	}

	if (action->action==cinema_action_fade_out) {
		view_fade_cinema_fade_out_start(action->end_msec-action->start_msec);
		return;
	}

		// route to proper action

	switch (action->actor_type) {

		case cinema_actor_camera:
			cinema_action_run_camera(action);
			return;

		case cinema_actor_player:
		case cinema_actor_object:
			cinema_action_run_object(action);
			return;

		case cinema_actor_movement:
			cinema_action_run_movement(action);
			return;

		case cinema_actor_particle:
			cinema_action_run_particle(action);
			return;

		case cinema_actor_sound:
			cinema_action_run_sound(action);
			return;

		case cinema_actor_hud_text:
			cinema_action_run_hud_text(action);
			return;

		case cinema_actor_hud_bitmap:
			cinema_action_run_hud_bitmap(action);
			return;

	}
}

bool cinema_actions(void)
{
	int						n,tick;
	map_cinema_type			*cinema;
	map_cinema_action_type	*action;

	cinema=&map.cinema.cinemas[view.cinema.idx];

		// is cinema over?

	tick=game_time_get()-view.cinema.start_tick;
	if (tick>=cinema->len_msec) return(TRUE);

		// run actions

	action=cinema->actions;

	for (n=0;n!=cinema->naction;n++) {

			// time to start action?

		if ((action->start_msec>=view.cinema.last_tick) && (action->start_msec<tick)) {
			cinema_action_run_generic(action);
		}

		action++;
	}

		// remember last tick

	view.cinema.last_tick=tick;

	return(FALSE);
}

/* =======================================================

      Run Cinema
      
======================================================= */

void cinema_run(void)
{
	bool				cinema_done;
	obj_type			*obj;

	if (!view.cinema.on) return;

		// run actions

	cinema_done=cinema_actions();
	
		// mouse buttons end cinema
		// if we are in an input freeze
		
	if ((map.cinema.cinemas[view.cinema.idx].freeze_input) && (!map.cinema.cinemas[view.cinema.idx].no_cancel)) {
		if (input_gui_is_click_down()) cinema_done=TRUE;
	}
	
		// cinema over
	
	if (cinema_done) {

			// send the finish event

		if (view.cinema.event_id!=-1) {
			scripts_post_event_console(js.game_script_idx,-1,sd_event_interface,sd_event_interface_cinema_done,view.cinema.event_id);
			scripts_post_event_console(js.course_script_idx,-1,sd_event_interface,sd_event_interface_cinema_done,view.cinema.event_id);

			if (!app.dedicated_host) {
				obj=server.obj_list.objs[server.player_obj_idx];
				scripts_post_event_console(obj->script_idx,-1,sd_event_interface,sd_event_interface_cinema_done,view.cinema.event_id);
			}
		}

		cinema_end();
	}
}


