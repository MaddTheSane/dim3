/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Model Mesh

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

/* =======================================================

      Add Mesh
      
======================================================= */

int model_mesh_add(model_type *model)
{
	int					nmesh;
	model_mesh_type		*mesh;
	
	nmesh=model->nmesh;
	if (nmesh>=max_model_mesh) return(-1);
		
		// create the new mesh
	
	mesh=&model->meshes[nmesh];
	
		// vertexes and polys
		
	mesh->nvertex=0;
	mesh->npoly=0;

	mesh->vertexes=NULL;
	mesh->polys=NULL;
	
		// setup

	strcpy(mesh->name,"New Mesh");
	mesh->no_lighting=FALSE;
	model->meshes[0].diffuse=TRUE;
	mesh->blend_add=FALSE;
	mesh->never_cull=FALSE;
	mesh->locked=FALSE;
	mesh->rt_non_light_blocking=FALSE;

	mesh->import_move.x=0;
	mesh->import_move.y=0;
	mesh->import_move.z=0;
	
	model->nmesh++;
	
	return(model->nmesh-1);
}

/* =======================================================

      Duplicate Mesh
      
======================================================= */

int model_mesh_duplicate(model_type *model,int mesh_idx)
{
	int					idx;
	model_mesh_type		*org_mesh,*mesh;
	
		// add new mesh
		
	idx=model_mesh_add(model);
	if (idx==-1) return(-1);
	
		// duplicate
	
	org_mesh=&model->meshes[mesh_idx];
	mesh=&model->meshes[idx];

	mesh->nvertex=org_mesh->nvertex;
	if (!model_mesh_set_vertex_count(model,idx,org_mesh->nvertex)) return(-1);
	memmove(mesh->vertexes,org_mesh->vertexes,(org_mesh->nvertex*sizeof(model_vertex_type)));

	mesh->npoly=org_mesh->npoly;
	if (!model_mesh_set_poly_count(model,idx,org_mesh->npoly)) return(-1);
	memmove(mesh->polys,org_mesh->polys,(org_mesh->npoly*sizeof(model_poly_type)));
	
	mesh->no_lighting=org_mesh->no_lighting;
	mesh->diffuse=org_mesh->diffuse;
	mesh->blend_add=org_mesh->blend_add;
	mesh->never_cull=org_mesh->never_cull;
	
	return(idx);
}

/* =======================================================

      Delete Mesh
      
======================================================= */

bool model_mesh_delete(model_type *model,int mesh_idx)
{
	int					sz;
	model_mesh_type		*mesh;
	
		// always need one mesh
		
	if (model->nmesh<=1) return(FALSE);
	
		// free the memory
		
	mesh=&model->meshes[mesh_idx];
		
	if (mesh->vertexes!=NULL) free(mesh->vertexes);
	if (mesh->polys!=NULL) free(mesh->polys);
	if (mesh->trans_sort.polys!=NULL) free(mesh->trans_sort.polys);
	
		// delete current mesh
				
	sz=(model->nmesh-mesh_idx)-1;
	if (sz>0) memmove(&model->meshes[mesh_idx],&model->meshes[mesh_idx+1],(sz*sizeof(model_mesh_type)));

	model->nmesh--;
	
	return(TRUE);
}

/* =======================================================

      Vertex and Poly Sizes
      
======================================================= */

bool model_mesh_set_vertex_count(model_type *model,int mesh_idx,int vertex_count)
{
	int					count;
	model_mesh_type		*mesh;
	model_vertex_type	*vtx;
	
	mesh=&model->meshes[mesh_idx];

	vtx=malloc(vertex_count*sizeof(model_vertex_type));
	if (vtx==NULL) return(FALSE);

	bzero(vtx,(vertex_count*sizeof(model_vertex_type)));
	
	if (mesh->vertexes!=NULL) {
		count=mesh->nvertex;
		if (count>vertex_count) count=vertex_count;
	
		if (count!=0) memmove(vtx,mesh->vertexes,(count*sizeof(model_vertex_type)));
	
		free(mesh->vertexes);
	}
	
	mesh->nvertex=vertex_count;
	mesh->vertexes=vtx;
	
	return(TRUE);
}

bool model_mesh_set_poly_count(model_type *model,int mesh_idx,int poly_count)
{
	int					count;
	model_mesh_type		*mesh;
	model_poly_type		*poly;
	
	mesh=&model->meshes[mesh_idx];

		// reset the poly count

	poly=malloc(poly_count*sizeof(model_poly_type));
	if (poly==NULL) return(FALSE);

	bzero(poly,(poly_count*sizeof(model_poly_type)));
	
		// if there was an old list, move that over

	if (mesh->polys!=NULL) {
		count=mesh->npoly;
		if (count>poly_count) count=poly_count;
	
		if (count!=0) memmove(poly,mesh->polys,(count*sizeof(model_poly_type)));
	
		free(mesh->polys);
	}
	
	mesh->npoly=poly_count;
	mesh->polys=poly;

		// also need to update the poly
		// sorting list for transparent polys

	if (mesh->trans_sort.polys!=NULL) free(mesh->trans_sort.polys);

	mesh->trans_sort.polys=(model_trans_sort_poly_type*)malloc(poly_count*sizeof(model_trans_sort_poly_type));
	if (mesh->trans_sort.polys==NULL) return(FALSE);
	
	return(TRUE);
}

