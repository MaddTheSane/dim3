/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Singleplayer XML

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

/* =======================================================

      Read Singleplayer XML
      
======================================================= */

void iface_read_settings_singleplayer(iface_type *iface)
{
	int							singleplayer_head_tag,
								settings_tag,options_head_tag,option_tag;
	char						path[1024];
	iface_sp_option_type		*sp_option;

		// get xml file
		
	file_paths_data(&file_path_setup,path,"Settings","Singleplayer","xml");
	if (!xml_open_file(path)) return;
	
	singleplayer_head_tag=xml_findrootchild("Singleplayer");		
    if (singleplayer_head_tag==-1) {
		xml_close_file();
		return;
	}

		// settings
		
    settings_tag=xml_findfirstchild("Settings",singleplayer_head_tag);
    if (settings_tag!=-1) {
		iface->singleplayer.skill=xml_get_attribute_boolean(settings_tag,"skill");
		iface->singleplayer.map_pick=xml_get_attribute_boolean(settings_tag,"map_pick");
		iface->singleplayer.map_pick_ss_reveal_id=xml_get_attribute_int_default(settings_tag,"map_pick_ss_reveal_id",-1);
	}

		// options

	options_head_tag=xml_findfirstchild("Options",singleplayer_head_tag);
	if (options_head_tag!=-1) {
		
		iface->singleplayer.option_list.noption=0;
		
		option_tag=xml_findfirstchild("Option",options_head_tag);
		while (option_tag!=-1) {
			
			sp_option=&iface->singleplayer.option_list.options[iface->singleplayer.option_list.noption];
	
			xml_get_attribute_text(option_tag,"name",sp_option->name,name_str_len);
			xml_get_attribute_text(option_tag,"description",sp_option->descript,64);
			sp_option->ss_reveal_id=xml_get_attribute_int_default(option_tag,"ss_reveal_id",-1);
			
			iface->singleplayer.option_list.noption++;
			if (iface->singleplayer.option_list.noption==max_net_option) break;

			option_tag=xml_findnextchild(option_tag);
		}
	}
	

	xml_close_file();
}

/* =======================================================

      Write Singleplayer XML
      
======================================================= */

bool iface_write_settings_singleplayer(iface_type *iface,char *err_str)
{
	int							n;
	char						path[1024];
	bool						ok;
	iface_sp_option_type		*sp_option;
	
		// start new file
		
	xml_new_file();

		// singleplayer

	xml_add_tagstart("Singleplayer");
	xml_add_tagend(FALSE);

		// settings
		
	xml_add_tagstart("Settings");
	xml_add_attribute_boolean("skill",iface->singleplayer.skill);
	xml_add_attribute_boolean("map_pick",iface->singleplayer.map_pick);
	xml_add_attribute_int("map_pick_ss_reveal_id",iface->singleplayer.map_pick_ss_reveal_id);
	xml_add_tagend(TRUE);

		// options

	xml_add_tagstart("Options");
	xml_add_tagend(FALSE);

	sp_option=iface->singleplayer.option_list.options;

	for (n=0;n!=iface->singleplayer.option_list.noption;n++) {
		xml_add_tagstart("Option");
		xml_add_attribute_text("name",sp_option->name);
		xml_add_attribute_text("description",sp_option->descript);
		xml_add_attribute_int("ss_reveal_id",sp_option->ss_reveal_id);
		xml_add_tagend(TRUE);

		sp_option++;
	}

	xml_add_tagclose("Options");

	xml_add_tagclose("Singleplayer");

        // write the xml
		
	file_paths_data(&file_path_setup,path,"Settings","Singleplayer","xml");
		
	ok=xml_save_file(path,err_str);
    xml_close_file();
	
	return(ok);
}
