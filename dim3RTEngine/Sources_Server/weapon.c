/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Weapon Creation/Dispose/Find

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

extern server_type			server;
extern iface_type			iface;
extern js_type				js;

/* =======================================================

      Weapon Counts
      
======================================================= */

int weapon_count_projectile_setups(weapon_type *weap)
{
	int				n,count;

	count=0;

	for (n=0;n!=max_proj_setup_list;n++) {
		if (weap->proj_setup_list.proj_setups[n]!=NULL) count++;
	}

	return(count);
}

/* =======================================================

      Find Weapon
      
======================================================= */

weapon_type* weapon_get_attach(JSObjectRef j_obj)
{
	int				script_idx;
	obj_type		*obj;
	script_type		*script;
	
	script_idx=(int)JSObjectGetPrivate(j_obj);
	script=js.script_list.scripts[script_idx];

	obj=server.obj_list.objs[script->attach.obj_idx];
	return(obj->weap_list.weaps[script->attach.weap_idx]);
}

weapon_type* weapon_find_name(obj_type *obj,char *name)
{
	int				n;
	weapon_type		*weap;

	for (n=0;n!=max_weap_list;n++) {
		weap=obj->weap_list.weaps[n];
		if (weap==NULL) continue;
		
		if (strcasecmp(weap->name,name)==0) return(weap);
	}
	
	return(NULL);
}

weapon_type* weapon_find_current(obj_type *obj)
{
	if (obj->held_weapon.current_idx==-1) return(NULL);
	return(obj->weap_list.weaps[obj->held_weapon.current_idx]);
}

/* =======================================================

      Initial Ammo
      
======================================================= */

void weapon_clear_ammo(weap_ammo_type *ammo,bool use)
{
	ammo->use_ammo=use;
	ammo->use_clips=FALSE;
	ammo->init_count=10;
	ammo->max_count=100;
	ammo->init_clip_count=0;
	ammo->max_clip_count=0;
	ammo->last_reload_tick=-1;
}

/* =======================================================

      Start Weapon Script
      
======================================================= */

bool weapon_start_script(obj_type *obj,weapon_type *weap,bool no_construct,char *err_str)
{
		// create script

	weap->script_idx=scripts_add(thing_type_weapon,"Weapons",weap->name,obj->idx,weap->idx,-1,err_str);
	if (weap->script_idx==-1) return(FALSE);
		
		// send the construct event
	
	if (no_construct) return(TRUE);

	return(scripts_post_event(weap->script_idx,-1,sd_event_construct,0,0,err_str));
}

/* =======================================================

      Create and Dispose Weapons
      
======================================================= */

bool weapon_add(obj_type *obj,char *name,char *err_str)
{
	int				n,idx;
	bool			ok;
	weapon_type		*weap;
	
		// find a free weapon
		
	idx=-1;
	
	for (n=0;n!=max_weap_list;n++) {
		if (obj->weap_list.weaps[n]==NULL) {
			idx=n;
			break;
		}
	}
	
	if (idx==-1) {
		strcpy(err_str,"Reached the maximum number of weapons per object");
		return(FALSE);
	}
	
		// create memory for new weapon
		
	weap=(weapon_type*)malloc(sizeof(weapon_type));
	if (weap==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// initialize weapon
	
	weap->idx=idx;
	
	weap->obj_idx=obj->idx;
	
	weap->hidden=FALSE;
	weap->fail_in_liquid=FALSE;
	
	strcpy(weap->name,name);
	weap->ammo.reset=TRUE;

	weapon_clear_ammo(&weap->ammo,TRUE);
	weapon_clear_ammo(&weap->alt_ammo,FALSE);
	
	weap->proj.fire_bone_name[0]=0x0;
	weap->proj.barrel_bone_name[0]=0x0;
	weap->proj.fire_pose_name[0]=0x0;
	weap->proj.object_fire_bone_name[0]=0x0;
	weap->proj.object_fire_pose_name[0]=0x0;
	weap->proj.repeat_on=FALSE;
	weap->proj.repeat_ok=TRUE;
	weap->proj.repeat_tick=1000;
	weap->proj.next_repeat_tick=0;

	weap->target.distance=25000;
	weap->target.on=FALSE;
	weap->target.col.r=weap->target.col.g=0.0f;
	weap->target.col.b=1.0f;
	
	weap->zoom.on=FALSE;
	weap->zoom.mode=zoom_mode_off;
	weap->zoom.fov_min=10.0f;
	weap->zoom.fov_max=30.0f;
	weap->zoom.step_count=3;
	weap->zoom.current_step=0;
	weap->zoom.sway_factor=1.0f;
	weap->zoom.crawl_sway_factor=0.5f;
	weap->zoom.turn_factor=0.3f;
	weap->zoom.crawl_turn_factor=0.1f;
	weap->zoom.look_factor=0.5f;
	weap->zoom.mask_name[0]=0x0;
	weap->zoom.mask_idx=-1;
	weap->zoom.show_weapon=FALSE;
	weap->zoom.tick=500;
	
	weap->crosshair.on=FALSE;
	weap->crosshair.type=ct_center;
	weap->crosshair.min_size=16;
	weap->crosshair.max_size=64;
	weap->crosshair.distance=35000;
	weap->crosshair.col.r=weap->crosshair.col.g=1;
	weap->crosshair.col.b=0;
	weap->crosshair.empty_col.r=weap->crosshair.empty_col.g=weap->crosshair.empty_col.b=0.5;
	weap->crosshair.pickup_col.r=1;
	weap->crosshair.pickup_col.g=weap->crosshair.pickup_col.b=0;
	weap->crosshair.fire_name[0]=0x0;
	weap->crosshair.fire_idx=-1;
	weap->crosshair.bone_name[0]=0x0;
	
	weap->hand.fov_override=0.0f;
	
	weap->hand.shift.x=0;
	weap->hand.shift.y=0;
	weap->hand.shift.z=0;
	weap->hand.ang.x=0;
	weap->hand.ang.y=0;
	weap->hand.ang.z=0;
	
	weap->hand.raise_tick=500;
	weap->hand.lower_tick=500;
	weap->hand.select_shift=300;
	weap->hand.bounce_ang=30.0f;
	weap->hand.bounce_speed=5.0f;
	
	weap->kickback.life_msec=0;
	weap->kickback.size=0;

	weap->recoil.min_ang.x=weap->recoil.min_ang.y=weap->recoil.min_ang.z=0.0f;
	weap->recoil.max_ang.x=weap->recoil.max_ang.y=weap->recoil.max_ang.z=0.0f;
	weap->recoil.reset_ang.x=weap->recoil.reset_ang.y=weap->recoil.reset_ang.z=0.01f;
	weap->recoil.look_reset=0.05f;
	weap->recoil.turn_reset=0.05f;

	weapon_recoil_clear(weap);
	
	weap->melee.strike_bone_name[0]=0x0;
	weap->melee.strike_pose_name[0]=0x0;
	weap->melee.object_strike_bone_name[0]=0x0;
	weap->melee.object_strike_pose_name[0]=0x0;
	weap->melee.radius=0;
	weap->melee.distance=0;
	weap->melee.damage=0;
	weap->melee.force=0;
	weap->melee.fall_off=TRUE;
	
	weap->fire.last_fire_tick=-1;
	weap->fire.last_fire_dual_tick=-1;
	
	weap->dual.on=FALSE;
	weap->dual.active=FALSE;
	weap->dual.in_dual=FALSE;
	weap->dual.hand_offset=0;

	object_clear_draw(&weap->draw);
	object_clear_draw(&weap->draw_dual);
	
		// connections for animated effects
		
	weap->draw.connect.obj_idx=obj->idx;
	weap->draw.connect.weap_idx=idx;
	weap->draw.connect.proj_idx=-1;
	weap->draw.connect.net_sound=FALSE;
	weap->draw.connect.motion_vct.x=0.0f;
	weap->draw.connect.motion_vct.y=0.0f;
	weap->draw.connect.motion_vct.z=0.0f;

	weap->draw_dual.connect.obj_idx=obj->idx;
	weap->draw_dual.connect.weap_idx=idx;
	weap->draw_dual.connect.proj_idx=-1;
	weap->draw_dual.connect.net_sound=FALSE;
	weap->draw_dual.connect.motion_vct.x=0.0f;
	weap->draw_dual.connect.motion_vct.y=0.0f;
	weap->draw_dual.connect.motion_vct.z=0.0f;

		// add to list
		
	obj->weap_list.weaps[idx]=weap;
	
		// clear projectile setups
		
	for (n=0;n!=max_proj_setup_list;n++) {
		weap->proj_setup_list.proj_setups[n]=NULL;
	}
	
		// the scripts
		
	ok=FALSE;
	if (weapon_start_script(obj,weap,FALSE,err_str)) {
		ok=model_draw_initialize(&weap->draw,"Weapon",weap->name,err_str);
		if ((weap->dual.on) && (ok)) {
			memmove(&weap->draw_dual,&weap->draw,sizeof(model_draw));
			ok=model_draw_initialize(&weap->draw_dual,"Weapon",weap->name,err_str);
		}
	}
	
		// there was an error
		// clean up and remove this weapon and
		// all it's projectile setups
		
	if (!ok) {
		for (n=0;n!=max_proj_setup_list;n++) {
			proj_setup_dispose(weap,n);
		}
	
		free(weap);
		obj->weap_list.weaps[idx]=NULL;
		
		return(FALSE);
	}

		// reset ammo
		
	weapon_reset_ammo(weap);
	
		// weapons always auto-spawn
		
	scripts_post_event_console(weap->script_idx,-1,sd_event_spawn,sd_event_spawn_init,0);
	
	return(TRUE);
}

void weapon_dispose(obj_type *obj,int idx)
{
	int				n;
	weapon_type		*weap;

	weap=obj->weap_list.weaps[idx];
	if (weap==NULL) return;
	
		// clear projectile setups
		
	for (n=0;n!=max_proj_setup_list;n++) {
		proj_setup_dispose(weap,n);
	}

		// clear scripts and models

	scripts_dispose(weap->script_idx);
	
	model_draw_free(&weap->draw);
	if (weap->dual.on) model_draw_free(&weap->draw_dual);
	
		// free and empty from list
		
	free(weap);
	obj->weap_list.weaps[idx]=NULL;
}

/* =======================================================

      Attaching Crosshairs and Masks
      
======================================================= */

void weapon_attach_fire_crosshair(weapon_type *weap)
{
	weap->crosshair.fire_idx=iface_crosshair_find(&iface,weap->crosshair.fire_name);
}

void weapon_attach_zoom_mask(weapon_type *weap)
{
	weap->zoom.mask_idx=iface_crosshair_find(&iface,weap->zoom.mask_name);
}
