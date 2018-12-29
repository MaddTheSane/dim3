/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Back Render Routines

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

GLuint						back_render_fbo_id,back_render_fbo_depth_stencil_id;
bool						back_render_on;

extern map_type				map;
extern setup_type			setup;
extern view_type			view;
extern render_info_type		render_info;

extern bool view_draw_node(node_type *node);

/* =======================================================

      Initialize Back Renderer
      
======================================================= */

void gl_back_render_initialize(void)
{
	GLint				old_fbo;
	
		// check if back rendering is enabled
		
	back_render_on=gl_check_frame_buffer_ok();
	if (!back_render_on) return;
	
	glGetIntegerv(GL_FRAMEBUFFER_BINDING,&old_fbo);
	
		// create depth buffer and stencil object
		
	glGenRenderbuffers(1,&back_render_fbo_depth_stencil_id);
	glBindRenderbuffer(GL_RENDERBUFFER,back_render_fbo_depth_stencil_id);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,back_render_texture_pixel_size,back_render_texture_pixel_size);

		// create the frame buffer object and attach depth/stencil

	glGenFramebuffers(1,&back_render_fbo_id);
	glBindFramebuffer(GL_FRAMEBUFFER,back_render_fbo_id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,back_render_fbo_depth_stencil_id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER,back_render_fbo_depth_stencil_id);

		// some additional setup
		
	gl_setup_context();

		// turn off framebuffer
		
	glBindFramebuffer(GL_FRAMEBUFFER,old_fbo);
}

void gl_back_render_shutdown(void)
{
	if (!back_render_on) return;

		// destroy frame buffer and depth/stencil

	glDeleteFramebuffers(1,&back_render_fbo_id);
	glDeleteRenderbuffers(1,&back_render_fbo_depth_stencil_id);
}

/* =======================================================

      Per Map Back rendering
      
======================================================= */

void gl_back_render_map_start(void)
{
	int				n;
	node_type		*node;

	if (!back_render_on) return;

	node=map.nodes;
	
	for (n=0;n!=map.nnode;n++) {
		node->back_render.txt_id=-1;
		node++;
	}
}

void gl_back_render_map_end(void)
{
	int				n;
	node_type		*node;

	if (!back_render_on) return;

	node=map.nodes;
	
	for (n=0;n!=map.nnode;n++) {
		if (node->back_render.txt_id!=-1) {
			glDeleteTextures(1,&node->back_render.txt_id);
		}
		node++;
	}
}

/* =======================================================

      Back Render Texture
      
======================================================= */

GLuint gl_back_render_create_texture(void)
{
#if !defined(D3_OS_IPHONE) && !defined(D3_OS_ANDRIOD)
	float			max;
#endif
	GLuint			gl_id;
	
	glGenTextures(1,&gl_id);
	gl_texture_bind(0,gl_id);
	
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,back_render_texture_pixel_size,back_render_texture_pixel_size,0,GL_RGBA,GL_UNSIGNED_BYTE,0);

		// mipmap
		
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		// anisotropic
		
#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.0);
#else
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&max);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,max);
#endif
	
	gl_texture_clear(0);
	
	return(gl_id);
}

/* =======================================================

      Per Frame Back rendering
      
======================================================= */

void gl_back_render_frame_node(char *node_name)
{
	int				node_idx;
	GLint			old_fbo;
	node_type		*node;

		// get node
		
	node_idx=map_find_node(&map,node_name);
	if (node_idx==-1) return;
	
	node=&map.nodes[node_idx];

		// alread rendered?

	if (node->back_render.render) return;
	
		// remember old fbo and viewport
		
	glGetIntegerv(GL_FRAMEBUFFER_BINDING,&old_fbo);
	
		// need to create a texture?

	if (node->back_render.txt_id==-1) {
		node->back_render.txt_id=gl_back_render_create_texture();
	}

		// setup fbo

	glBindFramebuffer(GL_FRAMEBUFFER,back_render_fbo_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,node->back_render.txt_id,0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,back_render_fbo_depth_stencil_id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER,back_render_fbo_depth_stencil_id);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE) {
		glBindFramebuffer(GL_FRAMEBUFFER,old_fbo);
		return;
	}
	
		// new viewport

	gl_save_viewport();
	gl_set_viewport(0,0,back_render_texture_pixel_size,back_render_texture_pixel_size);


		// draw back buffer

	view_draw_node(node);
	
	glFlush();
	
	fprintf(stdout,"render to %s.%d\n",node_name,node->back_render.txt_id);
	
		// restore viewport
	
	gl_restore_viewport();
	
		// restore fbo
		
	glBindFramebuffer(GL_FRAMEBUFFER,old_fbo);

		// generate mipmaps
		
	gl_texture_bind(0,node->back_render.txt_id);
	glGenerateMipmap(GL_TEXTURE_2D);
	gl_texture_clear(0);

		// mark as rendered for this frame
		
	node->back_render.render=TRUE;
}

void gl_back_render_frame_start(void)
{
	int					n,k;
	node_type			*node;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	map_liquid_type		*liq;
	
	if (!back_render_on) return;

		// flag rendering so we don't draw twice

	node=map.nodes;
	
	for (n=0;n!=map.nnode;n++) {
		node->back_render.render=FALSE;
		node++;
	}
	
		// run through all the meshes or liquids
	
	for (n=0;n!=view.render->draw_list.count;n++) {
		
		switch (view.render->draw_list.items[n].type) {
		
			case view_render_type_mesh:
				mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];
				if (!mesh->precalc_flag.poly_has_camera) break;
				
				poly=mesh->polys;

				for (k=0;k!=mesh->npoly;k++) {
					if (poly->camera[0]!=0x0) gl_back_render_frame_node(poly->camera);
					poly++;
				}
				break;
				
			case view_render_type_liquid:
				liq=&map.liquid.liquids[view.render->draw_list.items[n].idx];
				if (liq->camera[0]!=0x0) gl_back_render_frame_node(liq->camera);
				break;
		}
	}
}

/* =======================================================

      Back Render Texture
      
======================================================= */

bool gl_back_render_get_texture(char *node_name,GLuint *txt_id,float *alpha)
{
	int				node_idx;
	node_type		*node;
	
	if (node_name[0]==0x0) return(FALSE);
	fprintf(stdout,"check %s\n",node_name);
	

		// get node
		
	node_idx=map_find_node(&map,node_name);
	if (node_idx==-1) return(FALSE);
	
	node=&map.nodes[node_idx];
	
		// get back render texture
		
	if (!node->back_render.render) return(FALSE);

	*txt_id=node->back_render.txt_id;
	if (alpha!=NULL) *alpha=node->alpha;
	
	fprintf(stdout,"draw to %d\n",node->back_render.txt_id);
	
	return(TRUE);
}
