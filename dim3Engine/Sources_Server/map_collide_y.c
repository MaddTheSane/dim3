/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Y Collisions

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

#define map_collide_pin_radius_len			300

extern map_type			map;
extern server_type		server;

d3fpnt					pin_trig_table[2][16];
d3pnt					pin_movement_spt[129],pin_movement_ept[129];
ray_trace_contact_type	pin_movement_contacts[129];

/* =======================================================

      Find Next Nearest Stand Poly Point
      
======================================================= */

int find_poly_nearest_stand(d3pnt *pnt,int my,bool ignore_higher)
{
	d3pnt					spt,ept;
	ray_trace_contact_type	contact;

	spt.x=pnt->x;
	spt.y=pnt->y;
	if (!ignore_higher) spt.y-=my;
	spt.z=pnt->z;

	ept.x=pnt->x;
	ept.y=pnt->y+my;
	ept.z=pnt->z;

	contact.obj.on=FALSE;
	contact.proj.on=FALSE;

	contact.origin=poly_ray_trace_origin_unknown;

	if (ray_trace_map_by_point(&spt,&ept,&contact)) return(contact.hpt.y);

	return(-1);
}

/* =======================================================

      Built Pin X/Z rays
      
======================================================= */

void pin_build_trig_table(void)
{
	int				n;
	float			rad,rad_shift,rad_add;

		// this precreates the circular rays
		// that pin up/down movement.  They are
		// constructed of a number of circles, or which
		// we slightly offset every other one
		// as to better hit thin polygons

	rad=0.0f;
	rad_shift=(TRIG_PI*2.0f)/(16.0f*3.0f);
	rad_add=(TRIG_PI*2.0f)/16.0f;

	for (n=0;n!=16;n++) {
		pin_trig_table[0][n].x=sinf(rad);
		pin_trig_table[0][n].z=cosf(rad);
		
		pin_trig_table[1][n].x=sinf(rad+rad_shift);
		pin_trig_table[1][n].z=cosf(rad+rad_shift);
		
		rad+=rad_add;
	}
}

int pin_build_ray_set_obj(obj_type *obj,int start_y,int end_y,d3pnt *bounds_min,d3pnt *bounds_max)
{
	int				n,k,x,z,radius,
					circle_count,ray_count;
	float			f_radius,f_radius_sub;
	d3fpnt			*pin_trig;

		// get radius
		
	radius=object_get_radius(obj);

		// get center

	x=obj->pnt.x;
	z=obj->pnt.z;

		// always add one for center
	
	pin_movement_spt[0].x=pin_movement_ept[0].x=x;
	pin_movement_spt[0].z=pin_movement_ept[0].z=z;
	pin_movement_spt[0].y=start_y;
	pin_movement_ept[0].y=end_y;
	
		// if simplified, just one ray
		
	if (obj->contact.collision_mode==collision_mode_simple) return(1);

		// how many concentric circles
		// always leave one more spot
		// for center ray (only do at most 8 circles
		// of 16 points each)

	circle_count=radius/map_collide_pin_radius_len;
	if (circle_count<2) circle_count=2;
	if (circle_count>8) circle_count=8;

	f_radius_sub=(float)radius/(float)circle_count;

		// create circles
		
	ray_count=1;

	f_radius=(float)radius;

	for (n=0;n!=circle_count;n++) {

		pin_trig=pin_trig_table[n&0x1];

		for (k=0;k!=16;k++) {
			pin_movement_spt[ray_count].x=pin_movement_ept[ray_count].x=x+(int)(f_radius*pin_trig->x);
			pin_movement_spt[ray_count].z=pin_movement_ept[ray_count].z=z-(int)(f_radius*pin_trig->z);
			pin_movement_spt[ray_count].y=start_y;
			pin_movement_ept[ray_count].y=end_y;

			pin_trig++;
			ray_count++;
		}

		f_radius-=f_radius_sub;
	}

		// and the min/max ray bounds

	bounds_min->x=x-radius;
	bounds_max->x=x+radius;
	if (start_y<end_y) {
		bounds_min->y=start_y;
		bounds_max->y=end_y;
	}
	else {
		bounds_min->y=end_y;
		bounds_max->y=start_y;
	}
	bounds_min->z=z-radius;
	bounds_max->z=z+radius;

	return(ray_count);
}

/* =======================================================

      Pin Downward Movements
      
======================================================= */

int pin_downward_movement_point(d3pnt *pnt,int my,poly_pointer_type *stand_poly)
{
	d3pnt					spt,ept;
	ray_trace_contact_type	contact;

	spt.x=pnt->x;
	spt.y=pnt->y-my;
	spt.z=pnt->z;

	ept.x=pnt->x;
	ept.y=pnt->y+my;
	ept.z=pnt->z;
	
	contact.obj.on=FALSE;
	contact.proj.on=FALSE;

	contact.origin=poly_ray_trace_origin_unknown;

	if (ray_trace_map_by_point(&spt,&ept,&contact)) {
		stand_poly->mesh_idx=contact.poly.mesh_idx;
		stand_poly->poly_idx=contact.poly.poly_idx;
		return(contact.hpt.y);
	}

	stand_poly->mesh_idx=-1;

	return(pnt->y+my);
}

int pin_downward_movement_obj(obj_type *obj,bool set_obj_contact,int my)
{
	int						n,cy,ray_count,
							start_y,end_y;
	d3pnt					bounds_min,bounds_max;
	ray_trace_contact_type	base_contact;
	obj_type				*stand_obj;
	
		// setup contact
		
	base_contact.obj.on=TRUE;
	base_contact.obj.ignore_idx=obj->idx;
	base_contact.proj.on=FALSE;

	base_contact.origin=poly_ray_trace_origin_unknown;

		// create the rays

	start_y=obj->pnt.y-my;
	end_y=obj->pnt.y+my;
	ray_count=pin_build_ray_set_obj(obj,start_y,end_y,&bounds_min,&bounds_max);

		// run the rays
		
	if (ray_count==1) {
		ray_trace_map_by_point(&pin_movement_spt[0],&pin_movement_ept[0],&pin_movement_contacts[0]);
	}
	else {
		ray_trace_map_by_point_array(ray_count,&bounds_min,&bounds_max,pin_movement_spt,pin_movement_ept,&base_contact,pin_movement_contacts);
	}
	
		// find the highest point
		
	obj->contact.stand_obj_idx=-1;
	obj->contact.stand_poly.mesh_idx=-1;
	
	cy=-1;
	
	for (n=0;n!=ray_count;n++) {
		
			// check poly collisions
			
		if (pin_movement_contacts[n].hit) {
		
			if ((cy==-1) || (pin_movement_contacts[n].hpt.y<cy)) {
			
					// always collide with poly

				obj->contact.stand_poly.mesh_idx=pin_movement_contacts[n].poly.mesh_idx;
				obj->contact.stand_poly.poly_idx=pin_movement_contacts[n].poly.poly_idx;
				
					// stand on object
				
				if ((pin_movement_contacts[n].obj.idx!=-1) && (set_obj_contact)) {
					stand_obj=server.obj_list.objs[pin_movement_contacts[n].obj.idx];
					if ((obj->pnt.y-my)<(stand_obj->pnt.y-stand_obj->size.y)) obj->contact.stand_obj_idx=pin_movement_contacts[n].obj.idx;
				}
				
				cy=pin_movement_contacts[n].hpt.y;
			}
			
		}
	}
	
	if (cy==-1) return(my);
	
	return(cy-obj->pnt.y);
}

int pin_downward_movement_proj(proj_type *proj,int my)
{
	d3pnt					spt,ept;
	ray_trace_contact_type	contact;

	spt.x=proj->pnt.x;
	spt.y=(proj->pnt.y-proj->size.y)-my;
	spt.z=proj->pnt.z;

	ept.x=proj->pnt.x;
	ept.y=proj->pnt.y+my;
	ept.z=proj->pnt.z;
	
	contact.obj.on=FALSE;
	contact.proj.on=FALSE;

	contact.origin=poly_ray_trace_origin_projectile;

	if (ray_trace_map_by_point(&spt,&ept,&contact)) {
		proj->contact.hit_poly.mesh_idx=contact.poly.mesh_idx;
		proj->contact.hit_poly.poly_idx=contact.poly.poly_idx;
		return(contact.hpt.y);
	}

	return(proj->pnt.y+my);
}

/* =======================================================

      Pin Upward Movements
      
======================================================= */

int pin_upward_movement_point(d3pnt *pnt,int my,poly_pointer_type *head_poly)
{
	d3pnt					spt,ept;
	ray_trace_contact_type	contact;

	spt.x=pnt->x;
	spt.y=pnt->y-my;
	spt.z=pnt->z;

	ept.x=pnt->x;
	ept.y=pnt->y+my;
	ept.z=pnt->z;

	contact.obj.on=FALSE;
	contact.proj.on=FALSE;

	contact.origin=poly_ray_trace_origin_unknown;

	if (ray_trace_map_by_point(&spt,&ept,&contact)) {
		head_poly->mesh_idx=contact.poly.mesh_idx;
		head_poly->poly_idx=contact.poly.poly_idx;
		return(contact.hpt.y);
	}

	head_poly->mesh_idx=-1;

	return(pnt->y-my);
}

int pin_upward_movement_obj(obj_type *obj,int my)
{
	int						n,cy,y_sz,ray_count,
							start_y,end_y;
	d3pnt					bounds_min,bounds_max;
	ray_trace_contact_type	base_contact;
	obj_type				*head_obj;

		// setup contact
		
	base_contact.obj.on=TRUE;
	base_contact.obj.ignore_idx=obj->idx;
	base_contact.proj.on=FALSE;

	base_contact.origin=poly_ray_trace_origin_unknown;

		// create ray arrays

	y_sz=obj->size.y;
	if (obj->duck.mode!=dm_stand) y_sz-=obj->duck.y_move;
	start_y=(obj->pnt.y-y_sz)-my;
	end_y=(obj->pnt.y-y_sz)+my;

	ray_count=pin_build_ray_set_obj(obj,start_y,end_y,&bounds_min,&bounds_max);

		// run the rays
		
	if (ray_count==1) {
		ray_trace_map_by_point(&pin_movement_spt[0],&pin_movement_ept[0],&pin_movement_contacts[0]);
	}
	else {
		ray_trace_map_by_point_array(ray_count,&bounds_min,&bounds_max,pin_movement_spt,pin_movement_ept,&base_contact,pin_movement_contacts);
	}
	
		// find the lowest point
		
	obj->contact.head_obj_idx=-1;
	obj->contact.head_poly.mesh_idx=-1;
	
	cy=-1;
	
	for (n=0;n!=ray_count;n++) {
		
			// check poly collisions
			
		if (pin_movement_contacts[n].hit) {
		
			if ((cy==-1) || (pin_movement_contacts[n].hpt.y>cy)) {
			
					// always collide with poly
					
				obj->contact.head_poly.mesh_idx=pin_movement_contacts[n].poly.mesh_idx;
				obj->contact.head_poly.poly_idx=pin_movement_contacts[n].poly.poly_idx;
				
					// only head on object if touching it
				
				if (pin_movement_contacts[n].obj.idx!=-1) {
					head_obj=server.obj_list.objs[pin_movement_contacts[n].obj.idx];
					obj->contact.head_obj_idx=pin_movement_contacts[n].obj.idx;
				}
				
				cy=pin_movement_contacts[n].hpt.y;
			}			
		}
	}
	
	if (cy==-1) return(my);

	return(cy-(obj->pnt.y-y_sz));
}

int pin_upward_movement_proj(proj_type *proj,int my)
{
	d3pnt					spt,ept;
	ray_trace_contact_type	contact;

	spt.x=proj->pnt.x;
	spt.y=(proj->pnt.y-proj->size.y)-my;
	spt.z=proj->pnt.z;

	ept.x=proj->pnt.x;
	ept.y=proj->pnt.y+my;
	ept.z=proj->pnt.z;
	
	contact.obj.on=FALSE;
	contact.proj.on=FALSE;

	contact.origin=poly_ray_trace_origin_projectile;

	if (ray_trace_map_by_point(&spt,&ept,&contact)) {
		proj->contact.hit_poly.mesh_idx=contact.poly.mesh_idx;
		proj->contact.hit_poly.poly_idx=contact.poly.poly_idx;
		return(contact.hpt.y);
	}

	return(proj->pnt.y-my);
}

/* =======================================================

      Check if Object is being Crushed from Above
      
======================================================= */

bool map_stand_crush_object(obj_type *obj)
{
	int					y,fudge;
	
		// pick a crushing fudge, reduce size and
		// see if you can move up that far

	fudge=obj->size.y>>3;
	
	obj->size.y-=fudge;
	y=pin_upward_movement_obj(obj,fudge);
	obj->size.y+=fudge;
	
	if (obj->contact.head_poly.mesh_idx==-1) return(FALSE);
	
		// can only be crushed by moveable meshes
		
	if (!map.mesh.meshes[obj->contact.head_poly.mesh_idx].flag.moveable) return(FALSE);
	
		// if we can, go into auto duck before crushing
		// if already in duck, crush
		
	if (obj->duck.mode==dm_duck) return(TRUE);
	
	object_start_duck(obj);
	
	return(FALSE);
}

/* =======================================================

      Check if Standing Up is Possible
      
======================================================= */

bool map_stand_check_object(obj_type *obj)
{
	int					y,sy;

		// total stand up height

	sy=obj->duck.y_change;

		// possible to move up?

	y=pin_upward_movement_obj(obj,sy);
	return(obj->contact.head_poly.mesh_idx==-1);
}

