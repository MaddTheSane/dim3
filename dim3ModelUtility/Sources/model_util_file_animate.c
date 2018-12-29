/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Animate File Loading

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

extern file_path_setup_type	file_path_setup;

extern int model_xml_get_attribute_bone(model_type *model,int tag,char *tag_name);

// supergumba -- eventually remove this code, this is v2 of animate reading

/* =======================================================

      Read Animate XML
      
======================================================= */

bool model_read_v2_animate_xml(model_type *model)
{
	int						n,k,t,nanimate,animate_idx,
							tag,model_head,poses_tag,animations_tag,animation_tag,
							particles_head,particle_tag,rings_head,ring_tag;
    char					sub_path[1024],path[1024],pose_name[name_str_len],
							mesh_name[name_str_len],str[256];
    model_pose_move_type	*pose_move;
	model_animate_type		*animate;

        // load the animate xml
        
	sprintf(sub_path,"Models/%s",model->name);
	file_paths_data(&file_path_setup,path,sub_path,"animate","xml");
	
	if (!xml_open_file(path)) return(FALSE);

    model_head=xml_findrootchild("Model");
    if (model_head==-1) {
		xml_close_file();
		return(FALSE);
    }

        // animations
        
    animations_tag=xml_findfirstchild("Animations",model_head);

    nanimate=xml_countchildren(animations_tag);
	animation_tag=xml_findfirstchild("Animation",animations_tag);
    
    for (n=0;n!=nanimate;n++) {
 
			// add new animation

		animate_idx=model_animate_add(model);
		if (animate_idx==-1) {
			xml_close_file();
			return(FALSE);
		}

		animate=&model->animates[animate_idx];

			// set pose animation

		xml_get_attribute_text(animation_tag,"name",animate->name,64);
        
        tag=xml_findfirstchild("Loop",animation_tag);
        animate->loop=xml_get_attribute_boolean(tag,"repeat");
        animate->no_smooth=xml_get_attribute_boolean(tag,"no_smooth");
        animate->auto_stop=xml_get_attribute_boolean(tag,"auto_stop");
		animate->enhance_factor=xml_get_attribute_float_default(tag,"enhance_factor",1.0f);
        animate->loop_start=xml_get_attribute_int(tag,"start");
        animate->loop_end=xml_get_attribute_int(tag,"end");
        
			// pose moves

        poses_tag=xml_findfirstchild("Poses",animation_tag);
        
        animate->npose_move=xml_countchildren(poses_tag);
		tag=xml_findfirstchild("Pose",poses_tag);
		
        pose_move=animate->pose_moves;
        
        for (k=0;k!=animate->npose_move;k++) {
            xml_get_attribute_text(tag,"name",pose_name,name_str_len);
            pose_move->pose_idx=model_find_pose(model,pose_name);
			
            pose_move->msec=xml_get_attribute_int(tag,"time");
            xml_get_attribute_3_coord_float(tag,"sway",&pose_move->sway.x,&pose_move->sway.y,&pose_move->sway.z);
            xml_get_attribute_3_coord_float(tag,"move",&pose_move->mov.x,&pose_move->mov.y,&pose_move->mov.z);

            pose_move->acceleration=xml_get_attribute_float_default(tag,"acceleration",0.0f);

			xml_get_attribute_text(tag,"sound",pose_move->sound.name,name_str_len);
			pose_move->sound.bone_idx=model_xml_get_attribute_bone(model,tag,"sound_bone");
			pose_move->sound.pitch=xml_get_attribute_float_default(tag,"sound_pitch",1.0f);
			pose_move->sound.no_position=xml_get_attribute_boolean(tag,"sound_global");
         
			xml_get_attribute_text(tag,"mesh_fade",mesh_name,name_str_len);
			pose_move->mesh_fade.fade_in_msec=xml_get_attribute_int(tag,"mesh_fade_in_time");
			pose_move->mesh_fade.fade_life_msec=xml_get_attribute_int_default(tag,"mesh_fade_life_time",0);
			pose_move->mesh_fade.fade_out_msec=xml_get_attribute_int(tag,"mesh_fade_out_time");
			pose_move->mesh_fade.mesh_idx=model_find_mesh(model,mesh_name);

			pose_move->flash.bone_idx=model_xml_get_attribute_bone(model,tag,"flash_bone");
			pose_move->flash.intensity=xml_get_attribute_int(tag,"flash_intensity");
			pose_move->flash.flash_msec=xml_get_attribute_int(tag,"flash_time");
			pose_move->flash.fade_msec=xml_get_attribute_int(tag,"flash_fade_time");
			pose_move->flash.exponent=xml_get_attribute_float_default(tag,"flash_exponent",1.0f);
			xml_get_attribute_color(tag,"flash_color",&pose_move->flash.col);
			xml_get_attribute_text(tag,"flash_bone",str,256);

			pose_move->shake.distance=xml_get_attribute_int(tag,"shake_distance");
			pose_move->shake.size=xml_get_attribute_int(tag,"shake_size");
			pose_move->shake.life_msec=xml_get_attribute_int(tag,"shake_time");

				// particles

			pose_move->particle.count=0;

			particles_head=xml_findfirstchild("Particles",tag);
    
			if (particles_head!=-1) {
				pose_move->particle.count=xml_countchildren(particles_head);
				particle_tag=xml_findfirstchild("Particle",particles_head);
    
				for (t=0;t!=pose_move->particle.count;t++) {
					xml_get_attribute_text(particle_tag,"particle",pose_move->particle.particles[t].name,name_str_len);
					pose_move->particle.particles[t].bone_idx=model_xml_get_attribute_bone(model,particle_tag,"bone");
					pose_move->particle.particles[t].rotate=xml_get_attribute_boolean(particle_tag,"particle_rotate");
					pose_move->particle.particles[t].motion=xml_get_attribute_boolean(particle_tag,"particle_motion");
					pose_move->particle.particles[t].motion_factor=xml_get_attribute_float_default(particle_tag,"particle_motion_factor",1.0f);
					pose_move->particle.particles[t].stick=xml_get_attribute_boolean(particle_tag,"particle_stick");
					xml_get_attribute_3_coord_int(particle_tag,"particle_slop",&pose_move->particle.particles[t].slop.x,&pose_move->particle.particles[t].slop.y,&pose_move->particle.particles[t].slop.z);

					particle_tag=xml_findnextchild(particle_tag);
				}
			}

				// rings

			pose_move->ring.count=0;

			rings_head=xml_findfirstchild("Rings",tag);
    
			if (rings_head!=-1) {
				pose_move->ring.count=xml_countchildren(rings_head);
				ring_tag=xml_findfirstchild("Ring",rings_head);
    
				for (t=0;t!=pose_move->ring.count;t++) {
					xml_get_attribute_text(ring_tag,"ring",pose_move->ring.rings[t].name,name_str_len);
					pose_move->ring.rings[t].bone_idx=model_xml_get_attribute_bone(model,ring_tag,"bone");
					pose_move->ring.rings[t].angle=xml_get_attribute_boolean(ring_tag,"ring_angle");
					xml_get_attribute_3_coord_int(ring_tag,"ring_slop",&pose_move->ring.rings[t].slop.x,&pose_move->ring.rings[t].slop.y,&pose_move->ring.rings[t].slop.z);

					ring_tag=xml_findnextchild(ring_tag);
				}
			}

            pose_move++;
			tag=xml_findnextchild(tag);
        }
        
 		if (animate->loop_start<0) animate->loop_start=0;
		if (animate->loop_end==-1) animate->loop_end=animate->npose_move-1;
        if (animate->loop_end>(animate->npose_move-1)) animate->loop_end=animate->npose_move-1;
         
		animation_tag=xml_findnextchild(animation_tag);
    }

    xml_close_file();
    
    return(TRUE);
}

