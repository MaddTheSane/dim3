/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Height Map Import Dialog

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
	#include "dim3Editor.h"
#endif

#include "glue.h"
#include "interface.h"

int						*dialog_height_import_div_cnt,
						*dialog_height_import_size,
						*dialog_height_import_high;

// controls

#define diag_prop_height_import_divisions	5000
#define diag_prop_height_import_size		5001
#define diag_prop_height_import_height		5002
#define diag_prop_height_import_cancel		5003
#define diag_prop_height_import_ok			5004

os_dialog_ctrl_type		diag_property_height_import_ctrls[]={
							{os_dialog_ctrl_type_text_right,0,"Divisions:",30,10,75,20},
							{os_dialog_ctrl_type_text_right,0,"Size:",30,40,75,20},
							{os_dialog_ctrl_type_text_right,0,"Height:",30,70,75,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_height_import_divisions,"",110,10,100,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_height_import_size,"",110,40,100,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_height_import_height,"",110,70,100,20},
							{os_dialog_ctrl_type_button,diag_prop_height_import_cancel,"Cancel",180,100,80,25},
							{os_dialog_ctrl_type_default_button,diag_prop_height_import_ok,"Import",270,100,80,25},
							{-1,-1,"",0,0,0,0}
						};

/* =======================================================

      Run Height Map Import Dialog
      
======================================================= */

void dialog_property_height_import_proc(int msg_type,int id)
{
	switch (msg_type) {

		case os_dialog_msg_type_init:
			os_dialog_set_int(diag_prop_height_import_divisions,25);
			os_dialog_set_int(diag_prop_height_import_size,100000);
			os_dialog_set_int(diag_prop_height_import_height,8000);
			os_dialog_set_focus(diag_prop_height_import_divisions,TRUE);
			break;

		case os_dialog_msg_type_command:

			if (id==diag_prop_height_import_cancel) {
				os_dialog_close(FALSE);
				return;
			}

			if (id==diag_prop_height_import_ok) {
				*dialog_height_import_div_cnt=os_dialog_get_int(diag_prop_height_import_divisions);
				*dialog_height_import_size=os_dialog_get_int(diag_prop_height_import_size);
				*dialog_height_import_high=os_dialog_get_int(diag_prop_height_import_height);
				os_dialog_close(TRUE);
				return;
			}

			break;
	}
}

bool dialog_height_import_run(int *div_cnt,int *size,int *high)
{
	dialog_height_import_div_cnt=div_cnt;
	dialog_height_import_size=size;
	dialog_height_import_high=high;

	return(os_dialog_run("Height Map Import",355,130,diag_property_height_import_ctrls,dialog_property_height_import_proc));
}

