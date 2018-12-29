/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Remote Player Objects

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
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

/* =======================================================

      Add and Remove Remote Object
      
======================================================= */

bool remote_add(network_request_remote_add *remote)
{
	int					idx,the_type;
	char				err_str[256];
	obj_type			*obj,*player_obj;
	
		// create new object
		
	the_type=(signed short)ntohs(remote->type);
		
	idx=object_create(remote->name,the_type,bt_game);
    if (idx==-1) return(FALSE);

	obj=server.obj_list.objs[idx];
	
		// setup remote
		
	obj->team_idx=(signed short)ntohs(remote->team_idx);
	obj->tint_color_idx=(signed short)ntohs(remote->tint_color_idx);
	
	obj->pnt.x=obj->pnt.y=obj->pnt.z=0;
	obj->ang.x=obj->ang.y=obj->ang.z=0.0f;

	obj->remote.net_uid=(signed short)ntohs(remote->add_net_uid);
	obj->remote.last_update=game_time_get();
	obj->remote.talking=FALSE;
	
	obj->remote.predict.move.x=0;
	obj->remote.predict.move.y=0;
	obj->remote.predict.move.z=0;
	
	obj->remote.predict.turn_y=0.0f;
	
	obj->mesh.last_stand_mesh_idx=-1;
	
		// remote script

	strcpy(obj->spawn_spot.script,remote->script_name);

	if (!object_start_script(obj,FALSE,err_str)) {
		free(server.obj_list.objs[idx]);
		server.obj_list.objs[idx]=NULL;
		return(FALSE);
	}

		// load models
		// we substitute the model passed through
		// from the remote as there is no script to
		// set it up
		
	strcpy(obj->draw.name,remote->draw_name);
	obj->draw.on=TRUE;
		
	if (!model_draw_load(&obj->draw,"Remote",obj->name,FALSE,err_str)) {
		console_add_error(err_str);
		free(server.obj_list.objs[idx]);
		server.obj_list.objs[idx]=NULL;
		return(FALSE);
	}
	
		// initial score
		
	obj->score.score=(signed short)ntohs(remote->score);
	
		// start remotes hidden
		
	obj->hidden=TRUE;

		// send event to player

	if ((!app.dedicated_host) && (obj->type==object_type_remote_player)) {
		player_obj=server.obj_list.objs[server.player_obj_idx];
		scripts_post_event_console(player_obj->script_idx,-1,sd_event_remote,sd_event_remote_join,obj->idx);
	}
	
	return(TRUE);
}

void remote_remove(network_request_remote_remove *remove)
{
	obj_type			*obj,*player_obj;
	
		// find remote index
		
	obj=object_find_remote_net_uid((signed short)ntohs(remove->remove_net_uid));
	if (obj==NULL) return;

		// send event to player
		// do it before dispose so player can
		// read the object if it wants to

	if ((!app.dedicated_host) && (obj->type==object_type_remote_player)) {
		player_obj=server.obj_list.objs[server.player_obj_idx];
		scripts_post_event_console(player_obj->script_idx,-1,sd_event_remote,sd_event_remote_leave,obj->idx);
	}
	
		// remove any projectiles

	projectile_dispose_object(obj);

		// remove the obj
		
	object_dispose_single(obj->idx);
}

/* =======================================================

      Remote Timeouts
      
======================================================= */

bool remote_timed_out(obj_type *obj)
{
	return((game_time_get()-obj->remote.last_update)>client_communication_timeout_msec);
}

/* =======================================================

      Predict Remote Objects
      
======================================================= */

void remote_predict_move(obj_type *obj)
{
		// stop predicting after slow timeout
		
	if (remote_timed_out(obj)) return;
	
		// predict rotations
	
    obj->ang.y=angle_add(obj->ang.y,obj->remote.predict.turn_y);
	
		// predict movements
		
	object_move_remote(obj);
	
		// run pickups
		
	item_pickup_check(obj);
	
		// reduce movements
		
	obj->remote.predict.move.x=(int)((float)obj->remote.predict.move.x*0.9f);
	obj->remote.predict.move.y=(int)((float)obj->remote.predict.move.y*0.9f);
	obj->remote.predict.move.z=(int)((float)obj->remote.predict.move.z*0.9f);
}

/* =======================================================

      Remote Game Resets and Host Exits
      
======================================================= */

void remote_game_reset(network_request_game_reset *reset)
{
	char				err_str[256];
	obj_type			*player_obj;

		// switch map

	strcpy(map.info.name,reset->map_name);
	map.info.player_start_name[0]=0x0;

	if (!map_rebuild_changes(err_str)) {
		game_end();
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// respawn the player
		
	if (app.dedicated_host) return;

	player_obj=server.obj_list.objs[server.player_obj_idx];
	
	player_obj->next_spawn_sub_event=sd_event_spawn_game_reset;
	
	if (!object_spawn(player_obj,err_str)) {
		game_end();
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}
}

void remote_host_exit(void)
{
		// quit current game
		
	map_end();
	game_end();

		// set error page

	error_setup("The network server has been shutdown or stopped responding","Network Game Canceled");
	server.next_state=gs_error;
}

/* =======================================================

      Remote Death
      
======================================================= */

void remote_death(network_request_remote_death *death)
{
	int					death_net_uid,killer_net_uid;
	bool				telefrag;
	obj_type			*obj,*kill_obj,*telefrag_obj,*player_obj;

	death_net_uid=(signed short)ntohs(death->death_net_uid);
	killer_net_uid=(signed short)ntohs(death->killer_net_uid);
	
	obj=object_find_remote_net_uid(death_net_uid);
	if (obj==NULL) return;
	
		// set health to obj
		// this stops dead remotes from picking things up
		
	obj->status.health.value=0;

		// normal death
		
	telefrag=(ntohs(death->telefrag)!=0);

	if (!telefrag) {
		
			// find kill object
			
		obj->damage_obj_idx=-1;
		
		if (killer_net_uid!=-1) {
			kill_obj=object_find_remote_net_uid(killer_net_uid);
			if (kill_obj!=NULL) obj->damage_obj_idx=kill_obj->idx;
		}

			// send death/suicide remote event

		if ((!app.dedicated_host) && (obj->type==object_type_remote_player)) {
			player_obj=server.obj_list.objs[server.player_obj_idx];
			if ((obj->damage_obj_idx==-1) || (obj->damage_obj_idx==obj->idx)) {
				scripts_post_event_console(player_obj->script_idx,-1,sd_event_remote,sd_event_remote_suicide,obj->idx);
			}
			else {
				scripts_post_event_console(player_obj->script_idx,-1,sd_event_remote,sd_event_remote_death,obj->idx);
			}
		}
	}

		// telefrag death

	else {
		telefrag_obj=object_find_remote_net_uid(killer_net_uid);
		object_telefrag(telefrag_obj,obj);
			
			// send telefrag remote event
			
		if ((!app.dedicated_host) && (obj->type==object_type_remote_player)) {
			player_obj=server.obj_list.objs[server.player_obj_idx];
			scripts_post_event_console(player_obj->script_idx,-1,sd_event_remote,sd_event_remote_telefrag,telefrag_obj->idx);
		}
	}
		
		// change the score
		
	object_score_death(obj);
	
		// send alert to all objects with watches on

	object_watch_death_alert(obj);
}

/* =======================================================

      Remote Updates
      
======================================================= */

void remote_update_pack(obj_type *obj,bool chat_on,network_request_remote_update *update)
{
	int								n,idx,tick,flags;
	weapon_type						*weap;
	model_draw						*draw;
	model_draw_animation			*animation;
	model_draw_dynamic_bone			*dyn_bone;
	network_request_animation		*net_animation;
	network_request_dynamic_bone	*net_dyn_bone;

	draw=&obj->draw;
	
		// net UID
		
	update->update_net_uid=htons((short)obj->remote.net_uid);

		// create flags
		
	flags=0;
	
	if (obj->hidden) flags|=net_update_flag_hidden;
	if (!obj->contact.object_on) flags|=net_update_flag_no_contact_object;
	if (!obj->contact.projectile_on) flags|=net_update_flag_no_contact_object;
	if (!obj->contact.force_on) flags|=net_update_flag_no_contact_object;
	if (obj->click.on) flags|=net_update_flag_clickable;
	if (obj->pickup.on) flags|=net_update_flag_pickup;
	if (chat_on) flags|=net_update_flag_talking;
	
	update->flags=htonl(flags);
	
		// position
		
	update->pnt_x=htonl(obj->pnt.x);
	update->pnt_y=htonl(obj->pnt.y);
	update->pnt_z=htonl(obj->pnt.z);
	
	update->fp_ang_x=htonf(obj->ang.x);
	update->fp_ang_y=htonf(obj->ang.y);
	update->fp_ang_z=htonf(obj->ang.z);
	
	update->fp_face_ang_x=htonf(obj->face.ang.x);
	update->fp_face_ang_y=htonf(obj->face.ang.y);
	update->fp_face_ang_z=htonf(obj->face.ang.z);

	update->offset_x=htons((short)draw->offset.x);
	update->offset_y=htons((short)draw->offset.y);
	update->offset_z=htons((short)draw->offset.z);

	update->fp_predict_move_x=htonl(obj->pnt.x-obj->last_pnt.x);
	update->fp_predict_move_y=htonl(obj->pnt.y-obj->last_pnt.y);
	update->fp_predict_move_z=htonl(obj->pnt.z-obj->last_pnt.z);
	
	update->fp_predict_turn_y=htonf(obj->last_ang.y-obj->ang.y);
	
		// model animations

	animation=draw->animations;
	net_animation=update->animation;

	tick=game_time_get();
	
	for (n=0;n!=max_model_blend_animation;n++) {
		net_animation->model_tick=htonl(animation->tick-tick);
		net_animation->model_mode=htons((short)animation->mode);
		net_animation->model_animate_idx=htons((short)animation->animate_idx);
		net_animation->model_animate_next_idx=htons((short)animation->animate_next_idx);
		net_animation->model_pose_move_idx=htons((short)animation->pose_move_idx);
		net_animation->model_smooth_animate_idx=htons((short)animation->smooth_animate_idx);
		net_animation->model_smooth_pose_move_idx=htons((short)animation->smooth_pose_move_idx);

		animation++;
		net_animation++;
	}
	
	update->model_mesh_mask=htonl(draw->mesh_mask);
	
	for (n=0;n!=max_model_texture;n++) {
		update->model_cur_texture_frame[n]=(unsigned char)draw->textures[n].frame;
	}

		// dynamic bones

	dyn_bone=draw->dynamic_bones;
	net_dyn_bone=update->dynamic_bones;

	for (n=0;n!=max_model_dynamic_bone;n++) {

		net_dyn_bone->bone_idx=htons((short)dyn_bone->bone_idx);

		if (dyn_bone->bone_idx!=-1) {
			net_dyn_bone->fp_mov_x=htonf(dyn_bone->mov.x);
			net_dyn_bone->fp_mov_y=htonf(dyn_bone->mov.y);
			net_dyn_bone->fp_mov_z=htonf(dyn_bone->mov.z);
			net_dyn_bone->fp_rot_x=htonf(dyn_bone->rot.x);
			net_dyn_bone->fp_rot_y=htonf(dyn_bone->rot.y);
			net_dyn_bone->fp_rot_z=htonf(dyn_bone->rot.z);
			net_dyn_bone->fp_resize=htonf(dyn_bone->resize);
		}

		dyn_bone++;
		net_dyn_bone++;
	}
	
		// status

	update->score=htons((short)obj->score.score);
	update->health=htons((short)obj->status.health.value);
	update->armor=htons((short)obj->status.armor.value);

	idx=0;
		
	for (n=0;n!=max_weap_list;n++) {
		weap=obj->weap_list.weaps[n];
		if (weap==NULL) continue;

		update->ammos[idx].hidden=htons((short)(weap->hidden?1:0));
		update->ammos[idx].ammo_count=htons((short)weap->ammo.count);
		update->ammos[idx].clip_count=htons((short)weap->ammo.clip_count);
		update->ammos[idx].alt_ammo_count=htons((short)weap->alt_ammo.count);
		update->ammos[idx].alt_clip_count=htons((short)weap->alt_ammo.clip_count);

		idx++;
		if (idx==net_max_weapon_per_remote) break;
	}

		// last mesh
		
	update->last_stand_mesh_idx=htons((short)obj->mesh.last_stand_mesh_idx);
}

void remote_update_unpack(obj_type *obj,network_request_remote_update *update)
{
	int								n,idx,flags,old_score,
									animation_mode,animate_idx,animate_next_idx;
	d3pnt							org_pnt;
	weapon_type						*weap;
	model_draw						*draw;
	model_draw_dynamic_bone			*dyn_bone;
	model_draw_animation			*animation;
	network_request_dynamic_bone	*net_dyn_bone;
	network_request_animation		*net_animation;
	
	draw=&obj->draw;
	
		// update position

	memmove(&org_pnt,&obj->pnt,sizeof(d3pnt));
	
	obj->pnt.x=ntohl(update->pnt_x);
	obj->pnt.y=ntohl(update->pnt_y);
	obj->pnt.z=ntohl(update->pnt_z);
				
	obj->ang.x=ntohf(update->fp_ang_x);
	obj->ang.y=ntohf(update->fp_ang_y);
	obj->ang.z=ntohf(update->fp_ang_z);
	
	obj->face.ang.x=ntohf(update->fp_face_ang_x);
	obj->face.ang.y=ntohf(update->fp_face_ang_y);
	obj->face.ang.z=ntohf(update->fp_face_ang_z);
	
	draw->offset.x=(signed short)ntohs(update->offset_x);
	draw->offset.y=(signed short)ntohs(update->offset_y);
	draw->offset.z=(signed short)ntohs(update->offset_z);

		// update predicition values
		
	obj->remote.predict.move.x=ntohl(update->fp_predict_move_x);
	obj->remote.predict.move.y=ntohl(update->fp_predict_move_y);
	obj->remote.predict.move.z=ntohl(update->fp_predict_move_z);

	obj->turn.fix_ang_add.x=obj->turn.fix_ang_add.z=0.0f;
	obj->remote.predict.turn_y=ntohf(update->fp_predict_turn_y);
	
		// update animations
		// only change animations if mode, animation, or next
		// animation are changing
		
	animation=draw->animations;
	net_animation=update->animation;

	for (n=0;n!=max_model_blend_animation;n++) {
	
		animation_mode=(signed short)ntohs(net_animation->model_mode);
		animate_idx=(signed short)ntohs(net_animation->model_animate_idx);
		animate_next_idx=(signed short)ntohs(net_animation->model_animate_next_idx);
		
		if ((animation_mode!=animation->mode) || (animation->animate_idx!=animate_idx) || (animation->animate_next_idx!=animate_next_idx)) {
			animation->tick=ntohl(net_animation->model_tick)+game_time_get();
			animation->mode=animation_mode;
			animation->animate_idx=animate_idx;
			animation->animate_next_idx=animate_next_idx;
			animation->pose_move_idx=(signed short)ntohs(net_animation->model_pose_move_idx);
			animation->smooth_animate_idx=(signed short)ntohs(net_animation->model_smooth_animate_idx);
			animation->smooth_pose_move_idx=(signed short)ntohs(net_animation->model_smooth_pose_move_idx);
		}
		
		animation++;
		net_animation++;
	}			
		
	draw->mesh_mask=ntohl(update->model_mesh_mask);
	
	for (n=0;n!=max_model_texture;n++) {
		draw->textures[n].frame=(int)update->model_cur_texture_frame[n];
	}

		// dynamic bones

	dyn_bone=draw->dynamic_bones;
	net_dyn_bone=update->dynamic_bones;

	for (n=0;n!=max_model_dynamic_bone;n++) {

		dyn_bone->bone_idx=(signed short)ntohs(net_dyn_bone->bone_idx);

		if (dyn_bone->bone_idx!=-1) {
			dyn_bone->mov.x=ntohf(net_dyn_bone->fp_mov_x);
			dyn_bone->mov.y=ntohf(net_dyn_bone->fp_mov_y);
			dyn_bone->mov.z=ntohf(net_dyn_bone->fp_mov_z);
			dyn_bone->rot.x=ntohf(net_dyn_bone->fp_rot_x);
			dyn_bone->rot.y=ntohf(net_dyn_bone->fp_rot_y);
			dyn_bone->rot.z=ntohf(net_dyn_bone->fp_rot_z);
			dyn_bone->resize=ntohf(net_dyn_bone->fp_resize);
		}

		dyn_bone++;
		net_dyn_bone++;
	}
	
		// update flags
	
	flags=ntohl(update->flags);
		
	obj->hidden=((flags&net_update_flag_hidden)!=0);
	obj->contact.object_on=!((flags&net_update_flag_no_contact_object)!=0);
	obj->contact.projectile_on=!((flags&net_update_flag_no_contact_projectile)!=0);
	obj->contact.force_on=!((flags&net_update_flag_no_contact_force)!=0);
	obj->click.on=((flags&net_update_flag_clickable)!=0);
	obj->pickup.on=((flags&net_update_flag_pickup)!=0);
	obj->remote.talking=((flags&net_update_flag_talking)!=0);
	
		// update status
		
	old_score=obj->score.score;
	
	obj->score.score=(signed short)ntohs(update->score);
	obj->status.health.value=(signed short)ntohs(update->health);
	obj->status.armor.value=(signed short)ntohs(update->armor);
	
	idx=0;
		
	for (n=0;n!=max_weap_list;n++) {
		weap=obj->weap_list.weaps[n];
		if (weap==NULL) continue;

		weap->hidden=(ntohs(update->ammos[idx].hidden)!=0);
		weap->ammo.count=(signed short)ntohs(update->ammos[idx].ammo_count);
		weap->ammo.clip_count=(signed short)ntohs(update->ammos[idx].clip_count);
		weap->alt_ammo.count=(signed short)ntohs(update->ammos[idx].alt_ammo_count);
		weap->alt_ammo.clip_count=(signed short)ntohs(update->ammos[idx].alt_clip_count);

		idx++;
		if (idx==net_max_weapon_per_remote) break;
	}

		// last stand mesh
		
	obj->mesh.last_stand_mesh_idx=(signed short)ntohs(update->last_stand_mesh_idx);
	
		// last update tick
		
	obj->remote.last_update=game_time_get();
}

void remote_update(network_request_remote_update *update)
{
	int						net_uid;
	obj_type				*obj;
	
		// get object

	net_uid=(signed short)ntohs(update->update_net_uid);
	
	obj=object_find_remote_net_uid(net_uid);
	if (obj==NULL) return;

		// update remote

	remote_update_unpack(obj,update);
	
		// handle pickups and
		// triggers
		
	item_pickup_check(obj);
	mesh_triggers(obj);
}

/* =======================================================

      Remote Vehicles
      
======================================================= */

void remote_vehicle(int net_uid,network_request_remote_update *update)
{

	// supergumba -- finish for vehicle support
/*	
		// check for vehicles
		
	map_spawn_idx=(signed short)ntohs(update->vehicle_map_spawn_idx);
	if (map_spawn_idx!=-1) {
		
			// hide original object
			
		obj->hidden=TRUE;
		obj->contact.object_on=FALSE;
		obj->contact.projectile_on=FALSE;
		obj->contact.force_on=FALSE;
		
			// switch to vehicle
			
		obj=object_find_spawn_idx(map_spawn_idx);
		if (obj==NULL) return;
	}
	*/
}

/* =======================================================

      Remote Chats and Sounds
      
======================================================= */

void remote_chat(network_request_remote_chat *chat)
{
	d3col				col;
	obj_type			*obj;
	
	obj=object_find_remote_net_uid((signed short)ntohs(chat->chat_net_uid));
	if (obj==NULL) return;
	
		// update chat

	object_get_tint(obj,&col);
	chat_add_message(obj,chat->str,&col);
}

void remote_sound(network_request_remote_sound *sound)
{
	int					buffer_idx;
	float				pitch;
	d3pnt				pnt;
	
	if (app.dedicated_host) return;
	
	pnt.x=ntohl(sound->pnt_x);
	pnt.y=ntohl(sound->pnt_y);
	pnt.z=ntohl(sound->pnt_z);
	
	pitch=ntohf(sound->fp_pitch);
	
	buffer_idx=al_find_buffer(sound->name);
	if (buffer_idx!=-1) al_play_source(buffer_idx,&pnt,pitch,FALSE,FALSE,FALSE,FALSE);
}

/* =======================================================

      Remote Projectiles, Hit Scans, and Melees
      
======================================================= */

void remote_projectile_add(obj_type *obj,network_request_remote_fire *proj_add)
{
	d3pnt				pt;
	d3ang				ang;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	
	weap=weapon_find_name(obj,proj_add->weap_name);
	if (weap==NULL) return;
	
	proj_setup=find_proj_setups(weap,proj_add->proj_setup_name);
	if (proj_setup==NULL) return;

	pt.x=ntohl(proj_add->pt_x);
	pt.y=ntohl(proj_add->pt_y);
	pt.z=ntohl(proj_add->pt_z);
	
	ang.x=ntohf(proj_add->fp_ang_x);
	ang.y=ntohf(proj_add->fp_ang_y);
	ang.z=ntohf(proj_add->fp_ang_z);

	weapon_add_projectile(obj,weap,proj_setup,&pt,&ang);
}

void remote_hit_scan_add(obj_type *obj,network_request_remote_fire *hitscan_add)
{
	d3pnt				pt;
	d3ang				ang;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	
	weap=weapon_find_name(obj,hitscan_add->weap_name);
	if (weap==NULL) return;
	
	proj_setup=find_proj_setups(weap,hitscan_add->proj_setup_name);
	if (proj_setup==NULL) return;
				
	pt.x=ntohl(hitscan_add->pt_x);
	pt.y=ntohl(hitscan_add->pt_y);
	pt.z=ntohl(hitscan_add->pt_z);
	
	ang.x=ntohf(hitscan_add->fp_ang_x);
	ang.y=ntohf(hitscan_add->fp_ang_y);
	ang.z=ntohf(hitscan_add->fp_ang_z);
	
	projectile_hitscan(obj,weap,proj_setup,&pt,&ang);
}

void remote_melee_add(obj_type *obj,network_request_remote_fire *rem_melee)
{
	d3pnt				pt;
	d3ang				ang;
	weapon_type			*weap;
	melee_type			melee;
	
	weap=NULL;
	if (rem_melee->weap_name[0]!=0x0) weap=weapon_find_name(obj,rem_melee->weap_name);
	
	pt.x=ntohl(rem_melee->pt_x);
	pt.y=ntohl(rem_melee->pt_y);
	pt.z=ntohl(rem_melee->pt_z);
	
	ang.x=ntohf(rem_melee->fp_ang_x);
	ang.y=ntohf(rem_melee->fp_ang_y);
	ang.z=ntohf(rem_melee->fp_ang_z);
	
	melee.radius=(int)ntohs(rem_melee->radius);
	melee.distance=(int)ntohs(rem_melee->distance);
	melee.damage=(int)ntohs(rem_melee->damage);
	melee.force=(int)ntohs(rem_melee->force);
	
	melee_add(obj,weap,&pt,&ang,&melee,obj->idx);
}

void remote_fire(network_request_remote_fire *fire)
{
	int				fire_type;
	obj_type		*obj;

	obj=object_find_remote_net_uid((signed short)ntohs(fire->fire_net_uid));
	if (obj==NULL) return;
	
	fire_type=(int)ntohs(fire->fire_type);

	switch (fire_type) {

		case net_remote_fire_type_projectile:
			remote_projectile_add(obj,fire);
			break;

		case net_remote_fire_type_hit_scan:
			remote_hit_scan_add(obj,fire);
			break;

		case net_remote_fire_type_melee:
			remote_melee_add(obj,fire);
			break;
	}
}

/* =======================================================

      Remote Pickup
      
======================================================= */

void remote_pickup(network_request_remote_pickup *pickup)
{
	obj_type		*obj,*item_obj;
	
		// setup pickup

	obj=object_find_remote_net_uid((signed short)ntohs(pickup->picking_net_uid));
	if (obj==NULL) return;

	item_obj=object_find_remote_net_uid((signed short)ntohs(pickup->picked_net_uid));
	if (item_obj==NULL) return;

		// run the pickup
		
	item_pickup(obj,item_obj);
}

/* =======================================================

      Remote Group Synching
      
======================================================= */

void remote_state_group_synch(network_reply_group_synch *group_synch)
{
		// group synches are used to
		// read latency, latency is half of
		// a round trip

	net_setup.client.latency=(game_time_get()-net_setup.client.latency_ping_tick)>>1;

		// run groups synch

	group_moves_synch_with_host(group_synch);
}

/* =======================================================

      Remote Networking Receives
      
======================================================= */

bool remote_route_message(net_queue_msg_type *msg)
{
	switch (msg->action) {
	
		case net_action_request_game_reset:
			remote_game_reset((network_request_game_reset*)msg->msg);
			return(TRUE);
			
		case net_action_request_remote_add:
			remote_add((network_request_remote_add*)msg->msg);
			return(TRUE);
			
		case net_action_request_remote_remove:
			remote_remove((network_request_remote_remove*)msg->msg);
			return(TRUE);
			
		case net_action_request_remote_death:
			remote_death((network_request_remote_death*)msg->msg);
			return(TRUE);
		
		case net_action_request_remote_update:
			remote_update((network_request_remote_update*)msg->msg);
			return(TRUE);
			
		case net_action_request_remote_chat:
			remote_chat((network_request_remote_chat*)msg->msg);
			return(TRUE);
			
		case net_action_request_remote_sound:
			remote_sound((network_request_remote_sound*)msg->msg);
			return(TRUE);

		case net_action_request_remote_fire:
			remote_fire((network_request_remote_fire*)msg->msg);
			return(TRUE);

		case net_action_request_remote_pickup:
			remote_pickup((network_request_remote_pickup*)msg->msg);
			return(TRUE);

		case net_action_request_host_exit:
			remote_host_exit();
			return(FALSE);				// break out of all the loops and exit client game

		case net_action_reply_group_synch:
			remote_state_group_synch((network_reply_group_synch*)msg->msg);
			return(TRUE);
			
		case net_action_request_game_score_limit:
			server.next_state=gs_score_limit;
			return(TRUE);
			
	}
	
	return(TRUE);
}

/* =======================================================

      Remote Networking Updates and Synchs
      
======================================================= */

void remote_network_send_updates(void)
{
	int					tick;
	obj_type			*obj;

		// time for an update

	tick=game_time_get();
	if (tick<server.time.network_update_tick) return;

	server.time.network_update_tick=tick+client_communication_update_msec_rate;

		// update the player

	obj=server.obj_list.objs[server.player_obj_idx];
	net_client_send_remote_update(obj);
}

void remote_network_send_group_synch(void)
{
	int				tick;

		// time for group synch?

	tick=game_time_get();
	if (tick<server.time.network_group_synch_tick) return;

	server.time.network_group_synch_tick=tick+client_communication_group_synch_msec_rate;

		// use group synches to
		// get a read on latency

	net_setup.client.latency_ping_tick=tick;

		// run group synch

	net_client_request_group_synch(server.obj_list.objs[server.player_obj_idx]);
}
