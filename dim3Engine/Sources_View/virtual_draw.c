/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Virtual Control Draw Routines

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
#include "objects.h"

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern render_info_type		render_info;

/* =======================================================

      Draw Virtual Sticks
      
======================================================= */

void virtual_stick_draw(iface_virtual_stick_type *stick)
{
	int					x,y,sx,sy,lft,rgt,top,bot,
						radius;
	float				f_outer_radius;
	d3vct				vct;
	bitmap_type			*bitmap;

	if (!stick->use_bitmap) return;
	
		// outer stick

	rgt=stick->pnt.x+stick->size.x;
	bot=stick->pnt.y+stick->size.y;
	
	bitmap=view_images_get_bitmap(stick->outer_image_idx);
	view_primitive_2D_texture_quad(bitmap->gl_id,&stick->color,1.0f,stick->pnt.x,rgt,stick->pnt.y,bot,0.0f,1.0f,0.0f,1.0f,TRUE);

		// inner stick location
		
	sx=stick->size.x>>1;
	sy=stick->size.y>>1;

	x=(int)(((float)sx)*stick->touch_x);
	y=(int)(((float)sy)*stick->touch_y);
	
		// make sure it's within circle
		
	radius=(int)sqrtf((float)((x*x)+(y*y)));
	
	vct.x=(float)x;
	vct.y=(float)y;
	vct.z=0.0f;
	vector_normalize(&vct);
		
	f_outer_radius=(float)(sx-(sx>>1));
	x=(int)(vct.x*f_outer_radius);
	y=(int)(vct.y*f_outer_radius);

	lft=(stick->pnt.x+sx)+x;
	lft-=(sx>>1);
	rgt=lft+sx;
	
	top=(stick->pnt.y+sy)+y;
	top-=(sy>>1);
	bot=top+sy;
	
	bitmap=view_images_get_bitmap(stick->inner_image_idx);
	view_primitive_2D_texture_quad(bitmap->gl_id,&stick->color,1.0f,lft,rgt,top,bot,0.0f,1.0f,0.0f,1.0f,TRUE);
}

/* =======================================================

      Draw Virtual Buttons
      
======================================================= */

void virtual_button_draw(iface_virtual_button_type *button)
{
	int					rgt,bot;
	bitmap_type			*bitmap;

	if (!button->use_bitmap) return;
	
	rgt=button->pnt.x+button->size.x;
	bot=button->pnt.y+button->size.y;
	
	if (button->down) {
		bitmap=view_images_get_bitmap(button->down_image_idx);
	}
	else {
		bitmap=view_images_get_bitmap(button->up_image_idx);
	}
	
	view_primitive_2D_texture_quad(bitmap->gl_id,&button->color,1.0f,button->pnt.x,rgt,button->pnt.y,bot,0.0f,1.0f,0.0f,1.0f,TRUE);
}

/* =======================================================

      Draw Virtual Controls
      
======================================================= */

void virtual_control_draw(void)
{
	int				n;
	
		// set up view
		
	gl_2D_view_interface();
	
	glDisable(GL_DEPTH_TEST);
	
		// draw the virtual controls
	
	for (n=0;n!=max_virtual_stick;n++) {
		if ((iface.virtual_control.sticks[n].on) && (iface.virtual_control.sticks[n].show)) virtual_stick_draw(&iface.virtual_control.sticks[n]);
	}
	
	for (n=0;n!=max_virtual_button;n++) {
		if ((iface.virtual_control.buttons[n].on) && (iface.virtual_control.buttons[n].show)) virtual_button_draw(&iface.virtual_control.buttons[n]);
	}
}

