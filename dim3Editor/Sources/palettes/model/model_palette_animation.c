/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette Animation

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

#define kAnimationPropertyName					0
#define kAnimationPropertyLoop					1
#define kAnimationPropertyNoSmooth				2
#define kAnimationPropertyAutoStop				3
#define kAnimationPropertyEnhanceFactor			4

#define kAnimationPropertyPoseAdd				10

#define kAnimationPropertyPoseMove				1000
#define kAnimationPropertyPoseMoveDelete		2000
#define kAnimationPropertyPoseMoveUp			3000
#define kAnimationPropertyPoseMoveDown			4000

extern model_type				model;
extern app_state_type			state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		model_palette;

/* =======================================================

      Property Palette Fill Animation
      
======================================================= */

void model_palette_fill_animation(int animate_idx)
{
	int						n;
	char					str[256];
	model_animate_type		*animate;

	animate=&model.animates[animate_idx];

	list_palette_set_title(&model_palette,"Animation",animate->name,NULL,NULL,NULL,NULL);

	list_palette_add_header(&model_palette,0,"Animation Options");
	list_palette_add_string(&model_palette,kAnimationPropertyName,"Name",animate->name,name_str_len,FALSE);

	list_palette_add_header(&model_palette,0,"Animation Settings");
	list_palette_add_checkbox(&model_palette,kAnimationPropertyLoop,"Looping",&animate->loop,FALSE);
	list_palette_add_checkbox(&model_palette,kAnimationPropertyNoSmooth,"No Smoothing",&animate->no_smooth,FALSE);
	list_palette_add_checkbox(&model_palette,kAnimationPropertyAutoStop,"Auto Stop",&animate->auto_stop,FALSE);
	list_palette_add_float(&model_palette,kAnimationPropertyEnhanceFactor,"Speed Enhance Factor",&animate->enhance_factor,FALSE);
	
	list_palette_add_header_button(&model_palette,kAnimationPropertyPoseAdd,"Animation Poses",list_button_plus);

	for (n=0;n!=animate->npose_move;n++) {
		sprintf(str,"%s [%d]",model.poses[animate->pose_moves[n].pose_idx].name,animate->pose_moves[n].msec);
		if ((n==animate->loop_start) && (n==animate->loop_end)) {
			strcat(str," (loop start/end)");
		}
		else {
			if (n==animate->loop_start) strcat(str," (loop start)");
			if (n==animate->loop_end) strcat(str," (loop end)");
		}
		list_palette_add_string_selectable_moveable_button(&model_palette,(kAnimationPropertyPoseMove+n),list_button_minus,(kAnimationPropertyPoseMoveDelete+n),(kAnimationPropertyPoseMoveUp+n),(kAnimationPropertyPoseMoveDown+n),str,((state.model.cur_animate_idx==animate_idx) && (state.model.cur_animate_pose_move_idx==n)),FALSE);
	}
}

/* =======================================================

      Property Palette Click Animation
      
======================================================= */

void model_palette_click_animation(int animate_idx,bool double_click)
{
	int						id;
	model_animate_type		*animate;

	animate=&model.animates[animate_idx];

	id=model_palette.item_pane.click.id;
	
		// pose moves
		
	if ((id>=kAnimationPropertyPoseMove) && (id<kAnimationPropertyPoseMoveDelete)) {
		state.model.cur_animate_pose_move_idx=id-kAnimationPropertyPoseMove;
		state.model.cur_pose_idx=animate->pose_moves[state.model.cur_animate_pose_move_idx].pose_idx;
		if (double_click) list_palette_set_level(&model_palette,2);
		return;
	}
	
		// pose move delete
		
	if ((id>=kAnimationPropertyPoseMoveDelete) && (id<kAnimationPropertyPoseMoveUp)) {
		state.model.cur_animate_pose_move_idx=-1;
		model_animate_pose_delete(&model,animate_idx,(id-kAnimationPropertyPoseMoveDelete));
		return;
	}
	
		// move pose up
		
	if ((id>=kAnimationPropertyPoseMoveUp) && (id<kAnimationPropertyPoseMoveDown)) {
		state.model.cur_animate_pose_move_idx=model_shift_animation_pose_move(state.model.cur_animate_idx,(id-kAnimationPropertyPoseMoveUp),-1);
		state.model.cur_pose_idx=animate->pose_moves[state.model.cur_animate_pose_move_idx].pose_idx;
		return;
	}
	
		// move pose down
		
	if (id>=kAnimationPropertyPoseMoveDown) {
		state.model.cur_animate_pose_move_idx=model_shift_animation_pose_move(state.model.cur_animate_idx,(id-kAnimationPropertyPoseMoveDown),1);
		state.model.cur_pose_idx=animate->pose_moves[state.model.cur_animate_pose_move_idx].pose_idx;
		return;
	}
	
		// pose move add
		
	if (id==kAnimationPropertyPoseAdd) {
		state.model.cur_animate_pose_move_idx=model_animate_pose_insert(&model,animate_idx,state.model.cur_animate_pose_move_idx,0);
		list_palette_start_picking_mode(&model_palette,"Pick a Pose",(char*)model.poses,model.npose,sizeof(model_pose_type),(int)offsetof(model_pose_type,name),FALSE,FALSE,TRUE,&model.animates[animate_idx].pose_moves[state.model.cur_animate_pose_move_idx].pose_idx,NULL);
		return;
	}
}

