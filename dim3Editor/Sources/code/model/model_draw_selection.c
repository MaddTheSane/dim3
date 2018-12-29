/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Draw Model Selection and Normals

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

extern model_type				model;
extern model_draw_setup			draw_setup;
extern app_state_type			state;
extern app_pref_type			pref;

/* =======================================================

      Draw Selected Vertexes
      
======================================================= */

void model_draw_selected_vertexes(int mesh_idx)
{
	int				n,nvertex;
	float			*pv;
	model_mesh_type	*mesh;
	
	glPointSize(model_draw_vertex_handle_size);
	
	mesh=&model.meshes[mesh_idx];
	nvertex=mesh->nvertex;
	
	pv=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array;

	for (n=0;n!=nvertex;n++) {
	
		if ((!model_vertex_mask_check_sel(mesh_idx,n)) || (model_vertex_mask_check_hide(mesh_idx,n))) {
			pv+=3;
			continue;
		}
		
		if (mesh->vertexes[n].major_bone_idx!=-1) {
			if ((mesh->vertexes[n].major_bone_idx==state.model.cur_bone_idx) && (state.model.cur_bone_idx!=-1)) {
				glColor4f(1.0f,0.2f,1.0f,1.0f);
			}
			else {
				glColor4f(0.0f,0.0f,0.0f,1.0f);
			}
		}
		else {
			glColor4f(0.5f,0.5f,0.5f,1.0f);
		}
	
		glVertexPointer(3,GL_FLOAT,0,pv);
		glDrawArrays(GL_POINTS,0,1);
		
		pv+=3;
	}
	
	glPointSize(1.0f);
}

/* =======================================================

      Draw Selected Poly
      
======================================================= */

void model_draw_selected_poly(int mesh_idx)
{
	int					n,k,npoly,vertex_idx;
	float				vertexes[8*3];
	float				*pv,*pa;
	model_mesh_type		*mesh;
	model_poly_type		*poly;

	mesh=&model.meshes[mesh_idx];
	npoly=mesh->npoly;

		// selection
	
	glColor4f(pref.col.mesh_sel.r,pref.col.mesh_sel.g,pref.col.mesh_sel.b,1.0f);
	glLineWidth(model_draw_poly_select_line_size);

	for (n=0;n!=npoly;n++) {

		if ((!model_poly_mask_check_sel(mesh_idx,n)) || (model_poly_mask_check_hide(mesh_idx,n))) continue;
		
		poly=&model.meshes[mesh_idx].polys[n];

			// draw the selected poly

		pv=vertexes;

		for (k=0;k!=poly->ptsz;k++) {
			pa=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array+(poly->v[k]*3);
			*pv++=*pa++;
			*pv++=*pa++;
			*pv++=*pa;
		}
	
		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINE_LOOP,0,poly->ptsz);
	}
		
	glLineWidth(1.0f);

		// points
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glPointSize(model_draw_vertex_handle_size);
		
	for (n=0;n!=npoly;n++) {

		if ((!model_poly_mask_check_sel(mesh_idx,n)) || (model_poly_mask_check_hide(mesh_idx,n))) continue;
		
		poly=&model.meshes[mesh_idx].polys[n];
		
			// draw any selected vertexes
			// on the poly
		
		for (k=0;k!=poly->ptsz;k++) {
			vertex_idx=poly->v[k];
			if ((model_vertex_mask_check_sel(mesh_idx,vertex_idx)) && (!model_vertex_mask_check_hide(mesh_idx,vertex_idx))) {
				pa=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array+(vertex_idx*3);

				glVertexPointer(3,GL_FLOAT,0,pa);
				glDrawArrays(GL_POINTS,0,1);
			}
		}
	}
		
	glPointSize(1.0f);
}

/* =======================================================

      Draw Model Normals
      
======================================================= */

void model_draw_normals(int mesh_idx)
{
	int					n,nvertex;
	float				fx,fy,fz,vertexes[6];
	float				*pv,*pn,*pt;
	d3vct				tangent,normal,binormal;
	model_mesh_type		*mesh;
	model_vertex_type	*vertex;
	
		// draw normals
	
	glLineWidth(model_draw_normal_size);
	
	mesh=&model.meshes[mesh_idx];
	
	nvertex=mesh->nvertex;
	vertex=mesh->vertexes;
	
	pv=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array;
	pn=draw_setup.mesh_arrays[mesh_idx].gl_normal_array;
	pt=draw_setup.mesh_arrays[mesh_idx].gl_tangent_array;

	for (n=0;n!=nvertex;n++) {
		
			// skip hiddens
			
		if (model_vertex_mask_check_hide(mesh_idx,n)) {
			pv+=3;
			pn+=3;
			pt+=3;
			continue;
		}

			// vertex point

		fx=*pv++;
		fy=*pv++;
		fz=*pv++;

		normal.x=*pn++;
		normal.y=*pn++;
		normal.z=*pn++;

		if (pref.show_tangent_binormal) {

				// tangent

			vertexes[0]=fx;
			vertexes[1]=fy;
			vertexes[2]=fz;

			tangent.x=*pt++;
			tangent.y=*pt++;
			tangent.z=*pt++;

			vertexes[3]=fx+(tangent.x*model_draw_normal_len);
			vertexes[4]=fy+(tangent.y*model_draw_normal_len);
			vertexes[5]=fz+(tangent.z*model_draw_normal_len);
		
			glVertexPointer(3,GL_FLOAT,0,vertexes);

			glColor4f(1.0f,0.0f,0.0f,1.0f);
			glDrawArrays(GL_LINES,0,2);

				// binormal

			vector_cross_product(&binormal,&tangent,&normal);

			vertexes[0]=fx;
			vertexes[1]=fy;
			vertexes[2]=fz;

			vertexes[3]=fx+(binormal.x*model_draw_normal_len);
			vertexes[4]=fy+(binormal.y*model_draw_normal_len);
			vertexes[5]=fz+(binormal.z*model_draw_normal_len);

			glVertexPointer(3,GL_FLOAT,0,vertexes);

			glColor4f(0.0f,0.0f,1.0f,1.0f);
			glDrawArrays(GL_LINES,0,2);
		}

			// normal

		vertexes[0]=fx;
		vertexes[1]=fy;
		vertexes[2]=fz;

		vertexes[3]=fx+(normal.x*model_draw_normal_len);
		vertexes[4]=fy+(normal.y*model_draw_normal_len);
		vertexes[5]=fz+(normal.z*model_draw_normal_len);

		glVertexPointer(3,GL_FLOAT,0,vertexes);

		glColor4f(1.0f,0.0f,1.0f,1.0f);
		glDrawArrays(GL_LINES,0,2);
	}
	
	glLineWidth(1.0f);
}
