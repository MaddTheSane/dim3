/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Model Auto Generation Main Line

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

#define ag_model_cylinder_side_count		8
#define ag_model_limb_radius_start			80
#define ag_model_limb_radius_extra			80

#define ag_model_hip_high_start				250
#define ag_model_hip_high_extra				100

/* =======================================================

      Clear Model
      
======================================================= */

void ag_model_clear(model_type *model)
{
	int				n;

		// clear first mesh

	if (model->nmesh==0) {
		model_mesh_add(model);
	}
	else {
		model_mesh_set_vertex_count(model,0,0);
		model_mesh_set_poly_count(model,0,0);
	}

		// clear bones

	/* supergumba -- we need to keep bones and animations
	 in a specific order for now as we are using existing animations, so
	 this code is a temporary hack, replace with the better code below
	 later
 
	while (model->nbone!=0) {
		model_bone_delete(model,0);
	}
	*/

	free(model->bones);
	model->bones=NULL;
	model->nbone=0;

		// clear bone connections

	model->bone_connect.name_bone_idx=-1;

	for (n=0;n!=max_model_light;n++) {
		model->bone_connect.light_bone_idx[n]=-1;
	}

	for (n=0;n!=max_model_halo;n++) {
		model->bone_connect.halo_bone_idx[n]=-1;
	}
}

/* =======================================================

      Remove Stray Vertexes
      
======================================================= */

void ag_model_remove_stray_vertexes(model_type *model)
{
	int					n,k,i,t,nvertex,npoly,sz;
	unsigned char		*v_ok;
    model_poly_type		*poly;
	
		// vertex hit list
			
	nvertex=model->meshes[0].nvertex;
	
	v_ok=(unsigned char*)malloc(nvertex);
	bzero(v_ok,nvertex);
	
		// find vertexes hit
		
	npoly=model->meshes[0].npoly;
	poly=model->meshes[0].polys;
	
	for (n=0;n!=npoly;n++) {
		for (k=0;k!=poly->ptsz;k++) {
			v_ok[poly->v[k]]=0x1;
		}
		poly++;
	}
	
		// delete unused vertexes
	
	for (n=(nvertex-1);n>=0;n--) {
	
		if (v_ok[n]==0x1) continue;
		
			// change all poly vertex pointers
	
		poly=model->meshes[0].polys;
		
		for (i=0;i!=npoly;i++) {
			for (t=0;t!=poly->ptsz;t++) {
				if (poly->v[t]>n) poly->v[t]--;
			}
			poly++;
		}
		
			// delete vertex
			
		sz=(nvertex-n)*sizeof(model_vertex_type);
		if (sz>0) memmove(&model->meshes[0].vertexes[n],&model->meshes[0].vertexes[n+1],sz);
		
		nvertex--;
	}
	
	model->meshes[0].nvertex=nvertex;

	free(v_ok);
}

/* =======================================================

      Limbs
      
======================================================= */

void ag_model_limb_bone(model_type *model,model_mesh_type *mesh,int bot_bone_idx,int limb_radius,bool secondary_texture)
{
	int				top_bone_idx,stack_count;
	float			gx_offset,gy_offset;
	d3pnt			spt,ept,radius,radius_extra;
	model_bone_type	*top_bone,*bot_bone;

	bot_bone=&model->bones[bot_bone_idx];

	top_bone_idx=bot_bone->parent_idx;
	top_bone=&model->bones[top_bone_idx];

		// size

	memmove(&spt,&top_bone->pnt,sizeof(d3pnt));
	memmove(&ept,&bot_bone->pnt,sizeof(d3pnt));

	radius.x=radius.z=limb_radius;
	radius_extra.x=radius_extra.y=radius_extra.z=0;

		// how many stacks?

	stack_count=2+ag_random_int(3);

		// create the limb

	gx_offset=gy_offset=(secondary_texture?0.5f:0.0f);

	ag_model_piece_complex_cylinder(model,mesh,top_bone_idx,bot_bone_idx,&spt,&ept,stack_count,&radius,&radius_extra,gx_offset,gy_offset,0.0f,0.0f,TRUE,TRUE);
}

/* =======================================================

      Build Primitives around bones
      
======================================================= */

void ag_model_build_around_bones(model_type *model)
{
	int				n,
					body_bone_idx,hip_bone_idx,
					limb_radius;
	model_mesh_type	*mesh;

		// build into first mesh

	mesh=&model->meshes[0];

		// random limb radius

	limb_radius=ag_model_limb_radius_start+ag_random_int(ag_model_limb_radius_extra);

		// find body and hips

	body_bone_idx=-1;
	hip_bone_idx=-1;

	for (n=0;n!=model->nbone;n++) {
		if (ag_model_bone_is_body(model,n)) body_bone_idx=n;
		if (ag_model_bone_is_hip(model,n)) hip_bone_idx=n;
	}

		// build body bone

	ag_model_piece_bone_body(model,mesh,body_bone_idx,hip_bone_idx,limb_radius);

		// build pieces

	ag_model_piece_setup();

	for (n=0;n!=model->nbone;n++) {
		if (ag_model_bone_is_arm(model,n)) {
			ag_model_limb_bone(model,mesh,n,limb_radius,FALSE);
			continue;
		}
		if (ag_model_bone_is_hand(model,n)) {
			ag_model_piece_bone_hand(model,mesh,n,limb_radius);
			continue;
		}
		if (ag_model_bone_is_leg(model,n)) {
			ag_model_limb_bone(model,mesh,n,limb_radius,TRUE);
			continue;
		}
		if (ag_model_bone_is_foot(model,n)) {
			ag_model_piece_bone_foot(model,mesh,n,limb_radius);
			continue;
		}
		if (ag_model_bone_is_head(model,n)) {

				// neck

			ag_model_limb_bone(model,mesh,n,limb_radius,FALSE);

				// head

			ag_model_piece_bone_head(model,mesh,n,limb_radius);
			continue;
		}
	}
}

/* =======================================================

      Randomize Vertexes
      
======================================================= */

void ag_model_collapse_similar_vertexes(model_type *model)
{
	int					n,k,t;
	model_mesh_type		*mesh;
	model_vertex_type	*vertex,*chk_vertex;
	model_poly_type		*poly;

	mesh=&model->meshes[0];

	vertex=mesh->vertexes;

	for (n=0;n!=mesh->nvertex;n++) {

		poly=mesh->polys;
			
		for (k=0;k!=mesh->npoly;k++) {
		
			for (t=0;t!=poly->ptsz;t++) {
				if (poly->v[t]==n) continue;

				chk_vertex=&mesh->vertexes[poly->v[t]];
				if ((chk_vertex->pnt.x==vertex->pnt.x) && (chk_vertex->pnt.y==vertex->pnt.y) && (chk_vertex->pnt.z==vertex->pnt.z)) poly->v[t]=n;
			}

			poly++;
		}

		vertex++;
	}
}

void ag_model_randomize_vertexes(model_type *model)
{
	int					n;
	model_mesh_type		*mesh;
	model_vertex_type	*vertex;

	mesh=&model->meshes[0];
	vertex=mesh->vertexes;

	for (n=0;n!=mesh->nvertex;n++) {
		vertex->pnt.x+=(ag_random_int(100)-200);
		vertex->pnt.z+=(ag_random_int(100)-200);
		vertex++;
	}
}

/* =======================================================

      Generate Textures
      
======================================================= */

texture_type* ag_model_create_texture(model_type *model,int idx,char *name,bool has_glow)
{
	int					n;
	texture_type		*texture;

		// close old texture

	model_textures_close_texture(model,idx);

		// start new texture

	texture=&model->textures[idx];
	
	sprintf(texture->material_name,name);
	texture->shader_name[0]=0x0;
	texture->additive=FALSE;
	texture->compress=FALSE;
	
	if (has_glow) {
		texture->glow.rate=2000;
		texture->glow.min=0.1f;
		texture->glow.max=0.3f;
	}

	for (n=0;n!=max_texture_frame;n++) {
		texture->frames[n].name[0]=0x0;
	}
	
	strcpy(texture->frames[0].name,texture->material_name);

	return(texture);
}

void auto_generate_model_create_texture_set(model_type *model,char *base_path)
{
	texture_type		*texture;

	ag_random_seed();

	texture=ag_model_create_texture(model,ag_texture_wall,"Skin",FALSE);
	bitmap_ag_texture_skin(&texture->frames[0],base_path,512);
	model_textures_read_texture(model,0);
}

/* =======================================================

      Generate Model
      
======================================================= */

void auto_generate_model_monster(model_type *model)
{
		// start with a random seed
		// and clear the current model

	ag_random_seed();
	ag_model_clear(model);

		// build the skeleton

	ag_model_bone_create_skeleton(model);

		// build model around bones

	ag_model_build_around_bones(model);
	ag_model_add_decorations(model);		// supergumba -- testing

		// remove any stray vertexes

	ag_model_collapse_similar_vertexes(model);
	ag_model_remove_stray_vertexes(model);

		// randomize vertexes

	ag_model_randomize_vertexes(model);

		// rebuild tangents
		// and animation settings

	model_recalc_normals(model,TRUE);
	model_calculate_parents(model);
}
