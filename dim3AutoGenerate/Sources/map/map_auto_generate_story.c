/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Auto Generation Room Second Story

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

extern ag_map_state_type		ag_map_state;

/* =======================================================

      Second Story Steps
      
======================================================= */

void ag_map_add_room_second_story_steps(map_type *map,d3pnt *min,d3pnt *max,d3pnt *mov)
{
	int				n,step_high,step_count;

	step_high=(ag_size_room_high+ag_size_floor_high)/ag_size_stair_count;
	step_count=ag_size_stair_count-1;

	for (n=0;n!=step_count;n++) {

			// down one

		min->y+=step_high;
		if (min->y>=max->y) break;

			// step length

		if (mov->x!=0) {
			if (mov->x<0) {
				min->x=max->x+mov->x;
			}
			else {
				max->x=min->x+mov->x;
			}
		}

		if (mov->z!=0) {
			if (mov->z<0) {
				min->z=max->z+mov->z;
			}
			else {
				max->z=min->z+mov->z;
			}
		}

		ag_map_add_connector_room_step(map,min,max);

			// next step

		min->x+=mov->x;
		max->x+=mov->x;
		min->z+=mov->z;
		max->z+=mov->z;
	}
}

/* =======================================================

      Second Story Chunks
      
======================================================= */

void ag_map_add_room_second_story_chunk(map_type *map,int room_idx,int mesh_idx,int poly_idx,int start_vertex_idx,int end_vertex_idx)
{
	int						n,k,t,t2,ptsz,mx,my,mz,ty,by,
							story_mesh_idx,
							group_idx,movement_idx,move_idx;
	int						px[8],py[8],pz[8],
							kx[4],ky[4],ky2[4],kz[4],
							wx[4],wy[4],wz[4];
	float					gx[8],gy[8],mgx,mgy,
							k_gx[4],k_gy[4];
	bool					elevator_chunk;
	d3pnt					*pt;
	ag_room_type			*room;
	map_mesh_type			*mesh,*story_mesh;
	map_mesh_poly_type		*poly;
	movement_type			*movement;
	movement_move_type		*move;

	room=&ag_map_state.rooms[room_idx];

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

	ptsz=poly->ptsz;

		// new hole vertexes

	mx=my=mz=0;
	mgx=mgy=0.0f;

	for (n=0;n!=ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];

		px[n]=pt->x;
		py[n]=pt->y;
		pz[n]=pt->z;

		gx[n]=poly->main_uv.uvs[n].x;
		gy[n]=poly->main_uv.uvs[n].y;

		mx+=pt->x;
		my+=pt->y;
		mz+=pt->z;

		mgx+=gx[n];
		mgy+=gy[n];
	}

	mx/=ptsz;
	my/=ptsz;
	mz/=ptsz;

	mgx=mgx/(float)ptsz;
	mgy=mgy/(float)ptsz;

	for (n=0;n!=ptsz;n++) {
		px[n]=((px[n]-mx)/2)+mx;
		py[n]=((py[n]-my)/2)+my;
		pz[n]=((pz[n]-mz)/2)+mz;

		gx[n]=((gx[n]-mgx)/2.0f)+mgx;
		gy[n]=((gy[n]-mgy)/2.0f)+mgy;
	}

		// new mesh for each vertex

	for (n=start_vertex_idx;n<=end_vertex_idx;n++) {
		
		story_mesh_idx=map_mesh_add(map);
		if (story_mesh_idx==-1) return;

		story_mesh=&map->mesh.meshes[story_mesh_idx];

			// determine height and position
			// if regular chunk or elevator chunk

		elevator_chunk=(n==start_vertex_idx);

		if (!elevator_chunk) {
			ty=room->max.y-(ag_size_room_high+ag_size_floor_high);
			by=ty+ag_size_floor_high;
		}
		else {
			ty=room->max.y-ag_size_bump_high; ;
			by=ty+(ag_size_room_high+ag_size_floor_high);
		}

			// get internal points
		
		mesh=&map->mesh.meshes[mesh_idx];			// adding a mesh could change these pointers
		poly=&mesh->polys[poly_idx];

		k=n+1;
		if (k==ptsz) k=0;

		pt=&mesh->vertexes[poly->v[n]];
		kx[0]=pt->x;
		ky[0]=ty;
		ky2[0]=by;
		kz[0]=pt->z;

		pt=&mesh->vertexes[poly->v[k]];
		kx[1]=pt->x;
		ky[1]=ty;
		ky2[1]=by;
		kz[1]=pt->z;

		kx[2]=px[k];
		ky[2]=ty;
		ky2[2]=by;
		kz[2]=pz[k];

		kx[3]=px[n];
		ky[3]=ty;
		ky2[3]=by;
		kz[3]=pz[n];

			// walls

		for (t=0;t!=4;t++) {

			t2=t+1;
			if (t2==4) t2=0;

			wx[0]=kx[t];
			wy[0]=ky[t];
			wz[0]=kz[t];

			wx[1]=kx[t2];
			wy[1]=ky[t2];
			wz[1]=kz[t2];

			wx[2]=kx[t2];
			wy[2]=ky2[t2];
			wz[2]=kz[t2];

			wx[3]=kx[t];
			wy[3]=ky2[t];
			wz[3]=kz[t];

			k_gx[0]=0.0f;
			k_gy[0]=0.0f;

			k_gx[1]=1.0f;
			k_gy[1]=0.0f;

			k_gx[2]=1.0f;
			k_gy[2]=1.0f;

			k_gx[3]=0.0f;
			k_gy[3]=1.0f;

			map_mesh_add_poly(map,story_mesh_idx,4,wx,wy,wz,k_gx,k_gy,(elevator_chunk?ag_texture_lift:ag_texture_second_floor));
		}

			// floor and ceiling

		k_gx[0]=poly->main_uv.uvs[n].x;
		k_gy[0]=poly->main_uv.uvs[n].y;

		k_gx[1]=poly->main_uv.uvs[k].x;
		k_gy[1]=poly->main_uv.uvs[k].y;

		k_gx[2]=gx[k];
		k_gy[2]=gy[k];

		k_gx[3]=gx[n];
		k_gy[3]=gy[n];

		map_mesh_add_poly(map,story_mesh_idx,4,kx,ky,kz,k_gx,k_gy,(elevator_chunk?ag_texture_lift:ag_texture_second_floor));
		if (!elevator_chunk) map_mesh_add_poly(map,story_mesh_idx,4,kx,ky2,kz,k_gx,k_gy,(elevator_chunk?ag_texture_lift:ag_texture_second_floor));

			// recalc UVs and normals

		map_mesh_reset_uv(map,story_mesh_idx);
		map_recalc_normals_mesh(map,&map->mesh.meshes[story_mesh_idx],normal_mode_out,FALSE);

			// if elevator, setup the movement

		if (!elevator_chunk) continue;

			// add elevator group

		group_idx=map_group_add(map);
		sprintf(map->group.groups[group_idx].name,"Lift %d",room_idx);

		map->mesh.meshes[story_mesh_idx].group_idx=group_idx;
		map->mesh.meshes[story_mesh_idx].flag.moveable=TRUE;

			// add in movement

		movement_idx=map_movement_add(map);

		movement=&map->movement.movements[movement_idx];

		sprintf(movement->name,"Lift %d",room_idx);
		movement->group_idx=group_idx;
		movement->reverse_group_idx=-1;
		movement->auto_open_stand=TRUE;

		move_idx=map_movement_move_add(map,movement_idx);

		move=&movement->moves[move_idx];
		move->msec=3000;
		move->mov.x=0;
		move->mov.y=-(ag_size_room_high+(ag_size_floor_high-ag_size_bump_high));
		move->mov.z=0;

		strcpy(move->sound_name,"Lift");
	}
}

/* =======================================================

      Second Story Complete
      
======================================================= */

void ag_map_add_room_second_story_complete(map_type *map,int room_idx)
{
	int						n,k,mesh_idx;
	int						px[8],py[8],pz[8];
	float					gx[8],gy[8];
	ag_room_type			*room;

	room=&ag_map_state.rooms[room_idx];

		// tell later options that there
		// can be more decorations and lights

	room->second_story_complete=TRUE;

		// complete floor mesh

	mesh_idx=map_mesh_add(map);

		// add the walls

	for (n=0;n!=room->nvertex;n++) {

		k=n+1;
		if (k==room->nvertex) k=0;

		px[0]=px[3]=room->vertexes[n].x;
		px[1]=px[2]=room->vertexes[k].x;
		pz[0]=pz[3]=room->vertexes[n].z;
		pz[1]=pz[2]=room->vertexes[k].z;
		py[0]=py[1]=room->max.y-(ag_size_room_high+ag_size_floor_high);
		py[2]=py[3]=room->max.y-ag_size_room_high;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_second_floor);
	}

		// add the floor and ceiling

	for (n=0;n!=room->nvertex;n++) {
		px[n]=room->vertexes[n].x;
		py[n]=room->max.y-ag_size_room_high;
		pz[n]=room->vertexes[n].z;
		gx[n]=gy[n]=0.0f;
	}
		
	map_mesh_add_poly(map,mesh_idx,room->nvertex,px,py,pz,gx,gy,ag_texture_second_floor);

	for (n=0;n!=room->nvertex;n++) {
		py[n]-=ag_size_floor_high;
	}

	map_mesh_add_poly(map,mesh_idx,room->nvertex,px,py,pz,gx,gy,ag_texture_second_floor);

		// recalc UVs and normals

	map_mesh_reset_uv(map,mesh_idx);
	map_recalc_normals_mesh(map,&map->mesh.meshes[mesh_idx],normal_mode_out,FALSE);
}

/* =======================================================

      Add Room Second Story Pieces
      
======================================================= */

void ag_map_add_room_second_story(map_type *map)
{
	int				n,poly_idx,
					start_vertex_idx,end_vertex_idx;
	bool			story_surround;
	ag_room_type	*room;

		// build second stories

	for (n=0;n!=ag_map_state.nroom;n++) {

			// did this room have
			// a second story

		room=&ag_map_state.rooms[n];
		if (!room->second_story) continue;
		if (!room->require_top_floor) continue;

			// needs more than 3 vertexes

		if (room->nvertex<=3) continue;

			// if this room is connected to two
			// other rooms with second stories, then
			// it's a complete two story room or
			// falls through and does a complete
			// 360 ledge

		story_surround=ag_map_room_surrounded_by_second_stories(n);

		if (story_surround) {
			if (ag_random_bool()) {
				ag_map_add_room_second_story_complete(map,n);
				continue;
			}
		}

			// get the floor to base
			// second story on

		poly_idx=ag_map_find_floor_polygon(map,n);
		if (poly_idx==-1) continue;

			// determine which vertexes
			// to build around

		start_vertex_idx=0;

		if (story_surround) {
			end_vertex_idx=room->nvertex-1;
		}
		else {
			end_vertex_idx=ag_random_int(map->mesh.meshes[room->mesh_idx].polys[poly_idx].ptsz);
			if (start_vertex_idx==end_vertex_idx) end_vertex_idx++;
		}

		ag_map_add_room_second_story_chunk(map,n,room->mesh_idx,poly_idx,start_vertex_idx,end_vertex_idx);
	}
}

