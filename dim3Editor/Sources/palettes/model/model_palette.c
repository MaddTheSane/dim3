/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette

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

extern model_type				model;
extern model_draw_setup			draw_setup;
extern app_state_type			state;
extern file_path_setup_type		file_path_setup;
extern iface_type				iface;

char							property_bone_list[max_model_bone][name_str_len+1];

list_palette_type				model_palette;

/* =======================================================

      Property Palette Setup
      
======================================================= */

void model_palette_initialize(void)
{
	list_palette_list_initialize(&model_palette,"No Properties",FALSE,FALSE,FALSE);

	model_palette.item_pane.click.id=0;
	model_palette.item_pane.click.idx=-1;
	model_palette.item_pane.click.item=NULL;

	model_palette.open=FALSE;
}

void model_palette_shutdown(void)
{
	list_palette_list_shutdown(&model_palette);
}

/* =======================================================

      Property Palette Fill
      
======================================================= */

void model_palette_fill_level_0(void)
{
	model_palette_fill_main();
}

void model_palette_fill_level_1(void)
{
	switch (state.model.cur_item) {

		case item_model:
			model_palette_fill_model();
			break;

		case item_model_mesh:
			model_palette_fill_mesh(state.model.cur_mesh_idx);
			break;

		case item_model_animate:
			model_palette_fill_animation(state.model.cur_animate_idx);
			break;

		case item_model_pose:
			model_palette_fill_pose(state.model.cur_pose_idx);
			break;

		case item_model_bone:
			model_palette_fill_bone(state.model.cur_bone_idx,state.model.cur_pose_idx);
			break;

		case item_model_hit_box:
			model_palette_fill_hit_box(state.model.cur_hit_box_idx);
			break;

	}
}

void model_palette_fill_level_2(void)
{
	switch (state.model.cur_item) {

		case item_model_animate:
			model_palette_fill_animate_pose_move(state.model.cur_animate_idx,state.model.cur_animate_pose_move_idx);
			break;

		case item_model_pose:
			model_palette_fill_pose_bone_move(state.model.cur_pose_idx,state.model.cur_pose_bone_move_idx);
			break;

	}
}

void model_palette_fill_level_3(void)
{
	switch (state.model.cur_item) {

		case item_model_animate:
			if ((state.model.cur_animate_idx==-1) || (state.model.cur_animate_pose_move_idx==-1)) break;

			if (state.model.cur_animate_pose_move_particle_idx!=-1) {
				model_palette_fill_animate_pose_move_particle(state.model.cur_animate_idx,state.model.cur_animate_pose_move_idx,state.model.cur_animate_pose_move_particle_idx);
				break;
			}

			if (state.model.cur_animate_pose_move_ring_idx!=-1) {
				model_palette_fill_animate_pose_move_ring(state.model.cur_animate_idx,state.model.cur_animate_pose_move_idx,state.model.cur_animate_pose_move_ring_idx);
				break;
			}

			break;

	}
}

/* =======================================================

      Property Palette Draw
      
======================================================= */

void model_palette_draw(void)
{
		// delete the properties

	list_palette_delete_all_items(&model_palette);

		// if texture window is up,
		// put in texture properties

	if (state.model.texture_edit_idx!=-1) {
		model_palette_fill_texture(state.model.texture_edit_idx);
		list_palette_draw(&model_palette);
		return;
	}

		// fill by level

	switch (list_palette_get_level(&model_palette)) {
		case 0:
			model_palette_fill_level_0();
			break;
		case 1:
			model_palette_fill_level_1();
			break;
		case 2:
			model_palette_fill_level_2();
			break;
		case 3:
			model_palette_fill_level_3();
			break;
	}

	list_palette_draw(&model_palette);
}

/* =======================================================

      Property Palette Scrolling
      
======================================================= */

void model_palette_reset(void)
{
	model_palette.item_pane.scroll_offset=0;
}

void model_palette_scroll_into_view(int item_type,int item_idx)
{
		// can only scroll into view
		// on main page

	if (list_palette_get_level(&model_palette)!=0) return;

	model_palette_fill_level_0();
	list_palette_scroll_item_into_view(&model_palette,item_type,item_idx);
}

void model_palette_scroll_wheel(d3pnt *pnt,int move)
{
	list_palette_scroll_wheel(&model_palette,pnt,move);
}

/* =======================================================

      Property Palette Delete
      
======================================================= */

bool model_palette_delete(void)
{
		// only delete on first level

	if (list_palette_get_level(&model_palette)!=0) return(FALSE);

		// anything to delete?

	if ((model_palette.item_pane.click.id==-1) || (model_palette.item_pane.click.idx==-1)) return(FALSE);

	switch (model_palette.item_pane.click.id) {

		case item_model_mesh:
			if (os_dialog_confirm("Delete Mesh","Is it okay to delete this mesh?",FALSE)!=0) return(FALSE);
			model_piece_delete_mesh(model_palette.item_pane.click.idx);
			return(TRUE);

		case item_model_bone:
			if (os_dialog_confirm("Delete Bone","Is it okay to delete this bone?",FALSE)!=0) return(FALSE);
			model_piece_delete_bone(model_palette.item_pane.click.idx);
			return(TRUE);

		case item_model_pose:
			if (os_dialog_confirm("Delete Pose","Is it okay to delete this pose?",FALSE)!=0) return(FALSE);
			model_piece_delete_pose(model_palette.item_pane.click.idx);
			return(TRUE);

		case item_model_animate:
			if (os_dialog_confirm("Delete Animation","Is it okay to delete this animation?",FALSE)!=0) return(FALSE);
			model_piece_delete_animate(model_palette.item_pane.click.idx);
			return(TRUE);

		case item_model_hit_box:
			if (os_dialog_confirm("Delete Hit Box","Is it okay to delete this hit box?",FALSE)!=0) return(FALSE);
			model_piece_delete_hit_box(model_palette.item_pane.click.idx);
			return(TRUE);

	}

	return(FALSE);
}

/* =======================================================

      Property Palette Click For Levels
      
======================================================= */

void model_palette_click_level_0(bool double_click)
{
	model_palette_click_main(double_click);
}

void model_palette_click_level_1(bool double_click)
{
	switch (state.model.cur_item) {

		case item_model:
			model_palette_click_model(double_click);
			break;

		case item_model_mesh:
			model_palette_click_mesh(state.model.cur_mesh_idx,double_click);
			break;

		case item_model_animate:
			model_palette_click_animation(state.model.cur_animate_idx,double_click);
			break;

		case item_model_pose:
			model_palette_click_pose(state.model.cur_pose_idx,double_click);
			break;

		case item_model_bone:
			model_palette_click_bone(state.model.cur_bone_idx,state.model.cur_pose_idx,double_click);
			break;

		case item_model_hit_box:
			model_palette_click_hit_box(state.model.cur_hit_box_idx,double_click);
			break;

	}
}

void model_palette_click_level_2(bool double_click)
{
	switch (state.model.cur_item) {

		case item_model_animate:
			model_palette_click_animate_pose_move(state.model.cur_animate_idx,state.model.cur_animate_pose_move_idx,double_click);
			break;

		case item_model_pose:
			model_palette_click_pose_bone_move(state.model.cur_pose_idx,state.model.cur_pose_bone_move_idx,double_click);
			break;

	}
}

void model_palette_click_level_3(bool double_click)
{
	switch (state.model.cur_item) {

		case item_model_animate:
			if ((state.model.cur_animate_idx==-1) || (state.model.cur_animate_pose_move_idx==-1)) break;

			if (state.model.cur_animate_pose_move_particle_idx!=-1) {
				model_palette_click_animate_pose_move_particle(state.model.cur_animate_idx,state.model.cur_animate_pose_move_idx,state.model.cur_animate_pose_move_particle_idx,double_click);
				break;
			}

			if (state.model.cur_animate_pose_move_ring_idx!=-1) {
				model_palette_click_animate_pose_move_ring(state.model.cur_animate_idx,state.model.cur_animate_pose_move_idx,state.model.cur_animate_pose_move_ring_idx,double_click);
				break;
			}

			break;

	}
}

/* =======================================================

      Property Palette Click MainLine
      
======================================================= */

void model_palette_click(d3pnt *pnt,bool double_click)
{
	bool				old_open;

		// check if open changes
	
	old_open=list_palette_is_open(&model_palette);

		// click

	if (!list_palette_click(&model_palette,pnt,double_click)) {
		if (old_open!=list_palette_is_open(&model_palette)) main_wind_draw();
		return;
	}

		// no clicks if no model

	if (!state.model.model_open) return;

		// if texture window is up, texture properties

	if (state.model.texture_edit_idx!=-1) {
		model_palette_click_texture(state.model.texture_edit_idx,model_palette.item_pane.click.id,double_click);
		main_wind_draw();
		return;
	}

		// click by level

	switch (list_palette_get_level(&model_palette)) {
		case 0:
			model_palette_click_level_0(double_click);
			break;
		case 1:
			model_palette_click_level_1(double_click);
			break;
		case 2:
			model_palette_click_level_2(double_click);
			break;
		case 3:
			model_palette_click_level_3(double_click);
			break;
	}
	
	main_wind_draw();
}

