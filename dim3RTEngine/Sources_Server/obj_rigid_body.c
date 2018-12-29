/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Rigid Body

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

extern map_type				map;
extern server_type			server;

/* =======================================================

      Rigid Body Utilities
      
======================================================= */

int object_rigid_body_get_point_y(obj_type *obj,d3pnt *offset,int x_off,int z_off,int y)
{
	int			fy,high;
	d3pnt		pnt;
	
	x_off+=offset->x;
	z_off+=offset->z;
	
	rotate_2D_point_center(&x_off,&z_off,angle_add(obj->ang.y,obj->draw.rot.y));

	pnt.x=obj->pnt.x+x_off;
	pnt.y=y;
	pnt.z=obj->pnt.z+z_off;
	
	high=object_get_radius(obj)*2;
	
	fy=find_poly_nearest_stand(&pnt,high,FALSE);
	if (fy==-1) fy=y;
	
	return(fy);
}

void object_rigid_body_angle_reset_x(obj_type *obj,model_type *mdl)
{
	obj->ang.x*=mdl->rigid_body.x.reset_factor;
	if (fabsf(obj->ang.x)<1.0f) obj->ang.x=0;
}

void object_rigid_body_offset_reset_y(obj_type *obj,model_type *mdl)
{
	float			f;

	if (obj->motion.rigid_body_offset_y==0) return;

	f=(float)obj->motion.rigid_body_offset_y;
	obj->motion.rigid_body_offset_y=(int)(f*mdl->rigid_body.y.reset_factor);
}

void object_rigid_body_angle_reset_z(obj_type *obj,model_type *mdl)
{
	obj->ang.z*=mdl->rigid_body.z.reset_factor;
	if (fabsf(obj->ang.z)<1.0f) obj->ang.z=0;
}

/* =======================================================

      Objects Reseting Angles for Rigid Bodies
      
======================================================= */

void object_rigid_body_reset_angle(obj_type *obj)
{
	int					xsz,zsz,y,ky,off_y,ry,
						fy[4];
	float				x_ang,x_neg_ang,x_pos_ang,
						z_ang,z_neg_ang,z_pos_ang;
	poly_pointer_type	stand_poly;
	model_type			*mdl;

		// is rigid body on?

	if (obj->draw.model_idx==-1) return;
	mdl=server.model_list.models[obj->draw.model_idx];

	if (!mdl->rigid_body.on) return;

		// if in air, reset angles

	if (obj->air_mode!=am_ground) {
		object_rigid_body_offset_reset_y(obj,mdl);
		object_rigid_body_angle_reset_x(obj,mdl);
		object_rigid_body_angle_reset_z(obj,mdl);
		return;
	}
	
		// move middle of object to closest ground

	y=obj->pnt.y;

	if (obj->contact.stand_obj_idx!=-1) {
		object_rigid_body_offset_reset_y(obj,mdl);			// no movement if standing on object
	}
	else {

		ky=pin_downward_movement_point(&obj->pnt,obj->size.y,&stand_poly);
		if (stand_poly.mesh_idx!=-1) {

			if (ky>y) {

					// reset draw offset (with smoothing)

				off_y=ky-y;
				ry=obj->motion.rigid_body_offset_y;

				obj->motion.rigid_body_offset_y=ry+(int)((float)(off_y-ry)*mdl->rigid_body.y.smooth_factor);

					// set y for angle changes

				y=ky;
			}
		}
	}

		// get rigid y points around
		// around view box

	xsz=mdl->view_box.size.x>>1;
	zsz=mdl->view_box.size.z>>1;

	fy[0]=object_rigid_body_get_point_y(obj,&mdl->view_box.offset,-xsz,-zsz,y);
	fy[1]=object_rigid_body_get_point_y(obj,&mdl->view_box.offset,xsz,-zsz,y);
	fy[2]=object_rigid_body_get_point_y(obj,&mdl->view_box.offset,xsz,zsz,y);
	fy[3]=object_rigid_body_get_point_y(obj,&mdl->view_box.offset,-xsz,zsz,y);
	
		// if all the same, skip out
		
	if ((fy[0]==fy[1]) && (fy[0]==fy[2]) && (fy[0]==fy[3])) {
		object_rigid_body_offset_reset_y(obj,mdl);
		object_rigid_body_angle_reset_x(obj,mdl);
		object_rigid_body_angle_reset_z(obj,mdl);
		return;
	}
	
		// find angles
	
	x_neg_ang=x_pos_ang=0.0f;
	
	if ((fy[0]!=-1) && (fy[3]!=-1)) x_neg_ang=angle_find(0,0,obj->size.z,abs(fy[0]-fy[3]))-90.0f;
	if ((fy[1]!=-1) && (fy[2]!=-1)) x_pos_ang=angle_find(0,0,obj->size.z,abs(fy[1]-fy[2]))-90.0f;

	z_neg_ang=z_pos_ang=0.0f;

	if ((fy[3]!=-1) && (fy[2]!=-1)) z_neg_ang=angle_find(0,0,obj->size.x,abs(fy[3]-fy[2]))-90.0f;
	if ((fy[0]!=-1) && (fy[1]!=-1)) z_pos_ang=angle_find(0,0,obj->size.x,abs(fy[0]-fy[1]))-90.0f;
	
	x_ang=x_neg_ang;
	if (x_pos_ang>x_ang) x_ang=x_pos_ang;
	if (x_ang>mdl->rigid_body.x.max_ang) x_ang=mdl->rigid_body.x.max_ang;
	if ((y<fy[0]) || (y<fy[1])) x_ang=-x_ang;

	z_ang=z_neg_ang;
	if (z_pos_ang>z_ang) z_ang=z_pos_ang;
	if (z_ang>mdl->rigid_body.z.max_ang) z_ang=mdl->rigid_body.z.max_ang;
	if ((y<fy[0]) || (y<fy[3])) z_ang=-z_ang;
	
		// set angle (smooth out drastic angle changes)
		
	if (fabsf(obj->ang.x-x_ang)<1.0f) {
		obj->ang.x=x_ang;
	}
	else {
		obj->ang.x=obj->ang.x+((x_ang-obj->ang.x)*mdl->rigid_body.x.smooth_factor);
	}

	z_ang=-z_ang;
	
	if (fabsf(obj->ang.z-z_ang)<1.0f) {
		obj->ang.z=z_ang;
	}
	else {
		obj->ang.z=obj->ang.z+((z_ang-obj->ang.z)*mdl->rigid_body.z.smooth_factor);
	}
}

