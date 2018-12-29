/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Draw Normal Routines

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

      Position Model Normals
      
======================================================= */

void model_move_single_tangent_space_vector(model_draw_bone_type *draw_bones,model_vertex_type *vertex,d3vct *v)
{
	float					bone_factor,
							majx,majy,majz,minx,miny,minz;
	model_draw_bone_type	*major_bone,*minor_bone;

		// find major bone rotation

	major_bone=&draw_bones[vertex->major_bone_idx];
	
	majx=(v->x*major_bone->rot_mat.data[0][0])+(v->y*major_bone->rot_mat.data[0][1])+(v->z*major_bone->rot_mat.data[0][2]);
	majy=(v->x*major_bone->rot_mat.data[1][0])+(v->y*major_bone->rot_mat.data[1][1])+(v->z*major_bone->rot_mat.data[1][2]);
	majz=(v->x*major_bone->rot_mat.data[2][0])+(v->y*major_bone->rot_mat.data[2][1])+(v->z*major_bone->rot_mat.data[2][2]);

		// find minor bone
		
	bone_factor=vertex->bone_factor;
		
	if ((vertex->minor_bone_idx==-1) || (bone_factor==1)) {
		v->x=majx;
		v->y=majy;
		v->z=majz;
		return;
	}
		
		// find minor bone rotation
		
	minor_bone=&draw_bones[vertex->minor_bone_idx];
	
	minx=(v->x*minor_bone->rot_mat.data[0][0])+(v->y*minor_bone->rot_mat.data[0][1])+(v->z*minor_bone->rot_mat.data[0][2]);
	miny=(v->x*minor_bone->rot_mat.data[1][0])+(v->y*minor_bone->rot_mat.data[1][1])+(v->z*minor_bone->rot_mat.data[1][2]);
	minz=(v->x*minor_bone->rot_mat.data[2][0])+(v->y*minor_bone->rot_mat.data[2][1])+(v->z*minor_bone->rot_mat.data[2][2]);

		// average the points
		
	v->x=minx+((majx-minx)*bone_factor);
	v->y=miny+((majy-miny)*bone_factor);
	v->z=minz+((majz-minz)*bone_factor);
}

/* =======================================================

      Build Normal Lists
      
======================================================= */

void model_create_draw_normals(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int						n,nvertex;
	float					*pt,*pn;
	bool					no_sway;
	model_mesh_type			*mesh;
	model_vertex_type		*vertex;
	matrix_type				rot_x_mat,rot_z_mat,rot_y_mat,sway_mat;
	tangent_space_type		space;
	
	mesh=&model->meshes[mesh_idx];
	
			// rotations
		
	matrix_rotate_x(&rot_x_mat,draw_setup->ang.x);
	matrix_rotate_z(&rot_z_mat,draw_setup->ang.z);
	matrix_rotate_y(&rot_y_mat,draw_setup->ang.y);
		
		// setup list
		
	nvertex=mesh->nvertex;
	vertex=mesh->vertexes;

	pt=draw_setup->mesh_arrays[mesh_idx].gl_tangent_array;
	pn=draw_setup->mesh_arrays[mesh_idx].gl_normal_array;

		// normals with sways

	no_sway=((draw_setup->sway.x==0) && (draw_setup->sway.y==0) && (draw_setup->sway.z==0));

	if (!no_sway) {
	
		matrix_rotate_zyx(&sway_mat,draw_setup->sway.x,draw_setup->sway.y,draw_setup->sway.z);
		
		for (n=0;n!=nvertex;n++) {

				// run the tangent
			
			space.tangent.x=vertex->tangent_space.tangent.x;
			space.tangent.y=vertex->tangent_space.tangent.y;
			space.tangent.z=vertex->tangent_space.tangent.z;

			if (vertex->major_bone_idx!=-1) model_move_single_tangent_space_vector(draw_setup->bones,vertex,&space.tangent);
			
			matrix_vertex_2_multiply_ignore_transform(&sway_mat,&space.tangent);
			
			matrix_vertex_2_multiply_ignore_transform(&rot_x_mat,&space.tangent);
			matrix_vertex_2_multiply_ignore_transform(&rot_z_mat,&space.tangent);
			matrix_vertex_2_multiply_ignore_transform(&rot_y_mat,&space.tangent);

			*pt++=space.tangent.x;
			*pt++=space.tangent.y;
			*pt++=space.tangent.z;
			
				// run the normal
			
			space.normal.x=vertex->tangent_space.normal.x;
			space.normal.y=vertex->tangent_space.normal.y;
			space.normal.z=vertex->tangent_space.normal.z;

			if (vertex->major_bone_idx!=-1) model_move_single_tangent_space_vector(draw_setup->bones,vertex,&space.normal);

			matrix_vertex_2_multiply_ignore_transform(&sway_mat,&space.normal);
			
			matrix_vertex_2_multiply_ignore_transform(&rot_x_mat,&space.normal);
			matrix_vertex_2_multiply_ignore_transform(&rot_z_mat,&space.normal);
			matrix_vertex_2_multiply_ignore_transform(&rot_y_mat,&space.normal);
			
			*pn++=space.normal.x;
			*pn++=space.normal.y;
			*pn++=space.normal.z;
			
			vertex++;
		}
		
		return;
	}

		// normals with no sways
		
	for (n=0;n!=nvertex;n++) {

			// run the tangent

		space.tangent.x=vertex->tangent_space.tangent.x;
		space.tangent.y=vertex->tangent_space.tangent.y;
		space.tangent.z=vertex->tangent_space.tangent.z;

		if (vertex->major_bone_idx!=-1) model_move_single_tangent_space_vector(draw_setup->bones,vertex,&space.tangent);
		
		matrix_vertex_2_multiply_ignore_transform(&rot_x_mat,&space.tangent);
		matrix_vertex_2_multiply_ignore_transform(&rot_z_mat,&space.tangent);
		matrix_vertex_2_multiply_ignore_transform(&rot_y_mat,&space.tangent);

		*pt++=space.tangent.x;
		*pt++=space.tangent.y;
		*pt++=space.tangent.z;
		
			// run the normal

		space.normal.x=vertex->tangent_space.normal.x;
		space.normal.y=vertex->tangent_space.normal.y;
		space.normal.z=vertex->tangent_space.normal.z;

		if (vertex->major_bone_idx!=-1) model_move_single_tangent_space_vector(draw_setup->bones,vertex,&space.normal);
		
		matrix_vertex_2_multiply_ignore_transform(&rot_x_mat,&space.normal);
		matrix_vertex_2_multiply_ignore_transform(&rot_z_mat,&space.normal);
		matrix_vertex_2_multiply_ignore_transform(&rot_y_mat,&space.normal);
		
		*pn++=space.normal.x;
		*pn++=space.normal.y;
		*pn++=space.normal.z;

		vertex++;
	}
}

void model_flip_draw_normals(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int						n;
	float					*pt,*pn;
	model_mesh_type			*mesh;
	
	mesh=&model->meshes[mesh_idx];
	
		// flip normals

	pn=draw_setup->mesh_arrays[mesh_idx].gl_normal_array;

	for (n=0;n!=mesh->nvertex;n++) {
		*pn=-(*pn);
		pn+=3;
	}
	
		// flip tangents
		
	pt=draw_setup->mesh_arrays[mesh_idx].gl_tangent_array;
	
	for (n=0;n!=mesh->nvertex;n++) {
		*pt=-(*pt);
		pt+=3;
	}
}

