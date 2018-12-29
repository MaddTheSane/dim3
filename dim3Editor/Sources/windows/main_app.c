/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main App Routines

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern iface_type				iface;
extern file_path_setup_type		file_path_setup;
extern app_state_type			state;

/* =======================================================

      Initialize and Shutdown
	        
======================================================= */

bool main_app_initialize(void)
{
	os_glue_start();
	os_set_arrow_cursor();
	
		// initial states

	state.mode=app_mode_project;
 	state.map.map_open=FALSE;
	state.model.model_open=FALSE;
   
		// setup file paths
		
	if (!file_paths_setup(&file_path_setup)) {
		os_dialog_alert("Error","No data folder found");
		os_glue_end();
		return(FALSE);
	}
	
		// load interface and setup files
		
	if (!iface_initialize(&iface)) {
		os_dialog_alert("Error","Out of memory");
		os_glue_end();
		return(FALSE);
	}

	iface_read(&iface);
		
	setup_xml_read();
	
		// setup undo
		
	map_undo_initialize();
	model_undo_initialize();
	
	return(TRUE);
}

void main_app_shutdown(void)
{
		// close interface and
		// end glue
		
	iface_shutdown(&iface);
	os_glue_end();
}

/* =======================================================

      Quit
	        
======================================================= */

bool main_app_quit(void)
{
	switch (state.mode) {

		case app_mode_project:
			return(project_menu_quit_save());
			
		case app_mode_map:
			return(file_close_map());

		case app_mode_model:
			return(model_file_close());
			
	}
	
	return(TRUE);
}

