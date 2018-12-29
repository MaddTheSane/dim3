/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Import and Insert

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"

model_type						ins_model;

extern model_type				model;
extern file_path_setup_type		file_path_setup;
extern app_state_type			state;

/* =======================================================

      Insert One Model Mesh Into Another
      
======================================================= */

void model_insert_mesh(char *file_name)
{
	int						n,k,t,b_off,v_off,t_off,nvertex,npoly,idx,
							parent_indexes[max_model_bone];
	char					path[1024],path2[1024],sub_path[1024];
	model_bone_type			*bone,*ins_bone;
	model_mesh_type			*mesh,*ins_mesh;
	model_vertex_type		*vertex;
	model_poly_type			*poly;
	texture_type			*texture,*ins_texture;
	
		// open model

	model_open(&ins_model,file_name,TRUE);
	
		// bring in the bones

	b_off=model.nbone;
	ins_bone=ins_model.bones;
	
	for (n=0;n!=ins_model.nbone;n++) {
		idx=model_bone_add(&model,0,0,0);
		if (idx==-1) break;
		
		bone=&model.bones[idx];
		
		memmove(bone,ins_bone,sizeof(model_bone_type));
		bone->parent_dist.x=bone->parent_dist.y=bone->parent_dist.z=0.0f;

			// remember the bone to fix the indexes

		parent_indexes[idx]=-1;
		if (bone->parent_idx!=-1) parent_indexes[idx]=bone->parent_idx+b_off;
		bone->parent_idx=-1;
	
			// fix duplicate names
			
		for (k=0;k!=b_off;k++) {
			if (strcasecmp(model.bones[k].name,bone->name)==0) {
				model_bone_create_name(&model,idx);
			}
		}
	
		ins_bone++;
	}

		// fix bone parenting

	for (n=b_off;n<model.nbone;n++) {
		model.bones[n].parent_idx=parent_indexes[n];
	}

		// model mesh

	mesh=&model.meshes[state.model.cur_mesh_idx];
	ins_mesh=&ins_model.meshes[0];
	
		// bring in the vertexes
		
	v_off=mesh->nvertex;
	nvertex=ins_mesh->nvertex;
	
	model_mesh_set_vertex_count(&model,state.model.cur_mesh_idx,(v_off+nvertex));
	
	vertex=&mesh->vertexes[v_off];
	memmove(vertex,ins_mesh->vertexes,(nvertex*sizeof(model_vertex_type)));
	
	for (n=0;n!=nvertex;n++) {
		if (vertex->major_bone_idx!=-1) vertex->major_bone_idx+=b_off;
		if (vertex->minor_bone_idx!=-1) vertex->minor_bone_idx+=b_off;
		vertex++;
	}

		// bring in the polys
		
	t_off=mesh->npoly;
	npoly=ins_mesh->npoly;
	
	model_mesh_set_poly_count(&model,state.model.cur_mesh_idx,(t_off+npoly));
	
	poly=&mesh->polys[t_off];
	memmove(poly,ins_mesh->polys,(npoly*sizeof(model_poly_type)));
	
	for (n=t_off;n!=mesh->npoly;n++) {
		for (k=0;k!=poly->ptsz;k++) {
			poly->v[k]+=v_off;
		}
		poly++;
	}

		// bring in the textures

	for (n=0;n!=max_model_texture;n++) {
		ins_texture=&ins_model.textures[n];
	
		if (ins_texture->frames[0].bitmap.gl_id==-1) continue;
	
			// find next blank texture
			
		k=0;
		while (k<max_model_texture) {
			if (model.textures[k].frames[0].bitmap.gl_id==-1) break;
			k++;
		}
		
		if (k>=max_model_texture) break;
		
			// copy texture

		texture=&model.textures[k];
		memmove(texture,ins_texture,sizeof(texture_type));
		
			// need to change names so they
			// don't conflict with what's already there

		for (t=0;t!=max_texture_frame;t++) {
			if (texture->frames[t].bitmap.gl_id!=-1) {
				sprintf(texture->frames[t].name,"Image_%d_%d",k,t);
			}
		}

			// fix the imported triangles texture index

		poly=&mesh->polys[t_off];

		for (t=t_off;t!=mesh->npoly;t++) {
			if (poly->txt_idx==n) poly->txt_idx=k;
			poly++;
		}
		
			// force bitmap to not close when closing insert model
					
		for (t=0;t!=max_texture_frame;t++) {
			ins_texture->frames[t].bitmap.gl_id=-1;
		}

			// copy over bitmap

		for (t=0;t!=max_texture_frame;t++) {
			if (texture->frames[t].bitmap.gl_id!=-1) {
				sprintf(sub_path,"Models/%s/Textures",file_name);
				file_paths_data_default(&file_path_setup,path,sub_path,ins_texture->frames[t].name,"png");	

				sprintf(sub_path,"Models/%s/Textures",model.name);
				file_paths_data_default(&file_path_setup,path2,sub_path,texture->frames[t].name,"png");

				bitmap_copy(path,path2);
			}
		}
	}

		// close model
		
	model_close(&ins_model);
}

/* =======================================================

      Insert One Model Animations Into Another
      
======================================================= */

void model_insert_animations(char *file_name)
{
		// open model

	model_open(&ins_model,file_name,TRUE);
	
		// bring in the bones, poses, animations
		
	model.nbone=ins_model.nbone;
	if (model.bones!=NULL) free(model.bones);
	model.bones=ins_model.bones;
	ins_model.bones=NULL;

	model.npose=ins_model.npose;
	if (model.poses!=NULL) free(model.poses);
	model.poses=ins_model.poses;
	ins_model.poses=NULL;
	
	model.nanimate=ins_model.nanimate;
	if (model.animates!=NULL) free(model.animates);
	model.animates=ins_model.animates;
	ins_model.animates=NULL;
	
		// close model
		
	model_close(&ins_model);
}


