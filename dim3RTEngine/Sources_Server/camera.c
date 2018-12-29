/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Camera Main Line

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

extern map_type				map;

camera_type					camera;
camera_type					state_camera;
map_camera_type				state_map_camera;

/* =======================================================

      Setup Camera Structure
      
======================================================= */

void camera_initialize(void)
{
	camera.cur_pos.pnt.x=0;
	camera.cur_pos.pnt.y=0;
	camera.cur_pos.pnt.z=0;
	
	camera.cur_pos.ang.x=0.0f;
	camera.cur_pos.ang.y=0.0f;
	camera.cur_pos.ang.z=0.0f;

	camera.auto_walk.on=FALSE;
	camera.auto_turn_angle_offset.on=FALSE;
	camera.auto_turn_chase_angle.on=FALSE;
	camera.animate.on=FALSE;
	
	memmove(&state_camera,&camera,sizeof(camera_type));
}

void camera_map_setup(void)
{
	int				node_idx;
	char			err_str[256];
	
		// if static, attach from node
		
	if (map.camera.camera_mode!=cv_static) return;
	
		// find node, go to fpp if missing
		
	node_idx=map_find_node(&map,map.camera.c_static.attach_node);
	if (node_idx==-1) {
		sprintf(err_str,"Can not attach camera to missing node: %s",map.camera.c_static.attach_node);
		console_add_error(err_str);
		map.camera.camera_mode=cv_fpp;
		return;
	}
	
	memmove(&camera.cur_pos.static_pnt,&map.nodes[node_idx].pnt,sizeof(d3pnt));
	memmove(&camera.cur_pos.static_ang,&map.nodes[node_idx].ang,sizeof(d3ang));
}

void camera_connect(obj_type *obj)
{
	camera.obj_idx=obj->idx;
    
    camera_chase_connect();
    camera_static_connect();
}

/* =======================================================

      Find Angle To Current Camera
      
======================================================= */

void camera_get_angle_from(d3pnt *pnt,d3ang *ang)
{
	int				dist;

	dist=distance_2D_get(0,0,(camera.cur_pos.pnt.x-pnt->x),(camera.cur_pos.pnt.z-pnt->z));
	ang->x=-angle_find(0,0,(camera.cur_pos.pnt.y-pnt->y),dist);

	ang->y=angle_find(pnt->x,pnt->z,camera.cur_pos.pnt.x,camera.cur_pos.pnt.z);

	ang->z=0.0f;
}

/* =======================================================

      Check Camera for Liquids
      
======================================================= */

int camera_check_liquid(obj_type *obj,d3pnt *pnt)
{
	int					n,y;
	map_liquid_type		*liq;

		// remove any bobbing

	y=pnt->y;
	if (obj->liquid.mode==lm_float) y-=obj->liquid.bob_y_move;

	liq=map.liquid.liquids;
	
	for (n=0;n!=map.liquid.nliquid;n++) {
		if ((pnt->x>=liq->lft) && (pnt->x<=liq->rgt) && (pnt->z>=liq->top) && (pnt->z<=liq->bot) && (y>=liq->y) && (y<(liq->y+liq->depth))) return(n);
		liq++;
	}

	return(-1);
}

/* =======================================================

      Camera States
      
======================================================= */

void camera_save(void)
{
	memmove(&state_camera,&camera,sizeof(camera_type));
	memmove(&state_map_camera,&map.camera,sizeof(map_camera_type));
}

void camera_restore(void)
{
	memmove(&camera,&state_camera,sizeof(camera_type));
	memmove(&map.camera,&state_map_camera,sizeof(map_camera_type));
}

/* =======================================================

      Camera Auto Turn
      
======================================================= */

void camera_auto_turn_set_angle_offset(d3ang *ang,int life_msec)
{
	float			f;

	if (life_msec<=0) return;

	f=((float)life_msec)/10.0f;
	camera.auto_turn_angle_offset.ang.x=ang->x/f;
	camera.auto_turn_angle_offset.ang.y=ang->y/f;
	camera.auto_turn_angle_offset.ang.z=ang->z/f;

	camera.auto_turn_angle_offset.on=TRUE;
	camera.auto_turn_angle_offset.end_tick=game_time_get()+life_msec;
}

void camera_auto_turn_set_chase_angle(d3ang *ang,int life_msec)
{
	float			f;

	if (life_msec<=0) return;

	f=((float)life_msec)/10.0f;
	camera.auto_turn_chase_angle.ang.x=ang->x/f;
	camera.auto_turn_chase_angle.ang.y=ang->y/f;
	camera.auto_turn_chase_angle.ang.z=ang->z/f;

	camera.auto_turn_chase_angle.on=TRUE;
	camera.auto_turn_chase_angle.end_tick=game_time_get()+life_msec;
}

void camera_auto_turn_run_single(int tick,camera_auto_turn_type *turn,d3ang *ang)
{
	if (!turn->on) return;
	
	if (tick>=turn->end_tick) {
		turn->on=FALSE;
	}
	else {
		ang->x+=turn->ang.x;
		ang->y+=turn->ang.y;
		ang->z+=turn->ang.z;
	}
}

void camera_auto_turn_run(void)
{
	int			tick;

	tick=game_time_get();
	
	camera_auto_turn_run_single(tick,&camera.auto_turn_angle_offset,&map.camera.ang_offset);
	camera_auto_turn_run_single(tick,&camera.auto_turn_chase_angle,&map.camera.chase.track_ang);
}

/* =======================================================

      Camera Animation
      
======================================================= */

void camera_animate_start(float fov,float aspect_ratio,int msec)
{
	camera.animate.on=TRUE;

	camera.animate.start_fov=map.camera.plane.fov;
	camera.animate.end_fov=fov;

	camera.animate.start_aspect_ratio=map.camera.plane.aspect_ratio;
	camera.animate.end_aspect_ratio=aspect_ratio;

	camera.animate.start_tick=game_time_get();
	camera.animate.end_tick=camera.animate.start_tick+msec;
}

void camera_animate_run(void)
{
	int			tick;
	float		f,f_tick,f_tot_tick;

	if (!camera.animate.on) return;

	tick=game_time_get();

		// are we done?

	if (tick>=camera.animate.end_tick) {
		camera.animate.on=FALSE;
		map.camera.plane.fov=camera.animate.end_fov;
		map.camera.plane.aspect_ratio=camera.animate.end_aspect_ratio;
		return;
	}

		// calculate new ratio

	f_tick=(float)(tick-camera.animate.start_tick);
	f_tot_tick=(float)(camera.animate.end_tick-camera.animate.start_tick);

	f=camera.animate.end_fov-camera.animate.start_fov;
	map.camera.plane.fov=camera.animate.start_fov+((f*f_tick)/f_tot_tick);

	f=camera.animate.end_aspect_ratio-camera.animate.start_aspect_ratio;
	map.camera.plane.aspect_ratio=camera.animate.start_aspect_ratio+((f*f_tick)/f_tot_tick);
}

/* =======================================================

      Run Cameras
      
======================================================= */

void camera_server_run(void)
{
	camera_auto_turn_run();
	camera_animate_run();

	switch (map.camera.camera_mode) {

		case cv_chase:
			camera_chase_run();
			break;

 		case cv_static:
			camera_static_run();
			break;

	}
}

void camera_view_draw_run(void)
{
	switch (map.camera.camera_mode) {

		case cv_fpp:
            camera_fpp_calc_position();
			break;
	
		case cv_chase:
			camera_chase_calc_position();
			break;

 		case cv_static:
			camera_static_calc_position();
			break;

 		case cv_chase_static:
            camera_chase_static_calc_position();
			break;

	}
}

