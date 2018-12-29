/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Drawing

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

extern iface_type			iface;
extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern setup_type			setup;
extern render_info_type		render_info;

extern view_render_type		view_camera_render,view_node_render;

float						shake_ang_x[16]={-1,0,1,2,1,0,-1,-2,-4,-2,0,4,8,12,8,4};

extern void model_calc_pose_bones(model_draw *draw);
extern void view_setup_objects_rtl(int tick);
extern void view_setup_projectiles_rtl(int tick);
extern void view_add_halos(void);
extern void view_add_model_halo(model_draw *draw,int obj_idx);
extern void view_calculate_scope(obj_type *obj,obj_type *camera_obj);
extern void view_calculate_recoil(obj_type *obj);
extern void view_calculate_shakes(obj_type *obj);
extern void view_calculate_sways(obj_type *obj);

/* =======================================================

      Drawing Mainline
      
======================================================= */

void view_draw_dim3rtl_scene_build(void)
{
	int					n,tick;
	obj_type			*obj;
	proj_type			*proj;
	map_light_type		*lit;
	map_particle_type	*prt;

		// build the scene
		// we don't have to do as much as opengl
		// so just setup the animations
		
	tick=game_time_get();

	view_setup_objects_rtl(tick);
	view_setup_projectiles_rtl(tick);

		// update the sky cube

	sky_update();

		// no halos yet

	halo_draw_clear();

		// create the model vertex/etc
		// lists for updating rtl and
		// add any halos

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
		if (obj->hidden) continue;

		render_model_build_vertex_lists(&obj->draw,TRUE);
		view_add_model_halo(&obj->draw,obj->idx);
	}

	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (!proj->on) continue;
		
		render_model_build_vertex_lists(&proj->draw,FALSE);
		view_add_model_halo(&proj->draw,-1);
	}

		// map and particle halos

	lit=map.lights;

	for (n=0;n!=map.nlight;n++) {
		halo_draw_add(&lit->pnt,-1,lit->setting.halo_idx);
		lit++;
	}

	prt=map.particles;

	for (n=0;n!=map.nparticle;n++) {
		halo_draw_add(&prt->pnt,-1,prt->light_setting.halo_idx);
		prt++;
	}
}

void view_draw_dim3rtl(void)
{
	obj_type		*obj,*camera_obj;
	weapon_type		*weap;

		// get player object and held weapon
	
	obj=server.obj_list.objs[server.player_obj_idx];
	weap=weapon_find_current(obj);

		// use the camera render

	view.render=&view_camera_render;
	
		// set view camera
		
	camera_view_draw_run();
	
	camera_obj=server.obj_list.objs[camera.obj_idx];
	memmove(&view.render->camera.pnt,&camera.cur_pos.pnt,sizeof(d3pnt));
	memmove(&view.render->camera.ang,&camera.cur_pos.ang,sizeof(d3ang));

	view.render->camera.fov=map.camera.plane.fov;
	view.render->camera.flip=FALSE;
	view.render->camera.under_liquid_idx=camera_check_liquid(obj,&view.render->camera.pnt);
	
	view.render->cull_idx=0;
	view.render->force_camera_obj=FALSE;

		// camera adjustments
	
	if (map.camera.camera_mode==cv_fpp) {
		view_calculate_scope(obj,camera_obj);
		view_calculate_recoil(obj);
	}
	
	view_calculate_shakes(obj);
	view_calculate_sways(obj);
	
		// build the scene

	view_draw_dim3rtl_scene_build();

		// we setup the 2D elements before
		// rendering because we get the last
		// frame so we want to use the last
		// frames eye point

	label_draw_setup();
	halo_draw_setup();

	if ((obj!=NULL) && (weap!=NULL)) {
		crosshair_setup(obj,weap);
		zoom_setup(obj,weap);
	}

		// dim3rtl rendering

	view_dim3rtl_render();

		// draw 2D opengl elements

	gl_2D_view_screen();

	label_draw_render();
	halo_draw_render();

	if (!view.cinema.on) {
		if ((obj!=NULL) && (weap!=NULL)) {
			crosshair_draw(obj,weap);
			zoom_draw(obj,weap);
		}
	}

		// draw tints and fades
		
	view_draw_liquid_tint(view.render->camera.under_liquid_idx);
	view_draw_effect_tint();
	view_fade_draw();
}

/* =======================================================

      Drawing Node Camera
      
======================================================= */

bool view_draw_node(node_type *node)
{
	// supergumba -- need ray trace version for this	
	return(TRUE);
}

