/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Auto Generation Utility

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

(c) 2000-2014 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3autogenerate.h"
#endif

extern ag_map_state_type		ag_map_state;

/* =======================================================

      Collisions
      
======================================================= */

bool ag_map_mesh_collision(map_type *map,d3pnt *min,d3pnt *max,int start_cmp_mesh_idx,int end_cmp_mesh_idx)
{
	int				n;
	d3pnt			cmp_min,cmp_max;

	for (n=start_cmp_mesh_idx;n<end_cmp_mesh_idx;n++) {

		map_mesh_calculate_extent(map,n,&cmp_min,&cmp_max);
		if ((min->x>=cmp_max.x) || (max->x<=cmp_min.x)) continue;
		if ((min->y>=cmp_max.y) || (max->y<=cmp_min.y)) continue;
		if ((min->z>=cmp_max.z) || (max->z<=cmp_min.z)) continue;
		
		return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Decoration Locations
      
======================================================= */

void ag_map_decoration_location_random(map_type *map,int room_idx,d3pnt *pnt)
{
	d3pnt			min,max,sz,margin;
	ag_room_type	*room;

		// get current room dimensions

	room=&ag_map_state.rooms[room_idx];
	map_mesh_calculate_extent(map,room->mesh_idx,&min,&max);

		// room size and margin

	sz.x=max.x-min.x;
	sz.z=max.z-min.z;

	margin.x=(int)(((float)sz.x)*0.25f);
	margin.z=(int)(((float)sz.z)*0.25f);

		// get random location

	pnt->x=(min.x+margin.x)+(ag_random_int(sz.x-(margin.x*2)));
	pnt->y=max.y;
	pnt->z=(min.z+margin.z)+(ag_random_int(sz.z-(margin.z*2)));
}

void ag_map_decoration_location_angle(map_type *map,int room_idx,float ang,d3pnt *pnt)
{
	d3pnt			min,max,sz;
	float			fx,fz,rang;
	ag_room_type	*room;

		// get current room dimensions

	room=&ag_map_state.rooms[room_idx];
	map_mesh_calculate_extent(map,room->mesh_idx,&min,&max);

		// room size and radius

	sz.x=max.x-min.x;
	sz.z=max.z-min.z;

	fx=((float)sz.x)*0.33f;
	fz=((float)sz.z)*0.33f;

		// get point

	pnt->x=(min.x+max.x)>>1;
	pnt->y=max.y;
	pnt->z=(min.z+max.z)>>1;

	rang=ang*ANG_to_RAD;

	pnt->x+=(int)(fx*sinf(rang));
	pnt->z-=(int)(fz*cosf(rang));
}


/* =======================================================

      Box Stack Decorations
      
======================================================= */

void ag_map_decoration_box(map_type *map,d3pnt *pnt,d3pnt *room_sz,int start_cmp_mesh_idx)
{
	int				n,x,z,sz;
	bool			hit;
	d3pnt			min,max;

		// find a random start position
	
	hit=FALSE;
	sz=ag_size_stack_box>>1;

	for (n=0;n!=10;n++) {
		x=pnt->x+(ag_random_int(room_sz->x)-(room_sz->x>>1));
		z=pnt->z+(ag_random_int(room_sz->z)-(room_sz->z>>1));

		min.x=x-sz;
		max.x=x+sz;
		min.y=pnt->y-ag_size_stack_box;
		max.y=pnt->y;
		min.z=z-sz;
		max.z=z+sz;

		if (start_cmp_mesh_idx!=-1) {
			if (!ag_map_mesh_collision(map,&min,&max,start_cmp_mesh_idx,map->mesh.nmesh)) {
				hit=TRUE;
				break;
			}
		}
	}

	if (hit) ag_map_primitive_cube(map,&min,&max,TRUE,-1,ag_texture_decoration_box);
}

void ag_map_decoration_box_stacks(map_type *map,int room_idx)
{
	int				n,row_count,high_count,cmp_mesh_idx;
	bool			horz;
	d3pnt			pnt,room_sz,min,max;
	ag_room_type	*room;

		// get random stack count

	row_count=ag_count_stack_box_row_start+ag_random_int(ag_count_stack_box_row_extra);

		// randomize location
		// move it so within stack of boxes

	room=&ag_map_state.rooms[room_idx];

	map_mesh_calculate_center(map,room->mesh_idx,&pnt);
	map_mesh_calculate_extent(map,room->mesh_idx,&min,&max);

	pnt.y=max.y;

	room_sz.x=max.x-min.x;
	room_sz.z=max.z-min.z;

	if (room_sz.x>room_sz.z) {
		horz=TRUE;
		room_sz.x=room_sz.x/3;
		room_sz.z=ag_size_stack_box*3;
	}
	else {
		horz=FALSE;
		room_sz.x=ag_size_stack_box*3;
		room_sz.z=room_sz.z/3;
	}

		// box stack

	high_count=0;
	cmp_mesh_idx=map->mesh.nmesh;

	while (row_count>ag_count_stack_box_row_min) {

		for (n=0;n!=row_count;n++) {
			ag_map_decoration_box(map,&pnt,&room_sz,cmp_mesh_idx);
		}

		if (horz) {
			room_sz.x-=ag_size_stack_box;
			if (room_sz.x<ag_size_stack_box) break;
		}
		else {
			room_sz.z-=ag_size_stack_box;
			if (room_sz.z<ag_size_stack_box) break;
		}

		row_count-=2;
		pnt.y-=ag_size_stack_box;
		
		high_count++;
		if (high_count>ag_count_stack_box_high_max) break;
	}
}

/* =======================================================

      Column Decorations
      
======================================================= */

void ag_map_decoration_columns(map_type *map,int room_idx)
{
	int				high,
					ang,start_ang,end_ang,ang_add;
	d3pnt			pnt;

		// get height

	high=ag_size_room_high+ag_size_floor_high;
	if (ag_map_state.rooms[room_idx].second_story) high+=ag_size_room_high;

		// create columns

	switch (ag_random_int(3)) {

		case ag_decoration_column_config_square:
			start_ang=45;
			end_ang=315;
			ang_add=90;
			break;

		case ag_decoration_column_config_diamond:
			start_ang=0;
			end_ang=270;
			ang_add=90;
			break;

		case ag_decoration_column_config_circle:
			start_ang=0;
			end_ang=315;
			ang_add=45;
			break;
	
	}

	ag_map_primitive_random_cylinder_setup(map);
	
	for (ang=start_ang;ang<=end_ang;ang+=ang_add) {
		ag_map_decoration_location_angle(map,room_idx,(float)ang,&pnt);
		ag_map_primitive_random_cylinder_y(map,&pnt,ag_size_column_normal_wid,high,ag_texture_decoration_pillar);
	}
}

/* =======================================================

      Equipment Decorations
      
======================================================= */

void ag_map_decoration_equipment_piece(map_type *map,d3pnt *pnt,int wid_x,int wid_z,int high,int texture_idx)
{
	int				mesh_idx;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];

		// equipment mesh

	mesh_idx=map_mesh_add(map);

	map->mesh.meshes[mesh_idx].flag.lock_uv=TRUE;

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

		// sides

	px[0]=px[1]=px[2]=px[3]=pnt->x;
	pz[0]=pz[3]=pnt->z;
	pz[1]=pz[2]=pnt->z+wid_z;
	py[0]=py[1]=pnt->y-high;
	py[2]=py[3]=pnt->y;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,texture_idx);

	px[0]=px[1]=px[2]=px[3]=pnt->x+wid_x;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,texture_idx);

	px[0]=px[3]=pnt->x;
	px[1]=px[2]=pnt->x+wid_x;
	pz[0]=pz[1]=pz[2]=pz[3]=pnt->z;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,texture_idx);

	pz[0]=pz[1]=pz[2]=pz[3]=pnt->z+wid_z;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,texture_idx);

		// top

	px[0]=px[3]=pnt->x;
	px[1]=px[2]=pnt->x+wid_x;
	pz[0]=pz[1]=pnt->z;
	pz[2]=pz[3]=pnt->z+wid_z;
	py[0]=py[1]=py[2]=py[3]=pnt->y-high;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,texture_idx);

		// reset just normals
		// as UVs are locked

	map_recalc_normals_mesh(map,&map->mesh.meshes[mesh_idx],normal_mode_out,FALSE);
}

void ag_map_decoration_equipment(map_type *map,int room_idx)
{
	int				n,high,
					dist,count,npoly,poly_idx;
	float			fx,fz;
	d3pnt			pnt,p1,p2;
	map_mesh_type	*mesh;
	ag_room_type	*room;

	room=&ag_map_state.rooms[room_idx];

		// get height

	high=ag_size_equipment_high_start+ag_random_int(ag_size_equipment_high_extra);

		// run through the polygons and find
		// walls we can put equipment against

	npoly=map->mesh.meshes[room->mesh_idx].npoly;

	for (poly_idx=0;poly_idx!=npoly;poly_idx++) {
			
		mesh=&map->mesh.meshes[room->mesh_idx];		// pointers might change as meshes are being added
		poly_idx=ag_random_int(mesh->npoly);
				
		if (!ag_map_is_polygon_window_target(map,room->mesh_idx,poly_idx)) continue;

			// get distance and determine
			// count

		ag_map_get_wall_line(map,room->mesh_idx,poly_idx,&p1,&p2);

		fx=(float)(p2.x-p1.x);
		fz=(float)(p2.z-p1.z);
		dist=(int)sqrtf((fx*fx)+(fz*fz));

		if (dist<(ag_size_equipment_pad_wid*2)) continue;

		count=dist/(ag_size_equipment_pad_wid*2);
		if (count<0) count=1;

			// equipment boxes

		for (n=0;n!=count;n++) {

			pnt.x=p1.x+(int)(((float)(p2.x-p1.x))*(((float)(n+1))/((float)(count+1))));
			pnt.y=room->max.y;
			pnt.z=p1.z+(int)(((float)(p2.z-p1.z))*(((float)(n+1))/((float)(count+1))));

				// pad

			pnt.x-=(ag_size_equipment_pad_wid>>1);
			pnt.z-=(ag_size_equipment_pad_wid>>1);

			ag_map_decoration_equipment_piece(map,&pnt,ag_size_equipment_pad_wid,ag_size_equipment_pad_wid,ag_size_floor_high,ag_texture_stair);

				// computer

			pnt.x+=((ag_size_equipment_pad_wid-ag_size_equipment_computer_wid)>>1);
			pnt.y-=ag_size_floor_high;
			pnt.z+=((ag_size_equipment_pad_wid-ag_size_equipment_computer_wid)>>1);

			ag_map_decoration_equipment_piece(map,&pnt,ag_size_equipment_computer_wid,ag_size_equipment_computer_wid,high,ag_texture_equipment);
		}
	}

		// these count as wall decorations,
		// so we avoid vertex decorations

	room->has_wall_decorations=TRUE;
}

/* =======================================================

      Core Decorations
      
======================================================= */

void ag_map_decoration_core(map_type *map,int room_idx)
{
	int					n,extrude_mesh_idx,
						poly_idx;
	d3pnt				extrude_pnt;
	ag_room_type		*room;

	room=&ag_map_state.rooms[room_idx];

		// find the ceiling

	poly_idx=ag_map_find_floor_polygon(map,room_idx);
	if (poly_idx==-1) return;

		// extrude it

	extrude_pnt.x=0;
	extrude_pnt.y=-(ag_size_room_high+ag_size_floor_high);
	if (room->second_story) extrude_pnt.y-=ag_size_room_high;
	extrude_pnt.z=0;

		// extrude it

	extrude_mesh_idx=map_mesh_poly_punch_hole(map,room->mesh_idx,poly_idx,&extrude_pnt,FALSE,normal_mode_out);
	if (extrude_mesh_idx==-1) return;

	for (n=0;n!=map->mesh.meshes[extrude_mesh_idx].npoly;n++) {
		map->mesh.meshes[extrude_mesh_idx].polys[n].txt_idx=ag_texture_alt_wall;
	}
	
	map_mesh_reset_uv(map,extrude_mesh_idx);
}

/* =======================================================

      Wall Decorations
      
======================================================= */

void ag_map_decoration_walls(map_type *map,int room_idx)
{
	int					n,k,mx,my,mz,ty,by,
						wall_mesh_idx;
	int					px[8],py[8],pz[8],px2[8],py2[8],pz2[8],
						kx[4],ky[4],kz[4];
	float				f_wall,gx[8],gy[8],mgx,mgy;
	bool				need_ceiling;
	d3pnt				*pt;
	ag_room_type		*room;

	room=&ag_map_state.rooms[room_idx];
	if (room->nvertex==0) return;

		// random wall size

	f_wall=0.49f-(((float)ag_random_int(6))*0.01f);

		// new wall vertexes

	mx=my=mz=0;
	mgx=mgy=0.0f;

	for (n=0;n!=room->nvertex;n++) {
		pt=&room->vertexes[n];

		px[n]=px2[n]=pt->x;
		py[n]=py2[n]=pt->y;
		pz[n]=pz2[n]=pt->z;

		mx+=pt->x;
		my+=pt->y;
		mz+=pt->z;
	}

	mx/=room->nvertex;
	my/=room->nvertex;
	mz/=room->nvertex;

	for (n=0;n!=room->nvertex;n++) {
		px[n]=(int)(((float)(px[n]-mx))*0.5f)+mx;
		py[n]=(int)(((float)(py[n]-my))*0.5f)+my;
		pz[n]=(int)(((float)(pz[n]-mz))*0.5f)+mz;
		px2[n]=(int)(((float)(px[n]-mx))*f_wall)+mx;
		py2[n]=(int)(((float)(py[n]-my))*f_wall)+my;
		pz2[n]=(int)(((float)(pz[n]-mz))*f_wall)+mz;
	}

		// wall heights
		
	if (ag_random_bool()) {
		ty=room->max.y-(ag_size_room_high+ag_size_floor_high);
		need_ceiling=room->second_story;
	}
	else {
		ty=room->max.y-(ag_size_room_high>>1);
		need_ceiling=TRUE;
	}
	by=room->max.y;

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

		// new mesh for each wall

	for (n=0;n!=room->nvertex;n++) {

			// random select a wall

		if (ag_random_bool()) continue;

			// new wall
		
		wall_mesh_idx=map_mesh_add(map);
		if (wall_mesh_idx==-1) return;

		k=n+1;
		if (k==room->nvertex) k=0;

		kx[0]=kx[3]=px[n];
		kx[1]=kx[2]=px[k];

		ky[0]=ky[1]=ty;
		ky[2]=ky[3]=by;

		kz[0]=kz[3]=pz[n];
		kz[1]=kz[2]=pz[k];

		map_mesh_add_poly(map,wall_mesh_idx,4,kx,ky,kz,gx,gy,ag_texture_alt_wall);

		kx[0]=kx[3]=px2[n];
		kx[1]=kx[2]=px2[k];

		kz[0]=kz[3]=pz2[n];
		kz[1]=kz[2]=pz2[k];

		map_mesh_add_poly(map,wall_mesh_idx,4,kx,ky,kz,gx,gy,ag_texture_alt_wall);

		kx[0]=kx[3]=px[k];
		kx[1]=kx[2]=px2[k];

		kz[0]=kz[3]=pz[k];
		kz[1]=kz[2]=pz2[k];

		map_mesh_add_poly(map,wall_mesh_idx,4,kx,ky,kz,gx,gy,ag_texture_alt_wall);

		kx[0]=kx[3]=px[n];
		kx[1]=kx[2]=px2[n];

		kz[0]=kz[3]=pz[n];
		kz[1]=kz[2]=pz2[n];

		map_mesh_add_poly(map,wall_mesh_idx,4,kx,ky,kz,gx,gy,ag_texture_alt_wall);

			// possible ceiling

		if (need_ceiling) {
			kx[0]=px[n];
			kx[1]=px[k];
			kx[2]=px2[k];
			kx[3]=px2[n];

			ky[0]=ky[1]=ky[2]=ky[3]=ty;

			kz[0]=pz[n];
			kz[1]=pz[k];
			kz[2]=pz2[k];
			kz[3]=pz2[n];

			map_mesh_add_poly(map,wall_mesh_idx,4,kx,ky,kz,gx,gy,ag_texture_second_floor);
		}

			// recalc UVs and normals

		map_mesh_reset_uv(map,wall_mesh_idx);
		map_recalc_normals_mesh(map,&map->mesh.meshes[wall_mesh_idx],normal_mode_out,FALSE);
	}
}

/* =======================================================

      Floor Trench Decorations
      
======================================================= */

void ag_map_decoration_floor_trench(map_type *map,int room_idx)
{
	int					n,depth_count,
						mesh_idx,poly_idx,extrude_mesh_idx;
	d3pnt				extrude_pnt;
	ag_room_type		*room;

	room=&ag_map_state.rooms[room_idx];

		// find the floor

	poly_idx=ag_map_find_floor_polygon(map,room_idx);
	if (poly_idx==-1) return;

		// run to random depth

	mesh_idx=room->mesh_idx;
	depth_count=1+ag_random_int(2);

	for (n=0;n!=depth_count;n++) {

			// extrude it

		extrude_pnt.x=0;
		extrude_pnt.y=ag_random_bool()?ag_size_trench_high:-ag_size_trench_high;
		extrude_pnt.z=0;

			// extrude it

		extrude_mesh_idx=map_mesh_poly_punch_hole(map,mesh_idx,poly_idx,&extrude_pnt,TRUE,((extrude_pnt.y>0)?normal_mode_in:normal_mode_out));
		if (extrude_mesh_idx==-1) break;
		
		map_mesh_reset_uv(map,extrude_mesh_idx);

			// last polygon in extrude mesh is always
			// the new extrude

		mesh_idx=extrude_mesh_idx;
		poly_idx=map->mesh.meshes[mesh_idx].npoly-1;
	}
}

/* =======================================================

      Machinery Decorations
      
======================================================= */

void ag_map_decoration_floor_machinery(map_type *map,int room_idx)
{
	int					x,z,count,wid,m_wid,high,sec,
						group_idx,movement_idx,move_idx;
	d3pnt				pnt,m_min,m_max,min,max;
	movement_type		*movement;
	movement_move_type	*move;
	ag_room_type		*room;

	room=&ag_map_state.rooms[room_idx];

		// all machinery same size

	map_mesh_calculate_extent(map,room->mesh_idx,&m_min,&m_max);

	wid=ag_size_machinery_start+ag_random_int(ag_size_machinery_extra);
	m_wid=(wid<<1)+(wid>>1);

	high=ag_size_room_high+ag_size_floor_high;
	if (room->second_story) high+=ag_size_room_high;

	if (ag_random_bool()) {
		count=2;
	}
	else {
		count=4;
	}

		// build machinery

	for (x=0;x!=count;x++) {

		for (z=0;z!=count;z++) {

				// position

			pnt.x=(((m_min.x+m_max.x)>>1)-((m_wid*count)>>1))+(x*m_wid);
			pnt.x+=wid;
			pnt.z=(((m_min.z+m_max.z)>>1)-((m_wid*count)>>1))+(z*m_wid);
			pnt.z+=wid;

				// create the group
				// and movement

			group_idx=map_group_add(map);
			sprintf(map->group.groups[group_idx].name,"Machine %d.%d",room_idx,((x*count)+z));

			movement_idx=map_movement_add(map);

			movement=&map->movement.movements[movement_idx];

			sprintf(movement->name,"Machine %d.%d",room_idx,((x*count)+z));
			movement->group_idx=group_idx;
			movement->auto_start=TRUE;
			movement->loop=TRUE;

			sec=3000+ag_random_int(3000);

			move_idx=map_movement_move_add(map,movement_idx);

			move=&movement->moves[move_idx];
			move->msec=sec;
			move->mov.y=-(high-100);

			strcpy(move->sound_name,"Lift");

			move_idx=map_movement_move_add(map,movement_idx);

			move=&movement->moves[move_idx];
			move->msec=sec;
			move->mov.y=high-100;

			strcpy(move->sound_name,"Lift");

				// machine

			min.x=pnt.x-wid;
			min.y=room->max.y-high;
			min.z=pnt.z-wid;

			max.x=pnt.x+wid;
			max.y=room->max.y;
			max.z=pnt.z+wid;

			ag_map_primitive_cube(map,&min,&max,FALSE,group_idx,ag_texture_lift);
		}
	}
}

/* =======================================================

      Rubble Decorations
      
======================================================= */

void ag_map_decoration_floor_rubble(map_type *map,int room_idx)
{
	int					n,count,wid_x,wid_z,high;
	d3pnt				pnt,min,max;
	ag_room_type		*room;

	room=&ag_map_state.rooms[room_idx];

	count=ag_count_rubble_start+ag_random_int(ag_count_rubble_extra);

	for (n=0;n!=count;n++) {
		ag_map_decoration_location_random(map,room_idx,&pnt);

		wid_x=ag_size_rubble_start+ag_random_int(ag_size_rubble_extra);
		wid_z=ag_size_rubble_start+ag_random_int(ag_size_rubble_extra);
		high=ag_size_rubble_start+ag_random_int(ag_size_rubble_extra);

		min.x=pnt.x-wid_x;
		min.y=room->max.y-high;
		min.z=pnt.z-wid_z;

		max.x=pnt.x+wid_x;
		max.y=room->max.y;
		max.z=pnt.z+wid_z;

		ag_map_primitive_rubble(map,&min,&max,(ag_random_bool()?ag_texture_stair:ag_texture_decoration_pillar));
	}
}

/* =======================================================

      Open Ceiling Decorations
      
======================================================= */

void ag_decoration_open_ceiling(map_type *map,int room_idx)
{
	int					extrude_mesh_idx,poly_idx;
	d3pnt				extrude_pnt;
	ag_room_type		*room;

	room=&ag_map_state.rooms[room_idx];

		// find the ceiling

	poly_idx=ag_map_find_ceiling_polygon(map,room_idx);
	if (poly_idx==-1) return;

		// extrude it

	extrude_pnt.x=0;
	extrude_pnt.y=-(ag_size_floor_high*2);
	extrude_pnt.z=0;

		// extrude it

	extrude_mesh_idx=map_mesh_poly_punch_hole(map,room->mesh_idx,poly_idx,&extrude_pnt,FALSE,normal_mode_in);
	if (extrude_mesh_idx!=-1) map_mesh_reset_uv(map,extrude_mesh_idx);
}

/* =======================================================

      Vertex Decorations
      
======================================================= */

void ag_gnerate_decoration_vertex_column(map_type *map,int room_idx)
{
	int				n,high,radius;
	ag_room_type	*room;

	room=&ag_map_state.rooms[room_idx];

		// get height and size

	high=ag_size_room_high+ag_size_floor_high;
	if (ag_map_state.rooms[room_idx].second_story) high+=ag_size_room_high;

	radius=ag_size_column_vertex_wid_start+ag_random_int(ag_size_column_vertex_wid_extra);

		// vertex columns

	ag_map_primitive_random_cylinder_setup(map);

	for (n=0;n!=room->nvertex;n++) {
		ag_map_primitive_random_cylinder_y(map,&room->vertexes[n],radius,high,ag_texture_stair);
	}
}

/* =======================================================

      Decorations
      
======================================================= */

void ag_map_decorations_add(map_type *map)
{
	int				n,k,dec_idx;
	ag_room_type	*room;

	for (n=0;n!=ag_map_state.nroom;n++) {
		room=&ag_map_state.rooms[n];

			// completely skip outside rooms

		if (room->outside) continue;

			// try a couple of times
			// to get the right decoration for
			// a room, skip any room with stairs

		dec_idx=ag_decoration_type_none;

		if (!room->has_stairs) {
			for (k=0;k!=10;k++) {
				dec_idx=ag_random_int(8);

				switch (dec_idx) {

						// columns, equipment, machinery
						// don't appear in two story rooms

					case ag_decoration_type_columns:
					case ag_decoration_type_equipment:
					case ag_decoration_type_machinery:
						if (room->second_story) continue;
						break;

						// core decoration requires window light

					case ag_decoration_type_core:
						if (!room->has_windows) continue;
						break;

				}

				break;
			}
		}

			// middle decorations

		switch (dec_idx) {

			case ag_decoration_type_columns:
				ag_map_decoration_columns(map,n);
				break;

			case ag_decoration_type_box_stacks:
				ag_map_decoration_box_stacks(map,n);
				break;

			case ag_decoration_type_equipment:
				ag_map_decoration_equipment(map,n);
				break;

			case ag_decoration_type_core:
				ag_map_decoration_core(map,n);
				break;

			case ag_decoration_type_walls:
				ag_map_decoration_walls(map,n);
				break;

			case ag_decoration_type_trench:
				ag_map_decoration_floor_trench(map,n);
				break;

			case ag_decoration_type_machinery:
				ag_map_decoration_floor_machinery(map,n);
				break;

			case ag_decoration_type_rubble:
				ag_map_decoration_floor_rubble(map,n);
				break;

		}

			// vertex decorations
			// skip if already things on wall

		if (!room->has_wall_decorations) {
			if (ag_random_bool()) ag_gnerate_decoration_vertex_column(map,n);
		}

			// ceiling decorations

		if ((room->second_story) && (room->has_windows)) {
			if (ag_random_bool()) {
				ag_decoration_open_ceiling(map,n);
			}
		}
	}
}

/* =======================================================

      Add Lights
      
======================================================= */

void ag_map_lights_add_fixture(map_type *map,d3pnt *pnt)
{
	int				mesh_idx,ty,by;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];

		// get top and bottom y

	ty=pnt->y-(ag_size_floor_high*3);
	by=ty+ag_size_floor_high;

		// fixture mesh

	mesh_idx=map_mesh_add(map);

	map->mesh.meshes[mesh_idx].flag.lock_uv=TRUE;

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

		// sides

	px[0]=px[1]=px[2]=px[3]=pnt->x-ag_size_light_fixture_wid;
	pz[0]=pz[3]=pnt->z-ag_size_light_fixture_wid;
	pz[1]=pz[2]=pnt->z+ag_size_light_fixture_wid;
	py[0]=py[1]=ty;
	py[2]=py[3]=by;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_pillar);

	px[0]=px[1]=px[2]=px[3]=pnt->x+ag_size_light_fixture_wid;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_pillar);

	px[0]=px[3]=pnt->x-ag_size_light_fixture_wid;
	px[1]=px[2]=pnt->x+ag_size_light_fixture_wid;
	pz[0]=pz[1]=pz[2]=pz[3]=pnt->z-ag_size_light_fixture_wid;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_pillar);

	pz[0]=pz[1]=pz[2]=pz[3]=pnt->z+ag_size_light_fixture_wid;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_pillar);

		// bottom

	px[0]=px[3]=pnt->x-ag_size_light_fixture_wid;
	px[1]=px[2]=pnt->x+ag_size_light_fixture_wid;
	pz[0]=pz[1]=pnt->z-ag_size_light_fixture_wid;
	pz[2]=pz[3]=pnt->z+ag_size_light_fixture_wid;
	py[0]=py[1]=py[2]=py[3]=by;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_pillar);

		// reset UVs and normals

	map_mesh_reset_uv(map,mesh_idx);
	map_recalc_normals_mesh(map,&map->mesh.meshes[mesh_idx],normal_mode_out,FALSE);
}

void ag_map_lights_add_spot(map_type *map,d3pnt *pnt)
{
	map_light_type	*lit;

		// exit if we run out of lights

	if (map->nlight==max_map_light) return;
		
		// add the light

	lit=&map->lights[map->nlight];
	sprintf(lit->name,"light %d",map->nlight);

	map->nlight++;
			
		// set the light

	memmove(&lit->pnt,pnt,sizeof(d3pnt));
	
	lit->setting.on=TRUE;
	lit->setting.light_map=TRUE;
	lit->setting.halo_name[0]=0x0;

	lit->setting.type=lt_normal;
	lit->setting.direction=ld_all;

	lit->setting.intensity=ag_size_light_width_start+ag_random_int(ag_size_light_width_extra);
	lit->setting.exponent=1.0f;

	lit->setting.col.r=1.0f-(((float)ag_random_int(25))/100.0f);
	lit->setting.col.g=1.0f-(((float)ag_random_int(25))/100.0f);
	lit->setting.col.b=1.0f-(((float)ag_random_int(25))/100.0f);
}

void ag_map_lights_add_room_light(map_type *map,ag_room_type *room,int y_add)
{
	d3pnt			pnt,min,max;

		// add the light

	map_mesh_calculate_extent(map,room->mesh_idx,&min,&max);

	pnt.x=(min.x+max.x)>>1;
	pnt.y=max.y-(ag_size_room_high-(ag_size_floor_high<<1));
	if (room->second_story) pnt.y-=ag_size_room_high;
	pnt.y+=y_add;
	pnt.z=(min.z+max.z)>>1;

	ag_map_lights_add_spot(map,&pnt);

		// add the fixture

	ag_map_lights_add_fixture(map,&pnt);
}

void ag_map_lights_add(map_type *map)
{
	int				n;
	ag_room_type	*room;

	for (n=0;n!=ag_map_state.nroom;n++) {

			// skip rooms that are lit
			// by windows

		room=&ag_map_state.rooms[n];
		if (room->outside) continue;
		if (room->has_windows) continue;

			// add the light
			// and a second one if a complete
			// two story room

		ag_map_lights_add_room_light(map,room,0);
		if (room->second_story_complete) ag_map_lights_add_room_light(map,room,(ag_size_room_high+ag_size_floor_high));
	}
}

/* =======================================================

      Add Spots
      
======================================================= */

void ag_map_spots_add_single(map_type *map,char *name,int spot_obj_type,char *script_name,bool leaf_only,d3pnt *rtn_pnt)
{
	int				n,room_idx,poly_idx;
	spot_type		*spot;
	ag_room_type	*room;

		// add the spot

	spot=&map->spots[map->nspot];
	map->nspot++;

		// usually for player spot

	if (leaf_only) {
		room_idx=0;

		for (n=0;n!=ag_map_state.nroom;n++) {
			if (ag_map_room_is_leaf(n)) {
				room_idx=n;
				break;
			}
		}
	}

		// randomize room

	else {
		room_idx=ag_random_int(ag_map_state.nroom);
	}

		// find a floor in the room
	
	room=&ag_map_state.rooms[room_idx];

	poly_idx=ag_map_find_floor_polygon(map,room_idx);
	if (poly_idx==-1) {
		spot->pnt.x=(room->min.x+room->max.x)>>1;
		spot->pnt.y=room->max.y;
		spot->pnt.z=(room->min.z+room->max.z)>>1;
	}
	else {
		map_mesh_poly_calculate_center(map,room->mesh_idx,poly_idx,&spot->pnt);
	}

	spot->ang.x=0.0f;
	spot->ang.y=0.0f;
	spot->ang.z=0.0f;

		// name and type
	
	strcpy(spot->name,name);
	spot->type=spot_obj_type;
	strcpy(spot->script,script_name);
	spot->display_model[0]=0x0;
	spot->params[0]=0x0;

		// return point

	if (rtn_pnt!=NULL) memmove(rtn_pnt,&spot->pnt,sizeof(d3pnt));
}

void ag_map_spots_add(map_type *map)
{
	int				n;

		// player spot

	ag_map_spots_add_single(map,"Start",spot_type_player,"Player",TRUE,&ag_map_state.start_pnt);

		// monster spots
		// supergumba -- hard coded!  FIX!

	for (n=0;n!=(ag_map_state.nroom*2);n++) {
	//	ag_map_spots_add_single(map,"Monster",spot_type_object,"Cyborg Knife",FALSE,NULL);
	}

}
