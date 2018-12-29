/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Render Setup

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
extern view_type			view;
extern setup_type			setup;
extern iface_type			iface;
extern render_info_type		render_info;

SDL_Window					*sdl_wind;
SDL_GLContext				*sdl_gl_ctx;

/* =======================================================

      Detect if in Windowed Mode
      
======================================================= */

bool gl_in_window_mode(void)
{
		// mobile always full screen,
		// everything else by settings
	
	#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)
		return(FALSE);
	#else
		return((!setup.full_screen) || (app.editor_override.on));
	#endif
}

/* =======================================================

      GL Context Settings
      
======================================================= */

void gl_setup_context(void)
{
		// regular setup
		
	glDisable(GL_DITHER);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	
#ifndef D3_OPENGL_ES
	glEnable(GL_LINE_SMOOTH);
#endif
	
		// hints

#ifndef D3_OPENGL_ES		
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	glHint(GL_TEXTURE_COMPRESSION_HINT,GL_NICEST);
#endif

	glHint(GL_GENERATE_MIPMAP_HINT,GL_NICEST);
}

/* =======================================================

      Open and Close OpenGL Context
      
======================================================= */

bool gl_initialize(char *err_str)
{
	int						screen_wid,screen_high,sdl_flags;
    GLint					ntxtsize;
#if defined(D3_OS_LINUX) || defined(D3_OS_WINDOWS)
	GLenum					glew_error;
#endif
#ifdef D3_OS_IPHONE
	const GLenum			discards[]={GL_DEPTH_ATTACHMENT,GL_STENCIL_ATTACHMENT};
#endif

		// reset sizes to the desktop
		// if they are at default
		
	if (setup.full_screen) {
		screen_wid=render_info.desktop.wid;
		screen_high=render_info.desktop.high;
	}
	else {
		screen_wid=dim3_window_wid;
		screen_high=dim3_window_high;
	}

		// setup rendering sizes

#ifndef D3_ROTATE_VIEW
	view.screen.x_sz=screen_wid;
	view.screen.y_sz=screen_high;
#else
	view.screen.x_sz=screen_high;
	view.screen.y_sz=screen_wid;
#endif

	view.screen.wide=gl_is_size_widescreen(view.screen.x_sz,view.screen.y_sz);
	
		// normal attributes
		
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,0);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	
#ifdef D3_OPENGL_ES
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
#endif

#ifdef D3_OS_IPHONE
	SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING,1);
#endif

		// full screen anti-aliasing attributes
		
	switch (setup.fsaa_mode) {
		case fsaa_mode_2x:
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,2);
			break;
		case fsaa_mode_4x:
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);
			break;
		case fsaa_mode_8x:
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,8);
			break;
	}
	
		// start window or full screen

	sdl_flags=SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN;
	if (!gl_in_window_mode()) sdl_flags|=(SDL_WINDOW_FULLSCREEN|SDL_WINDOW_BORDERLESS);
	
	sdl_wind=SDL_CreateWindow("dim3",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,screen_wid,screen_high,sdl_flags);
	if (sdl_wind==NULL) {
		sprintf(err_str,"SDL: Could not create window (Error: %s)",SDL_GetError());
		return(FALSE);
	}
	
	sdl_gl_ctx=SDL_GL_CreateContext(sdl_wind);

		// use glew on linux and windows
		
#if defined(D3_OS_LINUX) || defined(D3_OS_WINDOWS)
	glew_error=glewInit();
	if (glew_error!=GL_NO_ERROR) {
		strcpy(err_str,glewGetErrorString(glew_error));
		return(FALSE);
	}
#endif

		// grab openGL attributes
		
	strncpy(render_info.name,(char*)glGetString(GL_RENDERER),64);
	render_info.name[63]=0x0;
	
	strncpy(render_info.ext_string,(char*)glGetString(GL_EXTENSIONS),8192);
	render_info.ext_string[8191]=0x0;
			
	glGetIntegerv(GL_MAX_TEXTURE_SIZE,&ntxtsize);
	render_info.texture_max_size=(int)ntxtsize;
	
		// stick refresh rate to 60

	render_info.monitor_refresh_rate=60;

#ifndef D3_ROTATE_VIEW
	glViewport(0,0,view.screen.x_sz,view.screen.y_sz);
#else
	glViewport(0,0,view.screen.y_sz,view.screen.x_sz);
#endif

	gl_setup_context();
	
#ifndef D3_OPENGL_ES
	if (setup.fsaa_mode!=fsaa_mode_none) glEnable(GL_MULTISAMPLE);
#endif

        // clear the entire window so it doesn't flash
        
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
#ifdef D3_OS_IPHONE
	glDiscardFramebufferEXT(GL_FRAMEBUFFER,2,discards);
#endif

	SDL_GL_SwapWindow(sdl_wind);	

		// texture utility initialize
		
	gl_texture_initialize();
	
	return(TRUE);
}
	
void gl_shutdown(void)
{
		 // shut down textures
		 
	gl_texture_shutdown();
	
		// close context
		
	SDL_GL_DeleteContext(sdl_gl_ctx);
	SDL_DestroyWindow(sdl_wind);
}

/* =======================================================

      Check Widescreen
      
======================================================= */

bool gl_is_size_widescreen(int wid,int high)
{
	return(((float)high/(float)wid)<=0.625f);
}

