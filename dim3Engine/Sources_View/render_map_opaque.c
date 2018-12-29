/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Opaque Segment Rendering

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

extern map_type			map;
extern server_type		server;
extern setup_type		setup;
extern camera_type		camera;
extern view_type		view;

/* =======================================================

      Opaque Map Rendering
      
======================================================= */

void render_map_mesh_opaque(void)
{

	int							n,k,mesh_idx,frame;
	float						alpha;
	bool						lighting_small;
	GLuint						gl_id;
	texture_type				*texture;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	view_glsl_light_list_type	light_list;

		// setup view
	
	glEnable(GL_DEPTH_TEST); 

	glDisable(GL_BLEND);
	
		// run through draw list

	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

			// skip meshes with no opaques

		mesh_idx=view.render->draw_list.items[n].idx;

		mesh=&map.mesh.meshes[mesh_idx];
		if (!mesh->draw.has_opaque) continue;
		
			// the mesh vbo
			
		view_bind_mesh_liquid_vertex_object(&mesh->vbo);
		view_bind_mesh_liquid_index_object(&mesh->vbo);

		gl_shader_draw_execute_reset_cached_offsets();

			// small meshes don't create light lists
			// per-poly, instead just use the mesh list
			// if the mesh list always has less than
			// max shader lights, just use that

		lighting_small=((mesh->precalc_flag.lighting_small) || (mesh->light_cache.count<=max_shader_light));
		if (lighting_small) gl_lights_build_mesh_glsl_light_list(mesh,&light_list);

			// draw the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// skip transparent or culled polys

			if ((poly->draw.transparent_on) || (poly->draw.culled[view.render->cull_idx])) {
				poly++;
				continue;
			}

				// setup shader

			texture=&map.textures[poly->txt_idx];
			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

			if (!lighting_small) gl_lights_build_poly_glsl_light_list(mesh,poly,&light_list);
			gl_shader_draw_execute_map_start(texture,poly->txt_idx,frame,poly->lmap_txt_idx,1.0f,0,(3*sizeof(float)),(5*sizeof(float)),(7*sizeof(float)),(10*sizeof(float)),mesh->vbo.vertex_stride,&light_list);

				// fix texture if any back rendering

			if (gl_back_render_get_texture(poly->camera,&gl_id,&alpha)) {
				gl_shader_texture_override(gl_id,alpha);
			}

				// draw polygon
				
			glDrawElements(GL_TRIANGLE_FAN,poly->ptsz,GL_UNSIGNED_SHORT,(GLvoid*)poly->vbo.index_offset);
			
			gl_shader_draw_execute_map_end(texture,&light_list);
		
			poly++;
			view.count.mesh_poly++;
		}
		
		view_unbind_mesh_liquid_vertex_object();
		view_unbind_mesh_liquid_index_object();
	}
}

