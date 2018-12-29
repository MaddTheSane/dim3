/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Sound XML

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

      Read Sound XML
      
======================================================= */

void iface_read_settings_sound(iface_type *iface)
{
	int					sounds_head_tag,sound_tag,tag;
	char				path[1024];
	iface_sound_type	*snd;

	iface->sound_list.nsound=0;
	
		// read in sound from setting files
		
	file_paths_data(&file_path_setup,path,"Settings","Sounds","xml");
	if (!xml_open_file(path)) return;
	
		// decode the file
      
    sounds_head_tag=xml_findrootchild("Sounds");
    if (sounds_head_tag!=-1) {
	
		sound_tag=xml_findfirstchild("Sound",sounds_head_tag);
		
		while (sound_tag!=-1) {
			snd=&iface->sound_list.sounds[iface->sound_list.nsound];

			xml_get_attribute_text(sound_tag,"name",snd->name,name_str_len);
			
			tag=xml_findfirstchild("Wave",sound_tag);
			if (tag!=-1) xml_get_attribute_text(tag,"file",snd->file_name,file_str_len);
			
			tag=xml_findfirstchild("Distance",sound_tag);
			if (tag!=-1) {
				snd->min_dist=xml_get_attribute_int(tag,"min");
				snd->max_dist=xml_get_attribute_int(tag,"max");
			}

			iface->sound_list.nsound++;
			if (iface->sound_list.nsound==max_iface_sound) break;

			sound_tag=xml_findnextchild(sound_tag);
		}
	}
	
	xml_close_file();
}

/* =======================================================

      Write Sounds XML
      
======================================================= */

bool iface_write_settings_sound(iface_type *iface,char *err_str)
{
	int					n;
	char				path[1024];
	bool				ok;
	iface_sound_type	*snd;
	
		// start new file
		
	xml_new_file();

	xml_add_tagstart("Sounds");
	xml_add_tagend(FALSE);

	snd=iface->sound_list.sounds;

	for (n=0;n!=iface->sound_list.nsound;n++) {

		xml_add_tagstart("Sound");
		xml_add_attribute_text("name",snd->name);
		xml_add_tagend(FALSE);

		xml_add_tagstart("Wave");
		xml_add_attribute_text("file",snd->file_name);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Distance");
		xml_add_attribute_int("min",snd->min_dist);
		xml_add_attribute_int("max",snd->max_dist);
		xml_add_tagend(TRUE);

		xml_add_tagclose("Sound");

		snd++;
	}

	xml_add_tagclose("Sounds");

        // write the xml
		
	file_paths_data(&file_path_setup,path,"Settings","Sounds","xml");
		
	ok=xml_save_file(path,err_str);
    xml_close_file();
	
	return(ok);
}
