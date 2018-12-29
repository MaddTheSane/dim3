/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: OpenGL Frame Routines

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

extern app_type				app;
extern map_type				map;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern render_info_type		render_info;
extern camera_type			camera;

extern SDL_Window			*sdl_wind;

float						gl_proj_matrix[16];

/* =======================================================

      Frame Clear and Swap
      
======================================================= */

void gl_frame_clear(void)
{
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void gl_frame_swap(void)
{
#ifdef D3_OS_IPHONE
	const GLenum		discards[]={GL_DEPTH_ATTACHMENT,GL_STENCIL_ATTACHMENT};
#endif
	d3col				col;
	
		// is this app deactivated?

	if (app.state==as_inactive) {
		gl_2D_view_screen();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		
		col.r=col.g=col.b=0.0f;
		view_primitive_2D_tint_screen(&col,0.5f);
	}
	
		// discard the depth and stencil

#ifdef D3_OS_IPHONE
	glDiscardFramebufferEXT(GL_FRAMEBUFFER,2,discards);
#endif

		// swap buffer

	SDL_GL_SwapWindow(sdl_wind);
}

/* =======================================================

      2D Projections
      
======================================================= */

void gl_2D_view_screen(void)
{
	matrix_type			mat;
	
		// create the projection matrix
		// as a column ordered opengl matrix

		// model view matrix is never required
				
#ifndef D3_ROTATE_VIEW
	matrix_ortho(&mat,0.0f,(float)view.screen.x_sz,(float)view.screen.y_sz,0.0f,-1.0f,1.0f);
#else
	matrix_ortho(&mat,0.0f,(float)view.screen.y_sz,(float)view.screen.x_sz,0.0f,-1.0f,1.0f);
	matrix_translate(&mat,(float)view.screen.y_sz,0.0f,0.0f);
	matrix_rotate(&mat,90.0f,0.0f,0.0f,1.0f);
#endif

	matrix_to_opengl_uniform_4x4(&mat,gl_proj_matrix);
}

void gl_2D_view_interface(void)
{
	matrix_type		mat;
	
		// create the projection matrix
		// as a column ordered opengl matrix

		// model view matrix is never required
				
#ifndef D3_ROTATE_VIEW
	matrix_ortho(&mat,0.0f,(float)iface.scale_x,(float)iface.scale_y,0.0f,-1.0f,1.0f);
#else
	matrix_ortho(&mat,0.0f,(float)iface.scale_y,(float)iface.scale_x,0.0f,-1.0f,1.0f);
	matrix_translate(&mat,(float)iface.scale_y,0.0f,0.0f);
	matrix_rotate(&mat,90.0f,0.0f,0.0f,1.0f);
#endif

	matrix_to_opengl_uniform_4x4(&mat,gl_proj_matrix);
}

/* =======================================================

      Scissoring
      
======================================================= */

void gl_2D_scissor_start(int lx,int rx,int ty,int by)
{
	lx=(view.screen.x_sz*(lx))/iface.scale_x;
	rx=(view.screen.x_sz*(rx))/iface.scale_x;
	ty=(view.screen.y_sz*(ty))/iface.scale_y;
	by=(view.screen.y_sz*(by))/iface.scale_y;

	glEnable(GL_SCISSOR_TEST);
	
#ifndef D3_ROTATE_VIEW
	glScissor(lx,(view.screen.y_sz-by),(rx-lx),(by-ty));
#else
	glScissor((view.screen.y_sz-by),lx,(by-ty),(rx-lx));
#endif
}

void gl_2D_scissor_end(void)
{
	glDisable(GL_SCISSOR_TEST);
}

