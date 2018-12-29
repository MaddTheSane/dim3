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
GLuint						vbo_text[view_vbo_text_count],
							vbo_utility[view_vbo_utility_count];

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
