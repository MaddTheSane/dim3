/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: External Editor Launching

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern iface_type				iface;
extern map_type					map;
extern app_pref_type			pref;
extern app_state_type			state;
extern file_path_setup_type		file_path_setup;

/* =======================================================

      Launch dim3 Engine
      
======================================================= */

void launch_engine(void)
{
	char					err_str[256],path[1024];
	unsigned char			uc_len;
	d3pnt					pnt;
	d3ang					ang;
	FILE					*file;
	
		// ok to save?

	if (os_dialog_confirm("Save Changes and Launch Engine?","You need to save changes to this project before you can launch the engine.  Click Yes to save changes and launch the map in the engine.",FALSE)!=0) return;

		// save specific mode
		// data

	switch (state.mode) {

		case app_mode_project:
			iface_write(&iface,err_str);
			break;

		case app_mode_map:
			if (state.map.map_open) file_save_map();
			break;

		case app_mode_model:
			if (state.model.model_open) model_file_save();
			break;

	}

		// if we are in the map
		// state, launch to that map

	if (state.mode==app_mode_map) {
		
			// the link file
			
		file_paths_dim3_app_data(&file_path_setup,path,"dim3EditorLink","tmp");

		file=fopen(path,"w");
		if (file==NULL) return;
		
			// map name
			
		uc_len=(unsigned char)strlen(map.info.name);
   		fwrite(&uc_len,1,1,file);
   		fwrite(map.info.name,1,strlen(map.info.name),file);
		
			// map position
			
		view_get_position(&pnt);
		view_get_angle(&ang);
			
		fwrite(&pnt,1,sizeof(d3pnt),file);
		fwrite(&ang,1,sizeof(d3ang),file);
		
		fclose(file);
	}
	
		// run engine
		
	file_paths_base(&file_path_setup,path,pref.engine_name,D3_APP_EXTENSION);
	if (!os_launch_process(path,FALSE)) {
		os_dialog_alert("Launch Engine","Could not find Engine, check engine name in preferences.");
    }
}

/* =======================================================

      Launch Map Script Editor
      
======================================================= */

void launch_map_script_editor(void)
{
	char					path[1024];
		
	file_paths_data(&file_path_setup,path,"Scripts/Courses",map.info.name,"js");
	if (!os_launch_process(path,TRUE)) {
		os_dialog_alert("Could not locate script file","No script file with this name exists.");
    }
}

/* =======================================================

      Launch Spot Script Editor
      
======================================================= */

void launch_spot_script_editor(spot_type *spot)
{
	char					path[1024];
	
	if (spot->script[0]==0x0) return;
		
	file_paths_data(&file_path_setup,path,"Scripts/Objects",spot->script,"js");
	if (!os_launch_process(path,TRUE)) {
		os_dialog_alert("Could not locate script file","No script file with this name exists.");
    }
}
