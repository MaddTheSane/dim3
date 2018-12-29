/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Model Menus

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

extern model_type				model;
extern app_state_type			state;

extern list_palette_type		model_palette;

os_menu_item_type		model_menu_setup[]=
								{

										// File menu

									{"File","Save",model_menu_item_FileSave,os_menu_key_cmd,'S'},
								#ifdef D3_OS_WINDOWS
									{"File","",0,os_menu_key_none,0x0},
									{"File","Exit",model_menu_item_FileQuit,os_menu_key_none,0x0},
								#endif

										// edit menu

									{"Edit","Undo",model_menu_item_EditUndo,os_menu_key_cmd,'Z'},
									{"Edit","",0,os_menu_key_none,0x0},
									{"Edit","Select More",model_menu_item_EditSelectMore,os_menu_key_cmd,'M'},

										// model menu

									{"Model","Prepare Model",model_menu_item_PrepareModel,os_menu_key_cmd,'R'},
									{"Model","Calculate Boxes",model_menu_item_CalcBoxes,os_menu_key_none,0x0},
									{"Model","Calculate Normals",model_menu_item_CalcNormals,os_menu_key_none,0x0},
									{"Model","",0,os_menu_key_none,0x0},
									{"Model","Import Animations...",model_menu_item_ImportAnimation,os_menu_key_none,0x0},
									{"Model","",0,os_menu_key_none,0x0},
									{"Model","Scale...",model_menu_item_ScaleAll,os_menu_key_none,0x0},
									{"Model","",0,os_menu_key_none,0x0},
									{"Model","Flip X",model_menu_item_FlipXAll,os_menu_key_none,0x0},
									{"Model","Flip Y",model_menu_item_FlipYAll,os_menu_key_none,0x0},
									{"Model","Flip Z",model_menu_item_FlipZAll,os_menu_key_none,0x0},
									{"Model","",0,os_menu_key_none,0x0},
									{"Model","Swap X/Z",model_menu_item_SwapXZAll,os_menu_key_none,0x0},
									{"Model","Swap Y/Z",model_menu_item_SwapYZAll,os_menu_key_none,0x0},
									{"Model","Center X/Z",model_menu_item_CenterXZAll,os_menu_key_none,0x0},
									{"Model","Floor Y",model_menu_item_FloorYAll,os_menu_key_none,0x0},
									{"Model","",0,os_menu_key_none,0x0},
									{"Model","Add Hit Box",model_menu_item_AddHitBox,os_menu_key_none,0x0},
									{"Model","",0,os_menu_key_none,0x0},
									{"Model","Auto Generate Textures...",model_menu_item_AutoGenerateTextures,os_menu_key_none,0x0},
									{"Model","Auto Generate Model...",model_menu_item_AutoGenerateModel,os_menu_key_none,0x0},
									{"Model","",0,os_menu_key_none,0x0},
									{"Model","Clean Up",model_menu_item_CleanUp,os_menu_key_none,0x0},

										// view menu

									{"View","Front",model_menu_item_Front,os_menu_key_cmd,'1'},
									{"View","Left",model_menu_item_Left,os_menu_key_cmd,'2'},
									{"View","Right",model_menu_item_Right,os_menu_key_cmd,'3'},
									{"View","Back",model_menu_item_Back,os_menu_key_cmd,'4'},
									{"View","Top",model_menu_item_Top,os_menu_key_cmd,'5'},
									{"View","Bottom",model_menu_item_Bottom,os_menu_key_cmd,'6'},

										// mesh menu

									{"Mesh","Add Mesh",model_menu_item_NewMesh,os_menu_key_none,0x0},
									{"Mesh","Duplicate Mesh",model_menu_item_DuplicateMesh,os_menu_key_none,0x0},
									{"Mesh","",0,os_menu_key_none,0x0},
									{"Mesh","Import OBJ...",model_menu_item_ImportOBJ,os_menu_key_none,0x0},
									{"Mesh","Replace OBJ...",model_menu_item_ReplaceOBJ,os_menu_key_none,0x0},
									{"Mesh","Add dim3 Mesh...",model_menu_item_InsertXML,os_menu_key_none,0x0},
									{"Mesh","",0,os_menu_key_none,0x0},
									{"Mesh","Scale...",model_menu_item_Scale,os_menu_key_cmd,'T'},
									{"Mesh","",0,os_menu_key_none,0x0},
									{"Mesh","Flip X",model_menu_item_FlipX,os_menu_key_none,0x0},
									{"Mesh","Flip Y",model_menu_item_FlipY,os_menu_key_none,0x0},
									{"Mesh","Flip Z",model_menu_item_FlipZ,os_menu_key_none,0x0},
									{"Mesh","",0,os_menu_key_none,0x0},
									{"Mesh","Flip Texture Us",model_menu_item_FlipU,os_menu_key_none,0x0},
									{"Mesh","Flip Texture Vs",model_menu_item_FlipV,os_menu_key_none,0x0},
									{"Mesh","",0,os_menu_key_none,0x0},
									{"Mesh","Swap X/Z",model_menu_item_SwapXZ,os_menu_key_none,0x0},
									{"Mesh","Swap Y/Z",model_menu_item_SwapYZ,os_menu_key_none,0x0},
									{"Mesh","Center X/Z",model_menu_item_CenterXZ,os_menu_key_none,0x0},
									{"Mesh","Floor Y",model_menu_item_FloorY,os_menu_key_none,0x0},
									{"Mesh","",0,os_menu_key_none,0x0},
									{"Mesh","Tessellate",model_menu_item_MeshTessellate,os_menu_key_none,0x0},

										// vertex menu

									{"Vertex","Select All",model_menu_item_VertexSelectAll,os_menu_key_cmd,'A'},
									{"Vertex","Select Attached",model_menu_item_VertexSelectAttached,os_menu_key_cmd_opt,'A'},
									{"Vertex","Select Not Attached",model_menu_item_VertexSelectNotAttached,os_menu_key_cmd_shift,'A'},
									{"Vertex","",0,os_menu_key_none,0x0},
									{"Vertex","Show All",model_menu_item_VertexShowAll,os_menu_key_none,0x0},
									{"Vertex","Hide Selected",model_menu_item_VertexHideSelected,os_menu_key_none,0x0},
									{"Vertex","Hide Non-Selected",model_menu_item_VertexHideNonSelected,os_menu_key_none,0x0},
									{"Vertex","",0,os_menu_key_none,0x0},
									{"Vertex","Invert Normals",model_menu_item_VertexInvertNormals,os_menu_key_cmd,'I'},
									{"Vertex","Set Normals...",model_menu_item_VertexSetNormals,os_menu_key_cmd_opt,'I'},
									{"Vertex","Set Normals Out",model_menu_item_VertexSetNormalsOut,os_menu_key_none,0x0},
									{"Vertex","Set Normals In",model_menu_item_VertexSetNormalsIn,os_menu_key_none,0x0},
									{"Vertex","",0,os_menu_key_none,0x0},
									{"Vertex","Clear Bone Attachments",model_menu_item_VertexClearBones,os_menu_key_none,0x0},
									{"Vertex","Auto Set Bone Attachments",model_menu_item_VertexAutoBones,os_menu_key_none,0x0},
									{"Vertex","",0,os_menu_key_none,0x0},
									{"Vertex","Collapse Selected Vertexes",model_menu_item_VertexCollapseSelected,os_menu_key_none,0x0},
									{"Vertex","Collapse Similar Vertexes",model_menu_item_VertexCollapseSimilar,os_menu_key_none,0x0},
									{"Vertex","",0,os_menu_key_none,0x0},
									{"Vertex","Delete Vertexes",model_menu_item_VertexDelete,os_menu_key_none,0x0},
									{"Vertex","Delete Polygons",model_menu_item_VertexPolygonDelete,os_menu_key_none,0x0},
									{"Vertex","",0,os_menu_key_none,0x0},
									{"Vertex","Make Quad",model_menu_item_VertexMakeQuad,os_menu_key_none,0x0},
									{"Vertex","Tessellate Polygon",model_menu_item_VertexTessellatePoly,os_menu_key_none,0x0},

										// bone menu

									{"Bone","Add Bone",model_menu_item_NewBone,os_menu_key_cmd,'B'},
									{"Bone","Set Bone...",model_menu_item_SetBone,os_menu_key_none,0x0},
									{"Bone","",0,os_menu_key_none,0x0},
									{"Bone","Goto Parent Bone",model_menu_item_GoToParentBone,os_menu_key_cmd_opt,'B'},
									{"Bone","",0,os_menu_key_none,0x0},
									{"Bone","Duplicate Bone Attachments",model_menu_item_DuplicateBoneAttach,os_menu_key_none,0x0},

										// pose menu

									{"Pose","New Pose",model_menu_item_NewPose,os_menu_key_cmd,'E'},
									{"Pose","Duplicate Pose",model_menu_item_DupPose,os_menu_key_cmd,'D'},
									{"Pose","",0,os_menu_key_none,0x0},
									{"Pose","Clear Pose",model_menu_item_ClearPose,os_menu_key_none,0x0},
									{"Pose","",0,os_menu_key_none,0x0},
									{"Pose","Previous Pose",model_menu_item_PreviousPose,os_menu_key_cmd,'['},
									{"Pose","Next Pose",model_menu_item_NextPose,os_menu_key_cmd,']'},

										// animation menu

									{"Animation","New Animation",model_menu_item_NewAnimate,os_menu_key_cmd,'G'},
									{"Animation","Duplicate Animation",model_menu_item_DupAnimate,os_menu_key_none,0x0},
									{"Animation","",0,os_menu_key_none,0x0},
									{"Animation","Scale Animation Time...",model_menu_item_ResetTimeAnimate,os_menu_key_none,0x0},
									{"Animation","",0,os_menu_key_none,0x0},
									{"Animation","Play Animation",model_menu_item_PlayAnimate,os_menu_key_cmd,'P'},
									{"Animation","Play Blended Animation...",model_menu_item_PlayBlendAnimate,os_menu_key_cmd_shift,'P'},

									{"","",-1,os_menu_key_none,0x0},
							};

/* =======================================================

      Create Menu
      
======================================================= */

void model_menu_create(void)
{
	os_menu_create(model_menu_setup);
}

void model_menu_dispose(void)
{
	os_menu_dispose();
}

/* =======================================================

      Menu Enable/Disable
      
======================================================= */

void model_menu_update(void)
{
	if (!state.model.model_open) {
		os_menu_enable_item(model_menu_file,1,FALSE);
   
		os_menu_enable_item(model_menu_edit,0,FALSE);
		os_menu_enable_item(model_menu_model,0,FALSE);
		os_menu_enable_item(model_menu_view,0,FALSE);
		os_menu_enable_item(model_menu_mesh,0,FALSE);
		os_menu_enable_item(model_menu_vertex,0,FALSE);
		os_menu_enable_item(model_menu_bone,0,FALSE);
		os_menu_enable_item(model_menu_pose,0,FALSE);
		os_menu_enable_item(model_menu_animate,0,FALSE);
	}
	else {
		os_menu_enable_item(model_menu_file,1,TRUE);
	
		os_menu_enable_item(model_menu_edit,0,TRUE);
		os_menu_enable_item(model_menu_model,0,TRUE);
		os_menu_enable_item(model_menu_view,0,TRUE);
		os_menu_enable_item(model_menu_mesh,0,TRUE);
		os_menu_enable_item(model_menu_vertex,0,TRUE);
		os_menu_enable_item(model_menu_bone,0,TRUE);
		os_menu_enable_item(model_menu_pose,0,TRUE);
		os_menu_enable_item(model_menu_animate,0,TRUE);
	}
}

/* =======================================================

      Misc Menu Routines
      
======================================================= */

void model_prepare(void)
{
    model_calculate_parents(&model);
    model_center_xz_all(&model);
    model_floor_all(&model);
    model_recalc_boxes(&model);
}

void model_setup_scale(d3fpnt *scale)
{
	int				minx,maxx,minz,maxz,miny,maxy;

	model_get_vertex_extent_all(&model,&minx,&maxx,&minz,&maxz,&miny,&maxy);
	scale->x=(float)abs(maxx-minx);
	scale->y=(float)abs(maxy-miny);
	scale->z=(float)abs(maxz-minz);
}

/* =======================================================

      Menu Events
      
======================================================= */

bool model_menu_event_run(int cmd)
{
	int				major_bone_idx,minor_bone_idx,parent_idx;
	float			bone_factor;
	d3fpnt			scale;
	
	switch (cmd) {
		
			// file menu

		case model_menu_item_FileSave:
			model_file_save();
			return(TRUE);

		case model_menu_item_FileQuit:
			if (model_file_close()) {
				os_application_quit();
			}
			return(TRUE);
			
			// edit menu

		case model_menu_item_EditUndo:
			model_undo_run();
			main_wind_draw();
			return(TRUE);

		case model_menu_item_EditSelectMore:
			model_poly_mask_select_more(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
			// model menu
        
		case model_menu_item_PrepareModel:
            model_prepare();
            main_wind_draw();
			return(TRUE);
			
		case model_menu_item_CalcBoxes:
            model_recalc_boxes(&model);
            main_wind_draw();
			return(TRUE);
            
		case model_menu_item_CalcNormals:
            model_recalc_normals(&model,FALSE);
            main_wind_draw();
			return(TRUE);
			
		case model_menu_item_ImportAnimation:
			model_file_import_animations();
			main_wind_draw();
			return(TRUE);
           
		case model_menu_item_ScaleAll:
			model_setup_scale(&scale);
			if (!dialog_scale_run(&scale)) return(TRUE);
			model_scale_all(&model,&scale);
            main_wind_draw();
			return(TRUE);
			
		case model_menu_item_FlipXAll:
			model_flip_all(&model,TRUE,FALSE,FALSE);
            main_wind_draw();
			return(TRUE);
			
		case model_menu_item_FlipYAll:
			model_flip_all(&model,FALSE,TRUE,FALSE);
            main_wind_draw();
			return(TRUE);

		case model_menu_item_FlipZAll:
			model_flip_all(&model,FALSE,FALSE,TRUE);
            main_wind_draw();
			return(TRUE);
			
		case model_menu_item_SwapXZAll:
			model_swap_xz_all(&model);
            main_wind_draw();
			return(TRUE);
			
		case model_menu_item_SwapYZAll:
			model_swap_yz_all(&model);
            main_wind_draw();
			return(TRUE);
            
		case model_menu_item_CenterXZAll:
			model_center_xz_all(&model);
            main_wind_draw();
			return(TRUE);
            
		case model_menu_item_FloorYAll:
			model_floor_all(&model);
            main_wind_draw();
			return(TRUE);

		case model_menu_item_AddHitBox:
			model_piece_add_hit_box();
            main_wind_draw();
			return(TRUE);

		case model_menu_item_AutoGenerateTextures:
			model_auto_generate_textures();
            main_wind_draw();
			return(TRUE);

		case model_menu_item_AutoGenerateModel:
			auto_generate_model_monster(&model);
            main_wind_draw();
			return(TRUE);
			
		case model_menu_item_CleanUp:
			model_clean_up();
			main_wind_draw();
			return(TRUE);
						
			// view menu
					
		case model_menu_item_Front:
			state.model.ang.x=0;
			state.model.ang.y=180;
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_Left:
			state.model.ang.x=0;
			state.model.ang.y=270;
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_Right:
			state.model.ang.x=0;
			state.model.ang.y=90;
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_Back:
			state.model.ang.x=0;
			state.model.ang.y=0;
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_Top:
			state.model.ang.x=270;
			state.model.ang.y=0;
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_Bottom:
			state.model.ang.x=90;
			state.model.ang.y=0;
			main_wind_draw();
			return(TRUE);
						
			// mesh menu
			
		case model_menu_item_NewMesh:
			model_piece_add_mesh();
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_DuplicateMesh:
			if (state.model.cur_mesh_idx==-1) return(TRUE);
			model_piece_duplicate_mesh(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_ImportOBJ:
			model_play(model_play_mode_stop);
			model_file_import_mesh_obj(FALSE);
			return(TRUE);
			
		case model_menu_item_ReplaceOBJ:
			model_play(model_play_mode_stop);
			model_file_import_mesh_obj(TRUE);
			return(TRUE);
			
		case model_menu_item_InsertXML:
			model_play(model_play_mode_stop);
			model_file_insert_mesh_dim3_model();
			return(TRUE);
			
		case model_menu_item_Scale:
			model_setup_scale(&scale);
			if (!dialog_scale_run(&scale)) return(TRUE);
			model_scale(&model,state.model.cur_mesh_idx,&scale);
            main_wind_draw();
			return(TRUE);
			
		case model_menu_item_FlipX:
			model_flip(&model,state.model.cur_mesh_idx,TRUE,FALSE,FALSE);
            main_wind_draw();
			return(TRUE);

		case model_menu_item_FlipY:
			model_flip(&model,state.model.cur_mesh_idx,FALSE,TRUE,FALSE);
            main_wind_draw();
			return(TRUE);
			
		case model_menu_item_FlipZ:
			model_flip(&model,state.model.cur_mesh_idx,FALSE,FALSE,TRUE);
            main_wind_draw();
			return(TRUE);
			
		case model_menu_item_FlipU:
			model_flip_uv(&model,state.model.cur_mesh_idx,TRUE,FALSE);
            main_wind_draw();
			return(TRUE);
			
		case model_menu_item_FlipV:
			model_flip_uv(&model,state.model.cur_mesh_idx,FALSE,TRUE);
            main_wind_draw();
			return(TRUE);
			
		case model_menu_item_SwapXZ:
			model_swap_xz(&model,state.model.cur_mesh_idx);
            main_wind_draw();
			return(TRUE);
			
		case model_menu_item_SwapYZ:
			model_swap_yz(&model,state.model.cur_mesh_idx);
            main_wind_draw();
			return(TRUE);
            
		case model_menu_item_CenterXZ:
			model_center_xz(&model,state.model.cur_mesh_idx);
            main_wind_draw();
			return(TRUE);
            
		case model_menu_item_FloorY:
			model_floor(&model,state.model.cur_mesh_idx);
            main_wind_draw();
			return(TRUE);

		case model_menu_item_MeshTessellate:
			model_polygon_tessellate(state.model.cur_mesh_idx,FALSE);
			main_wind_draw();
			return(TRUE);
			
			// vertex menu
			
		case model_menu_item_VertexSelectAll:
			state.model.select_mode=select_mode_vertex;
			model_vertex_mask_set_sel_all(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_VertexSelectAttached:
			state.model.select_mode=select_mode_vertex;
			model_vertex_mask_set_sel_has_bone(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_VertexSelectNotAttached:
			state.model.select_mode=select_mode_vertex;
			model_vertex_mask_set_sel_no_bone(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_VertexShowAll:
			model_vertex_mask_hide_show_all_vertexes(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_VertexHideSelected:
			model_vertex_mask_hide_set_sel_vertexes(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_VertexHideNonSelected:
			model_vertex_mask_hide_set_non_sel_vertexes(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_VertexInvertNormals:
			model_vertex_invert_normals(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_VertexSetNormals:
			model_vertex_set_normals(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);

		case model_menu_item_VertexSetNormalsOut:
			model_vertex_set_normals_in_out(state.model.cur_mesh_idx,TRUE);
			main_wind_draw();
			return(TRUE);

		case model_menu_item_VertexSetNormalsIn:
			model_vertex_set_normals_in_out(state.model.cur_mesh_idx,FALSE);
			main_wind_draw();
			return(TRUE);
		
		case model_menu_item_VertexClearBones:
			state.model.select_mode=select_mode_vertex;
			model_vertex_clear_bone_attachments_sel_vertexes(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_VertexAutoBones:
			state.model.select_mode=select_mode_vertex;
			model_vertex_auto_bone_attachments(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
				
		case model_menu_item_VertexDelete:
			state.model.select_mode=select_mode_vertex;
			model_vertex_delete_sel_vertex(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_VertexPolygonDelete:
			if (state.model.select_mode!=select_mode_polygon) return(TRUE);
			model_vertex_delete_sel_poly(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_VertexCollapseSelected:
			if (state.model.select_mode==select_mode_mesh) return(TRUE);
			model_vertex_collapse_selected(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);

		case model_menu_item_VertexCollapseSimilar:
			model_vertex_collapse_similar(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);

		case model_menu_item_VertexMakeQuad:
			model_polygon_make_quad(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);

		case model_menu_item_VertexTessellatePoly:
			model_polygon_tessellate(state.model.cur_mesh_idx,TRUE);
			main_wind_draw();
			return(TRUE);
			
			// bone menu
			
		case model_menu_item_NewBone:
			model_piece_add_bone();
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_SetBone:
			if (!dialog_set_vertex_bone_run(&major_bone_idx,&minor_bone_idx,&bone_factor)) return(TRUE);
			model_vertex_mask_set_sel_vertex_to_bone(state.model.cur_mesh_idx,major_bone_idx,minor_bone_idx,bone_factor);
			main_wind_draw();
			return(TRUE);

		case model_menu_item_GoToParentBone:
			if (state.model.cur_bone_idx==-1) return(TRUE);
			
			parent_idx=model.bones[state.model.cur_bone_idx].parent_idx;
			if (parent_idx!=-1) state.model.cur_bone_idx=parent_idx;

			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_DuplicateBoneAttach:
			model_bone_attach_duplicate(state.model.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
			// pose menu
			
		case model_menu_item_NewPose:
			model_piece_add_pose();
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_DupPose:
			if (state.model.cur_pose_idx==-1) return(TRUE);
			model_piece_duplicate_pose(state.model.cur_pose_idx);
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_ClearPose:
			if (state.model.cur_pose_idx==-1) return(TRUE);

			model_pose_clear(&model,state.model.cur_pose_idx);
			return(TRUE);
			
		case model_menu_item_PreviousPose:
			if (state.model.cur_pose_idx==-1) return(TRUE);
			state.model.cur_pose_idx=model_shift_pose_index(state.model.cur_pose_idx,-1);
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_NextPose:
			if (state.model.cur_pose_idx==-1) return(TRUE);
			state.model.cur_pose_idx=model_shift_pose_index(state.model.cur_pose_idx,1);
			main_wind_draw();
			return(TRUE);
			
			// animation menu
			
		case model_menu_item_NewAnimate:
			model_piece_add_animate();
			main_wind_draw();
			return(TRUE);
			
		case model_menu_item_DupAnimate:
			if (state.model.cur_animate_idx==-1) return(TRUE);
			model_piece_duplicate_animate(state.model.cur_animate_idx);
			main_wind_draw();
			return(TRUE);
            
		case model_menu_item_ResetTimeAnimate:
			if (state.model.cur_animate_idx==-1) return(TRUE);
			model_play(model_play_mode_stop);

			dialog_animation_scale_time_run(state.model.cur_animate_idx);
			return(TRUE);
			
		case model_menu_item_PlayAnimate:
			model_play(model_play_mode_normal);
			return(TRUE);
			
		case model_menu_item_PlayBlendAnimate:
			if (dialog_play_blend_animation_run()) {
				model_play(model_play_mode_blend);
			}
			return(TRUE);
						
	}

	return(FALSE);
}

