/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Projectile Creation/Dispose/Finding

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

extern iface_type			iface;
extern map_type				map;
extern server_type			server;
extern js_type				js;

/* =======================================================

      Projectile List
      
======================================================= */

bool projectile_initialize_list(void)
{
	int				n;

		// pre-alloc all projectiles

	for (n=0;n!=max_proj_list;n++) {

			// memory for projectile

		server.proj_list.projs[n]=(proj_type*)malloc(sizeof(proj_type));
		if (server.proj_list.projs[n]==NULL) return(FALSE);

			// not used

		server.proj_list.projs[n]->on=FALSE;
	}

	return(TRUE);
}

void projectile_free_list(void)
{
	int				n;

	for (n=0;n!=max_proj_list;n++) {
		if (server.proj_list.projs[n]!=NULL) {
			free(server.proj_list.projs[n]);
			server.proj_list.projs[n]=NULL;
		}
	}
}

int projectile_count_list(void)
{
	int				n,count;

	count=0;

	for (n=0;n!=max_proj_list;n++) {
		if (server.proj_list.projs[n]->on) count++;
	}

	return(count);
}

/* =======================================================

      Create Projectile
      
======================================================= */

proj_type* projectile_create(obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,bool hit_scan)
{
	int					n,idx;
	proj_type			*proj;

		// find free projectile

	idx=-1;

	for (n=0;n!=max_proj_list;n++) {
		if (!server.proj_list.projs[n]->on) {
			idx=n;
			break;
		}
	}

	if (idx==-1) return(NULL);

		// initialize
	
	proj=server.proj_list.projs[idx];
	
	proj->idx=idx;
	proj->on=TRUE;
	
	proj->script_dispose=FALSE;
	
	proj->obj_idx=obj->idx;
	proj->weap_idx=weap->idx;
	proj->proj_setup_idx=proj_setup->idx;

	proj->start_tick=game_time_get();
	
	object_clear_position(&proj->pnt);
	object_clear_angle(&proj->ang);
	object_clear_motion(&proj->motion);
	object_clear_force(&proj->force);
	
	proj->speed=proj_setup->speed;
	proj->decel_speed=proj_setup->decel_speed;
	proj->decel_min_speed=proj_setup->decel_min_speed;
    proj->stick=FALSE;
	
	memmove(&proj->size,&proj_setup->size,sizeof(obj_size));

	proj->parent_grace=pd_parent_grace;
	proj->decel_grace=proj_setup->decel_grace;
	proj->reset_angle=proj_setup->reset_angle;

	object_clear_contact(&proj->contact);
	
	memmove(&proj->draw,&proj_setup->draw,sizeof(model_draw));
	memmove(&proj->action,&proj_setup->action,sizeof(proj_action_type));
	
		// connections for animated effects
		
	proj->draw.connect.obj_idx=obj->idx;
	proj->draw.connect.weap_idx=weap->idx;
	proj->draw.connect.proj_idx=idx;
	proj->draw.connect.net_sound=FALSE;
	proj->draw.connect.motion_vct.x=0.0f;
	proj->draw.connect.motion_vct.y=0.0f;
	proj->draw.connect.motion_vct.z=0.0f;

		// scripts
		
	proj->script_idx=proj_setup->script_idx;

		// dim3rtl meshes

	if (!hit_scan) view_dim3rtl_projectile_model_setup(proj);

    return(proj);
}

/* =======================================================

      Projectile Radius
      
======================================================= */

int projectile_get_radius(proj_type *proj)
{
	int			radius;
	
	radius=proj->size.x;
	if (proj->size.z>radius) radius=proj->size.z;
	
	return(radius>>1);
}

/* =======================================================

      Set Projectile Positions
      
======================================================= */

void projectile_set_position(proj_type *proj,d3pnt *pt,d3ang *ang)
{
	d3pnt			*pnt;
	
		// position
		
	pnt=&proj->pnt;
	
	pnt->x=pt->x;
	pnt->y=pt->y;
	pnt->z=pt->z;

	memmove(&proj->last_pnt,pnt,sizeof(d3pnt));

		// angles
		
	memmove(&proj->ang,ang,sizeof(d3ang));
	memmove(&proj->motion.ang,ang,sizeof(d3ang));
	
		// make sure draw is setup
		// for launch effects and spawn animations
		
	memmove(&proj->draw.pnt,pnt,sizeof(d3pnt));
	
		// initial gravity
	
	proj->force.gravity=gravity_start_power;
	proj->gravity_add=0.0f;
}

/* =======================================================

      Projectile Spawn Position
      
======================================================= */

void projectile_set_origin(proj_type *proj)
{
	memmove(&proj->org_pnt,&proj->pnt,sizeof(d3pnt));
}

void projectile_spawn_position(proj_type *proj,d3pnt *pt,d3ang *ang,obj_type *parentobj)
{
	projectile_set_position(proj,pt,ang);
	projectile_set_origin(proj);
	
	proj->contact.obj_idx=-1;
}

/* =======================================================

      Dispose Projectiles
      
======================================================= */

void projectile_dispose(proj_type *proj)
{
		// can't dispose if not on

	if (!proj->on) return;

		// delete all timers
		
	timers_clear(proj->script_idx,timer_mode_repeat);
	timers_clear(proj->script_idx,timer_mode_single);
	
		// remove any effects attached
		// to a bone
		
	effect_projectile_bone_attach_particle_dispose(proj->idx);

		// remove any dim3rtl meshes

	view_dim3rtl_projectile_model_close(proj);

		// mark as unused

	proj->on=FALSE;
}

void projectile_dispose_object(obj_type *obj)
{
	int				n;
	proj_type		*proj;
	
	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (!proj->on) continue;
		
		if (proj->obj_idx==obj->idx) projectile_dispose(proj);
	}
}

void projectile_dispose_all(void)
{
	int				n;
	proj_type		*proj;
	
	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (proj->on) projectile_dispose(proj);
	}
}
