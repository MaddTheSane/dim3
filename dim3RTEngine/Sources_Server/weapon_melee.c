/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Weapon Melee

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
extern view_type			view;
extern setup_type			setup;
extern network_setup_type	net_setup;

/* =======================================================

      Add Melee
      
======================================================= */

void melee_add(obj_type *obj,weapon_type *weap,d3pnt *pt,d3ang *ang,melee_type *melee,int ignore_obj_idx)
{
	int				n,xadd,zadd;
	char			weap_name[name_str_len];
	d3pnt			pnt,sphere_pnt;
	obj_type		*hurt_obj;
	proj_type		*proj;

		// fail under liquid?

	if (weap!=NULL) {
		if ((weap->fail_in_liquid) && (obj->liquid.mode==lm_under)) return;
	}
	
		// original position for network melees
		
	pnt.x=pt->x;
	pnt.y=pt->y;
	pnt.z=pt->z;
    
		// move melee ahead for distance
		
	angle_get_movement(ang->y,melee->distance,&xadd,&zadd);

	sphere_pnt.x=pt->x+xadd;
	sphere_pnt.y=pt->y;
	sphere_pnt.z=pt->z+zadd;
	
		// check objects
		
	for (n=0;n!=max_obj_list;n++) {
		hurt_obj=server.obj_list.objs[n];
		if (hurt_obj==NULL) continue;

        if ((hurt_obj->hidden) || (!hurt_obj->contact.projectile_on) || (hurt_obj->idx==ignore_obj_idx)) continue;
		
			// melee hit?
			
		if (collide_object_to_sphere(&sphere_pnt,melee->radius,hurt_obj)) {
			object_melee_hit(hurt_obj,obj,weap,melee,&sphere_pnt);
		}
	}
  
		// check projectiles
		
	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (!proj->on) continue;

		if (collide_projectile_to_sphere(&sphere_pnt,melee->radius,proj)) projectile_hit(proj);
	}
	
		// do any pushes
		
	if (melee->force!=0) {
		collide_objects_push(&sphere_pnt,melee->radius,melee->force);
	}
	
		// if this object is the player or multiplayer/map bot,
		// then spawn melee in remotes
		
	if (net_setup.mode!=net_mode_none) {
		if (object_networkable(obj)) {
			weap_name[0]=0x0;
			if (weap!=NULL) strcpy(weap_name,weap->name);
			net_client_send_melee_add(obj,weap_name,melee->radius,melee->distance,melee->damage,melee->force,&pnt,ang);
		}
	}
}

/* =======================================================

      Get Melee Strike Position
      
======================================================= */

bool melee_strike_position_weapon_model(obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,char *err_str)
{
	int						pose_idx,bone_idx;
	model_type				*mdl;
	model_draw_setup		*setup;

		// get model

	if (weap->draw.model_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has no model");
		return(FALSE);
	}

	mdl=server.model_list.models[weap->draw.model_idx];

		// check pose and bone

	pose_idx=model_find_pose(mdl,weap->melee.strike_pose_name);
	if (pose_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has missing or no strike pose");
		return(FALSE);
	}
	
	bone_idx=model_find_bone(mdl,weap->melee.strike_bone_name);
	if (bone_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has missing or no strike bone");
		return(FALSE);
	}

		// create melee position

	model_draw_setup_weapon(obj,weap,TRUE,FALSE);

	setup=&weap->draw.setup;

	setup->move.x=setup->move.y=setup->move.z=0;
	setup->sway.x=setup->sway.y=setup->sway.z=0.0f;

	model_calc_draw_bone_position(mdl,setup,pose_idx,bone_idx,fire_pnt);
	
		// move fire point to weapon

	fire_pnt->x+=weap->draw.pnt.x;
	fire_pnt->y+=weap->draw.pnt.y+obj->duck.y_move;
	fire_pnt->z+=weap->draw.pnt.z;

	return(TRUE);
}

bool melee_strike_position_object_model(obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,char *err_str)
{
	int						pose_idx,bone_idx;
	model_type				*mdl;
	model_draw_setup		*setup;

		// get model

	if (obj->draw.model_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Object has no model");
		return(FALSE);
	}

	mdl=server.model_list.models[obj->draw.model_idx];

		// check current pose and bone

	if (weap==NULL) {
		pose_idx=model_find_pose(mdl,obj->melee.strike_pose_name);
		bone_idx=model_find_bone(mdl,obj->melee.strike_bone_name);
	}
	else {
		pose_idx=model_find_pose(mdl,weap->melee.object_strike_pose_name);
		bone_idx=model_find_bone(mdl,weap->melee.object_strike_bone_name);
	}
	
	if (pose_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Object has missing or no strike pose");
		return(FALSE);
	}

	if (bone_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Object has missing or no strike bone");
		return(FALSE);
	}

		// create melee position
	
	model_draw_setup_object(obj);

	setup=&obj->draw.setup;

	setup->move.x=setup->move.y=setup->move.z=0;
	setup->sway.x=setup->sway.y=setup->sway.z=0.0f;

	model_calc_draw_bone_position(mdl,setup,pose_idx,bone_idx,fire_pnt);
	
		// move fire point to obj

	fire_pnt->x+=obj->draw.pnt.x;
	fire_pnt->y+=obj->draw.pnt.y+obj->duck.y_move;
	fire_pnt->z+=obj->draw.pnt.z;

	return(TRUE);
}

bool melee_strike_position_projectile_model(obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,proj_type *proj,d3pnt *fire_pnt,char *err_str)
{
	int						pose_idx,bone_idx;
	model_type				*mdl;
	model_draw_setup		*setup;

		// get model

	if (proj->draw.model_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Projectile has no model");
		return(FALSE);
	}

	mdl=server.model_list.models[proj->draw.model_idx];

		// check pose and bone

	pose_idx=model_find_pose(mdl,proj_setup->melee.strike_pose_name);
	if (pose_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Projectile has missing or no strike pose");
		return(FALSE);
	}

	bone_idx=model_find_bone(mdl,proj_setup->melee.strike_bone_name);
	if (bone_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Projectile has missing or no strike bone");
		return(FALSE);
	}

		// create melee position

	model_draw_setup_projectile(proj);

	setup=&proj->draw.setup;

	setup->move.x=setup->move.y=setup->move.z=0;
	setup->sway.x=setup->sway.y=setup->sway.z=0.0f;

	model_calc_draw_bone_position(mdl,setup,pose_idx,bone_idx,fire_pnt);
	
		// move fire point to projectile

	fire_pnt->x+=proj->draw.pnt.x;
	fire_pnt->y+=proj->draw.pnt.y;
	fire_pnt->z+=proj->draw.pnt.z;

	return(TRUE);
}

/* =======================================================

      Script Melee Spawning
      
======================================================= */

bool melee_script_spawn_weapon_model(obj_type *obj,weapon_type *weap,char *err_str)
{
	d3pnt				pt;

		// get strike position

	if (!melee_strike_position_weapon_model(obj,weap,&pt,err_str)) return(FALSE);
	
		// add melee
		
	melee_add(obj,weap,&pt,&obj->ang,&weap->melee,obj->idx);

	return(TRUE);
}

bool melee_script_spawn_object_model(obj_type *obj,weapon_type *weap,char *err_str)
{
	d3pnt				pt;

		// get strike position

	if (!melee_strike_position_object_model(obj,weap,&pt,err_str)) return(FALSE);

		// add melee
	
	if (weap==NULL) {
		melee_add(obj,NULL,&pt,&obj->ang,&obj->melee,obj->idx);
	}
	else {
		melee_add(obj,weap,&pt,&obj->ang,&weap->melee,obj->idx);
	}

	return(TRUE);
}

bool melee_script_spawn_object_touch(obj_type *obj,char *err_str)
{
	melee_add(obj,NULL,&obj->touch.pnt,&obj->ang,&obj->melee,obj->idx);
	return(TRUE);
}

bool melee_script_spawn_projectile_model(obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,proj_type *proj,char *err_str)
{
	d3pnt				pt;
	
		// get strike position

	if (!melee_strike_position_projectile_model(obj,weap,proj_setup,proj,&pt,err_str)) return(FALSE);

		// add melee
		
	melee_add(obj,weap,&pt,&proj->ang,&proj_setup->melee,-1);

	return(TRUE);
}

