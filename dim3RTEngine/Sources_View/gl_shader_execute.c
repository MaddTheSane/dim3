/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Shader Execution

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
	#include "dim3engine.h"
#endif

#include "interface.h"

extern map_type				map;
extern view_type			view;
extern setup_type			setup;
extern file_path_setup_type	file_path_setup;

extern shader_type			*gl_shader_current,
							color_shader,gradient_shader,bitmap_shader;

extern float				gl_proj_matrix[16];

/* =======================================================

      Shader Generic Set Program and Variables
      
======================================================= */

void gl_shader_draw_execute_set_program(shader_type *shader)
{
		// switch program if necessary

	if (shader!=gl_shader_current) {
		gl_shader_current=shader;
		glUseProgram(shader->program_obj);
	}

		// set projection matrix
		// we never use model view as this is always 2D drawing

	glUniformMatrix4fv(shader->var_locs.dim3ProjectionMatrix,1,GL_FALSE,gl_proj_matrix);
}

/* =======================================================

      Execute Simple Color Shaders
      
======================================================= */

void gl_shader_draw_execute_simple_color_set_color(d3col *col,float alpha)
{
	glUniform4f(color_shader.var_locs.dim3SimpleColor,col->r,col->g,col->b,alpha);
}

void gl_shader_draw_execute_simple_color_start(int vertex_size,int vertex_offset,d3col *col,float alpha)
{
	gl_shader_draw_execute_set_program(&color_shader);
	gl_shader_draw_execute_simple_color_set_color(col,alpha);
		
	glEnableVertexAttribArray(color_shader.var_locs.dim3Vertex);
	glVertexAttribPointer(color_shader.var_locs.dim3Vertex,vertex_size,GL_FLOAT,GL_FALSE,0,(void*)vertex_offset);
}

void gl_shader_draw_execute_simple_color_end(void)
{
	glDisableVertexAttribArray(color_shader.var_locs.dim3Vertex);
}

/* =======================================================

      Execute Simple Gradient Shaders
      
======================================================= */

void gl_shader_draw_execute_simple_gradient_start(int vertex_size,int vertex_offset,int color_offset)
{
	gl_shader_draw_execute_set_program(&gradient_shader);
		
	glEnableVertexAttribArray(gradient_shader.var_locs.dim3Vertex);
	glVertexAttribPointer(gradient_shader.var_locs.dim3Vertex,vertex_size,GL_FLOAT,GL_FALSE,0,(void*)vertex_offset);

	glEnableVertexAttribArray(gradient_shader.var_locs.dim3VertexColor);
	glVertexAttribPointer(gradient_shader.var_locs.dim3VertexColor,4,GL_UNSIGNED_BYTE,GL_TRUE,0,(void*)color_offset);
}

void gl_shader_draw_execute_simple_gradient_end(void)
{
	glDisableVertexAttribArray(gradient_shader.var_locs.dim3Vertex);
	glDisableVertexAttribArray(gradient_shader.var_locs.dim3VertexColor);
}

/* =======================================================

      Execute Simple Bitmap Shaders
      
======================================================= */

void gl_shader_draw_execute_simple_bitmap_set_color(d3col *col,float alpha)
{
	glUniform4f(bitmap_shader.var_locs.dim3SimpleColor,col->r,col->g,col->b,alpha);
}

void gl_shader_draw_execute_simple_bitmap_set_texture(unsigned long gl_id)
{
	gl_texture_bind(0,gl_id);
}

void gl_shader_draw_execute_simple_bitmap_start(int vertex_size,int vertex_offset,int uv_offset,int stride,d3col *col,float alpha)
{
	gl_shader_draw_execute_set_program(&bitmap_shader);
	gl_shader_draw_execute_simple_bitmap_set_color(col,alpha);
	
	glEnableVertexAttribArray(bitmap_shader.var_locs.dim3Vertex);
	glVertexAttribPointer(bitmap_shader.var_locs.dim3Vertex,vertex_size,GL_FLOAT,GL_FALSE,stride,(void*)vertex_offset);

	glEnableVertexAttribArray(bitmap_shader.var_locs.dim3VertexUV);
	glVertexAttribPointer(bitmap_shader.var_locs.dim3VertexUV,2,GL_FLOAT,GL_FALSE,stride,(void*)uv_offset);
}

void gl_shader_draw_execute_simple_bitmap_end(void)
{
	glDisableVertexAttribArray(bitmap_shader.var_locs.dim3Vertex);
	glDisableVertexAttribArray(bitmap_shader.var_locs.dim3VertexUV);
}

