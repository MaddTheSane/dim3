/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette Animate Pose Move Ring

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

#define kAnimationPoseMovePropertyRingName					0
#define kAnimationPoseMovePropertyRingBone					1
#define kAnimationPoseMovePropertyRingAngle					2
#define kAnimationPoseMovePropertyRingSlop					3

extern model_type				model;
extern file_path_setup_type		file_path_setup;
extern iface_type				iface;

extern list_palette_type		model_palette;

/* =======================================================

      Property Palette Fill Animate Pose Move Ring
      
======================================================= */

void model_palette_fill_animate_pose_move_ring(int animate_idx,int pose_move_idx,int ring_idx)
{
	model_animate_type		*animate;
	model_pose_move_type	*pose_move;
	model_ring_type			*ring;

	if (ring_idx==-1) return;

	animate=&model.animates[animate_idx];
	pose_move=&animate->pose_moves[pose_move_idx];
	ring=&pose_move->ring.rings[ring_idx];

	list_palette_set_title(&model_palette,"Animation",animate->name,"Pose Move",model.poses[pose_move->pose_idx].name,"Ring",ring->name);
	list_palette_add_picker_list_string(&model_palette,kAnimationPoseMovePropertyRingName,"Name",(char*)iface.ring_list.rings,iface.ring_list.nring,sizeof(iface_ring_type),(int)offsetof(iface_ring_type,name),TRUE,TRUE,ring->name,FALSE);
	list_palette_add_picker_list_int(&model_palette,kAnimationPoseMovePropertyRingBone,"Bone",(char*)model.bones,model.nbone,sizeof(model_bone_type),(int)offsetof(model_bone_type,name),TRUE,TRUE,&ring->bone_idx,FALSE);
	list_palette_add_checkbox(&model_palette,kAnimationPoseMovePropertyRingAngle,"Follow Model Angle",&ring->angle,FALSE);
	list_palette_add_point(&model_palette,kAnimationPoseMovePropertyRingSlop,"Position Slop",&ring->slop,FALSE);
}

/* =======================================================

      Property Palette Click Animate Pose Move Ring
      
======================================================= */

void model_palette_click_animate_pose_move_ring(int animate_idx,int pose_move_idx,int ring_idx,bool double_click)
{
}

