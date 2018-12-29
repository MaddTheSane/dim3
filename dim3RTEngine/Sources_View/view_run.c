/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Periodic View Processes

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
extern setup_type			setup;
extern camera_type			camera;

extern void run_objects_animations(void);
extern void ambient_add_map_sounds(void);
extern void ambient_add_objects(void);
extern void ambient_add_liquids(void);

/* =======================================================

      FPS
      
======================================================= */

void view_clear_fps(void)
{
	view.fps.tick=view.fps.count=0;
	view.fps.last_time=-1;
	view.fps.total=0.0f;
	view.fps.first_skip=TRUE;
}

void view_calculate_fps(void)
{
	int				tick;

	tick=game_time_get();

	if (view.fps.last_time!=-1) {
		view.fps.tick+=(tick-view.fps.last_time);

			// average the fps over 1 second
			// we always skip the first one as we might
			// be dealing with remaining change activity

		if (view.fps.tick>=1000) {
			if (!view.fps.first_skip) view.fps.total=(((float)view.fps.count)*1000.0f)/(float)view.fps.tick;
			view.fps.first_skip=FALSE;
			view.fps.tick=view.fps.count=0;
		}
	}
	
	view.fps.last_time=tick;
}

/* =======================================================

      Run View Processes
      
======================================================= */

void view_run(void)
{
	int				tick;
	obj_type		*obj;
	
	tick=game_time_get();

		// run-tick objects
		
	if (tick>=view.time.run_tick) {

		while (tick>=view.time.run_tick) {
			view.time.run_tick+=50;
		}

			// check clicable objects
			
		obj=server.obj_list.objs[server.player_obj_idx];
		obj->click.current_click_obj_idx=object_find_idx_click_object(obj);

			// listener position
			
		if (map.camera.camera_mode==cv_fpp) {
			al_set_listener(&obj->pnt,obj->ang.y);
		}
		else {
			al_set_listener(&camera.cur_pos.pnt,camera.cur_pos.ang.y);
		}
		
			// ambients

		al_ambient_list_clear();
		ambient_add_map_sounds();
		ambient_add_objects();
		ambient_add_liquids();
		
		al_ambients_run();

			// music

		al_music_run();
	}
}

