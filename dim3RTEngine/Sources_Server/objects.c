/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Creation/Dispose

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

extern map_type				map;
extern server_type			server;
extern iface_type			iface;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

/* =======================================================

      Initialize Object List
      
======================================================= */

void object_initialize_list(void)
{
	int				n;

		// clear all objects

	for (n=0;n!=max_obj_list;n++) {
		server.obj_list.objs[n]=NULL;
	}
}

void object_free_list(void)
{
	int				n;

	for (n=0;n!=max_obj_list;n++) {
		if (server.obj_list.objs[n]!=NULL) free(server.obj_list.objs[n]);
	}
}

int object_count_list(void)
{
	int				n,count;

	count=0;

	for (n=0;n!=max_obj_list;n++) {
		if (server.obj_list.objs[n]!=NULL) count++;
	}

	return(count);
}

int object_count_weapons(obj_type *obj)
{
	int				n,count;

	count=0;

	for (n=0;n!=max_weap_list;n++) {
		if (obj->weap_list.weaps[n]!=NULL) count++;
	}

	return(count);
}

/* =======================================================

      Clear Object Structures
      
======================================================= */

void object_clear_size(obj_size *size)
{
	size->x=size->z=1500;
	size->y=3000;
	size->eye_offset=-2500;
	size->weight=200;

	size->node_touch_dist=500;
}

void object_clear_position(d3pnt *pnt)
{
	pnt->x=pnt->y=pnt->z=0;
}

void object_clear_angle(d3ang *ang)
{
	ang->x=ang->y=ang->z=0;
}

void object_clear_motion(obj_motion *motion)
{
	motion->last_y_change=0;
	motion->rigid_body_offset_y=0;
	motion->vct.x=motion->vct.z=motion->vct.y=0;
	motion->ang.y=motion->ang.x=motion->ang.z=0;
}

void object_clear_movement(obj_movement *move)
{
    move->speed=0;
    move->max_crawl_speed=move->max_walk_speed=move->max_run_speed=move->max_air_speed=0;
    move->accelerate=move->decelerate=move->air_accelerate=move->air_decelerate=0;
	move->slop=3.0f;
	move->seek_ang=0.0f;
	move->running=FALSE;
    move->moving=FALSE;
    move->reverse=FALSE;
	move->seeking=FALSE;
}

void object_clear_force(obj_force *force)
{
	force->vct.x=force->vct.z=force->vct.y=0.0f;
    force->gravity=gravity_start_power;
}

void object_clear_contact(obj_contact *contact)
{
	contact->hit_poly.mesh_idx=-1;
	contact->stand_poly.mesh_idx=-1;
	contact->head_poly.mesh_idx=-1;

	contact->liquid_idx=-1;
	
	contact->obj_idx=-1;
	contact->proj_idx=-1;
	contact->stand_obj_idx=-1;
	
	contact->melee=FALSE;
}

void object_clear_touch(obj_touch *touch)
{
	touch->obj_idx=-1;
	touch->last_obj_idx=-1;
	
	touch->stand=FALSE;
	
	touch->last_touch_tick=0;
	touch->last_stand=FALSE;
	
	touch->pnt.x=touch->pnt.y=touch->pnt.z=0;
	touch->ang.x=touch->ang.y=touch->ang.z=0.0f;
}

void object_clear_hit(obj_hit *hit)
{
	hit->obj_idx=-1;
	hit->weap_idx=-1;
	hit->proj_setup_idx=-1;
	
	hit->pnt.x=hit->pnt.y=hit->pnt.z=0;
	hit->ang.x=hit->ang.y=hit->ang.z=0.0f;
	
	hit->damage=0;
}

void object_clear_pickup(obj_pickup *pickup)
{
	pickup->obj_idx=-1;
	pickup->item_idx=-1;
}

void object_clear_remote(obj_remote *remote)
{
	remote->net_uid=-1;
}

void object_clear_watch(obj_watch *watch)
{
	watch->on=FALSE;

	watch->obj_idx=-1;
	watch->dist=8000;

	watch->watch_restrict.on=FALSE;
	watch->watch_restrict.ray_trace=FALSE;
	watch->watch_restrict.ang=0.0f;

	memset(watch->obj_flags,0x0,max_obj_list);
}

void object_clear_draw(model_draw *draw)
{
	int				k;
	
		// model draw
		
	draw->on=FALSE;
	draw->model_idx=-1;
	draw->name[0]=0x0;
	draw->bounce=FALSE;
	draw->face_forward=FALSE;
	draw->offset.x=draw->offset.z=draw->offset.y=0;
	draw->rot.x=draw->rot.z=draw->rot.y=0.0f;
	draw->spin.x=draw->spin.z=draw->spin.y=0.0f;
	draw->spin_tick=0;
	draw->alpha=1.0f;
	draw->resize=1.0f;
	draw->flip_x=FALSE;

		// dynamic bones

	for (k=0;k!=max_model_dynamic_bone;k++) {
		draw->dynamic_bones[k].bone_idx=-1;
	}
	
		// lights
		
	draw->script_light_idx=0;

	for (k=0;k!=max_model_light;k++) {
		draw->lights[k].on=FALSE;
		draw->lights[k].type=lt_normal;
		draw->lights[k].direction=ld_all;
		draw->lights[k].intensity=0;
		draw->lights[k].exponent=1.0f;
		draw->lights[k].col.r=draw->lights[k].col.g=draw->lights[k].col.b=1;
	}
	
		// halos
		
	draw->script_halo_idx=0;

	for (k=0;k!=max_model_halo;k++) {
		draw->halos[k].on=FALSE;
	}

		// default normal

	draw->normal[0]=0.5f;
	draw->normal[1]=0.5f;
	draw->normal[2]=1.0f;
	
		// animate setup
		
	for (k=0;k!=max_model_blend_animation;k++) {
		draw->animations[k].mode=am_stopped;
		draw->setup.poses[k].idx_1=draw->setup.poses[k].idx_2=-1;
	}

		// mesh and textures

	draw->mesh_mask=0x1;				// default to first mesh
	
	for (k=0;k!=max_model_texture;k++) {
		draw->textures[k].animation_on=FALSE;
		draw->textures[k].animation_reverse=FALSE;
		draw->textures[k].frame=0;
	}

		// mesh fades

	for (k=0;k!=max_model_mesh;k++) {
		draw->meshes[k].fade.on=FALSE;
	}
}

/* =======================================================

      Object Radius
      
======================================================= */

int object_get_radius(obj_type *obj)
{
	int			radius;
	
	radius=obj->size.x;
	if (obj->size.z>radius) radius=obj->size.z;
	
	return(radius>>1);
}

/* =======================================================

      Object Positions
      
======================================================= */

void object_set_current_mesh(obj_type *obj)
{
	obj->mesh.last_stand_mesh_idx=-1;
	obj->contact.stand_poly.mesh_idx=-1;
}

void object_set_position(obj_type *obj,int x,int y,int z,float ang_y,float ymove)
{
	d3pnt				*pnt;
	d3ang				*ang;
	obj_motion			*motion;
	
	pnt=&obj->pnt;
	
	pnt->x=x;
	pnt->y=y;
	pnt->z=z;
	
	memmove(&obj->last_pnt,pnt,sizeof(d3pnt));
	memmove(&obj->draw.pnt,pnt,sizeof(d3pnt));
	
	ang=&obj->ang;
	ang->x=0;
	ang->y=ang_y;
	ang->z=0;
	
	memmove(&obj->last_ang,ang,sizeof(d3ang));
	
	ang=&obj->view_ang;
	ang->x=0;
	ang->y=0;
	ang->z=0;
	
	motion=&obj->motion;
	
	motion->last_y_change=0;
	motion->ang.y=ang_y;
	motion->vct.y=ymove;
	
	obj->turn.ang_to.y=ang_y;
	
	obj->force.gravity=gravity_start_power;

	object_set_current_mesh(obj);
}

void object_stop(obj_type *obj)
{
    obj->forward_move.running=FALSE;
	
	object_clear_angle(&obj->turn.ang_add);
	object_clear_angle(&obj->turn.ang_to);
	object_clear_angle(&obj->turn.fix_ang_add);
    obj->look.ang_add=0;

	obj->face.obj_idx=-1;
	object_clear_angle(&obj->face.ang);
	
	obj->forward_move.moving=FALSE;
	obj->side_move.moving=FALSE;
	obj->vert_move.moving=FALSE;
	
	obj->forward_move.speed=0;
	obj->side_move.speed=0;
	obj->vert_move.speed=0;

	obj->motion.last_y_change=0;
	
	obj->force.vct.x=obj->force.vct.z=obj->force.vct.y=0;
	
	obj->auto_walk.mode=aw_none;
	obj->auto_walk.node_seek_idx=obj->auto_walk.node_dest_idx=-1;
	obj->auto_walk.dodge.on=FALSE;
	
	obj->last_move_animation_event=-1;
	obj->last_turn_animation_event=-1;
	
	obj->thrust.vct.x=obj->thrust.vct.y=obj->thrust.vct.z=0.0f;
}   

/* =======================================================

      Object Team Colors
      
======================================================= */

void object_team_get_name(int team_idx,char *str)
{
	if (team_idx==net_team_red) {
		strcpy(str,"Red");
	}
	else {
		strcpy(str,"Blue");
	}
}

void object_team_get_tint(int team_idx,d3col *tint)
{
	if (team_idx==net_team_red) {
		tint->r=1.0f;
		tint->g=0.0f;
		tint->b=0.0f;
	}
	else {
		tint->r=0.0f;
		tint->g=0.0f;
		tint->b=1.0f;
	}
}

void object_get_tint(obj_type *obj,d3col *tint)
{
		// if in multiplayer and using teams,
		// then get team tint

	if (net_setup.mode!=net_mode_none) {
		if (iface.multiplayer.game_list.games[net_setup.game_idx].use_teams) {
			object_team_get_tint(obj->team_idx,tint);
			return;
		}
	}

		// otherwise use white

	tint->r=tint->g=tint->b=1.0f;
}

/* =======================================================

      Check if Object Sends Network Messages
      
======================================================= */

bool object_networkable(obj_type *obj)
{
		// players and bots always send messages

	if ((obj->idx==server.player_obj_idx) || (obj->type==object_type_bot_multiplayer)) return(TRUE);
	
		// map type bots only send messages if
		// this process is the host

	if (obj->type==object_type_bot_map) {
		return(net_setup.mode==net_mode_host);
	}
	
	return(FALSE);
}

/* =======================================================

      Create a New Object
      
======================================================= */

int object_create(char *name,int type,int bind)
{
	int				n,idx;
	obj_type		*obj;

		// find a unused object

	idx=-1;

	for (n=0;n!=max_obj_list;n++) {
		if (server.obj_list.objs[n]==NULL) {
			idx=n;
			break;
		}
	}

	if (idx==-1) return(-1);

		// create memory for new object

	obj=(obj_type*)malloc(sizeof(obj_type));
	if (obj==NULL) return(-1);

		// put in list

	server.obj_list.objs[idx]=obj;

		// initial setup
		
	strcpy(obj->name,name);

	obj->idx=idx;
	obj->type=type;
	obj->bind=bind;
	
	obj->next_spawn_sub_event=sd_event_spawn_init;

		// initialize object
	
	obj->hidden=FALSE;
	obj->suspend=FALSE;
	obj->scenery.on=FALSE;
	obj->fly=FALSE;
	obj->single_speed=FALSE;
	obj->side_step=TRUE;
	obj->floating=TRUE;
	obj->quick_reverse=TRUE;
	obj->no_slide=FALSE;
	obj->crawl=TRUE;
	obj->open_doors=FALSE;
	obj->hide_all_weapons=FALSE;
	obj->find_on=TRUE;
	obj->hit_box.on=FALSE;

	obj->contact.object_on=TRUE;
	obj->contact.projectile_on=TRUE;
	obj->contact.force_on=TRUE;
	obj->contact.pushable=FALSE;
	obj->contact.collision_mode=collision_mode_cylinder;
	
	obj->pickup.on=FALSE;
	obj->pickup.ignore=FALSE;
	
	obj->damage.on=FALSE;
	obj->damage.crushable=FALSE;
	obj->damage.invincible=FALSE;
	obj->damage.in_damage=FALSE;
	
	obj->tint_color_idx=0;
	obj->character_idx=0;
	
	obj->input.mode=im_fpp;
	obj->input.freeze=FALSE;
	obj->input.respawn_freeze=FALSE;
	
	obj->in_collide_event=FALSE;
	obj->death_trigger=FALSE;

	obj->last_spawn_spot_idx=-1;
	
	obj->lock.x=obj->lock.y=obj->lock.z=FALSE;
	
	obj->damage_obj_idx=-1;
	
	obj->team_idx=net_team_none;

	obj->spawn_spot.name[0]=0x0;
	obj->spawn_spot.script[0]=0x0;
	obj->spawn_spot.params[0]=0x0;
	
	obj->click.on=FALSE;
	obj->click.distance=1500;
	obj->click.crosshair_up_idx=-1;
	obj->click.crosshair_down_idx=-1;
	
	object_clear_size(&obj->size);
	object_clear_position(&obj->pnt);
	object_clear_angle(&obj->ang);
	object_clear_motion(&obj->motion);
	object_clear_force(&obj->force);
	object_clear_movement(&obj->forward_move);
    object_clear_movement(&obj->side_move);
	object_clear_movement(&obj->vert_move);
	object_stop(obj);

	obj->sight.side_angle=20.0f;
	obj->sight.look_angle=0.0f;
	obj->sight.distance=22000;
	
    obj->bump.on=TRUE;
    obj->bump.high=450;
	obj->bump.smooth_factor=0.1f;
	obj->bump.smooth_offset=0;

	obj->slope_gravity=TRUE;
	
	obj->bounce.mesh_idx=-1;
	obj->bounce.factor=0.0f;
	
	obj->turn.ignore_mouse=FALSE;
	obj->turn.only_when_moving=FALSE;
	obj->turn.restrict_player_turning=FALSE;

	obj->face.obj_idx=-1;
	object_clear_angle(&obj->face.ang);
	
	obj->thrust.speed=0.5f;
	obj->thrust.max_speed=60.0f;
	obj->thrust.vct.x=obj->thrust.vct.y=obj->thrust.vct.z=0.0f;
	obj->thrust.drag=FALSE;
	
	obj->look.ang_add=0;
	obj->look.up_angle=80;
	obj->look.down_angle=35;
	obj->look.effect_weapons=TRUE;
	
	obj->duck.on=TRUE;
    obj->duck.mode=dm_stand;
    obj->duck.y_move=0;
    obj->duck.y_add=16;
    obj->duck.y_change=300;
	
	obj->jump.on=TRUE;
    obj->jump.y_add=32;
	
	obj->climb.on=FALSE;
	obj->climb.poly_ptr.mesh_idx=-1;
	
	obj->kickback.size=100;

	obj->vehicle.on=FALSE;
	obj->vehicle.use_vehicles=TRUE;
	obj->vehicle.in_enter=obj->vehicle.in_exit=FALSE;
	obj->vehicle.attach_obj_idx=-1;

	obj->radar.on=FALSE;
	obj->radar.icon[0]=0x0;
	obj->radar.icon_idx=-1;
	obj->radar.motion_only=FALSE;
	obj->radar.always_visible=FALSE;
	obj->radar.fade_start_tick=0;
	
	obj->status.health.value=obj->status.health.start_value=obj->status.health.max_value=100;
	obj->status.health.recover_tick=obj->status.health.recover_count=0;
	obj->status.health.recover_amount=1;
	obj->status.health.factor=1.0f;
	obj->status.armor.value=obj->status.armor.start_value=obj->status.armor.max_value=0;
	obj->status.armor.recover_tick=obj->status.armor.recover_count=0;
	obj->status.armor.recover_amount=1;
	obj->status.armor.factor=1.0f;
	obj->status.mesh_harm_count=0;
	
	object_clear_draw(&obj->draw);
	obj->ambient.on=FALSE;
    
    obj->held_weapon.mode=wm_held;
    obj->held_weapon.swap_tick=0;
	obj->held_weapon.current_idx=obj->held_weapon.next_idx=-1;
	obj->held_weapon.bounce_y=0;
	
	obj->crosshair_draw.on=FALSE;
	obj->crosshair_draw.alt_tick=0;

	obj->melee.strike_bone_name[0]=0x0;
	obj->melee.strike_pose_name[0]=0x0;
	obj->melee.object_strike_bone_name[0]=0x0;
	obj->melee.object_strike_pose_name[0]=0x0;
	obj->melee.radius=0;
	obj->melee.distance=0;
	obj->melee.damage=0;
	obj->melee.force=0;
	obj->melee.fall_off=TRUE;
	
	obj->hit_box.obj_hit_box_idx=-1;
	obj->hit_box.proj_hit_box_idx=-1;
	
	obj->count=0;
	obj->item_count=0;

	obj->turn.ang_add.y=0;
	obj->turn.fix_ang_add.y=0;
	obj->turn.ang_to.y=0;
	obj->turn.walk_speed=obj->turn.run_speed=obj->turn.crawl_speed=obj->turn.air_speed=1;
	obj->turn.walk_motion_speed=obj->turn.run_motion_speed=obj->turn.crawl_motion_speed=obj->turn.air_motion_speed=1;
	obj->turn.key_speed=1.8f;
	obj->turn.top_down_ang_offset=0.0f;

	obj->air_mode=am_ground;

    obj->liquid.mode=lm_out;
	obj->liquid.bob_y_move=0;

	obj->fall.dist=0;
	obj->fall.damage_minimum_height=3000;
	obj->fall.damage_factor=0.0f;
    obj->fall.started=FALSE;
	obj->fall.land_event_ok=FALSE;
	
	obj->last_move_animation_event=-1;
	obj->last_turn_animation_event=-1;

	obj->auto_walk.mode=aw_none;
	obj->auto_walk.dodge.on=FALSE;

	obj->label.text.str[0]=0x0;
	obj->label.bitmap.idx=-1;
	obj->label.bar.value=-1.0f;

	obj->debug.str[0]=0x0;
	
	object_clear_contact(&obj->contact);
	object_clear_touch(&obj->touch);
	object_clear_hit(&obj->hit);
	object_clear_pickup(&obj->pickup);
	object_clear_watch(&obj->watch);

	object_clear_remote(&obj->remote);

	object_grow_clear(obj);
	
	obj->score.kill=obj->score.death=obj->score.suicide=obj->score.goal=obj->score.score=0;
	obj->score.place=1;
	
		// clear weapons

	for (n=0;n!=max_weap_list;n++) {
		obj->weap_list.weaps[n]=NULL;
	}
	
		// connections for animated effects
		
	obj->draw.connect.obj_idx=idx;
	obj->draw.connect.weap_idx=-1;
	obj->draw.connect.proj_idx=-1;
	obj->draw.connect.net_sound=FALSE;
	obj->draw.connect.motion_vct.x=0.0f;
	obj->draw.connect.motion_vct.y=0.0f;
	obj->draw.connect.motion_vct.z=0.0f;
	
	return(idx);
}

/* =======================================================

      Scripts for Objects
      
======================================================= */

bool object_start_script(obj_type *obj,bool no_construct,char *err_str)
{
	char				script_name[file_str_len];

		// is it a non-script scenery?
		// this is usually something set when re-loading
		// state from a saved file

	if (obj->scenery.on) {
		obj->script_idx=-1;
		return(TRUE);
	}

		// if player, use player script
		// or overridden multiplayer script,
		// otherwise use script setup by spot
	
	script_name[0]=0x0;

	if (obj->type==object_type_player) {
		if (net_setup.mode!=net_mode_none) strcpy(script_name,iface.multiplayer.game_list.games[net_setup.game_idx].script.player_script);
		if (script_name[0]==0x0) strcpy(script_name,"Player");
	}
	else {
		strcpy(script_name,obj->spawn_spot.script);
	}
	
		// create the script

	obj->script_idx=scripts_add(thing_type_object,"Objects",script_name,obj->idx,-1,-1,err_str);
	if (obj->script_idx==-1) return(FALSE);
			
		// send the construct event
	
	if (no_construct) return(TRUE);

	return(scripts_post_event(obj->script_idx,-1,sd_event_construct,0,0,err_str));
}

/* =======================================================

      Reset Object to Original Spawn Spot
      
======================================================= */

void object_reset_prepare(obj_type *obj)
{
	memmove(&obj->org_pnt,&obj->pnt,sizeof(d3pnt));
	memmove(&obj->org_ang,&obj->ang,sizeof(d3ang));
}

void object_reset(obj_type *obj)
{
	object_stop(obj);
	
	memmove(&obj->pnt,&obj->org_pnt,sizeof(d3pnt));	
	memmove(&obj->ang,&obj->org_ang,sizeof(d3ang));
	
	memmove(&obj->last_pnt,&obj->pnt,sizeof(d3pnt));
	memmove(&obj->last_ang,&obj->ang,sizeof(d3ang));
	
	memmove(&obj->draw.pnt,&obj->pnt,sizeof(d3pnt));

	obj->motion.ang.x=obj->motion.ang.z=0.0f;
	obj->motion.ang.y=obj->turn.ang_to.y=obj->ang.y;
	obj->motion.last_y_change=0;
	obj->motion.vct.x=obj->motion.vct.y=obj->motion.vct.z=0.0f;
	
	obj->force.gravity=gravity_start_power;
	
	object_set_current_mesh(obj);
}

/* =======================================================

      Attaching Crosshairs
      
======================================================= */

void object_attach_click_crosshair_up(obj_type *obj)
{
	obj->click.crosshair_up_idx=iface_crosshair_find(&iface,obj->click.crosshair_up_name);
}

void object_attach_click_crosshair_down(obj_type *obj)
{
	obj->click.crosshair_down_idx=iface_crosshair_find(&iface,obj->click.crosshair_down_name);
}

/* =======================================================

      Multiplayer Teams, Spawns, and Textures
      
======================================================= */

void object_multiplayer_setup(obj_type *obj)
{
	iface_mp_game_type			*mp_game;

	if (net_setup.mode==net_mode_none) return;
	if ((obj->type!=object_type_player) && (obj->type!=object_type_remote_player) && (obj->type!=object_type_bot_multiplayer)) return;

		// team setup

	mp_game=&iface.multiplayer.game_list.games[net_setup.game_idx];

	obj->team_idx=net_team_none;

	if (mp_game->use_teams) object_set_even_team(obj);

		// spawn spot

	obj->spawn_spot.name[0]=0x0;

	if ((mp_game->spawn.force_team_spot) && (obj->team_idx!=net_team_none)) {
		if (obj->team_idx==net_team_red) {
			strcpy(obj->spawn_spot.name,"Red");
		}
		else {
			strcpy(obj->spawn_spot.name,"Blue");
		}
	}
	else {
		strcpy(obj->spawn_spot.name,mp_game->spawn.spot_name);
	}
	
	if (obj->spawn_spot.name[0]==0x0) strcpy(obj->spawn_spot.name,"Start");
}

void object_multiplayer_setup_model_team_texture(obj_type *obj)
{
	int					n,frame;
	model_draw			*draw;
	model_type			*mdl;
	texture_type		*texture;

	if (net_setup.mode==net_mode_none) return;
	if ((obj->type!=object_type_player) && (obj->type!=object_type_remote_player) && (obj->type!=object_type_bot_multiplayer)) return;
	if (obj->team_idx==net_team_none) return;

		// get model

	if (obj->draw.model_idx==-1) return;
	mdl=server.model_list.models[obj->draw.model_idx];

	draw=&obj->draw;
	frame=obj->team_idx;

	texture=mdl->textures;

	for (n=0;n!=max_model_texture;n++) {
		if (texture->frames[frame].bitmap.gl_id!=-1) draw->textures[n].frame=frame;
		texture++;
	}
}

/* =======================================================

      Start Objects
      
======================================================= */

int object_start(spot_type *spot,char *name,int type,int bind,char *err_str)
{
	int					idx;
	obj_type			*obj;
	weapon_type			*weap;
	
		// create object
		
	idx=object_create(name,type,bind);
	if (idx==-1) {
		strcpy(err_str,"Out of memory");
		return(-1);
	}
	
	obj=server.obj_list.objs[idx];

		// player default setup
		
	if (obj->type==object_type_player) {
		obj->team_idx=net_team_none;
		obj->spawn_spot.name[0]=0x0;
		
		obj->hidden=FALSE;
		
		obj->tint_color_idx=setup.network.tint_color_idx;
		obj->character_idx=setup.network.character_idx;

		server.player_obj_idx=obj->idx;
	}

		// regular object setup

	else {

			// if there's an editor display model, then
			// default model to it
			
		if (spot->display_model[0]!=0x0) {
			obj->draw.on=TRUE;
			strcpy(obj->draw.name,spot->display_model);
		}

			// attach object to spot

		object_set_position(obj,spot->pnt.x,spot->pnt.y,spot->pnt.z,spot->ang.y,0);
		obj->turn.ang_to.y=spot->ang.y;
	}

		// parameters

	obj->spawn_spot.script[0]=0x0;
	obj->spawn_spot.params[0]=0x0;
	if (spot!=NULL) {
		strcpy(obj->spawn_spot.script,spot->script);
		strcpy(obj->spawn_spot.params,spot->params);
	}

		// if networked player or multiplayer bot, run rules
		// to pick the team
	
	if (net_setup.mode!=net_mode_none) {
		if ((obj->type==object_type_player) || (obj->type==object_type_bot_multiplayer)) object_multiplayer_setup(obj);
	}

		// start script

	if (!object_start_script(obj,FALSE,err_str)) {
		server.obj_list.objs[idx]=NULL;
		free(obj);
		return(-1);
	}

		// load object model

	if (!model_draw_initialize(&obj->draw,"Object",obj->name,err_str)) {
		server.obj_list.objs[idx]=NULL;
		free(obj);
		return(-1);
	}

		// setup held weapon

	if (obj->held_weapon.current_idx!=-1) {
		weap=weapon_find_current(obj);
		weapon_set(obj,weap);
	}

	return(obj->idx);
}

/* =======================================================

      Dispose Objects
      
======================================================= */

void object_dispose_single(int idx)
{
	int					n;
	obj_type			*obj;

	obj=server.obj_list.objs[idx];

		// dispose weapons

	for (n=0;n!=max_weap_list;n++) {
		weapon_dispose(obj,n);
	}

		// if this is map bound, then
		// remove any particles attached
		// to bones
		
	if (obj->bind==bt_map) effect_object_bone_attach_particle_dispose(idx);

		// clear scripts and models

	scripts_dispose(obj->script_idx);
	model_draw_free(&obj->draw);

		// free and empty from list

	free(obj);
	server.obj_list.objs[idx]=NULL;
}

void object_dispose_2(int bind)
{
	int				n;
	obj_type		*obj;
	
		// delete bound objects

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
	
		if (obj->bind==bind) object_dispose_single(n);
	}
}

void object_dispose_all(void)
{
	int				n;
	obj_type		*obj;

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj!=NULL) object_dispose_single(n);
	}
}

/* =======================================================

      Script Count Objects
      
======================================================= */

int object_script_count(void)
{
	int				n,count;

	count=0;

	for (n=0;n!=max_obj_list;n++) {
		if (server.obj_list.objs[n]!=NULL) count++;
	}

	return(count);
}

/* =======================================================

      Script Object Spawn/Remove
      
======================================================= */

int object_script_spawn(char *name,char *script,char *params,d3pnt *pnt,d3ang *ang,bool hide,char *err_str)
{
	int					idx;
	char				obj_err_str[256];
	spot_type			spot;
	obj_type			*obj;
	
		// create fake spot

	bzero(&spot,sizeof(spot_type));

	strcpy(spot.name,name);
	strcpy(spot.script,script);
	strcpy(spot.params,params);

	memmove(&spot.pnt,pnt,sizeof(d3pnt));
	memmove(&spot.ang,ang,sizeof(d3ang));

		// start object

	idx=object_start(&spot,name,object_type_object,bt_map,obj_err_str);
	if (idx==-1) {
		sprintf(err_str,"Object Spawn Failed: %s",obj_err_str);
		console_add_error(err_str);
		
		return(-1);
	}
	
	obj=server.obj_list.objs[idx];

		// hide object

	if (hide) {
		obj->hidden=TRUE;
		obj->contact.object_on=FALSE;
		obj->contact.projectile_on=FALSE;
		obj->contact.force_on=FALSE;
	}
	
		// force a spawn call
		
	if (!object_spawn(obj,obj_err_str)) {
		sprintf(err_str,"Object Spawn Failed: %s",obj_err_str);
		console_add_error(err_str);
		
		object_dispose_single(idx);
		return(-1);
	}

		// return index

	return(idx);
}

bool object_script_remove(int idx,char *err_str)
{
		// can not dispose player object

	if (idx==server.player_obj_idx) {
		strcpy(err_str,"Can not dispose player object");
		return(FALSE);
	}
	
		// does this object exist?
	
	if (server.obj_list.objs[idx]==NULL) {
		sprintf(err_str,"No object exists with ID: %d",idx);
		return(FALSE);
	}

		// dispose object

	object_dispose_single(idx);

	return(TRUE);
}
