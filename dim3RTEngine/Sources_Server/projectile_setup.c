/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Projectile Setups

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
extern js_type				js;

extern int mark_find(char *name);

/* =======================================================

      Find Projectile Setup
      
======================================================= */

proj_setup_type* find_proj_setups(weapon_type *weap,char *name)
{
	int				n;
	proj_setup_type	*proj_setup;

	for (n=0;n!=max_proj_setup_list;n++) {
		proj_setup=weap->proj_setup_list.proj_setups[n];
		if (proj_setup==NULL) continue;
		
		if (strcasecmp(proj_setup->name,name)==0) return(proj_setup);
	}
	
	return(NULL);
}

/* =======================================================

      Start Projectile Setup Script
      
======================================================= */

bool proj_setup_start_script(obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,bool no_construct,char *err_str)
{
		// create the script

	proj_setup->script_idx=scripts_add(thing_type_projectile,"Projectiles",proj_setup->name,obj->idx,weap->idx,proj_setup->idx,err_str);
	if (proj_setup->script_idx==-1) return(FALSE);
		
		// send the construct event
	
	if (no_construct) return(TRUE);

	return(scripts_post_event(proj_setup->script_idx,-1,sd_event_construct,0,0,err_str));
}

/* =======================================================

      Create and Dispose Projectile Setup
      
======================================================= */

bool proj_setup_create(obj_type *obj,weapon_type *weap,char *name,char *err_str)
{
	int					n,idx;
	proj_setup_type		*proj_setup;
	
		// find a free proj setup
		
	idx=-1;
	
	for (n=0;n!=max_proj_setup_list;n++) {
		if (weap->proj_setup_list.proj_setups[n]==NULL) {
			idx=n;
			break;
		}
	}
	
	if (idx==-1) {
		strcpy(err_str,"Reached the maximum number of weapons per object");
		return(FALSE);
	}
	
		// create memory for new projectile setup
		
	proj_setup=(proj_setup_type*)malloc(sizeof(proj_setup_type));
	if (proj_setup==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

		// initialize projectile setup
	
	proj_setup->idx=idx;
	
	proj_setup->obj_idx=obj->idx;
	proj_setup->weap_idx=weap->idx;
	
	strcpy(proj_setup->name,name);
	
	proj_setup->speed=150;
	proj_setup->decel_speed=0;
	proj_setup->decel_grace=0;
	proj_setup->decel_min_speed=0;
	proj_setup->inherit_motion_factor=0.0f;
	
	proj_setup->collision=FALSE;
	proj_setup->reset_angle=FALSE;
	proj_setup->damage=0;
	
	proj_setup->action.hit_tick=0;
	proj_setup->action.bounce=FALSE;
	proj_setup->action.bounce_min_move=10.0f;
	proj_setup->action.bounce_reduce=1.0f;
	proj_setup->action.reflect=FALSE;

	object_clear_size(&proj_setup->size);
	
	proj_setup->hitscan.on=FALSE;
	proj_setup->hitscan.max_dist=15000;
	
	proj_setup->push.on=FALSE;
	proj_setup->push.force=0;
	
	proj_setup->melee.strike_bone_name[0]=0x0;
	proj_setup->melee.strike_pose_name[0]=0x0;
	proj_setup->melee.object_strike_bone_name[0]=0x0;
	proj_setup->melee.object_strike_pose_name[0]=0x0;
	proj_setup->melee.radius=0;
	proj_setup->melee.distance=0;
	proj_setup->melee.damage=0;
	proj_setup->melee.force=0;
	proj_setup->melee.fall_off=TRUE;
	
	object_clear_draw(&proj_setup->draw);
	
		// add to list
		
	weap->proj_setup_list.proj_setups[idx]=proj_setup;
	
		// start the script
		// and load the models

	if (proj_setup_start_script(obj,weap,proj_setup,FALSE,err_str)) {
		if (model_draw_initialize(&proj_setup->draw,"Projectile",proj_setup->name,err_str)) return(TRUE);
	}
	
		// there was an error
		// clean up and remove this projectile setup
	
	free(proj_setup);
	weap->proj_setup_list.proj_setups[idx]=NULL;
	
	return(FALSE);
}

void proj_setup_dispose(weapon_type *weap,int idx)
{
	proj_setup_type		*proj_setup;

	proj_setup=weap->proj_setup_list.proj_setups[idx];
	if (proj_setup==NULL) return;

		// clear scripts and models

	scripts_dispose(proj_setup->script_idx);
	model_draw_free(&proj_setup->draw);
	
		// free and empty from list
		
	free(proj_setup);
	weap->proj_setup_list.proj_setups[idx]=NULL;
}

/* =======================================================

      Get Script Projectile Setup
      
======================================================= */

proj_setup_type* proj_setup_get_attach(JSObjectRef j_obj)
{
	int					script_idx;
	obj_type			*obj;
	weapon_type			*weap;
	script_type			*script;
	
	script_idx=(int)JSObjectGetPrivate(j_obj);
	script=js.script_list.scripts[script_idx];

	obj=server.obj_list.objs[script->attach.obj_idx];
	weap=obj->weap_list.weaps[script->attach.weap_idx];
	return(weap->proj_setup_list.proj_setups[script->attach.proj_setup_idx]);
}

proj_type* proj_get_attach(JSObjectRef j_obj)
{
	int					script_idx;
	script_type			*script;
	
	script_idx=(int)JSObjectGetPrivate(j_obj);
	script=js.script_list.scripts[script_idx];
	
	return(server.proj_list.projs[script->attach.proj_idx]);
}

