/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: RTL to OpenGL

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
#include "ray_interface.h"

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern iface_type			iface;

/* =======================================================

      dim3RTL OpenGL Texture Buffer
      
======================================================= */

bool view_dim3rtl_create_opengl_texture(GLuint *p_gl_id,int wid,int high,char *err_str)
{
	int					n,sz;
	GLuint				gl_id;
	unsigned char		*data,*dptr;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1,&gl_id);
	glBindTexture(GL_TEXTURE_2D,gl_id);

	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	sz=(wid*4)*high;
	data=malloc(sz);
	if (data==NULL) {
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}
	bzero(data,sz);

	dptr=data;
	for (n=0;n!=(sz/4);n++) {
		*dptr++=0xFF;
		*dptr++=0x0;
		*dptr++=0x0;
		*dptr++=0xFF;
	}

	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,wid,high,0,GL_RGBA,GL_UNSIGNED_BYTE,data);

	free(data);

	glBindTexture(GL_TEXTURE_2D,0);
	
	*p_gl_id=gl_id;
	return(TRUE);
}

/* =======================================================

      Transfer Buffer to Screen
      
======================================================= */

void view_dim3rtl_transfer_texture_to_view(int scene_id,int x,int y,int wid,int high,GLuint gl_id)
{
		// draws on 2D screen

	gl_2D_view_screen();

		// draw the quad

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,gl_id);

	view_primitive_2D_texture_quad(gl_id,NULL,1.0f,x,(x+wid),y,(y+high),0.0f,1.0f,0.0f,1.0f,TRUE);
}

