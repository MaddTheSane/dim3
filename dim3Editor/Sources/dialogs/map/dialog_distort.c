/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog Distort

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
	#include "dim3Editor.h"
#endif

#include "glue.h"
#include "interface.h"

d3pnt					*dialog_distort_mov_pnt;

// controls

#define diag_prop_distort_x			5000
#define diag_prop_distort_y			5001
#define diag_prop_distort_z			5002
#define diag_prop_distort_cancel	5003
#define diag_prop_distort_ok		5004

os_dialog_ctrl_type		diag_property_distort_ctrls[]={
							{os_dialog_ctrl_type_text_right,0,"X:",10,10,35,20},
							{os_dialog_ctrl_type_text_right,0,"Y:",10,40,35,20},
							{os_dialog_ctrl_type_text_right,0,"Z:",10,70,35,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_distort_x,"",50,10,100,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_distort_y,"",50,40,100,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_distort_z,"",50,70,100,20},
							{os_dialog_ctrl_type_button,diag_prop_distort_cancel,"Cancel",210,100,80,25},
							{os_dialog_ctrl_type_default_button,diag_prop_distort_ok,"OK",300,100,80,25},
							{-1,-1,"",0,0,0,0}
						};

/* =======================================================

      Run Distort
      
======================================================= */

void dialog_property_distort_proc(int msg_type,int id)
{
	switch (msg_type) {

		case os_dialog_msg_type_init:
			os_dialog_set_int(diag_prop_distort_x,500);
			os_dialog_set_int(diag_prop_distort_y,500);
			os_dialog_set_int(diag_prop_distort_z,500);
			os_dialog_set_focus(diag_prop_distort_x,TRUE);
			break;

		case os_dialog_msg_type_command:

			if (id==diag_prop_distort_cancel) {
				os_dialog_close(FALSE);
				return;
			}

			if (id==diag_prop_distort_ok) {
				dialog_distort_mov_pnt->x=os_dialog_get_int(diag_prop_distort_x);
				dialog_distort_mov_pnt->y=os_dialog_get_int(diag_prop_distort_y);
				dialog_distort_mov_pnt->z=os_dialog_get_int(diag_prop_distort_z);

				os_dialog_close(TRUE);
				return;
			}

			break;
	}
}

bool dialog_distort_run(d3pnt *mov_pnt)
{
	dialog_distort_mov_pnt=mov_pnt;
	return(os_dialog_run("Distort Mesh Vertexes",385,130,diag_property_distort_ctrls,dialog_property_distort_proc));
}
