/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Map Auto Generation Main Line

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

(c) 2000-2014 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3autogenerate.h"
#endif

ag_map_state_type				ag_map_state;

/* =======================================================

      Clear Map
      
======================================================= */

void ag_map_clear(map_type *map)
{
		// clear static memory

	map->nspot=0;
	map->nnode=0;
	map->nscenery=0;
	map->nlight=0;
	map->nsound=0;
	map->nparticle=0;

		// meshes and liquids

	while (map->mesh.nmesh!=0) {
		map_mesh_delete(map,0);
	}

	while (map->liquid.nliquid!=0) {
		map_liquid_delete(map,0);
	}

		// groups, movements, cinemas

	while (map->group.ngroup!=0) {
		map_group_delete(map,0);
	}

	while (map->movement.nmovement!=0) {
		map_movement_delete(map,0);
	}

	while (map->cinema.ncinema!=0) {
		map_cinema_delete(map,0);
	}
}

/* =======================================================

      Flatten Sides of Rooms
      
======================================================= */

bool ag_map_add_room_flatten_top(ag_room_type *room)
{
	int				n,top_idx,other_idx,
					prev_idx,next_idx;

		// find top z vertex

	top_idx=0;

	for (n=1;n!=room->nvertex;n++) {
		if (room->vertexes[n].z<room->vertexes[top_idx].z) top_idx=n;
	}

		// find closest top z
		// partner index

	prev_idx=top_idx-1;
	if (prev_idx==-1) prev_idx=room->nvertex-1;

	next_idx=top_idx+1;
	if (next_idx==room->nvertex) next_idx=0;

	other_idx=next_idx;
	if (room->vertexes[prev_idx].z<room->vertexes[next_idx].z) other_idx=prev_idx;

		// do not flatten if wall
		// gets too small

	if (abs(room->vertexes[other_idx].x-room->vertexes[top_idx].x)<ag_flat_connector_min_width) return(FALSE);

		// flatten top

	room->vertexes[other_idx].z=room->vertexes[top_idx].z;

	if (room->vertexes[other_idx].x<room->vertexes[top_idx].x) {
		room->flat.top.p1_idx=other_idx;
		room->flat.top.p2_idx=top_idx;
	}
	else {
		room->flat.top.p1_idx=top_idx;
		room->flat.top.p2_idx=other_idx;
	}

	room->flat.top.on=TRUE;

	return(TRUE);
}

bool ag_map_add_room_flatten_bottom(ag_room_type *room)
{
	int				n,bot_idx,other_idx,
					prev_idx,next_idx;

		// find bottom z vertex

	bot_idx=0;

	for (n=1;n!=room->nvertex;n++) {
		if (room->vertexes[n].z>room->vertexes[bot_idx].z) bot_idx=n;
	}

		// find closest bottom z
		// partner index

	prev_idx=bot_idx-1;
	if (prev_idx==-1) prev_idx=room->nvertex-1;

	next_idx=bot_idx+1;
	if (next_idx==room->nvertex) next_idx=0;

	other_idx=next_idx;
	if (room->vertexes[prev_idx].z>room->vertexes[next_idx].z) other_idx=prev_idx;

		// do not flatten if wall
		// gets too small

	if (abs(room->vertexes[other_idx].x-room->vertexes[bot_idx].x)<ag_flat_connector_min_width) return(FALSE);

		// flatten bottom

	room->vertexes[other_idx].z=room->vertexes[bot_idx].z;

	if (room->vertexes[other_idx].x<room->vertexes[bot_idx].x) {
		room->flat.bot.p1_idx=other_idx;
		room->flat.bot.p2_idx=bot_idx;
	}
	else {
		room->flat.bot.p1_idx=bot_idx;
		room->flat.bot.p2_idx=other_idx;
	}

	room->flat.bot.on=TRUE;

	return(TRUE);
}

bool ag_map_add_room_flatten_left(ag_room_type *room)
{
	int				n,lft_idx,other_idx,
					prev_idx,next_idx;

		// find left x vertex

	lft_idx=0;

	for (n=1;n!=room->nvertex;n++) {
		if (room->vertexes[n].x<room->vertexes[lft_idx].x) lft_idx=n;
	}

		// find closest left x
		// partner index

	prev_idx=lft_idx-1;
	if (prev_idx==-1) prev_idx=room->nvertex-1;

	next_idx=lft_idx+1;
	if (next_idx==room->nvertex) next_idx=0;

	other_idx=next_idx;
	if (room->vertexes[prev_idx].x<room->vertexes[next_idx].x) other_idx=prev_idx;

		// do not flatten if wall
		// gets too small

	if (abs(room->vertexes[other_idx].z-room->vertexes[lft_idx].z)<ag_flat_connector_min_width) return(FALSE);

		// flatten left

	room->vertexes[other_idx].x=room->vertexes[lft_idx].x;

	if (room->vertexes[other_idx].z<room->vertexes[lft_idx].z) {
		room->flat.lft.p1_idx=other_idx;
		room->flat.lft.p2_idx=lft_idx;
	}
	else {
		room->flat.lft.p1_idx=lft_idx;
		room->flat.lft.p2_idx=other_idx;
	}

	room->flat.lft.on=TRUE;

	return(TRUE);
}

bool ag_map_add_room_flatten_right(ag_room_type *room)
{
	int				n,rgt_idx,other_idx,
					prev_idx,next_idx;

		// find right x vertex

	rgt_idx=0;

	for (n=1;n!=room->nvertex;n++) {
		if (room->vertexes[n].x>room->vertexes[rgt_idx].x) rgt_idx=n;
	}

		// find closest right x
		// partner index

	prev_idx=rgt_idx-1;
	if (prev_idx==-1) prev_idx=room->nvertex-1;

	next_idx=rgt_idx+1;
	if (next_idx==room->nvertex) next_idx=0;

	other_idx=next_idx;
	if (room->vertexes[prev_idx].x>room->vertexes[next_idx].x) other_idx=prev_idx;

		// do not flatten if wall
		// gets too small

	if (abs(room->vertexes[other_idx].z-room->vertexes[rgt_idx].z)<ag_flat_connector_min_width) return(FALSE);

		// flatten right

	room->vertexes[other_idx].x=room->vertexes[rgt_idx].x;

	if (room->vertexes[other_idx].z<room->vertexes[rgt_idx].z) {
		room->flat.rgt.p1_idx=other_idx;
		room->flat.rgt.p2_idx=rgt_idx;
	}
	else {
		room->flat.rgt.p1_idx=rgt_idx;
		room->flat.rgt.p2_idx=other_idx;
	}

	room->flat.rgt.on=TRUE;

	return(TRUE);
}

void ag_map_add_room_fix_flat_flags(ag_room_type *room)
{
	if (room->flat.top.on) {
		if ((room->vertexes[room->flat.top.p1_idx].z!=room->vertexes[room->flat.top.p2_idx].z)) room->flat.top.on=FALSE;
		if (abs(room->vertexes[room->flat.top.p1_idx].z-room->vertexes[room->flat.top.p2_idx].z)<15000) room->flat.top.on=FALSE;
	}
	if (room->flat.bot.on) {
		if ((room->vertexes[room->flat.bot.p1_idx].z!=room->vertexes[room->flat.bot.p2_idx].z)) room->flat.bot.on=FALSE;
		if (abs(room->vertexes[room->flat.bot.p1_idx].z-room->vertexes[room->flat.bot.p2_idx].z)<15000) room->flat.top.on=FALSE;
	}
	if (room->flat.lft.on) {
		if ((room->vertexes[room->flat.lft.p1_idx].x!=room->vertexes[room->flat.lft.p2_idx].x)) room->flat.lft.on=FALSE;
		if (abs(room->vertexes[room->flat.lft.p1_idx].z-room->vertexes[room->flat.lft.p2_idx].z)<15000) room->flat.top.on=FALSE;
	}
	if (room->flat.rgt.on) {
		if ((room->vertexes[room->flat.rgt.p1_idx].x!=room->vertexes[room->flat.rgt.p2_idx].x)) room->flat.rgt.on=FALSE;
		if (abs(room->vertexes[room->flat.rgt.p1_idx].z-room->vertexes[room->flat.rgt.p2_idx].z)<15000) room->flat.top.on=FALSE;
	}
}

/* =======================================================

      Room Utilities
      
======================================================= */

void ag_map_add_room_get_point_from_angle(d3pnt *center_pnt,float x_radius,float z_radius,float ang,d3pnt *pnt)
{
	float			rad;

	rad=ang*ANG_to_RAD;
	pnt->x=center_pnt->x+(int)(x_radius*sinf(rad));
	pnt->z=center_pnt->z-(int)(z_radius*cosf(rad));
}

/* =======================================================

      Create a Room
      
======================================================= */

bool ag_map_add_room_random(ag_room_type *room,int wid_x,int wid_z,int connect_room_idx,int connect_side)
{
	int				n,
					nvertex,random_ang;
	float			last_ang,next_ang;
	d3pnt			pnt,p1,p2;

		// create random polygon

	nvertex=5+random_int(4);
	random_ang=(int)(360.0f/(float)nvertex);

	room->nvertex=0;

	last_ang=0.0f;

	pnt.x=(room->min.x+room->max.x)>>1;
	pnt.z=(room->min.z+room->max.z)>>1;

	for (n=0;n!=nvertex;n++) {

			// get next angle

		if (n==(nvertex-1)) {
			next_ang=0.0f;
		}
		else {
			next_ang=last_ang+(30.0f+(float)ag_random_int(random_ang));
			if (next_ang>360.0f) next_ang=0.0f;
		}

		ag_map_add_room_get_point_from_angle(&pnt,(float)wid_x,(float)wid_z,last_ang,&p1);
		ag_map_add_room_get_point_from_angle(&pnt,(float)wid_x,(float)wid_z,next_ang,&p2);

		last_ang=next_ang;

			// polygon points

		room->vertexes[room->nvertex].x=p1.x;
		room->vertexes[room->nvertex].y=room->max.y;
		room->vertexes[room->nvertex].z=p1.z;

		room->nvertex++;

			// exit early if we reconnect early

		if (last_ang==0.0f) break;
	}

		// if not connecting,
		// flatten top/bottom or
		// left/right randomly, and
		// then possible either of the other
		// so there's always at least one pass-through

	if (connect_room_idx==-1) {
		if (ag_random_bool()) {
			ag_map_add_room_flatten_top(room);
			ag_map_add_room_flatten_bottom(room);

			if (ag_random_bool()) ag_map_add_room_flatten_left(room);
			if (ag_random_bool()) ag_map_add_room_flatten_right(room);
		}
		else {
			ag_map_add_room_flatten_left(room);
			ag_map_add_room_flatten_right(room);

			if (ag_random_bool()) ag_map_add_room_flatten_top(room);
			if (ag_random_bool()) ag_map_add_room_flatten_bottom(room);
		}
	}

		// if connecting, make sure to flatten
		// the opposite side and another random side
		// skip if we can't flatten

	else {

		switch (connect_side) {
		
			case ag_connect_side_bottom:
			 if (!ag_map_add_room_flatten_top(room)) return(FALSE);
				ag_map_add_room_flatten_bottom(room);
				if (ag_random_bool()) ag_map_add_room_flatten_left(room);
				if (ag_random_bool()) ag_map_add_room_flatten_right(room);
				break;

			case ag_connect_side_top:
				if (!ag_map_add_room_flatten_bottom(room)) return(FALSE);
				ag_map_add_room_flatten_top(room);
				if (ag_random_bool()) ag_map_add_room_flatten_left(room);
				if (ag_random_bool()) ag_map_add_room_flatten_right(room);
				break;
			
			case ag_connect_side_right:
				if (!ag_map_add_room_flatten_left(room)) return(FALSE);
				ag_map_add_room_flatten_right(room);
				if (ag_random_bool()) ag_map_add_room_flatten_top(room);
				if (ag_random_bool()) ag_map_add_room_flatten_bottom(room);
				break;

			case ag_connect_side_left:
				if (!ag_map_add_room_flatten_right(room)) return(FALSE);
				ag_map_add_room_flatten_left(room);
				if (ag_random_bool()) ag_map_add_room_flatten_top(room);
				if (ag_random_bool()) ag_map_add_room_flatten_bottom(room);
				break;

		}
	}

	return(TRUE);
}

void ag_map_add_room_square(ag_room_type *room,int wid_x,int wid_z,int connect_side)
{
	int				k;

		// create square room based
		// on connection side

	room->nvertex=8;

		// top to bottom pass through

	if ((connect_side==ag_connect_side_left) || (connect_side==ag_connect_side_right)) {
		k=(room->max.z-room->min.z)/3;

		room->vertexes[0].x=room->min.x;
		room->vertexes[0].y=room->max.y;
		room->vertexes[0].z=room->min.z;

		room->vertexes[1].x=room->max.x;
		room->vertexes[1].y=room->max.y;
		room->vertexes[1].z=room->min.z;

		room->vertexes[2].x=room->max.x;
		room->vertexes[2].y=room->max.y;
		room->vertexes[2].z=room->min.z+k;

		room->vertexes[3].x=room->max.x;
		room->vertexes[3].y=room->max.y;
		room->vertexes[3].z=room->max.z-k;

		room->vertexes[4].x=room->max.x;
		room->vertexes[4].y=room->max.y;
		room->vertexes[4].z=room->max.z;

		room->vertexes[5].x=room->min.x;
		room->vertexes[5].y=room->max.y;
		room->vertexes[5].z=room->max.z;

		room->vertexes[6].x=room->min.x;
		room->vertexes[6].y=room->max.y;
		room->vertexes[6].z=room->max.z-k;

		room->vertexes[7].x=room->min.x;
		room->vertexes[7].y=room->max.y;
		room->vertexes[7].z=room->min.z+k;

		room->flat.top.p1_idx=0;
		room->flat.top.p2_idx=1;
		room->flat.top.on=TRUE;

		room->flat.bot.p1_idx=5;
		room->flat.bot.p2_idx=4;
		room->flat.bot.on=TRUE;

		room->flat.lft.p1_idx=7;
		room->flat.lft.p2_idx=6;
		room->flat.lft.on=TRUE;

		room->flat.rgt.p1_idx=2;
		room->flat.rgt.p2_idx=3;
		room->flat.rgt.on=TRUE;
		
		return;
	}
	
		// left to right pass through

	k=(room->max.x-room->min.x)/3;

	room->vertexes[0].x=room->min.x;
	room->vertexes[0].y=room->max.y;
	room->vertexes[0].z=room->min.z;

	room->vertexes[1].x=room->min.x+k;
	room->vertexes[1].y=room->max.y;
	room->vertexes[1].z=room->min.z;

	room->vertexes[2].x=room->max.x-k;
	room->vertexes[2].y=room->max.y;
	room->vertexes[2].z=room->min.z;

	room->vertexes[3].x=room->max.x;
	room->vertexes[3].y=room->max.y;
	room->vertexes[3].z=room->min.z;

	room->vertexes[4].x=room->max.x;
	room->vertexes[4].y=room->max.y;
	room->vertexes[4].z=room->max.z;

	room->vertexes[5].x=room->max.x-k;
	room->vertexes[5].y=room->max.y;
	room->vertexes[5].z=room->max.z;

	room->vertexes[6].x=room->min.x+k;
	room->vertexes[6].y=room->max.y;
	room->vertexes[6].z=room->max.z;

	room->vertexes[7].x=room->min.x;
	room->vertexes[7].y=room->max.y;
	room->vertexes[7].z=room->max.z;

	room->flat.top.p1_idx=1;
	room->flat.top.p2_idx=2;
	room->flat.top.on=TRUE;

	room->flat.bot.p1_idx=6;
	room->flat.bot.p2_idx=5;
	room->flat.bot.on=TRUE;

	room->flat.lft.p1_idx=0;
	room->flat.lft.p2_idx=7;
	room->flat.lft.on=TRUE;

	room->flat.rgt.p1_idx=3;
	room->flat.rgt.p2_idx=4;
	room->flat.rgt.on=TRUE;
}

void ag_map_add_room(map_type *map,bool first_room)
{
	int				n,k,wid_x,wid_z,room_idx,mesh_idx,
					connect_room_idx,org_connect_room_idx,
					connect_side,org_connect_side,connect_box_offset;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];
	bool			second_story,connect_stairs,hit;
	d3pnt			pnt,min,max;
	ag_room_type	*room,*connect_room;

		// random size
		// 10% of rooms are double sized

	wid_x=ag_size_room_width_start+ag_random_int(ag_size_room_width_extra);
	wid_z=ag_size_room_width_start+ag_random_int(ag_size_room_width_extra);

	if (ag_random_int(100)<10) {
		if (wid_x>wid_z) {
			wid_x*=2;
			wid_z=(wid_z*3)>>1;
		}
		else {
			wid_x=(wid_x*3)>>1;
			wid_z*=2;
		}
	}

		// determine if second story room
		// or connecting by stairs

	second_story=ag_random_bool();
	connect_stairs=(ag_random_int(100)<33);

		// if first room, in center of map

	if (first_room) {
		pnt.x=pnt.z=map_max_size>>1;
		
		min.x=pnt.x-wid_x;
		max.x=pnt.x+wid_x;

		max.y=ag_map_bottom_start_y;

		min.z=pnt.z-wid_z;
		max.z=pnt.z+wid_z;

		connect_room_idx=-1;
		connect_side=-1;
	}

		// otherwise connect it to a side
		// of an existing room, plus the size
		// of the connecting cooridor

		// we start at a random room and
		// random direction and search forward
		// to find the first empty spot

	else {

			// get random room and connect side

		org_connect_room_idx=connect_room_idx=ag_random_int(ag_map_state.nroom);
		org_connect_side=connect_side=ag_random_int(4);

			// run through sides and rooms to find
			// a good connection

		while (TRUE) {

				// get random connection size,
				// stairs need extra

			if (!connect_stairs) {
				connect_box_offset=2000+ag_random_int(5000);
			}
			else {
				connect_box_offset=ag_size_stair_length_start+ag_random_int(ag_size_stair_length_extra);
			}

				// check connection

			pnt.x=(ag_map_state.rooms[connect_room_idx].min.x+ag_map_state.rooms[connect_room_idx].max.x)>>1;
			pnt.z=(ag_map_state.rooms[connect_room_idx].min.z+ag_map_state.rooms[connect_room_idx].max.z)>>1;

			hit=FALSE;

			switch (connect_side) {
				case ag_connect_side_top:
					if (ag_map_state.rooms[connect_room_idx].flat.top.on) {
						min.x=pnt.x-wid_x;
						max.x=pnt.x+wid_x;
						min.z=(ag_map_state.rooms[connect_room_idx].min.z-(wid_z<<1))-connect_box_offset;
						max.z=ag_map_state.rooms[connect_room_idx].min.z-connect_box_offset;
						hit=TRUE;
					}
					break;
				case ag_connect_side_bottom:
					if (ag_map_state.rooms[connect_room_idx].flat.bot.on) {
						min.x=pnt.x-wid_x;
						max.x=pnt.x+wid_x;
						min.z=ag_map_state.rooms[connect_room_idx].max.z+connect_box_offset;
						max.z=(ag_map_state.rooms[connect_room_idx].max.z+(wid_z<<1))+connect_box_offset;
						hit=TRUE;
					}
					break;
				case ag_connect_side_left:
					if (ag_map_state.rooms[connect_room_idx].flat.lft.on) {
						min.x=(ag_map_state.rooms[connect_room_idx].min.x-(wid_x<<1))-connect_box_offset;
						max.x=ag_map_state.rooms[connect_room_idx].min.x-connect_box_offset;
						min.z=pnt.z-wid_z;
						max.z=pnt.z+wid_z;
						hit=TRUE;
					}
					break;
				case ag_connect_side_right:
					if (ag_map_state.rooms[connect_room_idx].flat.rgt.on) {
						min.x=ag_map_state.rooms[connect_room_idx].max.x+connect_box_offset;
						max.x=(ag_map_state.rooms[connect_room_idx].max.x+(wid_x<<1))+connect_box_offset;
						min.z=pnt.z-wid_z;
						max.z=pnt.z+wid_z;
						hit=TRUE;
					}
					break;
			}

				// legal spot?
				// don't need to check if first room

			if (hit) {

				for (n=0;n!=ag_map_state.nroom;n++) {
					if (max.x<=ag_map_state.rooms[n].min.x) continue;
					if (min.x>=ag_map_state.rooms[n].max.x) continue;
					if (max.z<=ag_map_state.rooms[n].min.z) continue;
					if (min.z>=ag_map_state.rooms[n].max.z) continue;

					hit=FALSE;
					break;
				}
			}

				// got a hit, we can use this spot
				// make sure we are on same Y level
				// if connected by stairs, go down on level

			if (hit) {
				max.y=ag_map_state.rooms[connect_room_idx].max.y;
				break;
			}

				// if no hit, then move on to the
				// next side or room

			if (!hit) {
				connect_side++;
				if (connect_side>=4) connect_side=0;

				if (connect_side==org_connect_side) {
					connect_room_idx++;
					if (connect_room_idx>=ag_map_state.nroom) connect_room_idx=0;

						// no where to place room, exit out

					if (connect_room_idx==org_connect_room_idx) return;
				}
			}
		}
	}

		// room structure
		// used to track main rooms

	mesh_idx=map_mesh_add(map);
	if (mesh_idx==-1) return;

	room_idx=ag_map_state.nroom;

	room=&ag_map_state.rooms[room_idx];
	room->mesh_idx=mesh_idx;

	ag_map_state.nroom++;

	memmove(&room->min,&min,sizeof(d3pnt));
	memmove(&room->max,&max,sizeof(d3pnt));

	room->flat.top.on=FALSE;
	room->flat.bot.on=FALSE;
	room->flat.lft.on=FALSE;
	room->flat.rgt.on=FALSE;

	room->connect_box.on=(connect_room_idx!=-1);
	room->connect_box.connect_type=connect_stairs?ag_connect_type_stairs:ag_connect_type_normal;
	room->connect_box.side=connect_side;
	room->connect_box.other_room_idx=connect_room_idx;
	room->connect_box.min.y=max.y-ag_size_room_high;
	room->connect_box.max.y=max.y;

	room->outside=FALSE;
	room->second_story=second_story;
	room->second_story_complete=FALSE;
	room->require_top_floor=FALSE;
	room->has_stairs=FALSE;
	room->has_windows=FALSE;
	room->has_wall_decorations=FALSE;

		// if this room is connected by steps,
		// then we need to move it down

	if (connect_stairs) room->max.y+=ag_size_room_high;

		// first room is automatically a
		// horizontal square room for maximum
		// connection possibilities

	if (first_room) {
		ag_map_add_room_square(room,wid_x,wid_z,ag_connect_side_left);
	}

		// other rooms are random pass-throughs
		// or a square room every once and a while

	else {
		
		if (ag_random_int(100)<10) {
			ag_map_add_room_square(room,wid_x,wid_z,connect_side);
		}
		else {
			if (!ag_map_add_room_random(room,wid_x,wid_z,connect_room_idx,connect_side)) {
				map_mesh_delete(map,mesh_idx);
				ag_map_state.nroom--;
				return;
			}
		}
	}

		// handle building the corridors
		// for connected rooms

	if (connect_room_idx!=-1) {

			// setup vertexes so they line up
			// when corridors are inserted

		connect_room=&ag_map_state.rooms[connect_room_idx];

		switch (connect_side) {

			case ag_connect_side_top:
				room->connect_box.min.x=connect_room->vertexes[connect_room->flat.top.p1_idx].x;
				room->connect_box.max.x=connect_room->vertexes[connect_room->flat.top.p2_idx].x;
				room->connect_box.min.z=connect_room->vertexes[connect_room->flat.top.p1_idx].z-connect_box_offset;
				room->connect_box.max.z=connect_room->vertexes[connect_room->flat.top.p1_idx].z;

				room->vertexes[room->flat.bot.p1_idx].x=connect_room->vertexes[connect_room->flat.top.p1_idx].x;
				room->vertexes[room->flat.bot.p1_idx].z=connect_room->vertexes[connect_room->flat.top.p1_idx].z-connect_box_offset;
				room->vertexes[room->flat.bot.p2_idx].x=connect_room->vertexes[connect_room->flat.top.p2_idx].x;
				room->vertexes[room->flat.bot.p2_idx].z=connect_room->vertexes[connect_room->flat.top.p2_idx].z-connect_box_offset;
				break;

			case ag_connect_side_bottom:
				room->connect_box.min.x=connect_room->vertexes[connect_room->flat.bot.p1_idx].x;
				room->connect_box.max.x=connect_room->vertexes[connect_room->flat.bot.p2_idx].x;
				room->connect_box.min.z=connect_room->vertexes[connect_room->flat.bot.p1_idx].z;
				room->connect_box.max.z=connect_room->vertexes[connect_room->flat.bot.p1_idx].z+connect_box_offset;

				room->vertexes[room->flat.top.p1_idx].x=connect_room->vertexes[connect_room->flat.bot.p1_idx].x;
				room->vertexes[room->flat.top.p1_idx].z=connect_room->vertexes[connect_room->flat.bot.p1_idx].z+connect_box_offset;
				room->vertexes[room->flat.top.p2_idx].x=connect_room->vertexes[connect_room->flat.bot.p2_idx].x;
				room->vertexes[room->flat.top.p2_idx].z=connect_room->vertexes[connect_room->flat.bot.p2_idx].z+connect_box_offset;
				break;

			case ag_connect_side_left:
				room->connect_box.min.x=connect_room->vertexes[connect_room->flat.lft.p1_idx].x-connect_box_offset;
				room->connect_box.max.x=connect_room->vertexes[connect_room->flat.lft.p1_idx].x;
				room->connect_box.min.z=connect_room->vertexes[connect_room->flat.lft.p1_idx].z;
				room->connect_box.max.z=connect_room->vertexes[connect_room->flat.lft.p2_idx].z;

				room->vertexes[room->flat.rgt.p1_idx].x=connect_room->vertexes[connect_room->flat.lft.p1_idx].x-connect_box_offset;
				room->vertexes[room->flat.rgt.p1_idx].z=connect_room->vertexes[connect_room->flat.lft.p1_idx].z;
				room->vertexes[room->flat.rgt.p2_idx].x=connect_room->vertexes[connect_room->flat.lft.p2_idx].x-connect_box_offset;
				room->vertexes[room->flat.rgt.p2_idx].z=connect_room->vertexes[connect_room->flat.lft.p2_idx].z;
				break;

			case ag_connect_side_right:
				room->connect_box.min.x=connect_room->vertexes[connect_room->flat.rgt.p1_idx].x;
				room->connect_box.max.x=connect_room->vertexes[connect_room->flat.rgt.p1_idx].x+connect_box_offset;
				room->connect_box.min.z=connect_room->vertexes[connect_room->flat.rgt.p1_idx].z;
				room->connect_box.max.z=connect_room->vertexes[connect_room->flat.rgt.p2_idx].z;

				room->vertexes[room->flat.lft.p1_idx].x=connect_room->vertexes[connect_room->flat.rgt.p1_idx].x+connect_box_offset;
				room->vertexes[room->flat.lft.p1_idx].z=connect_room->vertexes[connect_room->flat.rgt.p1_idx].z;
				room->vertexes[room->flat.lft.p2_idx].x=connect_room->vertexes[connect_room->flat.rgt.p2_idx].x+connect_box_offset;
				room->vertexes[room->flat.lft.p2_idx].z=connect_room->vertexes[connect_room->flat.rgt.p2_idx].z;
				break;

		}

			// check if two second story rooms,
			// these can require a top floor and
			// need the arch connection type

		if ((room->second_story) && (connect_room->second_story)) {
			room->require_top_floor=TRUE;
			connect_room->require_top_floor=TRUE;
			if (room->connect_box.connect_type==ag_connect_type_normal) room->connect_box.connect_type=ag_connect_type_arch;
		}

			// determine if we can connect with a door

		if (room->connect_box.connect_type==ag_connect_type_normal) {
			wid_x=room->connect_box.max.x-room->connect_box.min.x;
			wid_z=room->connect_box.max.z-room->connect_box.min.z;
			if ((wid_x<=ag_size_door_min_width) && (wid_z<=ag_size_door_min_width)) room->connect_box.connect_type=ag_connect_type_door;
		}

			// need to rebuild what's a flat connecting side as
			// deformations might break flat sides

		ag_map_add_room_fix_flat_flags(room);
	}

		// add the walls and short
		// ceiling/second-story walls

	for (n=0;n!=room->nvertex;n++) {

		k=n+1;
		if (k==room->nvertex) k=0;

		px[0]=px[3]=room->vertexes[n].x;
		px[1]=px[2]=room->vertexes[k].x;
		pz[0]=pz[3]=room->vertexes[n].z;
		pz[1]=pz[2]=room->vertexes[k].z;
		py[0]=py[1]=room->max.y-ag_size_room_high;
		py[2]=py[3]=room->max.y;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,(room->second_story?ag_texture_wall_2:ag_texture_wall));

		py[0]=py[1]=room->max.y-(ag_size_room_high+ag_size_floor_high);
		py[2]=py[3]=room->max.y-ag_size_room_high;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_wall);

		if (room->second_story) {
			py[0]=py[1]=room->max.y-(ag_size_room_high+ag_size_floor_high+ag_size_room_high);
			py[2]=py[3]=room->max.y-(ag_size_room_high+ag_size_floor_high);

			map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,(room->second_story?ag_texture_wall_2:ag_texture_wall));
		}
	}

		// add the floor

	for (n=0;n!=room->nvertex;n++) {
		px[n]=room->vertexes[n].x;
		py[n]=room->max.y;
		pz[n]=room->vertexes[n].z;
		gx[n]=gy[n]=0.0f;
	}
		
	map_mesh_add_poly(map,mesh_idx,room->nvertex,px,py,pz,gx,gy,(room->second_story?ag_texture_floor_2:ag_texture_floor));

		// add the ceiling

	for (n=0;n!=room->nvertex;n++) {
		py[n]=room->max.y-(ag_size_room_high+ag_size_floor_high);
		if (room->second_story) py[n]-=ag_size_room_high;
	}
// supergumba -- testing decorations
	map_mesh_add_poly(map,mesh_idx,room->nvertex,px,py,pz,gx,gy,(room->second_story?ag_texture_ceiling_2:ag_texture_ceiling));

		// reset the UV and normals

	map_mesh_reset_uv(map,mesh_idx);
	map_recalc_normals_mesh(map,&map->mesh.meshes[mesh_idx],normal_mode_in,FALSE);
}

/* =======================================================

      Generate Map
      
======================================================= */

bool ag_map_run(map_type *map,char *err_str)
{
	int				n,room_count;

	ag_map_state.nroom=0;
	ag_map_state.current_door_idx=0;

		// initialize auto generate structures

	ag_map_state.rooms=(ag_room_type*)malloc(ag_max_room*sizeof(ag_room_type));
	if (ag_map_state.rooms==NULL) return(FALSE);

		// clear the map data

	ag_map_clear(map);

		// add rooms
		// sometimes it fails because of randomness
		// so give it a chance to built up to room_count
		// rooms
		
	room_count=ag_room_start_count+ag_random_int(ag_room_extra_count);

	for (n=0;n!=(room_count*10);n++) {
		ag_map_add_room(map,(ag_map_state.nroom==0));
		if (ag_map_state.nroom==room_count) break;
	}

		// add square connector
		// rooms that happen when connected rooms
		// are squared off

	ag_map_add_connector_rooms(map);

		// add second story items to
		// rooms

	ag_map_add_room_second_story(map);

		// delete any polygons that share the
		// same space

	ag_map_delete_shared_polygons(map);

		// other items

	ag_map_windows_add(map);
	ag_map_decorations_add(map);
	ag_map_lights_add(map);

		// add spots and nodes

	ag_map_spots_add(map);

		// free auto generate structures

	free(ag_map_state.rooms);

	return(TRUE);
}

bool auto_generate_map(map_type *map,char *err_str)
{
		// run the auto generator
		// with a new unique seed

	ag_random_seed();
	return(ag_map_run(map,err_str));
}

bool auto_generate_map_previous(map_type *map,char *err_str)
{
	ag_random_previous_seed();
	return(ag_map_run(map,err_str));
}

bool auto_generate_map_next(map_type *map,char *err_str)
{
	ag_random_next_seed();
	return(ag_map_run(map,err_str));
}
