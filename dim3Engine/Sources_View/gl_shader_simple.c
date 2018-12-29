/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Simple Shader Routines

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
extern setup_type			setup;
extern view_type			view;
extern render_info_type		render_info;

shader_type					color_shader,gradient_shader,black_shader,
							bitmap_shader;

/* =======================================================

      Build Simple Color Shader
      
======================================================= */

char* gl_simple_color_shader_build_vert(void)
{
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// need to define out lowp/mediump/highp
		// for non ES2 shaders

	gl_core_shader_build_generic_precision_defines(buf);

		// build vert shader

	strcat(buf,"uniform highp mat4 dim3ProjectionMatrix,dim3ModelViewMatrix;\n");
	strcat(buf,"attribute highp vec3 dim3Vertex;\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	strcat(buf,"gl_Position=dim3ProjectionMatrix*dim3ModelViewMatrix*vec4(dim3Vertex,1.0);\n");
	strcat(buf,"}\n");

	return(buf);
}

char* gl_simple_color_shader_build_frag(void)
{
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// need to define out lowp/mediump/highp
		// for non ES2 shaders

	gl_core_shader_build_generic_precision_defines(buf);

		// build frag shader
		
	strcat(buf,"uniform lowp vec4 dim3SimpleColor;\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	strcat(buf,"gl_FragColor=dim3SimpleColor;\n");
	strcat(buf,"}\n");

	return(buf);
}

bool gl_simple_color_shader_create(shader_type *shader,char *err_str)
{
	char				*vertex_data,*fragment_data;
	bool				ok;
	
		// create the shader code

	vertex_data=gl_simple_color_shader_build_vert();
	if (vertex_data==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	fragment_data=gl_simple_color_shader_build_frag();
	if (fragment_data==NULL) {
		free(vertex_data);
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// create the name
		
	strcpy(shader->name,"simple_color");
	sprintf(shader->vertex_name,"%s_vert",shader->name);
	sprintf(shader->fragment_name,"%s_frag",shader->name);
	
		// compile the code

	ok=gl_shader_code_compile(shader,vertex_data,fragment_data,err_str);

		// free the code

	free(vertex_data);
	free(fragment_data);

	return(ok);
}

/* =======================================================

      Build Simple Gradient Shader
      
======================================================= */

char* gl_simple_gradient_shader_build_vert(void)
{
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// need to define out lowp/mediump/highp
		// for non ES2 shaders

	gl_core_shader_build_generic_precision_defines(buf);

		// build vert shader

	strcat(buf,"uniform highp mat4 dim3ProjectionMatrix,dim3ModelViewMatrix;\n");
	strcat(buf,"attribute highp vec3 dim3Vertex;\n");
	strcat(buf,"attribute lowp vec4 dim3VertexColor;\n");
	strcat(buf,"varying lowp vec4 color;\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	strcat(buf,"gl_Position=dim3ProjectionMatrix*dim3ModelViewMatrix*vec4(dim3Vertex,1.0);\n");
	strcat(buf,"color=dim3VertexColor;\n");
	strcat(buf,"}\n");

	return(buf);
}

char* gl_simple_gradient_shader_build_frag(void)
{
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// need to define out lowp/mediump/highp
		// for non ES2 shaders

	gl_core_shader_build_generic_precision_defines(buf);

		// build frag shader
		
	strcat(buf,"varying lowp vec4 color;\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	strcat(buf,"gl_FragColor=color;\n");
	strcat(buf,"}\n");

	return(buf);
}

bool gl_simple_gradient_shader_create(shader_type *shader,char *err_str)
{
	char				*vertex_data,*fragment_data;
	bool				ok;
	
		// create the shader code

	vertex_data=gl_simple_gradient_shader_build_vert();
	if (vertex_data==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	fragment_data=gl_simple_gradient_shader_build_frag();
	if (fragment_data==NULL) {
		free(vertex_data);
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// create the name
		
	strcpy(shader->name,"simple_gradient");
	sprintf(shader->vertex_name,"%s_vert",shader->name);
	sprintf(shader->fragment_name,"%s_frag",shader->name);
	
		// compile the code

	ok=gl_shader_code_compile(shader,vertex_data,fragment_data,err_str);

		// free the code

	free(vertex_data);
	free(fragment_data);

	return(ok);
}

/* =======================================================

      Build Simple Black Shader
      
======================================================= */

char* gl_simple_black_shader_build_vert(void)
{
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// need to define out lowp/mediump/highp
		// for non ES2 shaders

	gl_core_shader_build_generic_precision_defines(buf);

		// build vert shader

	strcat(buf,"uniform highp mat4 dim3ProjectionMatrix,dim3ModelViewMatrix;\n");
	strcat(buf,"attribute highp vec3 dim3Vertex;\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	strcat(buf,"gl_Position=dim3ProjectionMatrix*dim3ModelViewMatrix*vec4(dim3Vertex,1.0);\n");
	strcat(buf,"}\n");

	return(buf);
}

char* gl_simple_black_shader_build_frag(void)
{
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// need to define out lowp/mediump/highp
		// for non ES2 shaders

	gl_core_shader_build_generic_precision_defines(buf);

		// build frag shader
		
	strcat(buf,"uniform lowp float dim3Alpha;\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	strcat(buf,"gl_FragColor=vec4(0,0,0,dim3Alpha);\n");
	strcat(buf,"}\n");

	return(buf);
}

bool gl_simple_black_shader_create(shader_type *shader,char *err_str)
{
	char				*vertex_data,*fragment_data;
	bool				ok;
	
		// create the shader code

	vertex_data=gl_simple_black_shader_build_vert();
	if (vertex_data==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	fragment_data=gl_simple_black_shader_build_frag();
	if (fragment_data==NULL) {
		free(vertex_data);
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// create the name
		
	strcpy(shader->name,"simple_black");
	sprintf(shader->vertex_name,"%s_vert",shader->name);
	sprintf(shader->fragment_name,"%s_frag",shader->name);
	
		// compile the code

	ok=gl_shader_code_compile(shader,vertex_data,fragment_data,err_str);

		// free the code

	free(vertex_data);
	free(fragment_data);

	return(ok);
}

/* =======================================================

      Build Simple Bitmap Shader
      
======================================================= */

char* gl_simple_bitmap_shader_build_vert(void)
{
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// need to define out lowp/mediump/highp
		// for non ES2 shaders

	gl_core_shader_build_generic_precision_defines(buf);

		// build vert shader

	strcat(buf,"uniform highp mat4 dim3ProjectionMatrix,dim3ModelViewMatrix;\n");
	strcat(buf,"attribute highp vec3 dim3Vertex;\n");
	strcat(buf,"attribute mediump vec2 dim3VertexUV;\n");
	strcat(buf,"varying mediump vec2 uv;\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	strcat(buf,"gl_Position=dim3ProjectionMatrix*dim3ModelViewMatrix*vec4(dim3Vertex,1.0);\n");
	strcat(buf,"uv=dim3VertexUV;\n");
	strcat(buf,"}\n");

	return(buf);
}

char* gl_simple_bitmap_shader_build_frag(void)
{
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// need to define out lowp/mediump/highp
		// for non ES2 shaders

	gl_core_shader_build_generic_precision_defines(buf);

		// build frag shader
		
	strcat(buf,"uniform lowp sampler2D dim3Tex;\n");
	strcat(buf,"uniform lowp vec4 dim3SimpleColor;\n");
	strcat(buf,"varying mediump vec2 uv;\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	strcat(buf,"gl_FragColor=texture2D(dim3Tex,uv)*dim3SimpleColor;\n");
	strcat(buf,"}\n");

	return(buf);
}

bool gl_simple_bitmap_shader_create(shader_type *shader,char *err_str)
{
	char				*vertex_data,*fragment_data;
	bool				ok;
	
		// create the shader code

	vertex_data=gl_simple_bitmap_shader_build_vert();
	if (vertex_data==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	fragment_data=gl_simple_bitmap_shader_build_frag();
	if (fragment_data==NULL) {
		free(vertex_data);
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// create the name
		
	strcpy(shader->name,"simple_bitmap");
	sprintf(shader->vertex_name,"%s_vert",shader->name);
	sprintf(shader->fragment_name,"%s_frag",shader->name);
	
		// compile the code

	ok=gl_shader_code_compile(shader,vertex_data,fragment_data,err_str);

		// free the code

	free(vertex_data);
	free(fragment_data);

	return(ok);
}

/* =======================================================

      Simple Shader Initialize/Shutdown
      
======================================================= */

bool gl_simple_shader_initialize(char *err_str)
{
		// clear simple shaders

	gl_shader_code_clear(&color_shader);
	gl_shader_code_clear(&gradient_shader);
	gl_shader_code_clear(&black_shader);
	gl_shader_code_clear(&bitmap_shader);

		// initialize simple shaders	
		
	if (!gl_simple_color_shader_create(&color_shader,err_str)) {
		gl_simple_shader_shutdown();
		return(FALSE);
	}
	
	if (!gl_simple_gradient_shader_create(&gradient_shader,err_str)) {
		gl_simple_shader_shutdown();
		return(FALSE);
	}

	if (!gl_simple_black_shader_create(&black_shader,err_str)) {
		gl_simple_shader_shutdown();
		return(FALSE);
	}

	if (!gl_simple_bitmap_shader_create(&bitmap_shader,err_str)) {
		gl_simple_shader_shutdown();
		return(FALSE);
	}
	
	return(TRUE);
}

void gl_simple_shader_shutdown(void)
{
		// shutdown shaders

	gl_shader_code_shutdown(&color_shader);
	gl_shader_code_shutdown(&gradient_shader);
	gl_shader_code_shutdown(&black_shader);
	gl_shader_code_shutdown(&bitmap_shader);
}

