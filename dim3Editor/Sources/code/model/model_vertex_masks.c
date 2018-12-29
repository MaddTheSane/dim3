/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Vertex Select/Hide Masks

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern model_type				model;

unsigned char					*model_vertex_mask_ptr;

/* =======================================================

      Initialize and Shutdown Vertex Masks
      
======================================================= */

bool model_vertex_mask_initialize(void)
{
	int				sz;
	
	sz=model_mask_max_vertex*max_model_mesh;
	
	model_vertex_mask_ptr=(unsigned char*)malloc(sz);
	if (model_vertex_mask_ptr==NULL) return(FALSE);

	bzero(model_vertex_mask_ptr,(model_mask_max_vertex*max_model_mesh));
	return(TRUE);
}

void model_vertex_mask_shutdown(void)
{
	free(model_vertex_mask_ptr);
}

unsigned char model_vertex_mask_get(int mesh_idx,int vertex_idx)
{
	return(*(model_vertex_mask_ptr+((model_mask_max_vertex*mesh_idx)+vertex_idx)));
}

void model_vertex_mask_set(int mesh_idx,int vertex_idx,unsigned char mask)
{
	*(model_vertex_mask_ptr+((model_mask_max_vertex*mesh_idx)+vertex_idx))=mask;
}

/* =======================================================

      Select Masks
      
======================================================= */

void model_vertex_mask_clear_sel(int mesh_idx)
{
	int				n;
	unsigned char	mask;
	model_mesh_type	*mesh;
	
	mesh=&model.meshes[mesh_idx];
	
	for (n=0;n!=mesh->nvertex;n++) {
		mask=model_vertex_mask_get(mesh_idx,n);
		mask&=(model_mask_flag_sel^0xFF);
		model_vertex_mask_set(mesh_idx,n,mask);
	}
}

void model_vertex_mask_set_sel(int mesh_idx,int vertex_idx,bool value)
{
	unsigned char	mask;
	
	mask=model_vertex_mask_get(mesh_idx,vertex_idx);
	
	if (value) {
		mask|=model_mask_flag_sel;
	}
	else {
		mask&=(model_mask_flag_sel^0xFF);
	}
	
	model_vertex_mask_set(mesh_idx,vertex_idx,mask);
}

void model_vertex_mask_set_sel_all(int mesh_idx)
{
	int				n;
	unsigned char	mask;
	model_mesh_type	*mesh;
	
	mesh=&model.meshes[mesh_idx];
	
	for (n=0;n!=mesh->nvertex;n++) {
		mask=model_vertex_mask_get(mesh_idx,n);
		mask|=model_mask_flag_sel;
		model_vertex_mask_set(mesh_idx,n,mask);
	}
}

bool model_vertex_mask_check_sel_any(int mesh_idx)
{
	int				n;
	unsigned char	mask;
	model_mesh_type	*mesh;
	
	mesh=&model.meshes[mesh_idx];
	
	for (n=0;n!=mesh->nvertex;n++) {
		mask=model_vertex_mask_get(mesh_idx,n);
		if ((mask&model_mask_flag_sel)!=0x0) return(TRUE);
	}
	
	return(FALSE);
}

bool model_vertex_mask_check_sel(int mesh_idx,int vertex_idx)
{
	unsigned char	mask;
	
	mask=model_vertex_mask_get(mesh_idx,vertex_idx);
	return((mask&model_mask_flag_sel)!=0x0);
}

int model_vertex_mask_count_sel(int mesh_idx)
{
	int				n,cnt;
	unsigned char	mask;
	model_mesh_type	*mesh;
	
	mesh=&model.meshes[mesh_idx];
	
	cnt=0;

	for (n=0;n!=mesh->nvertex;n++) {
		mask=model_vertex_mask_get(mesh_idx,n);
		if ((mask&model_mask_flag_sel)!=0x0) cnt++;
	}
	
	return(cnt);
}

int model_vertex_mask_get_first_sel(int mesh_idx)
{
	int				n;
	unsigned char	mask;
	model_mesh_type	*mesh;
	
	mesh=&model.meshes[mesh_idx];

	for (n=0;n!=mesh->nvertex;n++) {
		mask=model_vertex_mask_get(mesh_idx,n);
		if ((mask&model_mask_flag_sel)!=0x0) return(n);
	}
	
	return(-1);
}

/* =======================================================

      Hide Masks
      
======================================================= */

void model_vertex_mask_clear_hide(int mesh_idx)
{
	int				n;
	unsigned char	mask;
	model_mesh_type	*mesh;
	
	mesh=&model.meshes[mesh_idx];
	
	for (n=0;n!=mesh->nvertex;n++) {
		mask=model_vertex_mask_get(mesh_idx,n);
		mask&=(model_mask_flag_hide^0xFF);
		model_vertex_mask_set(mesh_idx,n,mask);
	}
}

void model_vertex_mask_set_hide(int mesh_idx,int vertex_idx,bool value)
{
	unsigned char	mask;
	
	mask=model_vertex_mask_get(mesh_idx,vertex_idx);

	if (value) {
		mask|=model_mask_flag_hide;
	}
	else {
		mask&=(model_mask_flag_hide^0xFF);
	}
	
	model_vertex_mask_set(mesh_idx,vertex_idx,mask);
}

bool model_vertex_mask_check_hide(int mesh_idx,int vertex_idx)
{
	unsigned char	mask;

	mask=model_vertex_mask_get(mesh_idx,vertex_idx);
	return((mask&model_mask_flag_hide)!=0x0);
}

/* =======================================================

      Hide/Select Interactions
      
======================================================= */

void model_vertex_mask_hide_set_sel_vertexes(int mesh_idx)
{
	int				n,nt;
	
	nt=model.meshes[mesh_idx].nvertex;
	
	for (n=0;n!=nt;n++) {
		if (model_vertex_mask_check_sel(mesh_idx,n)) model_vertex_mask_set_hide(mesh_idx,n,TRUE);
	}
	
	model_vertex_mask_clear_sel(mesh_idx);
	model_poly_mask_clear_sel(mesh_idx);
}

void model_vertex_mask_hide_set_non_sel_vertexes(int mesh_idx)
{
	int				n,nt;
	
	nt=model.meshes[mesh_idx].nvertex;
	
	for (n=0;n!=nt;n++) {
		if (!model_vertex_mask_check_sel(mesh_idx,n)) model_vertex_mask_set_hide(mesh_idx,n,TRUE);
	}
}

void model_vertex_mask_hide_show_all_vertexes(int mesh_idx)
{
	model_vertex_mask_clear_hide(mesh_idx);
}

/* =======================================================

      Hide/Poly Interactions
      
======================================================= */

bool model_vertex_mask_check_hide_poly(int mesh_idx,model_poly_type *poly)
{
	int				n;
	
	for (n=0;n!=poly->ptsz;n++) {
		if (model_vertex_mask_check_hide(mesh_idx,poly->v[n])) return(TRUE);
	}
	
	return(FALSE);
}

/* =======================================================

      Sel/Bone Interactions
      
======================================================= */

void model_vertex_mask_set_sel_bone(int mesh_idx,int bone_idx)
{
	int					n,nt;
	model_vertex_type	*vertex;
	
		// clear selection
		
	model_vertex_mask_clear_sel(mesh_idx);
	
		// find vertexes attached to bone

	nt=model.meshes[mesh_idx].nvertex;
		
	vertex=model.meshes[mesh_idx].vertexes;
	
	for (n=0;n!=nt;n++) {
		if ((vertex->major_bone_idx==bone_idx) || (vertex->minor_bone_idx==bone_idx)) {
			if (!model_vertex_mask_check_hide(mesh_idx,n)) model_vertex_mask_set_sel(mesh_idx,n,TRUE);
		}
		vertex++;
	}
}

void model_vertex_mask_set_sel_has_bone(int mesh_idx)
{
	int					n,nt;
	model_vertex_type	*vertex;
	
		// clear selection
		
	model_vertex_mask_clear_sel(mesh_idx);
	
		// find vertexes to no bone

	nt=model.meshes[mesh_idx].nvertex;
		
	vertex=model.meshes[mesh_idx].vertexes;
	
	for (n=0;n!=nt;n++) {
		if ((vertex->major_bone_idx!=-1) || (vertex->minor_bone_idx!=-1)) {
			if (!model_vertex_mask_check_hide(mesh_idx,n)) model_vertex_mask_set_sel(mesh_idx,n,TRUE);
		}
		vertex++;
	}
}

void model_vertex_mask_set_sel_no_bone(int mesh_idx)
{
	int					n,nt;
	model_vertex_type	*vertex;
	
		// clear selection
		
	model_vertex_mask_clear_sel(mesh_idx);
	
		// find vertexes to no bone

	nt=model.meshes[mesh_idx].nvertex;
		
	vertex=model.meshes[mesh_idx].vertexes;
	
	for (n=0;n!=nt;n++) {
		if ((vertex->major_bone_idx==-1) && (vertex->minor_bone_idx==-1)) {
			if (!model_vertex_mask_check_hide(mesh_idx,n)) model_vertex_mask_set_sel(mesh_idx,n,TRUE);
		}
		vertex++;
	}
}

void model_vertex_mask_set_sel_near_bone(int mesh_idx,int bone_idx,float percentage)
{
	int					n,nt,x,y,z,v_dist,dist;
	model_bone_type		*bone;
	model_vertex_type	*vertex;
	
		// get bone to check
		
	bone=&model.bones[bone_idx];
	
		// clear selection
		
	model_vertex_mask_clear_sel(mesh_idx);

		// get check distance
		
	x=model.view_box.size.x;
	z=model.view_box.size.z;
	y=model.view_box.size.y;
	
	v_dist=(int)(sqrt((x*x)+(z*z)+(y*y)));
	v_dist=(int)((float)v_dist*percentage);
	
	nt=model.meshes[mesh_idx].nvertex;
	vertex=model.meshes[mesh_idx].vertexes;
	
	for (n=0;n!=nt;n++) {
	
		x=labs(bone->pnt.x-vertex->pnt.x);
		z=labs(bone->pnt.z-vertex->pnt.z);
		y=labs(bone->pnt.y-vertex->pnt.y);
		dist=(int)(sqrt((x*x)+(z*z)+(y*y)));
	
		if (dist<v_dist) model_vertex_mask_set_sel(mesh_idx,n,TRUE);
	
		vertex++;
	}
	
}

/* =======================================================

      Sel/Poly Interactions
      
======================================================= */

void model_vertex_mask_set_sel_poly_mask(int mesh_idx)
{
	int					n,k,npoly;
	model_mesh_type		*mesh;
	model_poly_type		*poly;

		// clear vertexes

	model_vertex_mask_clear_sel(mesh_idx);

		// set for all polys

	mesh=&model.meshes[mesh_idx];
	npoly=mesh->npoly;

		// selection
	
	for (n=0;n!=npoly;n++) {

		if ((!model_poly_mask_check_sel(mesh_idx,n)) || (model_poly_mask_check_hide(mesh_idx,n))) continue;
		
		poly=&model.meshes[mesh_idx].polys[n];
	
		for (k=0;k!=poly->ptsz;k++) {
			model_vertex_mask_set_sel(mesh_idx,poly->v[k],TRUE);
		}
	}
}

/* =======================================================

      Set Vertexes To Bone
      
======================================================= */

void model_vertex_mask_set_sel_vertex_to_bone(int mesh_idx,int major_bone_idx,int minor_bone_idx,float factor)
{
	int					i,nt;
	model_vertex_type	*vertex;
	
		// attach vertexes to bone
		
	nt=model.meshes[mesh_idx].nvertex;
	vertex=model.meshes[mesh_idx].vertexes;
	
	for (i=0;i!=nt;i++) {
	
		if (model_vertex_mask_check_sel(mesh_idx,i)) {
			vertex->major_bone_idx=major_bone_idx;
			vertex->minor_bone_idx=minor_bone_idx;
			vertex->bone_factor=factor;
		}
		
		vertex++;
	}
}

/* =======================================================

      Sel/Texture Interactions
      
======================================================= */

void model_vertex_mask_set_sel_texture(int mesh_idx,int txt_idx)
{
	int					n,k;
	model_mesh_type		*mesh;
	model_poly_type		*poly;
	
		// clear selection
		
	model_vertex_mask_clear_sel(mesh_idx);
	
		// select vertexes in polys

	mesh=&model.meshes[mesh_idx];
	poly=mesh->polys;
	
	for (n=0;n!=mesh->npoly;n++) {

		if (poly->txt_idx==txt_idx) {
			for (k=0;k!=poly->ptsz;k++) {
				if (!model_vertex_mask_check_hide(mesh_idx,poly->v[k])) model_vertex_mask_set_sel(mesh_idx,poly->v[k],TRUE);
			}
		}
		
		poly++;
	}
}

