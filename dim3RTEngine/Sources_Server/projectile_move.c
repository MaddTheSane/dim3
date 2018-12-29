/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Projectile Movement

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
#include "scripts.h"

extern app_type				app;
extern map_type				map;
extern server_type			server;
extern view_type			view;

/* =======================================================

      Projectile Slice Setup
      
======================================================= */

void projectile_slice_setup(proj_type *proj)
{
		// counts
		
	proj->count++;
	
	if (proj->parent_grace>0) proj->parent_grace--;
		
		// old position

	proj->last_pnt.x=proj->pnt.x;
	proj->last_pnt.y=proj->pnt.y;
	proj->last_pnt.z=proj->pnt.z;
}

/* =======================================================

      Get Projectile Motion
      
======================================================= */

void projectile_set_motion(proj_type *proj,float speed,float ang_y,float ang_x,d3pnt *motion)
{
	float			fx,fy,fz;
	matrix_type		mat;
	
		// sticking
		
	if (proj->stick) {
		motion->x=0;
		motion->y=0;
		motion->z=0;
		return;
	}
	
		// get motion
		
	fx=fy=0;
	fz=-speed;
	
	matrix_rotate_x(&mat,ang_x);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	matrix_rotate_y(&mat,ang_y);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
		// add in the forces
		
	fx+=proj->force.vct.x;
	fy+=proj->force.vct.y;
	fz+=proj->force.vct.z;

	fy+=proj->gravity_add;
	
		// save for projectile
		
	proj->motion.vct.x=fx;
	proj->motion.vct.z=fz;
	proj->motion.vct.y=fy;
	
		// return as integers
	
	motion->x=(int)fx;
	motion->y=(int)fy;
	motion->z=(int)fz;
}

/* =======================================================

      Increase Gravity
      
======================================================= */

void projectile_gravity(proj_type *proj)
{
	float			gravity;
    
		// increase the gravity
	
	gravity=proj->force.gravity;
	if (gravity<map.physics.gravity_max_power) {
		gravity=gravity+((float)proj->size.weight*(map.physics.gravity/gravity_factor));
		if (gravity>map.physics.gravity_max_power) gravity=map.physics.gravity_max_power;
		proj->force.gravity=gravity;
	}
	
		// positive gravity drags down the Y move
	
	if (gravity>0) proj->gravity_add+=gravity;
}

/* =======================================================

      Projectile Speed
      
======================================================= */

void projectile_speed(proj_type *proj)
{
	d3vct			*vct;

		// are we within the grace period?

	if (proj->decel_grace>0) {
		proj->decel_grace--;
		return;
	}

	if (proj->decel_speed==0) return;

		// reduce any forces

	vct=&proj->force.vct;
		
	if (vct->x!=0) {
		if (vct->x>0) {
			vct->x-=proj->decel_speed;
			if (vct->x<0) vct->x=0;
		}
		else {
			vct->x+=proj->decel_speed;
			if (vct->x>0) vct->x=0;
		}
	}

	if (vct->y!=0) {
		if (vct->y>0) {
			vct->y-=proj->decel_speed;
			if (vct->y<0) vct->y=0;
		}
		else {
			vct->y+=proj->decel_speed;
			if (vct->y>0) vct->y=0;
		}
	}
	
	if (vct->z!=0) {
		if (vct->z>0) {
			vct->z-=proj->decel_speed;
			if (vct->z<0) vct->z=0;
		}
		else {
			vct->z+=proj->decel_speed;
			if (vct->z>0) vct->z=0;
		}
	}

		// alter the speed

	proj->speed=proj->speed-proj->decel_speed;
	if (proj->speed<proj->decel_min_speed) proj->speed=proj->decel_min_speed;
}

/* =======================================================

      Do Movement
      
======================================================= */

void projectile_move(proj_type *proj)
{
	bool				wall_hit;
	d3pnt				motion;
	
	object_clear_contact(&proj->contact);
	
		// project movement
		
	projectile_set_motion(proj,proj->speed,proj->motion.ang.y,proj->motion.ang.x,&motion);
	
        // xz move

	collide_projectile_to_map(proj,&motion);

	proj->pnt.x+=motion.x;
	proj->pnt.z+=motion.z;

		// y move

	if (motion.y!=0) {
		if (motion.y<0) {
			proj->pnt.y=pin_upward_movement_proj(proj,abs(motion.y));
		}
		else {
			proj->pnt.y=pin_downward_movement_proj(proj,motion.y);
		}
	}
	
		// sticking ignores poly hits
		
	if (proj->stick) proj->contact.hit_poly.mesh_idx=-1;
	
		// hits
		
	if ((proj->contact.hit_poly.mesh_idx==-1) && (proj->contact.obj_idx==-1) && (proj->contact.proj_idx==-1)) return;
	
		// auto-bounces and reflects
		
	if (proj->contact.hit_poly.mesh_idx!=-1) {
		wall_hit=collide_contact_is_wall_hit(&proj->contact.hit_poly);

		if ((proj->action.reflect) && (wall_hit)) {
			projectile_reflect(proj,TRUE);
			return;
		}
		
		if ((proj->action.bounce) && (!wall_hit)) {
			if (!projectile_bounce(proj,proj->action.bounce_min_move,proj->action.bounce_reduce,TRUE)) return;
		}
	}
	
		// run hit
		
	projectile_hit(proj);
}

/* =======================================================

      Turn and Seek
      
======================================================= */

void projectile_turn_xz_towards(proj_type *proj,obj_type *to_obj,float turn_add)
{
	float			ang;
	
	ang=angle_find(proj->pnt.x,proj->pnt.z,to_obj->pnt.x,to_obj->pnt.z);
	proj->ang.y=proj->motion.ang.y=angle_turn_toward(proj->ang.y,ang,turn_add);
}

void projectile_thrust_y_towards(proj_type *proj,obj_type *to_obj,float thrust_add)
{
	int			py,oy,dist;
	float		rang;
	
		// find new thrust angle

	py=proj->pnt.y-(proj->size.y>>1);
	oy=to_obj->pnt.y-(to_obj->size.y>>1);

	dist=distance_2D_get(proj->pnt.x,proj->pnt.z,to_obj->pnt.x,to_obj->pnt.z);
	rang=180.0f-angle_find(py,0,oy,dist);

		// smooth out changes

	proj->motion.ang.x=(proj->motion.ang.x-rang)/2.0f;
}

void projectile_seek(proj_type *proj,obj_type *to_obj,float turn_add,float thrust_add)
{
	projectile_turn_xz_towards(proj,to_obj,turn_add);
	projectile_thrust_y_towards(proj,to_obj,thrust_add);
}

/* =======================================================

      Projectile Bounce
      
======================================================= */

bool projectile_bounce(proj_type *proj,float min_ymove,float reduce,bool send_event)
{
	float				fy,slope_y;
	d3pnt				motion;
	poly_pointer_type	*poly;
	
		// get polygon slope

	poly=&proj->contact.hit_poly;
	slope_y=0.0f;

	if (poly->mesh_idx!=-1) slope_y=map.mesh.meshes[poly->mesh_idx].polys[poly->poly_idx].slope.y;

		// reset to last good position

	memmove(&proj->pnt,&proj->last_pnt,sizeof(d3pnt));

		// if floor is over a certain slope, just reflect instead

	if (slope_y>=proj_reflect_slope_max_y) {
		proj->ang.y=proj->motion.ang.y=angle_add(proj->motion.ang.y,180);
		return(FALSE);
	}

		// if any slope, slow down projectile
		// if slowed down to nothing, then bounce is over

	if (slope_y!=0.0f) {
		proj->speed*=(1.0f-(slope_y/proj_reflect_slope_max_y));
	}

	if (proj->speed<proj_bounce_min_speed) return(TRUE);

		// bounce projectile
	
	projectile_set_motion(proj,proj->speed,proj->motion.ang.y,proj->motion.ang.x,&motion);
		
	fy=((float)motion.y)*reduce;
	if (fy<min_ymove) return(TRUE);
		
	proj->force.vct.y=-fy;
	proj->motion.ang.x=0.0f;

	proj->force.gravity*=reduce;
	proj->gravity_add=0.0f;
	
	proj->pnt.y-=(int)fy;

	if (send_event) scripts_post_event_console(proj->script_idx,proj->idx,sd_event_projectile,sd_event_projectile_bounce,0);
	
	return(FALSE);
}

/* =======================================================

      Projectile Reflect
      
======================================================= */

float projectile_reflect_angle(proj_type *proj)
{
	float				f,ang;
	bool				wall_hit;
	d3pnt				motion;
	d3vct				proj_vct,normal_vct;
	poly_pointer_type	*poly;
	map_mesh_poly_type	*mesh_poly;

		// get information about collided mesh

	poly=&proj->contact.hit_poly;

	mesh_poly=NULL;
	wall_hit=FALSE;
	
	if (poly->mesh_idx!=-1) {
		mesh_poly=&map.mesh.meshes[poly->mesh_idx].polys[poly->poly_idx];
		wall_hit=mesh_poly->box.wall_like;
	}
	
		// if not wall-like, then just reverse
		
	if ((mesh_poly==NULL) || (!wall_hit)) {
		return(proj->motion.ang.y=angle_add(proj->motion.ang.y,180.0f));
	}

		// get the projectile and polygon vector

	projectile_set_motion(proj,1000,proj->motion.ang.y,0,&motion);
	
	proj_vct.x=(float)motion.x;			// projectile vector
	proj_vct.y=0.0f;
	proj_vct.z=(float)motion.z;
	vector_normalize(&proj_vct);
	
	vector_create(&normal_vct,mesh_poly->box.min.x,0,mesh_poly->box.min.z,mesh_poly->box.max.x,0,mesh_poly->box.max.z);		// perpendicular vector (swap x/z)
	
		// get the angle between them

	f=vector_dot_product(&proj_vct,&normal_vct);
	f=acosf(f)*RAD_to_ANG;

	if (f>90.0f) f=180.0f-f;							// always use the most shallow angle

		// calculate the reflection angle
		
	ang=angle_add(proj->motion.ang.y,-(180.0f-(f*2)));
	if (ang==proj->ang.y) ang=angle_add(ang,180.0f);	// special check for straight hits

	return(ang);
}

void projectile_reflect(proj_type *proj,bool send_event)
{
		// setup the reflect

	memmove(&proj->pnt,&proj->last_pnt,sizeof(d3pnt));
	proj->ang.y=proj->motion.ang.y=projectile_reflect_angle(proj);
	
		// send event

	if (send_event) scripts_post_event_console(proj->script_idx,proj->idx,sd_event_projectile,sd_event_projectile_reflect,0);
}

/* =======================================================

      Reset Projectile Angles
      
======================================================= */

void projectile_reset_angle_for_flight(proj_type *proj)
{
	int				dist;
	float			f,fx,fz,fy;
	
	if (!proj->reset_angle) return;

		// get projectile motion

	fx=proj->motion.vct.x;
	fz=proj->motion.vct.z;
	fy=proj->motion.vct.y;
	
		// rest angle if moving down
		
	f=(fx*fx)+(fz*fz);
	dist=(int)sqrtf(f);
	proj->ang.x=angle_add(180,angle_find(0,0,(int)fy,dist));
}

/* =======================================================

      Hit Vectors
      
======================================================= */

void projectile_eject_vector(proj_type *proj,d3vct *vct)
{
	float				wall_ang;
	d3pnt				motion;
	poly_pointer_type	*poly;
	map_mesh_poly_type	*mesh_poly;

		// if it hit anything other than a polygon
		// or we are in dedicated mode (no camera),
		// then just eject it backwards

	poly=&proj->contact.hit_poly;
		
	if ((poly->mesh_idx==-1) || (app.dedicated_host)) {
		projectile_set_motion(proj,1000,proj->motion.ang.y,proj->motion.ang.x,&motion);
	
		vct->x=-(float)motion.x;
		vct->y=-(float)motion.y;
		vct->z=-(float)motion.z;
		vector_normalize(vct);
		
		return;
	}
	
		// if the polygon is floor like, eject towards camera
		// either up or down
	
	mesh_poly=&map.mesh.meshes[poly->mesh_idx].polys[poly->poly_idx];
		
	if (!mesh_poly->box.wall_like) {
		if (view.render->camera.pnt.y<proj->pnt.y) {
			vct->y=-1.0f;
		}
		else {
			vct->y=1.0f;
		}
		vct->x=vct->z=0.0f;
		return;
	}
	
		// else eject at angle of wall
		// always eject towards the camera
		
	wall_ang=angle_find(mesh_poly->box.min.x,mesh_poly->box.min.z,mesh_poly->box.max.x,mesh_poly->box.max.z);
	wall_ang=angle_add(wall_ang,90.0f);

	angle_get_movement(wall_ang,1000,&motion.x,&motion.z);
	
	motion.x=abs(motion.x);
	if (view.render->camera.pnt.x<proj->pnt.x) motion.x=-motion.x;
	
	motion.z=abs(motion.z);
	if (view.render->camera.pnt.z<proj->pnt.z) motion.z=-motion.z;
	
	vct->x=(float)motion.x;
	vct->y=0;
	vct->z=(float)motion.z;

	vector_normalize(vct);
}

void projectile_reflect_vector(proj_type *proj,d3vct *vct)
{
	float			ang_y;
	d3pnt			motion;

	ang_y=projectile_reflect_angle(proj);

	projectile_set_motion(proj,1000,ang_y,proj->motion.ang.x,&motion);
	
	vct->x=(float)motion.x;
	vct->y=(float)motion.y;
	vct->z=(float)motion.z;
	vector_normalize(vct);
}


