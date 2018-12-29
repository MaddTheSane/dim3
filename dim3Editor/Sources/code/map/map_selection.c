/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Selection Routines

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern map_type				map;

int							nselect_item,nselect_duplicate_item;
select_item_type			select_items[select_max_item],select_duplicate_items[select_max_item];

/* =======================================================

      Selection Utilities
      
======================================================= */

void select_clear(void)
{
	nselect_item=0;
}

int select_find(int type,int main_idx,int sub_idx)
{
	int					n;
	select_item_type	*select_item;
	
	select_item=select_items;
	
	for (n=0;n!=nselect_item;n++) {
		if (main_idx==-1) {
			if (select_item->type==type) return(n);
		}
		else {
			if (select_item->sub_idx!=-1) {
				if ((select_item->type==type) && (select_item->main_idx==main_idx) && (select_item->sub_idx==sub_idx)) return(n);
			}
			else {
				if ((select_item->type==type) && (select_item->main_idx==main_idx)) return(n);
			}
		}
		select_item++;
	}
	
	return(-1);
}

bool select_check(int type,int main_idx,int sub_idx)
{
	return(select_find(type,main_idx,sub_idx)!=-1);
}

bool select_has_type(int type)
{
	return(select_find(type,-1,-1)!=-1);
}

void select_add(int type,int main_idx,int sub_idx)
{
	if (nselect_item==select_max_item) return;

		// already in list?

	if (select_check(type,main_idx,sub_idx)) return;
		
		// add to selection list
	
	select_items[nselect_item].type=type;
	select_items[nselect_item].main_idx=main_idx;
	select_items[nselect_item].sub_idx=sub_idx;
	nselect_item++;
}

int select_count(void)
{
	return(nselect_item);
}

void select_get(int sel_idx,int *type,int *main_idx,int *sub_idx)
{
	*type=select_items[sel_idx].type;
	*main_idx=select_items[sel_idx].main_idx;
	*sub_idx=select_items[sel_idx].sub_idx;
}

void select_switch(int sel_idx,int type,int main_idx,int sub_idx)
{
	select_items[sel_idx].type=type;
	select_items[sel_idx].main_idx=main_idx;
	select_items[sel_idx].sub_idx=sub_idx;
}

void select_flip(int type,int main_idx,int sub_idx)
{
	int				n,sel_idx;
	
	sel_idx=select_find(type,main_idx,sub_idx);
		
	if (sel_idx==-1) {
		select_add(type,main_idx,sub_idx);
		return;
	}
	
	for (n=sel_idx;n<nselect_item;n++) {
		memmove(&select_items[n],&select_items[n+1],sizeof(select_item_type));
	}
	
	nselect_item--;
}

void select_remove_type(int type)
{
	int					idx;
	select_item_type	*select_item;
	
	idx=0;
	
	while (idx<nselect_item) {
		select_item=&select_items[idx];
		if (select_item->type!=type) {
			idx++;
			continue;
		}
		
		if ((nselect_item>1) && (idx!=(nselect_item-1))) memmove(&select_items[idx],&select_items[idx+1],sizeof(select_item_type));
		
		nselect_item--;
		if (nselect_item==0) break;
	}
}

/* =======================================================

      Group, Movement, and Cinema Selects
      
======================================================= */

void select_add_group(int group_idx)
{
	int				n;

	if (group_idx==-1) return;

	for (n=0;n!=map.mesh.nmesh;n++) {
		if (map.mesh.meshes[n].group_idx==group_idx) select_add(item_map_mesh,n,0);
	}

	for (n=0;n!=map.liquid.nliquid;n++) {
		if (map.liquid.liquids[n].group_idx==group_idx) select_add(item_map_liquid,n,-1);
	}
}

void select_add_movement(int movement_idx)
{
	if (movement_idx==-1) return;

	select_add_group(map.movement.movements[movement_idx].group_idx);
	select_add_group(map.movement.movements[movement_idx].reverse_group_idx);
}

void select_add_cinema(int cinema_idx)
{
	int						n,idx;
	map_cinema_type			*cinema;
	map_cinema_action_type	*action;

	if (cinema_idx==-1) return;

	cinema=&map.cinema.cinemas[cinema_idx];

	action=cinema->actions;

	for (n=0;n!=cinema->naction;n++) {

		switch (action->actor_type) {

			case cinema_actor_player:
				idx=map_find_spot(&map,"Start",spot_type_player);
				if (idx!=-1) select_add(item_map_node,idx,-1);
				break;

			case cinema_actor_object:
				idx=map_find_spot(&map,action->actor_name,spot_type_object);
				if (idx!=-1) select_add(item_map_node,idx,-1);
				break;

			case cinema_actor_movement:
				idx=map_movement_find(&map,action->actor_name);
				if (idx!=-1) select_add_movement(idx);
				break;
		}

		action++;
	}
}

/* =======================================================

      Changing Selection For Deletions
      
======================================================= */

void select_delete_move_index(int type,int main_idx,int sub_idx)
{
	int					n;
	select_item_type	*select_item;
	
	select_item=select_items;
	
	for (n=0;n!=nselect_item;n++) {
		if ((select_item->type==type) && (select_item->main_idx==main_idx)) {
			if (select_item->sub_idx>=sub_idx) select_item->sub_idx--;
		}
		select_item++;
	}
}

/* =======================================================

      Sorting Selection
      
======================================================= */

void select_sort(void)
{
	int						n;
	bool					switch_item;
	select_item_type		temp_item;
	
	switch_item=TRUE;
	
	while (switch_item) {
		
		switch_item=FALSE;
		
		for (n=0;n<(nselect_item-1);n++) {
			if (select_items[n].main_idx<select_items[n+1].main_idx) {
				memmove(&temp_item,&select_items[n],sizeof(select_item_type));
				memmove(&select_items[n],&select_items[n+1],sizeof(select_item_type));
				memmove(&select_items[n+1],&temp_item,sizeof(select_item_type));
				switch_item=TRUE;
			}
		}
	}
}

/* =======================================================

      Duplicate Selection
      
======================================================= */

void select_duplicate_clear(void)
{
	nselect_duplicate_item=0;
}

void select_duplicate_add(int type,int main_idx,int sub_idx)
{
	if (nselect_duplicate_item==select_max_item) return;
	
	select_duplicate_items[nselect_duplicate_item].type=type;
	select_duplicate_items[nselect_duplicate_item].main_idx=main_idx;
	select_duplicate_items[nselect_duplicate_item].sub_idx=sub_idx;
	nselect_duplicate_item++;
}

void select_duplicate_backup(void)
{
	memmove(select_duplicate_items,select_items,(sizeof(select_item_type)*nselect_item));
	nselect_duplicate_item=nselect_item;
}

void select_duplicate_copy(void)
{
	memmove(select_items,select_duplicate_items,(sizeof(select_item_type)*nselect_duplicate_item));
	nselect_item=nselect_duplicate_item;
}

/* =======================================================

      Get Selection Extent
      
======================================================= */

void select_get_extent(d3pnt *min,d3pnt *max)
{
	int				n,sel_count,
					type,main_idx,sub_idx;
	d3pnt			t_min,t_max,size;
	
	min->x=min->z=min->y=map_max_size;
	max->x=max->z=max->y=-map_max_size;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&main_idx,&sub_idx);
		
		switch (type) {
		
			case item_map_mesh:
				map_mesh_calculate_extent(&map,main_idx,&t_min,&t_max);
				break;
				
			case item_map_liquid:
				t_min.x=map.liquid.liquids[main_idx].lft;
				t_max.x=map.liquid.liquids[main_idx].rgt;
				t_min.z=map.liquid.liquids[main_idx].top;
				t_max.z=map.liquid.liquids[main_idx].bot;
				t_min.y=t_max.y=map.liquid.liquids[main_idx].y;
				break;
				
			case item_map_node:
				t_min.x=map.nodes[main_idx].pnt.x-view_sprite_size;
				t_max.x=map.nodes[main_idx].pnt.x+view_sprite_size;
				t_min.y=map.nodes[main_idx].pnt.y-(view_sprite_size*2);
				t_max.y=map.nodes[main_idx].pnt.y;
				t_min.z=map.nodes[main_idx].pnt.z-view_sprite_size;
				t_max.z=map.nodes[main_idx].pnt.z+view_sprite_size;
				break;
				
			case item_map_spot:
				map_view_model_get_size(map.spots[main_idx].display_model,&size);
				t_min.x=map.spots[main_idx].pnt.x-(size.x>>1);
				t_max.x=map.spots[main_idx].pnt.x+(size.x>>1);
				t_min.y=map.spots[main_idx].pnt.y-size.y;
				t_max.y=map.spots[main_idx].pnt.y;
				t_min.z=map.spots[main_idx].pnt.z-(size.z>>1);
				t_max.z=map.spots[main_idx].pnt.z+(size.z>>1);
				break;
				
			case item_map_scenery:
				map_view_model_get_size(map.sceneries[main_idx].model_name,&size);
				t_min.x=map.sceneries[main_idx].pnt.x-(size.x>>1);
				t_max.x=map.sceneries[main_idx].pnt.x+(size.x>>1);
				t_min.y=map.sceneries[main_idx].pnt.y-size.y;
				t_max.y=map.sceneries[main_idx].pnt.y;
				t_min.z=map.sceneries[main_idx].pnt.z-(size.z>>1);
				t_max.z=map.sceneries[main_idx].pnt.z+(size.z>>1);
				break;
				
			case item_map_light:
				t_min.x=map.lights[main_idx].pnt.x-view_sprite_size;
				t_max.x=map.lights[main_idx].pnt.x+view_sprite_size;
				t_min.y=map.lights[main_idx].pnt.y-(view_sprite_size*2);
				t_max.y=map.lights[main_idx].pnt.y;
				t_min.z=map.lights[main_idx].pnt.z-view_sprite_size;
				t_max.z=map.lights[main_idx].pnt.z+view_sprite_size;
				break;
				
			case item_map_sound:
				t_min.x=map.sounds[main_idx].pnt.x-view_sprite_size;
				t_max.x=map.sounds[main_idx].pnt.x+view_sprite_size;
				t_min.y=map.sounds[main_idx].pnt.y-(view_sprite_size*2);
				t_max.y=map.sounds[main_idx].pnt.y;
				t_min.z=map.sounds[main_idx].pnt.z-view_sprite_size;
				t_max.z=map.sounds[main_idx].pnt.z+view_sprite_size;
				break;
				
			case item_map_particle:
				t_min.x=map.particles[main_idx].pnt.x-view_sprite_size;
				t_max.x=map.particles[main_idx].pnt.x+view_sprite_size;
				t_min.y=map.particles[main_idx].pnt.y-(view_sprite_size*2);
				t_max.y=map.particles[main_idx].pnt.y;
				t_min.z=map.particles[main_idx].pnt.z-view_sprite_size;
				t_max.z=map.particles[main_idx].pnt.z+view_sprite_size;
				break;
				
		}
			
		if (t_min.x<(min->x)) min->x=t_min.x;
		if (t_min.y<(min->y)) min->y=t_min.y;
		if (t_min.z<(min->z)) min->z=t_min.z;
		if (t_max.x>(max->x)) max->x=t_max.x;
		if (t_max.y>(max->y)) max->y=t_max.y;
		if (t_max.z>(max->z)) max->z=t_max.z;
	}
}

void select_get_center(d3pnt *mid)
{
	d3pnt			min,max;

	select_get_extent(&min,&max);

	mid->x=(min.x+max.x)>>1;
	mid->y=(min.y+max.y)>>1;
	mid->z=(min.z+max.z)>>1;
}

void select_get_angle(d3ang *ang)
{
	int			type,main_idx,sub_idx;

	ang->x=ang->y=ang->z=0.0f;

		// is this something with an
		// angle?

	if (select_count()!=1) return;

	select_get(0,&type,&main_idx,&sub_idx);

	switch (type) {
	
		case item_map_node:
			memmove(ang,&map.nodes[main_idx].ang,sizeof(d3ang));
			break;

		case item_map_spot:
			memmove(ang,&map.spots[main_idx].ang,sizeof(d3ang));
			break;

		case item_map_scenery:
			memmove(ang,&map.sceneries[main_idx].ang,sizeof(d3ang));
			break;
	
	}
}

