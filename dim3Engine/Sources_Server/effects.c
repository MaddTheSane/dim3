/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Effects Main Line

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

extern iface_type			iface;
extern map_type				map;
extern server_type			server;

/* =======================================================

      Effect List
      
======================================================= */

bool effect_initialize_list(void)
{
	int				n;

		// pre-alloc all effects

	for (n=0;n!=max_effect_list;n++) {

			// memory for effects

		server.effect_list.effects[n]=(effect_type*)malloc(sizeof(effect_type));
		if (server.effect_list.effects[n]==NULL) return(FALSE);

			// not used

		server.effect_list.effects[n]->on=FALSE;

			// clear the vbo

		view_clear_effect_vertex_object(server.effect_list.effects[n]);
	}

	return(TRUE);
}

void effect_free_list(void)
{
	int				n;

	for (n=0;n!=max_effect_list;n++) {
		if (server.effect_list.effects[n]==NULL) continue;
				
			// dispose vbo if it's active

		view_dispose_effect_vertex_object(server.effect_list.effects[n]);

			// free effect

		free(server.effect_list.effects[n]);
	}
}

int effect_count_list(void)
{
	int				n,count;

	count=0;

	for (n=0;n!=max_effect_list;n++) {
		if (server.effect_list.effects[n]!=NULL) {
			if (server.effect_list.effects[n]->on) count++;
		}
	}

	return(count);
}

/* =======================================================

      Create a New Effect
      
======================================================= */

int effect_spawn(int effecttype,d3pnt *pt,int life_tick)
{
	int				n,idx;
	effect_type		*effect;
	
		// can't spawn 0 time effects
		
	if (life_tick<=0) {
		console_add_error("Can't spawn effects with no life time");
		return(-1);
	}
	
		// any more effect spots?
		// this is a silent error, as it's not fatal
		// and not script-based

	idx=-1;

	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (!effect->on) {
			idx=n;
			break;
		}
	}
		
	if (idx==-1) return(-1);
	
		// create effect
	
	effect=server.effect_list.effects[idx];
	effect->on=TRUE;
	
	effect->effecttype=effecttype;
	
	memmove(&effect->pnt,pt,sizeof(d3pnt));

	effect->mesh_idx=map_mesh_find(&map,pt);

	effect->start_tick=game_time_get();
	effect->life_tick=life_tick;

	effect->rtl_mesh_id=-1;

	return(idx);
}

/* =======================================================

      Delete Effects
      
======================================================= */

static inline void effect_dispose_single(effect_type *effect)
{
		// turn off effect

	effect->on=FALSE;

		// dispose any active VBOs
		
	view_dispose_effect_vertex_object(effect);
}

void effect_dispose(void)
{
	int					n,tick;
	effect_type			*effect;
	
	tick=game_time_get();
	
		// delete all effects that have timed out

	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (!effect->on) continue;
	
			// has it timed out?

		if ((tick-effect->start_tick)<effect->life_tick) continue;

			// is this a chained particle?

		if (effect->effecttype==ef_particle) {
			if (particle_chain(effect)) continue;
		}

			// dispose effect
			
		effect_dispose_single(effect);
	}
}

/* =======================================================

      Delete Particle Effects With Bone Attachments
	  This is to clear up bone-attach particles when
	  an object or a projectile is deleted
      
======================================================= */

void effect_object_bone_attach_particle_dispose(int obj_idx)
{
	int						n;
	effect_type				*effect;
	particle_effect_data	*eff_particle;

	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (!effect->on) continue;
		
			// is a particle?
			
		if (effect->effecttype!=ef_particle) continue;
		
			// has bone attachment?
			
		eff_particle=&effect->data.particle;
		if (eff_particle->motion.bone_idx==-1) continue;
		if (eff_particle->motion.obj_idx!=obj_idx) continue;

			// dispose effect
			
		effect_dispose_single(effect);
	}
}

void effect_weapon_bone_attach_particle_dispose(int obj_idx,int weap_idx)
{
	int						n;
	effect_type				*effect;
	particle_effect_data	*eff_particle;

	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (!effect->on) continue;
		
			// is a particle?
			
		if (effect->effecttype!=ef_particle) continue;
		
			// has bone attachment?
			
		eff_particle=&effect->data.particle;
		if (eff_particle->motion.bone_idx==-1) continue;
		if (eff_particle->motion.obj_idx!=obj_idx) continue;
		if (eff_particle->motion.weap_idx!=weap_idx) continue;

			// dispose effect
			
		effect_dispose_single(effect);
	}
}

void effect_projectile_bone_attach_particle_dispose(int proj_idx)
{
	int						n;
	effect_type				*effect;
	particle_effect_data	*eff_particle;

	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (!effect->on) continue;
		
			// is a particle?
			
		if (effect->effecttype!=ef_particle) continue;
		
			// has bone attachment?
			
		eff_particle=&effect->data.particle;
		if (eff_particle->motion.bone_idx==-1) continue;
		if (eff_particle->motion.proj_idx!=proj_idx) continue;

			// dispose effect
			
		effect_dispose_single(effect);
	}
}
