/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Dos

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
extern file_path_setup_type		file_path_setup;
extern app_state_type			state;

extern list_palette_type		file_palette,model_palette;

/* =======================================================

      Reset State
      
======================================================= */

void model_file_reset_state(void)
{
	int				n;

		// if there is an open model,
		// change the palette opens

	if (state.model.model_open) {
		file_palette.open=FALSE;
		model_palette.open=TRUE;
		model_palette_reset();
	}

		// setup state

	state.model.texture_edit_idx=-1;

	state.model.texture=TRUE;
	state.model.mesh=FALSE;
	state.model.bone=FALSE;
    state.model.hit_box=FALSE;
	state.model.normal=FALSE;
	state.model.view_box=FALSE;
	state.model.bone_names=TRUE;
	state.model.sel_vertex_with_bone=FALSE;

	state.model.ang.x=0;
    state.model.ang.y=180;
	
	state.model.shift.x=0;
	state.model.shift.y=0;
	
	state.model.select_mode=select_mode_vertex;
	state.model.drag_bone_mode=handle_mode_rotate;
	
	state.model.drag_sel_on=FALSE;
	state.model.magnify_z=3000;

	state.model.cur_item=item_model;
	state.model.cur_mesh_idx=0;
	state.model.cur_bone_idx=-1;

	state.model.cur_pose_idx=-1;
	
	state.model.cur_animate_idx=-1;
	if (model.nanimate!=0) state.model.cur_animate_idx=0;
	
	state.model.cur_animate_pose_move_idx=-1;
	state.model.cur_animate_pose_move_particle_idx=-1;
	state.model.cur_animate_pose_move_ring_idx=-1;
	state.model.cur_pose_bone_move_idx=-1;
	state.model.cur_hit_box_idx=-1;

		// clear selections and states

	model_play(model_play_mode_stop);
	
	state.model.texture_edit_idx=-1;
	
	model_vertex_mask_clear_sel(state.model.cur_mesh_idx);
	model_vertex_mask_clear_hide(state.model.cur_mesh_idx);

	model_poly_mask_clear_sel(state.model.cur_mesh_idx);
	model_poly_mask_clear_hide(state.model.cur_mesh_idx);

	for (n=0;n!=max_model_blend_animation;n++) {
		state.model.blend[n].animate_idx=-1;
	}

	for (n=0;n!=max_model_mesh;n++) {
		state.model.show_mesh[n]=FALSE;
	}

	model_undo_clear();
	model_menu_update();

		// make sure window is
		// seleted

    os_select_window();

	main_wind_draw();
}

/* =======================================================

      New Model
      
======================================================= */

bool model_file_new(void)
{
	char		file_name[256],err_str[256],
				base_path[1024],path[1024];

		// close model

	state.model.texture_edit_idx=-1;

	if (!model_file_close()) return(FALSE);
	
		// get name
	
	strcpy(file_name,"NewModel");
	if (!dialog_new_model_run(file_name)) {
		model_file_reset_state();
		return(FALSE);
	}

		// create model

	os_set_wait_cursor();
		
	model_new(&model,file_name);
	
	model.nmesh=1;
	strcpy(model.meshes[0].name,"Default");
	
	model.view_box.size.x=model.view_box.size.y=model.view_box.size.z=100;

	os_select_window();
	
	os_set_wait_cursor();
	
		// create new folders
		
	file_paths_data_default(&file_path_setup,base_path,"Models",NULL,NULL);
		
	strcat(base_path,"/");
	strcat(base_path,file_name);
	os_create_directory(base_path);
	
	sprintf(path,"%s/Textures",base_path);
	os_create_directory(path);

        // write the XML
	
	if (!model_save(&model,err_str)) {
		os_set_arrow_cursor();
		os_dialog_alert("dim3 Animator could not save model.",err_str);
		model_file_reset_state();
		return(FALSE);
	}
	
	os_set_arrow_cursor();
	
		// finish
		
	state.model.model_open=TRUE;
	strcpy(state.model.model_file_name,file_name);
	
	model_file_reset_state();
	
	return(TRUE);
}

/* =======================================================

      Open Model
      
======================================================= */

bool model_file_open(char *file_name)
{
		// close model

	if (!model_file_close()) return(FALSE);
	
		// open model
		
	os_set_wait_cursor();
	
	os_select_window();
 
	if (!model_open(&model,file_name,TRUE)) {
		os_set_arrow_cursor();
		os_dialog_alert("Animator","There was a problem loading the model file.");
		model_file_reset_state();
		return(FALSE);
	}
    	
	os_set_arrow_cursor();

		// finish
		
	state.model.model_open=TRUE;
	strcpy(state.model.model_file_name,file_name);
	
	model_file_reset_state();

	return(TRUE);
}

/* =======================================================

      Save Model
      
======================================================= */

bool model_file_save(void)
{
	char			err_str[256];
	bool			ok;
	
	os_set_wait_cursor();
	
	ok=model_save(&model,err_str);
	os_set_arrow_cursor();
	
	if (!ok) os_dialog_alert("dim3 Animator could not save model.",err_str);
	
	return(ok);
}

/* =======================================================

      Close Model
      
======================================================= */

bool model_file_close(void)
{
	int			n,choice;
	
		// if no model open, just
		// return OK to close

	if (!state.model.model_open) return(TRUE);

		// check for save

	choice=os_dialog_confirm("Save Changes?","Do you want to save the changes to this model?",TRUE);
	if (choice==1) return(FALSE);
	
	if (choice==0) model_file_save();

		// close model
	
	model_close(&model);
	
	for (n=0;n!=max_model_mesh;n++) {
		model.meshes[n].nvertex=0;
		model.meshes[n].npoly=0;
	}
	
	model.nbone=0;
	model.npose=0;
	model.nanimate=0;
	model.nhit_box=0;
	
	for (n=0;n!=max_model_light;n++) {
		model.bone_connect.light_bone_idx[n]=-1;
	}
	
	for (n=0;n!=max_model_halo;n++) {
		model.bone_connect.halo_bone_idx[n]=-1;
	}
	
	model.bone_connect.name_bone_idx=-1;

	model.label.text.bone_idx=-1;
	model.label.bitmap.bone_idx=-1;
	model.label.bar.bone_idx=-1;

		// reset state

	state.model.model_open=FALSE;
	model_file_reset_state();

	return(TRUE);
}

/* =======================================================

      Import Meshes
      
======================================================= */

void model_file_import_mesh_obj(bool replace)
{
	char			path[1024],err_str[256];
	bool			found_normals;
    
	os_set_arrow_cursor();
	if (!os_load_file("Select an OBJ to Import",path,"obj")) return;
	
	if (state.model.cur_mesh_idx==-1) state.model.cur_mesh_idx=0;
	
	if (!model_import_obj(path,replace,&found_normals,err_str)) {
		os_dialog_alert("OBJ Import",err_str);
		return;
	}

		// finish setup
		
	model_vertex_mask_clear_sel(state.model.cur_mesh_idx);
	model_vertex_mask_clear_hide(state.model.cur_mesh_idx);

	model_poly_mask_clear_sel(state.model.cur_mesh_idx);
	model_poly_mask_clear_hide(state.model.cur_mesh_idx);
	
	main_wind_draw();
}

/* =======================================================

      Insert Meshes
      
======================================================= */

void model_file_insert_mesh_dim3_model(void)
{
	char			file_name[256];
	
	os_set_arrow_cursor();

    if (!dialog_file_open_run("Open a Model","Models",NULL,"Mesh.xml;Model.xml",file_name)) return;
	
	if (state.model.cur_mesh_idx==-1) state.model.cur_mesh_idx=0;
	
	os_set_wait_cursor();
	
	model_insert_mesh(file_name);
	
    model_calculate_parents(&model);
    model_center_xz(&model,state.model.cur_mesh_idx);
    model_floor(&model,state.model.cur_mesh_idx);
    model_recalc_boxes(&model);
	
	os_set_arrow_cursor();
	
	main_wind_draw();
}

/* =======================================================

      Insert Animations
      
======================================================= */

void model_file_import_animations(void)
{
	char			file_name[256];
	
	os_set_arrow_cursor();

    if (!dialog_file_open_run("Open a Model","Models",NULL,"Mesh.xml;Model.xml",file_name)) return;
	
	os_set_wait_cursor();
	
	model_insert_animations(file_name);
	
    model_calculate_parents(&model);
	
	os_set_arrow_cursor();
	
	main_wind_draw();
}

/* =======================================================

      Auto Generate Textures
      
======================================================= */

void model_auto_generate_textures(void)
{
	char			base_path[1024],dir_path[1024];

	os_set_wait_cursor();

		// texture directory

	file_paths_data_default(&file_path_setup,base_path,"Models",NULL,NULL);
		
	strcat(base_path,"/");
	strcat(base_path,state.model.model_file_name);
	
	sprintf(dir_path,"%s/Textures",base_path);

		// run bitmap generation for model

	auto_generate_model_create_texture_set(&model,dir_path);

	os_set_arrow_cursor();

	main_wind_draw();
}
