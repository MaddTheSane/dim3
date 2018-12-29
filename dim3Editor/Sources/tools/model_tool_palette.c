/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Model Tool Palette

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

#define model_tool_count		20

extern map_type					map;
extern app_state_type			state;
extern app_pref_type			pref;
extern file_path_setup_type		file_path_setup;

tool_palette_type				model_tool_palette;

tool_palette_tool_type			model_tool_palette_setup[model_tool_count]=
									{
										{"Model/Tool Textured",FALSE,"Show Textured Model"},
										{"Model/Tool Mesh",FALSE,"Show Model Mesh"},
										{"Model/Tool Bones",FALSE,"Show Model Bones"},
										{"Model/Tool Hit Boxes",FALSE,"Show Model Hit Boxes"},
										{"Model/Tool Edit Mesh",TRUE,"Select Whole Mesh"},
										{"Model/Tool Edit Polygons",FALSE,"Select Polygons"},
										{"Model/Tool Edit Vertexes",FALSE,"Select Vertexes"},
										{"Model/Tool Boxes",TRUE,"Show View Box"},
										{"Model/Tool Normals",FALSE,"Show Normals"},
										{"Model/Tool Show Bone Names",FALSE,"Show Bone Names"},
										{"Model/Tool Select Vertex With Bone",TRUE,"Select Attached Vertexes When Selecting Bones"},
										{"Model/Tool Rotate Mode",TRUE,"Bone Rotate Mode"},
										{"Model/Tool Move Mode",FALSE,"Bone Move Mode"},
										{"",FALSE,""},
										{"Model/Tool Play Stop",FALSE,"Stop Playing Animation"},
										{"Model/Tool Play",TRUE,"Play Animation"},
										{"Model/Tool Play Blend",FALSE,"Play Multiple Blended Animations"},
										{"Model/Tool Play Slow",FALSE,"Play Animation Slowly"},
										{"Model/Tool Play Previous",TRUE,"Previous Pose in Animation"},
										{"Model/Tool Play Next",FALSE,"Next Pose in Animation"},
									};

/* =======================================================

      Model Tool Palette Initialize/Shutdown
      
======================================================= */

void model_tool_palette_initialize(void)
{
	tool_palette_initialize(&model_tool_palette,model_tool_count,model_tool_palette_setup);
}

void model_tool_palette_shutdown(void)
{
	tool_palette_shutdown(&model_tool_palette);
}

/* =======================================================

      Model Tool Palette State
      
======================================================= */

void model_tool_palette_set_state(void)
{
	bool			disabled;

	disabled=!state.model.model_open;

	tool_palette_set_state(&model_tool_palette,0,state.model.texture,disabled);
	tool_palette_set_state(&model_tool_palette,1,state.model.mesh,disabled);
	tool_palette_set_state(&model_tool_palette,2,state.model.bone,disabled);
	tool_palette_set_state(&model_tool_palette,3,state.model.hit_box,disabled);
	tool_palette_set_state(&model_tool_palette,4,(state.model.select_mode==select_mode_mesh),disabled);
	tool_palette_set_state(&model_tool_palette,5,(state.model.select_mode==select_mode_polygon),disabled);
	tool_palette_set_state(&model_tool_palette,6,(state.model.select_mode==select_mode_vertex),disabled);
	tool_palette_set_state(&model_tool_palette,7,state.model.view_box,disabled);
	tool_palette_set_state(&model_tool_palette,8,state.model.normal,disabled);
	tool_palette_set_state(&model_tool_palette,9,state.model.bone_names,disabled);
	tool_palette_set_state(&model_tool_palette,10,state.model.sel_vertex_with_bone,disabled);
	tool_palette_set_state(&model_tool_palette,11,(state.model.drag_bone_mode==handle_mode_rotate),disabled);
	tool_palette_set_state(&model_tool_palette,12,(state.model.drag_bone_mode==handle_mode_move),disabled);
	tool_palette_set_state(&model_tool_palette,13,FALSE,disabled);
	tool_palette_set_state(&model_tool_palette,14,(state.model.play_mode==model_play_mode_stop),disabled);
	tool_palette_set_state(&model_tool_palette,15,(state.model.play_mode==model_play_mode_normal),disabled);
	tool_palette_set_state(&model_tool_palette,16,(state.model.play_mode==model_play_mode_blend),disabled);
	tool_palette_set_state(&model_tool_palette,17,(state.model.play_mode==model_play_mode_slow),disabled);
	tool_palette_set_state(&model_tool_palette,18,(state.model.play_mode==model_play_mode_prev),disabled);
	tool_palette_set_state(&model_tool_palette,19,(state.model.play_mode==model_play_mode_next),disabled);
}

/* =======================================================

      Model Tool Palette Clicking
      
======================================================= */

void model_tool_palette_click(int tool_idx)
{
	switch (tool_idx) {
			
			// left side

		case 0:
			state.model.texture=!state.model.texture;
			break;
			
		case 1:
			state.model.mesh=!state.model.mesh;
			break;
			
		case 2:
			state.model.bone=!state.model.bone;
			break;
			
		case 3:
			state.model.hit_box=!state.model.hit_box;
			break;
			
		case 4:
			model_vertex_mask_clear_sel(state.model.cur_mesh_idx);
			state.model.select_mode=select_mode_mesh;
			break;

		case 5:
			model_vertex_mask_clear_sel(state.model.cur_mesh_idx);
			state.model.select_mode=select_mode_polygon;
			break;
			
		case 6:
			model_vertex_mask_clear_sel(state.model.cur_mesh_idx);
			state.model.select_mode=select_mode_vertex;
			break;
			
		case 7:
			state.model.view_box=!state.model.view_box;
			break;
			
		case 8:
			state.model.normal=!state.model.normal;
			break;
			
		case 9:
			state.model.bone_names=!state.model.bone_names;
			break;
			
		case 10:
			state.model.sel_vertex_with_bone=!state.model.sel_vertex_with_bone;
			break;
		
		case 11:
			state.model.drag_bone_mode=handle_mode_rotate;
			break;
			
		case 12:
			state.model.drag_bone_mode=handle_mode_move;
			break;
			
			// right side

		case 14:
			model_play(model_play_mode_stop);
			break;

		case 15:
			model_play(model_play_mode_normal);
			break;

		case 16:
			if (dialog_play_blend_animation_run()) model_play(model_play_mode_blend);
			break;

		case 17:
			model_play(model_play_mode_slow);
			break;

		case 18:
			model_play(model_play_mode_prev);
			break;

		case 19:
			model_play(model_play_mode_next);
			break;

	}
	
	main_wind_draw();
}

/* =======================================================

      Project Tool Palette Default State
      
======================================================= */

void model_tool_default(void)
{
	state.model.texture=TRUE;
	state.model.mesh=FALSE;
	state.model.bone=FALSE;
    state.model.hit_box=FALSE;
	state.model.normal=FALSE;
	state.model.view_box=FALSE;
	state.model.bone_names=TRUE;
	state.model.sel_vertex_with_bone=FALSE;

	state.model.select_mode=select_mode_vertex;
	state.model.drag_bone_mode=handle_mode_rotate;
}
