/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Label XML

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

      Read Label XML
      
======================================================= */

void iface_read_settings_label(iface_type *iface)
{
	int					nlabel,labels_head_tag,label_tag,tag;
	char				path[1024];
	iface_label_type	*label;

		// read in interface from setting files
		
	file_paths_data(&file_path_setup,path,"Settings","Labels","xml");
	if (!xml_open_file(path)) return;
	
		// get counts
		
    labels_head_tag=xml_findrootchild("Labels");
    if (labels_head_tag==-1) {
		xml_close_file();
		return;
	}

	nlabel=xml_countchildren(labels_head_tag);

	if (nlabel==0) {
		xml_close_file();
		return;
	}

		// read the marks
	
	label_tag=xml_findfirstchild("Label",labels_head_tag);
	
	while (label_tag!=-1) {
	
			// create a new label
			
		if (iface->label_list.nlabel>=max_iface_label) {
			xml_close_file();
			return;
		}
		
		label=&iface->label_list.labels[iface->label_list.nlabel];
		iface->label_list.nlabel++;
			
			// read settings
		
		xml_get_attribute_text(label_tag,"name",label->name,name_str_len);
		
		tag=xml_findfirstchild("Setting",label_tag);
		if (tag!=-1) {
			label->sz=xml_get_attribute_int(tag,"size");
		}
	
		tag=xml_findfirstchild("Image",label_tag);
		if (tag!=-1) {
			xml_get_attribute_text(tag,"file",label->bitmap_name,file_str_len);
		}

			// move on to next label
			
		label_tag=xml_findnextchild(label_tag);
	}
	
	xml_close_file();
}

/* =======================================================

      Write Label XML
      
======================================================= */

bool iface_write_settings_label(iface_type *iface,char *err_str)
{
	int					n;
	char				path[1024];
	bool				ok;
	iface_label_type	*label;
	
		// start new file
		
	xml_new_file();

	xml_add_tagstart("Labels");
	xml_add_tagend(FALSE);

	label=iface->label_list.labels;

	for (n=0;n!=iface->label_list.nlabel;n++) {

		xml_add_tagstart("Label");
		xml_add_attribute_text("name",label->name);
		xml_add_tagend(FALSE);
		
		xml_add_tagstart("Setting");
		xml_add_attribute_int("size",label->sz);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Image");
		xml_add_attribute_text("file",label->bitmap_name);
		xml_add_tagend(TRUE);

		xml_add_tagclose("Label");

		label++;
	}

	xml_add_tagclose("Labels");

        // write the xml
		
	file_paths_data(&file_path_setup,path,"Settings","Labels","xml");
		
	ok=xml_save_file(path,err_str);
    xml_close_file();
	
	return(ok);
}
