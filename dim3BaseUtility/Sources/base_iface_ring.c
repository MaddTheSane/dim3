/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Ring XML

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

      Read Ring XML
      
======================================================= */

void iface_read_settings_ring(iface_type *iface)
{
	int					nring,ring_head_tag,ring_tag,tag;
	char				path[1024];
	iface_ring_type		*ring;

		// read in rings from setting files
		
	file_paths_data(&file_path_setup,path,"Settings","Rings","xml");
	if (!xml_open_file(path)) return;
	
		// get counts
       
    ring_head_tag=xml_findrootchild("Rings");
    if (ring_head_tag==-1) {
		xml_close_file();
		return;
	}

	nring=xml_countchildren(ring_head_tag);

	if (nring==0) {
		xml_close_file();
		return;
	}

		// read in the rings

	ring_tag=xml_findfirstchild("Ring",ring_head_tag);
    
	while (ring_tag!=-1) {
	
			// create a new ring

		if (iface->ring_list.nring>=max_iface_ring) {
			xml_close_file();
			return;
		}
			
		ring=&iface->ring_list.rings[iface->ring_list.nring];
		iface->ring_list.nring++;
		
			// read in ring
		
		xml_get_attribute_text(ring_tag,"name",ring->name,name_str_len);
		
		ring->life_msec=1000;
		ring->blend_add=FALSE;

        tag=xml_findfirstchild("Settings",ring_tag);
        if (tag!=-1) {
            ring->life_msec=xml_get_attribute_int(tag,"time");
            ring->blend_add=xml_get_attribute_boolean(tag,"additive");
        }
		
		ring->animate.image_count=1;
		ring->animate.image_per_row=1;
		ring->animate.msec=200;
		ring->animate.loop=TRUE;
		ring->animate.loop_back=FALSE;
		
        tag=xml_findfirstchild("Image",ring_tag);
        if (tag!=-1) {
            xml_get_attribute_text(tag,"file",ring->bitmap_name,file_str_len);
            ring->animate.image_count=xml_get_attribute_int(tag,"count");
			ring->animate.image_per_row=(int)sqrtf((float)ring->animate.image_count);
			ring->animate.msec=xml_get_attribute_int(tag,"time");
            ring->animate.loop=xml_get_attribute_boolean(tag,"loop");
            ring->animate.loop_back=xml_get_attribute_boolean(tag,"loop_back");
		}
		
        tag=xml_findfirstchild("Outer",ring_tag);
        if (tag!=-1) {
            ring->start_outer_size=xml_get_attribute_int(tag,"start");
            ring->end_outer_size=xml_get_attribute_int(tag,"end");
        }

        tag=xml_findfirstchild("Inner",ring_tag);
        if (tag!=-1) {
            ring->start_inner_size=xml_get_attribute_int(tag,"start");
            ring->end_inner_size=xml_get_attribute_int(tag,"end");
        }

        tag=xml_findfirstchild("Rotate",ring_tag);
        if (tag!=-1) {
            ring->ang.x=xml_get_attribute_float(tag,"x");
            ring->ang.y=xml_get_attribute_float(tag,"y");
            ring->ang.z=xml_get_attribute_float(tag,"z");
            ring->rot.x=xml_get_attribute_float_default(tag,"x_add",0.0f);
            ring->rot.y=xml_get_attribute_float_default(tag,"y_add",0.0f);
            ring->rot.z=xml_get_attribute_float_default(tag,"z_add",0.0f);
            ring->rot_accel.x=xml_get_attribute_float_default(tag,"x_accel",0.0f);
            ring->rot_accel.y=xml_get_attribute_float_default(tag,"y_accel",0.0f);
            ring->rot_accel.z=xml_get_attribute_float_default(tag,"z_accel",0.0f);
		}

        tag=xml_findfirstchild("Move",ring_tag);
        if (tag!=-1) {
            ring->vct.x=xml_get_attribute_float(tag,"x");
            ring->vct.y=xml_get_attribute_float(tag,"y");
            ring->vct.z=xml_get_attribute_float(tag,"z");
        }
		
		ring->start_color.r=ring->start_color.g=ring->start_color.b=1.0f;
		ring->end_color.r=ring->end_color.g=ring->end_color.b=1.0f;

        tag=xml_findfirstchild("Color",ring_tag);
        if (tag!=-1) {
			xml_get_attribute_color(tag,"start",&ring->start_color);
			xml_get_attribute_color(tag,"end",&ring->end_color);
       }
		
        tag=xml_findfirstchild("Alpha",ring_tag);
        if (tag!=-1) {
            ring->start_alpha=xml_get_attribute_float(tag,"start");
            ring->end_alpha=xml_get_attribute_float(tag,"end");
        }
	
			// move on to next ring
			
		ring_tag=xml_findnextchild(ring_tag);
	}
	
	xml_close_file();
}

/* =======================================================

      Write Rings XML
      
======================================================= */

bool iface_write_settings_ring(iface_type *iface,char *err_str)
{
	int					n;
	char				path[1024];
	bool				ok;
	iface_ring_type		*ring;
	
		// start new file
		
	xml_new_file();

	xml_add_tagstart("Rings");
	xml_add_tagend(FALSE);

	ring=iface->ring_list.rings;

	for (n=0;n!=iface->ring_list.nring;n++) {

		xml_add_tagstart("Ring");
		xml_add_attribute_text("name",ring->name);
		xml_add_tagend(FALSE);
		
  		xml_add_tagstart("Settings");
		xml_add_attribute_int("time",ring->life_msec);
		xml_add_attribute_boolean("additive",ring->blend_add);
		xml_add_tagend(TRUE);
		
        xml_add_tagstart("Image");
		xml_add_attribute_text("file",ring->bitmap_name);
		xml_add_attribute_int("count",ring->animate.image_count);
		xml_add_attribute_int("time",ring->animate.msec);
		xml_add_attribute_boolean("loop",ring->animate.loop);
		xml_add_attribute_boolean("loop_back",ring->animate.loop_back);
		xml_add_tagend(TRUE);
		
        xml_add_tagstart("Outer");
		xml_add_attribute_int("start",ring->start_outer_size);
		xml_add_attribute_int("end",ring->end_outer_size);
		xml_add_tagend(TRUE);

        xml_add_tagstart("Inner");
		xml_add_attribute_int("start",ring->start_inner_size);
		xml_add_attribute_int("end",ring->end_inner_size);
		xml_add_tagend(TRUE);

        xml_add_tagstart("Rotate");
		xml_add_attribute_float("x",ring->ang.x);
		xml_add_attribute_float("y",ring->ang.y);
		xml_add_attribute_float("z",ring->ang.z);
		xml_add_attribute_float("x_add",ring->rot.x);
		xml_add_attribute_float("y_add",ring->rot.y);
		xml_add_attribute_float("z_add",ring->rot.z);
		xml_add_attribute_float("x_accel",ring->rot_accel.x);
		xml_add_attribute_float("y_accel",ring->rot_accel.y);
		xml_add_attribute_float("z_accel",ring->rot_accel.z);
		xml_add_tagend(TRUE);

        xml_add_tagstart("Move");
		xml_add_attribute_float("x",ring->vct.x);
		xml_add_attribute_float("y",ring->vct.y);
		xml_add_attribute_float("z",ring->vct.z);
		xml_add_tagend(TRUE);

        xml_add_tagstart("Color");
		xml_add_attribute_color("start",&ring->start_color);
		xml_add_attribute_color("end",&ring->end_color);
		xml_add_tagend(TRUE);

        xml_add_tagstart("Alpha");
        xml_add_attribute_float("start",ring->start_alpha);
        xml_add_attribute_float("end",ring->end_alpha);
		xml_add_tagend(TRUE);
	
		xml_add_tagclose("Ring");

		ring++;
	}

	xml_add_tagclose("Rings");

        // write the xml
		
	file_paths_data(&file_path_setup,path,"Settings","Rings","xml");
		
	ok=xml_save_file(path,err_str);
    xml_close_file();
	
	return(ok);
}
