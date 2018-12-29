/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Crosshair XML

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

      Read Crosshairs XML
      
======================================================= */

void iface_read_settings_crosshair(iface_type *iface)
{
	int						ncrosshair,crosshairs_head_tag,crosshair_tag,tag;
	char					path[1024];
	iface_crosshair_type	*crosshair;

		// read in crosshairs from setting files
		
	file_paths_data(&file_path_setup,path,"Settings","Crosshairs","xml");
	if (!xml_open_file(path)) return;
	
		// get counts
		
    crosshairs_head_tag=xml_findrootchild("Crosshairs");
    if (crosshairs_head_tag==-1) {
		xml_close_file();
		return;
	}

	ncrosshair=xml_countchildren(crosshairs_head_tag);

	if (ncrosshair==0) {
		xml_close_file();
		return;
	}

		// read the crosshairs
	
	crosshair_tag=xml_findfirstchild("Crosshair",crosshairs_head_tag);
	
	while (crosshair_tag!=-1) {
	
			// create a new crosshair

		if (iface->crosshair_list.ncrosshair>=max_iface_crosshair) {
			xml_close_file();
			return;
		}
			
		crosshair=&iface->crosshair_list.crosshairs[iface->crosshair_list.ncrosshair];
		iface->crosshair_list.ncrosshair++;
			
			// read settings
		
		xml_get_attribute_text(crosshair_tag,"name",crosshair->name,name_str_len);
		
		tag=xml_findfirstchild("Image",crosshair_tag);
		if (tag!=-1) {
			xml_get_attribute_text(tag,"file",crosshair->bitmap_name,file_str_len);
		}
		
			// move on to next crosshair
			
		crosshair_tag=xml_findnextchild(crosshair_tag);
	}
	
	xml_close_file();
}

/* =======================================================

      Write Crosshair XML
      
======================================================= */

bool iface_write_settings_crosshair(iface_type *iface,char *err_str)
{
	int						n;
	char					path[1024];
	bool					ok;
	iface_crosshair_type	*crosshair;
	
		// start new file
		
	xml_new_file();

	xml_add_tagstart("Crosshairs");
	xml_add_tagend(FALSE);

	crosshair=iface->crosshair_list.crosshairs;

	for (n=0;n!=iface->crosshair_list.ncrosshair;n++) {

		xml_add_tagstart("Crosshair");
		xml_add_attribute_text("name",crosshair->name);
		xml_add_tagend(FALSE);

		xml_add_tagstart("Image");
		xml_add_attribute_text("file",crosshair->bitmap_name);
		xml_add_tagend(TRUE);

		xml_add_tagclose("Crosshair");

		crosshair++;
	}

	xml_add_tagclose("Crosshairs");

        // write the xml
		
	file_paths_data(&file_path_setup,path,"Settings","Crosshairs","xml");
		
	ok=xml_save_file(path,err_str);
    xml_close_file();
	
	return(ok);
}
