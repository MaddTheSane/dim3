/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Drawing Setup Routines

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

      Draw Setup Memory
      
======================================================= */

bool model_draw_setup_initialize(model_type *model,model_draw_setup *draw_setup,bool tangent_space)
{
	int				n,sz;
	model_mesh_type	*mesh;
		
		// clear per-mesh arrays

	for (n=0;n!=model->nmesh;n++) {
		draw_setup->mesh_arrays[n].gl_vertex_array=NULL;
		draw_setup->mesh_arrays[n].gl_tangent_array=NULL;
		draw_setup->mesh_arrays[n].gl_normal_array=NULL;
		draw_setup->mesh_arrays[n].poly_cull_array=NULL;
	}
	
		// setup per-mesh arrays

	mesh=model->meshes;
	
	for (n=0;n!=model->nmesh;n++) {

		sz=(mesh->nvertex*3)*sizeof(float);

		draw_setup->mesh_arrays[n].gl_vertex_array=(float*)malloc(sz);
		if (draw_setup->mesh_arrays[n].gl_vertex_array==NULL) return(FALSE);
		bzero(draw_setup->mesh_arrays[n].gl_vertex_array,sz);
		
		if (tangent_space) {
			draw_setup->mesh_arrays[n].gl_tangent_array=(float*)malloc(sz);
			if (draw_setup->mesh_arrays[n].gl_tangent_array==NULL) return(FALSE);
			bzero(draw_setup->mesh_arrays[n].gl_tangent_array,sz);

			draw_setup->mesh_arrays[n].gl_normal_array=(float*)malloc(sz);
			if (draw_setup->mesh_arrays[n].gl_normal_array==NULL) return(FALSE);
			bzero(draw_setup->mesh_arrays[n].gl_normal_array,sz);
		}

		draw_setup->mesh_arrays[n].poly_cull_array=(unsigned char*)malloc(mesh->npoly);
		if (draw_setup->mesh_arrays[n].poly_cull_array==NULL) return(FALSE);
		bzero(draw_setup->mesh_arrays[n].poly_cull_array,mesh->npoly);
		
		mesh++;
	}
	
	return(TRUE);
}

void model_draw_setup_shutdown(model_type *model,model_draw_setup *draw_setup)
{
	int				n;
	
		// free per mesh arrays

	for (n=0;n!=model->nmesh;n++) {
		if (draw_setup->mesh_arrays[n].gl_vertex_array!=NULL) free(draw_setup->mesh_arrays[n].gl_vertex_array);
		if (draw_setup->mesh_arrays[n].gl_tangent_array!=NULL) free(draw_setup->mesh_arrays[n].gl_tangent_array);
		if (draw_setup->mesh_arrays[n].gl_normal_array!=NULL) free(draw_setup->mesh_arrays[n].gl_normal_array);
		if (draw_setup->mesh_arrays[n].poly_cull_array!=NULL) free(draw_setup->mesh_arrays[n].poly_cull_array);
	}
}

/* =======================================================

      Clear Draw Setup
      
======================================================= */

void model_draw_setup_clear(model_type *model,model_draw_setup *draw_setup)
{
	int							n;
	model_draw_alter_bone_type	*alter_bone;

		// enhancements at 1.0f

	draw_setup->enhance_factor=1.0f;
	
		// turn off all blended animations
		
	for (n=0;n!=max_model_blend_animation;n++) {
		draw_setup->poses[n].idx_1=draw_setup->poses[n].idx_2=-1;
		draw_setup->poses[n].factor=1.0f;
		draw_setup->poses[n].acceleration=0.0f;
	}
	
		// setup centering
		
	if (model!=NULL) {
		draw_setup->center.x=model->center.x;
		draw_setup->center.y=model->center.y;
		draw_setup->center.z=model->center.z;
	}
	else {
		draw_setup->center.x=draw_setup->center.y=draw_setup->center.z=0;
	}
	
		// no full model sways, moves, turns
		
	draw_setup->move.x=draw_setup->move.y=draw_setup->move.z=0;
	draw_setup->ang.x=draw_setup->ang.y=draw_setup->ang.z=0;
	draw_setup->sway.x=draw_setup->sway.y=draw_setup->sway.z=0;
	
		// turn off any bone alters
		
	alter_bone=draw_setup->alter_bones;
	
	for (n=0;n!=model->nbone;n++) {
		alter_bone->resize=1.0f;
		alter_bone->parent_dist_add.x=alter_bone->parent_dist_add.y=alter_bone->parent_dist_add.z=0.0f;
		alter_bone->rot_add.x=alter_bone->rot_add.y=alter_bone->rot_add.z=0.0f;
		alter_bone++;
	}
}

