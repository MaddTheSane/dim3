/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette Bone

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

#define kBonePropertyName						0
#define kBonePropertyPosition					1
#define kBonePropertyParent						2

#define kBonePropertyNaturalRot					10
#define kBonePropertyNaturalOffset				11

#define kPoseBoneMovePropertyRot				20
#define kPoseBoneMovePropertyMove				21
#define kPoseBoneMovePropertyAcceleration		22
#define kPoseBoneMovePropertySkipBlended		23

#define kPoseBoneMovePropertyConstraintBone		30
#define kPoseBoneMovePropertyConstraintOffset	31

extern model_type				model;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		model_palette;

char							property_circular_bone_list[max_model_bone][name_str_len];

/* =======================================================

      Property Palette Fill Bone Utility
      
======================================================= */

void model_palette_fill_bone_create_circular_list(int bone_idx,int circular_check_bone_idx)
{
	int				n,idx;

	for (n=0;n!=model.nbone;n++) {

		idx=n*name_str_len;

		if (circular_check_bone_idx!=-1) {
			if ((n==circular_check_bone_idx) || (model_check_bone_circular(&model,&model.bones[circular_check_bone_idx]))) {
				property_circular_bone_list[n][0]='~';
				strcpy((char*)&property_circular_bone_list[n][1],model.bones[n].name);
				continue;
			}
		}

		strcpy(property_circular_bone_list[n],model.bones[n].name);
	}
}

/* =======================================================

      Property Palette Fill Bone
      
======================================================= */

void model_palette_fill_bone(int bone_idx,int pose_idx)
{
	model_bone_type			*bone;
	model_bone_move_type	*bone_move;

	bone=&model.bones[bone_idx];

	list_palette_set_title(&model_palette,"Bone",bone->name,NULL,NULL,NULL,NULL);

		// bone

	list_palette_add_header(&model_palette,0,"Bone Options");
	list_palette_add_string(&model_palette,kBonePropertyName,"Name",bone->name,name_str_len,FALSE);
	list_palette_add_point(&model_palette,kBonePropertyPosition,"Position",&bone->pnt,FALSE);

	model_palette_fill_bone_create_circular_list(bone->parent_idx,bone_idx);
	list_palette_add_picker_list_int(&model_palette,kBonePropertyParent,"Parent Bone",(char*)property_circular_bone_list,model.nbone,name_str_len,0,TRUE,TRUE,&bone->parent_idx,FALSE);

		// handles
	
	list_palette_add_header(&model_palette,0,"Natural Settings");
	list_palette_add_angle(&model_palette,kBonePropertyNaturalRot,"Natural Rotation",&bone->natural_rot,FALSE);
	list_palette_add_point(&model_palette,kBonePropertyNaturalOffset,"Natural Offset",&bone->natural_offset,FALSE);

		// current pose

	if (pose_idx!=-1) {
		bone_move=&model.poses[pose_idx].bone_moves[bone_idx];

		list_palette_add_header(&model_palette,0,"Current Pose Move Position");
		list_palette_add_vector(&model_palette,kPoseBoneMovePropertyRot,"Rot",&bone_move->rot,FALSE);
		list_palette_add_vector(&model_palette,kPoseBoneMovePropertyMove,"Move",&bone_move->mov,FALSE);
	
		list_palette_add_header(&model_palette,0,"Current Pose Move Options");
		list_palette_add_float(&model_palette,kPoseBoneMovePropertyAcceleration,"Acceleration",&bone_move->acceleration,FALSE);
		list_palette_add_checkbox(&model_palette,kPoseBoneMovePropertySkipBlended,"Skip Blending",&bone_move->skip_blended,FALSE);

		list_palette_add_header(&model_palette,0,"Current Pose Move Constraint");
		list_palette_add_picker_list_int(&model_palette,kPoseBoneMovePropertyConstraintBone,"Constraint Bone",(char*)model.bones,model.nbone,sizeof(model_bone_type),(int)offsetof(model_bone_type,name),TRUE,TRUE,&bone_move->constraint.bone_idx,FALSE);
		list_palette_add_point(&model_palette,kPoseBoneMovePropertyConstraintOffset,"Constaint Offset",&bone_move->constraint.offset,FALSE);
	}
}

/* =======================================================

      Property Palette Click Bone
      
======================================================= */

void model_palette_click_bone(int bone_idx,int pose_idx,bool double_click)
{
}

