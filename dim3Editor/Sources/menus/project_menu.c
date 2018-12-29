/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Project Menus

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern map_type					map;
extern app_state_type			state;

extern iface_type				iface;

os_menu_item_type	setup_menu_setup[]=
						{

									// File menu

								{"File","Save",project_menu_item_Save,os_menu_key_cmd,'S'},
							#ifdef D3_OS_WINDOWS
								{"File","",0,os_menu_key_none,0x0},
								{"File","Exit",project_menu_item_Quit,os_menu_key_none,0x0},
							#endif

								{"","",-1,os_menu_key_none,0x0},
						};

/* =======================================================

      Create Menu
      
======================================================= */

void project_menu_create(void)
{
	os_menu_create(setup_menu_setup);
}

void project_menu_dispose(void)
{
	os_menu_dispose();
}

/* =======================================================

      Menu Enable/Disable
      
======================================================= */

void project_menu_update(void)
{
}

/* =======================================================

      Menu Commands
      
======================================================= */

bool project_menu_save(void)
{
	char		err_str[256];

	if (iface_write(&iface,err_str)) return(TRUE);
	
	os_dialog_alert("Could not save setup files",err_str);
	return(FALSE);
}

bool project_menu_quit_save(void)
{
	int			choice;

	choice=os_dialog_confirm("Save Changes?","Do you want to save the changes to this project?",TRUE);
	if (choice==1) return(FALSE);
	
	if (choice==0) return(project_menu_save());
	
	return(TRUE);
}

/* =======================================================

      Menu Events
      
======================================================= */

bool project_menu_event_run(int cmd)
{
	switch (cmd) {
	
			// file menu
			
		case project_menu_item_Save:
			project_menu_save();
			return(TRUE);						

		case project_menu_item_Quit:
			if (project_menu_quit_save()) os_application_quit();
			return(TRUE);
							
	}
	
	return(FALSE);
}

