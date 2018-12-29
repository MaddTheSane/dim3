/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog File Open

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

int								dialog_open_file_index;
bool							dialog_open_ok;
file_path_directory_type		*dialog_open_fpd;

extern file_path_setup_type		file_path_setup;

// controls

#define diag_prop_open_files	5000
#define diag_prop_open_cancel	5001
#define diag_prop_open_ok		5002

os_dialog_ctrl_type		diag_property_open_ctrls[]={
							{os_dialog_ctrl_type_files,diag_prop_open_files,"",5,5,440,410},
							{os_dialog_ctrl_type_button,diag_prop_open_cancel,"Cancel",275,420,80,25},
							{os_dialog_ctrl_type_default_button,diag_prop_open_ok,"OK",365,420,80,25},
							{-1,-1,"",0,0,0,0}
						};

/* =======================================================

      Run File Open
      
======================================================= */

bool dialog_property_open_proc(int msg_type,int id)
{
	int					idx;

	switch (msg_type) {

		case os_dialog_msg_type_init:
			os_dialog_tree_add(diag_prop_open_files,dialog_open_fpd);
			os_dialog_enable(diag_prop_open_ok,FALSE);
			return(TRUE);

		case os_dialog_msg_type_command:

			if (id==diag_prop_open_cancel) {
				dialog_open_ok=FALSE;
				os_dialog_close(FALSE);
				return(TRUE);
			}

			if (id==diag_prop_open_ok) {
				dialog_open_file_index=os_dialog_tree_get_value(diag_prop_open_files)&0xFFFF;

				dialog_open_ok=TRUE;
				os_dialog_close(TRUE);
				return(TRUE);
			}

			break;

		case os_dialog_msg_type_tree_change:
			idx=os_dialog_tree_get_value(diag_prop_open_files);

			if ((idx&0xFFFF0000)!=0) {
				os_dialog_enable(diag_prop_open_ok,FALSE);
			}
			else {
				os_dialog_enable(diag_prop_open_ok,TRUE);
			}
			return(TRUE);

		case os_dialog_msg_type_tree_double_click:
			idx=os_dialog_tree_get_value(diag_prop_open_files)&0xFFFF;
			if ((idx&0xFFFF0000)!=0) break;

			dialog_open_file_index=idx&0xFFFF;
			dialog_open_ok=TRUE;
			os_dialog_close(TRUE);
			return(TRUE);

	}

	return(FALSE);
}

bool dialog_file_open_run(char *title,char *search_path,char *extension,char *required_file_name,char *file_name)
{
		// scan for files
		
	if (extension!=NULL) {
		dialog_open_fpd=file_paths_read_directory_data(&file_path_setup,search_path,extension);
	}
	else {
		dialog_open_fpd=file_paths_read_directory_data_dir(&file_path_setup,search_path,required_file_name);
	}

		// run dialog

	os_dialog_run(title,450,450,diag_property_open_ctrls,dialog_property_open_proc);

		// get the file

	if ((dialog_open_ok) && (dialog_open_file_index!=-1)) file_paths_get_complete_path_from_index(dialog_open_fpd,dialog_open_file_index,file_name);

	file_paths_close_directory(dialog_open_fpd);

	return(dialog_open_ok);
}

