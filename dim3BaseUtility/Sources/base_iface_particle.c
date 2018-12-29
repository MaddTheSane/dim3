/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Particle XML

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

      Read Particle XML
      
======================================================= */

void iface_read_settings_particle(iface_type *iface)
{
	int					n,nparticle,
						particle_data_head_tag,particle_head_tag,particle_tag,
						particle_group_head_tag,particle_group_tag,tag;
	char				path[1024];
	iface_particle_type	*particle;

		// read in particles from setting files
		
	file_paths_data(&file_path_setup,path,"Settings","Particles","xml");
	if (!xml_open_file(path)) return;
	
		// can accept file with a particle_data tag or not
		
	particle_data_head_tag=xml_findrootchild("Particle_Data");
	
		// particle and group head tags
       
	if (particle_data_head_tag==-1) {
		particle_head_tag=xml_findrootchild("Particles");
		particle_group_head_tag=xml_findrootchild("Particle_Groups");
	}
	else {
		particle_head_tag=xml_findfirstchild("Particles",particle_data_head_tag);
		particle_group_head_tag=xml_findfirstchild("Particle_Groups",particle_data_head_tag);
	}
	
    if (particle_head_tag==-1) {
		xml_close_file();
		return;
	}

		// get counts

	nparticle=xml_countchildren(particle_head_tag);
	if (particle_group_head_tag!=-1) nparticle+=xml_countchildren(particle_group_head_tag);

	if (nparticle==0) {
		xml_close_file();
		return;
	}

		// load regular particles

	particle_tag=xml_findfirstchild("Particle",particle_head_tag);

	while (particle_tag!=-1) {
	
			// create a new particle
			
		if (iface->particle_list.nparticle>=max_iface_particle) {
			xml_close_file();
			return;
		}
			
		particle=&iface->particle_list.particles[iface->particle_list.nparticle];
		iface->particle_list.nparticle++;
		
			// read in particle

		xml_get_attribute_text(particle_tag,"name",particle->name,name_str_len);
		
		particle->group.on=FALSE;

		particle->count=1;
		particle->life_msec=1000;
		particle->reverse=FALSE;
		particle->ambient_factor=1.0f;
		particle->blend_add=FALSE;
		particle->globe=FALSE;
		particle->chain_name[0]=0x0;

        tag=xml_findfirstchild("Settings",particle_tag);
        if (tag!=-1) {
            particle->count=xml_get_attribute_int(tag,"count");
            particle->life_msec=xml_get_attribute_int(tag,"time");
            particle->reverse=xml_get_attribute_boolean(tag,"reverse");
            particle->ambient_factor=xml_get_attribute_float_default(tag,"ambient",1.0f);
            particle->blend_add=xml_get_attribute_boolean(tag,"additive");
            particle->globe=xml_get_attribute_boolean(tag,"globe");
			xml_get_attribute_text(tag,"chain",particle->chain_name,name_str_len);
		}

		if (particle->count>max_particle_count) particle->count=max_particle_count;
		
		particle->animate.image_count=1;
		particle->animate.image_per_row=1;
		particle->animate.msec=200;
		particle->animate.loop=TRUE;
		particle->animate.loop_back=FALSE;
		
        tag=xml_findfirstchild("Image",particle_tag);
        if (tag!=-1) {
            xml_get_attribute_text(tag,"file",particle->bitmap_name,file_str_len);
            particle->animate.image_count=xml_get_attribute_int(tag,"count");
			particle->animate.image_per_row=(int)sqrtf((float)particle->animate.image_count);
			particle->animate.msec=xml_get_attribute_int(tag,"time");
            particle->animate.loop=xml_get_attribute_boolean(tag,"loop");
            particle->animate.loop_back=xml_get_attribute_boolean(tag,"loop_back");
		}
		
        tag=xml_findfirstchild("Trail",particle_tag);
        if (tag!=-1) {
            particle->trail_count=xml_get_attribute_int(tag,"count");
            particle->trail_step=xml_get_attribute_float(tag,"step");
            particle->reduce_pixel_fact=xml_get_attribute_float(tag,"reduce");
        }
		
		if (particle->trail_step>max_particle_trail) particle->trail_step=max_particle_trail;
		
        tag=xml_findfirstchild("Gravity",particle_tag);
        if (tag!=-1) {
            particle->start_gravity=xml_get_attribute_float(tag,"start");
            particle->gravity_add=xml_get_attribute_float(tag,"add");
        }
		
        tag=xml_findfirstchild("Size",particle_tag);
        if (tag!=-1) {
            particle->start_pixel_size=xml_get_attribute_int(tag,"start");
            particle->end_pixel_size=xml_get_attribute_int(tag,"end");
        }

		tag=xml_findfirstchild("X",particle_tag);
		if (tag!=-1) {
			particle->pt.x=xml_get_attribute_int(tag,"offset");
			particle->vct.x=xml_get_attribute_float(tag,"move");
			particle->rot.x=xml_get_attribute_float_default(tag,"rot",0.0f);
			particle->rot_accel.x=xml_get_attribute_float_default(tag,"rot_accel",0.0f);
		}

		tag=xml_findfirstchild("Y",particle_tag);
		if (tag!=-1) {
			particle->pt.y=xml_get_attribute_int(tag,"offset");
			particle->vct.y=xml_get_attribute_float(tag,"move");
			particle->rot.y=xml_get_attribute_float_default(tag,"rot",0.0f);
			particle->rot_accel.y=xml_get_attribute_float_default(tag,"rot_accel",0.0f);
		}

		tag=xml_findfirstchild("Z",particle_tag);
		if (tag!=-1) {
			particle->pt.z=xml_get_attribute_int(tag,"offset");
			particle->vct.z=xml_get_attribute_float(tag,"move");
			particle->rot.z=xml_get_attribute_float_default(tag,"rot",0.0f);
			particle->rot_accel.z=xml_get_attribute_float_default(tag,"rot_accel",0.0f);
		}

		particle->start_color.r=particle->start_color.g=particle->start_color.b=1.0f;
		particle->end_color.r=particle->end_color.g=particle->end_color.b=1.0f;

        tag=xml_findfirstchild("Color",particle_tag);
        if (tag!=-1) {
			xml_get_attribute_color(tag,"start",&particle->start_color);
			xml_get_attribute_color(tag,"end",&particle->end_color);
        }
		
        tag=xml_findfirstchild("Alpha",particle_tag);
        if (tag!=-1) {
            particle->start_alpha=xml_get_attribute_float(tag,"start");
            particle->end_alpha=xml_get_attribute_float(tag,"end");
        }
	
			// move on to next particle
		
		particle_tag=xml_findnextchild(particle_tag);
	}

		// load group particles

    if (particle_group_head_tag==-1) {
		xml_close_file();
		return;
	}

  	particle_group_tag=xml_findfirstchild("Particle_Group",particle_group_head_tag);
   
	while (particle_group_tag!=-1) {
	
			// create a new particle group
			
		if (iface->particle_list.nparticle>=max_iface_particle) {
			xml_close_file();
			return;
		}
			
		particle=&iface->particle_list.particles[iface->particle_list.nparticle];
		iface->particle_list.nparticle++;
		
			// read in particle group
		
		xml_get_attribute_text(particle_group_tag,"name",particle->name,name_str_len);
		
		particle->group.on=TRUE;
		particle->group.count=0;

		particle->count=0;

        particle_head_tag=xml_findfirstchild("Particles",particle_group_tag);
        if (particle_head_tag!=-1) {

			particle->group.count=xml_countchildren(particle_head_tag);
			if (particle->group.count>max_particle_group) particle->group.count=max_particle_group;

			particle_tag=xml_findfirstchild("Particle",particle_head_tag);
    
			for (n=0;n!=particle->group.count;n++) {
				xml_get_attribute_text(particle_tag,"name",particle->group.particles[n].name,name_str_len);
				particle->group.particles[n].shift=xml_get_attribute_int_default(particle_tag,"offset",0);
				particle_tag=xml_findnextchild(particle_tag);
			}
		}

			// move on to next particle group
		
		particle_group_tag=xml_findnextchild(particle_group_tag);
	}
	
	xml_close_file();
}

/* =======================================================

      Write Particles XML
      
======================================================= */

bool iface_write_settings_particle(iface_type *iface,char *err_str)
{
	int						n,k;
	char					path[1024];
	bool					ok;
	iface_particle_type		*particle;
	
		// start new file
		
	xml_new_file();

	xml_add_tagstart("Particle_Data");
	xml_add_tagend(FALSE);

		// particles

	xml_add_tagstart("Particles");
	xml_add_tagend(FALSE);

	for (n=0;n!=iface->particle_list.nparticle;n++) {

		particle=&iface->particle_list.particles[n];
		if (particle->group.on) continue;

		xml_add_tagstart("Particle");
		xml_add_attribute_text("name",particle->name);
		xml_add_tagend(FALSE);

		xml_add_tagstart("Settings");
		xml_add_attribute_int("count",particle->count);
		xml_add_attribute_int("time",particle->life_msec);
		xml_add_attribute_boolean("reverse",particle->reverse);
		xml_add_attribute_float("ambient",particle->ambient_factor);
		xml_add_attribute_boolean("additive",particle->blend_add);
		xml_add_attribute_boolean("globe",particle->globe);
		xml_add_attribute_text("chain",particle->chain_name);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Image");
		xml_add_attribute_text("file",particle->bitmap_name);
		xml_add_attribute_int("count",particle->animate.image_count);
		xml_add_attribute_int("time",particle->animate.msec);
		xml_add_attribute_boolean("loop",particle->animate.loop);
		xml_add_attribute_boolean("loop_back",particle->animate.loop_back);
		xml_add_tagend(TRUE);
		
		xml_add_tagstart("Trail");
		xml_add_attribute_int("count",particle->trail_count);
		xml_add_attribute_float("step",particle->trail_step);
		xml_add_attribute_float("reduce",particle->reduce_pixel_fact);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Gravity");
		xml_add_attribute_float("start",particle->start_gravity);
		xml_add_attribute_float("add",particle->gravity_add);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Size");
		xml_add_attribute_int("start",particle->start_pixel_size);
		xml_add_attribute_int("end",particle->end_pixel_size);
		xml_add_tagend(TRUE);

		xml_add_tagstart("X");
		xml_add_attribute_int("offset",particle->pt.x);
		xml_add_attribute_float("move",particle->vct.x);
		xml_add_attribute_float("rot",particle->rot.x);
		xml_add_attribute_float("rot_accel",particle->rot_accel.x);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Y");
		xml_add_attribute_int("offset",particle->pt.y);
		xml_add_attribute_float("move",particle->vct.y);
		xml_add_attribute_float("rot",particle->rot.y);
		xml_add_attribute_float("rot_accel",particle->rot_accel.y);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Z");
		xml_add_attribute_int("offset",particle->pt.z);
		xml_add_attribute_float("move",particle->vct.z);
		xml_add_attribute_float("rot",particle->rot.z);
		xml_add_attribute_float("rot_accel",particle->rot_accel.z);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Color");
		xml_add_attribute_color("start",&particle->start_color);
		xml_add_attribute_color("end",&particle->end_color);
		xml_add_tagend(TRUE);

		xml_add_tagstart("Alpha");
		xml_add_attribute_float("start",particle->start_alpha);
		xml_add_attribute_float("end",particle->end_alpha);
		xml_add_tagend(TRUE);

		xml_add_tagclose("Particle");
	}

	xml_add_tagclose("Particles");

		// particle groups

	xml_add_tagstart("Particle_Groups");
	xml_add_tagend(FALSE);

	for (n=0;n!=iface->particle_list.nparticle;n++) {

		particle=&iface->particle_list.particles[n];
		if (!particle->group.on) continue;

		xml_add_tagstart("Particle_Group");
		xml_add_attribute_text("name",particle->name);
		xml_add_tagend(FALSE);

		xml_add_tagstart("Particles");
		xml_add_tagend(FALSE);

		for (k=0;k!=particle->group.count;k++) {
			xml_add_tagstart("Particle");
			xml_add_attribute_text("name",particle->group.particles[k].name);
			xml_add_attribute_int("offset",particle->group.particles[k].shift);
			xml_add_tagend(TRUE);
		}

		xml_add_tagclose("Particles");

		xml_add_tagclose("Particle_Group");
	}

	xml_add_tagclose("Particle_Groups");

		// close particle data

	xml_add_tagclose("Particle_Data");

        // write the xml
		
	file_paths_data(&file_path_setup,path,"Settings","Particles","xml");
		
	ok=xml_save_file(path,err_str);
    xml_close_file();
	
	return(ok);
}
