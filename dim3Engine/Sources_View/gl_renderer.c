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
		return((setup.window) || ((app.editor_override.on) && (setup.window_editor)));
	#endif
}

/* =======================================================

      GL Context Settings
      
======================================================= */

void gl_setup_context(void)
{
		// regular setup
		
	glDisable(GL_DITHER);

	glDepthFunc(GL_LEQUAL);
	
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

bool gl_initialize(int screen_wid,int screen_high,int fsaa_mode,char *err_str)
{
	int						sdl_flags;
    GLint					ntxtsize;
#if defined(D3_OS_LINUX) || defined(D3_OS_WINDOWS)
	GLenum					glew_error;
#endif
#ifdef D3_OS_IPHONE
	const GLenum			discards[]={GL_DEPTH_ATTACHMENT,GL_STENCIL_ATTACHMENT};
#endif

		// reset sizes to the desktop
		// if they are at default
		
	if ((screen_wid==-1) || (screen_high==-1)) {
		screen_wid=render_info.desktop.wid;
		screen_high=render_info.desktop.high;
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
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	
#ifdef D3_OPENGL_ES
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
#endif

#ifdef D3_OS_IPHONE
	SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING,1);
#endif

		// full screen anti-aliasing attributes
		
	switch (fsaa_mode) {
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
	
	if (!gl_check_initialize(err_str)) return(FALSE);
	
		// stick refresh rate to 60

	render_info.monitor_refresh_rate=60;

#ifndef D3_ROTATE_VIEW
	gl_set_viewport(0,0,view.screen.x_sz,view.screen.y_sz);
#else
	gl_set_viewport(0,0,view.screen.y_sz,view.screen.x_sz);
#endif

	gl_setup_context();
	
#ifndef D3_OPENGL_ES
	if (fsaa_mode!=fsaa_mode_none) glEnable(GL_MULTISAMPLE);
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

/* =======================================================

      ScreenShots
      
======================================================= */

bool gl_screen_shot(int lft_x,int top_y,int wid,int high,bool thumbnail,char *path)
{
	int					x,y,ss_wid,ss_high,dsz;
	float				x_off,y_off,x_add,y_add;
	unsigned char		*pixel_buffer,*data,*sptr,*dptr,*s2ptr,*d2ptr;
	bool				ok;
	
	pixel_buffer=(unsigned char*)malloc((wid*3)*high);
	if (pixel_buffer==NULL) return(FALSE);
	
	glReadPixels(lft_x,top_y,wid,high,GL_RGB,GL_UNSIGNED_BYTE,pixel_buffer);
	
		// is this is a thumbnail,
		// then reduce the picture (but keep
		// the dimensions)
		
	x_add=y_add=1.0f;
	ss_wid=wid;
	ss_high=high;

	dsz=(wid*3)*high;
	
	if (thumbnail) {
		x_add=((float)wid)/512.0f;
		ss_wid=512;

		ss_high=(high*512)/wid;
		y_add=((float)high)/((float)ss_high);

		dsz=(ss_wid*3)*ss_high;
	}
	
		// flip the data
		
	data=(unsigned char*)malloc(dsz);
	if (data==NULL) {
		free(pixel_buffer);
		return(FALSE);
	}
	
	bzero(data,dsz);

	y_off=0.0f;
	
	dptr=data+((ss_high-1)*(ss_wid*3));

	for (y=0;y!=ss_high;y++) {

		x_off=0.0f;
	
		sptr=pixel_buffer+(((int)y_off)*(wid*3));
		d2ptr=dptr;
		
		for (x=0;x!=ss_wid;x++) {

			s2ptr=sptr+(((int)x_off)*3);

			*d2ptr++=*s2ptr;
			*d2ptr++=*(s2ptr+1);
			*d2ptr++=*(s2ptr+2);
			
			x_off+=x_add;
		}
			
		dptr-=(ss_wid*3);
		y_off+=y_add;
	}

	free(pixel_buffer);

		// save screenshot

	ok=bitmap_write_png_data(data,ss_wid,ss_high,FALSE,path);
		
	free(data);
	
	return(ok);
}
	
