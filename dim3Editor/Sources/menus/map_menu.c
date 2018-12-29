/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Menus

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

extern map_type					map;
extern app_state_type			state;

extern list_palette_type		map_palette;

os_menu_item_type		map_menu_setup[]=
								{

										// file

									{"File","Save",map_menu_item_FileSave,os_menu_key_cmd,'S'},
								#ifdef D3_OS_WINDOWS
									{"File","",0,os_menu_key_none,0x0},
									{"File","Exit",map_menu_item_FileQuit,os_menu_key_none,0x0},
								#endif

										// edit

									{"Edit","Undo",map_menu_item_EditUndo,os_menu_key_cmd,'Z'},
									{"Edit","",0,os_menu_key_none,0x0},
									{"Edit","Delete",map_menu_item_EditDelete,os_menu_key_none,0x0},
									{"Edit","Duplicate",map_menu_item_EditDuplicate,os_menu_key_cmd,'D'},
									{"Edit","",0,os_menu_key_none,0x0},
									{"Edit","Select More",map_menu_item_EditSelectMore,os_menu_key_cmd,'M'},

										// view

									{"View","Front",map_menu_item_ViewFront,os_menu_key_cmd,'1'},
									{"View","Left",map_menu_item_ViewLeft,os_menu_key_cmd,'2'},
									{"View","Right",map_menu_item_ViewRight,os_menu_key_cmd,'3'},
									{"View","Back",map_menu_item_ViewBack,os_menu_key_cmd,'4'},
									{"View","Top",map_menu_item_ViewTop,os_menu_key_cmd,'5'},
									{"View","Bottom",map_menu_item_ViewBottom,os_menu_key_cmd,'6'},
									{"View","",0,os_menu_key_none,0x0},
									{"View","Perspective",map_menu_item_ViewPerspective,os_menu_key_cmd,'9'},
									{"View","Ortho",map_menu_item_ViewOrtho,os_menu_key_cmd,'0'},
									{"View","",0,os_menu_key_none,0x0},
									{"View","Normal UV Layer",map_menu_item_ViewUVLayer1,os_menu_key_cmd_opt,'1'},
									{"View","Light Map UV Layer",map_menu_item_ViewUVLayer2,os_menu_key_cmd_opt,'2'},
									{"View","",0,os_menu_key_none,0x0},
									{"View","Goto Selection",map_menu_item_ViewGotoSelect,os_menu_key_cmd,'G'},
									{"View","Goto Map Center",map_menu_item_ViewGotoMapCenter,os_menu_key_cmd_shift,'G'},
									{"View","",0,os_menu_key_none,0x0},
									{"View","Clipping",map_menu_item_ViewClip,os_menu_key_cmd_opt,'3'},
									{"View","",0,os_menu_key_none,0x0},
									{"View","Liquids",map_menu_item_ViewShowHideLiquids,os_menu_key_cmd_shift,'1'},
									{"View","Spots, Scenery",map_menu_item_ViewShowHideSpots,os_menu_key_cmd_shift,'2'},
									{"View","Lights, Sounds, Particles",map_menu_item_ViewShowHideLights,os_menu_key_cmd_shift,'3'},
									{"View","Nodes",map_menu_item_ViewShowHideNodes,os_menu_key_cmd_shift,'4'}, \
									{"View","Movements",map_menu_item_ViewShowHideMovements,os_menu_key_cmd_shift,'5'},
									{"View","",0,os_menu_key_none,0x0},
									{"View","Split Panel Horizontal",map_menu_item_ViewSplitHorizontal,os_menu_key_none,0x0},
									{"View","Split Panel Vertical",map_menu_item_ViewSplitVertical,os_menu_key_none,0x0},
									{"View","Remove Panel",map_menu_item_ViewRemoveSplit,os_menu_key_none,0x0},

										// map

									{"Map","Raise Y",map_menu_item_MapRaiseY,os_menu_key_none,0x0},\
									{"Map","Lower Y",map_menu_item_MapLowerY,os_menu_key_none,0x0},
									{"Map","Center",map_menu_item_MapCenter,os_menu_key_none,0x0},
									{"Map","",0,os_menu_key_none,0x0},
									{"Map","Reset UVs",map_menu_item_MapResetUV,os_menu_key_none,0x0},
									{"Map","Recalc Normals",map_menu_item_MapResetNormals,os_menu_key_none,0x0},
									{"Map","Sort Mesh Polygons",map_menu_item_MapSortMeshPolys,os_menu_key_none,0x0},
									{"Map","",0,os_menu_key_none,0x0},
									{"Map","Clear Light Maps",map_menu_item_ClearLightMaps,os_menu_key_none,0x0},
									{"Map","Build Light Maps...",map_menu_item_BuildLightMaps,os_menu_key_none,0x0},
									{"Map","",0,os_menu_key_none,0x0},
									{"Map","Build Liquid Reflection Maps...",map_menu_item_BuildLiquidReflectionMaps,os_menu_key_none,0x0},
									{"Map","",0,os_menu_key_none,0x0},
									{"Map","Auto Generate Textures...",map_menu_item_AutoGenerateTextures,os_menu_key_none,0x0},
									{"Map","Auto Generate Map...",map_menu_item_AutoGenerateMap,os_menu_key_none,0x0},
									{"Map","",0,os_menu_key_none,0x0},
									{"Map","Run...",map_menu_item_Run,os_menu_key_cmd,'R'},

										// mesh

									{"Mesh","Combine",map_menu_item_MeshCombine,os_menu_key_cmd,'P'},
									{"Mesh","Split",map_menu_item_MeshSplit,os_menu_key_cmd_shift,'P'},
									{"Mesh","Tesselate",map_menu_item_MeshTesselate,os_menu_key_none,0x0},
									{"Mesh","Distort",map_menu_item_MeshDistort,os_menu_key_none,0x0},
									{"Mesh","",0,os_menu_key_none,0x0},
									{"Mesh","Scale...",map_menu_item_MeshResize,os_menu_key_none,0x0},
									{"Mesh","Force Bounds to Grid",map_menu_item_MeshForceGrid,os_menu_key_none,0x0},
									{"Mesh","",0,os_menu_key_none,0x0},
									{"Mesh","Flip X",map_menu_item_MeshFlipX,os_menu_key_cmd,'H'},
									{"Mesh","Flip Y",map_menu_item_MeshFlipY,os_menu_key_cmd,'U'},
									{"Mesh","Flip Z",map_menu_item_MeshFlipZ,os_menu_key_cmd,'V'},
									{"Mesh","",0,os_menu_key_none,0x0},
									{"Mesh","Rotate X",map_menu_item_MeshRotateX,os_menu_key_none,0x0},
									{"Mesh","Rotate Y",map_menu_item_MeshRotateY,os_menu_key_cmd_opt,'R'},
									{"Mesh","Rotate Z",map_menu_item_MeshRotateZ,os_menu_key_none,0x0},
									{"Mesh","Free Rotate...",map_menu_item_MeshFreeRotate,os_menu_key_cmd_shift,'R'},
									{"Mesh","",0,os_menu_key_none,0x0},
									{"Mesh","Raise Y",map_menu_item_MeshRaiseY,os_menu_key_cmd_shift,'Y'},
									{"Mesh","Lower Y",map_menu_item_MeshLowerY,os_menu_key_cmd,'Y'},
									{"Mesh","",0,os_menu_key_none,0x0},
									{"Mesh","Select All Polygons",map_menu_item_MeshSelectAllPoly,os_menu_key_none,0x0},
									{"Mesh","",0,os_menu_key_none,0x0},
									{"Mesh","Reset Mesh UVs",map_menu_item_MeshResetUV,os_menu_key_none,0x0},
									{"Mesh","Whole Mesh UVs",map_menu_item_MeshWholeUV,os_menu_key_none,0x0},
									{"Mesh","Single Stamp Mesh UVs",map_menu_item_MeshSingleUV,os_menu_key_none,0x0},
									{"Mesh","",0,os_menu_key_none,0x0},
									{"Mesh","Recalc Normals",map_menu_item_MeshCreateNormals,os_menu_key_none,0x0},
									{"Mesh","Invert Normals",map_menu_item_MeshInvertNormals,os_menu_key_cmd_opt,'I'},
									{"Mesh","Set Normals Out",map_menu_item_MeshSetNormalsOut,os_menu_key_none,0x0},
									{"Mesh","Set Normals In",map_menu_item_MeshSetNormalsIn,os_menu_key_none,0x0},

										// polygon

									{"Polygon","Punch Middle Hole",map_menu_item_PolygonHole,os_menu_key_none,0x0},
									{"Polygon","Tesselate",map_menu_item_PolyTesselate,os_menu_key_none,0x0},
									{"Polygon","",0,os_menu_key_none,0x0},
									{"Polygon","Rotate Polygon UVs",map_menu_item_PolygonRotateUV,os_menu_key_cmd,'T'},
									{"Polygon","Flip Us",map_menu_item_PolygonFlipU,os_menu_key_none,0x0},
									{"Polygon","Flip Vs",map_menu_item_PolygonFlipV,os_menu_key_none,0x0},
									{"Polygon","",0,os_menu_key_none,0x0},
									{"Polygon","Recalc Normals",map_menu_item_PolygonRecalcNormal,os_menu_key_cmd_shift,'N'},
									{"Polygon","Invert Normals",map_menu_item_PolygonInvertNormal,os_menu_key_cmd,'I'},
									{"Polygon","",0,os_menu_key_none,0x0},
									{"Polygon","Reset Polygon UVs",map_menu_item_PolygonResetUV,os_menu_key_none,0x0},
									{"Polygon","Whole Polygon UVs",map_menu_item_PolygonWholeUV,os_menu_key_none,0x0},
									{"Polygon","Single Stamp Polygon UVs",map_menu_item_PolygonSingleUV,os_menu_key_none,0x0},

									{"","",-1,os_menu_key_none,0x0},
								};

/* =======================================================

      Create Menu
      
======================================================= */

void map_menu_create(void)
{
	os_menu_create(map_menu_setup);
}

void map_menu_dispose(void)
{
	os_menu_dispose();
}

/* =======================================================

      Menu Enable/Disable
      
======================================================= */

void map_menu_update(void)
{
	editor_view_type			*view;

	if ((!state.map.map_open) || (state.map.auto_generate_on)) {
		os_menu_enable_item(map_menu_file,1,FALSE);
	
		os_menu_enable_item(map_menu_edit,0,FALSE);
		os_menu_enable_item(map_menu_view,0,FALSE);
		os_menu_enable_item(map_menu_map,0,FALSE);
		os_menu_enable_item(map_menu_mesh,0,FALSE);
		os_menu_enable_item(map_menu_polygon,0,FALSE);
	}
	else {
	
			// file menu
			
		os_menu_enable_item(map_menu_file,1,TRUE);
	
			// other menus
			
		os_menu_enable_item(map_menu_edit,0,TRUE);
		os_menu_enable_item(map_menu_view,0,TRUE);
		os_menu_enable_item(map_menu_map,0,TRUE);

			// view menu

		view=view_get_current_view();

		os_menu_check_item(map_menu_view,8,(!view->ortho));
		os_menu_check_item(map_menu_view,9,view->ortho);

		os_menu_check_item(map_menu_view,11,(view->uv_layer==uv_layer_normal));
		os_menu_check_item(map_menu_view,12,(view->uv_layer==uv_layer_light_map));

		os_menu_check_item(map_menu_view,16,(view->clip));

		os_menu_check_item(map_menu_view,19,state.map.show_liquid);
		os_menu_check_item(map_menu_view,20,state.map.show_object);
		os_menu_check_item(map_menu_view,21,state.map.show_lightsoundparticle);
		os_menu_check_item(map_menu_view,22,state.map.show_node);
		os_menu_check_item(map_menu_view,23,state.map.show_movements);

		if (map.editor_views.count<max_editor_view) {
			os_menu_enable_item(map_menu_view,25,TRUE);
			os_menu_enable_item(map_menu_view,26,TRUE);
		}
		else {
			os_menu_enable_item(map_menu_view,25,FALSE);
			os_menu_enable_item(map_menu_view,26,FALSE);
		}

		if (map.editor_views.count>1) {
			os_menu_enable_item(map_menu_view,27,TRUE);
		}
		else {
			os_menu_enable_item(map_menu_view,27,FALSE);
		}
       
            // pieces menu
        
		if (select_has_type(item_map_mesh)) {
			os_menu_enable_item(map_menu_mesh,0,TRUE);
			if (state.map.select_mode==select_mode_polygon) {
				os_menu_enable_item(map_menu_polygon,0,TRUE);
			}
			else {
				os_menu_enable_item(map_menu_polygon,0,FALSE);
			}
		}
		else {
			os_menu_enable_item(map_menu_mesh,0,FALSE);
			os_menu_enable_item(map_menu_polygon,0,FALSE);
		}
	}
}

/* =======================================================

      Menu Events
      
======================================================= */

bool map_menu_event_run(int cmd)
{
	d3ang			rot_ang;

	switch (cmd) {
            
			// file menu
			
		case map_menu_item_FileSave:
			file_save_map();
			return(TRUE);

		case map_menu_item_FileQuit:
			if (file_close_map()) {
				os_application_quit();
			}
			return(TRUE);
			
			// edit menu
			
		case map_menu_item_EditUndo:
			map_undo_pull();
			return(TRUE);
			
		case map_menu_item_EditDelete:
			piece_delete();
			return(TRUE);
			
		case map_menu_item_EditDuplicate:
			piece_duplicate();
			return(TRUE);
			
		case map_menu_item_EditSelectMore:
			piece_select_more();
			return(TRUE);
			
			// view menu
			
		case map_menu_item_ViewFront:
			view_face_front();
			main_wind_draw();
			break;
			
		case map_menu_item_ViewLeft:
			view_face_left();
			main_wind_draw();
			break;
			
		case map_menu_item_ViewRight:
			view_face_right();
			main_wind_draw();
			break;
			
		case map_menu_item_ViewBack:
			view_face_back();
			main_wind_draw();
			break;
			
		case map_menu_item_ViewTop:
			view_face_top();
			main_wind_draw();
			break;
			
		case map_menu_item_ViewBottom:
			view_face_bottom();
			main_wind_draw();
			break;

		case map_menu_item_ViewPerspective:
			view_perspective_ortho(FALSE);
			main_wind_menu_update();
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_ViewOrtho:
			view_perspective_ortho(TRUE);
			main_wind_menu_update();
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_ViewUVLayer1:
			view_set_uv_layer(uv_layer_normal);
			main_wind_menu_update();
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_ViewUVLayer2:
			view_set_uv_layer(uv_layer_light_map);
			main_wind_menu_update();
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_ViewGotoSelect:
			map_view_goto_select();
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_ViewGotoMapCenter:
			map_view_goto_map_center(view_get_current_view());
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_ViewClip:
			view_flip_clip();
			main_wind_menu_update();
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_ViewShowHideLiquids:
			select_remove_type(item_map_liquid);
			state.map.show_liquid=!state.map.show_liquid;
			main_wind_menu_update();
			main_wind_draw();
			break;

		case map_menu_item_ViewShowHideSpots:
			select_remove_type(item_map_spot);
			select_remove_type(item_map_scenery);
			state.map.show_object=!state.map.show_object;
			main_wind_menu_update();
			main_wind_draw();
			break;
			
		case map_menu_item_ViewShowHideLights:
			select_remove_type(item_map_light);
			select_remove_type(item_map_sound);
			select_remove_type(item_map_particle);
			state.map.show_lightsoundparticle=!state.map.show_lightsoundparticle;
			main_wind_menu_update();
			main_wind_draw();
			break;
			
		case map_menu_item_ViewShowHideNodes:
			select_remove_type(item_map_node);
			state.map.show_node=!state.map.show_node;
			main_wind_menu_update();
			main_wind_draw();
			break;
			
		case map_menu_item_ViewShowHideMovements:
			state.map.show_movements=!state.map.show_movements;
			main_wind_menu_update();
			main_wind_draw();
			break;

		case map_menu_item_ViewSplitHorizontal:
			view_split_horizontal();
			main_wind_menu_update();
			main_wind_draw();
			break;
			
		case map_menu_item_ViewSplitVertical:
			view_split_vertical();
			main_wind_menu_update();
			main_wind_draw();
			break;
			
		case map_menu_item_ViewRemoveSplit:
			view_split_remove();
			main_wind_menu_update();
			main_wind_draw();
			break;
			
			// map menu

		case map_menu_item_MapRaiseY:
			map_mesh_move_all(0,-2000,0);
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_MapLowerY:
			map_mesh_move_all(0,2000,0);
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_MapCenter:
			map_mesh_center_all();
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_MapResetUV:
			map_mesh_reset_uv_all();
			main_wind_draw();
			return(TRUE);

		case map_menu_item_MapResetNormals:
			os_set_wait_cursor();
			map_recalc_normals(&map,FALSE);
			os_set_arrow_cursor();
			main_wind_draw();
			return(TRUE);

		case map_menu_item_MapSortMeshPolys:
			os_set_wait_cursor();
			map_mesh_sort_polys(&map);
			os_set_arrow_cursor();
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_ClearLightMaps:
			light_maps_clear();
			view_set_uv_layer(uv_layer_normal);
			main_wind_menu_update();
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_BuildLightMaps:
			if (light_maps_create()) {
				view_set_uv_layer(uv_layer_light_map);
				main_wind_menu_update();
				main_wind_draw();
			}
			return(TRUE);

		case map_menu_item_BuildLiquidReflectionMaps:
			liquid_reflection_maps_create();
			return(TRUE);

		case map_menu_item_AutoGenerateTextures:
			view_map_auto_generate_textures();
			return(TRUE);

		case map_menu_item_AutoGenerateMap:
			view_map_auto_generate_start();
			return(TRUE);

		case map_menu_item_Run:
			launch_engine();
			return(TRUE);
			            
			// mesh menu

		case map_menu_item_MeshCombine:
			piece_combine_mesh();
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_MeshSplit:
			piece_split_mesh();
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_MeshTesselate:
			piece_tesselate(TRUE);
			return(TRUE);

		case map_menu_item_MeshDistort:
			piece_distort();
			return(TRUE);
			
		case map_menu_item_MeshResize:
			piece_resize();
			return(TRUE);

		case map_menu_item_MeshForceGrid:
			piece_force_grid();
			return(TRUE);

		case map_menu_item_MeshFlipX:
			piece_flip(TRUE,FALSE,TRUE);
			return(TRUE);
			
		case map_menu_item_MeshFlipY:
			piece_flip(FALSE,TRUE,FALSE);
			return(TRUE);
			
		case map_menu_item_MeshFlipZ:
			piece_flip(FALSE,FALSE,TRUE);
			return(TRUE);
			
		case map_menu_item_MeshRotateX:
			rot_ang.x=90.0f;
			rot_ang.y=rot_ang.z=0.0f;
			piece_rotate(&rot_ang);
			return(TRUE);
			
		case map_menu_item_MeshRotateY:
			rot_ang.y=90.0f;
			rot_ang.x=rot_ang.z=0.0f;
			piece_rotate(&rot_ang);
			return(TRUE);
			
		case map_menu_item_MeshRotateZ:
			rot_ang.z=90.0f;
			rot_ang.x=rot_ang.y=0.0f;
			piece_rotate(&rot_ang);
			return(TRUE);

		case map_menu_item_MeshFreeRotate:
			piece_free_rotate();
			return(TRUE);

		case map_menu_item_MeshRaiseY:
			piece_move(0,-100,0);
			main_wind_draw();
			return(TRUE);
            
		case map_menu_item_MeshLowerY:
			piece_move(0,100,0);
            main_wind_draw();
 			return(TRUE);

		case map_menu_item_MeshSelectAllPoly:
			piece_mesh_select_all_poly();
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_MeshResetUV:
			piece_reset_uvs(FALSE);
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_MeshWholeUV:
			piece_whole_uvs(FALSE);
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_MeshSingleUV:
			piece_single_uvs(FALSE);
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_MeshCreateNormals:
			piece_mesh_recalc_normals(FALSE);
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_MeshInvertNormals:
			piece_mesh_invert_normals(FALSE);
			main_wind_draw();
			return(TRUE);

		case map_menu_item_MeshSetNormalsOut:
			piece_mesh_set_normals_in_out(TRUE);
			main_wind_draw();
			return(TRUE);

		case map_menu_item_MeshSetNormalsIn:
			piece_mesh_set_normals_in_out(FALSE);
			main_wind_draw();
			return(TRUE);
			
			// polygon menu
			
		case map_menu_item_PolygonHole:
			piece_poly_hole();
			return(TRUE);
			
		case map_menu_item_PolyTesselate:
			piece_tesselate(FALSE);
			return(TRUE);
			
		case map_menu_item_PolygonRotateUV:
			piece_rotate_uvs();
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_PolygonFlipU:
			piece_flip_uvs(TRUE,FALSE);
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_PolygonFlipV:
			piece_flip_uvs(FALSE,TRUE);
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_PolygonRecalcNormal:
			piece_mesh_recalc_normals(TRUE);
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_PolygonInvertNormal:
			piece_mesh_invert_normals(TRUE);
			main_wind_draw();
			return(TRUE);

		case map_menu_item_PolygonResetUV:
			piece_reset_uvs(TRUE);
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_PolygonWholeUV:
			piece_whole_uvs(TRUE);
			main_wind_draw();
			return(TRUE);
			
		case map_menu_item_PolygonSingleUV:
			piece_single_uvs(TRUE);
			main_wind_draw();
			return(TRUE);

	}
	
	return(FALSE);
}

