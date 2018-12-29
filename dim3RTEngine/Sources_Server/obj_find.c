/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Finding

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
extern server_type			server;
extern network_setup_type	net_setup;
extern js_type				js;

/* =======================================================

      Find Objects
      
======================================================= */

obj_type* object_get_attach(JSObjectRef j_obj)
{
	int					script_idx;
	script_type			*script;

	script_idx=(int)JSObjectGetPrivate(j_obj);
	script=js.script_list.scripts[script_idx];
	
	return(server.obj_list.objs[script->attach.obj_idx]);
}

obj_type* object_find_remote_net_uid(int net_uid)
{
	int				n;
	obj_type		*obj;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (obj->remote.net_uid==net_uid) return(obj);
	}
	
	return(NULL);
}

/* =======================================================

      Find Objects by Name and Tag
      
======================================================= */

obj_type* object_find_name(char *name)
{
	int			n;
	obj_type	*obj;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (strcasecmp(obj->name,name)==0) return(obj);
	}
	
	return(NULL);
}

/* =======================================================

      Find Objects by Position
      
======================================================= */

obj_type* object_find_nearest(d3pnt *pnt,char *name,int type,int team_idx,float ang,float ang_sweep,int min_dist,int max_dist,bool player,int skip_obj_idx)
{
	int				n,i,d,dist;
	float			fang;
	obj_type		*obj;

	i=-1;
    dist=max_dist;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
		
		if (obj->hidden) continue;
		if (!obj->find_on) continue;
		
			// player and remotes
			
		if (player) {
			if ((obj->type!=object_type_player) && (obj->type!=object_type_remote_player) && (obj->type!=object_type_bot_multiplayer)) continue;
		}

		if (obj->idx==skip_obj_idx) continue;
		
			// check name
			
		if (name!=NULL) {
			if (strstr(obj->name,name)==NULL) continue;
		}
		
			// check type
			
		if (type!=-1) {
			if (obj->type!=type) continue;
		}
		
			// check team
			
		if (team_idx!=-1) {
			if (obj->team_idx!=team_idx) continue;
		}
		
			// check distance
			
		d=distance_get(pnt->x,pnt->y,pnt->z,obj->pnt.x,obj->pnt.y,obj->pnt.z);
		if (d<min_dist) continue;
		if (d>dist) continue;
		
			// check angle
			
		if (ang!=-1) {
			fang=angle_find(pnt->x,pnt->z,obj->pnt.x,obj->pnt.z);
			if (angle_dif(fang,ang,NULL)>ang_sweep) continue;
		}
		
		i=n;
		dist=d;
	}
	
	if (i==-1) return(NULL);
	
	return(server.obj_list.objs[i]);
}

int object_find_nearest_list(d3pnt *pnt,int min_dist,int max_dist,int *ids)
{
	int				n,cnt,dist;
	obj_type		*obj;

	cnt=0;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
		
		if (obj->hidden) continue;
		if (!obj->find_on) continue;
				
			// check distance
			
		dist=distance_get(pnt->x,pnt->y,pnt->z,obj->pnt.x,obj->pnt.y,obj->pnt.z);
		if (dist<min_dist) continue;
		if (dist>max_dist) continue;
		
		ids[cnt++]=n;
	}
	
	return(cnt);
}

/* =======================================================

      Teams
      
======================================================= */

int object_count_team(int team_idx,int ignore_obj_idx)
{
	int				n,count;
	obj_type		*obj;

	count=0;

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
		
		if ((obj->idx!=ignore_obj_idx) && (obj->team_idx==team_idx)) count++;
	}

	return(count);
}

void object_set_even_team(obj_type *obj)
{
	if (object_count_team(net_team_red,obj->idx)<object_count_team(net_team_blue,obj->idx)) {
		obj->team_idx=net_team_red;
	}
	else {
		obj->team_idx=net_team_blue;
	}
}

/* =======================================================

      Find Click Object
      
======================================================= */

int object_find_idx_click_object(obj_type *obj)
{
	int				n,idx,d,dist;
	float			fang;
	obj_type		*click_obj;
	
	idx=-1;
	dist=0;

	for (n=0;n!=max_obj_list;n++) {
		click_obj=server.obj_list.objs[n];
		if (click_obj==NULL) continue;
		
		if (click_obj->hidden) continue;
		if (!click_obj->click.on) continue;
		
			// check distance
			
		d=distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,click_obj->pnt.x,click_obj->pnt.y,click_obj->pnt.z);
		if (d>click_obj->click.distance) continue;
		if ((idx!=-1) && (d>dist)) continue;
		
			// check angle
			
		fang=angle_find(obj->pnt.x,obj->pnt.z,click_obj->pnt.x,click_obj->pnt.z);
		if (angle_dif(fang,obj->ang.y,NULL)>ci_object_click_angle) continue;
		
		idx=click_obj->idx;
		dist=d;
	}
	
	return(idx);
}

/* =======================================================

      Find Objects by Standing Under
      
======================================================= */

int object_find_idx_by_stood_on_object_idx(int stand_obj_idx)
{
	int				n;
	obj_type		*obj;
	
	for (n=0;n!=max_obj_list;n++) {
		if (stand_obj_idx==n) continue;
		
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if ((obj->touch.stand) && (obj->touch.obj_idx==stand_obj_idx)) return(obj->idx);
	}
	
	return(-1);
}

/* =======================================================

      Sight Path Tests
      
======================================================= */

bool object_sight_test_object(obj_type *obj,int test_obj_idx)
{
	int						n,eye_y,sz;
	float					ang;
	bool					in_sight;
	d3pnt					spt[9],ept[9],
							min,max,bounds_min,bounds_max;
	ray_trace_contact_type	contacts[9],base_contact;
	obj_type				*test_obj;
	
		// get test object
		
	if (test_obj_idx==-1) return(FALSE);
	
	test_obj=server.obj_list.objs[test_obj_idx];
	if (test_obj==NULL) return(FALSE);
	
		// check distance first
		
	if (distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,test_obj->pnt.x,test_obj->pnt.y,test_obj->pnt.z)>obj->sight.distance) return(FALSE);
	
		// get min/max for test object
		
	sz=test_obj->size.x>>1;
	min.x=test_obj->pnt.x-sz;
	max.x=test_obj->pnt.x+sz;
	
	sz=test_obj->size.z>>1;
	min.z=test_obj->pnt.z-sz;
	max.z=test_obj->pnt.z+sz;
	
	min.y=test_obj->pnt.y-test_obj->size.y;
	max.y=test_obj->pnt.y;
	
		// get eye y for object running
		// sight check
		
	eye_y=obj->pnt.y+obj->size.eye_offset;
	
		// check if within sight angles
		
		// anything that is within the x/y/z
		// automatically counts
		
	in_sight=FALSE;
	
	in_sight|=((obj->pnt.x>=min.x) && (obj->pnt.x<=max.x));
	in_sight|=((eye_y>=min.y) && (eye_y<=max.y));
	in_sight|=((obj->pnt.z>=min.z) && (obj->pnt.z<=max.z));
	
		// otherwise check angles
		
	if (!in_sight) {
		ang=angle_find(obj->pnt.x,obj->pnt.z,test_obj->pnt.x,test_obj->pnt.z);
		if (angle_dif(ang,obj->ang.y,NULL)>(obj->sight.side_angle*0.5f)) return(FALSE);
		
		sz=distance_2D_get(obj->pnt.x,obj->pnt.z,test_obj->pnt.x,test_obj->pnt.z);
		ang=angle_find(0,obj->pnt.y,sz,test_obj->pnt.y);
		if (angle_dif(ang,obj->ang.y,NULL)>(obj->sight.look_angle*0.5f)) return(FALSE);
	}

		// setup the rays
		
		// we aim at 8 corners and 1 middle
		
	spt[0].x=spt[1].x=spt[2].x=spt[3].x=spt[4].x=spt[5].x=spt[6].x=spt[7].x=spt[8].x=obj->pnt.x;
	spt[0].y=spt[1].y=spt[2].y=spt[3].y=spt[4].y=spt[5].y=spt[6].y=spt[7].y=spt[8].y=eye_y;
	spt[0].z=spt[1].z=spt[2].z=spt[3].z=spt[4].z=spt[5].z=spt[6].z=spt[7].z=spt[8].z=obj->pnt.z;
		
	ept[0].x=ept[3].x=ept[4].x=ept[7].x=bounds_min.x=min.x;
	ept[1].x=ept[2].x=ept[5].x=ept[6].x=bounds_max.x=max.x;
	ept[0].y=ept[1].y=ept[4].y=ept[5].y=bounds_min.y=min.y;
	ept[2].y=ept[3].y=ept[6].y=ept[7].y=bounds_max.y=max.y;
	ept[0].z=ept[1].z=ept[2].z=ept[3].z=bounds_min.z=min.z;
	ept[4].z=ept[5].z=ept[6].z=ept[7].z=bounds_max.z=max.z;

		// get the ray bounds

	if (obj->pnt.x<bounds_min.x) bounds_min.x=obj->pnt.x;
	if (obj->pnt.x>bounds_max.x) bounds_max.x=obj->pnt.x;
	if (eye_y<bounds_min.y) bounds_min.y=eye_y;
	if (eye_y>bounds_max.y) bounds_max.y=eye_y;
	if (obj->pnt.z<bounds_min.z) bounds_min.z=obj->pnt.z;
	if (obj->pnt.z>bounds_max.z) bounds_max.z=obj->pnt.z;
	
		// run the rays

	base_contact.obj.on=TRUE;
	base_contact.proj.on=FALSE;

	base_contact.obj.ignore_idx=obj->idx;
	base_contact.proj.ignore_idx=-1;

	base_contact.origin=poly_ray_trace_origin_object;
	
	ray_trace_map_by_point_array(9,&bounds_min,&bounds_max,spt,ept,&base_contact,contacts);
	
	for (n=0;n!=9;n++) {
		if (contacts[n].obj.idx==test_obj_idx) return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Nearest Object Searches
      
======================================================= */

int object_find_neareset_unheld_weapon_node(obj_type *obj,int from_node_idx)
{
	int				n,k,idx,d,dist;
	obj_type		*weap_obj;
	weapon_type		*weap;
	spot_type		*spot;

		// check all unheld weapons
		
	idx=-1;
	dist=-1;
	
	for (n=0;n!=max_weap_list;n++) {

			// get the weapon

		weap=obj->weap_list.weaps[n];
		if (weap==NULL) continue;
		if (!weap->hidden) continue;

			// find all spots with the name of the weapon

		for (k=0;k!=map.nspot;k++) {
			spot=&map.spots[k];
			if (strcmp(spot->name,weap->name)!=0) continue;
			if (spot->lookup.nearest_node_idx==-1) continue;
			if (spot->lookup.spawned_obj_idx==-1) continue;
			
				// is weapon hidden?
			
			weap_obj=server.obj_list.objs[spot->lookup.spawned_obj_idx];
			if (weap_obj->hidden) continue;

				// get distance

			d=map_node_to_node_distance(&map,from_node_idx,spot->lookup.nearest_node_idx);
			if (d==-1) continue;

			if ((dist==-1) || (d<dist)) {
				dist=d;
				idx=spot->lookup.nearest_node_idx;
			}
		}
	}
	
	return(idx);
}

int object_find_neareset_held_weapon_ammo_node(obj_type *obj,int from_node_idx,bool empty_only)
{
	int				n,k,idx,d,dist;
	char			name[name_str_len];
	obj_type		*ammo_obj;
	weapon_type		*weap;
	spot_type		*spot;

		// check all unheld weapons
		
	idx=-1;
	dist=-1;
	
	for (n=0;n!=max_weap_list;n++) {

			// get the weapon

		weap=obj->weap_list.weaps[n];
		if (weap==NULL) continue;
		if (!weap->hidden) continue;

			// empty only flag

		if (empty_only) {
			if (weap->ammo.count!=0) continue;
		}

			// find all spots with the name of the weapon+ammo

		strcpy(name,weap->name);
		string_safe_strcat(name," Ammo",name_str_len);

		for (k=0;k!=map.nspot;k++) {
			spot=&map.spots[k];
			if (strcmp(spot->name,name)!=0) continue;
			if (spot->lookup.nearest_node_idx==-1) continue;
			if (spot->lookup.spawned_obj_idx==-1) continue;
			
				// is ammo hidden?
			
			ammo_obj=server.obj_list.objs[spot->lookup.spawned_obj_idx];
			if (ammo_obj->hidden) continue;

				// get distance

			d=map_node_to_node_distance(&map,from_node_idx,spot->lookup.nearest_node_idx==-1);
			if (d==-1) continue;

			if ((dist==-1) || (d<dist)) {
				dist=d;
				idx=spot->lookup.nearest_node_idx;
			}
		}
	}
	
	return(idx);
}

int object_find_neareset_armor_node(obj_type *obj,int from_node_idx)
{
	int				n,idx,d,dist;
	spot_type		*spot;

		// check all unheld weapons
		
	idx=-1;
	dist=-1;

	for (n=0;n!=map.nspot;n++) {
		spot=&map.spots[n];
		if (strcmp(spot->name,"Armor")!=0) continue;
		if (spot->lookup.nearest_node_idx==-1) continue;

			// get distance

		d=map_node_to_node_distance(&map,from_node_idx,spot->lookup.nearest_node_idx==-1);
		if (d==-1) continue;

		if ((dist==-1) || (d<dist)) {
			dist=d;
			idx=spot->lookup.nearest_node_idx;
		}
	}
	
	return(idx);
}

