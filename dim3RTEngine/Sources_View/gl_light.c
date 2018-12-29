/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: OpenGL Lights

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

extern map_type				map;
extern view_type			view;
extern server_type			server;
extern camera_type			camera;
extern setup_type			setup;

float						light_flicker_value[64]={
														1.00,1.00,1.00,1.00,1.00,1.00,1.00,1.00,
														0.92,0.85,0.70,0.85,0.92,1.00,0.92,0.85,
														0.92,1.00,1.00,1.00,1.00,0.92,0.85,0.70,
														0.50,0.70,0.85,0.70,0.85,0.92,1.00,1.00,
														1.00,1.00,1.00,1.00,0.92,0.85,0.70,0.85,
														0.70,0.85,0.92,1.00,1.00,0.92,0.85,0.70,
														0.50,0.70,0.85,0.92,1.00,1.00,0.92,1.00,
														0.92,0.85,0.70,0.85,0.92,1.00,1.00,1.00
													},
													
							light_fail_value[64]=	{
														0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
														0.00,0.15,0.45,0.15,0.00,0.00,0.00,0.15,
														0.00,0.00,0.00,0.00,0.00,0.00,0.15,0.45,
														0.85,0.45,0.15,0.45,0.15,0.00,0.00,0.00,
														0.00,0.00,0.00,0.00,0.00,0.15,0.45,0.15,
														0.45,0.15,0.00,0.00,0.00,0.00,0.15,0.45,
														0.85,0.45,0.15,0.00,0.00,0.00,0.00,0.00,
														0.00,0.15,0.45,0.15,0.00,0.00,0.00,0.00
													};
		
// supergumba -- move this somewhere else

/* =======================================================

      Get Light Intensity For Type
      
======================================================= */

int gl_light_get_intensity(int tick,int light_type,int intensity)
{
	switch (light_type) {
	
		case lt_normal:
			return(intensity);
	
		case lt_blink:
			if ((tick&0x100)==0) return(intensity);
			return(0);
			
		case lt_glow:
			tick=tick&0x7FF;
			if (tick>1024) tick=2048-tick;
			return((int)((float)intensity*(0.75f+(((float)tick)*0.0003f))));
			
		case lt_pulse:
			tick=tick&0x3FF;
			if (tick>512) tick=1024-tick;
			return((int)((float)intensity*(0.5f+(((float)tick)*0.001f))));
			
		case lt_flicker:
			return((int)((float)intensity*light_flicker_value[(tick>>7)&0x3F]));
			break;
			
		case lt_failing:
			return((int)((float)intensity*light_fail_value[(tick>>7)&0x3F]));
			break;
	}
	
	return(intensity);
}


