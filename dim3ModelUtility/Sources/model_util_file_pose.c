/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Pose File Loading

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

// supergumba -- eventually remove this code, this is v2 of pose reading

/* =======================================================

      Read Pose XML
      
======================================================= */

bool model_read_v2_pose_xml(model_type *model)
{
	int						n,k,bone_idx,npose,pose_idx,cnt,
							tag,model_head,bone_tag,poses_tag,pose_tag;
	char					sub_path[1024],path[1024];
    model_bone_move_type	*bone_move;
	model_pose_type			*pose;

        // load the pose xml
        
	sprintf(sub_path,"Models/%s",model->name);
	file_paths_data(&file_path_setup,path,sub_path,"pose","xml");

	if (!xml_open_file(path)) return(FALSE);

    model_head=xml_findrootchild("Model");
    if (model_head==-1) return(FALSE);
    
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
			bone_move->acceleration=0;
			bone_move->skip_blended=FALSE;
			bone_move->constraint.bone_idx=-1;
			bone_move->constraint.offset.x=bone_move->constraint.offset.y=bone_move->constraint.offset.z=0;
			bone_move++;
        }
            
        bone_tag=xml_findfirstchild("Bones",pose_tag);
        
        cnt=xml_countchildren(bone_tag);
		tag=xml_findfirstchild("Bone",bone_tag);
        
        for (k=0;k!=cnt;k++) {
			
			bone_idx=model_xml_get_attribute_bone(model,tag,"tag");
		
            if (bone_idx!=-1) {
                bone_move=&pose->bone_moves[bone_idx];
                xml_get_attribute_3_coord_float(tag,"rot",&bone_move->rot.x,&bone_move->rot.y,&bone_move->rot.z);
				xml_get_attribute_3_coord_float(tag,"move",&bone_move->mov.x,&bone_move->mov.y,&bone_move->mov.z);
				
				bone_move->acceleration=xml_get_attribute_float(tag,"acceleration");
				bone_move->skip_blended=xml_get_attribute_boolean(tag,"skip_blended");
				
				bone_move->constraint.bone_idx=model_xml_get_attribute_bone(model,tag,"constraint_bone");

				if (bone_move->constraint.bone_idx!=-1) {
					xml_get_attribute_3_coord_int(tag,"constraint_offset",&bone_move->constraint.offset.x,&bone_move->constraint.offset.y,&bone_move->constraint.offset.z);
				}
			}
		  
			tag=xml_findnextchild(tag);
        }
    
		pose_tag=xml_findnextchild(pose_tag);
    }

    xml_close_file();
    
    return(TRUE);
}

