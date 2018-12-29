/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Fading

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

int							fade_screen_tick,fade_screen_msec;
bool						fade_screen_in;

extern iface_type			iface;
extern view_type			view;
extern setup_type			setup;

/* =======================================================

      Start Screen Fade
      
======================================================= */

void view_fade_start(void)
{
	if (iface.fade.map_msec<=0) {
		fade_screen_tick=-1;
		return;
	}

	fade_screen_tick=game_time_get();
	fade_screen_msec=iface.fade.map_msec;
	fade_screen_in=TRUE;
}

void view_fade_cinema_fade_in_start(int msec)
{
	fade_screen_tick=game_time_get();
	fade_screen_msec=msec;
	fade_screen_in=TRUE;
}

void view_fade_cinema_fade_out_start(int msec)
{
	fade_screen_tick=game_time_get();
	fade_screen_msec=msec;
	fade_screen_in=FALSE;
}

void view_fade_cancel(void)
{
	fade_screen_tick=-1;
}

/* =======================================================

      Run Screen Fade
      
======================================================= */

void view_fade_draw(void)
{
	int				tick;
	float			alpha;
	d3col			col;

	if (fade_screen_tick==-1) return;

	tick=game_time_get()-fade_screen_tick;
	if (tick>fade_screen_msec) {
		fade_screen_tick=-1;
		return;
	}

		// calculate fade

	alpha=((float)tick)/((float)fade_screen_msec);
	if (fade_screen_in) alpha=1.0f-alpha;
	
		// draw
		
	gl_2D_view_screen();

	col.r=col.g=col.b=0.0f;
	view_primitive_2D_color_quad(&col,alpha,0,view.screen.x_sz,0,view.screen.y_sz);
}

