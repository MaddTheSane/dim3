/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog Set Normal

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

d3vct					*dialog_set_normal_vct;

// controls

#define diag_prop_set_normal_x		5000
#define diag_prop_set_normal_y		5001
#define diag_prop_set_normal_z		5002
#define diag_prop_set_normal_cancel	5003
#define diag_prop_set_normal_ok		5004

os_dialog_ctrl_type		diag_property_set_normal_ctrls[]={
							{os_dialog_ctrl_type_text_right,0,"X:",5,10,35,20},
							{os_dialog_ctrl_type_text_right,0,"Y:",5,40,35,20},
							{os_dialog_ctrl_type_text_right,0,"Z:",5,70,35,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_set_normal_x,"",45,10,100,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_set_normal_y,"",45,40,100,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_set_normal_z,"",45,70,100,20},
							{os_dialog_ctrl_type_button,diag_prop_set_normal_cancel,"Cancel",180,100,80,25},
							{os_dialog_ctrl_type_default_button,diag_prop_set_normal_ok,"OK",270,100,80,25},
							{-1,-1,"",0,0,0,0}
						};

/* =======================================================

      Run Set Normal
      
======================================================= */

void dialog_property_set_normal_proc(int msg_type,int id)
{
	switch (msg_type) {

		case os_dialog_msg_type_init:
			os_dialog_set_float(diag_prop_set_normal_x,dialog_set_normal_vct->x);
			os_dialog_set_float(diag_prop_set_normal_y,dialog_set_normal_vct->y);
			os_dialog_set_float(diag_prop_set_normal_z,dialog_set_normal_vct->z);
			os_dialog_set_focus(diag_prop_set_normal_x,TRUE);
			break;

		case os_dialog_msg_type_command:

			if (id==diag_prop_set_normal_cancel) {
				os_dialog_close(FALSE);
				return;
			}

			if (id==diag_prop_set_normal_ok) {
				dialog_set_normal_vct->x=os_dialog_get_float(diag_prop_set_normal_x);
				dialog_set_normal_vct->y=os_dialog_get_float(diag_prop_set_normal_y);
				dialog_set_normal_vct->z=os_dialog_get_float(diag_prop_set_normal_z);

				os_dialog_close(TRUE);
				return;
			}

			break;
	}
}

bool dialog_set_normal_run(d3vct *vct)
{
	dialog_set_normal_vct=vct;
	return(os_dialog_run("Set Normal",355,130,diag_property_set_normal_ctrls,dialog_property_set_normal_proc));
}
