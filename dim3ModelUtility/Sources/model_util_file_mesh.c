/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Mesh File Loading

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

char									deform_mode_str[][32]={"single_rotate","comulative_rotate",""};
char									bone_v2_tags[max_model_bone][8];

// supergumba -- eventually remove this code, this is v2 of mesh reading

/* =======================================================

      Decode Mesh XML
      
======================================================= */

int model_find_v2_bone_tag(model_type *model,char *tag_name)
{
	int				n;

	for (n=0;n!=model->nbone;n++) {
		if (strcasecmp(bone_v2_tags[n],tag_name)==0) return(n);
	}

	return(-1);
}

int model_xml_get_attribute_bone(model_type *model,int tag,char *tag_name)
{
	char			bone_name[name_str_len];

	xml_get_attribute_text(tag,tag_name,bone_name,name_str_len);
	return(model_find_v2_bone_tag(model,bone_name));
}

void model_decode_v2_mesh_xml(model_type *model,int model_head)
{
	int						i,n,k,j,deform_mode,bone_idx,nbone,hit_box_idx,nhit_box,trig_count,
							mesh_idx,nmesh,nfill,trig_idx,
							tag,hit_box_tag,rigid_body_tag,meshes_tag,mesh_tag,
							vertex_tag,bone_tag,vtag,trig_tag,
							materials_tag,material_tag,fills_tag,fill_tag;
	bool					had_tangent;
	char					tag_name[32],
							bone_parent_tags[max_model_bone][8];
	model_hit_box_type		*hit_box;
	model_mesh_type			*mesh;
    model_vertex_type		*vertex;
    model_bone_type			*bone;
    model_poly_type			*trig;
    texture_type			*texture;
	tangent_space_type		*trig_tangent_spaces,*trig_ts;

        // options
    
    tag=xml_findfirstchild("Options",model_head);
    if (tag!=-1) {
		deform_mode=xml_get_attribute_list(tag,"deform",(char*)deform_mode_str);
		model->comulative_rotation=(deform_mode!=0);
	}
	
        // center
    
    tag=xml_findfirstchild("Center",model_head);
    if (tag!=-1) {
		xml_get_attribute_3_coord_int(tag,"offset",&model->center.x,&model->center.y,&model->center.z);
    }
    
        // boxes
    
    tag=xml_findfirstchild("View_Box",model_head);
    if (tag!=-1) {
        xml_get_attribute_3_coord_int(tag,"size",&model->view_box.size.x,&model->view_box.size.y,&model->view_box.size.z);
        xml_get_attribute_3_coord_int(tag,"offset",&model->view_box.offset.x,&model->view_box.offset.y,&model->view_box.offset.z);
    }
 
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
 		xml_get_attribute_text(tag,"tag",bone_v2_tags[bone_idx],8);
		
		if (bone->name[0]==0x0) strcpy(bone->name,bone_v2_tags[bone_idx]);		// if no name, use the tag name
       
        xml_get_attribute_3_coord_int(tag,"c3",&bone->pnt.x,&bone->pnt.y,&bone->pnt.z);
        xml_get_attribute_3_coord_int(tag,"o3",&bone->natural_offset.x,&bone->natural_offset.y,&bone->natural_offset.z);
		xml_get_attribute_3_coord_float(tag,"r3",&bone->natural_rot.x,&bone->natural_rot.y,&bone->natural_rot.z);

  		xml_get_attribute_text(tag,"parent",bone_parent_tags[bone_idx],8);
   
		tag=xml_findnextchild(tag);
    }
		
		// reset the bones from tags to indexes
		
	bone=model->bones;
	
	for (n=0;n!=model->nbone;n++) {
		bone->parent_idx=model_find_v2_bone_tag(model,bone_parent_tags[n]);
		bone++;
	}
	
        // light
        
    tag=xml_findfirstchild("Light",model_head);
    if (tag!=-1) {

		for (n=0;n!=max_model_light;n++) {
			if (n==0) {
				strcpy(tag_name,"light_bone");
			}
			else {
				sprintf(tag_name,"light_bone_%d",n);
			}

			model->bone_connect.light_bone_idx[n]=model_xml_get_attribute_bone(model,tag,tag_name);
		}

		for (n=0;n!=max_model_halo;n++) {
			if (n==0) {
				strcpy(tag_name,"halo_bone");
			}
			else {
				sprintf(tag_name,"halo_bone_%d",n);
			}
			
			model->bone_connect.halo_bone_idx[n]=model_xml_get_attribute_bone(model,tag,tag_name);
		}

		model->bone_connect.name_bone_idx=model_xml_get_attribute_bone(model,tag,"name_bone");
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
			hit_box->bone_idx=model_xml_get_attribute_bone(model,tag,"bone");
			xml_get_attribute_3_coord_int(tag,"size",&hit_box->box.size.x,&hit_box->box.size.y,&hit_box->box.size.z);
			xml_get_attribute_3_coord_int(tag,"offset",&hit_box->box.offset.x,&hit_box->box.offset.y,&hit_box->box.offset.z);
			
			tag=xml_findnextchild(tag);
		}
    }
	else {
		model->nhit_box=0;
	}

		// rigid body

    rigid_body_tag=xml_findfirstchild("Rigid_Body",model_head);
	if (rigid_body_tag!=-1) {

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

	}
	else {
		model->rigid_body.on=FALSE;

		model->rigid_body.y.max_ang=0.0f;			// unused, but in struct
		model->rigid_body.y.reset_factor=0.8f;
		model->rigid_body.y.smooth_factor=0.2f;
		model->rigid_body.x.max_ang=45.0f;
		model->rigid_body.x.reset_factor=0.8f;
		model->rigid_body.x.smooth_factor=0.2f;
		model->rigid_body.z.max_ang=45.0f;
		model->rigid_body.z.reset_factor=0.8f;
		model->rigid_body.z.smooth_factor=0.2f;
	}
	
		// meshes
		
	had_tangent=FALSE;
		
	meshes_tag=xml_findfirstchild("Meshes",model_head);

    nmesh=xml_countchildren(meshes_tag);
	mesh_tag=xml_findfirstchild("Mesh",meshes_tag);
	
	model->nmesh=0;				// new models have a single mesh
	
	for (j=0;j!=nmesh;j++) {
		mesh_idx=model_mesh_add(model);
		if (mesh_idx==-1) break;
	
			// mesh settings
			
		mesh=&model->meshes[mesh_idx];
		xml_get_attribute_text(mesh_tag,"name",mesh->name,name_str_len);
		mesh->no_lighting=xml_get_attribute_boolean(mesh_tag,"no_lighting");
		mesh->diffuse=xml_get_attribute_boolean(mesh_tag,"diffuse");
		mesh->blend_add=xml_get_attribute_boolean(mesh_tag,"additive");
		mesh->locked=xml_get_attribute_boolean(mesh_tag,"locked");
		xml_get_attribute_3_coord_int(mesh_tag,"import_move",&mesh->import_move.x,&mesh->import_move.y,&mesh->import_move.z);
		memmove(&mesh->org_import_move,&mesh->import_move,sizeof(d3pnt));

		mesh->never_cull=TRUE;
		
			// don't allow blank mesh names
			
		if (mesh->name[0]==0x0) {
			if (mesh_idx==0) {
				strcpy(mesh->name,"Default");
			}
			else {
				sprintf(mesh->name,"Mesh %d",mesh_idx);
			}
		}
		
			// vertexes
		
		vertex_tag=xml_findfirstchild("Vertexes",mesh_tag);

		mesh->nvertex=xml_countchildren(vertex_tag);
		tag=xml_findfirstchild("v",vertex_tag);

		model_mesh_set_vertex_count(model,mesh_idx,mesh->nvertex);
		
		vertex=mesh->vertexes;
		
		for (i=0;i!=mesh->nvertex;i++) {
			xml_get_attribute_3_coord_int(tag,"c3",&vertex->pnt.x,&vertex->pnt.y,&vertex->pnt.z);
			
			vertex->major_bone_idx=model_xml_get_attribute_bone(model,tag,"major");
			vertex->minor_bone_idx=model_xml_get_attribute_bone(model,tag,"minor");
			vertex->bone_factor=xml_get_attribute_float_default(tag,"factor",1.0f);
		
			vertex++;
			tag=xml_findnextchild(tag);
		}
		
			// count the trigs
			
		materials_tag=xml_findfirstchild("Materials",mesh_tag);

		nfill=xml_countchildren(materials_tag);
		material_tag=xml_findfirstchild("Material",materials_tag);

		mesh->npoly=0;
		
		for (n=0;n!=nfill;n++) {
			trig_tag=xml_findfirstchild("Triangles",material_tag);
			mesh->npoly+=(xml_countchildren(trig_tag)/3);
			material_tag=xml_findnextchild(material_tag);
		}

		model_mesh_set_poly_count(model,mesh_idx,mesh->npoly);
		
			// memory for the old trig based tangent spaces
			
		trig_tangent_spaces=(tangent_space_type*)malloc((mesh->npoly*3)*sizeof(tangent_space_type));

			// run the materials
		
		trig_idx=0;
		trig=mesh->polys;
		
		trig_ts=trig_tangent_spaces;

		material_tag=xml_findfirstchild("Material",materials_tag);
		
		for (n=0;n!=nfill;n++) {
		
			trig_tag=xml_findfirstchild("Triangles",material_tag);
			
			trig_count=xml_countchildren(trig_tag)/3;
			
			vtag=xml_findfirstchild("v",trig_tag);
		
			for (i=0;i!=trig_count;i++) {

				trig->txt_idx=n;
				trig->ptsz=3;
		
				for (k=0;k!=3;k++) {
					trig->v[k]=xml_get_attribute_int(vtag,"id");
					xml_get_attribute_2_coord_float(vtag,"uv",&trig->gx[k],&trig->gy[k]);

					had_tangent=xml_get_attribute_3_coord_float(vtag,"t3",&trig_ts->tangent.x,&trig_ts->tangent.y,&trig_ts->tangent.z);
					xml_get_attribute_3_coord_float(vtag,"n3",&trig_ts->normal.x,&trig_ts->normal.y,&trig_ts->normal.z);
					
					trig_ts++;

					vtag=xml_findnextchild(vtag);
				}
				
				trig++;
				trig_idx++;
			}
			
			material_tag=xml_findnextchild(material_tag);
		}
		
			// create the vertex level tangent
			// space
			
		vertex=mesh->vertexes;
		
		for (i=0;i!=mesh->nvertex;i++) {

				// run through the trigs
				// to build the vertex tangent
				// space

			vertex->tangent_space.normal.x=vertex->tangent_space.normal.y=vertex->tangent_space.normal.z=0.0f;
			vertex->tangent_space.tangent.x=vertex->tangent_space.tangent.y=vertex->tangent_space.tangent.z=0.0f;

			trig_count=0;
			trig=mesh->polys;
			
			trig_ts=trig_tangent_spaces;

			for (n=0;n!=mesh->npoly;n++) {
				for (k=0;k!=3;k++) {
					if (trig->v[k]==i) {
						vertex->tangent_space.normal.x+=trig_ts->normal.x;
						vertex->tangent_space.normal.y+=trig_ts->normal.y;
						vertex->tangent_space.normal.z+=trig_ts->normal.z;
						vertex->tangent_space.tangent.x+=trig_ts->tangent.x;
						vertex->tangent_space.tangent.y+=trig_ts->tangent.y;
						vertex->tangent_space.tangent.z+=trig_ts->tangent.z;
						trig_count++;
					}
					trig_ts++;
				}
				
				trig++;
			}

			if (trig_count!=0) {
				vertex->tangent_space.normal.x/=(float)trig_count;
				vertex->tangent_space.normal.y/=(float)trig_count;
				vertex->tangent_space.normal.z/=(float)trig_count;
				vector_normalize(&vertex->tangent_space.normal);
				vertex->tangent_space.tangent.x/=(float)trig_count;
				vertex->tangent_space.tangent.y/=(float)trig_count;
				vertex->tangent_space.tangent.z/=(float)trig_count;
				vector_normalize(&vertex->tangent_space.tangent);
			}

			vertex++;
		}
		
		mesh_tag=xml_findnextchild(mesh_tag);
	}
	
	model->nmesh=nmesh;
	
	free(trig_tangent_spaces);
  
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
	
		// if no tangent space, then calculate it
		
	if (!had_tangent) model_recalc_normals(model,FALSE);
}

/* =======================================================

      Read Mesh XML
      
======================================================= */

bool model_read_v2_mesh_xml(model_type *model)
{
	int						model_head;
	char					sub_path[1024],path[1024];

        // load the mesh xml
		
	sprintf(sub_path,"Models/%s",model->name);
	file_paths_data(&file_path_setup,path,sub_path,"mesh","xml");
	
	if (!xml_open_file(path)) return(FALSE);

    model_head=xml_findrootchild("Model");
    if (model_head==-1) {
		xml_close_file();
		return(FALSE);
	}
		
	model_decode_v2_mesh_xml(model,model_head);
	
	xml_close_file(); 
	  
    return(TRUE);
}

