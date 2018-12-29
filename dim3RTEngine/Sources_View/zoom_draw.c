/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Zoom Setup/Drawing

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
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;

/* =======================================================

      Zoom Setup
      
======================================================= */

void zoom_setup(obj_type *obj,weapon_type *weap)
{
	int					tx,ty,old_x,old_y;
	obj_zoom_draw		*zoom_draw;
	
	zoom_draw=&obj->zoom_draw;
	
		// get last position is zoom was on
		
	old_x=old_y=-1;
	
	if (zoom_draw->on) {
		old_x=zoom_draw->x;
		old_y=zoom_draw->y;
	}

		// zoom on?
		
	zoom_draw->on=FALSE;
	
	if (obj->hide_all_weapons) return;
	if ((!weap->zoom.on) || (weap->zoom.mode==zoom_mode_off)) return;
	
		// iron sites effect masking
				
	if (game_time_get()<(zoom_draw->start_tick+weap->zoom.tick)) return;
		
		// get the zoom place
		
	if (!crosshair_get_location(obj,weap,&tx,&ty,NULL,NULL)) return;
	
	zoom_draw->on=TRUE;
	
		// smooth out
	
	if (old_x!=-1) {
		tx=(tx+old_x)>>1;
		ty=(ty+old_y)>>1;
	}
		
	zoom_draw->x=tx;
	zoom_draw->y=ty;
}

/* =======================================================

      Zoom Drawing
      
======================================================= */

void zoom_draw(obj_type *obj,weapon_type *weap)
{
	int				x,y,sz,lft,rgt,top,bot;
	GLuint			gl_id;
	d3col			col;
	
		// is weapon not in zoom?
		
	if ((!weap->zoom.on) || (weap->zoom.mode==zoom_mode_off)) return;
	
		// is there a mask?
		
	if (!obj->zoom_draw.on) return;
	if (weap->zoom.mask_idx==-1) return;
		
		// position
		
	x=obj->zoom_draw.x;
	y=obj->zoom_draw.y;
	
	sz=view.screen.y_sz>>1;

	lft=x-sz;
	rgt=x+sz;
	top=y-(sz+1);
	bot=y+(sz+1);
	
		// scope bitmap
		
	gl_id=view_images_get_gl_id(iface.crosshair_list.crosshairs[weap->zoom.mask_idx].image_idx);
	
		// setup drawing
		
	glDisable(GL_BLEND);
	
		// borders

	col.r=col.g=col.b=0.0f;
		
	if (lft>0) view_primitive_2D_color_quad(&col,1.0f,0,(lft+1),0,view.screen.y_sz);
	if (rgt<view.screen.x_sz) view_primitive_2D_color_quad(&col,1.0f,(rgt-1),view.screen.x_sz,0,view.screen.y_sz);
	if (top>0) view_primitive_2D_color_quad(&col,1.0f,0,view.screen.x_sz,0,(top+1));
	if (bot<view.screen.y_sz) view_primitive_2D_color_quad(&col,1.0f,0,view.screen.x_sz,(bot-1),view.screen.y_sz);

		// zoom

	col.r=col.g=col.b=1.0f;
	
	view_primitive_2D_texture_quad(gl_id,&col,1.0f,lft,rgt,top,bot,0.0f,1.0f,0.0f,1.0f,TRUE);
}

