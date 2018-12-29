/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette Pose Bone Move

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

#define kPoseBoneMovePropertyRot					0
#define kPoseBoneMovePropertyMove					1
#define kPoseBoneMovePropertyAcceleration			2
#define kPoseBoneMovePropertySkipBlended			3

#define kPoseBoneMovePropertyConstraintBone			4
#define kPoseBoneMovePropertyConstraintOffset		5

extern model_type				model;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		model_palette;

/* =======================================================

      Property Palette Fill Pose Bone Move
      
======================================================= */

void model_palette_fill_pose_bone_move(int pose_idx,int bone_move_idx)
{
	model_bone_move_type	*bone_move;

	if (bone_move_idx==-1) return;

	bone_move=&model.poses[pose_idx].bone_moves[bone_move_idx];

	list_palette_set_title(&model_palette,"Pose",model.poses[pose_idx].name,"Bone Move",model.bones[bone_move_idx].name,NULL,NULL);

	list_palette_add_header(&model_palette,0,"Pose Bone Move Position");
	list_palette_add_vector(&model_palette,kPoseBoneMovePropertyRot,"Rot",&bone_move->rot,FALSE);
	list_palette_add_vector(&model_palette,kPoseBoneMovePropertyMove,"Move",&bone_move->mov,FALSE);
	
	list_palette_add_header(&model_palette,0,"Pose Bone Move Options");
	list_palette_add_float(&model_palette,kPoseBoneMovePropertyAcceleration,"Acceleration",&bone_move->acceleration,FALSE);
	list_palette_add_checkbox(&model_palette,kPoseBoneMovePropertySkipBlended,"Skip Blending",&bone_move->skip_blended,FALSE);

	list_palette_add_header(&model_palette,0,"Pose Bone Move Constraint");
	list_palette_add_picker_list_int(&model_palette,kPoseBoneMovePropertyConstraintBone,"Constraint Bone",(char*)model.bones,model.nbone,sizeof(model_bone_type),(int)offsetof(model_bone_type,name),TRUE,TRUE,&bone_move->constraint.bone_idx,FALSE);
	list_palette_add_point(&model_palette,kPoseBoneMovePropertyConstraintOffset,"Constaint Offset",&bone_move->constraint.offset,FALSE);
}

/* =======================================================

      Property Palette Click Pose Bone Move
      
======================================================= */

void model_palette_click_pose_bone_move(int pose_idx,int bone_move_idx,bool double_click)
{
}

