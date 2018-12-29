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
extern void view_start_draw_list(void);
extern void view_add_mesh_liquid_draw_list(void);
extern void view_setup_objects_gl(int tick);
extern void view_setup_projectiles_gl(int tick);
extern void view_setup_objects_rtl(int tick);
extern void view_setup_projectiles_rtl(int tick);
extern void view_add_effect_draw_list(void);
extern void view_add_halos(void);
extern void view_add_model_halo(model_draw *draw,int obj_idx);
extern void view_calculate_scope(obj_type *obj,obj_type *camera_obj);
extern void view_calculate_recoil(obj_type *obj);
extern void view_calculate_shakes(obj_type *obj);
extern void view_calculate_sways(obj_type *obj);
extern void view_draw_debug_object(obj_type *obj);
extern void view_draw_debug_projectile(proj_type *proj);

/* =======================================================

      Draw Models in Scene
      
======================================================= */

void view_draw_model_opaque(void)
{
	int					n;
	obj_type			*obj;
	proj_type			*proj;

		// render the models

	for (n=0;n!=view.render->draw_list.count;n++) {

		switch (view.render->draw_list.items[n].type) {

			case view_render_type_object:
				obj=server.obj_list.objs[view.render->draw_list.items[n].idx];
				if (((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) && (obj->draw.has_opaque)) {
					render_model_build_vertex_lists(&obj->draw,FALSE);
					render_model_opaque(&obj->draw);
				}
				break;

			case view_render_type_projectile:
				proj=server.proj_list.projs[view.render->draw_list.items[n].idx];
				if (((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) && (proj->draw.has_opaque)) {
					render_model_build_vertex_lists(&proj->draw,TRUE);	// projectiles share vertex lists
					render_model_opaque(&proj->draw);
				}
				break;

		}
	}
}

void view_draw_model_transparent(void)
{
	int					n;
	obj_type			*obj;
	proj_type			*proj;

		// render the models
		// draw backwards to sort back to front

	for (n=(view.render->draw_list.count-1);n>=0;n--) {

		switch (view.render->draw_list.items[n].type) {

			case view_render_type_object:
				obj=server.obj_list.objs[view.render->draw_list.items[n].idx];
				if (((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) && (obj->draw.has_transparent)) {
					render_model_build_vertex_lists(&obj->draw,FALSE);
					render_model_transparent(&obj->draw);
				}
				break;

			case view_render_type_projectile:
				proj=server.proj_list.projs[view.render->draw_list.items[n].idx];
				if (((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) && (proj->draw.has_transparent)) {
					render_model_build_vertex_lists(&proj->draw,TRUE);		// projectiles share draw lists
					render_model_transparent(&proj->draw);
				}
				break;

		}
	}
}

void view_draw_models_final(void)
{
	int					n;
	bool				shadow_on;
	d3col				col;
	obj_type			*obj;
	proj_type			*proj;

		// shadow overrides

	shadow_on=setup.shadow_on;

		// render the shadows, remote names,
		// and any debugging information

	for (n=0;n!=view.render->draw_list.count;n++) {

		switch (view.render->draw_list.items[n].type) {

			case view_render_type_object:
				obj=server.obj_list.objs[view.render->draw_list.items[n].idx];

				if ((shadow_on) && (obj->draw.shadow.on)) {
					if ((view.render->draw_list.items[n].flag&view_list_item_flag_shadow_in_view)!=0x0) {
						render_model_build_vertex_lists(&obj->draw,FALSE);
						shadow_render_model(&obj->draw);
					}
				}

				if ((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) {
					if (object_is_targetted(obj,&col)) render_model_target(&obj->draw,&col);
					if (setup.debug_on) view_draw_debug_object(obj);
				}
				break;

			case view_render_type_projectile:
				proj=server.proj_list.projs[view.render->draw_list.items[n].idx];

				if ((shadow_on) && (proj->draw.shadow.on)) {
					if ((view.render->draw_list.items[n].flag&view_list_item_flag_shadow_in_view)!=0x0) {
						render_model_build_vertex_lists(&proj->draw,TRUE);		// projectiles share draw lists
						shadow_render_model(&proj->draw);
					}
				}

				if ((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) {
					if (setup.debug_on) view_draw_debug_projectile(proj);
				}
				break;

		}
	}
}

/* =======================================================

      Drawing Mainline OpenGL
      
======================================================= */

void view_draw_scene_build(void)
{
	int			tick;

	tick=game_time_get();

		// setup projection
		// and clipping planes

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	
	view_cull_setup_frustum_clipping_planes();

		// setup draw lists

	view_start_draw_list();

		// add mesh and liquids to draw list

	view_add_mesh_liquid_draw_list();
	
		// setup objects and projectiles
		// and add to draw list
		
	view_setup_objects_gl(tick);
	view_setup_projectiles_gl(tick);

		// add scene effects

	view_add_effect_draw_list();

		// add scene halos
		
	halo_draw_clear();
	view_add_halos();

		// do view obscuring

	view_obscure_run();
		
		// setup polygon obscuring
		
	view_cull_draw_list_mesh_poly();
	
		// compile all lights in map
		
	gl_lights_compile(tick);
}

void view_draw_scene_render(obj_type *obj,weapon_type *weap)
{
		// draw background
		// backgrounds are on the 2D screen
	
	if (map.background.on) {
		gl_2D_view_screen();
		sky_draw_background();
	}

		// draw skies, on non-rotated
		// 3D view

	if (map.sky.on) {
		gl_3D_view();
		gl_3D_rotate(NULL,&view.render->camera.ang);
		sky_draw();
	}

		// rebuild anything in the map VBOs
		// that needs chaning (lighting, UV shifts,
		// moving meshes, etc)
	
	view_map_vbo_rebuild();

		// these next drawings are done
		// in rotated 3D

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);

		// draw opaque scene items

	render_map_mesh_opaque();
	render_map_liquid_opaque();
	view_draw_model_opaque();
	
		// additional model drawing
		// shadows, remote names, etc

	view_draw_models_final();
	
		// draw transparent scene items

	render_map_liquid_transparent();
	render_map_mesh_transparent();
	view_draw_model_transparent();

		// draw decals

	decal_render();

		// effects

	effect_draw();
	
		// rain and fog
		
	rain_draw();
	fog_draw_textured();

		// weapon in hand

	if ((obj!=NULL) && (weap!=NULL)) {
		if ((map.camera.camera_mode==cv_fpp) && (!setup.no_draw_weapon)) draw_weapon_hand(obj,weap);
	}
	
		// setup halos, crosshairs, zoom masks
		// this happens in 3D space, and the 2D
		// x,y is created
		
	label_draw_setup();
	halo_draw_setup();
	
	if ((obj!=NULL) && (weap!=NULL)) {
		crosshair_setup(obj,weap);
		zoom_setup(obj,weap);
	}

		// draw the remote names, halos,
		// crosshairs, and zoom masks
		// this happens in 2D space
	
	gl_2D_view_screen();

	label_draw_render();
	halo_draw_render();
	
	if (!view.cinema.on) {
		if ((obj!=NULL) && (weap!=NULL)) {
			crosshair_draw(obj,weap);
			zoom_draw(obj,weap);
		}
	}
}

void view_draw(void)
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
	
		// reset view counts
	
	view.count.mesh=0;
	view.count.mesh_poly=0;
	view.count.liquid=0;
	view.count.liquid_poly=0;
	view.count.model=0;
	view.count.model_poly=0;
	view.count.shadow=0;
	view.count.shadow_poly=0;
	view.count.effect=0;
	view.count.obscure_percent=0;
	
		// build the scene
		
	view_draw_scene_build();

		// do any back frame rendering
		
	gl_back_render_frame_start();

		// render the scene

	gl_fs_shader_render_begin();
	view_draw_scene_render(obj,weap);
	gl_fs_shader_render_finish();

		// draw tints and fades
		
	view_draw_liquid_tint(view.render->camera.under_liquid_idx);
	view_draw_effect_tint();
	view_fade_draw();
}

bool view_draw_node(node_type *node)
{
		// switch out to node rendering

	view.render=&view_node_render;

		// camera position
		
	memmove(&view.render->camera.pnt,&node->pnt,sizeof(d3pnt));

	if (!node->follow_camera) {
		memmove(&view.render->camera.ang,&node->ang,sizeof(d3ang));
	}
	else {
		view.render->camera.ang.x=angle_find(node->pnt.y,node->pnt.z,camera.cur_pos.pnt.y,camera.cur_pos.pnt.z);
		view.render->camera.ang.y=angle_find(node->pnt.x,node->pnt.z,camera.cur_pos.pnt.x,camera.cur_pos.pnt.z);
		view.render->camera.ang.z=angle_find(node->pnt.x,node->pnt.y,camera.cur_pos.pnt.x,camera.cur_pos.pnt.y);
	}

	view.render->camera.fov=map.camera.plane.fov;
	view.render->camera.flip=TRUE;
	view.render->camera.under_liquid_idx=-1;
	view.render->camera.z_adjust=0;
	
	view.render->cull_idx=1;
	view.render->force_camera_obj=TRUE;

		// clear the frame

	gl_frame_clear(TRUE);
	gl_shader_frame_start();
		
		// build the scene
		
	view_draw_scene_build();

		// render the scene

	view_draw_scene_render(NULL,NULL);

		// restore the old rendering
		
	view.render=&view_camera_render;
	
	return(TRUE);
}

