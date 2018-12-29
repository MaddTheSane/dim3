/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Model XML

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
	#include "dim3modelutility.h"
#endif

extern file_path_setup_type	file_path_setup;

/* =======================================================

      Model Read XML Utilities
      
======================================================= */

int model_read_xml_bone(model_type *model,int tag,char *attrib_name)
{
	char			name[name_str_len];

	xml_get_attribute_text(tag,attrib_name,name,name_str_len);
	if (name[0]==0x0) return(-1);

	return(model_find_bone(model,name));
}

/* =======================================================

      Model Decode XML
      
======================================================= */

bool model_read_xml(model_type *model)
{
	int						n,k,t,version,model_head,
							bone_idx,nbone,hit_box_idx,nhit_box,
							label_tag,ui_tag,mesh_idx,nmesh,nfill,
							tag,light_tag,halo_tag,hit_box_tag,
							rigid_body_tag,meshes_tag,mesh_tag,
							vertex_tag,poly_tag,bone_tag,fills_tag,fill_tag,
							particles_head,particle_tag,rings_head,ring_tag,
							npose,poses_tag,pose_tag,pose_idx,cnt,
							nanimate,animations_tag,animation_tag,animate_idx,
							sound_tag,fade_tag,flash_tag,shake_tag;
	char					sub_path[1024],path[1024],
							pose_name[name_str_len],mesh_name[name_str_len],
							bone_parent_name[max_model_bone][name_str_len];
	model_hit_box_type		*hit_box;
	model_mesh_type			*mesh;
    model_vertex_type		*vertex;
    model_bone_type			*bone;
    model_poly_type			*poly;
    texture_type			*texture;
	model_pose_type			*pose;
    model_bone_move_type	*bone_move;
	model_animate_type		*animate;
    model_pose_move_type	*pose_move;

        // load the model xml
		
	sprintf(sub_path,"Models/%s",model->name);
	file_paths_data(&file_path_setup,path,sub_path,"model","xml");
	
	if (!xml_open_file(path)) return(FALSE);

		// header and version

    model_head=xml_findrootchild("Model");
    if (model_head==-1) {
		xml_close_file();
		return(FALSE);
	}
	
	version=xml_get_attribute_int_default(model_head,"version",model_current_version);
	if (version!=model_current_version) {
		xml_close_file();
		return(FALSE);
	}

        // options
    
    tag=xml_findfirstchild("Options",model_head);
	model->comulative_rotation=xml_get_attribute_boolean(tag,"comulative_rotation");
	
        // center
    
    tag=xml_findfirstchild("Center",model_head);
	xml_get_attribute_3_coord_int(tag,"offset",&model->center.x,&model->center.y,&model->center.z);
    
        // boxes
    
    tag=xml_findfirstchild("View_Box",model_head);
	xml_get_attribute_3_coord_int(tag,"size",&model->view_box.size.x,&model->view_box.size.y,&model->view_box.size.z);
	xml_get_attribute_3_coord_int(tag,"offset",&model->view_box.offset.x,&model->view_box.offset.y,&model->view_box.offset.z);

        // bones
 
    bone_tag=xml_findfirstchild("Bones",model_head);

    nbone=xml_countchildren(bone_tag);
	tag=xml_findfirstchild("Bone",bone_tag);
    
    for (n=0;n!=nbone;n++) {

 			// add new bone

		bone_idx=model_bone_add(model,0,0,0);
		if (bone_idx==-1) break;

			// fill in bone

		bone=&model->bones[bone_idx];
		
		xml_get_attribute_text(tag,"name",bone->name,name_str_len);
        xml_get_attribute_3_coord_int(tag,"c3",&bone->pnt.x,&bone->pnt.y,&bone->pnt.z);
        xml_get_attribute_3_coord_int(tag,"o3",&bone->natural_offset.x,&bone->natural_offset.y,&bone->natural_offset.z);
		xml_get_attribute_3_coord_float(tag,"r3",&bone->natural_rot.x,&bone->natural_rot.y,&bone->natural_rot.z);
  		xml_get_attribute_text(tag,"parent",bone_parent_name[bone_idx],name_str_len);
   
		tag=xml_findnextchild(tag);
    }

		// fix parent bones
		
	bone=model->bones;
	
	for (n=0;n!=model->nbone;n++) {
		if (bone_parent_name[n][0]==0x0) {
			bone->parent_idx=-1;
		}
		else {
			bone->parent_idx=model_find_bone(model,bone_parent_name[n]);
		}
		bone++;
	}
	
        // light

    light_tag=xml_findfirstchild("Lights",model_head);
	tag=xml_findfirstchild("Light",light_tag);

	for (n=0;n!=max_model_light;n++) {
		model->bone_connect.light_bone_idx[n]=model_read_xml_bone(model,tag,"bone");
		tag=xml_findnextchild(tag);
	}

    halo_tag=xml_findfirstchild("Halos",model_head);
	tag=xml_findfirstchild("Halo",halo_tag);

	for (n=0;n!=max_model_halo;n++) {
		model->bone_connect.halo_bone_idx[n]=model_read_xml_bone(model,tag,"bone");
		tag=xml_findnextchild(tag);
	}

		// name bone

    tag=xml_findfirstchild("Name",model_head);
	if (tag!=-1) model->bone_connect.name_bone_idx=model_read_xml_bone(model,tag,"bone");

		// labels

    label_tag=xml_findfirstchild("Label",model_head);
    if (label_tag!=-1) {
		tag=xml_findfirstchild("Text",label_tag);
		if (tag!=-1) {
			model->label.text.size=xml_get_attribute_int(tag,"size");
			xml_get_attribute_color(tag,"color",&model->label.text.col);
			model->label.text.bone_idx=model_read_xml_bone(model,tag,"bone");
		}

		tag=xml_findfirstchild("Bitmap",label_tag);
		if (tag!=-1) {
			model->label.bitmap.size=xml_get_attribute_int(tag,"size");
			model->label.bitmap.bone_idx=model_read_xml_bone(model,tag,"bone");
		}

		tag=xml_findfirstchild("Bar",label_tag);
		if (tag!=-1) {
			model->label.bar.wid=xml_get_attribute_int(tag,"wid");
			model->label.bar.high=xml_get_attribute_int(tag,"high");
			model->label.bar.border_on=xml_get_attribute_boolean(tag,"border_on");
			model->label.bar.background_on=xml_get_attribute_boolean(tag,"background_on");
			xml_get_attribute_color(tag,"border_color",&model->label.bar.border_col);
			xml_get_attribute_color(tag,"background_color",&model->label.bar.background_col);
			xml_get_attribute_color(tag,"bar_color",&model->label.bar.bar_col);
			model->label.bar.bone_idx=model_read_xml_bone(model,tag,"bone");
		}
	}
	
        // hit boxes
    
    hit_box_tag=xml_findfirstchild("Hit_Boxes",model_head);
	if (hit_box_tag!=-1) {
	
		nhit_box=xml_countchildren(hit_box_tag);
		tag=xml_findfirstchild("Hit_Box",hit_box_tag);
		
		for (n=0;n!=nhit_box;n++) {
 				
				// add new hit box

			hit_box_idx=model_hit_box_add(model);
			if (hit_box_idx==-1) break;

				// fill in hit box

			hit_box=&model->hit_boxes[hit_box_idx];

			xml_get_attribute_text(tag,"name",hit_box->name,64);
			hit_box->bone_idx=model_read_xml_bone(model,tag,"bone");
			xml_get_attribute_3_coord_int(tag,"size",&hit_box->box.size.x,&hit_box->box.size.y,&hit_box->box.size.z);
			xml_get_attribute_3_coord_int(tag,"offset",&hit_box->box.offset.x,&hit_box->box.offset.y,&hit_box->box.offset.z);
			
			tag=xml_findnextchild(tag);
		}
    }

		// rigid body

    rigid_body_tag=xml_findfirstchild("Rigid_Body",model_head);
	model->rigid_body.on=xml_get_attribute_boolean(rigid_body_tag,"on");
	model->rigid_body.y.max_ang=0.0f;			// unused, but in struct
	model->rigid_body.y.reset_factor=xml_get_attribute_float_default(rigid_body_tag,"y_factor",0.8f);
	model->rigid_body.y.smooth_factor=xml_get_attribute_float_default(rigid_body_tag,"y_smooth",0.2f);
	model->rigid_body.x.max_ang=xml_get_attribute_float_default(rigid_body_tag,"x_max_ang",45.0f);
	model->rigid_body.x.reset_factor=xml_get_attribute_float_default(rigid_body_tag,"x_factor",0.8f);
	model->rigid_body.x.smooth_factor=xml_get_attribute_float_default(rigid_body_tag,"x_smooth",0.2f);
	model->rigid_body.z.max_ang=xml_get_attribute_float_default(rigid_body_tag,"z_max_ang",45.0f);
	model->rigid_body.z.reset_factor=xml_get_attribute_float_default(rigid_body_tag,"z_factor",0.8f);
	model->rigid_body.z.smooth_factor=xml_get_attribute_float_default(rigid_body_tag,"z_smooth",0.2f);
	
  		// ui
		
    ui_tag=xml_findfirstchild("UI",model_head);
	model->ui.light_intensity=xml_get_attribute_int(ui_tag,"light_intensity");
	model->ui.light_exponent=xml_get_attribute_float(ui_tag,"light_exponent");
	xml_get_attribute_3_coord_int(ui_tag,"light_offset",&model->ui.light_offset.x,&model->ui.light_offset.y,&model->ui.light_offset.z);
	xml_get_attribute_color(ui_tag,"light_color",&model->ui.light_col);

		// meshes
		
	model->nmesh=0;
		
	meshes_tag=xml_findfirstchild("Meshes",model_head);

    nmesh=xml_countchildren(meshes_tag);
	mesh_tag=xml_findfirstchild("Mesh",meshes_tag);
	
	for (n=0;n!=nmesh;n++) {
	
		mesh_idx=model_mesh_add(model);
		if (mesh_idx==-1) break;
	
			// mesh settings
			
		mesh=&model->meshes[mesh_idx];
		xml_get_attribute_text(mesh_tag,"name",mesh->name,name_str_len);
		mesh->no_lighting=xml_get_attribute_boolean(mesh_tag,"no_lighting");
		mesh->diffuse=xml_get_attribute_boolean(mesh_tag,"diffuse");
		mesh->blend_add=xml_get_attribute_boolean(mesh_tag,"additive");
		mesh->never_cull=xml_get_attribute_boolean(mesh_tag,"never_cull");
		mesh->locked=xml_get_attribute_boolean(mesh_tag,"locked");
		mesh->rt_non_light_blocking=xml_get_attribute_boolean(mesh_tag,"rt_non_light_blocking");
		xml_get_attribute_3_coord_int(mesh_tag,"import_move",&mesh->import_move.x,&mesh->import_move.y,&mesh->import_move.z);
		memmove(&mesh->org_import_move,&mesh->import_move,sizeof(d3pnt));
		
			// vertexes
		
		vertex_tag=xml_findfirstchild("Vertexes",mesh_tag);

		mesh->nvertex=xml_countchildren(vertex_tag);
		model_mesh_set_vertex_count(model,mesh_idx,mesh->nvertex);
		
		vertex=mesh->vertexes;
		tag=xml_findfirstchild("v",vertex_tag);
		
		for (k=0;k!=mesh->nvertex;k++) {
			xml_get_attribute_3_coord_int(tag,"c3",&vertex->pnt.x,&vertex->pnt.y,&vertex->pnt.z);
			xml_get_attribute_3_coord_float(tag,"t3",&vertex->tangent_space.tangent.x,&vertex->tangent_space.tangent.y,&vertex->tangent_space.tangent.z);
			xml_get_attribute_3_coord_float(tag,"n3",&vertex->tangent_space.normal.x,&vertex->tangent_space.normal.y,&vertex->tangent_space.normal.z);

			vertex->major_bone_idx=model_read_xml_bone(model,tag,"major");
			vertex->minor_bone_idx=model_read_xml_bone(model,tag,"minor");
			vertex->bone_factor=xml_get_attribute_float_default(tag,"factor",1.0f);
			
			vector_normalize(&vertex->tangent_space.tangent);
			vector_normalize(&vertex->tangent_space.normal);
		
			vertex++;
			tag=xml_findnextchild(tag);
		}
		
			// polys

		poly_tag=xml_findfirstchild("Polys",mesh_tag);

		mesh->npoly=xml_countchildren(poly_tag);
		model_mesh_set_poly_count(model,mesh_idx,mesh->npoly);
		
		poly=mesh->polys;
		tag=xml_findfirstchild("p",poly_tag);
		
		for (k=0;k!=mesh->npoly;k++) {
			poly->ptsz=xml_get_attribute_int_array(tag,"v",poly->v,8);
			poly->txt_idx=xml_get_attribute_int(tag,"txt");
			xml_get_attribute_float_array(tag,"x",poly->gx,8);
			xml_get_attribute_float_array(tag,"y",poly->gy,8);

			poly++;
			tag=xml_findnextchild(tag);
		}

		mesh_tag=xml_findnextchild(mesh_tag);
	}
  
        // fills

    fills_tag=xml_findfirstchild("Fills",model_head);
	nfill=xml_countchildren(fills_tag);

	texture=model->textures;
	
	fill_tag=xml_findfirstchild("Fill",fills_tag);
    
    for (n=0;n!=nfill;n++) {
	
		texture->animate.current_frame=0;
		texture->animate.total_wait=0;
		
		bitmap_texture_read_xml(texture,fill_tag,FALSE);
		texture++;
		
		fill_tag=xml_findnextchild(fill_tag);
    }
	
        // poses
        
    poses_tag=xml_findfirstchild("Poses",model_head);

    npose=xml_countchildren(poses_tag);
	pose_tag=xml_findfirstchild("Pose",poses_tag);
    
    for (n=0;n!=npose;n++) {

			// add new pose

		pose_idx=model_pose_add(model);
		if (pose_idx==-1) {
			xml_close_file();
			return(FALSE);
		}

		pose=&model->poses[pose_idx];

			// set pose data

        xml_get_attribute_text(pose_tag,"name",pose->name,64);
        
        bone_move=pose->bone_moves;
        for (k=0;k!=model->nbone;k++) {
            bone_move->rot.x=bone_move->rot.z=bone_move->rot.y=0.0f;
            bone_move->mov.x=bone_move->mov.z=bone_move->mov.y=1.0f;
			bone_move->acceleration=0.0f;
			bone_move->skip_blended=FALSE;
			bone_move->constraint.bone_idx=-1;
			bone_move->constraint.offset.x=bone_move->constraint.offset.y=bone_move->constraint.offset.z=0;
			bone_move++;
        }
            
        bone_tag=xml_findfirstchild("Bones",pose_tag);
        
        cnt=xml_countchildren(bone_tag);
		tag=xml_findfirstchild("Bone",bone_tag);
        
        for (k=0;k!=cnt;k++) {
			
			bone_idx=model_read_xml_bone(model,tag,"name");
		
            if (bone_idx!=-1) {
                bone_move=&pose->bone_moves[bone_idx];
                xml_get_attribute_3_coord_float(tag,"rot",&bone_move->rot.x,&bone_move->rot.y,&bone_move->rot.z);
				xml_get_attribute_3_coord_float(tag,"move",&bone_move->mov.x,&bone_move->mov.y,&bone_move->mov.z);
				
				bone_move->acceleration=xml_get_attribute_float(tag,"acceleration");
				bone_move->skip_blended=xml_get_attribute_boolean(tag,"skip_blended");
				
				bone_move->constraint.bone_idx=model_read_xml_bone(model,tag,"constraint_bone");
				xml_get_attribute_3_coord_int(tag,"constraint_offset",&bone_move->constraint.offset.x,&bone_move->constraint.offset.y,&bone_move->constraint.offset.z);
			}
		  
			tag=xml_findnextchild(tag);
        }
    
		pose_tag=xml_findnextchild(pose_tag);
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

				// sound
				
			sound_tag=xml_findfirstchild("Sound",tag);
			xml_get_attribute_text(sound_tag,"name",pose_move->sound.name,name_str_len);
			pose_move->sound.bone_idx=model_read_xml_bone(model,sound_tag,"bone");
			pose_move->sound.pitch=xml_get_attribute_float_default(sound_tag,"pitch",1.0f);
			pose_move->sound.no_position=xml_get_attribute_boolean(sound_tag,"global");
         
				// fade
				
			fade_tag=xml_findfirstchild("Fade",tag);
			xml_get_attribute_text(fade_tag,"mesh",mesh_name,name_str_len);
			pose_move->mesh_fade.mesh_idx=model_find_mesh(model,mesh_name);
			pose_move->mesh_fade.fade_in_msec=xml_get_attribute_int(fade_tag,"in_time");
			pose_move->mesh_fade.fade_life_msec=xml_get_attribute_int_default(fade_tag,"life_time",0);
			pose_move->mesh_fade.fade_out_msec=xml_get_attribute_int(fade_tag,"out_time");

				// flash
				
			flash_tag=xml_findfirstchild("Flash",tag);
			pose_move->flash.bone_idx=model_read_xml_bone(model,flash_tag,"bone");
			pose_move->flash.intensity=xml_get_attribute_int(flash_tag,"intensity");
			pose_move->flash.flash_msec=xml_get_attribute_int(flash_tag,"time");
			pose_move->flash.fade_msec=xml_get_attribute_int(flash_tag,"fade_time");
			pose_move->flash.exponent=xml_get_attribute_float_default(flash_tag,"exponent",1.0f);
			xml_get_attribute_color(flash_tag,"color",&pose_move->flash.col);

				// shake
				
			shake_tag=xml_findfirstchild("Shake",tag);
			pose_move->shake.distance=xml_get_attribute_int(shake_tag,"distance");
			pose_move->shake.size=xml_get_attribute_int(shake_tag,"size");
			pose_move->shake.life_msec=xml_get_attribute_int(shake_tag,"time");

				// particles

			pose_move->particle.count=0;

			particles_head=xml_findfirstchild("Particles",tag);
    
			pose_move->particle.count=xml_countchildren(particles_head);
			particle_tag=xml_findfirstchild("Particle",particles_head);

			for (t=0;t!=pose_move->particle.count;t++) {
				xml_get_attribute_text(particle_tag,"particle",pose_move->particle.particles[t].name,name_str_len);
				pose_move->particle.particles[t].bone_idx=model_read_xml_bone(model,particle_tag,"bone");
				pose_move->particle.particles[t].rotate=xml_get_attribute_boolean(particle_tag,"particle_rotate");
				pose_move->particle.particles[t].motion=xml_get_attribute_boolean(particle_tag,"particle_motion");
				pose_move->particle.particles[t].motion_factor=xml_get_attribute_float_default(particle_tag,"particle_motion_factor",1.0f);
				pose_move->particle.particles[t].stick=xml_get_attribute_boolean(particle_tag,"particle_stick");
				xml_get_attribute_3_coord_int(particle_tag,"particle_slop",&pose_move->particle.particles[t].slop.x,&pose_move->particle.particles[t].slop.y,&pose_move->particle.particles[t].slop.z);

				particle_tag=xml_findnextchild(particle_tag);
			}

				// rings

			pose_move->ring.count=0;

			rings_head=xml_findfirstchild("Rings",tag);

			pose_move->ring.count=xml_countchildren(rings_head);
			ring_tag=xml_findfirstchild("Ring",rings_head);

			for (t=0;t!=pose_move->ring.count;t++) {
				xml_get_attribute_text(ring_tag,"ring",pose_move->ring.rings[t].name,name_str_len);
				pose_move->ring.rings[t].bone_idx=model_read_xml_bone(model,ring_tag,"bone");
				pose_move->ring.rings[t].angle=xml_get_attribute_boolean(ring_tag,"ring_angle");
				xml_get_attribute_3_coord_int(ring_tag,"ring_slop",&pose_move->ring.rings[t].slop.x,&pose_move->ring.rings[t].slop.y,&pose_move->ring.rings[t].slop.z);

				ring_tag=xml_findnextchild(ring_tag);
			}

            pose_move++;
			tag=xml_findnextchild(tag);
        }
        
			// fix any bad animation loops
			
 		if (animate->loop_start<0) animate->loop_start=0;
		if (animate->loop_end==-1) animate->loop_end=animate->npose_move-1;
        if (animate->loop_end>(animate->npose_move-1)) animate->loop_end=animate->npose_move-1;
         
		animation_tag=xml_findnextchild(animation_tag);
    }

	xml_close_file(); 
	  
    return(TRUE);
}

/* =======================================================

      Model Write XML Utilities
      
======================================================= */

void model_write_xml_bone(model_type *model,char *attrib_name,int bone_idx)
{
	if (bone_idx==-1) {
		xml_add_attribute_text(attrib_name,"");
		return;
	}

	xml_add_attribute_text(attrib_name,model->bones[bone_idx].name);
}

bool model_write_xml_check_bone_move(model_bone_move_type *bone_move)
{
    if ((bone_move->rot.x!=0.0f) || (bone_move->rot.z!=0.0f) || (bone_move->rot.y!=0.0f)) return(TRUE);
    if ((bone_move->mov.x!=1.0f) || (bone_move->mov.z!=1.0f) || (bone_move->mov.y!=1.0f)) return(TRUE);
	if (bone_move->acceleration!=0.0f) return(TRUE);
	if (bone_move->skip_blended) return(TRUE);
    return(FALSE);
}

/* =======================================================

      Model Write XML
      
======================================================= */

bool model_write_xml(model_type *model,char *err_str)
{
	int						n,k,t,frame_count;
	char					path[1024];
	bool					ok;
	model_hit_box_type		*hit_box;
	model_mesh_type			*mesh;
    model_vertex_type		*vertex;
    model_bone_type			*bone;
    model_poly_type			*poly;
    texture_type			*texture;
	model_pose_type			*pose;
    model_bone_move_type	*bone_move;
	model_animate_type		*animate;
    model_pose_move_type	*pose_move;
    
    xml_new_file();

		// model header
    
    xml_add_tagstart("Model");
	xml_add_attribute_int("version",model_current_version);
    xml_add_tagend(FALSE);
	
        // options
    
    xml_add_tagstart("Options");
	xml_add_attribute_boolean("comulative_rotation",model->comulative_rotation);
	xml_add_tagend(TRUE);
 	
        // center
	
    xml_add_tagstart("Center");
    xml_add_attribute_3_coord_int("offset",model->center.x,model->center.y,model->center.z);
    xml_add_tagend(TRUE);
    
        // boxes
    
    xml_add_tagstart("View_Box");
    xml_add_attribute_3_coord_int("size",model->view_box.size.x,model->view_box.size.y,model->view_box.size.z);
    xml_add_attribute_3_coord_int("offset",model->view_box.offset.x,model->view_box.offset.y,model->view_box.offset.z);
    xml_add_tagend(TRUE);

		// bones
        
    xml_add_tagstart("Bones");
    xml_add_tagend(FALSE);
    
    bone=model->bones;
    
    for (n=0;n!=model->nbone;n++) {
    
        xml_add_tagstart("Bone");
        
		xml_add_attribute_text("name",bone->name);
        xml_add_attribute_3_coord_int("c3",bone->pnt.x,bone->pnt.y,bone->pnt.z);
        xml_add_attribute_3_coord_int("o3",bone->natural_offset.x,bone->natural_offset.y,bone->natural_offset.z);
		xml_add_attribute_3_coord_float("r3",bone->natural_rot.x,bone->natural_rot.y,bone->natural_rot.z);
		model_write_xml_bone(model,"parent",bone->parent_idx);
        
        xml_add_tagend(TRUE);
    
        bone++;
    }

    xml_add_tagclose("Bones");
	
		// light
	
    xml_add_tagstart("Lights");
    xml_add_tagend(FALSE);

	for (n=0;n!=max_model_light;n++) {
		xml_add_tagstart("Light");
		model_write_xml_bone(model,"bone",model->bone_connect.light_bone_idx[n]);
		xml_add_tagend(TRUE);
	}

    xml_add_tagclose("Lights");

		// halos

    xml_add_tagstart("Halos");
    xml_add_tagend(FALSE);

	for (n=0;n!=max_model_halo;n++) {
		xml_add_tagstart("Halo");
		model_write_xml_bone(model,"bone",model->bone_connect.halo_bone_idx[n]);
		xml_add_tagend(TRUE);
	}

    xml_add_tagclose("Halos");
	
        // name
    
    xml_add_tagstart("Name");
 	model_write_xml_bone(model,"bone",model->bone_connect.name_bone_idx);
	xml_add_tagend(TRUE);

        // label
    
    xml_add_tagstart("Label");
	xml_add_tagend(FALSE);

    xml_add_tagstart("Text");
	xml_add_attribute_int("size",model->label.text.size);
	xml_add_attribute_color("color",&model->label.text.col);
 	model_write_xml_bone(model,"bone",model->label.text.bone_idx);
	xml_add_tagend(TRUE);

    xml_add_tagstart("Bitmap");
	xml_add_attribute_int("size",model->label.bitmap.size);
 	model_write_xml_bone(model,"bone",model->label.bitmap.bone_idx);
	xml_add_tagend(TRUE);

    xml_add_tagstart("Bar");
	xml_add_attribute_int("wid",model->label.bar.wid);
	xml_add_attribute_int("high",model->label.bar.high);
	xml_add_attribute_boolean("border_on",model->label.bar.border_on);
	xml_add_attribute_boolean("background_on",model->label.bar.background_on);
	xml_add_attribute_color("border_color",&model->label.bar.border_col);
	xml_add_attribute_color("background_color",&model->label.bar.background_col);
	xml_add_attribute_color("bar_color",&model->label.bar.bar_col);
 	model_write_xml_bone(model,"bone",model->label.bar.bone_idx);
	xml_add_tagend(TRUE);

	xml_add_tagclose("Label");

		// hit boxes
		
    xml_add_tagstart("Hit_Boxes");
    xml_add_tagend(FALSE);
    
    hit_box=model->hit_boxes;
    
    for (n=0;n!=model->nhit_box;n++) {
		xml_add_tagstart("Hit_Box");
		xml_add_attribute_text("name",hit_box->name);
		model_write_xml_bone(model,"bone",hit_box->bone_idx);
		xml_add_attribute_3_coord_int("size",hit_box->box.size.x,hit_box->box.size.y,hit_box->box.size.z);
		xml_add_attribute_3_coord_int("offset",hit_box->box.offset.x,hit_box->box.offset.y,hit_box->box.offset.z);
		xml_add_tagend(TRUE);
		
		hit_box++;
	}
	
    xml_add_tagclose("Hit_Boxes");

		// rigid body

    xml_add_tagstart("Rigid_Body");
	xml_add_attribute_boolean("on",model->rigid_body.on);
	xml_add_attribute_float("y_factor",model->rigid_body.y.reset_factor);
	xml_add_attribute_float("y_smooth",model->rigid_body.y.smooth_factor);
	xml_add_attribute_float("x_max_ang",model->rigid_body.x.max_ang);
	xml_add_attribute_float("x_factor",model->rigid_body.x.reset_factor);
	xml_add_attribute_float("x_smooth",model->rigid_body.x.smooth_factor);
	xml_add_attribute_float("z_max_ang",model->rigid_body.z.max_ang);
	xml_add_attribute_float("z_factor",model->rigid_body.z.reset_factor);
	xml_add_attribute_float("z_smooth",model->rigid_body.z.smooth_factor);
    xml_add_tagend(TRUE);

  		// ui
		
    xml_add_tagstart("UI");
	xml_add_attribute_int("light_intensity",model->ui.light_intensity);
	xml_add_attribute_float("light_exponent",model->ui.light_exponent);
	xml_add_attribute_3_coord_int("light_offset",model->ui.light_offset.x,model->ui.light_offset.y,model->ui.light_offset.z);
	xml_add_attribute_color("light_color",&model->ui.light_col);
	xml_add_tagend(TRUE);
 		
        // meshes
        
    xml_add_tagstart("Meshes");
    xml_add_tagend(FALSE);
	
	for (n=0;n!=model->nmesh;n++) {
	
		mesh=&model->meshes[n];
	
		xml_add_tagstart("Mesh");
		xml_add_attribute_text("name",mesh->name);
		xml_add_attribute_boolean("no_lighting",mesh->no_lighting);
		xml_add_attribute_boolean("diffuse",mesh->diffuse);
		xml_add_attribute_boolean("additive",mesh->blend_add);
		xml_add_attribute_boolean("never_cull",mesh->never_cull);
		xml_add_attribute_boolean("locked",mesh->locked);
		xml_add_attribute_boolean("rt_non_light_blocking",mesh->rt_non_light_blocking);
		xml_add_attribute_3_coord_int("import_move",mesh->import_move.x,mesh->import_move.y,mesh->import_move.z);
		xml_add_tagend(FALSE);
	   
			// vertexes
			
		xml_add_tagstart("Vertexes");
		xml_add_tagend(FALSE);
		
		vertex=mesh->vertexes;
		
		for (k=0;k!=mesh->nvertex;k++) {
		
			xml_add_tagstart("v");
			
			xml_add_attribute_3_coord_int("c3",vertex->pnt.x,vertex->pnt.y,vertex->pnt.z);
			xml_add_attribute_3_coord_float("t3",vertex->tangent_space.tangent.x,vertex->tangent_space.tangent.y,vertex->tangent_space.tangent.z);
			xml_add_attribute_3_coord_float("n3",vertex->tangent_space.normal.x,vertex->tangent_space.normal.y,vertex->tangent_space.normal.z);

			model_write_xml_bone(model,"major",vertex->major_bone_idx);
			model_write_xml_bone(model,"minor",vertex->minor_bone_idx);
			xml_add_attribute_float("factor",vertex->bone_factor);
			
			xml_add_tagend(TRUE);
		
			vertex++;
		}
						
		xml_add_tagclose("Vertexes");
		
			// triangles
				
		xml_add_tagstart("Polys");
		xml_add_tagend(FALSE);
		
		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
		
			xml_add_tagstart("p");
			xml_add_attribute_int_array("v",poly->v,poly->ptsz,FALSE);
			xml_add_attribute_int("txt",poly->txt_idx);
			xml_add_attribute_float_array("x",poly->gx,poly->ptsz);
			xml_add_attribute_float_array("y",poly->gy,poly->ptsz);
			xml_add_tagend(TRUE);

			poly++;
		}
			
		xml_add_tagclose("Polys");
		
		xml_add_tagclose("Mesh");
	}
	
	xml_add_tagclose("Meshes");
   
         // fills
         
    xml_add_tagstart("Fills");
    xml_add_tagend(FALSE);
    
    texture=model->textures;
    
    for (n=0;n!=max_model_texture;n++) {
    
        if (texture->frames[0].name[0]==0x0) {
            texture++;
            continue;
        }

		frame_count=model_count_texture_frames(model,n);
        
        xml_add_tagstart("Fill");
		bitmap_texture_write_xml(texture,frame_count,FALSE);
        xml_add_tagclose("Fill");
        
		texture++;
    }
    
    xml_add_tagclose("Fills");

        // poses
   
    xml_add_tagstart("Poses");
    xml_add_tagend(FALSE);
    
    pose=model->poses;
    
    for (n=0;n!=model->npose;n++) {
    
        xml_add_tagstart("Pose");
        xml_add_attribute_text("name",pose->name);
        xml_add_tagend(FALSE);
        
        xml_add_tagstart("Bones");
        xml_add_tagend(FALSE);
        
        bone_move=pose->bone_moves;
        
        for (k=0;k!=model->nbone;k++) {
        
            if (model_write_xml_check_bone_move(bone_move)) {
                xml_add_tagstart("Bone");
                xml_add_attribute_text("name",model->bones[k].name);
                xml_add_attribute_3_coord_float("rot",bone_move->rot.x,bone_move->rot.y,bone_move->rot.z);
                xml_add_attribute_3_coord_float("move",bone_move->mov.x,bone_move->mov.y,bone_move->mov.z);
				
				xml_add_attribute_float("acceleration",bone_move->acceleration);
				xml_add_attribute_boolean("skip_blended",bone_move->skip_blended);
				
				model_write_xml_bone(model,"constraint_bone",bone_move->constraint.bone_idx);
				xml_add_attribute_3_coord_int("constraint_offset",bone_move->constraint.offset.x,bone_move->constraint.offset.y,bone_move->constraint.offset.z);

				xml_add_tagend(TRUE);
            }
            
            bone_move++;
        }
            
        xml_add_tagclose("Bones");
        xml_add_tagclose("Pose");
    
        pose++;
    }
    
    xml_add_tagclose("Poses");

        // animations
   
    xml_add_tagstart("Animations");
    xml_add_tagend(FALSE);
    
    animate=model->animates;
    
    for (n=0;n!=model->nanimate;n++) {
    
        xml_add_tagstart("Animation");
        xml_add_attribute_text("name",animate->name);
        xml_add_tagend(FALSE);
        
        xml_add_tagstart("Loop");
        xml_add_attribute_boolean("repeat",animate->loop);
        xml_add_attribute_boolean("no_smooth",animate->no_smooth);
        xml_add_attribute_boolean("auto_stop",animate->auto_stop);
		xml_add_attribute_float("enhance_factor",animate->enhance_factor);
        xml_add_attribute_int("start",animate->loop_start);
        xml_add_attribute_int("end",animate->loop_end);
        xml_add_tagend(TRUE);
        
			// pose moves

        xml_add_tagstart("Poses");
        xml_add_tagend(FALSE);
        
        pose_move=animate->pose_moves;
        
        for (k=0;k!=animate->npose_move;k++) {
        
            xml_add_tagstart("Pose");
			
            xml_add_attribute_text("name",model->poses[pose_move->pose_idx].name);
            xml_add_attribute_int("time",pose_move->msec);
            xml_add_attribute_3_coord_float("sway",pose_move->sway.x,pose_move->sway.y,pose_move->sway.z);
            xml_add_attribute_3_coord_float("move",pose_move->mov.x,pose_move->mov.y,pose_move->mov.z);
			xml_add_attribute_float("acceleration",pose_move->acceleration);

			xml_add_tagend(FALSE);

				// sound

 			xml_add_tagstart("Sound");
			xml_add_attribute_text("name",pose_move->sound.name);
 			model_write_xml_bone(model,"bone",pose_move->sound.bone_idx);
			xml_add_attribute_float("pitch",pose_move->sound.pitch);
			xml_add_attribute_boolean("global",pose_move->sound.no_position);
			xml_add_tagend(TRUE);

				// mesh fade

 			xml_add_tagstart("Fade");
			if (pose_move->mesh_fade.mesh_idx!=-1) {
				xml_add_attribute_text("mesh",model->meshes[pose_move->mesh_fade.mesh_idx].name);
			}
			else {
				xml_add_attribute_text("mesh","");
			}
			xml_add_attribute_int("in_time",pose_move->mesh_fade.fade_in_msec);
			xml_add_attribute_int("life_time",pose_move->mesh_fade.fade_life_msec);
			xml_add_attribute_int("out_time",pose_move->mesh_fade.fade_out_msec);
			xml_add_tagend(TRUE);

				// flash

 			xml_add_tagstart("Flash");
 			model_write_xml_bone(model,"bone",pose_move->flash.bone_idx);
			xml_add_attribute_int("intensity",pose_move->flash.intensity);
			xml_add_attribute_int("time",pose_move->flash.flash_msec);
			xml_add_attribute_int("fade_time",pose_move->flash.fade_msec);
			xml_add_attribute_float("exponent",pose_move->flash.exponent);
			xml_add_attribute_color("color",&pose_move->flash.col);
			xml_add_tagend(TRUE);

				// shake

 			xml_add_tagstart("Shake");
			xml_add_attribute_int("distance",pose_move->shake.distance);
			xml_add_attribute_int("size",pose_move->shake.size);
			xml_add_attribute_int("time",pose_move->shake.life_msec);
			xml_add_tagend(TRUE);

				// particles

			xml_add_tagstart("Particles");
			xml_add_tagend(FALSE);

			for (t=0;t!=pose_move->particle.count;t++) {
			
				xml_add_tagstart("Particle");
				xml_add_attribute_text("particle",pose_move->particle.particles[t].name);
				model_write_xml_bone(model,"bone",pose_move->particle.particles[t].bone_idx);
				xml_add_attribute_boolean("particle_rotate",pose_move->particle.particles[t].rotate);
				xml_add_attribute_boolean("particle_motion",pose_move->particle.particles[t].motion);
				xml_add_attribute_float("particle_motion_factor",pose_move->particle.particles[t].motion_factor);
				xml_add_attribute_boolean("particle_stick",pose_move->particle.particles[t].stick);
				xml_add_attribute_3_coord_int("particle_slop",pose_move->particle.particles[t].slop.x,pose_move->particle.particles[t].slop.y,pose_move->particle.particles[t].slop.z);
				
				xml_add_tagend(TRUE);
			}

			xml_add_tagclose("Particles");

				// rings

			xml_add_tagstart("Rings");
			xml_add_tagend(FALSE);

			for (t=0;t!=pose_move->ring.count;t++) {
			
				xml_add_tagstart("Ring");
				xml_add_attribute_text("ring",pose_move->ring.rings[t].name);
				model_write_xml_bone(model,"bone",pose_move->ring.rings[t].bone_idx);
				xml_add_attribute_boolean("ring_angle",pose_move->ring.rings[t].angle);
				xml_add_attribute_3_coord_int("ring_slop",pose_move->ring.rings[t].slop.x,pose_move->ring.rings[t].slop.y,pose_move->ring.rings[t].slop.z);
				
				xml_add_tagend(TRUE);
			}

			xml_add_tagclose("Rings");
			
			xml_add_tagclose("Pose");
            
            pose_move++;
        }
        
        xml_add_tagclose("Poses");
        xml_add_tagclose("Animation");
        
        animate++;
    }

    xml_add_tagclose("Animations");

        // save model
        
    xml_add_tagclose("Model");
    
	sprintf(path,"%s/model.xml",model->load_base_path);
	ok=xml_save_file(path,err_str);
	
    xml_close_file();

	return(ok);
}
