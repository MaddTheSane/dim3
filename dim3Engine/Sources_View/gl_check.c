/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Check Rendering Features

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

bool						gl_check_value_frame_buffer,
							gl_check_value_fsaa,
							gl_check_value_texture_generate_mipmaps,
							gl_check_value_npot_textures;

extern setup_type			setup;
extern render_info_type		render_info;

/* =======================================================

      GL Checks Setup
      
======================================================= */

bool gl_check_initialize(char *err_str)
{
#ifndef D3_OPENGL_ES
	bool			shader_ok;
#endif
#ifdef D3_OS_MAC
	GLint			fragGPU,vertGPU;
#endif

		// determine if shaders are active as
		// they are required.  Shaders are just part
		// of the ES2 spec

#ifndef D3_OPENGL_ES

	shader_ok=TRUE;

		// check GL string
		
	if (strstr(render_info.ext_string,"GL_ARB_shader_objects")==NULL) shader_ok=FALSE;
	if (strstr(render_info.ext_string,"GL_ARB_fragment_shader")==NULL) shader_ok=FALSE;
	if (strstr(render_info.ext_string,"GL_ARB_vertex_shader")==NULL) shader_ok=FALSE;
	if (strstr(render_info.ext_string,"GL_ARB_shading_language_100")==NULL) shader_ok=FALSE;
	
		// check if GPU will actually handle it
		
#ifdef D3_OS_MAC
	CGLGetParameter(CGLGetCurrentContext(),kCGLCPGPUFragmentProcessing,&fragGPU);
	CGLGetParameter(CGLGetCurrentContext(),kCGLCPGPUVertexProcessing,&vertGPU);
	shader_ok=(fragGPU && vertGPU);
#endif

	if (!shader_ok) {
		strcpy(err_str,"A GPU capable of running shader programs are required to for dim3");
		return(FALSE);
	}

#endif

		// cache optional values
		// luckily, in ES2, a lot of these are just
		// part of the spec

#ifndef D3_OPENGL_ES

	gl_check_value_frame_buffer=FALSE;
	if (strstr(render_info.ext_string,"GL_EXT_framebuffer_object")!=NULL) {
		gl_check_value_frame_buffer=(strstr(render_info.ext_string,"GL_EXT_packed_depth_stencil")!=NULL);
	}

	gl_check_value_texture_generate_mipmaps=(strstr(render_info.ext_string,"GL_SGIS_generate_mipmap")!=NULL);
	gl_check_value_npot_textures=(strstr(render_info.ext_string,"GL_ARB_texture_non_power_of_two")!=NULL);

#else
	gl_check_value_frame_buffer=TRUE;
	gl_check_value_texture_generate_mipmaps=TRUE;
	gl_check_value_npot_textures=TRUE;
#endif

		// generic checks

	gl_check_value_fsaa=(strstr(render_info.ext_string,"GL_ARB_multisample")!=NULL);

	return(TRUE);
}

/* =======================================================

      GL Checks
      
======================================================= */

bool gl_check_frame_buffer_ok(void)
{
	return(gl_check_value_frame_buffer);
}

bool gl_check_fsaa_ok(void)
{
	return(gl_check_value_fsaa);
}

bool gl_check_texture_generate_mipmaps_ok(void)
{
	return(gl_check_value_texture_generate_mipmaps);
}

bool gl_check_npot_textures_ok(void)
{
	return(gl_check_value_npot_textures);
}
