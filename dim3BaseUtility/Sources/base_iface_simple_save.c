/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Simple Save XML

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

      Read Simple Save
      
======================================================= */

void simple_save_xml_read(iface_type *iface)
{
	int							n,idx,count,
								head_tag,tag;
	char						path[1024];
	iface_simple_save_type		*save;
	
		// clear saves
		
	save=iface->simple_save_list.saves;
		
	for (n=0;n!=max_simple_save_spot;n++) {
		save->save_id=-1;
		save->desc[0]=0x0;
		save->points=0;
		save++;
	}
	
		// any file to load
		
	if (!file_paths_app_data_exist(&file_path_setup,path,"Settings","SimpleSave","xml")) return;
	
		// read file
		
	if (!xml_open_file(path)) return;
    
		// decode the file
       
    head_tag=xml_findrootchild("Simple_Saves");
    if (head_tag!=-1) {
	
		count=xml_countchildren(head_tag);
		tag=xml_findfirstchild("Simple_Save",head_tag);
		
        for (n=0;n!=count;n++) {
			idx=xml_get_attribute_int(tag,"idx");
			
			save=&iface->simple_save_list.saves[idx];
			
			save->save_id=xml_get_attribute_int(tag,"id");
			xml_get_attribute_text(tag,"desc",save->desc,64);
			save->points=xml_get_attribute_int(tag,"points");

			tag=xml_findnextchild(tag);
        }
	}
  
	xml_close_file();
}

/* =======================================================

      Write Simple Save
      
======================================================= */

void simple_save_xml_write(iface_type *iface,char *err_str)
{
	int							n;
	char						path[1024];
	bool						ok;
	iface_simple_save_type		*save;
	
		// start the file
		
    xml_new_file();
	
		// create the xml
    
    xml_add_tagstart("Simple_Saves");
    xml_add_tagend(FALSE);
	
	save=iface->simple_save_list.saves;
	
	for (n=0;n!=max_simple_save_spot;n++) {
	
		if (save->save_id!=-1) {
			xml_add_tagstart("Simple_Save");
			xml_add_attribute_int("idx",n);
			xml_add_attribute_int("id",save->save_id);
			xml_add_attribute_text("desc",save->desc);
			xml_add_attribute_int("points",save->points);
			xml_add_tagend(TRUE);
		}
		
		save++;
	}

    xml_add_tagclose("Simple_Saves");

        // save the file
		
	file_paths_app_data(&file_path_setup,path,"Settings","SimpleSave","xml");
		
	ok=xml_save_file(path,err_str);
    xml_close_file();
}

