/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Mesh Updates

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

/* =======================================================

      Combine Meshes
      
======================================================= */

void map_mesh_combine_single_mesh(map_mesh_type *mesh,map_mesh_type *mesh_copy)
{
	int					n,t,k,v_idx;
	d3pnt				*d3pt,*chk_d3pt;
	map_mesh_poly_type	*mesh_poly,*mesh_copy_poly;
	
		// get place we last stopped adding
		
	mesh_poly=&mesh->polys[mesh->npoly];
	mesh_copy_poly=mesh_copy->polys;
	
	for (n=0;n!=mesh_copy->npoly;n++) {
	
			// add the poly
			
		memmove(mesh_poly,mesh_copy_poly,sizeof(map_mesh_poly_type));
	
			// add in the vertexes, checking for combines
			
		for (t=0;t!=mesh_copy_poly->ptsz;t++) {
			chk_d3pt=&mesh_copy->vertexes[mesh_copy_poly->v[t]];
			
			v_idx=-1;
			d3pt=mesh->vertexes;

			for (k=0;k!=mesh->nvertex;k++) {
				if ((d3pt->x==chk_d3pt->x) && (d3pt->y==chk_d3pt->y) && (d3pt->z==chk_d3pt->z)) {
					v_idx=k;
					break;
				}
				
				d3pt++;
			}

				// need to add new vertex
				
			if (v_idx==-1) {
				v_idx=mesh->nvertex;
				mesh->nvertex++;
				
				d3pt=&mesh->vertexes[v_idx];
				d3pt->x=chk_d3pt->x;
				d3pt->y=chk_d3pt->y;
				d3pt->z=chk_d3pt->z;
			}

			mesh_poly->v[t]=v_idx;
		}
		
		mesh->npoly++;
		
		mesh_poly++;
		mesh_copy_poly++;
	}
}

int map_mesh_combine(map_type *map,int mesh_1_idx,int mesh_2_idx)
{
	int					mesh_idx;
	map_mesh_type		*mesh,*mesh_1,*mesh_2;
	
		// create new combined mesh
		
	mesh_idx=map_mesh_add(map);
	if (mesh_idx==-1) return(-1);

		// get combined meshes
		// need to get after mesh add as mesh add can change mesh pointers
		
	mesh_1=&map->mesh.meshes[mesh_1_idx];
	mesh_2=&map->mesh.meshes[mesh_2_idx];
	
		// setup enough vertexes and polys for new mesh
		
	if (!map_mesh_set_vertex_count(map,mesh_idx,(mesh_1->nvertex+mesh_2->nvertex))) {
		map_mesh_delete(map,mesh_idx);
		return(-1);
	}
	
	if (!map_mesh_set_poly_count(map,mesh_idx,(mesh_1->npoly+mesh_2->npoly))) {
		map_mesh_delete(map,mesh_idx);
		return(-1);
	}
	
		// transfer over some settings from first mesh
		
	mesh=&map->mesh.meshes[mesh_idx];

	mesh->npoly=0;
	mesh->nvertex=0;
		
	mesh->group_idx=mesh_1->group_idx;
	mesh->hide_mode=mesh_1->hide_mode;
	mesh->harm=mesh_1->harm;

	memmove(&mesh->rot_off,&mesh_1->rot_off,sizeof(d3pnt));
	memmove(&mesh->flag,&mesh_1->flag,sizeof(map_mesh_flag_type));
	memmove(&mesh->msg,&mesh_1->msg,sizeof(map_mesh_message_type));

		// combined meshes
		
	map_mesh_combine_single_mesh(mesh,mesh_1);
	map_mesh_combine_single_mesh(mesh,mesh_2);
	
		// get back to correct size
		// ignore failures as it's just a waste of space that
		// will be reclaimed later
		
	map_mesh_set_vertex_count(map,mesh_idx,mesh->nvertex);
	map_mesh_set_poly_count(map,mesh_idx,mesh->npoly);
	
		// delete original meshes, making sure to
		// change delete index depending on first delete
		// and return mesh index minus the two
		// deleted meshes
		
	map_mesh_delete(map,mesh_1_idx);
	
	if (mesh_1_idx<mesh_2_idx) mesh_2_idx--;
	map_mesh_delete(map,mesh_2_idx);	

	return(mesh_idx-2);
}

/* =======================================================

      In Game Mesh Move and Rotate With Copy Vertexes
      
======================================================= */

void map_mesh_move_rotate_copy_reset(map_type *map,int mesh_idx)
{
	if (map->mesh.meshes[mesh_idx].copy.vertexes!=NULL) free(map->mesh.meshes[mesh_idx].copy.vertexes);
	map->mesh.meshes[mesh_idx].copy.vertexes=NULL;
}

void map_mesh_move_rotate_copy(map_type *map,int mesh_idx,d3pnt *center_pnt,d3pnt *move_pnt,d3ang *rot_ang)
{
	int							n,nvertex,npoly;
	float						fx,fy,fz;
	bool						is_rot;
	d3pnt						*srce_pt,*dest_pt;
	d3pnt						mpt;
	matrix_type					mat;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;

	mesh=&map->mesh.meshes[mesh_idx];

		// get the copy

	if (!map_mesh_create_copy_data(map,mesh_idx)) return;

		// get mesh rotation center

	mpt.x=center_pnt->x+mesh->rot_off.x;
	mpt.y=center_pnt->y+mesh->rot_off.y;
	mpt.z=center_pnt->z+mesh->rot_off.z;

		// matrixes

	is_rot=(rot_ang->x!=0.0f) || (rot_ang->y!=0.0f) || (rot_ang->z!=0.0f);
	if (is_rot) matrix_rotate_xyz(&mat,rot_ang->x,rot_ang->y,rot_ang->z);

		// rotate and move vertexes

	nvertex=mesh->nvertex;
	npoly=mesh->npoly;

	srce_pt=mesh->copy.vertexes;
	dest_pt=mesh->vertexes;

		// version with no rotation

	if (!is_rot) {
		for (n=0;n!=nvertex;n++) {
			dest_pt->x=srce_pt->x+move_pnt->x;
			dest_pt->y=srce_pt->y+move_pnt->y;
			dest_pt->z=srce_pt->z+move_pnt->z;

			srce_pt++;
			dest_pt++;
		}
	}

		// version with rotation

	else {
		for (n=0;n!=nvertex;n++) {
			fx=(float)(srce_pt->x-mpt.x);
			fy=(float)(srce_pt->y-mpt.y);
			fz=(float)(srce_pt->z-mpt.z);

			matrix_vertex_multiply_ignore_transform(&mat,&fx,&fy,&fz);

			dest_pt->x=(((int)fx)+mpt.x)+move_pnt->x;
			dest_pt->y=(((int)fy)+mpt.y)+move_pnt->y;
			dest_pt->z=(((int)fz)+mpt.z)+move_pnt->z;

			srce_pt++;
			dest_pt++;
		}
	
			// rotate normals
			
		poly=mesh->polys;
		
		for (n=0;n!=npoly;n++) {
			poly->tangent_space.tangent.x=mesh->copy.tangent_spaces[n].tangent.x;
			poly->tangent_space.tangent.y=mesh->copy.tangent_spaces[n].tangent.y;
			poly->tangent_space.tangent.z=mesh->copy.tangent_spaces[n].tangent.z;
			matrix_vertex_multiply_ignore_transform(&mat,&poly->tangent_space.tangent.x,&poly->tangent_space.tangent.y,&poly->tangent_space.tangent.z);

			poly->tangent_space.normal.x=mesh->copy.tangent_spaces[n].normal.x;
			poly->tangent_space.normal.y=mesh->copy.tangent_spaces[n].normal.y;
			poly->tangent_space.normal.z=mesh->copy.tangent_spaces[n].normal.z;
			matrix_vertex_multiply_ignore_transform(&mat,&poly->tangent_space.normal.x,&poly->tangent_space.normal.y,&poly->tangent_space.normal.z);

			poly++;
		}
	}

		// fix boxes

	poly=mesh->polys;
	
	for (n=0;n!=npoly;n++) {
		map_prepare_mesh_poly(map,mesh,poly);
		poly++;
	}

		// fix mesh box

	map_prepare_mesh_box(mesh);
}

/* =======================================================

      Move Mesh
      
======================================================= */

void map_mesh_move(map_type *map,int mesh_idx,d3pnt *mov_pnt)
{
	int							n,nvertex,npoly;
	d3pnt						*pt;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;

	mesh=&map->mesh.meshes[mesh_idx];

		// move meshes vertexes

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		pt->x+=mov_pnt->x;
		pt->y+=mov_pnt->y;
		pt->z+=mov_pnt->z;
		pt++;
	}

		// move all poly boxes
		
	npoly=mesh->npoly;
	poly=mesh->polys;
	
	for (n=0;n!=npoly;n++) {
		map_prepare_mesh_poly(map,mesh,poly);
		poly++;
	}

		// fix mesh box

	map_prepare_mesh_box(mesh);
}

/* =======================================================

      Resize Mesh
      
======================================================= */

void map_mesh_resize(map_type *map,int mesh_idx,d3pnt *min,d3pnt *max)
{
	int					n,nvertex;
	d3pnt				*pt,org_min,org_max,dif,org_dif;
	d3vct				fct;
	map_mesh_type		*mesh;

		// get original size and uv center
		
	map_mesh_calculate_extent(map,mesh_idx,&org_min,&org_max);

		// get resize factor
		
	dif.x=max->x-min->x;
	dif.y=max->y-min->y;
	dif.z=max->z-min->z;
	
	org_dif.x=org_max.x-org_min.x;
	org_dif.y=org_max.y-org_min.y;
	org_dif.z=org_max.z-org_min.z;

	fct.x=fct.y=fct.z=0.0f;

	if (org_dif.x!=0) fct.x=(float)dif.x/(float)org_dif.x;
	if (org_dif.y!=0) fct.y=(float)dif.y/(float)org_dif.y;
	if (org_dif.z!=0) fct.z=(float)dif.z/(float)org_dif.z;
	
		// resize vertexes

	mesh=&map->mesh.meshes[mesh_idx];

		// resize vertexes

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {

		pt->x=(int)((float)(pt->x-org_min.x)*fct.x)+min->x;
		pt->y=(int)((float)(pt->y-org_min.y)*fct.y)+min->y;
		pt->z=(int)((float)(pt->z-org_min.z)*fct.z)+min->z;

		pt++;
	}
}

/* =======================================================

      Flip Mesh
      
======================================================= */

void map_mesh_flip(map_type *map,int mesh_idx,bool flip_x,bool flip_y,bool flip_z)
{
	int						n,nvertex,npoly;
	d3pnt					mpt;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

		// get center

	map_mesh_calculate_center(map,mesh_idx,&mpt);

		// flip vertexes

	mesh=&map->mesh.meshes[mesh_idx];

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		if (flip_x) pt->x=mpt.x-(pt->x-mpt.x);
		if (flip_y) pt->y=mpt.y-(pt->y-mpt.y);
		if (flip_z) pt->z=mpt.z-(pt->z-mpt.z);
		pt++;
	}
	
		// flip polys
		
	npoly=mesh->npoly;
	poly=mesh->polys;
	
	for (n=0;n!=npoly;n++) {
		if (flip_x) poly->tangent_space.normal.x=-poly->tangent_space.normal.x;
		if (flip_y) poly->tangent_space.normal.y=-poly->tangent_space.normal.y;
		if (flip_z) poly->tangent_space.normal.z=-poly->tangent_space.normal.z;
		poly++;
	}
}

/* =======================================================

      Rotate Mesh
      
======================================================= */

void map_mesh_rotate(map_type *map,int mesh_idx,d3pnt *center_pnt,d3ang *rot_ang)
{
	int							n,nvertex,npoly;
	float						fx,fy,fz;
	d3pnt						*pt,mpt;
	matrix_type					mat;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;

	mesh=&map->mesh.meshes[mesh_idx];

		// get mesh rotation center

	mpt.x=center_pnt->x+mesh->rot_off.x;
	mpt.y=center_pnt->y+mesh->rot_off.y;
	mpt.z=center_pnt->z+mesh->rot_off.z;

		// matrixes

	matrix_rotate_xyz(&mat,rot_ang->x,rot_ang->y,rot_ang->z);

		// rotate vertexes

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		fx=(float)(pt->x-mpt.x);
		fy=(float)(pt->y-mpt.y);
		fz=(float)(pt->z-mpt.z);

		matrix_vertex_multiply_ignore_transform(&mat,&fx,&fy,&fz);

		pt->x=((int)floor(fx))+mpt.x;
		pt->y=((int)floor(fy))+mpt.y;
		pt->z=((int)floor(fz))+mpt.z;

		pt++;
	}
	
		// rotate normals
		
	npoly=mesh->npoly;
	poly=mesh->polys;
	
	for (n=0;n!=npoly;n++) {
		matrix_vertex_multiply_ignore_transform(&mat,&poly->tangent_space.tangent.x,&poly->tangent_space.tangent.y,&poly->tangent_space.tangent.z);
		matrix_vertex_multiply_ignore_transform(&mat,&poly->tangent_space.normal.x,&poly->tangent_space.normal.y,&poly->tangent_space.normal.z);
		poly++;
	}

		// fix boxes

	poly=mesh->polys;
	
	for (n=0;n!=npoly;n++) {
		map_prepare_mesh_poly(map,mesh,poly);
		poly++;
	}

		// fix mesh box

	map_prepare_mesh_box(mesh);
}

/* =======================================================

      Skew Mesh
      
======================================================= */

void map_mesh_skew(map_type *map,int mesh_idx,int axis,int dir,int size)
{
	int						n,nvertex;
	float					skew_fact,skew_size;
	d3pnt					min,max,sz;
	d3pnt					*pt;
	map_mesh_type			*mesh;

		// get extent

	map_mesh_calculate_extent(map,mesh_idx,&min,&max);

	sz.x=max.x-min.x;
	sz.y=max.y-min.y;
	sz.z=max.z-min.z;
	
	skew_size=(float)size;

		// skew from min to max

	mesh=&map->mesh.meshes[mesh_idx];

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
	
			// get the skew direction factor
			
		skew_fact=0.0f;
		
		switch (dir) {
			case 0:
				skew_fact=(float)(pt->x-min.x)/(float)sz.x;
				break;
			case 1:
				skew_fact=(float)(pt->y-min.y)/(float)sz.y;
				break;
			case 2:
				skew_fact=(float)(pt->z-min.z)/(float)sz.z;
				break;
		}
		
			// skew that axis
			
		switch (axis) {
			case 0:
				pt->x+=(int)(skew_size*skew_fact);
				break;
			case 1:
				pt->y+=(int)(skew_size*skew_fact);
				break;
			case 2:
				pt->z+=(int)(skew_size*skew_fact);
				break;
		}

		pt++;
	}
}

/* =======================================================

      Tesselate Mesh
      
======================================================= */

bool map_mesh_tesselate(map_type *map,int mesh_idx)
{
	int						n,k,cnt,ntrig,npoly;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly,*trig_polys,*trig_poly;

	mesh=&map->mesh.meshes[mesh_idx];

	if (mesh->npoly==0) return(TRUE);

		// count number of eventually trigs

	npoly=mesh->npoly;
	poly=mesh->polys;

	ntrig=0;

	for (n=0;n!=npoly;n++) {
		ntrig+=(poly->ptsz-2);
		poly++;
	}

		// create trigs

	trig_polys=(map_mesh_poly_type*)malloc(sizeof(map_mesh_poly_type)*ntrig);
	if (trig_polys==NULL) return(FALSE);

	trig_poly=trig_polys;
	poly=mesh->polys;

	for (n=0;n!=npoly;n++) {

		cnt=poly->ptsz-2;

		for (k=0;k!=cnt;k++) {
			memmove(trig_poly,poly,sizeof(map_mesh_poly_type));

			trig_poly->ptsz=3;

			trig_poly->v[0]=poly->v[0];
			trig_poly->v[1]=poly->v[k+1];
			trig_poly->v[2]=poly->v[k+2];
			
			trig_poly->main_uv.uvs[0].x=poly->main_uv.uvs[0].x;
			trig_poly->main_uv.uvs[1].x=poly->main_uv.uvs[k+1].x;
			trig_poly->main_uv.uvs[2].x=poly->main_uv.uvs[k+2].x;
			
			trig_poly->main_uv.uvs[0].y=poly->main_uv.uvs[0].y;
			trig_poly->main_uv.uvs[1].y=poly->main_uv.uvs[k+1].y;
			trig_poly->main_uv.uvs[2].y=poly->main_uv.uvs[k+2].y;
			
			trig_poly++;
		}

		poly++;
	}

		// substitute the trigs

	free(mesh->polys);

	mesh->npoly=ntrig;
	mesh->polys=trig_polys;

	return(TRUE);
}

/* =======================================================

      Poly Punch Hole
      
======================================================= */

int map_mesh_poly_punch_hole(map_type *map,int mesh_idx,int poly_idx,d3pnt *extrude_pnt,bool extrude_close,int extrude_normal_mode)
{
	int						n,ptsz,next_poly_idx,mx,my,mz,
							extrude_mesh_idx,extrude_poly_idx;
	int						px[8],py[8],pz[8],
							k,kx[8],ky[8],kz[8];
	float					gx[8],gy[8],mgx,mgy,
							k_gx[8],k_gy[8];
	d3pnt					*pt;
	map_mesh_type			*mesh,*extrude_mesh;
	map_mesh_poly_type		*poly;

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

		// new polygon for each vertex

	for (n=0;n!=ptsz;n++) {

		poly=&mesh->polys[poly_idx];			// add might force memory move, need to always rebuild pointer

		k=n+1;
		if (k==ptsz) k=0;

		pt=&mesh->vertexes[poly->v[n]];
		kx[0]=pt->x;
		ky[0]=pt->y;
		kz[0]=pt->z;

		pt=&mesh->vertexes[poly->v[k]];
		kx[1]=pt->x;
		ky[1]=pt->y;
		kz[1]=pt->z;

		kx[2]=px[k];
		ky[2]=py[k];
		kz[2]=pz[k];

		kx[3]=px[n];
		ky[3]=py[n];
		kz[3]=pz[n];

		k_gx[0]=poly->main_uv.uvs[n].x;
		k_gy[0]=poly->main_uv.uvs[n].y;

		k_gx[1]=poly->main_uv.uvs[k].x;
		k_gy[1]=poly->main_uv.uvs[k].y;

		k_gx[2]=gx[k];
		k_gy[2]=gy[k];

		k_gx[3]=gx[n];
		k_gy[3]=gy[n];

		next_poly_idx=map_mesh_add_poly(map,mesh_idx,4,kx,ky,kz,k_gx,k_gy,poly->txt_idx);
		if (next_poly_idx==-1) return(-1);

		memmove(&mesh->polys[next_poly_idx].tangent_space,&mesh->polys[poly_idx].tangent_space,sizeof(tangent_space_type));		// preserve original tangent space
	}
	
		// extruded polys

	extrude_mesh_idx=-1;
		
	if (extrude_pnt!=NULL) {

		extrude_mesh_idx=map_mesh_add(map);
		if (extrude_mesh_idx==-1) return(-1);

		mesh=&map->mesh.meshes[mesh_idx];			// add might force memory move, need to always rebuild pointer

		for (n=0;n!=ptsz;n++) {

			poly=&mesh->polys[poly_idx];			// add might force memory move, need to always rebuild pointer

			k=n+1;
			if (k==ptsz) k=0;

			kx[0]=px[n]+extrude_pnt->x;
			ky[0]=py[n]+extrude_pnt->y;
			kz[0]=pz[n]+extrude_pnt->z;

			kx[1]=px[n];
			ky[1]=py[n];
			kz[1]=pz[n];

			kx[2]=px[k];
			ky[2]=py[k];
			kz[2]=pz[k];

			kx[3]=px[k]+extrude_pnt->x;
			ky[3]=py[k]+extrude_pnt->y;
			kz[3]=pz[k]+extrude_pnt->z;

			k_gx[0]=0.0f;
			k_gy[0]=0.0f;

			k_gx[1]=1.0f;
			k_gy[1]=0.0f;

			k_gx[2]=1.0f;
			k_gy[2]=1.0f;

			k_gx[3]=0.0f;
			k_gy[3]=1.0f;

			extrude_poly_idx=map_mesh_add_poly(map,extrude_mesh_idx,4,kx,ky,kz,k_gx,k_gy,poly->txt_idx);
			if (extrude_poly_idx==-1) return(-1);
		}

			// extruded close

		if (extrude_close) {

			poly=&mesh->polys[poly_idx];

			for (n=0;n!=ptsz;n++) {
				kx[n]=px[n]+extrude_pnt->x;
				ky[n]=py[n]+extrude_pnt->y;
				kz[n]=pz[n]+extrude_pnt->z;
			}

			extrude_poly_idx=map_mesh_add_poly(map,extrude_mesh_idx,ptsz,kx,ky,kz,gx,gy,poly->txt_idx);
			if (extrude_poly_idx==-1) return(-1);
		}

			// rebuild normals
			// if an close, it needs same normals as original polygon

		extrude_mesh=&map->mesh.meshes[extrude_mesh_idx];

		map_recalc_normals_mesh(map,extrude_mesh,extrude_normal_mode,FALSE);
		if (extrude_close) memmove(&extrude_mesh->polys[extrude_poly_idx].tangent_space,&mesh->polys[poly_idx].tangent_space,sizeof(tangent_space_type));
	}

		// finish by deleting original polygon

	if (!map_mesh_delete_poly(map,mesh_idx,poly_idx)) return(-1);

	if (extrude_mesh_idx==-1) return(mesh_idx);
	return(extrude_mesh_idx);
}

/* =======================================================

      Tesselate Poly
      
======================================================= */

bool map_mesh_poly_tesselate(map_type *map,int mesh_idx,int poly_idx)
{
	int						n,cnt;
	int						new_poly_idxs[8],px[3],py[3],pz[3];
	float					gx[3],gy[3];
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly,*trig_poly;

	mesh=&map->mesh.meshes[mesh_idx];
	
	poly=&mesh->polys[poly_idx];
	if (poly->ptsz==3) return(TRUE);

		// create trigs for poly
		// we'll make fake trigs and move over
		// data later

	cnt=poly->ptsz-2;
		
	px[0]=px[1]=px[2]=0;
	py[0]=py[1]=py[2]=0;
	pz[0]=pz[1]=pz[2]=0;
	gx[0]=gx[1]=gx[2]=0.0f;
	gy[0]=gy[1]=gy[2]=0.0f;
		
	for (n=0;n!=cnt;n++) {
		new_poly_idxs[n]=map_mesh_add_poly(map,mesh_idx,3,px,py,pz,gx,gy,0);
	}
		
		// build the new polys
		// need to get poly pointer again because
		// memory might have changed
		
	poly=&mesh->polys[poly_idx];

	for (n=0;n!=cnt;n++) {
		trig_poly=&mesh->polys[new_poly_idxs[n]];
		
		memmove(trig_poly,poly,sizeof(map_mesh_poly_type));

		trig_poly->ptsz=3;

		trig_poly->v[0]=poly->v[0];
		trig_poly->v[1]=poly->v[n+1];
		trig_poly->v[2]=poly->v[n+2];
		
		trig_poly->main_uv.uvs[0].x=poly->main_uv.uvs[0].x;
		trig_poly->main_uv.uvs[1].x=poly->main_uv.uvs[n+1].x;
		trig_poly->main_uv.uvs[2].x=poly->main_uv.uvs[n+2].x;
		
		trig_poly->main_uv.uvs[0].y=poly->main_uv.uvs[0].y;
		trig_poly->main_uv.uvs[1].y=poly->main_uv.uvs[n+1].y;
		trig_poly->main_uv.uvs[2].y=poly->main_uv.uvs[n+2].y;
		
		trig_poly++;
	}
	
		// delete the original
		
	map_mesh_delete_poly(map,mesh_idx,poly_idx);

	return(TRUE);
}

/* =======================================================

      Update Meshes for Texture Shifting
      
======================================================= */

static inline float map_mesh_poly_run_shifts_single_choord(float f_tick,float shift)
{
	int				i_add;
	float			f_add;

	f_add=(f_tick*0.001f)*shift;
	i_add=(int)f_add;				// keep within 0..1
	return(f_add-((float)i_add));
}

void map_mesh_poly_run_shifts(map_type *map,int tick)
{
	int						n,k,nmesh,npoly;
	float					f_tick;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	
	f_tick=(float)tick;

		// run through the meshes

	nmesh=map->mesh.nmesh;
	mesh=map->mesh.meshes;

	for (n=0;n!=nmesh;n++) {

			// shiftable?

		if (!mesh->precalc_flag.shiftable) {
			mesh++;
			continue;
		}

			// shift polygons

		npoly=mesh->npoly;
		poly=mesh->polys;

		for (k=0;k!=npoly;k++) {

			if (poly->draw.shift_on) {
				poly->draw.shift_offset.x=map_mesh_poly_run_shifts_single_choord(f_tick,poly->shift.x);
				poly->draw.shift_offset.y=map_mesh_poly_run_shifts_single_choord(f_tick,poly->shift.y);
			}

			poly++;
		}

		mesh++;
	}
}

/* =======================================================

      Poly UV Coordinates as Offset/Size
      
======================================================= */

float map_get_texture_reduce_coord(float f)
{
	int			i;

	i=(int)f;
	return(f-(float)i);
}

float map_get_texture_round_coord(float f)
{
	int			i;

	i=(int)(f*100.0f);
	return(((float)i)/100.0f);
}

void map_mesh_get_poly_uv_as_box(map_type *map,int mesh_idx,int poly_idx,bool light_map_uv,float *x_txtoff,float *y_txtoff,float *x_txtfact,float *y_txtfact)
{
	int						n;
	float					gx_min,gx_max,gy_min,gy_max;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	map_mesh_poly_uv_type	*uv;

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

	if (!light_map_uv) {
		uv=&poly->main_uv;
	}
	else {
		uv=&poly->lmap_uv;
	}

		// get UV extends

	gx_min=gx_max=uv->uvs[0].x;
	gy_min=gy_max=uv->uvs[0].y;

	for (n=1;n<poly->ptsz;n++) {
		if (uv->uvs[n].x<gx_min) gx_min=uv->uvs[n].x;
		if (uv->uvs[n].x>gx_max) gx_max=uv->uvs[n].x;
		if (uv->uvs[n].y<gy_min) gy_min=uv->uvs[n].y;
		if (uv->uvs[n].y>gy_max) gy_max=uv->uvs[n].y;
	}

		// create boxed coordinates

	*x_txtoff=map_get_texture_round_coord(gx_min);
	*y_txtoff=map_get_texture_round_coord(gy_min);

	*x_txtfact=map_get_texture_round_coord(gx_max-gx_min);
	*y_txtfact=map_get_texture_round_coord(gy_max-gy_min);
}

void map_mesh_set_poly_uv_as_box(map_type *map,int mesh_idx,int poly_idx,bool light_map_uv,float x_txtoff,float y_txtoff,float x_txtfact,float y_txtfact)
{
	int						n;
	float					gx,gy,
							org_x_txtoff,org_y_txtoff,org_x_txtfact,org_y_txtfact;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	map_mesh_poly_uv_type	*uv;

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

	if (!light_map_uv) {
		uv=&poly->main_uv;
	}
	else {
		uv=&poly->lmap_uv;
	}

		// get current box coordinates

	map_mesh_get_poly_uv_as_box(map,mesh_idx,poly_idx,light_map_uv,&org_x_txtoff,&org_y_txtoff,&org_x_txtfact,&org_y_txtfact);

		// reset to new coordinates

	for (n=0;n!=poly->ptsz;n++) {
	
		if (uv->uvs[n].x==org_x_txtoff) {
			uv->uvs[n].x=x_txtoff;
		}
		else {
			if (uv->uvs[n].x==(org_x_txtoff+org_x_txtfact)) {
				uv->uvs[n].x=x_txtoff+x_txtfact;
			}
			else {
				gx=uv->uvs[n].x-org_x_txtoff;
				gx/=org_x_txtfact;
				gx*=x_txtfact;
				uv->uvs[n].x=x_txtoff+gx;
			}
		}
		
		if (uv->uvs[n].y==org_y_txtoff) {
			uv->uvs[n].y=y_txtoff;
		}
		else {
			if (uv->uvs[n].y==(org_y_txtoff+org_y_txtfact)) {
				uv->uvs[n].y=y_txtoff+y_txtfact;
			}
			else {
				gy=uv->uvs[n].y-org_y_txtoff;
				gy/=org_y_txtfact;
				gy*=y_txtfact;
				uv->uvs[n].y=y_txtoff+gy;
			}
		}
	}
}

/* =======================================================

      Rotate and Flip UVs
      
======================================================= */

void map_mesh_rotate_poly_uv(map_type *map,int mesh_idx,int poly_idx,int rot_ang)
{
	int						n;
	float					g;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	
	if (rot_ang==ta_0) return;

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
	for (n=0;n!=poly->ptsz;n++) {
	
		switch (rot_ang) {
		
			case 90:
				g=poly->main_uv.uvs[n].x;
				poly->main_uv.uvs[n].x=poly->main_uv.uvs[n].y;
				poly->main_uv.uvs[n].y=-g;
				break;
				
			case 180:
				poly->main_uv.uvs[n].x=-poly->main_uv.uvs[n].x;
				poly->main_uv.uvs[n].y=-poly->main_uv.uvs[n].y;
				break;
				
			case 270:
				g=poly->main_uv.uvs[n].x;
				poly->main_uv.uvs[n].x=-poly->main_uv.uvs[n].y;
				poly->main_uv.uvs[n].y=g;
				break;
		
		}
	}
}

void map_mesh_flip_poly_uv(map_type *map,int mesh_idx,int poly_idx,bool flip_u,bool flip_v)
{
	int						n;
	float					min_gx,max_gx,min_gy,max_gy,g;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	
	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
		// get min and max
		
	min_gx=max_gx=poly->main_uv.uvs[0].x;
	min_gy=max_gy=poly->main_uv.uvs[0].y;
	
	for (n=1;n<poly->ptsz;n++) {
		if (poly->main_uv.uvs[n].x<min_gx) min_gx=poly->main_uv.uvs[n].x;
		if (poly->main_uv.uvs[n].x>max_gx) max_gx=poly->main_uv.uvs[n].x;
		if (poly->main_uv.uvs[n].y<min_gy) min_gy=poly->main_uv.uvs[n].y;
		if (poly->main_uv.uvs[n].y>max_gy) max_gy=poly->main_uv.uvs[n].y;
	}
	
	if ((flip_u) && (max_gx==min_gx)) return;
	if ((flip_v) && (max_gy==min_gy)) return;
	
		// flip
		
	for (n=0;n!=poly->ptsz;n++) {
		
		if (flip_u) {
			g=1.0f-((poly->main_uv.uvs[n].x-min_gx)/(max_gx-min_gx));
			poly->main_uv.uvs[n].x=min_gx+((max_gx-min_gx)*g);
		}
			
		if (flip_v) {
			g=1.0f-((poly->main_uv.uvs[n].y-min_gy)/(max_gy-min_gy));
			poly->main_uv.uvs[n].y=min_gy+((max_gy-min_gy)*g);
		}
	}
}

/* =======================================================

      Recalculate UVs
      
======================================================= */

void map_get_texture_uv_get_scale(map_type *map,int txt_idx,d3uv *uv_offset,d3uv *uv_size)
{
	texture_type		*texture;

	texture=&map->textures[txt_idx];

	uv_offset->x=texture->scale.uv_offset.x;
	uv_offset->y=texture->scale.uv_offset.y;
	uv_size->x=texture->scale.uv_size.x/1000.0f;
	uv_size->y=texture->scale.uv_size.y/1000.0f;
}

void map_mesh_reset_poly_uv(map_type *map,int mesh_idx,int poly_idx)
{
	int						n,kx,ky,kz;
	float					f,fx,fy,fz,ltxtx,rtxtx,ltxty,rtxty,ltxtz,rtxtz,
							x_txtoff,y_txtoff,x_txtfact,y_txtfact,x_txtfact_2,
							f_dist_1,f_dist_2;
	d3pnt					*pt;
	d3uv					uv_offset,uv_size;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
		// get scale

	map_get_texture_uv_get_scale(map,poly->txt_idx,&uv_offset,&uv_size);
	
		// setup box and slope
		// needed for texture calculations

	map_prepare_mesh_poly(map,mesh,poly);

		// walls-like polygons
		
	if (poly->box.wall_like) {
		
		ltxtx=(float)(poly->line.lx+poly->line.lz)*uv_size.x;
		rtxtx=(float)(poly->line.rx+poly->line.rz)*uv_size.x;
			
			// get point texture factor
				
		x_txtfact=rtxtx-ltxtx;
			
			// get distance texture factor
				
		fx=(float)(poly->line.rx-poly->line.lx);
		fz=(float)(poly->line.rz-poly->line.lz);
		f=(fx*fx)+(fz*fz);
		x_txtfact_2=sqrtf(f)*uv_size.x;
		if (x_txtfact<0) x_txtfact_2=-x_txtfact_2;
		
		if (fabsf(x_txtfact_2)>fabsf(x_txtfact)) x_txtfact=x_txtfact_2;		// if distance calc is longer, use that
		
		x_txtoff=map_get_texture_round_coord(map_get_texture_reduce_coord(ltxtx))+uv_offset.x;
		x_txtfact=map_get_texture_round_coord(x_txtfact);
		
		ltxty=((float)poly->box.min.y)*uv_size.y;
		rtxty=((float)poly->box.max.y)*uv_size.y;
		
		y_txtoff=map_get_texture_round_coord(map_get_texture_reduce_coord(ltxty))+uv_offset.y;
		y_txtfact=map_get_texture_round_coord(rtxty-ltxty);
				
			// create the polygons UVs
			
		fx=(float)(poly->box.max.x-poly->line.lx);
		fz=(float)(poly->box.max.z-poly->line.lz);
		f_dist_2=sqrtf((fx*fx)+(fz*fz));
			
		for (n=0;n!=poly->ptsz;n++) {
			pt=&mesh->vertexes[poly->v[n]];

			fx=(float)(pt->x-poly->line.lx);
			fz=(float)(pt->z-poly->line.lz);
			f_dist_1=sqrtf((fx*fx)+(fz*fz));
			
			ky=pt->y-poly->box.min.y;

			fx=f_dist_1/f_dist_2;
			fy=(float)ky/(float)(poly->box.max.y-poly->box.min.y);

			poly->main_uv.uvs[n].x=x_txtoff+(x_txtfact*fx);
			poly->main_uv.uvs[n].y=y_txtoff+(y_txtfact*fy);
		}
		
		return;
	}
			
		// floor-like polygon

	ltxtx=((float)poly->box.min.x)*uv_size.x;
	rtxtx=((float)poly->box.max.x)*uv_size.x;

	ltxtz=((float)poly->box.min.z)*uv_size.y;
	rtxtz=((float)poly->box.max.z)*uv_size.y;

	x_txtoff=map_get_texture_round_coord(map_get_texture_reduce_coord(ltxtx))+uv_offset.x;
	x_txtfact=map_get_texture_round_coord(rtxtx-ltxtx);
	
	y_txtoff=map_get_texture_round_coord(map_get_texture_reduce_coord(ltxtz))+uv_offset.y;
	y_txtfact=map_get_texture_round_coord(rtxtz-ltxtz);

		// set the polygon UVs
		
	for (n=0;n!=poly->ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];

		kx=pt->x-poly->box.min.x;
		kz=pt->z-poly->box.min.z;

		fx=(float)kx/(float)(poly->box.max.x-poly->box.min.x);
		fz=(float)kz/(float)(poly->box.max.z-poly->box.min.z);

		poly->main_uv.uvs[n].x=x_txtoff+(x_txtfact*fx);
		poly->main_uv.uvs[n].y=y_txtoff+(y_txtfact*fz);
	}
}

void map_mesh_reset_uv(map_type *map,int mesh_idx)
{
	int						n,npoly;
	map_mesh_type			*mesh;

	mesh=&map->mesh.meshes[mesh_idx];

	npoly=mesh->npoly;

	for (n=0;n!=npoly;n++) {
		map_mesh_reset_poly_uv(map,mesh_idx,n);
	}
}

/* =======================================================

      Whole Number UVs
      
======================================================= */

void map_mesh_whole_poly_uv(map_type *map,int mesh_idx,int poly_idx)
{
	int						n;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

	for (n=0;n!=poly->ptsz;n++) {
		poly->main_uv.uvs[n].x=(float)floor(poly->main_uv.uvs[n].x);
		poly->main_uv.uvs[n].y=(float)floor(poly->main_uv.uvs[n].y);
	}
}

void map_mesh_whole_uv(map_type *map,int mesh_idx)
{
	int						n,npoly;
	map_mesh_type			*mesh;

	mesh=&map->mesh.meshes[mesh_idx];

	npoly=mesh->npoly;

	for (n=0;n!=npoly;n++) {
		map_mesh_whole_poly_uv(map,mesh_idx,n);
	}
}

/* =======================================================

      Single Stamp Number UVs
      
======================================================= */

void map_mesh_poly_single_uv(map_type *map,int mesh_idx,int poly_idx)
{
	int						n;
	float					m_gx,m_gy;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

		// find the middle gx/gy

	m_gx=m_gy=0.0f;

	for (n=0;n!=poly->ptsz;n++) {
		m_gx+=poly->main_uv.uvs[n].x;
		m_gy+=poly->main_uv.uvs[n].y;
	}

	m_gx=m_gx/(float)poly->ptsz;
	m_gy=m_gy/(float)poly->ptsz;

		// spread the 0-1 around the middle

	for (n=0;n!=poly->ptsz;n++) {

		if (poly->main_uv.uvs[n].x<m_gx) {
			poly->main_uv.uvs[n].x=0.0f;
		}
		else {
			poly->main_uv.uvs[n].x=1.0f;
		}

		if (poly->main_uv.uvs[n].y<m_gy) {
			poly->main_uv.uvs[n].y=0.0f;
		}
		else {
			poly->main_uv.uvs[n].y=1.0f;
		}
	}
}

void map_mesh_poly_multipy_uv(map_type *map,int mesh_idx,int poly_idx,float x,float y)
{
	int						n;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

	for (n=0;n!=poly->ptsz;n++) {
		poly->main_uv.uvs[n].x*=x;
		poly->main_uv.uvs[n].y*=y;
	}
}

void map_mesh_single_uv(map_type *map,int mesh_idx)
{
	int						n,npoly;
	map_mesh_type			*mesh;

	mesh=&map->mesh.meshes[mesh_idx];

	npoly=mesh->npoly;

	for (n=0;n!=npoly;n++) {
		map_mesh_poly_single_uv(map,mesh_idx,n);
	}
}

/* =======================================================

      Sort Polys Within a Mesh
      
======================================================= */

void map_mesh_sort_polys(map_type *map)
{
	int						n,k,t,sz,nmesh,npoly,
							txt_idx,poly_idx;
	bool					skip;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly,bk_poly;

	nmesh=map->mesh.nmesh;

	for (n=0;n!=nmesh;n++) {

		mesh=&map->mesh.meshes[n];
		npoly=mesh->npoly;

			// ignore meshes with
			// the same texture on all polygons

		skip=TRUE;
		txt_idx=mesh->polys[0].txt_idx;

		for (k=1;k<npoly;k++) {
			if (mesh->polys[k].txt_idx!=txt_idx) {
				skip=FALSE;
				break;
			}
		}

		if (skip) continue;

			// sort the polygons by
			// texture index

		for (k=1;k<npoly;k++) {

			poly=&mesh->polys[k];
			txt_idx=poly->txt_idx;

				// find position in mesh

			poly_idx=-1;

			for (t=0;t<k;t++) {
				if (mesh->polys[t].txt_idx>txt_idx) {
					poly_idx=t;
					break;
				}
			}

			if ((poly_idx==-1) || (poly_idx==k)) continue;

				// move poly

			sz=(k-poly_idx)*sizeof(map_mesh_poly_type);
			if (sz==0) continue;

			memmove(&bk_poly,poly,sizeof(map_mesh_poly_type));
			memmove(&mesh->polys[poly_idx+1],&mesh->polys[poly_idx],sz);
			memmove(&mesh->polys[poly_idx],&bk_poly,sizeof(map_mesh_poly_type));
		}
	}
}
