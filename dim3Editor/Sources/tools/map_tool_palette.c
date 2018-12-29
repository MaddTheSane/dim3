/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Tool Palette

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

#define map_tool_count			38

extern map_type					map;
extern app_state_type			state;
extern app_pref_type			pref;
extern file_path_setup_type		file_path_setup;

tool_palette_type				map_tool_palette;

tool_palette_tool_type			map_tool_palette_setup[map_tool_count]=
									{
										{"Map/Tool Move Points",FALSE,"Move Vertexes and Meshes Freely"},
										{"Map/Tool Move Points Together",FALSE,"Move Equal Vertexes Together"},
										{"Map/Tool Snap Points",FALSE,"Snap Vertexes and Meshes"},
										{"Map/Tool Free Look",TRUE,"Free Look Mode"},
										{"Map/Tool Toggle Mode",FALSE,"Multi-Select Mode"},
										{"Map/Tool Edit Mesh",TRUE,"Select Meshes"},
										{"Map/Tool Edit Polygons",FALSE,"Select Polygons"},
										{"Map/Tool Edit Vertexes",FALSE,"Select Vertexes"},
										{"Map/Tool Combine Meshes",TRUE,"Combine Meshes"},
										{"Map/Tool Split Mesh",FALSE,"Split Mesh"},
										{"Map/Tool Tesselate Mesh",FALSE,"Tesselate Mesh"},
										{"Map/Tool No Grid",TRUE,"No Grid"},
										{"Map/Tool Small Grid",FALSE,"Small Grid"},
										{"Map/Tool Large Grid",FALSE,"Large Grid"},
										{"Map/Tool Auto-Texture Mesh",TRUE,"Auto-Texture Mesh"},
										{"Map/Tool Rotate Mode",TRUE,"Rotate Mode"},
										{"Map/Tool Move Mode",FALSE,"Move Mode"},
										{"Map/Tool Node Select",TRUE,"Click Node To Select"},
										{"Map/Tool Node Duplicate",FALSE,"Click Node To Duplicate, Drag and Auto-Link"},
										{"Map/Tool Node Link",FALSE,"Click Node To Add Link From Selected Node"},
										{"Map/Tool Node Remove Link",FALSE,"Click Node To Remove Link From Selected Node"},
										{"Map/Tool Normals",TRUE,"Show Normals"},
										{"Map/Tool Cull",FALSE,"Show Obscured (Green) and Culled (Gray) Polygons"},
										{"Map/Tool Edit Map Script",TRUE,"Edit Map Script"},
										{"Map/Tool Run Map",FALSE,"Run Map In Engine"},
										{"",FALSE,""},
										{"Map/Tool Spot",FALSE,"Add Spot"},
										{"Map/Tool Light",FALSE,"Add Light"},
										{"Map/Tool Sound",FALSE,"Add Sound"},
										{"Map/Tool Particle",FALSE,"Add Particle"},
										{"Map/Tool Scenery",FALSE,"Add Scenery"},
										{"Map/Tool Node",FALSE,"Add Node"},
										{"Map/Tool Mesh",FALSE,"Add Imported Mesh"},
										{"Map/Tool Mesh UV",FALSE,"Replace UVs on an Existing Mesh"},
										{"Map/Tool Height Map",FALSE,"Add Imported Height Map"},
										{"Map/Tool Grid",FALSE,"Add Grid"},
										{"Map/Tool Polygon",FALSE,"Add Polygon"},
										{"Map/Tool Liquid",FALSE,"Add Liquid"},
									};

/* =======================================================

      Map Tool Palette Initialize/Shutdown
      
======================================================= */

void map_tool_palette_initialize(void)
{
	tool_palette_initialize(&map_tool_palette,map_tool_count,map_tool_palette_setup);
}

void map_tool_palette_shutdown(void)
{
	tool_palette_shutdown(&map_tool_palette);
}

/* =======================================================

      Map Tool Palette State
      
======================================================= */

void map_tool_palette_set_state(void)
{
	bool				disabled;
	editor_view_type	*view;

	view=view_get_current_view();
	disabled=(!state.map.map_open)||(state.map.auto_generate_on);

	tool_palette_set_state(&map_tool_palette,0,(state.map.vertex_mode==vertex_mode_none),disabled);
	tool_palette_set_state(&map_tool_palette,1,(state.map.vertex_mode==vertex_mode_lock),disabled);
	tool_palette_set_state(&map_tool_palette,2,(state.map.vertex_mode==vertex_mode_snap),disabled);
	tool_palette_set_state(&map_tool_palette,3,(state.map.free_look),disabled);
	tool_palette_set_state(&map_tool_palette,4,(state.map.select_add),disabled);
	tool_palette_set_state(&map_tool_palette,5,(state.map.select_mode==select_mode_mesh),disabled);
	tool_palette_set_state(&map_tool_palette,6,(state.map.select_mode==select_mode_polygon),disabled);
	tool_palette_set_state(&map_tool_palette,7,(state.map.select_mode==select_mode_vertex),disabled);
	tool_palette_set_state(&map_tool_palette,8,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,9,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,10,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,11,(state.map.grid_mode==grid_mode_none),disabled);
	tool_palette_set_state(&map_tool_palette,12,(state.map.grid_mode==grid_mode_small),disabled);
	tool_palette_set_state(&map_tool_palette,13,(state.map.grid_mode==grid_mode_large),disabled);
	tool_palette_set_state(&map_tool_palette,14,(state.map.auto_texture),disabled);
	tool_palette_set_state(&map_tool_palette,15,(state.map.handle_mode==handle_mode_rotate),disabled);
	tool_palette_set_state(&map_tool_palette,16,(state.map.handle_mode==handle_mode_move),disabled);
	tool_palette_set_state(&map_tool_palette,17,(state.map.node_mode==node_mode_select),disabled);
	tool_palette_set_state(&map_tool_palette,18,(state.map.node_mode==node_mode_duplicate),disabled);
	tool_palette_set_state(&map_tool_palette,19,(state.map.node_mode==node_mode_link),disabled);
	tool_palette_set_state(&map_tool_palette,20,(state.map.node_mode==node_mode_remove_link),disabled);
	tool_palette_set_state(&map_tool_palette,21,(state.map.show_normals),disabled);
	tool_palette_set_state(&map_tool_palette,22,(view->cull),disabled);

	tool_palette_set_state(&map_tool_palette,23,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,24,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,25,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,26,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,27,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,28,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,29,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,30,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,31,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,32,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,33,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,34,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,35,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,36,FALSE,disabled);
	tool_palette_set_state(&map_tool_palette,37,FALSE,disabled);
}

/* =======================================================

      Map Tool Palette Clicking
      
======================================================= */

void map_tool_palette_click(int tool_idx)
{
	editor_view_type		*view;

		// nothing to do if map closed

	if (!state.map.map_open) return;

		// if in texture view, any tool click
		// leaves it

	if (state.map.texture_edit_idx!=-1) texture_edit_switch(-1);

		// run the click

	switch (tool_idx) {
	
			// vertex mode
			
		case 0:
			state.map.vertex_mode=vertex_mode_none;
			break;
			
		case 1:
			state.map.vertex_mode=vertex_mode_lock;
			break;
			
		case 2:
			state.map.vertex_mode=vertex_mode_snap;
			break;
			
			// free look and selection toggle
			
		case 3:
			state.map.free_look=!state.map.free_look;
			break;

		case 4:
			state.map.select_add=!state.map.select_add;
			break;
			
			// select mode buttons
			
		case 5:
			state.map.select_mode=select_mode_mesh;
			break;
			
		case 6:
			state.map.select_mode=select_mode_polygon;
			break;
			
		case 7:
			state.map.select_mode=select_mode_vertex;
			break;
			
			// mesh polygons
			
		case 8:
			piece_combine_mesh();
			break;
			
		case 9:
			piece_split_mesh();
			break;

		case 10:
			piece_tesselate(TRUE);
			break;
			
			
			// grids
			
		case 11:
			state.map.grid_mode=grid_mode_none;
			break;

		case 12:
			state.map.grid_mode=grid_mode_small;
			break;

		case 13:
			state.map.grid_mode=grid_mode_large;
			break;
			
			// auto-texture
			
		case 14:
			state.map.auto_texture=!state.map.auto_texture;
			break;
			
			// handle modes
			
		case 15:
			state.map.handle_mode=handle_mode_rotate;
			break;
			
		case 16:
			state.map.handle_mode=handle_mode_move;
			break;
			
			// node editing
			
		case 17:
			state.map.show_node=TRUE;
			main_wind_menu_update();
			state.map.node_mode=node_mode_select;
			break;
			
		case 18:
			state.map.show_node=TRUE;
			main_wind_menu_update();
			state.map.node_mode=node_mode_duplicate;
			break;
			
		case 19:
			state.map.show_node=TRUE;
			main_wind_menu_update();
			state.map.node_mode=node_mode_link;
			break;
			
		case 20:
			state.map.show_node=TRUE;
			main_wind_menu_update();
			state.map.node_mode=node_mode_remove_link;
			break;
			
			// normals
			
		case 21:
			state.map.show_normals=!state.map.show_normals;
			break;
			
		case 22:
			view=view_get_current_view();
			view_cull(!view->cull);
			break;
			
			// script and run buttons
			
		case 23:
			launch_map_script_editor();
			break;
			
		case 24:
			launch_engine();
			break;
			
			// pieces create
			
		case 26:
			piece_create_spot();
			break;
			
		case 27:
			piece_create_light();
			break;
			
		case 28:
			piece_create_sound();
			break;
			
		case 29:
			piece_create_particle();
			break;
			
		case 30:
			piece_create_scenery();
			break;
			
		case 31:
			piece_create_node();
			break;
			
		case 32:
			piece_add_obj_mesh();
			break;
			
		case 33:
			piece_add_obj_mesh_uv();
			break;
			
		case 34:
			piece_add_height_map_mesh();
			break;
			
		case 35:
			piece_add_grid_mesh();
			break;
			
		case 36:
			piece_add_polygon_mesh();
			break;
			
		case 37:
			piece_create_liquid();
			break;
			
	}
	
	main_wind_menu_update();
	map_texture_palette_reset();
	
	main_wind_draw();
}

/* =======================================================

      Map Tool Palette Default State
      
======================================================= */

void map_tool_default(void)
{
	state.map.free_look=pref.free_look;
	state.map.select_add=FALSE;
	
    state.map.vertex_mode=vertex_mode_none;
	state.map.select_mode=select_mode_mesh;
	state.map.grid_mode=grid_mode_small;
	state.map.node_mode=node_mode_select;
	state.map.handle_mode=handle_mode_rotate;
	
	state.map.auto_texture=pref.auto_texture;
	
	state.map.show_liquid=TRUE;
	state.map.show_object=TRUE;
	state.map.show_lightsoundparticle=TRUE;
	state.map.show_node=FALSE;
	
	state.map.show_normals=FALSE;
}
