/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Tinting

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

int							tint_start_tick,
							tint_fade_in_tick,
							tint_life_tick,
							tint_fade_out_tick;
float						tint_alpha;
d3col						tint_col;

extern map_type				map;
extern setup_type			setup;

/* =======================================================

      Liquid Tint Drawing
      
======================================================= */

void view_draw_liquid_tint(int liquid_idx)
{
	map_liquid_type		*liq;
	
		// under liquid
		
	if (liquid_idx==-1) return;
	
		// get the liquid
		
	liq=&map.liquid.liquids[liquid_idx];
	
		// draw tint

	gl_2D_view_screen();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	view_primitive_2D_tint_screen(&liq->col,liq->tint_alpha);
}

/* =======================================================

      View Tint Drawing
      
======================================================= */

void view_draw_tint_start(d3col *col,float alpha,int fade_in_msec,int life_msec,int fade_out_msec)
{
	if (fade_in_msec<=0) fade_in_msec=1;
	if (life_msec<=0) life_msec=1;
	if (fade_out_msec<=0) fade_out_msec=1;

    tint_start_tick=game_time_get();
    tint_fade_in_tick=fade_in_msec;
    tint_life_tick=life_msec;
	tint_fade_out_tick=fade_out_msec;
	tint_alpha=alpha;
    
	memmove(&tint_col,col,sizeof(d3col));
}

void view_draw_effect_tint(void)
{
	int				tick;
	float			alpha;
	
		// is flash on?
		
	if (tint_start_tick==-1) return;
	
		// is flash over?
		
	tick=game_time_get()-tint_start_tick;
		
	if (tick>=(tint_fade_in_tick+tint_life_tick+tint_fade_out_tick)) {
		tint_start_tick=-1;
		return;
	}
	
		// get color and alpha
		
	if (tick>(tint_fade_in_tick+tint_life_tick)) {
		alpha=1.0f-((float)(tick-(tint_fade_in_tick+tint_life_tick))/(float)tint_fade_out_tick);
	}
	else {
		if (tick>tint_fade_in_tick) {
			alpha=1.0f;
		}
		else {
			alpha=(float)tick/(float)tint_fade_in_tick;
		}
	}
	
		// draw tint
		
	gl_2D_view_screen();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	view_primitive_2D_tint_screen(&tint_col,(alpha*tint_alpha));
}

