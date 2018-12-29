/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Halo XML

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

      Read Halos XML
      
======================================================= */

void iface_read_settings_halo(iface_type *iface)
{
	int					nhalo,halos_head_tag,halo_tag,tag;
	char				path[1024];
	iface_halo_type		*halo;

		// read in interface from setting files
		
	file_paths_data(&file_path_setup,path,"Settings","Halos","xml");
	if (!xml_open_file(path)) return;
	
		// get counts
		
    halos_head_tag=xml_findrootchild("Halos");
    if (halos_head_tag==-1) {
		xml_close_file();
		return;
	}

	nhalo=xml_countchildren(halos_head_tag);

	if (nhalo==0) {
		xml_close_file();
		return;
	}

		// read the halos

	halo_tag=xml_findfirstchild("Halo",halos_head_tag);
	
	while (halo_tag!=-1) {
	
			// create a new halo

		if (iface->halo_list.nhalo>=max_iface_halo) {
			xml_close_file();
			return;
		}
			
		halo=&iface->halo_list.halos[iface->halo_list.nhalo];
		iface->halo_list.nhalo++;
			
			// read settings
			
		xml_get_attribute_text(halo_tag,"name",halo->name,name_str_len);
		
		tag=xml_findfirstchild("Image",halo_tag);
		if (tag!=-1) {
			xml_get_attribute_text(tag,"file",halo->bitmap_name,file_str_len);
		}
		
		halo->min_dist=3000;
		halo->max_dist=50000;
		halo->min_size=500;
		halo->max_size=200;
		halo->min_alpha=0.8f;
		halo->max_alpha=0.05f;
		halo->no_clip_object=FALSE;
		halo->no_clip_self=TRUE;

		tag=xml_findfirstchild("Distance",halo_tag);
		if (tag!=-1) {
			halo->min_dist=xml_get_attribute_int(tag,"min");
			halo->max_dist=xml_get_attribute_int(tag,"max");
		}

		tag=xml_findfirstchild("Size",halo_tag);
		if (tag!=-1) {
			halo->min_size=xml_get_attribute_int(tag,"min");
			halo->max_size=xml_get_attribute_int(tag,"max");
		}

		tag=xml_findfirstchild("Alpha",halo_tag);
		if (tag!=-1) {
			halo->min_alpha=xml_get_attribute_float(tag,"min");
			halo->max_alpha=xml_get_attribute_float(tag,"max");
		}

		tag=xml_findfirstchild("Option",halo_tag);
		if (tag!=-1) {
			halo->no_clip_object=xml_get_attribute_boolean(tag,"no_clip_object");
			halo->no_clip_self=xml_get_attribute_boolean(tag,"no_clip_self");
		}
		
			// move on to next halo
			
		halo_tag=xml_findnextchild(halo_tag);
	}
	
	xml_close_file();
}

/* =======================================================

      Write Halos XML
      
======================================================= */

bool iface_write_settings_halo(iface_type *iface,char *err_str)
{
	int					n;
	char				path[1024];
	bool				ok;
	iface_halo_type		*halo;
	
		// start new file
		
	xml_new_file();

	xml_add_tagstart("Halos");
	xml_add_tagend(FALSE);

	halo=iface->halo_list.halos;

	for (n=0;n!=iface->halo_list.nhalo;n++) {

		xml_add_tagstart("Halo");
		xml_add_attribute_text("name",halo->name);
		xml_add_tagend(FALSE);

		xml_add_tagstart("Image");
		xml_add_attribute_text("file",halo->bitmap_name);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Distance");
		xml_add_attribute_int("min",halo->min_dist);
		xml_add_attribute_int("max",halo->max_dist);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Size");
		xml_add_attribute_int("min",halo->min_size);
		xml_add_attribute_int("max",halo->max_size);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Alpha");
		xml_add_attribute_float("min",halo->min_alpha);
		xml_add_attribute_float("max",halo->max_alpha);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Option");
		xml_add_attribute_boolean("no_clip_object",halo->no_clip_object);
		xml_add_attribute_boolean("no_clip_self",halo->no_clip_self);
		xml_add_tagend(TRUE);

		xml_add_tagclose("Halo");

		halo++;
	}

	xml_add_tagclose("Halos");

        // write the xml
		
	file_paths_data(&file_path_setup,path,"Settings","Halos","xml");
		
	ok=xml_save_file(path,err_str);
    xml_close_file();
	
	return(ok);
}
