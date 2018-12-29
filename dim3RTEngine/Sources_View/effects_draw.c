/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Effect Drawing

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
#include "objects.h"

extern map_type			map;
extern server_type		server;
extern view_type		view;
extern iface_type		iface;

/* =======================================================

      Get UV for animated Images
      
======================================================= */

void effect_image_animate_get_uv(int tick,int image_offset,iface_image_animation_type *animate,float *gx,float *gy,float *g_size)
{
	int				k,nimage,image_per_row;
	float			gsz;
	
	nimage=animate->image_count;
	
	*gx=*gy=0.0f;
	*g_size=1.0f;
	
	if (nimage>1) {
		if (animate->msec==0) {
			k=0;
		}
		else {
			k=(tick/animate->msec)+image_offset;
		}

		if ((!animate->loop) && (k>=nimage)) {
			k=nimage-1;
		}
		else {
			if (!animate->loop_back) {
				k=k%nimage;
			}
			else {
				k=k%((nimage<<1)-2);
				if (k>=nimage) k=(nimage+2)-k;
			}
		}

		image_per_row=animate->image_per_row;
		gsz=1.0f/(float)image_per_row;
		
		*gx=gsz*(float)(k%image_per_row);
		*gy=gsz*(float)(k/image_per_row);
		
		*g_size=gsz;
	}
}

/* =======================================================

      Draw Ring Position
      
======================================================= */

void ring_draw_position(effect_type *effect,int count,d3pnt *pnt)
{
	float				m_tick;
	iface_ring_type		*ring;
	ring_effect_data	*eff_ring;
	
	eff_ring=&effect->data.ring;
	ring=&iface.ring_list.rings[eff_ring->ring_idx];

	m_tick=((float)count)/10.0f;
	
	pnt->x=effect->pnt.x+(int)(ring->vct.x*m_tick);
	pnt->y=effect->pnt.y+(int)(ring->vct.y*m_tick);
	pnt->z=effect->pnt.z+(int)(ring->vct.z*m_tick);
}

/* =======================================================

      Get Particle Connected Model Draw
      
======================================================= */

model_draw* particle_draw_get_model(particle_motion *motion)
{
	obj_type				*obj;
	weapon_type				*weap;
	proj_type				*proj;

		// is it a projectile?

	if (motion->proj_idx!=-1) {
		proj=server.proj_list.projs[motion->proj_idx];
		if (!proj->on) return(NULL);
		return(&proj->draw);
	}

		// is it a weapon?

	if (motion->weap_idx!=-1) {
		obj=server.obj_list.objs[motion->obj_idx];
		weap=obj->weap_list.weaps[motion->weap_idx];
		if (weap==NULL) return(NULL);
		if (motion->weap_in_dual) return(&weap->draw_dual);
		return(&weap->draw);
	}

		// is object

	if (motion->obj_idx==-1) return(NULL);

	obj=server.obj_list.objs[motion->obj_idx];
	if (obj==NULL) return(NULL);
	return(&obj->draw);
}

/* =======================================================

      Get Particle Position
      
======================================================= */

void particle_draw_position(effect_type *effect,int count,d3pnt *pnt)
{
	float					f_tick;
	model_draw				*draw;
	particle_effect_data	*eff_particle;

		// position
	
	pnt->x=effect->pnt.x;
	pnt->y=effect->pnt.y;
	pnt->z=effect->pnt.z;

	eff_particle=&effect->data.particle;

		// sticking to bone?

	if (eff_particle->motion.bone_idx!=-1) {
		draw=particle_draw_get_model(&eff_particle->motion);
		if (draw!=NULL) model_get_last_draw_bone_position(draw,eff_particle->motion.bone_idx,pnt);
	}

		// motion

	else {
		f_tick=((float)count)/10.0f;
		pnt->x+=(int)(eff_particle->motion.vct.x*f_tick);
		pnt->y+=(int)(eff_particle->motion.vct.y*f_tick);
		pnt->z+=(int)(eff_particle->motion.vct.z*f_tick);
	}
}

