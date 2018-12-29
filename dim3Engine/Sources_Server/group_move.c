/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Group Movements

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

extern map_type			map;
extern server_type		server;
extern js_type			js;

/* =======================================================

      Start Group Movement
      
======================================================= */

bool group_move_start(int group_idx,int movement_idx,int movement_move_idx,d3pnt *mov,d3ang *rot,int count,int user_id,int obj_idx,bool main_move)
{
	int					n,unit_cnt;
	float				f_count;
	char				err_str[256];
	group_type			*group;
	group_run_type		*run;
	group_unit_type		*unit_list;

	group=&map.group.groups[group_idx];

		// no negative move counts
		
	if (count<=0) {
		sprintf(err_str,"Attemping to move group '%s' with negative time count",group->name);
		console_add_error(err_str);
		return(FALSE);
	}
	
		// check for certain error states
		// this check is a little bit heavy but these are
		// common errors with users that are hard to track down

	unit_cnt=group->unit_count;

	if (unit_cnt==0) {
		sprintf(err_str,"Attemping to move group '%s' which has no meshes attached to it",group->name);
		console_add_error(err_str);
		return(FALSE);
	}
	
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {
		if (unit_list->type==group_type_mesh) {
			if (!map.mesh.meshes[unit_list->idx].flag.moveable) {
				sprintf(err_str,"Attemping to move a mesh in group '%s' which does not have the moveable flag set",group->name);
				console_add_error(err_str);
				return(FALSE);
			}
		}
		unit_list++;
	}
	
		// setup move over time

	run=&group->run;

	run->count=count;
	f_count=(float)run->count;

	run->f_mov_add.x=((float)mov->x)/f_count;
	run->f_mov_add.y=((float)mov->y)/f_count;
	run->f_mov_add.z=((float)mov->z)/f_count;
	
	run->f_mov_accum_add.x=0.0f;
	run->f_mov_accum_add.y=0.0f;
	run->f_mov_accum_add.z=0.0f;

	run->rot_add.x=rot->x/f_count;
	run->rot_add.y=rot->y/f_count;
	run->rot_add.z=rot->z/f_count;
		
	run->movement_idx=movement_idx;
	run->movement_move_idx=movement_move_idx;
	run->user_id=user_id;
	
	run->main_move=main_move;
	
	run->freeze=FALSE;
	run->on=TRUE;
	
		// attached object, otherwise it calls the course
		// script on events
		
	run->attach_obj_idx=obj_idx;

	return(TRUE);
}

void group_move_clear_all(void)
{
	int				n;
	group_type		*group;

	group=map.group.groups;

	for (n=0;n!=map.group.ngroup;n++) {

			// no movements yet

		group->run.on=FALSE;
		group->run.was_moved=FALSE;
		group->run.cuml_mov_add.x=group->run.cuml_mov_add.y=group->run.cuml_mov_add.z=0;
		group->run.cuml_rot_add.x=group->run.cuml_rot_add.y=group->run.cuml_rot_add.z=0;

			// get group center.  We need to
			// keep a separate center to stop
			// rotations from floating

		map_group_get_center(&map,n,&group->run.center_pnt);

		group++;
	}
}

/* =======================================================

      Freeze and UnFreeze
      
======================================================= */

void group_move_freeze(int group_idx,bool freeze)
{
	map.group.groups[group_idx].run.freeze=freeze;
}

bool group_move_frozen(int group_idx)
{
	return(map.group.groups[group_idx].run.freeze);
}

/* =======================================================

      Check if Object is Standing on Group
      
======================================================= */

bool group_move_object_stand(int group_idx,int stand_mesh_idx)
{
	int					n,unit_cnt;
	group_type			*group;
	group_unit_type		*unit_list;

	if (group_idx==-1) return(FALSE);

	group=&map.group.groups[group_idx];

	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		if (unit_list->type==group_type_mesh) {
			if (unit_list->idx==stand_mesh_idx) return(TRUE);
		}
		
		unit_list++;
	}

	return(FALSE);
}

/* =======================================================

      Move and Rotate Groups
      
======================================================= */

void group_move_and_rotate(group_type *group,d3pnt *move_pnt,d3ang *rot_ang,bool skip_obj_decal)
{
	int					n,unit_cnt;
	bool				move_objs,rot_objs;
	group_unit_type		*unit_list;
	map_mesh_type		*mesh;

		// add up the cumlative moves

	group->run.cuml_mov_add.x+=move_pnt->x;
	group->run.cuml_mov_add.y+=move_pnt->y;
	group->run.cuml_mov_add.z+=move_pnt->z;

	group->run.cuml_rot_add.x+=rot_ang->x;
	group->run.cuml_rot_add.y+=rot_ang->y;
	group->run.cuml_rot_add.z+=rot_ang->z;

		// can this group move or
		// rotate objects?

	if (skip_obj_decal) {
		move_objs=rot_objs=FALSE;
	}
	else {
		move_objs=(group->run.cuml_mov_add.x!=0) || (group->run.cuml_mov_add.z!=0);
		rot_objs=(group->run.cuml_rot_add.y!=0.0f);
	}
	
		// move the meshes

	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		switch (unit_list->type) {

			case group_type_mesh:

					// is mesh moveable?

				mesh=&map.mesh.meshes[unit_list->idx];
				if (!mesh->flag.moveable) break;

					// move/rotate mesh

				if (mesh->flag.rot_independent) {
					map_mesh_move_rotate_copy(&map,unit_list->idx,&mesh->box.mid,&group->run.cuml_mov_add,&group->run.cuml_rot_add);
				}
				else {
					map_mesh_move_rotate_copy(&map,unit_list->idx,&group->run.center_pnt,&group->run.cuml_mov_add,&group->run.cuml_rot_add);
				}

					// will need to update vertex/lights in vbo

				mesh->draw.moved=TRUE;

					// move objects and decals with mesh

				if (move_objs) {
					object_move_with_mesh(unit_list->idx,move_pnt);
					decal_move_with_mesh(unit_list->idx,move_pnt);
				}

					// rotate objects and decals with mesh
				
				if (rot_objs) {
					object_rotate_with_mesh(unit_list->idx,rot_ang->y);
					decal_rotate_with_mesh(unit_list->idx,rot_ang->y);
				}

				break;

			case group_type_liquid:
				map_liquid_move_copy(&map,unit_list->idx,&group->run.cuml_mov_add);
				break;

		}
		
		unit_list++;
	}

		// group center changes

	group->run.center_pnt.x+=move_pnt->x;
	group->run.center_pnt.y+=move_pnt->y;
	group->run.center_pnt.z+=move_pnt->z;
}

/* =======================================================

      Run Moves
      
======================================================= */

void group_moves_run(bool run_events)
{
	int				n,script_idx,user_id;
	d3pnt			mov_pnt;
	group_type		*group;
	group_run_type	*run;
	obj_type		*obj;

		// run all moves
		
	for (n=0;n!=map.group.ngroup;n++) {

		group=&map.group.groups[n];
		run=&group->run;
		if ((!run->on) || (run->freeze)) continue;
		
			// calculate the move
			// we have to use floats so we don't
			// lose numbers in the divide
			
		run->f_mov_accum_add.x+=run->f_mov_add.x;
		run->f_mov_accum_add.y+=run->f_mov_add.y;
		run->f_mov_accum_add.z+=run->f_mov_add.z;
		
		mov_pnt.x=(int)run->f_mov_accum_add.x;
		mov_pnt.y=(int)run->f_mov_accum_add.y;
		mov_pnt.z=(int)run->f_mov_accum_add.z;
		
		run->f_mov_accum_add.x-=(float)mov_pnt.x;
		run->f_mov_accum_add.y-=(float)mov_pnt.y;
		run->f_mov_accum_add.z-=(float)mov_pnt.z;
		
			// move it
		
		group_move_and_rotate(group,&mov_pnt,&run->rot_add,FALSE);

		run->was_moved=TRUE;
		
		run->count--;
	}
	
		// handle any finished moves
		// we do this separately as multiple group moves
		// hooked up to a move can interfere with each other
		
	for (n=0;n!=map.group.ngroup;n++) {

		run=&map.group.groups[n].run;
		if ((!run->on) || (run->freeze) || (run->count!=0)) continue;

		run->on=FALSE;

			// only post events and single map movements if
			// this move is the main move of a group of movements
			
		if (!run->main_move) continue;
		
			// get proper attachment
			
		script_idx=-1;
		
		if (run_events) {
			if (run->attach_obj_idx==-1) {
				script_idx=js.course_script_idx;
			}
			else {
				obj=server.obj_list.objs[run->attach_obj_idx];
				if (obj!=NULL) script_idx=obj->script_idx;
			}
		}

			// post the finished event
	
		user_id=run->user_id;
		if (script_idx!=-1) scripts_post_event_console(script_idx,-1,sd_event_move,sd_event_move_done,user_id);

			// signal back to the original map movement
	
		if (map_movement_next_move(run->movement_idx,run->movement_move_idx,run->attach_obj_idx)) {
			if (script_idx!=-1) scripts_post_event_console(script_idx,-1,sd_event_move,sd_event_move_loop,user_id);
		}
	}
}

void group_moves_rebuild(void)
{
	int				n;
	d3pnt			mov;
	d3ang			rot;

		// rebuild moves after save/load
		// groups have the cumlative moves, so we
		// we just pass a 0 move and 0 ang to
		// rebuild from those moves
	
	mov.x=mov.y=mov.z=0;
	rot.x=rot.y=rot.z=0.0f;
		
	for (n=0;n!=map.group.ngroup;n++) {
		group_move_and_rotate(&map.group.groups[n],&mov,&rot,TRUE);
	}
}

/* =======================================================

      Synch Group Movements with Host
      
======================================================= */

void group_moves_synch_with_client(int group_idx,network_reply_group_synch *synch)
{
	int				flags;
	group_type		*group;

		// setup the synch for the client

	synch->group_idx=htons((short)group_idx);

	group=&map.group.groups[group_idx];

	flags=0;
	if (group->run.on) flags|=net_group_synch_flag_on;
	if (group->run.freeze) flags|=net_group_synch_flag_freeze;
	if (group->run.main_move) flags|=net_group_synch_flag_main_move;

	synch->flags=htonl(flags);

	synch->count=htons((short)group->run.count);
	synch->user_id=htons((short)group->run.user_id);
	synch->movement_idx=htons((short)group->run.movement_idx);
	synch->movement_move_idx=htons((short)group->run.movement_move_idx);

	synch->fp_mov_add_x=htonf(group->run.f_mov_add.x);
	synch->fp_mov_add_y=htonf(group->run.f_mov_add.y);
	synch->fp_mov_add_z=htonf(group->run.f_mov_add.z);
	
	synch->fp_mov_accum_add_x=htonf(group->run.f_mov_accum_add.x);
	synch->fp_mov_accum_add_y=htonf(group->run.f_mov_accum_add.y);
	synch->fp_mov_accum_add_z=htonf(group->run.f_mov_accum_add.z);

	synch->fp_rot_add_x=htonf(group->run.rot_add.x);
	synch->fp_rot_add_y=htonf(group->run.rot_add.y);
	synch->fp_rot_add_z=htonf(group->run.rot_add.z);

	synch->cuml_mov_add_x=htonl(group->run.cuml_mov_add.x);
	synch->cuml_mov_add_y=htonl(group->run.cuml_mov_add.y);
	synch->cuml_mov_add_z=htonl(group->run.cuml_mov_add.z);

	synch->fp_cuml_rot_add_x=htonf(group->run.cuml_rot_add.x);
	synch->fp_cuml_rot_add_y=htonf(group->run.cuml_rot_add.y);
	synch->fp_cuml_rot_add_z=htonf(group->run.cuml_rot_add.z);
}

void group_moves_synch_with_host(network_reply_group_synch *synch)
{
	int				group_idx,flags;
	d3pnt			cuml_mov_add;
	d3ang			cuml_rot_add;
	group_type		*group;

		// get the group

	group_idx=(int)ntohs(synch->group_idx);
	group=&map.group.groups[group_idx];

		// fix the movement

	flags=ntohl(synch->flags);

	group->run.on=((flags&net_group_synch_flag_on)!=0);
	group->run.freeze=((flags&net_group_synch_flag_freeze)!=0);
	group->run.main_move=((flags&net_group_synch_flag_main_move)!=0);

	group->run.count=(int)ntohs(synch->count);
	group->run.user_id=(int)ntohs(synch->user_id);
	group->run.movement_idx=(int)ntohs(synch->movement_idx);
	group->run.movement_move_idx=(int)ntohs(synch->movement_move_idx);

	group->run.f_mov_add.x=ntohf(synch->fp_mov_add_x);
	group->run.f_mov_add.y=ntohf(synch->fp_mov_add_y);
	group->run.f_mov_add.z=ntohf(synch->fp_mov_add_z);
	
	group->run.f_mov_accum_add.x=ntohf(synch->fp_mov_accum_add_x);
	group->run.f_mov_accum_add.y=ntohf(synch->fp_mov_accum_add_y);
	group->run.f_mov_accum_add.z=ntohf(synch->fp_mov_accum_add_z);

	group->run.rot_add.x=ntohf(synch->fp_rot_add_x);
	group->run.rot_add.y=ntohf(synch->fp_rot_add_y);
	group->run.rot_add.z=ntohf(synch->fp_rot_add_z);

		// synch the commulative moves

		// if there's already been movement, get rid
		// of that before synching all commulative
		// movement

	cuml_mov_add.x=ntohl(synch->cuml_mov_add_x)-group->run.cuml_mov_add.x;
	cuml_mov_add.y=ntohl(synch->cuml_mov_add_y)-group->run.cuml_mov_add.y;
	cuml_mov_add.z=ntohl(synch->cuml_mov_add_z)-group->run.cuml_mov_add.z;

	cuml_rot_add.x=ntohf(synch->fp_cuml_rot_add_x)-group->run.cuml_rot_add.x;
	cuml_rot_add.y=ntohf(synch->fp_cuml_rot_add_y)-group->run.cuml_rot_add.y;
	cuml_rot_add.z=ntohf(synch->fp_cuml_rot_add_z)-group->run.cuml_rot_add.z;

	group_move_and_rotate(group,&cuml_mov_add,&cuml_rot_add,FALSE);
}

