/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Free Rotate Dialog

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

d3ang							*dialog_free_rot_ang;

// controls

#define diag_prop_free_rot_x		5000
#define diag_prop_free_rot_y		5001
#define diag_prop_free_rot_z		5002
#define diag_prop_free_rot_cancel	5003
#define diag_prop_free_rot_ok		5004

os_dialog_ctrl_type		diag_property_free_rot_ctrls[]={
							{os_dialog_ctrl_type_text_right,0,"X:",15,10,30,20},
							{os_dialog_ctrl_type_text_right,0,"Y:",15,40,30,20},
							{os_dialog_ctrl_type_text_right,0,"Z:",15,70,30,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_free_rot_x,"",55,10,100,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_free_rot_y,"",55,40,100,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_free_rot_z,"",55,70,100,20},
							{os_dialog_ctrl_type_button,diag_prop_free_rot_cancel,"Cancel",180,100,80,25},
							{os_dialog_ctrl_type_default_button,diag_prop_free_rot_ok,"OK",270,100,80,25},
							{-1,-1,"",0,0,0,0}
						};

/* =======================================================

      Run Free Rotate Dialog
      
======================================================= */

void dialog_property_free_rot_proc(int msg_type,int id)
{
	switch (msg_type) {

		case os_dialog_msg_type_init:
			os_dialog_set_float(diag_prop_free_rot_x,0.0f);
			os_dialog_set_float(diag_prop_free_rot_y,0.0f);
			os_dialog_set_float(diag_prop_free_rot_z,0.0f);
			os_dialog_set_focus(diag_prop_free_rot_x,TRUE);
			break;

		case os_dialog_msg_type_command:

			if (id==diag_prop_free_rot_cancel) {
				os_dialog_close(FALSE);
				return;
			}

			if (id==diag_prop_free_rot_ok) {
				dialog_free_rot_ang->x=os_dialog_get_float(diag_prop_free_rot_x);
				dialog_free_rot_ang->y=os_dialog_get_float(diag_prop_free_rot_y);
				dialog_free_rot_ang->z=os_dialog_get_float(diag_prop_free_rot_z);

				os_dialog_close(TRUE);
				return;
			}

			break;
	}
}

bool dialog_free_rotate_run(d3ang *ang)
{
	dialog_free_rot_ang=ang;
	return(os_dialog_run("Free Rotate",355,130,diag_property_free_rot_ctrls,dialog_property_free_rot_proc));
}

