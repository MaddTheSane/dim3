/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Texture Routines

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
extern server_type			server;
extern setup_type			setup;
extern render_info_type		render_info;

bitmap_type					lmap_black_bitmap,lmap_white_bitmap;

int							gl_texture_current_active;
GLuint						gl_texture_current_binds[5];

/* =======================================================

      Texture Utility Initialization
      
======================================================= */

void gl_texture_initialize(void)
{
	int				n;
	d3col			col;

		// a all black bitmap for polygons
		// missing a lightmap

	col.r=col.b=col.g=0.0f;
	bitmap_color(&lmap_black_bitmap,&col);

		// an all white bitmap for debug mode

	col.r=col.b=col.g=1.0f;
	bitmap_color(&lmap_white_bitmap,&col);

		// we keep track of the active
		// texture and all bound textures
		// to reduce the number of bindings
		
	glActiveTexture(GL_TEXTURE0);
	gl_texture_current_active=0;

	for (n=0;n!=5;n++) {
		gl_texture_current_binds[n]=-1;
	}
}

void gl_texture_shutdown(void)
{
	bitmap_close(&lmap_black_bitmap);
	bitmap_close(&lmap_white_bitmap);
}

/* =======================================================

      Texture Binding
      
======================================================= */

void gl_texture_bind(int unit,GLuint txt_id)
{
	if (gl_texture_current_binds[unit]==txt_id) return;

	gl_texture_current_binds[unit]=txt_id;

	if (gl_texture_current_active!=unit) {
		gl_texture_current_active=unit;
		glActiveTexture(GL_TEXTURE0+unit);
	}
	
	glBindTexture(GL_TEXTURE_2D,txt_id);
}

void gl_texture_clear(int unit)
{
	gl_texture_bind(unit,0);
	gl_texture_current_binds[unit]=-1;
}

