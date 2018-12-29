/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Draw A Model

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
#include "interface.h"

int								cur_txt_idx;

extern model_type				model;
extern model_draw_setup			draw_setup;
extern app_state_type			state;

/* =======================================================

      Setup Model Textures
      
======================================================= */

void model_texture_start(void)
{
		// setup texturing

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glColor3f(1.0f,1.0f,1.0f);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);

		// no texture set

	cur_txt_idx=-1;
}

void model_texture_set(int txt_idx)
{
	GLuint			gl_id;
	texture_type	*texture;
	
	if (txt_idx==cur_txt_idx) return;

	cur_txt_idx=txt_idx;
	texture=&model.textures[txt_idx];

		// blending
		
	if (texture->additive) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
	
		// get texture ids
		
	if (state.model.play_mode!=model_play_mode_stop) {
		gl_id=texture->frames[texture->animate.current_frame].bitmap.gl_id;
	}
	else {
		gl_id=texture->frames[0].bitmap.gl_id;
	}
	
	glBindTexture(GL_TEXTURE_2D,gl_id);
}

void model_texture_end(void)
{
		// reset blending and combine
		
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

		// turn off texturing
		
	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Transparent Polygon Sorting
      
======================================================= */

float model_draw_transparent_poly_closest_z(int mesh_idx,model_poly_type *poly)
{
	int				n;
	float			d,dist;
	float			*pv;
	d3fpnt			pnt;

		// calculate the closest z
		// that is on screen

	dist=2.0f;

	for (n=0;n!=poly->ptsz;n++) {
		pv=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array+(poly->v[n]*3);
		pnt.x=*pv++;
		pnt.y=*pv++;
		pnt.z=*pv;
		
		d=model_draw_project_get_depth(&pnt);
		if (d<dist) dist=d;
	}

	return(dist);
}

bool model_draw_transparent_sort(int mesh_idx)
{
	int					n,k,sort_cnt,sort_idx;
	float				dist;
	model_mesh_type		*mesh;
    model_poly_type		*poly;

		// setup projections

	model_draw_setup_project_point();

		// create sort list

	mesh=&model.meshes[mesh_idx];

	sort_cnt=0;
	
	for (n=0;n!=mesh->npoly;n++) {

		poly=&mesh->polys[n];

			// skip out if hidden or opaque

		if (model_vertex_mask_check_hide_poly(mesh_idx,poly)) continue;
		if (model.textures[poly->txt_idx].frames[0].bitmap.opaque) continue;

			// find distance from camera

		dist=model_draw_transparent_poly_closest_z(mesh_idx,poly);

			// find position in sort list
			// this list will be reversed, from nearest
			// to farests (as that works better for sorting)

		sort_idx=sort_cnt;

		for (k=0;k!=sort_cnt;k++) {
			if (dist<mesh->trans_sort.polys[k].dist) {
				sort_idx=k;
				break;
			}
		}

			// add to sort list

		if (sort_idx<sort_cnt) {
			memmove(&mesh->trans_sort.polys[sort_idx+1],&mesh->trans_sort.polys[sort_idx],((sort_cnt-sort_idx)*sizeof(model_trans_sort_poly_type)));
		}

		mesh->trans_sort.polys[sort_idx].poly_idx=n;
		mesh->trans_sort.polys[sort_idx].dist=dist;

		sort_cnt++;
	}

	mesh->trans_sort.count=sort_cnt;

	return(sort_cnt!=0);
}

/* =======================================================

      Draw Model
      
======================================================= */

void model_draw_mesh_polygons_opaque(int mesh_idx)
{
	int					n,k;
	float				vertexes[8*3],uvs[8*2];
	float				*pa,*pv,*pt;
	model_mesh_type		*mesh;
    model_poly_type		*poly;

	glDisable(GL_BLEND);

	mesh=&model.meshes[mesh_idx];

	for (n=0;n!=mesh->npoly;n++) {
		poly=&mesh->polys[n];

			// skip hidden or transparent

		if (model_vertex_mask_check_hide_poly(mesh_idx,poly)) continue;
		if (!model.textures[poly->txt_idx].frames[0].bitmap.opaque) continue;

			// switch texture?

		model_texture_set(poly->txt_idx);

			// setup and draw triangle

		pv=vertexes;
		pt=uvs;

		for (k=0;k!=poly->ptsz;k++) {
			pa=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array+(poly->v[k]*3);
			*pv++=*pa++;
			*pv++=*pa++;
			*pv++=*pa;
			*pt++=poly->gx[k];
			*pt++=poly->gy[k];
		}

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glTexCoordPointer(2,GL_FLOAT,0,uvs);

		glDrawArrays(GL_POLYGON,0,poly->ptsz);
	}
}

void model_draw_mesh_transparent_polygons(int mesh_idx)
{
	int					n,k;
	float				vertexes[8*3],uvs[8*2];
	float				*pa,*pv,*pt;
	model_mesh_type		*mesh;
    model_poly_type		*poly;

		// sort the polys

	if (!model_draw_transparent_sort(mesh_idx)) return;

		// draw transparent sorted polygons
		// sorting already removes hidden
		// or opaque polygons

		// list is nearest to farthest, so we
		// run it backwards

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);

	mesh=&model.meshes[mesh_idx];

	for (n=(mesh->trans_sort.count-1);n>=0;n--) {
		poly=&mesh->polys[mesh->trans_sort.polys[n].poly_idx];

			// switch texture?

		model_texture_set(poly->txt_idx);

			// setup and draw triangle

		pv=vertexes;
		pt=uvs;

		for (k=0;k!=poly->ptsz;k++) {
			pa=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array+(poly->v[k]*3);
			*pv++=*pa++;
			*pv++=*pa++;
			*pv++=*pa;
			*pt++=poly->gx[k];
			*pt++=poly->gy[k];
		}

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glTexCoordPointer(2,GL_FLOAT,0,uvs);

		glDrawArrays(GL_POLYGON,0,poly->ptsz);
	}

	glDepthMask(GL_TRUE);
}


void model_draw(int mesh_idx)
{
	model_mesh_type			*mesh;
	
		// model vertexes and normal arrays
		
	mesh=&model.meshes[mesh_idx];

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
		// setup the current texture frames
		
	if (state.model.play_mode!=model_play_mode_stop) {
		model_setup_animated_textures(&model,time_get());
	}
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);

	model_texture_start();
	model_draw_mesh_polygons_opaque(mesh_idx);
	model_draw_mesh_transparent_polygons(mesh_idx);

		// finish drawing

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	model_texture_end();	
}

