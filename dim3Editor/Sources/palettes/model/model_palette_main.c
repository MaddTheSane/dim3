/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette Main

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

#define kModelPropertyMeshAdd				100		// need to be greater than item numbers
#define kModelPropertyAnimateAdd			101
#define kModelPropertyPoseAdd				102
#define kModelPropertyBoneAdd				103
#define kModelPropertyHitBoxAdd				104

extern model_type				model;
extern app_state_type			state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		model_palette;

/* =======================================================

      Property Palette Fill Main
      
======================================================= */

void model_palette_fill_main(void)
{
	int			n;

	list_palette_set_title(&model_palette,"Model",NULL,NULL,NULL,NULL,NULL);

		// map

	list_palette_add_header(&model_palette,item_model,"Model");
	list_palette_add_item(&model_palette,item_model,0,"Settings",(state.model.cur_item==item_model),FALSE);

		// meshes

	list_palette_add_header_button(&model_palette,kModelPropertyMeshAdd,"Meshes",list_button_plus);

	for (n=0;n!=model.nmesh;n++) {
		list_palette_add_item(&model_palette,item_model_mesh,n,model.meshes[n].name,((state.model.cur_item==item_model_mesh)&&(state.model.cur_mesh_idx==n)),FALSE);
	}

		// animations

	list_palette_add_header_button(&model_palette,kModelPropertyAnimateAdd,"Animations",list_button_plus);
	list_palette_sort_mark_start(&model_palette,&model_palette.item_pane);

	for (n=0;n!=model.nanimate;n++) {
		list_palette_add_item(&model_palette,item_model_animate,n,model.animates[n].name,((state.model.cur_item==item_model_animate)&&(state.model.cur_animate_idx==n)),FALSE);
	}

	list_palette_sort(&model_palette,&model_palette.item_pane);

		// poses

	list_palette_add_header_button(&model_palette,kModelPropertyPoseAdd,"Poses",list_button_plus);
	list_palette_add_item(&model_palette,item_model_neutral_pose,0,"[Neutral]",((state.model.cur_item==item_model_neutral_pose)&&(state.model.cur_pose_idx==-1)),FALSE);

	list_palette_sort_mark_start(&model_palette,&model_palette.item_pane);

	for (n=0;n!=model.npose;n++) {
		list_palette_add_item(&model_palette,item_model_pose,n,model.poses[n].name,((state.model.cur_item==item_model_pose)&&(state.model.cur_pose_idx==n)),FALSE);
	}

	list_palette_sort(&model_palette,&model_palette.item_pane);

		// bones

	list_palette_add_header_button(&model_palette,kModelPropertyBoneAdd,"Bones",list_button_plus);

	list_palette_sort_mark_start(&model_palette,&model_palette.item_pane);

	for (n=0;n!=model.nbone;n++) {
		list_palette_add_item(&model_palette,item_model_bone,n,model.bones[n].name,((state.model.cur_item==item_model_bone)&&(state.model.cur_bone_idx==n)),FALSE);
	}

	list_palette_sort(&model_palette,&model_palette.item_pane);

		// hit boxes

	list_palette_add_header_button(&model_palette,kModelPropertyHitBoxAdd,"Hit Boxes",list_button_plus);
	list_palette_sort_mark_start(&model_palette,&model_palette.item_pane);

	for (n=0;n!=model.nhit_box;n++) {
		list_palette_add_item(&model_palette,item_model_hit_box,n,model.hit_boxes[n].name,((state.model.cur_item==item_model_hit_box)&&(state.model.cur_hit_box_idx==n)),FALSE);
	}

	list_palette_sort(&model_palette,&model_palette.item_pane);
}

/* =======================================================

      Property Palette Click Main
      
======================================================= */

void model_palette_click_main(bool double_click)
{
		// adding

	switch (model_palette.item_pane.click.id) {

		case kModelPropertyMeshAdd:
			model_piece_add_mesh();
			return;

		case kModelPropertyAnimateAdd:
			model_piece_add_animate();
			return;

		case kModelPropertyPoseAdd:
			model_piece_add_pose();
			return;

		case kModelPropertyBoneAdd:
			model_piece_add_bone();
			return;

		case kModelPropertyHitBoxAdd:
			model_piece_add_hit_box();
			return;

	}

		// regular clicks

	if (model_palette.item_pane.click.idx==-1) return;

		// handle click

	switch (model_palette.item_pane.click.id) {

		case item_model:
			state.model.cur_item=item_model;
			break;

		case item_model_mesh:
			state.model.cur_item=item_model_mesh;
			state.model.cur_mesh_idx=model_palette.item_pane.click.idx;
			break;

		case item_model_animate:
			state.model.cur_item=item_model_animate;
			state.model.cur_animate_idx=model_palette.item_pane.click.idx;
			state.model.cur_animate_pose_move_idx=-1;
			state.model.cur_animate_pose_move_particle_idx=-1;
			state.model.cur_animate_pose_move_ring_idx=-1;
			break;

		case item_model_pose:
			state.model.cur_item=item_model_pose;
			state.model.cur_pose_idx=model_palette.item_pane.click.idx;
			state.model.cur_pose_bone_move_idx=-1;
			break;
			
		case item_model_neutral_pose:
			state.model.cur_item=item_model_neutral_pose;
			state.model.cur_pose_idx=-1;
			state.model.cur_pose_bone_move_idx=-1;
			break;

		case item_model_bone:
			state.model.cur_item=item_model_bone;
			state.model.cur_bone_idx=model_palette.item_pane.click.idx;
			break;

		case item_model_hit_box:
			state.model.cur_item=item_model_hit_box;
			state.model.cur_hit_box_idx=model_palette.item_pane.click.idx;
			break;
	}

		// netural pose has no properties

	if (model_palette.item_pane.click.id==item_model_neutral_pose) {
		list_palette_set_level(&model_palette,0);
		return;
	}

		// switch to properties
		// if double-click

	if (double_click) {
		list_palette_set_level(&model_palette,1);
	}
}

