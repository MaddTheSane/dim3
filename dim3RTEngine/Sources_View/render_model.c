/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Model Drawing

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
	#include "dim3engine.h"
#endif

#include "interface.h"

extern map_type				map;
extern server_type			server;
extern iface_type			iface;
extern view_type			view;
extern setup_type			setup;

/* =======================================================

      Texture and Alpha Utilities
      
======================================================= */

inline int render_model_get_texture_frame(model_type *mdl,model_draw *draw,int txt_idx)
{
	texture_type		*texture;

		// regular texture animation
		
	texture=&mdl->textures[txt_idx];
	if (texture->animate.on) return(texture->animate.current_frame);
	
		// if script based animation is on, replace
		// frame
		
	if (draw->textures[txt_idx].animation_on) draw->textures[txt_idx].frame=bitmap_texture_get_current_frame(texture,draw->textures[txt_idx].animation_reverse,game_time_get());
	return(draw->textures[txt_idx].frame);
}

inline float render_model_get_mesh_alpha(model_draw *draw,int mesh_idx)
{
	if (!draw->meshes[mesh_idx].fade.on) return(draw->alpha);
	return(draw->meshes[mesh_idx].fade.alpha);
}

/* =======================================================

      Model Normals
      
======================================================= */

void render_model_create_normal_vertexes(model_type *mdl,int mesh_mask,model_draw *draw)
{
	int				n;

	for (n=0;n!=mdl->nmesh;n++) {
		if ((mesh_mask&(0x1<<n))==0) continue;
		model_create_draw_normals(mdl,n,&draw->setup);
		if (draw->flip_x) model_flip_draw_normals(mdl,n,&draw->setup);
	}
}

/* =======================================================

      Setup Model and Vertex Lists
      
======================================================= */

void render_model_setup(model_draw *draw,int tick)
{
	int					n,k,frame,txt_idx;
	bool				texture_hits[max_model_texture];
	model_type			*mdl;
	model_mesh_type		*mesh;
	model_poly_type		*poly;
    texture_type		*texture;
	
		// get model
	
	if ((draw->model_idx==-1) || (!draw->on)) return;
	
	mdl=server.model_list.models[draw->model_idx];

		// setup animated textures

	model_setup_animated_textures(mdl,tick);

		// setup the rendering mesh mask

	draw->render_mesh_mask=draw->mesh_mask;

	for (n=0;n!=mdl->nmesh;n++) {
		if (draw->meshes[n].fade.on) {
			draw->render_mesh_mask|=(0x1<<n);
		}
	}

		// hasn't build vertex list yet

	draw->built_vertex_list=FALSE;

		// run through the meshes and
		// setup flags

		// we setup glow, opaque, and transparent
		// flag for textures, meshes, and drawing as
		// a whole to quickly eliminate some paths

		// we also setup current texture frames
		// and current mesh alphas (can be changed
		// by fades.)

	draw->has_opaque=FALSE;
	draw->has_transparent=FALSE;
	draw->has_glow=FALSE;
	
	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;
		
		mesh=&mdl->meshes[n];

			// get alpha
		
		draw->meshes[n].alpha=render_model_get_mesh_alpha(draw,n);

			// setup textures

		texture=mdl->textures;
		
		for (k=0;k!=max_model_texture;k++) {
			texture_hits[k]=FALSE;

			frame=render_model_get_texture_frame(mdl,draw,k);
			draw->meshes[n].textures[k].frame=frame;

			draw->meshes[n].textures[k].glow=(texture->frames[frame].glowmap.gl_id!=-1);
			if ((!texture->frames[frame].bitmap.opaque) || (draw->meshes[n].alpha!=1.0f)) {
				draw->meshes[n].textures[k].opaque=FALSE;
				draw->meshes[n].textures[k].transparent=TRUE;
			}
			else {
				draw->meshes[n].textures[k].opaque=TRUE;
				draw->meshes[n].textures[k].transparent=FALSE;
			}

			texture++;
		}
		
			// check for any transparent textures
		
		draw->meshes[n].has_opaque=FALSE;
		draw->meshes[n].has_glow=FALSE;

		if (draw->meshes[n].alpha!=1.0f) {
			draw->meshes[n].has_transparent=TRUE;
			draw->has_transparent=TRUE;
		}
		else {
			draw->meshes[n].has_transparent=FALSE;
		}

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// only check once per texture

			txt_idx=poly->txt_idx;
			if (texture_hits[txt_idx]) {
				poly++;
				continue;
			}

			texture_hits[txt_idx]=TRUE;

			texture=&mdl->textures[txt_idx];
			frame=draw->meshes[n].textures[txt_idx].frame;

				// look for glows, opaque, and transparent
			
			if (texture->frames[frame].glowmap.gl_id!=-1) {
				draw->meshes[n].has_glow=TRUE;
				draw->has_glow=TRUE;
			}
	
			if (!texture->frames[frame].bitmap.opaque) {
				draw->meshes[n].has_transparent=TRUE;
				draw->has_transparent=TRUE;
			}
			else {
				draw->meshes[n].has_opaque=TRUE;
				draw->has_opaque=TRUE;
			}
			
			poly++;
		}
	}
}

void render_model_build_vertex_lists(model_draw *draw,bool always_build)
{
	int					n,k,t,idx;
	float				f,*va,*na;
	unsigned char		*cull_ptr;
	d3fpnt				pnt,camera_pnt;
	d3vct				normal;
	model_type			*mdl;
	model_mesh_type		*mesh;
	model_poly_type		*poly;

		// already built list?
		// this happens if we've already draw for
		// the model but need it again for shadows
		// or other rendering

	if ((!always_build) && (draw->built_vertex_list)) return;

	draw->built_vertex_list=TRUE;

		// get model
		
	if ((draw->model_idx==-1) || (!draw->on)) return;

	mdl=server.model_list.models[draw->model_idx];

		// create vertex and uv lists
		
	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;

			// build model vertex list

		model_create_draw_vertexes(mdl,n,&draw->setup);
		if (draw->resize!=1.0f) model_resize_draw_vertex(mdl,n,draw->resize,&draw->setup);
		if (draw->flip_x) model_flip_draw_vertex(mdl,n,&draw->setup);

			// translate vertex to view
			
		model_translate_draw_vertex(mdl,n,draw->pnt.x,draw->pnt.y,draw->pnt.z,&draw->setup);
	}

		// setup the normals

	render_model_create_normal_vertexes(mdl,draw->render_mesh_mask,draw);

		// setup culling

	camera_pnt.x=(float)view.render->camera.pnt.x;
	camera_pnt.y=(float)view.render->camera.pnt.y;
	camera_pnt.z=(float)view.render->camera.pnt.z;

	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;

		mesh=&mdl->meshes[n];

			// no culling
			// initial array is set to 0s so we
			// can just skip out

		if ((mesh->never_cull) || (draw->no_culling)) continue;

			// culled

		poly=mesh->polys;
		cull_ptr=draw->setup.mesh_arrays[n].poly_cull_array;

		for (k=0;k!=mesh->npoly;k++) {

				// get poly normal
				// don't need to normalize or divide
				// out average for normal, but have to
				// for point

			pnt.x=pnt.y=pnt.z=0.0f;
			normal.x=normal.y=normal.z=0.0f;

			for (t=0;t!=poly->ptsz;t++) {
				idx=poly->v[t]*3;

				va=draw->setup.mesh_arrays[n].gl_vertex_array+idx;
				pnt.x+=*va++;
				pnt.y+=*va++;
				pnt.z+=*va;

				na=draw->setup.mesh_arrays[n].gl_normal_array+idx;
				normal.x+=*na++;
				normal.y+=*na++;
				normal.z+=*na;
			}

			f=(float)poly->ptsz;
			pnt.x=(pnt.x/f)-camera_pnt.x;
			pnt.y=(pnt.y/f)-camera_pnt.y;
			pnt.z=(pnt.z/f)-camera_pnt.z;

			*cull_ptr++=(((normal.x*pnt.x)+(normal.y*pnt.y)+(normal.z*pnt.z))>0.0f)?0x1:0x0;

			poly++;
		}
	}
}

