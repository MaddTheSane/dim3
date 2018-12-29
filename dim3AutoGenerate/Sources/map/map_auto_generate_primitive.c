/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Auto Generation Primitive Generation

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

int								ag_cylinder_side_count,ag_cylinder_split_count;

/* =======================================================

      Cubes
      
======================================================= */

void ag_map_primitive_cube(map_type *map,d3pnt *min,d3pnt *max,bool rotate,int group_idx,int txt_idx)
{
	int				mesh_idx;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];
	d3pnt			mpt;
	d3ang			ang;

		// box mesh

	mesh_idx=map_mesh_add(map);

	map->mesh.meshes[mesh_idx].flag.lock_uv=TRUE;

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

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);

	px[0]=px[1]=px[2]=px[3]=max->x;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);

	px[0]=px[3]=min->x;
	px[1]=px[2]=max->x;
	pz[0]=pz[1]=pz[2]=pz[3]=min->z;
	py[0]=py[1]=min->y;
	py[2]=py[3]=max->y;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);

	pz[0]=pz[1]=pz[2]=pz[3]=max->z;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);

		// top and bottom

	px[0]=px[3]=min->x;
	px[1]=px[2]=max->x;
	pz[0]=pz[1]=min->z;
	pz[2]=pz[3]=max->z;
	py[0]=py[1]=py[2]=py[3]=max->y;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);

	py[0]=py[1]=py[2]=py[3]=min->y;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);

		// possibly rotate box

	if (rotate) {
		if (ag_random_bool()) {
			ang.x=ang.z=0.0f;
			ang.y=(float)ag_random_int(30);

			map_mesh_calculate_center(map,mesh_idx,&mpt);
			map_mesh_rotate(map,mesh_idx,&mpt,&ang);
		}
	}

		// reset just normals
		// as UVs are locked

	map_recalc_normals_mesh(map,&map->mesh.meshes[mesh_idx],normal_mode_out,FALSE);

		// if a group, than moveable

	if (group_idx!=1) {
		map->mesh.meshes[mesh_idx].flag.moveable=TRUE;
		map->mesh.meshes[mesh_idx].group_idx=group_idx;
	}
}

/* =======================================================

      Rubble
      
======================================================= */

void ag_map_primitive_rubble(map_type *map,d3pnt *min,d3pnt *max,int txt_idx)
{
	int				n,mesh_idx;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];
	d3pnt			mpt;
	d3pnt			*pnt;
	d3ang			ang;
	map_mesh_type	*mesh;

		// box mesh

	mesh_idx=map_mesh_add(map);

	map->mesh.meshes[mesh_idx].flag.lock_uv=TRUE;

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

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);

	px[0]=px[1]=px[2]=px[3]=max->x;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);

	px[0]=px[3]=min->x;
	px[1]=px[2]=max->x;
	pz[0]=pz[1]=pz[2]=pz[3]=min->z;
	py[0]=py[1]=min->y;
	py[2]=py[3]=max->y;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);

	pz[0]=pz[1]=pz[2]=pz[3]=max->z;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);

		// top and bottom

	px[0]=px[3]=min->x;
	px[1]=px[2]=max->x;
	pz[0]=pz[1]=min->z;
	pz[2]=pz[3]=max->z;
	py[0]=py[1]=py[2]=py[3]=max->y;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);

	py[0]=py[1]=py[2]=py[3]=min->y;

	map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);

		// possibly rotate box

	if (ag_random_bool()) {
		ang.x=ang.z=0.0f;
		ang.y=(float)ag_random_int(30);

		map_mesh_calculate_center(map,mesh_idx,&mpt);
		map_mesh_rotate(map,mesh_idx,&mpt,&ang);
	}

		// randomize the vertexes
		// that aren't on the bottom

	mesh=&map->mesh.meshes[mesh_idx];

	pnt=mesh->vertexes;

	for (n=0;n!=mesh->nvertex;n++) {
		if (pnt->y<max->y) {
			pnt->x+=(200-ag_random_int(400));
			pnt->y+=(200-ag_random_int(400));
			pnt->z+=(200-ag_random_int(400));
		}
		pnt++;
	}

		// reset just normals
		// as UVs are locked

	map_recalc_normals_mesh(map,&map->mesh.meshes[mesh_idx],normal_mode_out,FALSE);
}

/* =======================================================

      Cylinders
      
======================================================= */

int ag_map_primitive_cylinder_y(map_type *map,int mesh_idx,d3pnt *pnt,int start_radius,int end_radius,int high,int side_count,int txt_idx)
{
	int				n;
	int				px[4],py[4],pz[4];
	float			ang,next_ang,ang_add,
					f_start_radius,f_end_radius;
	float			gx[4],gy[4];

		// the mesh

	if (mesh_idx==-1) mesh_idx=map_mesh_add(map);

		// column

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

		// sides

	ang=0.0f;
	ang_add=(2.0f*TRIG_PI)/((float)side_count);

	f_start_radius=(float)start_radius;
	f_end_radius=(float)end_radius;

	for (n=0;n!=side_count;n++) {

		next_ang=ang+ang_add;
		if (n==9) next_ang=0.0f;

		px[0]=pnt->x+(int)(f_end_radius*sinf(ang));
		px[1]=pnt->x+(int)(f_end_radius*sinf(next_ang));
		px[2]=pnt->x+(int)(f_start_radius*sinf(next_ang));
		px[3]=pnt->x+(int)(f_start_radius*sinf(ang));

		pz[0]=pnt->z-(int)(f_end_radius*cosf(ang));
		pz[1]=pnt->z-(int)(f_end_radius*cosf(next_ang));
		pz[2]=pnt->z-(int)(f_start_radius*cosf(next_ang));
		pz[3]=pnt->z-(int)(f_start_radius*cosf(ang));

		py[0]=py[1]=pnt->y-high;
		py[2]=py[3]=pnt->y;

		map_mesh_add_poly(map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);

			// exit early if we reconnect early

		ang=next_ang;
	}

		// reset UVs and normals

	map_mesh_reset_uv(map,mesh_idx);
	map_recalc_normals_mesh(map,&map->mesh.meshes[mesh_idx],normal_mode_out,FALSE);

	return(mesh_idx);
}

void ag_map_primitive_random_cylinder_setup(map_type *map)
{
	ag_cylinder_side_count=4+ag_random_int(8);
	ag_cylinder_split_count=1+ag_random_int(4);
}

void ag_map_primitive_random_cylinder_y(map_type *map,d3pnt *pnt,int radius,int high,int txt_idx)
{
	int			n,mesh_idx,d_high,
				start_radius,end_radius;
	d3pnt		d_pnt;

	d_pnt.x=pnt->x;
	d_pnt.y=pnt->y;
	d_pnt.z=pnt->z;

	start_radius=radius;
	d_high=high/ag_cylinder_split_count;

	mesh_idx=-1;

	for (n=0;n!=ag_cylinder_split_count;n++) {
		if (n==(ag_cylinder_split_count-1)) d_high=d_pnt.y-(pnt->y-high);

		end_radius=radius-(ag_random_int(radius>>1));

		mesh_idx=ag_map_primitive_cylinder_y(map,mesh_idx,&d_pnt,start_radius,end_radius,d_high,ag_cylinder_side_count,txt_idx);

		d_pnt.y-=d_high;
		start_radius=end_radius;
	}
}

