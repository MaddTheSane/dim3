/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object External Movement

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

      Objects External Movement Utilities
      
======================================================= */

static inline void object_move_get_turn_motion(obj_type *obj,d3pnt *mpt,float rot_y,d3pnt *turn_motion)
{
	float				fx,fy,fz;
	matrix_type			mat;
	
	fx=(float)(obj->pnt.x-mpt->x);
	fy=0.0f;
	fz=(float)(obj->pnt.z-mpt->z);
	
	matrix_rotate_y(&mat,rot_y);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);

	turn_motion->x=(((int)fx)+mpt->x)-obj->pnt.x;
	turn_motion->y=0;
	turn_motion->z=(((int)fz)+mpt->z)-obj->pnt.z;
}

static inline void object_move_with_motion(obj_type *obj,d3pnt *motion,int crush_mesh_idx,bool no_crush)
{
		// collide with map and crush if blocked
		
	if (collide_object_to_map(obj,motion)) {
		if (no_crush) return;
		if ((obj->contact.hit_poly.mesh_idx!=-1) && (obj->contact.hit_poly.mesh_idx!=crush_mesh_idx)) {
			object_crush(obj,TRUE);
			return;
		}
	}
	
		// otherwise move
		
	object_motion_lock(obj,motion);

	obj->pnt.x+=motion->x;
	obj->pnt.y+=motion->y;
	obj->pnt.z+=motion->z;
}

/* =======================================================

      Objects Moving by Meshes
      
======================================================= */

void object_move_with_mesh(int mesh_idx,d3pnt *motion)
{
	int			n;
	d3pnt		move_motion;
	obj_type	*obj;

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
		
			// stand on or collide with mesh?
			
		if ((obj->contact.stand_poly.mesh_idx==mesh_idx) || (collide_object_to_mesh(obj,mesh_idx))) {
		
			move_motion.x=motion->x;
			move_motion.y=0;
			move_motion.z=motion->z;
			
			object_move_with_motion(obj,&move_motion,mesh_idx,FALSE);
		}
	}
}

void object_rotate_with_mesh(int mesh_idx,float rot_y)
{
	int				n;
	bool			no_crush;
	d3pnt			mpt,turn_motion;
	map_mesh_type	*mesh;
	obj_type		*obj;

		// get center point for mesh

	mesh=&map.mesh.meshes[mesh_idx];

	map_mesh_calculate_center(&map,mesh_idx,&mpt);
	mpt.x+=mesh->rot_off.x;
	mpt.y+=mesh->rot_off.y;
	mpt.z+=mesh->rot_off.z;

		// check objects interacting with mesh

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
		
			// stand on or collide with mesh?
			
		if ((obj->contact.stand_poly.mesh_idx==mesh_idx) || (collide_object_to_mesh(obj,mesh_idx))) {
			object_move_get_turn_motion(obj,&mpt,rot_y,&turn_motion);
			no_crush=(obj->contact.stand_poly.mesh_idx==mesh_idx);		// if we are being turned by a moving object, then no crush
			object_move_with_motion(obj,&turn_motion,mesh_idx,no_crush);
			obj->ang.y=angle_add(obj->ang.y,rot_y);
		}
	}
}

/* =======================================================

      Objects Moving by Other Objects
      
======================================================= */

bool object_push_object_allowed(obj_type *obj,obj_type *push_obj)
{
	int			weight_dif;

	if (!push_obj->contact.pushable) return(FALSE);

		// compare weights

	weight_dif=obj->size.weight-push_obj->size.weight;
	return(weight_dif>=0);
}

bool object_push_with_object(obj_type *obj,d3pnt *motion)
{
	int				weight_dif;
	float			f,f_xmove,f_zmove;
	bool			contact_on;
	d3pnt			move_motion;
	obj_type		*pushed_obj;
	
	if (obj->contact.obj_idx==-1) return(FALSE);
	
		// get object to push

	pushed_obj=server.obj_list.objs[obj->contact.obj_idx];
	if (pushed_obj==NULL) return(FALSE);

	if (!pushed_obj->contact.pushable) return(FALSE);

		// compare weights

	weight_dif=obj->size.weight-pushed_obj->size.weight;
	if (weight_dif<0) return(FALSE);

		// calculate the force by weight

	f=0.25f+(((float)weight_dif)/((float)obj->size.weight));
	if (f>1.0f) f=1.0f;

		// slow down pushing object

	obj->forward_move.speed*=f;
	obj->side_move.speed*=f;
	
		// add in vector

	f_xmove=((float)motion->x)*f;
	f_zmove=((float)motion->z)*f;

	f_xmove=(pushed_obj->force.vct.x+f_xmove)/2.0f;
	f_zmove=(pushed_obj->force.vct.z+f_zmove)/2.0f;
	
	pushed_obj->force.vct.x=f_xmove;
	pushed_obj->force.vct.z=f_zmove;

		// movement, turn off contact for
		// pushing object so pushed object
		// doesn't collide with it

	contact_on=obj->contact.object_on;
	obj->contact.object_on=FALSE;
	
	move_motion.x=(int)f_xmove;
	move_motion.y=0;
	move_motion.z=(int)f_zmove;
	
	object_move_with_motion(pushed_obj,&move_motion,-1,FALSE);

	obj->contact.object_on=contact_on;

	return(TRUE);
}

void object_move_with_standing_object(obj_type *obj,d3pnt *motion,bool y_only)
{
	int			n;
	d3pnt		move_motion;
	obj_type	*obj_check;
	
	for (n=0;n!=max_obj_list;n++) {
		obj_check=server.obj_list.objs[n];
		if (obj_check==NULL) continue;

		if (obj_check->contact.stand_obj_idx!=obj->idx) continue;
		if (obj_check->suspend) continue;
		
		if (!y_only) {
			move_motion.x=motion->x;
			move_motion.y=0;
			move_motion.z=motion->z;
		}
		else {
			move_motion.x=0;
			move_motion.y=motion->y;
			move_motion.z=0;
		}

		object_move_with_motion(obj_check,&move_motion,-1,TRUE);
	}
}

void object_rotate_with_standing_object(obj_type *obj,float rot_y)
{
	int				n;
	d3pnt			mpt,turn_motion;
	obj_type		*obj_check;

		// get center point for object

	mpt.x=obj->pnt.x;
	mpt.y=obj->pnt.y;
	mpt.z=obj->pnt.z;

		// turn any object standing on it
	
	for (n=0;n!=max_obj_list;n++) {
		obj_check=server.obj_list.objs[n];
		if (obj_check==NULL) continue;

		if (obj_check->contact.stand_obj_idx!=obj->idx) continue;
		if (obj_check->suspend) continue;
			
		object_move_get_turn_motion(obj_check,&mpt,rot_y,&turn_motion);
		object_move_with_motion(obj_check,&turn_motion,-1,TRUE);
		obj_check->ang.y=angle_add(obj_check->ang.y,rot_y);
	}
}
