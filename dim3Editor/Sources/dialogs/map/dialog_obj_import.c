/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: OBJ Import Dialog

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

int								*dialog_obj_import_mode,
								*dialog_obj_import_axis,
								*dialog_obj_import_unit;
bool							*dialog_obj_import_force_grid;

// controls

#define diag_prop_obj_import_mode			5000
#define diag_prop_obj_import_axis			5001
#define diag_prop_obj_import_unit			5002
#define diag_prop_obj_import_force_grid		5003
#define diag_prop_obj_import_cancel			5004
#define diag_prop_obj_import_ok				5005

os_dialog_ctrl_type		diag_property_obj_import_ctrls[]={
							{os_dialog_ctrl_type_text_right,0,"Import Type:",30,10,85,20},
							{os_dialog_ctrl_type_text_right,0,"Scale Axis:",30,40,85,20},
							{os_dialog_ctrl_type_text_right,0,"Scale Units:",30,70,85,20},
							{os_dialog_ctrl_type_text_left,0,"(new import only)",240,40,125,20},
							{os_dialog_ctrl_type_text_left,0,"(new import only)",240,70,125,20},
							{os_dialog_ctrl_type_combo,diag_prop_obj_import_mode,"",120,10,350,20},
							{os_dialog_ctrl_type_combo,diag_prop_obj_import_axis,"",120,40,60,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_obj_import_unit,"",120,70,100,20},
							{os_dialog_ctrl_type_checkbox,diag_prop_obj_import_force_grid,"Force to Current Grid",125,100,200,20},
							{os_dialog_ctrl_type_button,diag_prop_obj_import_cancel,"Cancel",310,130,80,25},
							{os_dialog_ctrl_type_default_button,diag_prop_obj_import_ok,"Import",400,130,80,25},
							{-1,-1,"",0,0,0,0}
						};

/* =======================================================

      Run OBJ Import Dialog
      
======================================================= */

void dialog_property_obj_import_proc(int msg_type,int id)
{
	switch (msg_type) {

		case os_dialog_msg_type_init:
			os_dialog_combo_add(diag_prop_obj_import_mode,"Import New Mesh(es)");
			os_dialog_combo_add(diag_prop_obj_import_mode,"Replace Selected Mesh with Imported Mesh(es)");
			os_dialog_combo_add(diag_prop_obj_import_mode,"Replace All Previously Imported Mesh(es) with New Copy");
			os_dialog_combo_set_value(diag_prop_obj_import_mode,0);

			os_dialog_combo_add(diag_prop_obj_import_axis,"X");
			os_dialog_combo_add(diag_prop_obj_import_axis,"Y");
			os_dialog_combo_add(diag_prop_obj_import_axis,"Z");
			os_dialog_combo_set_value(diag_prop_obj_import_axis,0);

			os_dialog_set_int(diag_prop_obj_import_unit,5000);
			os_dialog_set_bool(diag_prop_obj_import_force_grid,TRUE);

			os_dialog_set_focus(diag_prop_obj_import_unit,TRUE);
			break;

		case os_dialog_msg_type_command:

			if (id==diag_prop_obj_import_cancel) {
				os_dialog_close(FALSE);
				return;
			}

			if (id==diag_prop_obj_import_ok) {
				*dialog_obj_import_mode=os_dialog_combo_get_value(diag_prop_obj_import_mode);
				*dialog_obj_import_axis=os_dialog_combo_get_value(diag_prop_obj_import_axis);
				*dialog_obj_import_unit=os_dialog_get_int(diag_prop_obj_import_unit);
				*dialog_obj_import_force_grid=os_dialog_get_bool(diag_prop_obj_import_force_grid);
				os_dialog_close(TRUE);
				return;
			}

			break;
	}
}

bool dialog_obj_import_run(int *import_mode,int *scale_axis,int *scale_unit,bool *force_grid)
{
	dialog_obj_import_mode=import_mode;
	dialog_obj_import_axis=scale_axis;
	dialog_obj_import_unit=scale_unit;
	dialog_obj_import_force_grid=force_grid;

	return(os_dialog_run("OBJ Import",485,160,diag_property_obj_import_ctrls,dialog_property_obj_import_proc));
}

