/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Auto Generation Room Connectors

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

      Door Room Connectors
      
======================================================= */

void ag_map_add_connector_door(map_type *map,char *name,d3pnt *min,d3pnt *max,char *name2,d3pnt *min2,d3pnt *max2,d3pnt *mov)
{
	int					mesh_idx,
						group_idx,reverse_group_idx,
						movement_idx,move_idx;
	int					px[8],py[8],pz[8];
	float				gx[8],gy[8];
	movement_type		*movement;
	movement_move_type	*move;

		// create first door

	mesh_idx=map_mesh_add(map);

	px[0]=px[3]=min->x;
	px[1]=px[2]=max->x;
	pz[0]=pz[3]=pz[1]=pz[2]=min->z;
	py[0]=py[1]=min->y;
	py[2]=py[3]=max->y;
	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);

	pz[0]=pz[3]=pz[1]=pz[2]=max->z;
	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);

	px[0]=px[3]=px[1]=px[2]=min->x;
	pz[0]=pz[3]=min->z;
	pz[1]=pz[2]=max->z;
	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);

	px[0]=px[3]=px[1]=px[2]=max->x;
	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);

	px[0]=px[3]=min->x;
	px[1]=px[2]=max->x;
	pz[0]=pz[1]=min->z;
	pz[2]=pz[3]=max->z;
	py[0]=py[1]=py[2]=py[3]=min->y;
	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);

	py[0]=py[1]=py[2]=py[3]=max->y;
	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);

	map_mesh_reset_uv(map,mesh_idx);
	map_recalc_normals_mesh(map,&map->mesh.meshes[mesh_idx],normal_mode_out,FALSE);

		// add door group

	group_idx=map_group_add(map);
	strcpy(map->group.groups[group_idx].name,name);

	map->mesh.meshes[mesh_idx].group_idx=group_idx;
	map->mesh.meshes[mesh_idx].flag.moveable=TRUE;

		// create optional second door

	reverse_group_idx=-1;

	if (name2!=NULL) {
		mesh_idx=map_mesh_add(map);

		px[0]=px[3]=min2->x;
		px[1]=px[2]=max2->x;
		pz[0]=pz[3]=pz[1]=pz[2]=min2->z;
		py[0]=py[1]=min2->y;
		py[2]=py[3]=max2->y;
		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);

		pz[0]=pz[3]=pz[1]=pz[2]=max2->z;
		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);

		px[0]=px[3]=px[1]=px[2]=min2->x;
		pz[0]=pz[3]=min2->z;
		pz[1]=pz[2]=max2->z;
		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);

		px[0]=px[3]=px[1]=px[2]=max2->x;
		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);

		px[0]=px[3]=min2->x;
		px[1]=px[2]=max2->x;
		pz[0]=pz[1]=min2->z;
		pz[2]=pz[3]=max2->z;
		py[0]=py[1]=py[2]=py[3]=min2->y;
		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);

		py[0]=py[1]=py[2]=py[3]=max2->y;
		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);

		map_mesh_reset_uv(map,mesh_idx);
		map_recalc_normals_mesh(map,&map->mesh.meshes[mesh_idx],normal_mode_out,FALSE);

			// add door group

		reverse_group_idx=map_group_add(map);
		strcpy(map->group.groups[reverse_group_idx].name,name2);

		map->mesh.meshes[mesh_idx].group_idx=reverse_group_idx;
		map->mesh.meshes[mesh_idx].flag.moveable=TRUE;
	}

		// add in door movement

	movement_idx=map_movement_add(map);

	movement=&map->movement.movements[movement_idx];

	strcpy(movement->name,name);
	movement->group_idx=group_idx;
	movement->reverse_group_idx=reverse_group_idx;
	movement->auto_open=TRUE;

	move_idx=map_movement_move_add(map,movement_idx);

	move=&movement->moves[move_idx];
	move->msec=2500;
	memmove(&move->mov,mov,sizeof(d3pnt));

	strcpy(move->sound_name,"Door");
}

void ag_map_add_connector_room_normal(map_type *map,int org_room_idx,bool has_door)
{
	int					mesh_idx,dx,dz,mx,mz;
	int					px[8],py[8],pz[8];
	float				gx[8],gy[8];
	char				door_name[32],door2_name[32];
	d3pnt				door_min,door_max,door_mov,
						door2_min,door2_max;
	ag_room_type		*room,*room2;

	room=&ag_map_state.rooms[org_room_idx];
	room2=&ag_map_state.rooms[room->connect_box.other_room_idx];

		// remove unused polygons

	ag_map_remove_polygons_in_box(map,room->mesh_idx,&room->connect_box.min,&room->connect_box.max);
	ag_map_remove_polygons_in_box(map,room2->mesh_idx,&room->connect_box.min,&room->connect_box.max);

		// get dimensions

	dx=room->connect_box.max.x-room->connect_box.min.x;
	dz=room->connect_box.max.z-room->connect_box.min.z;

		// add the mesh

	mesh_idx=map_mesh_add(map);

		// walls

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

	if (dx>dz) {
		px[0]=px[1]=px[2]=px[3]=room->connect_box.min.x;
		pz[0]=pz[3]=room->connect_box.min.z;
		pz[1]=pz[2]=room->connect_box.max.z;
		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.max.y;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,(has_door?ag_texture_connect:ag_texture_wall));

		px[0]=px[1]=px[2]=px[3]=room->connect_box.max.x;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,(has_door?ag_texture_connect:ag_texture_wall));
	}
	else {
		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.min.z;
		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.max.y;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,(has_door?ag_texture_connect:ag_texture_wall));

		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.max.z;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,(has_door?ag_texture_connect:ag_texture_wall));
	}

		// floor and ceiling

	px[0]=px[3]=room->connect_box.min.x;
	px[1]=px[2]=room->connect_box.max.x;
	pz[0]=pz[1]=room->connect_box.min.z;
	pz[2]=pz[3]=room->connect_box.max.z;
	py[0]=py[1]=py[2]=py[3]=room->connect_box.max.y;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,(has_door?ag_texture_connect:ag_texture_floor));

	py[0]=py[1]=py[2]=py[3]=room->connect_box.min.y;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,(has_door?ag_texture_connect:ag_texture_ceiling));

	map_mesh_reset_uv(map,mesh_idx);
	map_recalc_normals_mesh(map,&map->mesh.meshes[mesh_idx],normal_mode_in,FALSE);

		// add in a door?

	if (!has_door) return;

		// up door

	if (ag_random_bool()) {
		sprintf(door_name,"Door %d",ag_map_state.current_door_idx);

		if (dx>dz) {
			door_min.x=room->connect_box.min.x;
			door_max.x=room->connect_box.max.x;
			door_min.z=room->connect_box.min.z+ag_size_door_margin;
			door_max.z=room->connect_box.max.z-ag_size_door_margin;
		}
		else {
			door_min.x=room->connect_box.min.x+ag_size_door_margin;
			door_max.x=room->connect_box.max.x-ag_size_door_margin;
			door_min.z=room->connect_box.min.z;
			door_max.z=room->connect_box.max.z;
		}

		door_min.y=room->connect_box.min.y;
		door_max.y=room->connect_box.max.y;

		door_mov.x=0;
		door_mov.y=-((room->connect_box.max.y-room->connect_box.min.y)-ag_size_door_margin);
		door_mov.z=0;

		ag_map_add_connector_door(map,door_name,&door_min,&door_max,NULL,NULL,NULL,&door_mov);
	}

		// split door

	else {

		if (dx>dz) {
			mx=(room->connect_box.min.x+room->connect_box.max.x)>>1;

			door_min.x=room->connect_box.min.x;
			door_max.x=mx;
			door_min.z=room->connect_box.min.z+ag_size_door_margin;
			door_max.z=room->connect_box.max.z-ag_size_door_margin;
			
			door2_min.x=mx;
			door2_max.x=room->connect_box.max.x;
			door2_min.z=room->connect_box.min.z+ag_size_door_margin;
			door2_max.z=room->connect_box.max.z-ag_size_door_margin;
			
			door_mov.x=-(((room->connect_box.max.x-room->connect_box.min.x)>>1)-ag_size_door_margin);
			door_mov.y=0;
			door_mov.z=0;
		}
		else {
			mz=(room->connect_box.min.z+room->connect_box.max.z)>>1;

			door_min.x=room->connect_box.min.x+ag_size_door_margin;
			door_max.x=room->connect_box.max.x-ag_size_door_margin;
			door_min.z=room->connect_box.min.z;
			door_max.z=mz;

			door2_min.x=room->connect_box.min.x+ag_size_door_margin;
			door2_max.x=room->connect_box.max.x-ag_size_door_margin;
			door2_min.z=mz;
			door2_max.z=room->connect_box.max.z;

			door_mov.x=0;
			door_mov.y=0;
			door_mov.z=-(((room->connect_box.max.z-room->connect_box.min.z)>>1)-ag_size_door_margin);
		}

		door_min.y=room->connect_box.min.y;
		door_max.y=room->connect_box.max.y;
		door2_min.y=room->connect_box.min.y;
		door2_max.y=room->connect_box.max.y;

		sprintf(door_name,"Door %d",ag_map_state.current_door_idx);
		sprintf(door2_name,"Door %dR",ag_map_state.current_door_idx);

		ag_map_add_connector_door(map,door_name,&door_min,&door_max,door2_name,&door2_min,&door2_max,&door_mov);
	}

		// next door

	ag_map_state.current_door_idx++;
}

/* =======================================================

      Arch Room Connectors
      
======================================================= */

void ag_map_add_connector_room_arch_add(map_type *map,ag_room_type *room,int ty,int by,int by2)
{
	int					n,mesh_idx,dx,dz;
	int					px[8],py[8],pz[8],
						flip_poly_idx[4];
	float				gx[8],gy[8];

		// get pillar direction

	dx=room->connect_box.max.x-room->connect_box.min.x;
	dz=room->connect_box.max.z-room->connect_box.min.z;

		// add mesh

	mesh_idx=map_mesh_add(map);

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

	if (dx>dz) {

			// outside walls

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.min.x+ag_size_pillar_wid;
		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.min.z;
		py[0]=ty;
		py[1]=ty+ag_size_pillar_wid;
		py[2]=by;
		py[3]=by2;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.max.z;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[3]=room->connect_box.max.x-ag_size_pillar_wid;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.min.z;
		py[0]=ty+ag_size_pillar_wid;
		py[1]=ty;
		py[2]=by2;
		py[3]=by;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.max.z;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[1]=px[2]=px[3]=room->connect_box.min.x+ag_size_pillar_wid;
		pz[0]=pz[3]=room->connect_box.min.z;
		pz[1]=pz[2]=room->connect_box.max.z;
		py[0]=py[1]=ty+ag_size_pillar_wid;
		py[2]=py[3]=by;

		flip_poly_idx[0]=map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[1]=px[2]=px[3]=room->connect_box.max.x-ag_size_pillar_wid;

		flip_poly_idx[1]=map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

			// top pillar

		px[0]=room->connect_box.min.x;
		px[1]=room->connect_box.max.x;
		px[2]=room->connect_box.max.x-ag_size_pillar_wid;
		px[3]=room->connect_box.min.x+ag_size_pillar_wid;
		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.min.z;
		py[0]=py[1]=ty;
		py[2]=py[3]=ty+ag_size_pillar_wid;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.max.z;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

			// bottom bump

		px[0]=room->connect_box.min.x+ag_size_pillar_wid;
		px[1]=room->connect_box.max.x-ag_size_pillar_wid;
		px[2]=room->connect_box.max.x;
		px[3]=room->connect_box.min.x;
		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.min.z;
		py[0]=py[1]=by;
		py[2]=py[3]=by2;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.max.z;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

			// ceiling and floor

		px[0]=px[3]=room->connect_box.min.x+ag_size_pillar_wid;
		px[1]=px[2]=room->connect_box.max.x-ag_size_pillar_wid;
		pz[0]=pz[1]=room->connect_box.min.z;
		pz[2]=pz[3]=room->connect_box.max.z;
		py[0]=py[1]=py[2]=py[3]=by;

		flip_poly_idx[2]=map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		py[0]=py[1]=py[2]=py[3]=ty+ag_size_pillar_wid;

		flip_poly_idx[3]=map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);
	}
	else {

			// outside walls

		px[0]=px[1]=px[2]=px[3]=room->connect_box.min.x;
		pz[0]=pz[3]=room->connect_box.min.z;
		pz[1]=pz[2]=room->connect_box.min.z+ag_size_pillar_wid;
		py[0]=ty;
		py[1]=ty+ag_size_pillar_wid;
		py[2]=by;
		py[3]=by2;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[1]=px[2]=px[3]=room->connect_box.max.x;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[1]=px[2]=px[3]=room->connect_box.min.x;
		pz[0]=pz[3]=room->connect_box.max.z-ag_size_pillar_wid;
		pz[1]=pz[2]=room->connect_box.max.z;
		py[0]=ty+ag_size_pillar_wid;
		py[1]=ty;
		py[2]=by2;
		py[3]=by;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[1]=px[2]=px[3]=room->connect_box.max.x;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.min.z+ag_size_pillar_wid;
		py[0]=py[1]=ty+ag_size_pillar_wid;
		py[2]=py[3]=by;

		flip_poly_idx[0]=map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.max.z-ag_size_pillar_wid;

		flip_poly_idx[1]=map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

			// top pillar

		px[0]=px[1]=px[2]=px[3]=room->connect_box.min.x;
		pz[0]=room->connect_box.min.z;
		pz[1]=room->connect_box.max.z;
		pz[2]=room->connect_box.max.z-ag_size_pillar_wid;
		pz[3]=room->connect_box.min.z+ag_size_pillar_wid;
		py[0]=py[1]=ty;
		py[2]=py[3]=ty+ag_size_pillar_wid;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[1]=px[2]=px[3]=room->connect_box.max.x;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

			// bottom bump

		px[0]=px[1]=px[2]=px[3]=room->connect_box.min.x;
		pz[0]=room->connect_box.min.z+ag_size_pillar_wid;
		pz[1]=room->connect_box.max.z-ag_size_pillar_wid;
		pz[2]=room->connect_box.max.z;
		pz[3]=room->connect_box.min.z;
		py[0]=py[1]=by;
		py[2]=py[3]=by2;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[1]=px[2]=px[3]=room->connect_box.max.x;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

			// ceiling and floor

		px[0]=px[1]=room->connect_box.min.x;
		px[2]=px[3]=room->connect_box.max.x;
		pz[0]=pz[3]=room->connect_box.min.z+ag_size_pillar_wid;
		pz[1]=pz[2]=room->connect_box.max.z-ag_size_pillar_wid;
		py[0]=py[1]=py[2]=py[3]=by;

		flip_poly_idx[2]=map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		py[0]=py[1]=py[2]=py[3]=ty+ag_size_pillar_wid;

		flip_poly_idx[3]=map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);
	}

		// recalc UVs and normals

	map_mesh_reset_uv(map,mesh_idx);
	map_recalc_normals_mesh(map,&map->mesh.meshes[mesh_idx],normal_mode_out,FALSE);

		// because of the inside-out nature,
		// we need to flip some normals

	for (n=0;n!=4;n++) {
		map_flip_normals_mesh_poly(map,&map->mesh.meshes[mesh_idx],flip_poly_idx[n]);
	}
}

void ag_map_add_connector_room_arch(map_type *map,int org_room_idx)
{
	int					ty,by,by2;
	ag_room_type		*room,*room2;

	room=&ag_map_state.rooms[org_room_idx];
	room2=&ag_map_state.rooms[room->connect_box.other_room_idx];

		// pillars go to the top

	room->connect_box.min.y-=(ag_size_room_high+ag_size_floor_high);

		// remove unused polygons

	ag_map_remove_polygons_in_box(map,room->mesh_idx,&room->connect_box.min,&room->connect_box.max);
	ag_map_remove_polygons_in_box(map,room2->mesh_idx,&room->connect_box.min,&room->connect_box.max);

		// add in upper and lower arches


	ty=room->connect_box.max.y-ag_size_room_high;
	by=room->connect_box.max.y-ag_size_bump_high;
	by2=room->connect_box.max.y;

	ag_map_add_connector_room_arch_add(map,room,ty,by,by2);

	ty=room->connect_box.max.y-((ag_size_room_high*2)+ag_size_floor_high);
	by=room->connect_box.max.y-(ag_size_room_high+ag_size_floor_high);
	by2=room->connect_box.max.y-ag_size_room_high;

	ag_map_add_connector_room_arch_add(map,room,ty,by,by2);
}

/* =======================================================

      Stair Room Connectors
      
======================================================= */

void ag_map_add_connector_room_step(map_type *map,d3pnt *min,d3pnt *max)
{
	int				mesh_idx;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];

		// step mesh

	mesh_idx=map_mesh_add(map);

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

		// sides

	px[0]=px[1]=px[2]=px[3]=min->x;
	pz[0]=pz[3]=min->z;
	pz[1]=pz[2]=max->z;
	py[0]=py[1]=min->y;
	py[2]=py[3]=max->y;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_stair);

	px[0]=px[1]=px[2]=px[3]=max->x;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_stair);

	px[0]=px[3]=min->x;
	px[1]=px[2]=max->x;
	pz[0]=pz[1]=pz[2]=pz[3]=min->z;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_stair);

	pz[0]=pz[1]=pz[2]=pz[3]=max->z;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_stair);

		// top

	px[0]=px[3]=min->x;
	px[1]=px[2]=max->x;
	pz[0]=pz[1]=min->z;
	pz[2]=pz[3]=max->z;
	py[0]=py[1]=py[2]=py[3]=min->y;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_stair);

		// reset UVs and normals

	map_mesh_reset_uv(map,mesh_idx);
	map_recalc_normals_mesh(map,&map->mesh.meshes[mesh_idx],normal_mode_out,FALSE);
}

void ag_map_add_connector_room_stairs(map_type *map,int org_room_idx)
{
	int					n,mesh_idx,
						stair_type,stair_len,step_len,step_high,
						step_x_add,step_z_add,sz;
	int					px[8],py[8],pz[8];
	float				gx[8],gy[8];
	d3pnt				min,max,step_min,step_max;
	ag_room_type		*room,*room2;

	room=&ag_map_state.rooms[org_room_idx];
	room2=&ag_map_state.rooms[room->connect_box.other_room_idx];

		// remove unused polygons
		// since they are on different levels, we
		// need to remove only correct Ys

	memmove(&min,&room->connect_box.min,sizeof(d3pnt));
	memmove(&max,&room->connect_box.max,sizeof(d3pnt));
	ag_map_remove_polygons_in_box(map,room2->mesh_idx,&min,&max);

	min.y+=ag_size_room_high;
	max.y+=ag_size_room_high;
	ag_map_remove_polygons_in_box(map,room->mesh_idx,&min,&max);

		// outside walls/floor/ceiling

	mesh_idx=map_mesh_add(map);

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

	if ((room->connect_box.side==ag_connect_side_top) || (room->connect_box.side==ag_connect_side_bottom)) {

			// walls

		px[0]=px[1]=px[2]=px[3]=room->connect_box.min.x;
		pz[0]=pz[3]=room->connect_box.min.z;
		pz[1]=pz[2]=room->connect_box.max.z;
		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.max.y+ag_size_room_high;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[1]=px[2]=px[3]=room->connect_box.max.x;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		py[0]=py[1]=room->connect_box.max.y;
		py[2]=py[3]=room->connect_box.max.y+ag_size_room_high;

		pz[0]=pz[1]=pz[2]=pz[3]=((room->connect_box.side==ag_connect_side_top)?room->connect_box.max.z:room->connect_box.min.z);

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

			// ceilings

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[1]=room->connect_box.min.z;
		pz[2]=pz[3]=room->connect_box.max.z;

		if (room->connect_box.side==ag_connect_side_top) {
			py[0]=py[1]=room->connect_box.min.y+ag_size_room_high;
			py[2]=py[3]=room->connect_box.min.y;
		}
		else {
			py[0]=py[1]=room->connect_box.min.y;
			py[2]=py[3]=room->connect_box.min.y+ag_size_room_high;
		}

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_ceiling);
	}
	else {
			// walls

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.min.z;
		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.max.y+ag_size_room_high;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.max.z;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		py[0]=py[1]=room->connect_box.max.y;
		py[2]=py[3]=room->connect_box.max.y+ag_size_room_high;
		pz[0]=pz[3]=room->connect_box.min.z;
		pz[1]=pz[2]=room->connect_box.max.z;

		px[0]=px[1]=px[2]=px[3]=((room->connect_box.side==ag_connect_side_left)?room->connect_box.max.x:room->connect_box.min.x);

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

			// ceilings

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[1]=room->connect_box.min.z;
		pz[2]=pz[3]=room->connect_box.max.z;

		if (room->connect_box.side==ag_connect_side_left) {
			py[0]=py[3]=room->connect_box.min.y+ag_size_room_high;
			py[1]=py[2]=room->connect_box.min.y;
		}
		else {
			py[0]=py[3]=room->connect_box.min.y;
			py[1]=py[2]=room->connect_box.min.y+ag_size_room_high;
		}

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_ceiling);

	}

		// floor

	px[0]=px[3]=room->connect_box.min.x;
	px[1]=px[2]=room->connect_box.max.x;
	py[0]=py[2]=py[1]=py[3]=room->connect_box.max.y+ag_size_room_high;
	pz[0]=pz[1]=room->connect_box.min.z;
	pz[2]=pz[3]=room->connect_box.max.z;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_floor);

		// recalc the UVs and normals
		// we do the normals first as this works
		// around a twist that can develop in the UVs
		// of the ceiling

	map_recalc_normals_mesh(map,&map->mesh.meshes[mesh_idx],normal_mode_in,FALSE);
	map_mesh_reset_uv(map,mesh_idx);

		// get stair type

	stair_type=ag_random_int(3);

		// stairs going across the Z

	if ((room->connect_box.side==ag_connect_side_top) || (room->connect_box.side==ag_connect_side_bottom)) {

		stair_len=room->connect_box.max.z-room->connect_box.min.z;
		step_len=stair_len/ag_size_stair_count;
		step_high=(room->connect_box.max.y-room->connect_box.min.y)/ag_size_stair_count;

			// steps

		if (room->connect_box.side==ag_connect_side_top) {
			step_min.z=room->connect_box.min.z;
			step_max.z=room->connect_box.min.z+step_len;
			step_z_add=step_len;
		}
		else {
			step_min.z=room->connect_box.max.z-step_len;
			step_max.z=room->connect_box.max.z;
			step_z_add=-step_len;
		}

		switch (stair_type) {
			
			case ag_stair_type_one_side:
				step_min.x=room->connect_box.min.x;
				step_max.x=room->connect_box.min.x+((room->connect_box.max.x-room->connect_box.min.x)>>1);
				break;

			case ag_stair_type_middle:
				sz=(room->connect_box.max.x-room->connect_box.min.x)>>2;
				step_min.x=room->connect_box.min.x+sz;
				step_max.x=room->connect_box.max.x-sz;
				break;

			case ag_stair_type_full:
				step_min.x=room->connect_box.min.x;
				step_max.x=room->connect_box.max.x;
				break;

		}

		step_min.y=(room->connect_box.max.y+ag_size_room_high)-step_high;
		step_max.y=(room->connect_box.max.y+ag_size_room_high);

		for (n=0;n!=ag_size_stair_count;n++) {
			ag_map_add_connector_room_step(map,&step_min,&step_max);
			step_min.y-=step_high;
			step_min.z+=step_z_add;
			step_max.z+=step_z_add;
		}
	}

		// stairs going across the X

	else {
		stair_len=room->connect_box.max.x-room->connect_box.min.x;
		step_len=stair_len/ag_size_stair_count;
		step_high=(room->connect_box.max.y-room->connect_box.min.y)/ag_size_stair_count;

			// steps

		if (room->connect_box.side==ag_connect_side_left) {
			step_min.x=room->connect_box.min.x;
			step_max.x=room->connect_box.min.x+step_len;
			step_x_add=step_len;
		}
		else {
			step_min.x=room->connect_box.max.x-step_len;
			step_max.x=room->connect_box.max.x;
			step_x_add=-step_len;
		}

		switch (stair_type) {
			
			case ag_stair_type_one_side:
				step_min.z=room->connect_box.min.z;
				step_max.z=room->connect_box.min.z+((room->connect_box.max.z-room->connect_box.min.z)>>1);
				break;

			case ag_stair_type_middle:
				sz=(room->connect_box.max.z-room->connect_box.min.z)>>2;
				step_min.z=room->connect_box.min.z+sz;
				step_max.z=room->connect_box.max.z-sz;
				break;

			case ag_stair_type_full:
				step_min.z=room->connect_box.min.z;
				step_max.z=room->connect_box.max.z;
				break;

		}

		step_min.y=(room->connect_box.max.y+ag_size_room_high)-step_high;
		step_max.y=(room->connect_box.max.y+ag_size_room_high);

		for (n=0;n!=ag_size_stair_count;n++) {
			ag_map_add_connector_room_step(map,&step_min,&step_max);
			step_min.y-=step_high;
			step_min.x+=step_x_add;
			step_max.x+=step_x_add;
		}
	}
}

/* =======================================================

      Add Room Connectors
      
======================================================= */

void ag_map_add_connector_rooms(map_type *map)
{
	int				n;
	ag_room_type	*room;

	for (n=0;n!=ag_map_state.nroom;n++) {

			// did this room have a connection box?

		room=&ag_map_state.rooms[n];
		if (!room->connect_box.on) continue;

		switch (room->connect_box.connect_type) {

			case ag_connect_type_normal:
				ag_map_add_connector_room_normal(map,n,FALSE);
				break;

			case ag_connect_type_door:
				ag_map_add_connector_room_normal(map,n,TRUE);
				break;

			case ag_connect_type_arch:
				ag_map_add_connector_room_arch(map,n);
				break;

			case ag_connect_type_stairs:
				ag_map_add_connector_room_stairs(map,n);
				break;

		}
	}
}

