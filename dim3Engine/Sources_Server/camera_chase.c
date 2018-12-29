/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Chase Camera

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

extern iface_type		iface;
extern server_type		server;
extern view_type		view;
extern map_type			map;
extern camera_type		camera;
extern setup_type		setup;

/* =======================================================

      Setup Chase Camera Structure
      
======================================================= */

void camera_chase_connect(void)
{
	obj_type		*obj;

	obj=server.obj_list.objs[camera.obj_idx];
	
	memmove(&camera.cur_pos.pnt,&obj->pnt,sizeof(d3pnt));
	
	camera.cur_pos.chase_ang.x=map.camera.chase.track_ang.x;
    camera.cur_pos.chase_ang.y=angle_add(obj->ang.y,map.camera.chase.track_ang.y);
	camera.cur_pos.chase_ang.z=map.camera.chase.track_ang.z;
}

/* =======================================================

      Run Chase Camera
      
======================================================= */

float camera_chase_run_slop(float ang,float goal,float slop,float chase_track_speed)
{
	float		dif;
	bool		cwise;
	
	if ((ang==goal) || (slop==0)) return(goal);
	
		// force angle to within min/max slop
		
	dif=angle_dif(ang,goal,&cwise);
	if (dif>slop) {
		ang=angle_add(goal,(cwise?(-slop):(slop)));
	}
	
		// now turn towards goal
		
	return(angle_turn_toward(ang,goal,chase_track_speed));	
}

void camera_chase_run(void)
{
	float			goal_y;
	obj_type		*obj;

	obj=server.obj_list.objs[camera.obj_idx];
	
	goal_y=angle_add(obj->ang.y,map.camera.chase.track_ang.y);
	camera.cur_pos.chase_ang.y=camera_chase_run_slop(camera.cur_pos.chase_ang.y,goal_y,map.camera.chase.slop.y,map.camera.chase.track_speed);
}

/* =======================================================

      Get Chase Camera Position
      
======================================================= */

void camera_chase_calc_position(void)
{
	int						radius;
	float					fang,fx,fy,fz;
	d3pnt					spt,ept;
	ray_trace_contact_type	contact;
	matrix_type				mat;
	obj_type				*obj;

	obj=server.obj_list.objs[camera.obj_idx];
	
		// get camera starting position

	radius=object_get_radius(obj);
	
	fx=0;
	fy=0;
	fz=(float)(radius>>2);			// always move camera back to protect against hitting walls in front of player
	
	fang=angle_add(camera.cur_pos.chase_ang.x,(obj->view_ang.x*2));
	
	matrix_rotate_zyx(&mat,fang,camera.cur_pos.chase_ang.y,camera.cur_pos.chase_ang.z);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	spt.x=((int)fx)+obj->pnt.x;
	spt.y=((int)fy)+(obj->pnt.y+obj->size.eye_offset);
	spt.z=((int)fz)+obj->pnt.z;
	
		// get camera ending position
		
	fx=0;
	fy=0;
	fz=(float)map.camera.chase.distance;
	
	fang=angle_add(camera.cur_pos.chase_ang.x,-obj->view_ang.x);
	
	matrix_rotate_zyx(&mat,fang,camera.cur_pos.chase_ang.y,camera.cur_pos.chase_ang.z);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	ept.x=((int)fx)+obj->pnt.x;
	ept.y=((int)fy)+(obj->pnt.y+obj->size.eye_offset);
	ept.z=((int)fz)+obj->pnt.z;

		// setup contacts

	contact.obj.on=TRUE;
	contact.obj.ignore_idx=obj->idx;

	contact.proj.on=FALSE;

	contact.origin=poly_ray_trace_origin_unknown;

	ray_trace_map_by_point(&spt,&ept,&contact);
				
		// the position
		
    camera.cur_pos.pnt.x=contact.hpt.x+map.camera.pnt_offset.x;
    camera.cur_pos.pnt.y=contact.hpt.y+map.camera.pnt_offset.y;
    camera.cur_pos.pnt.z=contact.hpt.z+map.camera.pnt_offset.z;
	
	if (obj->bump.on) camera.cur_pos.pnt.y+=obj->bump.smooth_offset;
	
		// looking angles
		// need to reverse X looking angle so it
		// behaves opposite track X angle

	camera.cur_pos.ang.x=(camera.cur_pos.chase_ang.x-map.camera.ang_offset.x)-obj->view_ang.x;
	camera.cur_pos.ang.y=angle_add(camera.cur_pos.chase_ang.y,map.camera.ang_offset.y);
	camera.cur_pos.ang.z=camera.cur_pos.chase_ang.z+map.camera.ang_offset.z;
}

/* =======================================================

      Get Chase Static Camera Position
      
======================================================= */

void camera_chase_static_calc_position(void)
{
	float			fx,fy,fz;
	obj_type		*obj;
	matrix_type		mat;

	obj=server.obj_list.objs[camera.obj_idx];
	
        // new camera position

	fx=0;
	fy=0;
	fz=(float)map.camera.chase.distance;
	
	matrix_rotate_zyx(&mat,map.camera.chase.track_ang.x,map.camera.chase.track_ang.y,map.camera.chase.track_ang.z);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);

	camera.cur_pos.pnt.x=(obj->pnt.x+((int)fx))+map.camera.pnt_offset.x;
	camera.cur_pos.pnt.y=((obj->pnt.y+obj->size.eye_offset)+((int)fy))+map.camera.pnt_offset.y;
	camera.cur_pos.pnt.z=(obj->pnt.z+((int)fz))+map.camera.pnt_offset.z;
	
		// looking angles

	camera.cur_pos.ang.x=map.camera.chase.track_ang.x+map.camera.ang_offset.x;
	if (camera.cur_pos.ang.x>180.0f) camera.cur_pos.ang.x-=360.0f;

	camera.cur_pos.ang.y=map.camera.chase.track_ang.y+map.camera.ang_offset.y;
	camera.cur_pos.ang.z=map.camera.chase.track_ang.z+map.camera.ang_offset.z;
}

