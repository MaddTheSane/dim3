/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Cursor Routines

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

extern iface_type			iface;
extern setup_type			setup;
extern file_path_setup_type	file_path_setup;

int							cursor_image_idx;

/* =======================================================

      Cursor Stubs if Mobile
      
======================================================= */

#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)

void cursor_initialize(void) {}
void cursor_shutdown(void) {}
void cursor_draw(void) {}

#else

/* =======================================================

      Cursor Initialize and Shutdown
      
======================================================= */

void cursor_initialize(void)
{
	char		path[1024];
	
	file_paths_data(&file_path_setup,path,"Bitmaps/UI_Elements","cursor","png");
	cursor_image_idx=view_images_load_single(path,FALSE,TRUE);
}

void cursor_shutdown(void)
{
	view_images_free_single(cursor_image_idx);
}

/* =======================================================

      Crosshair Draw
      
======================================================= */

void cursor_draw(void)
{
	int				x,y,sz,lft,top,rgt,bot;
	GLuint			gl_id;
	
		// get cursor position
		
	input_gui_get_position(&x,&y);

	sz=(int)(((float)iface.scale_x)*cursor_size_factor);
		
	lft=x;
	rgt=lft+sz;
	top=y;
	bot=top+sz;

		// draw cursor
		
	gl_id=view_images_get_gl_id(cursor_image_idx);
	view_primitive_2D_texture_quad(gl_id,NULL,1.0f,lft,rgt,top,bot,0.0f,1.0f,0.0f,1.0f,TRUE);
}

#endif


