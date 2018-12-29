/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Vertex Objects

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
extern view_type			view;
extern server_type			server;
extern setup_type			setup;

int							vbo_text_index,vbo_utility_index;
bool						vbo_utility_init;
GLuint						vbo_sky,vbo_fog,vbo_rain,
							vbo_text[view_vbo_text_count],
							vbo_utility[view_vbo_utility_count];

/* =======================================================

      Mesh and Liquid VBOs
      
======================================================= */

void view_create_mesh_liquid_vertex_object(map_vbo_type *vbo,int vertex_count,int vertex_stride,int index_count)
{
	glGenBuffers(1,&vbo->vertex);
	glGenBuffers(1,&vbo->index);

		// init the vertex buffer

	vbo->vertex_count=vertex_count;
	vbo->vertex_mem_sz=vertex_count*vertex_stride;
	vbo->vertex_stride=vertex_stride;

	glBindBuffer(GL_ARRAY_BUFFER,vbo->vertex);
	glBufferData(GL_ARRAY_BUFFER,vbo->vertex_mem_sz,NULL,GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,0);

		// init the index buffer

	vbo->index_count=index_count;
	vbo->index_mem_sz=index_count*sizeof(unsigned short);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vbo->index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,vbo->index_mem_sz,NULL,GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

void view_dispose_mesh_liquid_vertex_object(map_vbo_type *vbo)
{
	glDeleteBuffers(1,&vbo->vertex);
	glDeleteBuffers(1,&vbo->index);
}

void view_bind_mesh_liquid_vertex_object(map_vbo_type *vbo)
{
	glBindBuffer(GL_ARRAY_BUFFER,vbo->vertex);
}

unsigned char* view_map_mesh_liquid_vertex_object(map_vbo_type *vbo)
{
	return((unsigned char*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY));
}

void view_unmap_mesh_liquid_vertex_object(void)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void view_unbind_mesh_liquid_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void view_bind_mesh_liquid_index_object(map_vbo_type *vbo)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vbo->index);
}

unsigned short* view_map_mesh_liquid_index_object(void)
{
	return((unsigned short*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_WRITE_ONLY));
}

void view_unmap_mesh_liquid_index_object(void)
{
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void view_unbind_mesh_liquid_index_object(void)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

/* =======================================================

      Model VBOs
      
======================================================= */

void view_create_model_vertex_object(model_draw *draw)
{
	int					n,k,t,vertex_cnt,stride;
	model_type			*mdl;
	model_mesh_type		*mesh;
	model_poly_type		*poly;
	
		// each mesh has own VBO
		
	mdl=server.model_list.models[draw->model_idx];

	for (n=0;n!=mdl->nmesh;n++) {
		glGenBuffers(1,&draw->vbo[n].vertex);
		
			// get the vertex count
			
		mesh=&mdl->meshes[n];
		
		vertex_cnt=0;
		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			vertex_cnt+=poly->ptsz;
			poly++;
		}
			
			// get the model vertex size
			
		stride=(3+2+3+3)*sizeof(float);					// 3 vertex, 2 uv, 3 tangent, 3 normal

			// init the vertex buffer

		draw->vbo[n].vertex_count=vertex_cnt;
		draw->vbo[n].vertex_stride=stride;
		draw->vbo[n].vertex_mem_sz=(stride*vertex_cnt);

		glBindBuffer(GL_ARRAY_BUFFER,draw->vbo[n].vertex);
		glBufferData(GL_ARRAY_BUFFER,draw->vbo[n].vertex_mem_sz,NULL,GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);

			// any shadows?

		if (!draw->shadow.on) continue;
		
			// shadows have multiple vertexes
			// so we don't have blocking problems
			
		draw->vbo[n].shadow_current_idx=0;
		draw->vbo[n].shadow_vertex_mem_sz=((3*sizeof(float))*vertex_cnt);
		
		for (t=0;t!=max_model_shadow_vbo_count;t++) {
			glGenBuffers(1,&draw->vbo[n].shadow_vertexes[t]);

			glBindBuffer(GL_ARRAY_BUFFER,draw->vbo[n].shadow_vertexes[t]);
			glBufferData(GL_ARRAY_BUFFER,draw->vbo[n].shadow_vertex_mem_sz,NULL,GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER,0);
		}
	}
}

void view_dispose_model_vertex_object(model_draw *draw)
{
	int					n,t;
	model_type			*mdl;
	
		// each mesh has own VBO
		
	mdl=server.model_list.models[draw->model_idx];

	for (n=0;n!=mdl->nmesh;n++) {
	
			// model vertexes
			
		glDeleteBuffers(1,&draw->vbo[n].vertex);
		if (!draw->shadow.on) continue;
		
			// shadow vertexes
		
		for (t=0;t!=max_model_shadow_vbo_count;t++) {
			glDeleteBuffers(1,&draw->vbo[n].shadow_vertexes[t]);
		}
	}
}

void view_bind_model_vertex_object(model_draw *draw,int mesh_idx)
{
	glBindBuffer(GL_ARRAY_BUFFER,draw->vbo[mesh_idx].vertex);
}

unsigned char* view_map_model_vertex_object(void)
{
	return((unsigned char*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY));
}

void view_unmap_model_vertex_object(void)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void view_unbind_model_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void view_bind_model_shadow_vertex_object(model_draw *draw,int mesh_idx)
{
		// bind current shadow vertex
		
	glBindBuffer(GL_ARRAY_BUFFER,draw->vbo[mesh_idx].shadow_vertexes[draw->vbo[mesh_idx].shadow_current_idx]);
	
		// and use the next one next time
		
	draw->vbo[mesh_idx].shadow_current_idx++;
	if (draw->vbo[mesh_idx].shadow_current_idx==max_model_shadow_vbo_count) draw->vbo[mesh_idx].shadow_current_idx=0;
}

unsigned char* view_map_model_shadow_vertex_object(void)
{
	return((unsigned char*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY));
}

void view_unmap_model_shadow_vertex_object(void)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void view_unbind_model_shadow_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

/* =======================================================

      Sky VBOs
      
======================================================= */

void view_create_sky_vertex_object(int vertex_mem_sz)
{
	glGenBuffers(1,&vbo_sky);

	glBindBuffer(GL_ARRAY_BUFFER,vbo_sky);
	glBufferData(GL_ARRAY_BUFFER,vertex_mem_sz,NULL,GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void view_dispose_sky_vertex_object(void)
{
	glDeleteBuffers(1,&vbo_sky);
}

void view_bind_sky_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,vbo_sky);
}

unsigned char* view_map_sky_vertex_object(void)
{
	return((unsigned char*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY));
}

void view_unmap_sky_vertex_object(void)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void view_unbind_sky_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

/* =======================================================

      Fog VBOs
      
======================================================= */

void view_create_fog_vertex_object(int vertex_mem_sz)
{
	glGenBuffers(1,&vbo_fog);

	glBindBuffer(GL_ARRAY_BUFFER,vbo_fog);
	glBufferData(GL_ARRAY_BUFFER,vertex_mem_sz,NULL,GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void view_dispose_fog_vertex_object(void)
{
	glDeleteBuffers(1,&vbo_fog);
}

void view_bind_fog_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,vbo_fog);
}

unsigned char* view_map_fog_vertex_object(void)
{
	return((unsigned char*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY));
}

void view_unmap_fog_vertex_object(void)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void view_unbind_fog_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

/* =======================================================

      Rain VBOs
      
======================================================= */

void view_create_rain_vertex_object(int vertex_mem_sz)
{
	glGenBuffers(1,&vbo_rain);

	glBindBuffer(GL_ARRAY_BUFFER,vbo_rain);
	glBufferData(GL_ARRAY_BUFFER,vertex_mem_sz,NULL,GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void view_dispose_rain_vertex_object(void)
{
	glDeleteBuffers(1,&vbo_rain);
}

void view_bind_rain_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,vbo_rain);
}

unsigned char* view_map_rain_vertex_object(void)
{
	return((unsigned char*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY));
}

void view_unmap_rain_vertex_object(void)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void view_unbind_rain_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

/* =======================================================

      Effect VBOs

	  Effect VBOs are the only VBOs that are dynamic
	  so we need a flag to tell if they are active so
	  we can clean them up later
      
======================================================= */

void view_clear_effect_vertex_object(effect_type *effect)
{
	effect->vbo.active=FALSE;
}

void view_create_effect_vertex_object(effect_type *effect,int vertex_mem_sz,int index_mem_sz)
{
		// is it already active?

	if (effect->vbo.active) return;

		// mark VBO as active

	effect->vbo.active=TRUE;

		// create the vertex buffer

	glGenBuffers(1,&effect->vbo.vertex);

	effect->vbo.vertex_mem_sz=vertex_mem_sz;

	glBindBuffer(GL_ARRAY_BUFFER,effect->vbo.vertex);
	glBufferData(GL_ARRAY_BUFFER,vertex_mem_sz,NULL,GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,0);

		// is there an index buffer?

	if (index_mem_sz==-1) {
		effect->vbo.index_mem_sz=-1;
		return;
	}

		// init the index buffer

	glGenBuffers(1,&effect->vbo.index);

	effect->vbo.index_mem_sz=index_mem_sz;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,effect->vbo.index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,index_mem_sz,NULL,GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

void view_dispose_effect_vertex_object(effect_type *effect)
{
		// is it already disposed?

	if (!effect->vbo.active) return;

		// dispose vertex

	effect->vbo.active=FALSE;
	glDeleteBuffers(1,&effect->vbo.vertex);

		// dispose index

	if (effect->vbo.index_mem_sz!=-1) glDeleteBuffers(1,&effect->vbo.index);
}

void view_bind_effect_vertex_object(effect_type *effect)
{
	glBindBuffer(GL_ARRAY_BUFFER,effect->vbo.vertex);
}

unsigned char* view_map_effect_vertex_object(void)
{
	return((unsigned char*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY));
}

void view_unmap_effect_vertex_object(void)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void view_unbind_effect_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void view_bind_effect_index_object(effect_type *effect)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,effect->vbo.index);
}

unsigned short* view_map_effect_index_object(void)
{
	return((unsigned short*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_WRITE_ONLY));
}

void view_unmap_effect_index_object(void)
{
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void view_unbind_effect_index_object(void)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

/* =======================================================

      Text VBOs
      
======================================================= */

void view_create_text_vertex_object(void)
{
	int			n,vertex_mem_sz;
	
	glGenBuffers(view_vbo_text_count,vbo_text);
	
	vertex_mem_sz=(1024*(2+2))*sizeof(float);

	for (n=0;n!=view_vbo_text_count;n++) {
		glBindBuffer(GL_ARRAY_BUFFER,vbo_text[n]);
		glBufferData(GL_ARRAY_BUFFER,vertex_mem_sz,NULL,GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}
	
	vbo_text_index=0;
}

void view_dispose_text_vertex_object(void)
{
	glDeleteBuffers(view_vbo_text_count,vbo_text);
}

void view_bind_text_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,vbo_text[vbo_text_index]);
	
	vbo_text_index++;
	if (vbo_text_index==view_vbo_text_count) vbo_text_index=0;
}

unsigned char* view_map_text_vertex_object(void)
{
	return((unsigned char*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY));
}

void view_unmap_text_vertex_object(void)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void view_unbind_text_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

/* =======================================================

      Utility VBOs
      
======================================================= */

void view_initialize_utility_vertex_object(void)
{
	vbo_utility_init=FALSE;
}

void view_create_utility_vertex_object(void)
{
	int			n,vertex_mem_sz;
	
	glGenBuffers(view_vbo_utility_count,vbo_utility);
	
	vertex_mem_sz=((128*(3+2))*sizeof(float))+((128*4)*sizeof(unsigned char));		// possibly up to 128 vertexes, uvs, colors

	for (n=0;n!=view_vbo_utility_count;n++) {
		glBindBuffer(GL_ARRAY_BUFFER,vbo_utility[n]);
		glBufferData(GL_ARRAY_BUFFER,vertex_mem_sz,NULL,GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}
	
	vbo_utility_index=0;
	
	vbo_utility_init=TRUE;
}

void view_dispose_utility_vertex_object(void)
{
	if (vbo_utility_init) glDeleteBuffers(view_vbo_utility_count,vbo_utility);
}

void view_bind_utility_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,vbo_utility[vbo_utility_index]);
	
	vbo_utility_index++;
	if (vbo_utility_index==view_vbo_utility_count) vbo_utility_index=0;
}

unsigned char* view_map_utility_vertex_object(void)
{
	return((unsigned char*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY));
}

void view_unmap_utility_vertex_object(void)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void view_unbind_utility_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,0);
}
