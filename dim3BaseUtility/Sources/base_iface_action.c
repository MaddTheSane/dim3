/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Actions XML

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
	#include "dim3baseutility.h"
#endif

extern file_path_setup_type		file_path_setup;

char							control_names_str[][32]=control_names;

/* =======================================================

      Read Action XML
      
======================================================= */

void iface_read_settings_action(iface_type *iface)
{
	int					n,actions_head_tag,action_tag,idx;
	char				name[name_str_len],path[1024];
						
		// default actions
		
	for (n=0;n!=ncontrol;n++) {
		strcpy(iface->action_display_list.action_displays[n].display_name,control_names_str[n]);
		iface->action_display_list.action_displays[n].show=FALSE;
	}
	
		// read in actions from setting files
		
	file_paths_data(&file_path_setup,path,"Settings","Actions","xml");
	if (!xml_open_file(path)) return;
	
		// decode the file
      
    actions_head_tag=xml_findrootchild("Actions");
    if (actions_head_tag!=-1) {
	
		action_tag=xml_findfirstchild("Action",actions_head_tag);
		
		while (action_tag!=-1) {
			xml_get_attribute_text(action_tag,"name",name,name_str_len);

				// find control index

			idx=-1;

			for (n=0;n!=ncontrol;n++) {
				if (strcasecmp(control_names_str[n],name)==0) {
					idx=n;
					break;
				}
			}

			if (idx!=-1) {
				xml_get_attribute_text(action_tag,"display",iface->action_display_list.action_displays[idx].display_name,name_str_len);
				iface->action_display_list.action_displays[idx].show=xml_get_attribute_boolean(action_tag,"show");
			}
			
			action_tag=xml_findnextchild(action_tag);
		}
	}
	
	xml_close_file();
}

/* =======================================================

      Write Action XML
      
======================================================= */

bool iface_write_settings_action(iface_type *iface,char *err_str)
{
	int							n;
	char						path[1024];
	bool						ok;
	iface_action_display_type	*action;
	
		// start new file
		
	xml_new_file();

	xml_add_tagstart("Actions");
	xml_add_tagend(FALSE);
	
	action=iface->action_display_list.action_displays;

	for (n=0;n!=ncontrol;n++) {

		xml_add_tagstart("Action");
		xml_add_attribute_text("name",control_names_str[n]);
		xml_add_attribute_text("display",action->display_name);
		xml_add_attribute_boolean("show",action->show);
		xml_add_tagend(TRUE);

		action++;
	}

	xml_add_tagclose("Actions");

        // write the xml
		
	file_paths_data(&file_path_setup,path,"Settings","Actions","xml");
		
	ok=xml_save_file(path,err_str);
    xml_close_file();
	
	return(ok);
}
