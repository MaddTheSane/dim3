/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Progress Routines

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

extern app_type				app;
extern map_type				map;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern file_path_setup_type	file_path_setup;

int							progress_start_tick,progress_cur_tick;
bool						progress_on;
bitmap_type					progress_background_bitmap,progress_bitmap;

/* =======================================================

      Setup Progress
      
======================================================= */

void progress_initialize(char *map_name)
{
	char			path[1024];
	bool			bitmap_ok;

		// ignore if dedicated host

	if (app.dedicated_host) return;
	
		// current progress
		
	progress_start_tick=-1;
	progress_cur_tick=0;
	
		// check for map progress background,
		// otherwise use default
		
	bitmap_ok=FALSE;
		
	if (map_name!=NULL) {
		file_paths_data(&file_path_setup,path,"Bitmaps/Backgrounds_Map",map_name,"png");
		bitmap_ok=bitmap_open(&progress_background_bitmap,path,FALSE,FALSE,FALSE,TRUE,FALSE,FALSE);
	}
	
	if (!bitmap_ok) {
		view_file_paths_bitmap_check_wide(path,"Bitmaps/Backgrounds","load");
		bitmap_open(&progress_background_bitmap,path,FALSE,FALSE,FALSE,TRUE,FALSE,FALSE);
	}

		// progress bitmap
		
	progress_on=TRUE;

	if (iface.progress.bitmap_name[0]!=0x0) {
		file_paths_data(&file_path_setup,path,"Bitmaps/Interface",iface.progress.bitmap_name,"png");
		progress_on=bitmap_open(&progress_bitmap,path,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE);
	}
}

void progress_shutdown(void)
{
		// ignore if dedicated host

	if (app.dedicated_host) return;

		// close bitmaps

	bitmap_close(&progress_background_bitmap);
	if (progress_on) bitmap_close(&progress_bitmap);
}

/* =======================================================

      Draw Progress
      
======================================================= */

void progress_update(void)
{
	int				lft,rgt,top,bot,
					time_tick;
	float			gx,gy,g_size;

		// ignore if dedicated host

	if (app.dedicated_host) return;
	
		// first time, get timing info
		// using direct timer as game is paused
		
	if (progress_start_tick==-1) progress_start_tick=time_get();

		// we don't want to skip frames
		// so we cheat with the progress timing

	time_tick=time_get();

	if ((time_tick-progress_start_tick)>=iface.progress.animate.msec) {
		progress_cur_tick+=iface.progress.animate.msec;
		progress_start_tick=time_tick+((time_tick-progress_start_tick)%iface.progress.animate.msec);
	}
	
		// start the frame
	
	gl_frame_clear();
	gl_2D_view_interface();

		// draw background
		// progress can be called while baseutility loads bitmaps, need to reset the current bitmap so it doesn't get lost

	gl_texture_clear(0);
	view_primitive_2D_texture_quad(progress_background_bitmap.gl_id,NULL,1.0f,0,iface.scale_x,0,iface.scale_y,0.0f,1.0f,0.0f,1.0f,TRUE);
	
		// draw the progress bitmap
		
	if (progress_on) {

			// draw the graphic
			
		lft=iface.progress.x;
		rgt=lft+iface.progress.wid;
		top=iface.progress.y;
		bot=top+iface.progress.high;
	
		effect_image_animate_get_uv(progress_cur_tick,0,&iface.progress.animate,&gx,&gy,&g_size);
		
		view_primitive_2D_texture_quad(progress_bitmap.gl_id,NULL,1.0f,lft,rgt,top,bot,gx,(gx+g_size),gy,(gy+g_size),TRUE);
	}

	gl_frame_swap();
}

