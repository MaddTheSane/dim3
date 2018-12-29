/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Server Periodic Processes

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
#include "network.h"
#include "scripts.h"
#include "objects.h"

extern app_type				app;
extern map_type				map;
extern server_type			server;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern void cinema_run(void);

/* =======================================================

      Mesh Triggers/Messages
  
======================================================= */

void mesh_triggers(obj_type *obj)
{
	int						mesh_idx,old_mesh_idx;
	map_mesh_type			*mesh;
	
		// no triggers if in air
		
	if (obj->air_mode!=am_ground) return;
	
		// any change on stand on?
		
	mesh_idx=obj->contact.stand_poly.mesh_idx;
	old_mesh_idx=obj->mesh.last_stand_mesh_idx;
		
	if (mesh_idx==old_mesh_idx) return;
	
	obj->mesh.last_stand_mesh_idx=mesh_idx;
	
		// run change messages

	if (mesh_idx!=-1) {	
		mesh=&map.mesh.meshes[mesh_idx];
		
			// entry messages
			
		if (mesh->msg.entry_on) {
			scripts_post_event_console(js.course_script_idx,-1,sd_event_message,sd_event_message_from_course,mesh->msg.entry_id);
			scripts_post_event_console(obj->script_idx,-1,sd_event_message,sd_event_message_from_course,mesh->msg.entry_id);
		}
		
			// map change messages
			
		if (mesh->msg.map_change_on) {
			strcpy(map.info.name,mesh->msg.map_name);
			strcpy(map.info.player_start_name,mesh->msg.map_spot_name);
			
			server.map_change.on=TRUE;
			server.map_change.skip_media=FALSE;
			server.map_change.player_restart=FALSE;
		}
		
			// base watch messages
			
		if (mesh->msg.base_on) object_watch_base_alert(mesh,obj,TRUE);
	}
	
	if (old_mesh_idx!=-1) {
	
		mesh=&map.mesh.meshes[old_mesh_idx];
	
			// exit messages
			
		if (mesh->msg.exit_on) {
			scripts_post_event_console(js.course_script_idx,-1,sd_event_message,sd_event_message_from_course,mesh->msg.exit_id);
			scripts_post_event_console(obj->script_idx,-1,sd_event_message,sd_event_message_from_course,mesh->msg.exit_id);
		}
		
			// base watch messages
			
		if (mesh->msg.base_on) object_watch_base_alert(mesh,obj,FALSE);
	}
}

/* =======================================================

      Run Specific Object
  
======================================================= */

void run_object_single(obj_type *obj)
{
		// slice setup
		
	object_slice_setup(obj);
	
		// turning and looking
		
	if (obj->type==object_type_player) {
		if (!obj->input.freeze) {
			object_player_turn(obj);
			object_player_turn_auto_aim(obj);
			object_player_look(obj);
		}
		else {
			if (!obj->suspend) {
				object_turn(obj);
			}
		}
	}
	else {
		if (!obj->suspend) {
			object_turn(obj);
			object_face(obj);
		}
	}
	
	object_thrust(obj);

		// watches

	object_watch(obj);

	if (obj->type==object_type_player) object_checkpoint(obj);

		// health recover

	object_health_recover(obj);

		// movement
	
	if (!obj->suspend) {
		object_auto_walk(obj);
	
		object_fix_motion(obj);
		object_movement(obj,&obj->forward_move);
		object_movement(obj,&obj->side_move);
		object_simple_movement(obj,&obj->vert_move);
		
		object_gravity(obj);
		object_fix_force(obj);
		object_move(obj);
		
		object_fix_bump_smooth(obj);
		object_ducking(obj);

		object_touch(obj);
		object_liquid(obj);

		object_crush(obj,FALSE);
		object_mesh_harm(obj);

		item_pickup_check(obj);
	}

		// auto-growing

	object_grow_run(obj);
	
		// animation events

	if (obj->type==object_type_player) {
		object_event_animations(obj);
	}
	
		// death check
		
	object_death(obj);
}

/* =======================================================

      Run Objects
  
======================================================= */

static inline void run_objects_slice_single(obj_type *obj)
{
		// remotes get predicted
		
	if ((obj->type==object_type_remote_player) || (obj->type==object_type_remote_object)) {
		remote_predict_move(obj);
		return;
	}
	
		// if in network and not host, map bots
		// get predicted
		
	if ((net_setup.mode==net_mode_client) && (obj->type==object_type_bot_map)) {
		remote_predict_move(obj);
		return;
	}
	
		// everything else is a regular move
	
	run_object_single(obj);
}

void run_objects_slice(void)
{
	int				n;
	obj_type		*obj;

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if ((!obj->scenery.on) && (!obj->hidden)) {
			run_objects_slice_single(obj);
			if ((!obj->suspend) && (obj->type!=object_type_remote_player) && (obj->type!=object_type_remote_object)) mesh_triggers(obj);
		}
	}
}

void run_objects_no_slice(void)
{
	int				n;
	obj_type		*obj;

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
	
		object_check_respawn(obj);

		if (!obj->hidden) {
			
			model_draw_setup_object(obj);
			model_run_animation(&obj->draw,game_time_get());

			if (!obj->scenery.on) {
				model_fade_run(&obj->draw);
				model_mesh_fade_run(&obj->draw);
			}
		}
	}
	
		// weapons in hand
		
	if (!app.dedicated_host) weapon_player_run_hand();
}

/* =======================================================

      Run Projectiles
      
======================================================= */

void run_projectiles_slice(void)
{
	int				n;
	proj_type		*proj;
	
	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (!proj->on) continue;
	   
		projectile_slice_setup(proj);
		
		if (projectile_hit_auto(proj)) continue;
			
		projectile_speed(proj);
		projectile_gravity(proj);
		projectile_move(proj);
	}
}

void run_projectiles_no_slice(void)
{
	int				n;
	proj_type		*proj;

	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (!proj->on) continue;
	
		model_draw_setup_projectile(proj);
		model_run_animation(&proj->draw,game_time_get());
		model_fade_run(&proj->draw);
		model_mesh_fade_run(&proj->draw);
	}
}

/* =======================================================

      Run Server Processes
      
======================================================= */

void server_run(void)
{
	int				tick;
	
		// time to run tasks

	tick=game_time_get();

	if (tick>=server.time.run_tick) {

			// tasks that do not require
			// 1/100th of an operation but a specific
			// tick count

		run_objects_no_slice();
		run_projectiles_no_slice();

			// tasks that require 1/100th of
			// a second operation
			
		while (tick>=server.time.run_tick) {
			server.time.run_tick+=10;

			collide_setup();

			group_moves_run(TRUE);
			cinema_run();
			
			run_objects_slice();
			run_projectiles_slice();
			
			map_movements_auto_open();
			
			if (!app.dedicated_host) {
				camera_server_run();
				weapon_player_hand_bounce();
			}

			particle_map_run();
		}

			// effects time-outs
		
		effect_dispose();
	}
}

