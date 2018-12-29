/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Movements

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

extern app_type			app;
extern map_type			map;
extern server_type		server;
extern camera_type		camera;

/* =======================================================

      Calculate Movement Centers
      
======================================================= */

void map_movements_get_center(movement_type *movement,d3pnt *pt)
{
	group_type	*group;

	group=&map.group.groups[movement->group_idx];

	pt->x=group->run.center_pnt.x;
	pt->y=group->run.center_pnt.y;
	pt->z=group->run.center_pnt.z;
	
	if (movement->reverse_group_idx!=-1) {
		group=&map.group.groups[movement->reverse_group_idx];
		pt->x=(pt->x+group->run.center_pnt.x)>>1;
		pt->y=(pt->y+group->run.center_pnt.y)>>1;
		pt->z=(pt->z+group->run.center_pnt.z)>>1;
	}
}

/* =======================================================

      Turn Movements On/Off
      
======================================================= */

void map_movements_start(int movement_idx,int move_idx,int obj_idx)
{
	int						msec,buffer_idx;
	d3pnt					rev_mov,pt;
	d3ang					rev_rot;
	movement_type			*movement;
	movement_move_type		*move;

	movement=&map.movement.movements[movement_idx];
	if (movement->nmove==0) return;
	
		// started
		
	movement->run.started=TRUE;
	
	move=&movement->moves[move_idx];
	
		// get reverse movement

	rev_mov.x=-move->mov.x;
	rev_mov.y=-move->mov.y;
	rev_mov.z=-move->mov.z;
	
	rev_rot.x=-move->rot.x;
	rev_rot.y=-move->rot.y;
	rev_rot.z=-move->rot.z;
	
		// start move over time
	
	msec=move->msec/10;
	
	if (!movement->run.reverse) {
		if (movement->group_idx!=-1) group_move_start(movement->group_idx,movement_idx,move_idx,&move->mov,&move->rot,msec,move->user_id,obj_idx,TRUE);
		if (movement->reverse_group_idx!=-1) group_move_start(movement->reverse_group_idx,movement_idx,move_idx,&rev_mov,&rev_rot,msec,move->user_id,obj_idx,FALSE);
	}
	else {
		if (movement->group_idx!=-1) group_move_start(movement->group_idx,movement_idx,move_idx,&rev_mov,&rev_rot,msec,move->user_id,obj_idx,TRUE);
		if (movement->reverse_group_idx!=-1) group_move_start(movement->reverse_group_idx,movement_idx,move_idx,&move->mov,&move->rot,msec,move->user_id,obj_idx,FALSE);
	}
	
		// play any sound
		
	if (move->sound_name[0]==0x0) return;
	
	if (!app.dedicated_host) {
		buffer_idx=al_find_buffer(move->sound_name);
		if (buffer_idx==-1) return;

		map_movements_get_center(movement,&pt);
		
		al_play_source(buffer_idx,&pt,move->sound_pitch,FALSE,FALSE,FALSE,FALSE);
	}
	
		// sound watches

	object_watch_sound_alert(&pt,-1,move->sound_name);
}

/* =======================================================

      Initialize Movements
      
======================================================= */

void map_movements_initialize(void)
{
	int				n,nmovement;
	movement_type	*movement;
	
	nmovement=map.movement.nmovement;
	movement=map.movement.movements;
	
	for (n=0;n!=nmovement;n++) {
	
			// start-up flags
			
		movement->run.started=FALSE;
		movement->run.opened=FALSE;
		
			// get center point
		
		map_movements_get_center(movement,&movement->run.auto_open_pnt);
			
			// run auto-starts
		
		if (movement->auto_start) {
			movement->run.reverse=FALSE;
			map_movements_start(n,0,-1);
		}
		
		movement++;
	}
}

/* =======================================================

      Script Movements
      
======================================================= */

void map_movements_script_start(int obj_idx,int movement_idx,bool reverse)
{
	movement_type	*movement;
	
	movement=&map.movement.movements[movement_idx];

	if (movement->run.started) return;
	
	if (!reverse) {
		movement->run.reverse=FALSE;
		map_movements_start(movement_idx,0,obj_idx);
	}
	else {
		movement->run.reverse=TRUE;
		map_movements_start(movement_idx,(movement->nmove-1),obj_idx);
	}
}

void map_movements_script_freeze(int movement_idx)
{
	movement_type	*movement;
	
	movement=&map.movement.movements[movement_idx];
	
	group_move_freeze(movement->group_idx,TRUE);
	if (movement->reverse_group_idx!=-1) group_move_freeze(movement->reverse_group_idx,TRUE);
}

void map_movements_script_thaw(int movement_idx)
{
	movement_type	*movement;
	
	movement=&map.movement.movements[movement_idx];
	
	group_move_freeze(movement->group_idx,FALSE);
	if (movement->reverse_group_idx!=-1) group_move_freeze(movement->reverse_group_idx,FALSE);
}

void map_movements_script_start_or_thaw(int obj_idx,int movement_idx)
{
	movement_type	*movement;
	
	movement=&map.movement.movements[movement_idx];

		// is movement started?
		
	if (movement->run.started) {
		if (group_move_frozen(movement->group_idx)) {
			group_move_freeze(movement->group_idx,FALSE);		// if frozen, then thaw
		}
	}
	else {
		movement->run.reverse=FALSE;
		map_movements_start(movement_idx,0,obj_idx);
	}
}

bool map_movements_script_is_looping(int movement_idx)
{	
	return(map.movement.movements[movement_idx].loop);
}

/* =======================================================

      Cinema Movements
      
======================================================= */

bool map_movements_cinema_start(int movement_idx,bool reverse,char *err_str)
{
	movement_type	*movement;
	
	movement=&map.movement.movements[movement_idx];

	if (movement->run.started) {
		sprintf(err_str,"Can't start another movement during a movement: %s",movement->name);
		return(FALSE);
	}
	
	if (!reverse) {
		movement->run.reverse=FALSE;
		map_movements_start(movement_idx,0,-1);
	}
	else {
		movement->run.reverse=TRUE;
		map_movements_start(movement_idx,(movement->nmove-1),-1);
	}
	
	return(TRUE);
}

/* =======================================================

      Run Movements
      
======================================================= */

bool map_movement_next_move(int movement_idx,int move_idx,int obj_idx)
{
	movement_type	*movement;
	
	movement=&map.movement.movements[movement_idx];
	
		// get next movement
		
	if (!movement->run.reverse) {
		
		move_idx++;
		
		if (move_idx>=movement->nmove) {
		
			if ((movement->auto_open) && (movement->auto_open_never_close)) {
				movement->auto_open=FALSE;
				movement->run.started=FALSE;
				return(TRUE);				// if auto-open and never close, then stop movement and turn off auto-open
			}
				
			if (!movement->loop) {
				movement->run.started=FALSE;
				return(TRUE);				// if looping is off, then stop
			}
			
			move_idx=0;
		}
	}
	else {
	
		move_idx--;
		
		if (move_idx<0) {
		
			if (!movement->loop) {
				movement->run.started=FALSE;
				return(TRUE);				// if looping is off, then stop
			}
			
			move_idx=movement->nmove-1;
		}
	}
	
		// start next movement
		
	map_movements_start(movement_idx,move_idx,obj_idx);
	
	return(FALSE);
}

/* =======================================================

      Auto-Open Movements
      
======================================================= */

void map_movements_auto_open(void)
{
	int				n,i,nmovement;
	bool			obj_in_range;
	movement_type	*movement;
	obj_type		*obj;

		// check movements
		
	nmovement=map.movement.nmovement;
	movement=map.movement.movements;
	
	for (n=0;n!=nmovement;n++) {
	
			// can't check started or non-auto-open movements
			
		if ((movement->run.started) || ((!movement->auto_open) && (!movement->auto_open_stand))) {
			movement++;
			continue;
		}

			// find any objects close to auto_open
			
		obj_in_range=FALSE;

		if (movement->auto_open) {

			for (i=0;i!=max_obj_list;i++) {
				obj=server.obj_list.objs[i];
				if (obj==NULL) continue;

				if (((obj->type==object_type_player) || (obj->type==object_type_remote_player) || (obj->open_doors)) && (obj->status.health.value!=0)) {
					if (distance_check(obj->pnt.x,(obj->pnt.y-(obj->size.y>>1)),obj->pnt.z,movement->run.auto_open_pnt.x,movement->run.auto_open_pnt.y,movement->run.auto_open_pnt.z,movement->auto_open_distance)) {
						obj_in_range=TRUE;
						break;
					}
				}
			}

				// check for camera node walks

			if ((map.camera.camera_mode==cv_static) && (camera.auto_walk.on) && (camera.auto_walk.open_doors)) {
				obj_in_range=distance_check(camera.cur_pos.pnt.x,camera.cur_pos.pnt.y,camera.cur_pos.pnt.z,movement->run.auto_open_pnt.x,movement->run.auto_open_pnt.y,movement->run.auto_open_pnt.z,movement->auto_open_distance);
			}

		}

			// check for standing on

		if (movement->auto_open_stand) {

			for (i=0;i!=max_obj_list;i++) {
				obj=server.obj_list.objs[i];
				if (obj==NULL) continue;

				if (((obj->type==object_type_player) || (obj->type==object_type_remote_player) || (obj->open_doors)) && (obj->status.health.value!=0)) {

					if (obj->contact.stand_poly.mesh_idx!=-1) {

						if (group_move_object_stand(movement->group_idx,obj->contact.stand_poly.mesh_idx)) {
							obj_in_range=TRUE;
							break;
						}
						if (group_move_object_stand(movement->reverse_group_idx,obj->contact.stand_poly.mesh_idx)) {
							obj_in_range=TRUE;
							break;
						}
					}
				}
			}

		}

			// time to open or close?
			
		if (!movement->run.opened) {
			if (obj_in_range) {
				movement->run.reverse=FALSE;
				movement->run.opened=TRUE;
				map_movements_start(n,0,-1);
			}
		}
		else {
			if (!obj_in_range) {
				movement->run.reverse=TRUE;
				movement->run.opened=FALSE;
				map_movements_start(n,(movement->nmove-1),-1);
			}
		}
		
		movement++;
	}
}
