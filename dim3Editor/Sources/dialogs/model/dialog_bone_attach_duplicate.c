/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog Bone Attach Duplicate

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

int							*dialog_from_bone_idx,*dialog_to_bone_idx,
							*dialog_vertex_slop;

// controls

#define diag_prop_bone_attach_from		5000
#define diag_prop_bone_attach_to		5001
#define diag_prop_bone_attach_slop		5002
#define diag_prop_bone_attach_cancel	5003
#define diag_prop_bone_attach_ok		5004

os_dialog_ctrl_type		diag_property_bone_attach_ctrls[]={
							{os_dialog_ctrl_type_text_left,0,"Duplicate the vertex attachment from one bone to another bone.",10,10,420,20},
							{os_dialog_ctrl_type_text_right,0,"From Bone:",10,40,75,20},
							{os_dialog_ctrl_type_combo,diag_prop_bone_attach_from,"",90,40,310,20},
							{os_dialog_ctrl_type_text_right,0,"To Bone:",10,70,75,20},
							{os_dialog_ctrl_type_combo,diag_prop_bone_attach_to,"",90,70,310,20},
							{os_dialog_ctrl_type_text_right,0,"Within:",10,100,75,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_bone_attach_slop,"",90,100,100,20},
							{os_dialog_ctrl_type_button,diag_prop_bone_attach_cancel,"Cancel",250,130,80,25},
							{os_dialog_ctrl_type_default_button,diag_prop_bone_attach_ok,"OK",340,130,80,25},
							{-1,-1,"",0,0,0,0}
						};

/* =======================================================

      Run Vertex Bone Attach Duplicate
      
======================================================= */

void dialog_property_bone_attach_proc(int msg_type,int id)
{
	switch (msg_type) {

		case os_dialog_msg_type_init:
			dialog_set_vertex_bone_set_bone_combo(diag_prop_bone_attach_to,-1);
			dialog_set_vertex_bone_set_bone_combo(diag_prop_bone_attach_from,-1);
			os_dialog_set_int(diag_prop_bone_attach_slop,0);
			os_dialog_set_focus(diag_prop_bone_attach_slop,TRUE);
			break;

		case os_dialog_msg_type_command:

			if (id==diag_prop_bone_attach_cancel) {
				os_dialog_close(FALSE);
				return;
			}

			if (id==diag_prop_bone_attach_ok) {
				*dialog_from_bone_idx=dialog_set_vertex_bone_get_bone_combo(diag_prop_bone_attach_from);
				*dialog_to_bone_idx=dialog_set_vertex_bone_get_bone_combo(diag_prop_bone_attach_to);
				*dialog_vertex_slop=os_dialog_get_int(diag_prop_bone_attach_slop);
				os_dialog_close(TRUE);
				return;
			}

			break;
	}
}

bool dialog_bone_attach_duplicate_run(int *from_bone_idx,int *to_bone_idx,int *vertex_slop)
{
	bool				ok;

	dialog_from_bone_idx=from_bone_idx;
	dialog_to_bone_idx=to_bone_idx;
	dialog_vertex_slop=vertex_slop;

	ok=os_dialog_run("Duplicate Bone Attachments",425,160,diag_property_bone_attach_ctrls,dialog_property_bone_attach_proc);

	if ((*dialog_from_bone_idx==-1) || (*dialog_to_bone_idx==-1)) return(FALSE);
	return(ok);
}
