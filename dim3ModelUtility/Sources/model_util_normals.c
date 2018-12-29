/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Normal Routines

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
	#include "dim3modelutility.h"
#endif

#define normal_near_vertex_center_dist		250

/* =======================================================

      Calculate Normals for Model
      
======================================================= */

bool model_recalc_normals_compare_sign(float f1,float f2)
{
	if ((f1<0.0f) && (f2<0.0f)) return(TRUE);
	if ((f1>=0.0f) && (f2>=0.0f)) return(TRUE);
	return(FALSE);
}

void map_recalc_normals_get_vertex_box(model_type *model,int mesh_idx,int vertex_idx,d3pnt *min,d3pnt *max)
{
	int					n,k,txt_idx;
	int					*vertex_txt_idx;
	bool				first_hit;
	d3pnt				*pnt;
	model_mesh_type		*mesh;
	model_vertex_type	*vertex;
	model_poly_type		*poly;

	mesh=&model->meshes[mesh_idx];
	
		// get a texture index for
		// each vertex
		
	vertex_txt_idx=(int*)malloc(mesh->nvertex*sizeof(int));
	bzero(vertex_txt_idx,(mesh->nvertex*sizeof(int)));
		
	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {
		for (k=0;k!=poly->ptsz;k++) {
			vertex_txt_idx[poly->v[k]]=poly->txt_idx;
		}
		poly++;
	}
	
		// find center by all vertexes
		// with the same texture
	
	txt_idx=vertex_txt_idx[vertex_idx];
	
	first_hit=FALSE;
	min->x=min->y=min->z=0;
	max->x=max->y=max->z=0;

	vertex=mesh->vertexes;

	for (n=0;n!=mesh->nvertex;n++) {
	
		if (txt_idx==vertex_txt_idx[n]) {
	
			pnt=&mesh->vertexes[n].pnt;

			if (!first_hit) {
				min->x=max->x=pnt->x;
				min->y=max->y=pnt->y;
				min->z=max->z=pnt->z;
				first_hit=TRUE;
			}
			else {
				if (pnt->x<min->x) min->x=pnt->x;
				if (pnt->x>max->x) max->x=pnt->x;
				if (pnt->y<min->y) min->y=pnt->y;
				if (pnt->y>max->y) max->y=pnt->y;
				if (pnt->z<min->z) min->z=pnt->z;
				if (pnt->z>max->z) max->z=pnt->z;
			}
		}
	}
}

bool model_recalc_normals_determine_vector_in_out(model_type *model,int mesh_idx,int vertex_idx)
{
	int					x,y,z,k,pos_dist,neg_dist;
	float				f_dist;
	bool				is_out;
	d3pnt				*pnt,min,max,center,pos_pt,neg_pt;
	d3vct				face_vct;
	model_mesh_type		*mesh;
	model_vertex_type	*vertex;

		// get box for vertex.  This will be the combination
		// of vertexes with the same material

	map_recalc_normals_get_vertex_box(model,mesh_idx,vertex_idx,&min,&max);

		// get the box center

	mesh=&model->meshes[mesh_idx];
	vertex=&mesh->vertexes[vertex_idx];
	pnt=&vertex->pnt;

	center.x=(min.x+max.x)>>1;
	center.y=(min.y+max.y)>>1;
	center.z=(min.z+max.z)>>1;

		// the dot product is the fall back position
		// if these specialized checks fail

	vector_create(&face_vct,pnt->x,pnt->y,pnt->z,center.x,center.y,center.z);
	is_out=(vector_dot_product(&vertex->tangent_space.normal,&face_vct)>0.0f);

		// get a point from the current normal vector
		// and inverse of the current normal vector, using 10%
		// of the distance to center
	
	f_dist=(float)distance_get(pnt->x,pnt->y,pnt->z,center.x,center.y,center.z);
	f_dist*=0.1f;

	pos_pt.x=pnt->x+(int)(vertex->tangent_space.normal.x*f_dist);
	pos_pt.y=pnt->y+(int)(vertex->tangent_space.normal.y*f_dist);
	pos_pt.z=pnt->z+(int)(vertex->tangent_space.normal.z*f_dist);

	neg_pt.x=pnt->x-(int)(vertex->tangent_space.normal.x*f_dist);
	neg_pt.y=pnt->y-(int)(vertex->tangent_space.normal.y*f_dist);
	neg_pt.z=pnt->z-(int)(vertex->tangent_space.normal.z*f_dist);

		// first we determine if we can think of the
		// poly's box (which is determined by all connected
		// polys) as a closed object in one direction

		// if one direction is at least 25% greater than the others
		// then consider it a tube like structure

		// if any distance calcs fail, fall back to dot product

	x=max.x-min.x;
	y=max.y-min.y;
	z=max.z-min.z;

	k=x-((x*25)/100);
	if ((x>y) && (x>z)) {
		pos_dist=distance_2D_get(pos_pt.y,pos_pt.z,center.y,center.z);
		neg_dist=distance_2D_get(neg_pt.y,neg_pt.z,center.y,center.z);
		if (pos_dist==neg_dist) return(is_out);

		return(pos_dist>neg_dist);
	}

	k=y-((y*25)/100);
	if ((y>x) && (y>z)) {
		pos_dist=distance_2D_get(pos_pt.x,pos_pt.z,center.x,center.z);
		neg_dist=distance_2D_get(neg_pt.x,neg_pt.z,center.x,center.z);
		if (pos_dist==neg_dist) return(is_out);

		return(pos_dist>neg_dist);
	}

	k=z-((z*25)/100);
	if ((z>x) && (z>y)) {
		pos_dist=distance_2D_get(pos_pt.x,pos_pt.y,center.x,center.y);
		neg_dist=distance_2D_get(neg_pt.x,neg_pt.y,center.x,center.y);
		if (pos_dist==neg_dist) return(is_out);

		return(pos_dist>neg_dist);
	}

		// finally fall back to dot product

	return(is_out);
}

void model_recalc_normals_mesh(model_type *model,int mesh_idx,bool only_tangent)
{
	int					n,k,t,neg_v;
    float				u10,u20,v10,v20,f_denom;
	bool				vertex_in_poly;
	d3vct				p10,p20,vlft,vrgt,v_num;
	d3vct				*normals,*nptr,*tangents,*tptr;
	d3pnt				*pt,*pt_1,*pt_2;
	model_mesh_type		*mesh;
	model_vertex_type	*vertex;
	model_poly_type		*poly;
	
	mesh=&model->meshes[mesh_idx];
	if ((mesh->nvertex==0) || (mesh->npoly==0)) return;

		// memory for tangent space

	normals=(d3vct*)malloc(mesh->npoly*sizeof(d3vct));
	if (normals==NULL) return;

	tangents=(d3vct*)malloc(mesh->npoly*sizeof(d3vct));
	if (tangents==NULL) {
		free(normals);
		return;
	}
	
        // find tangent and binormal for triangles
		
	poly=mesh->polys;

	nptr=normals;
	tptr=tangents;
	
	for (n=0;n!=mesh->npoly;n++) {

			// treat all polys as triangles

		neg_v=poly->ptsz-1;
    
			// get the side vectors (p1-p0) and (p2-p0)

		pt=&mesh->vertexes[poly->v[0]].pnt;
		pt_1=&mesh->vertexes[poly->v[1]].pnt;
		pt_2=&mesh->vertexes[poly->v[neg_v]].pnt;

		vector_create(&p10,pt_1->x,pt_1->y,pt_1->z,pt->x,pt->y,pt->z);
		vector_create(&p20,pt_2->x,pt_2->y,pt_2->z,pt->x,pt->y,pt->z);
		
			// calculate the normal by the cross

		vector_cross_product(nptr,&p10,&p20);
		nptr++;

			// get the UV scalars (u1-u0), (u2-u0), (v1-v0), (v2-v0)

		u10=poly->gx[1]-poly->gx[0];
		u20=poly->gx[neg_v]-poly->gx[0];
		v10=poly->gy[1]-poly->gy[0];
		v20=poly->gy[neg_v]-poly->gy[0];

			// calculate the tangent
			// (v20xp10)-(v10xp20) / (u10*v20)-(v10*u20)

		vector_scalar_multiply(&vlft,&p10,v20);
		vector_scalar_multiply(&vrgt,&p20,v10);
		vector_subtract(&v_num,&vlft,&vrgt);

		f_denom=(u10*v20)-(v10*u20);
		if (f_denom!=0.0f) f_denom=1.0f/f_denom;
		vector_scalar_multiply(tptr,&v_num,f_denom);

		vector_normalize(tptr);
		tptr++;
			
		poly++;
	}
    
		// average tangent space for each
		// poly vertex to find one for shared
		// vertexes
		
	vertex=mesh->vertexes;
		
	for (n=0;n!=mesh->nvertex;n++) {
	
		vertex->tangent_space.tangent.x=vertex->tangent_space.tangent.y=vertex->tangent_space.tangent.z=0.0f;
		vertex->tangent_space.normal.x=vertex->tangent_space.normal.y=vertex->tangent_space.normal.z=0.0f;
		
		poly=mesh->polys;
		
		nptr=normals;
		tptr=tangents;
		
		for (k=0;k!=mesh->npoly;k++) {
		
			vertex_in_poly=FALSE;
			
			for (t=0;t!=poly->ptsz;t++) {
				if (poly->v[t]==n) {
					vertex_in_poly=TRUE;
					break;
				}
			}
			
			if (vertex_in_poly) {
				vertex->tangent_space.tangent.x+=tptr->x;
				vertex->tangent_space.tangent.y+=tptr->y;
				vertex->tangent_space.tangent.z+=tptr->z;
				
				vertex->tangent_space.normal.x+=nptr->x;
				vertex->tangent_space.normal.y+=nptr->y;
				vertex->tangent_space.normal.z+=nptr->z;
			}
			
			poly++;
			nptr++;
			tptr++;
		}

		vector_normalize(&vertex->tangent_space.tangent);
		if (!only_tangent) vector_normalize(&vertex->tangent_space.normal);

		vertex++;
	}
		
		// free the tangent spaces

	free(normals);
	free(tangents);
	
		// fix any normals that are 0,0,0
		// this usually happens when there are bad
		// polys
		
	for (k=0;k!=mesh->nvertex;k++) {
		
		if ((vertex->tangent_space.normal.x==0.0f) && (vertex->tangent_space.normal.y==0.0f) && (vertex->tangent_space.normal.z==0.0f)) {
			pt=&mesh->vertexes[k].pnt;
			
			vertex->tangent_space.normal.x=(float)(pt->x-vertex->pnt.x);
			vertex->tangent_space.normal.y=(float)(pt->y-vertex->pnt.y);
			vertex->tangent_space.normal.z=(float)(pt->z-vertex->pnt.z);
			vector_normalize(&vertex->tangent_space.normal);
		}
	}

		// determine in-out to map flips
		// skip if not calculating normals
		
	if (only_tangent) return;

		// determine in/out and invert
		
	vertex=mesh->vertexes;

	for (n=0;n!=mesh->nvertex;n++) {
	
		if (!model_recalc_normals_determine_vector_in_out(model,mesh_idx,n)) {
			vertex->tangent_space.normal.x=-vertex->tangent_space.normal.x;
			vertex->tangent_space.normal.y=-vertex->tangent_space.normal.y;
			vertex->tangent_space.normal.z=-vertex->tangent_space.normal.z;
		}
		
		vertex++;
	}
}

void model_recalc_normals(model_type *model,bool only_tangent)
{
	int				n;

	for (n=0;n!=model->nmesh;n++) {
		model_recalc_normals_mesh(model,n,only_tangent);
	}
}


