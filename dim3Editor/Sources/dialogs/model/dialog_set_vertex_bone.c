/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog Set Vertex Bone

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

extern model_type			model;

int							*dialog_major_bone_idx,*dialog_minor_bone_idx;
float						*dialog_bone_factor;

// controls

#define diag_prop_set_bone_major	5000
#define diag_prop_set_bone_minor	5001
#define diag_prop_set_bone_factor	5002
#define diag_prop_set_bone_cancel	5003
#define diag_prop_set_bone_ok		5004

os_dialog_ctrl_type		diag_property_set_bone_ctrls[]={
							{os_dialog_ctrl_type_text_right,0,"Major Bone:",10,10,75,20},
							{os_dialog_ctrl_type_combo,diag_prop_set_bone_major,"",90,10,310,20},
							{os_dialog_ctrl_type_text_right,0,"Minor Bone:",10,40,75,20},
							{os_dialog_ctrl_type_combo,diag_prop_set_bone_minor,"",90,40,310,20},
							{os_dialog_ctrl_type_text_right,0,"Bone Factor:",10,70,75,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_set_bone_factor,"",90,70,100,20},
							{os_dialog_ctrl_type_text_left,0,"%",205,70,25,20},
							{os_dialog_ctrl_type_button,diag_prop_set_bone_cancel,"Cancel",250,100,80,25},
							{os_dialog_ctrl_type_default_button,diag_prop_set_bone_ok,"OK",340,100,80,25},
							{-1,-1,"",0,0,0,0}
						};

/* =======================================================

      Set and Get Bone Combo
      
======================================================= */

void dialog_set_vertex_bone_set_bone_combo(int id,int bone_idx)
{
	int				n;
	
	os_dialog_combo_clear(id);

	os_dialog_combo_add(id,"None");
	
	for (n=0;n!=model.nbone;n++) {
		os_dialog_combo_add(id,model.bones[n].name);
	}
	
	if (bone_idx==-1) {
		os_dialog_combo_set_value(id,0);
	}
	else {
		os_dialog_combo_set_value(id,(bone_idx+1));
	}
}

int dialog_set_vertex_bone_get_bone_combo(int id)
{
	int				sel_idx;
	
	sel_idx=os_dialog_combo_get_value(id);
	if (sel_idx==0) return(-1);
	
	return(sel_idx-1);
}

/* =======================================================

      Run Vertex Bone Set
      
======================================================= */

void dialog_property_set_bone_proc(int msg_type,int id)
{
	switch (msg_type) {

		case os_dialog_msg_type_init:
			dialog_set_vertex_bone_set_bone_combo(diag_prop_set_bone_major,-1);
			dialog_set_vertex_bone_set_bone_combo(diag_prop_set_bone_minor,-1);
			os_dialog_set_int(diag_prop_set_bone_factor,100);
			os_dialog_set_focus(diag_prop_set_bone_factor,TRUE);
			break;

		case os_dialog_msg_type_command:

			if (id==diag_prop_set_bone_cancel) {
				os_dialog_close(FALSE);
				return;
			}

			if (id==diag_prop_set_bone_ok) {
				*dialog_major_bone_idx=dialog_set_vertex_bone_get_bone_combo(diag_prop_set_bone_major);
				*dialog_minor_bone_idx=dialog_set_vertex_bone_get_bone_combo(diag_prop_set_bone_minor);
				*dialog_bone_factor=((float)os_dialog_get_int(diag_prop_set_bone_factor))/100.0f;

				os_dialog_close(TRUE);
				return;
			}

			break;
	}
}

bool dialog_set_vertex_bone_run(int *major_bone_idx,int *minor_bone_idx,float *factor)
{
	dialog_major_bone_idx=major_bone_idx;
	dialog_minor_bone_idx=minor_bone_idx;
	dialog_bone_factor=factor;

	return(os_dialog_run("Set Bone",425,130,diag_property_set_bone_ctrls,dialog_property_set_bone_proc));
}
