/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Setup

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

extern bool					shadow_on;

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern setup_type			setup;

/* =======================================================

      RL Model Setup and Draw List
	  This version doesn't use drawing lists
      
======================================================= */

void view_setup_objects_rtl(int tick)
{
	int					n;
	bool				is_camera;
	obj_type			*obj;
	weapon_type			*weap;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
		
		if (obj->hidden) continue;
		
		is_camera=((map.camera.camera_mode==cv_fpp) && (obj->idx==camera.obj_idx));
		
			// setup model positions
			
		object_rigid_body_reset_angle(obj);
		object_fly_reset_angle(obj);
		model_draw_setup_object(obj);
		
		model_calc_animation(&obj->draw,tick);
		model_calc_draw_bones(&obj->draw);
					
			// setup model rendering
			
		render_model_setup(&obj->draw,tick);
		
			// if the player, setup the held
			// weapon (and dual if necessary)

		if ((obj->idx==server.player_obj_idx) && (obj->held_weapon.current_idx!=-1)) {
			weap=weapon_find_current(obj);
			if (weap!=NULL) {

					// regular weapon

				model_draw_setup_weapon(obj,weap,FALSE,FALSE);
				
					// dual weapon

				if ((weap->dual.on) && (weap->dual.active)) {
					model_draw_setup_weapon(obj,weap,FALSE,TRUE);
				}
			}
		}
	}
	
		// player weapons
		
	obj=server.obj_list.objs[server.player_obj_idx];
	weap=weapon_find_current(obj);

	if (weap!=NULL) draw_weapon_hand_setup(obj,weap);
}

void view_setup_projectiles_rtl(int tick)
{
	int					n;
	proj_type			*proj;
	
	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (!proj->on) continue;
		
			// setup model positions
			
		projectile_reset_angle_for_flight(proj);
		model_draw_setup_projectile(proj);
		
		model_calc_animation(&proj->draw,tick);
		model_calc_draw_bones(&proj->draw);
				
			// setup model rendering
			
		render_model_setup(&proj->draw,tick);
	}
}

/* =======================================================

      Halos in View
      
======================================================= */

void view_add_model_halo(model_draw *draw,int obj_idx)
{
	int					n;
	d3pnt				pnt;
	model_type			*mdl;
	model_draw_halo		*halo;

		// any model?
		
	if ((draw->model_idx==-1) || (!draw->on)) return;
	
	mdl=server.model_list.models[draw->model_idx];
	
		// add halo
		
	halo=draw->halos;

	for (n=0;n!=max_model_halo;n++) {

		if (halo->on) {
				
				// get position

			pnt.x=draw->pnt.x;
			pnt.y=draw->pnt.y;
			pnt.z=draw->pnt.z;

			model_get_halo_position(mdl,&draw->setup,n,&pnt);
	
				// add the halo
				
			halo_draw_add(&pnt,obj_idx,halo->idx);
		}

		halo++;
	}
}

void view_add_halos(void)
{
	int					n;
	obj_type			*obj;
	proj_type			*proj;
	map_light_type		*lit;
	map_particle_type	*prt;

		// halos from objects
		
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj!=NULL) view_add_model_halo(&obj->draw,obj->idx);
	}

		// halos from projectiles

	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (proj->on) view_add_model_halo(&proj->draw,-1);
	}

		// halos from map lights

	lit=map.lights;

	for (n=0;n!=map.nlight;n++) {
		halo_draw_add(&lit->pnt,-1,lit->setting.halo_idx);
		lit++;
	}
	
		// halos from map particles

	prt=map.particles;

	for (n=0;n!=map.nparticle;n++) {
		halo_draw_add(&prt->pnt,-1,prt->light_setting.halo_idx);
		prt++;
	}
}

/* =======================================================

      Scope FOV changes
      
======================================================= */

void view_calculate_scope(obj_type *obj,obj_type *camera_obj)
{
	int				tick;
	float			f_step,f_max_step,f_zoom,
					sway_add,sway_max;
	weapon_type		*weap;
	
		// is object the camera object?

	if (camera_obj->idx!=obj->idx) return;
	
		// scoped weapon on?
	
	weap=weapon_find_current(obj);
	if (weap==NULL) return;
	
	if ((!weap->zoom.on) || (weap->zoom.mode==zoom_mode_off)) return;
	
		// calculate fov
		
	if (weap->zoom.step_count<=1) {
		view.render->camera.fov=weap->zoom.fov_max;
		return;
	}
		
	f_step=(float)weap->zoom.current_step;
	f_max_step=(float)(weap->zoom.step_count-1);
	
	f_zoom=(weap->zoom.fov_max-(((weap->zoom.fov_max-weap->zoom.fov_min)/f_max_step)*f_step))-view.render->camera.fov;
	
		// zoom look down sites

	tick=game_time_get();

	switch (weap->zoom.mode) {

		case zoom_mode_in:
			if (tick<(obj->zoom_draw.start_tick+weap->zoom.tick)) {
				f_zoom*=((float)(tick-obj->zoom_draw.start_tick)/(float)weap->zoom.tick);
			}
			else {
				weap->zoom.mode=zoom_mode_on;
			}
			break;

		case zoom_mode_out:
			if (tick<(obj->zoom_draw.start_tick+weap->zoom.tick)) {
				f_zoom*=1.0f-((float)(tick-obj->zoom_draw.start_tick)/(float)weap->zoom.tick);
			}
			else {
				weapon_zoom_off(obj,weap);
				return;
			}
			break;
	}
	
		// change perspective
	
	view.render->camera.fov+=f_zoom;

		// no sways until zoom modes is on

	if (weap->zoom.mode!=zoom_mode_on) return;
	if (weap->zoom.sway_factor==0.0f) return;

	sway_add=0.025f*weap->zoom.sway_factor;
	sway_max=3.0f*weap->zoom.sway_factor;

		// first time start of sways

	if (obj->zoom_draw.sway_reset) {
		obj->zoom_draw.sway_reset=FALSE;
		obj->zoom_draw.sway_ang.x=obj->zoom_draw.sway_ang.y=0.0f;
		obj->zoom_draw.sway_next.x=random_float(sway_max)-(sway_max/2.0f);
		obj->zoom_draw.sway_next.y=random_float(sway_max)-(sway_max/2.0f);
	}

		// calculate any sways

	if (obj->zoom_draw.sway_next.x<obj->zoom_draw.sway_ang.x) {
		obj->zoom_draw.sway_ang.x-=sway_add;
		if (obj->zoom_draw.sway_next.x>=obj->zoom_draw.sway_ang.x) {
			obj->zoom_draw.sway_next.x=random_float(sway_max)-(sway_max/2.0f);
		}
	}
	else {
		obj->zoom_draw.sway_ang.x+=sway_add;
		if (obj->zoom_draw.sway_next.x<=obj->zoom_draw.sway_ang.x) {
			obj->zoom_draw.sway_next.x=random_float(sway_max)-(sway_max/2.0f);
		}
	}

	if (obj->zoom_draw.sway_next.y<obj->zoom_draw.sway_ang.y) {
		obj->zoom_draw.sway_ang.y-=sway_add;
		if (obj->zoom_draw.sway_next.y>=obj->zoom_draw.sway_ang.y) {
			obj->zoom_draw.sway_next.y=random_float(sway_max)-(sway_max/2.0f);
		}
	}
	else {
		obj->zoom_draw.sway_ang.y+=sway_add;
		if (obj->zoom_draw.sway_next.y<=obj->zoom_draw.sway_ang.y) {
			obj->zoom_draw.sway_next.y=random_float(sway_max)-(sway_max/2.0f);
		}
	}

		// set sways

	view.render->camera.ang.x+=obj->zoom_draw.sway_ang.x;
	view.render->camera.ang.y+=obj->zoom_draw.sway_ang.y;
}

void view_calculate_recoil(obj_type *obj)
{
	weapon_type		*weap;

	weap=weapon_find_current(obj);
	if (weap==NULL) return;

	weapon_recoil_add(obj,weap,&view.render->camera.ang);
}

/* =======================================================

      Shakes and Sways
      
======================================================= */

void view_calculate_shakes(obj_type *obj)
{
	int					n,k,d,shake_sz,shake_cnt;
	float				shake_freq;
	effect_type			*effect;
	
	shake_cnt=0;
	shake_freq=0.0f;
	shake_sz=0;
	
		// find any shake effects
		
	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (!effect->on) continue;
	
		if (effect->effecttype==ef_shake) {
			
			d=distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,effect->pnt.x,effect->pnt.y,effect->pnt.z);
			if (d<effect->data.shake.distance) {
				shake_sz+=effect->size;
				shake_freq+=(0.3f-((0.3f*(float)d)/(float)effect->data.shake.distance))+0.2f;
				shake_cnt++;
			}
		}
	}
	
		// any shake data?
		
	if (shake_cnt==0) return;

	shake_sz=shake_sz/shake_cnt;
	shake_freq=shake_freq/(float)shake_cnt;
	
		// do shake
		
	k=(int)((float)(game_time_get()-effect->start_tick)*shake_freq)%shake_sz;
	if (k>(shake_sz>>1)) k=shake_sz-k;
	
	view.render->camera.ang.x+=((float)k/25.0f);
}

void view_calculate_sways(obj_type *obj)
{
	float				f;

	return;

	f=(float)(((game_time_get()>>4)%12)-6);
	view.render->camera.ang.z+=f;

	// supergumba -- work on sways

}

