/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: HUD Radar Routines

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
#include "objects.h"

extern server_type			server;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern render_info_type		render_info;

/* =======================================================

      Draw HUD Radar
      
======================================================= */

void radar_draw(void)
{
	int						n,tick,x,y,lx,rx,ty,by,
							dist,max_dist,fade_dist,fade_count,radar_sz;
	unsigned long			gl_id;
	float					alpha,fade_mult;
	d3col					tint;
	obj_type				*obj,*player_obj;
	iface_radar_icon_type	*icon;
	
		// radar on?
		
	if (!iface.radar.on) return;
	
		// get player object (center of radar)
		
	player_obj=server.obj_list.objs[server.player_obj_idx];

		// set up view
		
	gl_2D_view_interface();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glDisable(GL_DEPTH_TEST);

		// get radar size

	radar_sz=iface.radar.display_radius;

	lx=iface.radar.pnt.x-radar_sz;
	ty=iface.radar.pnt.y-radar_sz;
	rx=lx+(radar_sz<<1);
	by=ty+(radar_sz<<1);

		// draw background
		
	gl_id=view_images_get_gl_id(iface.radar.background_image_idx);
	view_primitive_2D_texture_quad(gl_id,NULL,1.0f,lx,rx,ty,by,0.0f,1.0f,0.0f,1.0f,TRUE);

		// ticks for fades

	tick=game_time_get();

		// draw radar objects
		
	tint.r=tint.g=tint.b=1.0f;
		
	max_dist=iface.radar.view_radius;
	fade_dist=max_dist>>1;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (obj->idx==server.player_obj_idx) continue;
		if ((obj->hidden) || (!obj->radar.on) || (obj->radar.icon_idx==-1)) continue;
			
			// get distance
			
		dist=distance_2D_get(obj->pnt.x,obj->pnt.z,player_obj->pnt.x,player_obj->pnt.z);
		if ((!obj->radar.always_visible) && (dist>max_dist)) continue;

			// motion only?

		fade_mult=1.0f;
			
		if (obj->radar.motion_only) {

				// if moving, turn on the fade count
				// for when it stops

			if ((obj->forward_move.moving) || (obj->vert_move.moving)) {
				obj->radar.fade_start_tick=tick;
			}

				// if not moving, handle the fading

			else {
				if (iface.radar.no_motion_fade==0) continue;

				fade_count=tick-obj->radar.fade_start_tick;
				if (fade_count>iface.radar.no_motion_fade) continue;

				fade_mult=1.0f-(((float)fade_count)/((float)iface.radar.no_motion_fade));
			}
		}

			// get the position

		x=obj->pnt.x-player_obj->pnt.x;
		y=-(obj->pnt.z-player_obj->pnt.z);
			
		if (iface.radar.rot) rotate_2D_point_center(&x,&y,player_obj->ang.y);

			// if outside max, stick to edge

		if (dist>max_dist) {
			x=iface.radar.pnt.x+((x*radar_sz)/dist);
			y=iface.radar.pnt.y-((y*radar_sz)/dist);
		}
		else {
			x=iface.radar.pnt.x+((x*radar_sz)/max_dist);
			y=iface.radar.pnt.y-((y*radar_sz)/max_dist);
		}

			// get alpha

		alpha=1.0f;
		if ((!obj->radar.always_visible) && (dist>fade_dist)) {
			alpha=1.0f-(float)(dist-fade_dist)/(float)fade_dist;
		}

		alpha*=fade_mult;
			
			// draw object
		
		icon=&iface.radar.icons[obj->radar.icon_idx];

		lx=x-icon->size;
		rx=x+icon->size;
		ty=y-icon->size;
		by=y+icon->size;

			// draw icon
		
		gl_id=view_images_get_gl_id(icon->image_idx);

		if (icon->rot) {
			view_primitive_2D_texture_quad_rot(gl_id,&tint,alpha,lx,rx,ty,by,obj->ang.y,0.0f,1.0f,0.0f,1.0f);
		}
		else {
			view_primitive_2D_texture_quad(gl_id,&tint,alpha,lx,rx,ty,by,0.0f,1.0f,0.0f,1.0f,TRUE);
		}
	}
}

