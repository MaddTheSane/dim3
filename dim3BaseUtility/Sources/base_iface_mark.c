/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Mark XML

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

      Read Marks XML
      
======================================================= */

void iface_read_settings_mark(iface_type *iface)
{
	int					nmark,marks_head_tag,mark_tag,tag;
	char				path[1024];
	iface_mark_type		*mark;

		// read in interface from setting files
		
	file_paths_data(&file_path_setup,path,"Settings","Marks","xml");
	if (!xml_open_file(path)) return;
	
		// get counts
		
    marks_head_tag=xml_findrootchild("Marks");
    if (marks_head_tag==-1) {
		xml_close_file();
		return;
	}

	nmark=xml_countchildren(marks_head_tag);

	if (nmark==0) {
		xml_close_file();
		return;
	}

		// read the marks
	
	mark_tag=xml_findfirstchild("Mark",marks_head_tag);
	
	while (mark_tag!=-1) {
	
			// create a new mark
			
		if (iface->mark_list.nmark>=max_iface_mark) {
			xml_close_file();
			return;
		}
		
		mark=&iface->mark_list.marks[iface->mark_list.nmark];
		iface->mark_list.nmark++;
			
			// read settings
		
		xml_get_attribute_text(mark_tag,"name",mark->name,name_str_len);
		
		tag=xml_findfirstchild("Setting",mark_tag);
		if (tag!=-1) {
			mark->life_msec=xml_get_attribute_int(tag,"time");
			mark->fade_in_msec=xml_get_attribute_int(tag,"fade_in");
			mark->fade_out_msec=xml_get_attribute_int(tag,"fade_out");
			mark->no_rotate=xml_get_attribute_boolean(tag,"no_rotate");
			mark->no_transparent=xml_get_attribute_boolean(tag,"no_transparent");
			mark->no_opaque=xml_get_attribute_boolean(tag,"no_opaque");
            mark->hilite=xml_get_attribute_boolean(tag,"highlight");
            mark->blend_add=xml_get_attribute_boolean(tag,"additive");
		}
		else {
			mark->life_msec=1000;
			mark->fade_in_msec=mark->fade_out_msec=500;
			mark->no_rotate=FALSE;
			mark->no_transparent=FALSE;
			mark->no_opaque=FALSE;
			mark->hilite=FALSE;
			mark->blend_add=FALSE;
		}
		
		mark->total_msec=mark->life_msec+mark->fade_in_msec+mark->fade_out_msec;
		
		mark->animate.image_count=1;
		mark->animate.image_per_row=1;
		mark->animate.msec=200;
		mark->animate.loop=TRUE;
		mark->animate.loop_back=FALSE;
	
		tag=xml_findfirstchild("Image",mark_tag);
		if (tag!=-1) {
			xml_get_attribute_text(tag,"file",mark->bitmap_name,file_str_len);
			mark->animate.image_count=xml_get_attribute_int(tag,"count");
			mark->animate.image_per_row=(int)sqrtf((float)mark->animate.image_count);
			mark->animate.msec=xml_get_attribute_int(tag,"time");
			mark->animate.loop=xml_get_attribute_boolean(tag,"loop");
			mark->animate.loop_back=xml_get_attribute_boolean(tag,"loop_back");
		}
	
		mark->color.r=mark->color.g=mark->color.b=1.0f;

        tag=xml_findfirstchild("Color",mark_tag);
        if (tag!=-1) {
			xml_get_attribute_color(tag,"color",&mark->color);
        }

			// move on to next mark
			
		mark_tag=xml_findnextchild(mark_tag);
	}
	
	xml_close_file();
}

/* =======================================================

      Write Mark XML
      
======================================================= */

bool iface_write_settings_mark(iface_type *iface,char *err_str)
{
	int					n;
	char				path[1024];
	bool				ok;
	iface_mark_type		*mark;
	
		// start new file
		
	xml_new_file();

	xml_add_tagstart("Marks");
	xml_add_tagend(FALSE);

	mark=iface->mark_list.marks;

	for (n=0;n!=iface->mark_list.nmark;n++) {

		xml_add_tagstart("Mark");
		xml_add_attribute_text("name",mark->name);
		xml_add_tagend(FALSE);
		
		xml_add_tagstart("Setting");
		xml_add_attribute_int("time",mark->life_msec);
		xml_add_attribute_int("fade_in",mark->fade_in_msec);
		xml_add_attribute_int("fade_out",mark->fade_out_msec);
		xml_add_attribute_boolean("no_rotate",mark->no_rotate);
		xml_add_attribute_boolean("no_transparent",mark->no_transparent);
		xml_add_attribute_boolean("no_opaque",mark->no_opaque);
		xml_add_attribute_boolean("highlight",mark->hilite);
		xml_add_attribute_boolean("additive",mark->blend_add);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Image");
		xml_add_attribute_text("file",mark->bitmap_name);
		xml_add_attribute_int("count",mark->animate.image_count);
		xml_add_attribute_int("time",mark->animate.msec);
		xml_add_attribute_boolean("loop",mark->animate.loop);
		xml_add_attribute_boolean("loop_back",mark->animate.loop_back);
		xml_add_tagend(TRUE);
	
		xml_add_tagstart("Color");
		xml_add_attribute_color("color",&mark->color);
        xml_add_tagend(TRUE);

		xml_add_tagclose("Mark");

		mark++;
	}

	xml_add_tagclose("Marks");

        // write the xml
		
	file_paths_data(&file_path_setup,path,"Settings","Marks","xml");
		
	ok=xml_save_file(path,err_str);
    xml_close_file();
	
	return(ok);
}
