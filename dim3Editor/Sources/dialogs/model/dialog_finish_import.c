/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog Import Finish

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

int							dialog_import_finish_high;

// controls

#define diag_prop_import_finish_high	5000
#define diag_prop_import_finish_ok		5001

os_dialog_ctrl_type		diag_property_import_finish_ctrls[]={
							{os_dialog_ctrl_type_text_right,0,"Enter a height for the imported mesh:",10,10,215,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_import_finish_high,"",230,10,100,20},
							{os_dialog_ctrl_type_default_button,diag_prop_import_finish_ok,"OK",340,40,80,25},
							{-1,-1,"",0,0,0,0}
						};

/* =======================================================

      Run Import Finish
      
======================================================= */

void dialog_property_import_finish_proc(int msg_type,int id)
{
	switch (msg_type) {

		case os_dialog_msg_type_init:
			os_dialog_set_int(diag_prop_import_finish_high,100);
			os_dialog_set_focus(diag_prop_import_finish_high,TRUE);
			break;

		case os_dialog_msg_type_command:

			if (id==diag_prop_import_finish_ok) {
				dialog_import_finish_high=os_dialog_get_int(diag_prop_import_finish_high);
				os_dialog_close(TRUE);
				return;
			}

			break;
	}
}

int dialog_import_finish_run(void)
{
	os_dialog_run("Mesh Height",425,70,diag_property_import_finish_ctrls,dialog_property_import_finish_proc);
	return(dialog_import_finish_high);
}
