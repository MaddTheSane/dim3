/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Static Camera

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

extern server_type		server;
extern map_type			map;
extern camera_type		camera;
extern js_type			js;

/* =======================================================

      Setup Chase Camera Structure
      
======================================================= */

void camera_static_connect(void)
{
	camera.auto_walk.on=FALSE;
}

/* =======================================================

      Chase Camera Position
      
======================================================= */

void camera_static_calc_position(void)
{
	int				y;
	obj_type		*obj;
    
		// get position

	camera.cur_pos.pnt.x=camera.cur_pos.static_pnt.x+map.camera.pnt_offset.x;
	camera.cur_pos.pnt.y=camera.cur_pos.static_pnt.y+map.camera.pnt_offset.y;
	camera.cur_pos.pnt.z=camera.cur_pos.static_pnt.z+map.camera.pnt_offset.z;
	
		// if following an object, point camera at object

    if (map.camera.c_static.follow) {
		obj=server.obj_list.objs[camera.obj_idx];
		
			// if bumping, need to add in the bump
			
		y=obj->pnt.y;
		if (obj->bump.on) y+=obj->bump.smooth_offset;
		
			// point camera towards object
			
		camera.cur_pos.ang.x=-(180.0f-angle_find(camera.cur_pos.pnt.y,camera.cur_pos.pnt.z,y,obj->pnt.z));
        camera.cur_pos.ang.y=angle_find(camera.cur_pos.pnt.x,camera.cur_pos.pnt.z,obj->pnt.x,obj->pnt.z);
		camera.cur_pos.ang.z=0.0f;
		
		return;
	}

		// else just use camera offset
		// x needs to be positive/negative numbers

	camera.cur_pos.ang.x=camera.cur_pos.static_ang.x+map.camera.ang_offset.x;
	if (camera.cur_pos.ang.x>180.0f) camera.cur_pos.ang.x-=360.0f;

	camera.cur_pos.ang.y=angle_add(camera.cur_pos.static_ang.y,map.camera.ang_offset.y);
	camera.cur_pos.ang.z=camera.cur_pos.static_ang.z+map.camera.ang_offset.z;
}

/* =======================================================

      Setup Node To Node Walk Values
      
======================================================= */

void camera_walk_to_node_setup_speed_turn(void)
{
	int					msec,dist;
	node_type			*node;
	
		// remember start and end points
		// and camera angles
		
	node=&map.nodes[camera.auto_walk.node_seek_idx];

	memmove(&camera.auto_walk.start_pnt,&camera.cur_pos.static_pnt,sizeof(d3pnt));
	memmove(&camera.auto_walk.end_pnt,&node->pnt,sizeof(d3pnt));
	
	memmove(&camera.auto_walk.start_ang,&camera.cur_pos.static_ang,sizeof(d3ang));
	memmove(&camera.auto_walk.end_ang,&node->ang,sizeof(d3ang));
	
		// find the amount of ticks this
		// to node walk will take
		
	dist=distance_get(camera.auto_walk.start_pnt.x,camera.auto_walk.start_pnt.y,camera.auto_walk.start_pnt.z,camera.auto_walk.end_pnt.x,camera.auto_walk.end_pnt.y,camera.auto_walk.end_pnt.z);
	msec=(camera.auto_walk.msec*dist)/camera.auto_walk.total_dist;
	
	camera.auto_walk.start_tick=game_time_get();
	camera.auto_walk.end_tick=camera.auto_walk.start_tick+msec;
}

/* =======================================================

      Setup Camera Auto Moves
      
======================================================= */

bool camera_walk_to_node_by_index_setup(int from_idx,int to_idx,int msec,int event_id,bool open_doors,bool in_freeze,char *err_str)
{
	int			dist;
	float		speed;
	obj_type	*player_obj;
	node_type	*node;

		// only works in static camera

	if (map.camera.camera_mode!=cv_static) {
		strcpy(err_str,"Can only walk cameras in static mode");
		return(FALSE);
	}
	
		// is end node in start node path?
		
	if (map_find_next_node_in_path(&map,from_idx,to_idx)==-1) {
		sprintf(err_str,"End node '%s' is not in the same path as the start node '%s'",map.nodes[to_idx].name,map.nodes[from_idx].name);
		return(FALSE);
	}

		// get total distance
		// make sure to add in the distance it takes to
		// get to the first node

	node=&map.nodes[from_idx];

	dist=map_node_to_node_distance(&map,from_idx,to_idx);
	dist+=distance_get(camera.cur_pos.static_pnt.x,camera.cur_pos.static_pnt.y,camera.cur_pos.static_pnt.z,node->pnt.x,node->pnt.y,node->pnt.z);

	if (dist==0) {
		strcpy(err_str,"Camera walk covers no distance");
		return(FALSE);
	}

	speed=((float)dist)/(float)(msec/10);
	
		// get from node

	from_idx=map_find_next_node_in_path(&map,from_idx,to_idx);
	if (from_idx==-1) return(TRUE);
	
		// setup seeking
		
	camera.auto_walk.on=TRUE;
	camera.auto_walk.open_doors=open_doors;
	camera.auto_walk.in_freeze=in_freeze;
	camera.auto_walk.node_seek_idx=from_idx;
	camera.auto_walk.node_dest_idx=to_idx;
	camera.auto_walk.total_dist=dist;
	camera.auto_walk.msec=msec;
	camera.auto_walk.speed=speed;
	camera.auto_walk.event_id=event_id;
	
		// setup the speed and turning
		// for walking to next node

	camera_walk_to_node_setup_speed_turn();
	
		// player freeze
		
	if (in_freeze) {
		player_obj=server.obj_list.objs[server.player_obj_idx];
		object_input_freeze(player_obj,TRUE);
	}

	return(TRUE);
}

bool camera_walk_to_node_setup(char *start_node,char *end_node,int msec,int event_id,bool open_doors,bool in_freeze,char *err_str)
{
	int			from_idx,to_idx;

		// get the nodes
		
	from_idx=map_find_node(&map,start_node);
	if (from_idx==-1) {
		sprintf(err_str,"Named node does not exist: %s",start_node);
		return(FALSE);
	}
	
	to_idx=map_find_node(&map,end_node);
	if (to_idx==-1) {
		sprintf(err_str,"Named node does not exist: %s",end_node);
		return(FALSE);
	}

		// call the index version

	return(camera_walk_to_node_by_index_setup(from_idx,to_idx,msec,event_id,open_doors,in_freeze,err_str));
}

/* =======================================================

      Walk to Node Angle Utility
      
======================================================= */

float camera_walk_to_node_move_angle(float s_ang,float e_ang,int tick,int tot_tick)
{
	float			ang;

	if (s_ang<e_ang) {
		if ((e_ang-s_ang)<=180.0f) return(s_ang+(((e_ang-s_ang)*((float)tick))/((float)tot_tick)));

		s_ang+=360.0f;
		ang=s_ang+(((e_ang-s_ang)*((float)tick))/((float)tot_tick));
		if (ang>=360.0f) ang-=360.0f;
		return(ang);
	}

	if ((s_ang-e_ang)<=180.0f) return(s_ang+(((e_ang-s_ang)*((float)tick))/((float)tot_tick)));

	e_ang+=360.0f;
	ang=s_ang+(((e_ang-s_ang)*((float)tick))/((float)tot_tick));
	if (ang>=360.0f) ang-=360.0f;
	return(ang);
}

/* =======================================================

      Run Static Camera
      
======================================================= */

void camera_static_run(void)
{
	int			seek_idx,dest_idx,
				tick,tot_tick;
	node_type	*node;
	obj_type	*player_obj;

		// auto-walk on?

	if (!camera.auto_walk.on) return;
	
		// get nodes
		
	seek_idx=camera.auto_walk.node_seek_idx;
	dest_idx=camera.auto_walk.node_dest_idx;
		
	node=&map.nodes[seek_idx];
	
		// have we finished this node
		// walk?
		
	tick=game_time_get();
	
		// no, use the ratio to walk
		
	if (tick<camera.auto_walk.end_tick) {
	
		tick-=camera.auto_walk.start_tick;
		tot_tick=camera.auto_walk.end_tick-camera.auto_walk.start_tick;

			// set the current walk pnt
		
		camera.cur_pos.static_pnt.x=camera.auto_walk.start_pnt.x+(((camera.auto_walk.end_pnt.x-camera.auto_walk.start_pnt.x)*tick)/tot_tick);
		camera.cur_pos.static_pnt.y=camera.auto_walk.start_pnt.y+(((camera.auto_walk.end_pnt.y-camera.auto_walk.start_pnt.y)*tick)/tot_tick);
		camera.cur_pos.static_pnt.z=camera.auto_walk.start_pnt.z+(((camera.auto_walk.end_pnt.z-camera.auto_walk.start_pnt.z)*tick)/tot_tick);

			// get the look angle if not following

		if (!map.camera.c_static.follow) {
			camera.cur_pos.static_ang.x=camera_walk_to_node_move_angle(camera.auto_walk.start_ang.x,camera.auto_walk.end_ang.x,tick,tot_tick);
			camera.cur_pos.static_ang.y=camera_walk_to_node_move_angle(camera.auto_walk.start_ang.y,camera.auto_walk.end_ang.y,tick,tot_tick);
			camera.cur_pos.static_ang.z=camera_walk_to_node_move_angle(camera.auto_walk.start_ang.z,camera.auto_walk.end_ang.z,tick,tot_tick);
		}
		
		return;
	}
	
		// finish by setting everything
		// to end node to eliminate any slop
		
	memmove(&camera.cur_pos.static_pnt,&camera.auto_walk.end_pnt,sizeof(d3pnt));
	if (!map.camera.c_static.follow) memmove(&camera.cur_pos.static_ang,&node->ang,sizeof(d3ang));
	
		// move on to next node
		
	if (seek_idx!=dest_idx) {
		camera.auto_walk.node_seek_idx=map_find_next_node_in_path(&map,seek_idx,dest_idx);
		camera_walk_to_node_setup_speed_turn();

		scripts_post_event_console(js.course_script_idx,-1,sd_event_path,sd_event_path_node,node->event_id);
		return;
	}
	
		// at last node
		
	camera.auto_walk.on=FALSE;

		// player freeze
		
	if (camera.auto_walk.in_freeze) {
		player_obj=server.obj_list.objs[server.player_obj_idx];
		object_input_freeze(player_obj,FALSE);
	}
	
		// send event
	
	scripts_post_event_console(js.course_script_idx,-1,sd_event_path,sd_event_path_done,camera.auto_walk.event_id);
}
