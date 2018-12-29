/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette Pose

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

#define kPosePropertyName							0

#define kPosePropertyBoneMove						100

extern model_type				model;
extern app_state_type			state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		model_palette;

/* =======================================================

      Property Palette Fill Pose
      
======================================================= */

void model_palette_fill_pose(int pose_idx)
{
	int						n;
	model_pose_type			*pose;

	pose=&model.poses[pose_idx];

	list_palette_set_title(&model_palette,"Pose",pose->name,NULL,NULL,NULL,NULL);

	list_palette_add_header(&model_palette,0,"Pose Options");
	list_palette_add_string(&model_palette,kPosePropertyName,"Name",pose->name,name_str_len,FALSE);

	list_palette_add_header_count(&model_palette,0,"Pose Bone Moves",model.nbone);
	list_palette_sort_mark_start(&model_palette,&model_palette.item_pane);

	for (n=0;n!=model.nbone;n++) {
		list_palette_add_string_selectable(&model_palette,(kPosePropertyBoneMove+n),model.bones[n].name,NULL,-1,((state.model.cur_pose_idx==pose_idx)&&(state.model.cur_pose_bone_move_idx==n)),FALSE);
	}

	list_palette_sort(&model_palette,&model_palette.item_pane);
}

/* =======================================================

      Property Palette Click Pose
      
======================================================= */

void model_palette_click_pose(int pose_idx,bool double_click)
{
		// bone moves

	if (model_palette.item_pane.click.id>=kPosePropertyBoneMove) {
		state.model.cur_pose_bone_move_idx=model_palette.item_pane.click.id-kPosePropertyBoneMove;
		if (double_click) list_palette_set_level(&model_palette,2);
		return;
	}
}

