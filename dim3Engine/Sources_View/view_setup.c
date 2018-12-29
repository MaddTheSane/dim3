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

      Draw Lists
      
======================================================= */

void view_start_draw_list(void)
{
	view.render->draw_list.count=0;
}

void view_add_draw_list(int item_type,int item_idx,int item_dist,int item_flag)
{
	int				t,idx,sz;

		// room for any more items?

	if (view.render->draw_list.count==max_view_render_item) return;
	
		// find place in sorted list

	idx=-1;

	for (t=0;t!=view.render->draw_list.count;t++) {
		if (view.render->draw_list.items[t].dist>item_dist) {
			idx=t;
			break;
		}
	}

		// insert at end of list
		
	if (idx==-1) {
		view.render->draw_list.items[view.render->draw_list.count].type=item_type;
		view.render->draw_list.items[view.render->draw_list.count].dist=item_dist;
		view.render->draw_list.items[view.render->draw_list.count].flag=item_flag;
		view.render->draw_list.items[view.render->draw_list.count].idx=(short)item_idx;
	}
	
		// insert in list
	
	else {
		sz=sizeof(view_render_draw_list_item_type)*(view.render->draw_list.count-idx);
		memmove(&view.render->draw_list.items[idx+1],&view.render->draw_list.items[idx],sz);
		
		view.render->draw_list.items[idx].type=item_type;
		view.render->draw_list.items[idx].dist=item_dist;
		view.render->draw_list.items[idx].flag=item_flag;
		view.render->draw_list.items[idx].idx=(short)item_idx;
	}

		// add up list

	view.render->draw_list.count++;
}

bool view_mesh_in_draw_list(int mesh_idx)
{
	int			n;

	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type==view_render_type_mesh) {
			if (view.render->draw_list.items[n].idx==mesh_idx) return(TRUE);
		}
	}

	return(FALSE);
}

bool view_obj_in_draw_list(int obj_idx)
{
	int			n;

	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type==view_render_type_object) {
			if (view.render->draw_list.items[n].idx==obj_idx) return(TRUE);
		}
	}

	return(FALSE);
}

bool view_proj_in_draw_list(int proj_idx)
{
	int			n;

	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type==view_render_type_projectile) {
			if (view.render->draw_list.items[n].idx==proj_idx) return(TRUE);
		}
	}

	return(FALSE);
}

/* =======================================================

      Add Map Items to Draw Lists
      
======================================================= */

void view_add_mesh_draw_list(int start_mesh_idx)
{
	int					n,dist;
	map_mesh_type		*start_mesh,*mesh;
	
	start_mesh=&map.mesh.meshes[start_mesh_idx];
	
		// check all visibile meshes from the start mesh
	
	for (n=0;n!=map.mesh.nmesh;n++) {

		mesh=&map.mesh.meshes[n];
		if (!mesh->flag.on) continue;

			// check if bound box is within view
				
		if (!view_cull_mesh(mesh)) continue;

			// sort meshes into drawing list

		dist=map_mesh_calculate_distance(mesh,&view.render->camera.pnt);
		view_add_draw_list(view_render_type_mesh,n,dist,0x0);

		view.count.mesh++;
	}
}

void view_add_liquid_draw_list(int start_mesh_idx)
{
	int					n,mx,mz,
						dist,obscure_dist;
	map_liquid_type		*liq;

		// obscure distance -- normally is the opengl projection
		// distance but can be the fog distance if fog is on

	if (!fog_solid_on()) {
		obscure_dist=map.camera.plane.far_z-map.camera.plane.near_z;
	}
	else {
		obscure_dist=(map.fog.outer_radius>>1)*3;
	}

		// find all drawable liquids

	for (n=0;n!=map.liquid.nliquid;n++) {

		liq=&map.liquid.liquids[n];
		
			// no draw liquids are volume only, so skip
			// any drawing
			
		if (liq->flag.no_draw) continue;
		
			// skip liquids with away facing normals
			// do dot product between normal and vector
			// from liq mid-eye point
			// liquid normal is always facing up (0,-1,0)
			// so this calculation is realitively easy
			
		if ((!liq->flag.never_cull) && (!map.optimize.never_cull)) {
			mx=(liq->lft+liq->rgt)>>1;
			mz=(liq->top+liq->bot)>>1;
			
			if ((-1.0f*(float)(liq->y-view.render->camera.pnt.y))>map.optimize.cull_angle) continue;
		}

			// eliminate liquids outside of view

		if (!view_cull_liquid(liq)) continue;
		
			// sort liquids into drawing list

		dist=map_liquid_calculate_distance(liq,&view.render->camera.pnt);
		view_add_draw_list(view_render_type_liquid,n,dist,0x0);

		view.count.liquid++;
	}
}

void view_add_mesh_liquid_draw_list(void)
{
	int				start_mesh_idx;

		// get mesh camera is in

	start_mesh_idx=map_mesh_find_always(&map,&view.render->camera.pnt);

		// add meshes

	view_add_mesh_draw_list(start_mesh_idx);

		// add liquids

	view_add_liquid_draw_list(start_mesh_idx);
}

/* =======================================================

      OpenGL Model Setup and Draw List
      
======================================================= */

void view_setup_objects_gl(int tick)
{
	int					n,flag;
	bool				is_camera,shadow_on;
	obj_type			*obj;
	weapon_type			*weap;
	
	shadow_on=setup.shadow_on;
	
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
		
			// set shadow lights and volumes
			// so we can cull them
			
		if ((shadow_on) && (obj->draw.shadow.on)) shadow_render_model_setup_lights(&obj->draw);

			// detect if model or shadow is culled
			// by the view window
			
		flag=0x0;
		
		if ((is_camera) && (!view.render->force_camera_obj)) {
			if ((shadow_on) && (obj->draw.shadow.on)) flag|=view_list_item_flag_shadow_in_view;
		}
		else {
			if (view_cull_model(&obj->draw)) flag|=view_list_item_flag_model_in_view;

			if ((shadow_on) && (obj->draw.shadow.on)) {
				if (view_cull_model_shadow(&obj->draw)) flag|=view_list_item_flag_shadow_in_view;
			}
		}
		
			// not in view, skip out
	
		if (flag==0x0) continue;
			
			// setup model rendering
			
		render_model_setup(&obj->draw,tick);
		
			// add to draw list

		view_add_draw_list(view_render_type_object,n,obj->draw.draw_dist,flag);

		if ((flag&view_list_item_flag_model_in_view)!=0x0) view.count.model++;
		if ((flag&view_list_item_flag_shadow_in_view)!=0x0) view.count.shadow++;
		
			// if the player, setup the held
			// weapon (and dual if necessary)

		if ((obj->idx==server.player_obj_idx) && (obj->held_weapon.current_idx!=-1)) {
			weap=weapon_find_current(obj);
			if (weap!=NULL) {

					// regular weapon

				model_draw_setup_weapon(obj,weap,FALSE,FALSE);
				if (weap->draw.on) view.count.model++;
				
					// dual weapon

				if ((weap->dual.on) && (weap->dual.active)) {
					model_draw_setup_weapon(obj,weap,FALSE,TRUE);
					if (weap->draw_dual.on) view.count.model++;
				}
			}
		}
	}
	
		// player weapons
		
	obj=server.obj_list.objs[server.player_obj_idx];
	weap=weapon_find_current(obj);

	if (weap!=NULL) draw_weapon_hand_setup(obj,weap);
}

void view_setup_projectiles_gl(int tick)
{
	int					n,flag;
	bool				shadow_on;
	proj_type			*proj;
	
	shadow_on=setup.shadow_on;
	
	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (!proj->on) continue;
		
			// setup model positions
			
		projectile_reset_angle_for_flight(proj);
		model_draw_setup_projectile(proj);
		
		model_calc_animation(&proj->draw,tick);
		model_calc_draw_bones(&proj->draw);
		
			// set shadow lights and volumes
			// so we can cull them
			
		if ((shadow_on) && (proj->draw.shadow.on)) shadow_render_model_setup_lights(&proj->draw);

			// detect if model or shadow is culled
			// by the view window
			
		flag=0x0;
		
		if (view_cull_model(&proj->draw)) flag|=view_list_item_flag_model_in_view;

		if ((shadow_on) && (proj->draw.shadow.on)) {
			if ((flag&view_list_item_flag_model_in_view)!=0x0) {		// model in view means shadow is automatically in view
				flag|=view_list_item_flag_shadow_in_view;
			}
			else {
				if (view_cull_model_shadow(&proj->draw)) flag|=view_list_item_flag_shadow_in_view;
			}
		}
	
		if (flag==0x0) continue;
		
			// setup model rendering
			
		render_model_setup(&proj->draw,tick);

			// add to draw list

		view_add_draw_list(view_render_type_projectile,n,proj->draw.draw_dist,flag);

		view.count.model++;
	}
}

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
				if (weap->draw.on) view.count.model++;
				
					// dual weapon

				if ((weap->dual.on) && (weap->dual.active)) {
					model_draw_setup_weapon(obj,weap,FALSE,TRUE);
					if (weap->draw_dual.on) view.count.model++;
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

      Effects in View
      
======================================================= */

void view_add_effect_draw_list(void)
{
	int					n,dist;
	d3pnt				center_pnt;
	effect_type			*effect;

		// find all effects

	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (!effect->on) continue;
		
			// effect inside a mesh that's hidden?

		if (effect->mesh_idx!=-1) {
			if (!view_mesh_in_draw_list(effect->mesh_idx)) continue;
		}
				
			// check for effect culling

		if (!view_cull_effect(effect,&center_pnt)) continue;

			// sort effects into drawing list
			
		dist=view_cull_distance_to_view_center(effect->pnt.x,effect->pnt.y,effect->pnt.z);
		view_add_draw_list(view_render_type_effect,n,dist,0x0);

		view.count.effect++;
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

				// check for halo culling

			if (!view_cull_halo(&pnt)) continue;
	
				// add the halo
				
			halo_draw_add(&pnt,obj_idx,halo->idx);

			view.count.effect++;
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

		// halos from objects and their weapons
		
	for (n=0;n!=view.render->draw_list.count;n++) {

		switch (view.render->draw_list.items[n].type) {

			case view_render_type_object:
				obj=server.obj_list.objs[view.render->draw_list.items[n].idx];
				view_add_model_halo(&obj->draw,obj->idx);
				break;

			case view_render_type_projectile:
				proj=server.proj_list.projs[view.render->draw_list.items[n].idx];
				view_add_model_halo(&proj->draw,-1);
				break;

		}
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

/* =======================================================

      View Script 3D to 2D transform
      
======================================================= */

void view_script_transform_3D_to_2D(d3pnt *pnt)
{
	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	
	gl_project_point(pnt);
}

