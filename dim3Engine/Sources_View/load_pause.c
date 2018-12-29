/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Load Pause

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
#include "scripts.h"

extern app_type				app;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern js_type				js;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

bool						load_pause_skip_media;
char						load_pause_map_name[name_str_len];

/* =======================================================

      Load Pause Operations
      
======================================================= */

void load_pause_open(void)
{
		// use map background if it exists

	if (load_pause_map_name!=NULL) {
		if (file_paths_data_exist(&file_path_setup,"Bitmaps/Backgrounds_Map",load_pause_map_name,"png")) {
			gui_initialize("Bitmaps/Backgrounds_Map",load_pause_map_name);
			return;
		}
	}
	
		// otherwise generic load background

	gui_initialize("Bitmaps/Backgrounds","load");
}

void load_pause_close(void)
{
	gui_shutdown();
}

void load_pause_setup(char *map_name,bool skip_media)
{
	strcpy(load_pause_map_name,map_name);
	load_pause_skip_media=skip_media;

		// switch to load pause state
		
	server.next_state=gs_load_pause;
}

/* =======================================================

      Run Load Pause
      
======================================================= */

void load_pause_run(void)
{
		// run the GUI

	gui_draw(1.0f,FALSE);
	
		// the input
		
	if (!input_gui_is_click_down()) return;

		// run the map and start
		// any music or media

	server.next_state=gs_running;

	map_start_finish(load_pause_skip_media);
}
