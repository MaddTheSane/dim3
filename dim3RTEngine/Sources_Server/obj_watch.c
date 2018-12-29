/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Watching

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
extern js_type				js;

/* =======================================================

      Watch Setup
      
======================================================= */

void object_watch_setup(obj_type *obj,int watch_obj_idx,int base_team,char *sound_name,d3pnt *pnt)
{
	obj_type		*watch_obj;

		// object ID, team,
		// and sound

	obj->watch.obj_idx=watch_obj_idx;
	obj->watch.base_team=base_team;
	if (sound_name==NULL) {
		obj->watch.sound_name[0]=0x0;
	}
	else {
		strcpy(obj->watch.sound_name,sound_name);
	}

		// position and angle

	if (pnt!=NULL) {
		obj->watch.pnt.x=pnt->x;
		obj->watch.pnt.y=pnt->y;
		obj->watch.pnt.z=pnt->z;
	}
	else {
		obj->watch.pnt.x=0;
		obj->watch.pnt.y=0;
		obj->watch.pnt.z=0;

		if (watch_obj_idx!=-1) {
			watch_obj=server.obj_list.objs[watch_obj_idx];
			if (watch_obj!=NULL) {
				obj->watch.pnt.x=watch_obj->pnt.x;
				obj->watch.pnt.y=watch_obj->pnt.y;
				obj->watch.pnt.z=watch_obj->pnt.z;
			}
		}
	}

	obj->watch.ang.x=angle_find(obj->pnt.y,obj->pnt.z,obj->watch.pnt.y,obj->watch.pnt.z);
	obj->watch.ang.y=angle_find(obj->pnt.x,obj->pnt.z,obj->watch.pnt.x,obj->watch.pnt.z);
	obj->watch.ang.z=angle_find(obj->pnt.x,obj->pnt.y,obj->watch.pnt.x,obj->watch.pnt.y);
}

/* =======================================================

      Object Watching
      
======================================================= */

bool object_watch_restrict(obj_type *obj,obj_type *watch_obj)
{
	float					ang_y,ang_dif;
	bool					cwise;
	d3pnt					spt,ept;
	ray_trace_contact_type	contact;
		
		// within angle
		
	ang_y=angle_find(obj->pnt.x,obj->pnt.z,watch_obj->pnt.x,watch_obj->pnt.z);
	ang_dif=angle_dif(ang_y,angle_add(obj->ang.y,obj->face.ang.y),&cwise);
	if (ang_dif>(obj->watch.watch_restrict.ang*0.5f)) return(FALSE);
		
		// is ray tracing on?
		
	if (!obj->watch.watch_restrict.ray_trace) return(TRUE);
	
		// can ray trace from eye to middle

	contact.obj.on=FALSE;
	contact.proj.on=FALSE;

	contact.obj.ignore_idx=-1;
	contact.proj.ignore_idx=-1;

	contact.origin=poly_ray_trace_origin_object;
	
	spt.x=obj->pnt.x;
	spt.y=obj->pnt.y+obj->size.eye_offset;
	spt.z=obj->pnt.z;
	
	ept.x=watch_obj->pnt.x;
	ept.y=watch_obj->pnt.y-(watch_obj->size.y>>1);
	ept.z=watch_obj->pnt.z;

	return(!ray_trace_map_by_point(&spt,&ept,&contact));
}

void object_watch(obj_type *obj)
{
	int						n,dist,x,z,y,kx,kz,ky;
	float					fx,fy,fz;
	bool					is_near;
	obj_type				*watch_obj;
	
		// watching on?
		
	if (!obj->watch.on) return;

		// object check properties
		
	x=obj->pnt.x;
	z=obj->pnt.z;
	y=obj->pnt.y;

		// check object
	
	dist=obj->watch.dist;

	for (n=0;n!=max_obj_list;n++) {
		watch_obj=server.obj_list.objs[n];
		if (watch_obj==NULL) continue;

		if ((watch_obj->hidden) || (watch_obj==obj)) continue;

			// check dist
			
		is_near=FALSE;
			
		kx=abs(x-watch_obj->pnt.x);
		if (kx<=dist) {
			kz=abs(z-watch_obj->pnt.z);
			if (kz<=dist) {
				ky=abs(y-watch_obj->pnt.y);
				if (ky<=dist) {
					fx=(float)(kx*kx);
					fy=(float)(ky*ky);
					fz=(float)(kz*kz);
					is_near=(((int)sqrtf(fx+fz+fy))<=dist);
				}
			}
		}
		
			// check angle and ray trace
			
		if ((is_near) && (obj->watch.watch_restrict.on)) {
			if (!object_watch_restrict(obj,watch_obj)) is_near=FALSE;
		}
		
			// has there been a change
		
		if (is_near) {
			if (obj->watch.obj_flags[n]==0x0) {
				obj->watch.obj_flags[n]=0x1;
				object_watch_setup(obj,watch_obj->idx,-1,NULL,NULL);
				scripts_post_event_console(obj->script_idx,-1,sd_event_watch,sd_event_watch_object_near,0);
			}
		}
		else {
			if (obj->watch.obj_flags[n]==0x1) {
				obj->watch.obj_flags[n]=0x0;
				object_watch_setup(obj,watch_obj->idx,-1,NULL,NULL);
				scripts_post_event_console(obj->script_idx,-1,sd_event_watch,sd_event_watch_object_far,0);
			}
		}
	}
}

/* =======================================================

      Object Watch Alerts
      
======================================================= */

void object_watch_damage_alert(d3pnt *pnt,obj_type *damage_obj)
{
	int				n;
	obj_type		*obj;
	
		// notify watching objects of damaged objects

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if ((obj->watch.on) && (obj->watch.dist!=0)) {

			if (distance_get(pnt->x,pnt->y,pnt->z,obj->pnt.x,obj->pnt.y,obj->pnt.z)<=obj->watch.dist) {
				object_watch_setup(obj,damage_obj->idx,-1,NULL,NULL);
				scripts_post_event_console(obj->script_idx,-1,sd_event_watch,sd_event_watch_object_damage,0);
			}
		}
	}
}

void object_watch_death_alert(obj_type *dead_obj)
{
	int				n;
	obj_type		*obj;
	
		// notify watching objects of death

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (obj->watch.on) {
			object_watch_setup(obj,dead_obj->idx,-1,NULL,NULL);
			scripts_post_event_console(obj->script_idx,-1,sd_event_watch,sd_event_watch_object_death,0);
		}
	}
}

void object_watch_base_alert(map_mesh_type *mesh,obj_type *enter_obj,bool entry)
{
	int				n;
	obj_type		*obj;
	
		// notify watching objects of base entries/exits

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (obj->watch.on) {
			object_watch_setup(obj,enter_obj->idx,mesh->msg.base_team,NULL,NULL);
			if (entry) {
				scripts_post_event_console(obj->script_idx,-1,sd_event_watch,sd_event_watch_object_enter_base,0);
			}
			else {
				scripts_post_event_console(obj->script_idx,-1,sd_event_watch,sd_event_watch_object_exit_base,0);
			}
		}
	}
}

/* =======================================================

      Object Watch Sound
      
======================================================= */

void object_watch_sound_alert(d3pnt *pnt,int sound_obj_idx,char *sound_name)
{
	int				n;
	obj_type		*obj;
	
		// notify watching objects of sounds

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if ((obj->watch.on) && (obj->watch.dist!=0)) {

			if (distance_get(pnt->x,pnt->y,pnt->z,obj->pnt.x,obj->pnt.y,obj->pnt.z)<=obj->watch.dist) {
				object_watch_setup(obj,sound_obj_idx,-1,sound_name,pnt);
				scripts_post_event_console(obj->script_idx,-1,sd_event_watch,sd_event_watch_object_sound,0);
			}
		}
	}
}

