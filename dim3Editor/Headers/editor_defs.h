/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Defines for Editor

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

//
// app modes
//

#define app_mode_project						0
#define app_mode_map							1
#define app_mode_model							2

//
// view selection
//

#define view_selection_size						10

//
// menus
//

//
// project menus
//

#define project_menu_file						0

#define project_menu_item_Save					5100
#define project_menu_item_Quit					5101

//
// map menus
//

#define map_menu_file							0
#define map_menu_edit							1
#define map_menu_view							2
#define map_menu_map							3
#define map_menu_mesh							4
#define map_menu_polygon						5

#define map_menu_item_FileSave					5100
#define map_menu_item_FileQuit					5101

#define map_menu_item_EditUndo					5200
#define map_menu_item_EditDelete				5201
#define map_menu_item_EditDuplicate				5202
#define map_menu_item_EditSelectMore			5203

#define map_menu_item_ViewFront					5300
#define map_menu_item_ViewLeft					5301
#define map_menu_item_ViewRight					5302
#define map_menu_item_ViewBack					5303
#define map_menu_item_ViewTop					5304
#define map_menu_item_ViewBottom				5305
#define map_menu_item_ViewPerspective			5306
#define map_menu_item_ViewOrtho					5307
#define map_menu_item_ViewUVLayer1				5308
#define map_menu_item_ViewUVLayer2				5309
#define map_menu_item_ViewGotoSelect			5310
#define map_menu_item_ViewGotoMapCenter			5311
#define map_menu_item_ViewClip					5312
#define map_menu_item_ViewShowHideLiquids		5313
#define map_menu_item_ViewShowHideSpots			5314
#define map_menu_item_ViewShowHideLights		5315
#define map_menu_item_ViewShowHideNodes			5316
#define map_menu_item_ViewShowHideMovements		5317
#define map_menu_item_ViewSplitHorizontal		5318
#define map_menu_item_ViewSplitVertical			5319
#define map_menu_item_ViewRemoveSplit			5320

#define map_menu_item_MapRaiseY					5400
#define map_menu_item_MapLowerY					5401
#define map_menu_item_MapCenter					5402
#define map_menu_item_MapResetUV				5403
#define map_menu_item_MapResetNormals			5404
#define map_menu_item_MapSortMeshPolys			5405
#define map_menu_item_ClearLightMaps			5406
#define map_menu_item_BuildLightMaps			5407
#define map_menu_item_BuildLiquidReflectionMaps	5408
#define map_menu_item_AutoGenerateTextures		5409
#define map_menu_item_AutoGenerateMap			5410
#define map_menu_item_Run						5411

#define map_menu_item_MeshCombine				5500
#define map_menu_item_MeshSplit					5501
#define map_menu_item_MeshTesselate				5502
#define map_menu_item_MeshDistort				5503
#define map_menu_item_MeshResize				5504
#define map_menu_item_MeshForceGrid				5506
#define map_menu_item_MeshFlipX					5508
#define map_menu_item_MeshFlipY					5509
#define map_menu_item_MeshFlipZ					5510
#define map_menu_item_MeshRotateX				5511
#define map_menu_item_MeshRotateY				5512
#define map_menu_item_MeshRotateZ				5513
#define map_menu_item_MeshFreeRotate			5514
#define map_menu_item_MeshRaiseY				5515
#define map_menu_item_MeshLowerY				5516
#define map_menu_item_MeshSelectAllPoly			5517
#define map_menu_item_MeshResetUV				5520
#define map_menu_item_MeshWholeUV				5521
#define map_menu_item_MeshSingleUV				5522
#define map_menu_item_MeshCreateNormals			5523
#define map_menu_item_MeshInvertNormals			5524
#define map_menu_item_MeshSetNormalsOut			5525
#define map_menu_item_MeshSetNormalsIn			5526

#define map_menu_item_PolygonHole				5600
#define map_menu_item_PolyTesselate				5601
#define map_menu_item_PolygonRotateUV			5603
#define map_menu_item_PolygonFlipU				5604
#define map_menu_item_PolygonFlipV				5605
#define map_menu_item_PolygonRecalcNormal		5606
#define map_menu_item_PolygonInvertNormal		5607
#define map_menu_item_PolygonResetUV			5608
#define map_menu_item_PolygonWholeUV			5609
#define map_menu_item_PolygonSingleUV			5610

//
// model menus
//

#define model_menu_file							0
#define model_menu_edit							1
#define model_menu_model						2
#define model_menu_view							3
#define model_menu_mesh							4
#define model_menu_vertex						5
#define model_menu_bone							6
#define model_menu_pose							7
#define model_menu_animate						8

#define model_menu_item_FileSave				5100
#define model_menu_item_FileQuit				5101

#define model_menu_item_EditUndo				5200
#define model_menu_item_EditSelectMore			5201

#define model_menu_item_PrepareModel			5300
#define model_menu_item_CalcBoxes				5301
#define model_menu_item_CalcNormals				5302
#define model_menu_item_ImportAnimation			5304
#define model_menu_item_ScaleAll				5305
#define model_menu_item_FlipXAll				5306
#define model_menu_item_FlipYAll				5307
#define model_menu_item_FlipZAll				5308
#define model_menu_item_SwapXZAll				5309
#define model_menu_item_SwapYZAll				5310
#define model_menu_item_CenterXZAll				5311
#define model_menu_item_FloorYAll				5312
#define model_menu_item_AddHitBox				5313
#define model_menu_item_AutoGenerateTextures	5314
#define model_menu_item_AutoGenerateModel		5315
#define model_menu_item_CleanUp					5316

#define model_menu_item_Front					5400
#define model_menu_item_Left					5401
#define model_menu_item_Right					5402
#define model_menu_item_Back					5403
#define model_menu_item_Top						5404
#define model_menu_item_Bottom					5405

#define model_menu_item_NewMesh					5500
#define model_menu_item_DuplicateMesh			5501
#define model_menu_item_ImportOBJ				5502
#define model_menu_item_ReplaceOBJ				5503
#define model_menu_item_InsertXML				5504
#define model_menu_item_Scale					5505
#define model_menu_item_FlipX					5506
#define model_menu_item_FlipY					5507
#define model_menu_item_FlipZ					5508
#define model_menu_item_FlipU					5509
#define model_menu_item_FlipV					5510
#define model_menu_item_SwapXZ					5511
#define model_menu_item_SwapYZ					5512
#define model_menu_item_CenterXZ				5513
#define model_menu_item_FloorY					5514
#define model_menu_item_MeshTessellate			5515

#define model_menu_item_VertexSelectAll			5600
#define model_menu_item_VertexSelectAttached	5601
#define model_menu_item_VertexSelectNotAttached	5602
#define model_menu_item_VertexShowAll			5603
#define model_menu_item_VertexHideSelected		5604
#define model_menu_item_VertexHideNonSelected	5605
#define model_menu_item_VertexInvertNormals		5606
#define model_menu_item_VertexSetNormals		5607
#define model_menu_item_VertexSetNormalsOut		5608
#define model_menu_item_VertexSetNormalsIn		5609
#define model_menu_item_VertexClearBones		5610
#define model_menu_item_VertexAutoBones			5611
#define model_menu_item_VertexDelete			5612
#define model_menu_item_VertexPolygonDelete		5613
#define model_menu_item_VertexCollapseSelected	5614
#define model_menu_item_VertexCollapseSimilar	5615
#define model_menu_item_VertexMakeQuad			5616
#define model_menu_item_VertexTessellatePoly	5617

#define model_menu_item_NewBone					5700
#define model_menu_item_SetBone					5701
#define model_menu_item_SelectVertexWithBone	5702
#define model_menu_item_GoToParentBone			5703
#define model_menu_item_DuplicateBoneAttach		5704

#define model_menu_item_NewPose					5800
#define model_menu_item_DupPose					5801
#define model_menu_item_ClearPose				5802
#define model_menu_item_PreviousPose			5803
#define model_menu_item_NextPose				5804

#define model_menu_item_NewAnimate				5900
#define model_menu_item_DupAnimate				5901
#define model_menu_item_ResetTimeAnimate		5902
#define model_menu_item_PlayAnimate				5903
#define model_menu_item_PlayBlendAnimate		5904

//
// list palette
//

#define list_max_item_count									4096
#define list_max_section_count								32

#define list_max_value_sz									64

#define list_title_font_size								14.0f
#define list_item_font_size									12.0f
#define list_item_font_high									14

#define list_wheel_scroll_size								14

#ifndef D3_SETUP
	#define list_palette_border_sz							10
	#define list_palette_tree_sz							260
	#define list_value_clip_size							25
#else
	#define list_palette_border_sz							0
	#define list_palette_tree_sz							400
	#define list_value_clip_size							35
#endif

#define list_palette_scroll_wid								15
#define list_title_line_high								15

#define list_item_ctrl_header								0
#define list_item_ctrl_text									1
#define list_item_ctrl_color								2
#define list_item_ctrl_string								3
#define list_item_ctrl_param								4
#define list_item_ctrl_na_blank								5
#define list_item_ctrl_int									6
#define list_item_ctrl_float								7
#define list_item_ctrl_checkbox								8
#define list_item_ctrl_point								9
#define list_item_ctrl_angle								10
#define list_item_ctrl_vector								11
#define list_item_ctrl_normal_vector						12
#define list_item_ctrl_uv									13
#define list_item_ctrl_picker								14
#define list_item_ctrl_pick_color							15

#define list_string_value_string							0
#define list_string_value_int								1
#define list_string_value_positive_int						2
#define list_string_value_range_int							3
#define list_string_value_float								4
#define list_string_value_positive_float					5
#define list_string_value_0_to_1_float						6

#define list_chord_value_point								0
#define list_chord_value_angle								1
#define list_chord_value_vector								2
#define list_chord_value_uv									3

#define list_button_none									-1
#define list_button_edit									0
#define list_button_plus									1
#define list_button_minus									2
#define list_button_set										3

#define list_button_push_type_none							0
#define list_button_push_type_button						1
#define list_button_push_type_up							2
#define list_button_push_type_down							3

typedef union		{
						int									*int_ptr;
						float								*float_ptr;
						bool								*bool_ptr;
						char								*str_ptr;
						d3pnt								*pnt_ptr;
						d3ang								*ang_ptr;
						d3vct								*vct_ptr;
						d3uv								*uv_ptr;
						d3col								*col_ptr;
					} list_palette_item_value_type;

typedef struct		{
						int									param_idx,str_len;
					} list_palette_item_limit_type;

typedef struct		{
						char								search_path[64],extension[8],required_file_name[file_str_len];
						bool								file_list;
					} list_palette_item_file_type;

typedef struct		{
						int									count,item_sz,name_offset;
						bool								is_index,include_none,sort;
						char								*ptr;
						list_palette_item_file_type			file;
					} list_palette_item_list_type;

typedef struct		{
						int									x,y,ctrl_type,button_type,
															id,idx,button_id,
															button_up_id,button_down_id,
															count;
						bool								selected,disabled,moveable;
						char								name[name_str_len];
						list_palette_item_value_type		value;
						list_palette_item_limit_type		limit;
						list_palette_item_list_type			list;
					} list_palette_item_type;

typedef struct		{
						int									*picker_idx_ptr;
						bool								on,file_list,catch_last_click;
						char								*picker_name_ptr;
					} list_palette_picker_type;

typedef struct		{
						int									id,idx,item_idx;
						list_palette_item_type				*item;
					} list_palette_click_type;

typedef struct		{
						int									level,item_count,
															item_sort_start_idx,
															item_push_idx,
															button_push_idx,
															scroll_offset,sav_scroll_offset,
															total_high;
						bool								back_push_on,item_push_on;
						char								titles[3][128];
						list_palette_click_type				click;
						list_palette_item_type				*items;
					} list_palette_pane_type;

typedef struct		{
						bool								left,never_hide_picker,never_open;
					} list_palette_flag_type;

typedef struct		{
						bool								open,hide;
						list_palette_flag_type				flag;
						list_palette_pane_type				item_pane,picker_pane;
						list_palette_picker_type			picker;
					} list_palette_type;

//
// tool tips
//

#define tool_tip_font_size							15.0f

//
// tool palette
//

#define tool_palette_seperator_size					4

//
// texture editing
//

#define texture_edit_item_high						140
#define texture_edit_scroll_wheel_move				25

#define texture_edit_frame_click_return_idx			100
#define texture_edit_frame_click_delete_idx			101

//
// project item types
//

#define item_interface							0
#define item_interface_project					1
#define item_interface_color					2
#define item_interface_device					3
#define item_interface_logo						4
#define item_interface_title_page				5
#define item_interface_singleplayer				6
#define item_interface_multiplayer				7
#define item_interface_setup					8
#define item_interface_progress					9
#define item_interface_hud						10
#define item_interface_virtual_control			11
#define item_interface_radar					12
#define item_interface_menu						13
#define item_interface_chooser					14
#define item_interface_sound					15
#define item_interface_particle					16
#define item_interface_ring						17
#define item_interface_halo						18
#define item_interface_mark						19
#define item_interface_label					20
#define item_interface_crosshair				21
#define item_interface_action					22
#define item_interface_shader					23
#define item_interface_preload_models			24
#define item_interface_preferences				25

#define item_intro_button_game_new				0
#define item_intro_button_game_load				1
#define item_intro_button_game_setup			2
#define item_intro_button_multiplayer_host		3
#define item_intro_button_multiplayer_join		4
#define item_intro_button_credit				5
#define item_intro_button_quit					6

#define item_intro_button_simple_save_start		10
#define item_intro_button_simple_save_erase		20

//
// map item types
//

#define item_map_setting						0
#define item_map_camera							1
#define item_map_light_media					2
#define item_map_sky_weather					3

#define item_map_mesh							10
#define item_map_liquid							11
#define item_map_node							12
#define item_map_spot							13
#define item_map_scenery						14
#define item_map_light							15
#define item_map_sound							16
#define item_map_particle						17

#define item_map_group							20
#define item_map_movement						21
#define item_map_cinema							22

//
// model item types
//

#define item_model						0
#define item_model_mesh					1
#define item_model_animate				2
#define item_model_pose					3
#define item_model_neutral_pose			4
#define item_model_bone					5
#define item_model_hit_box				6

#define item_model_pose_move_particle	10
#define item_model_pose_move_ring		11

//
// tools
//

#define tool_max_tools							64
#define tool_button_size						32

typedef struct		{
						char						file_name[64];
						bool						separator;
						char						tool_tip[64];
						bitmap_type					bitmap;
					} tool_palette_tool_type;

typedef struct		{
						bool						disabled,selected;
					} tool_palette_state_type;

typedef struct		{
						int							count,push_idx;
						tool_palette_tool_type		tools[tool_max_tools];
						tool_palette_state_type		state[tool_max_tools];
					} tool_palette_type;

//
// texture palettes
//

#define texture_texture_per_page					32

//
// vertex modes
//

#define vertex_mode_none						0
#define vertex_mode_lock						1
#define vertex_mode_snap						2

//
// selection modes
//

#define select_mode_mesh						0
#define select_mode_polygon						1
#define select_mode_vertex						2

//
// grid modes
//

#define grid_mode_none							0
#define grid_mode_small							1
#define grid_mode_large							2

//
// node modes
//

#define node_mode_select						0
#define node_mode_duplicate						1
#define node_mode_link							2
#define node_mode_remove_link					3

//
// handle modes
//

#define handle_mode_rotate						0
#define handle_mode_move						1

//
// drag handle types
//

#define drag_handle_none				-1
#define drag_handle_x					0
#define drag_handle_y					1
#define drag_handle_z					2

//
// normal modes
//

#define normal_mode_none				0
#define normal_mode_vertex				1
#define normal_mode_face				2

//
// movement scales
//

#ifdef D3_OS_MAC
	#define move_mouse_min_scale				200
	#define move_mouse_distance_ratio			800
#else
	#define move_mouse_min_scale				40
	#define move_mouse_distance_ratio			1000
#endif

#define move_mouse_turn_reduce_scale			5.0f

#define move_mouse_drag_distance_ratio			1000

#define move_scroll_wheel_scale					300

#define move_key_shift_multiply					10

#define view_texture_shift_factor				100.0f

//
// opengl settings
//

#define view_near_offset						430
#define view_look_snap							10.0f

//
// display sizes
//

#define view_handle_size						12.0f
#define view_skew_handle_size					8.0f

#define view_handle_length_min					150.0f
#define view_handle_length_factor				0.05f
#define view_handle_line_width					2.0f

#define view_sprite_size						400

#define view_snap_clip_size_factor				150

//
// light maps
//

#define light_map_texture_block_size						8

#define light_map_max_textures								64
#define light_map_max_solid_color							64
#define light_map_render_margin								5

#define light_map_texture_min_rect_size						16

#define light_map_blur_count								3

#define light_map_quality_very_low							0
#define light_map_quality_low								1
#define light_map_quality_medium							2
#define light_map_quality_high								3
#define light_map_quality_very_high							4

#define light_map_quality_values							{(10.0f/10000.0f),(30.0f/10000.0f),(50.0f/10000.0f),(75.0f/10000.0f),(100.0f/10000.0f)}

typedef struct		{
						int									x_shift,y_shift;
						unsigned char						col[3];
					} light_map_solid_color_cache_type;

typedef struct		{
						int									solid_color_cache_count;
						unsigned char						*block,*pixel_data,
															*pixel_touch;
						light_map_solid_color_cache_type	*solid_color_cache;
					} light_map_texture_type;
					
typedef struct		{
						int									ptsz,txt_idx,
															mesh_idx,poly_idx,liquid_idx,
															x[8],y[8],
															x_shift,y_shift,
															x_sz,y_sz;
						bool								solid_color;
						d3pnt								pt[8];
						d3rect								box;
					} light_map_poly_type;
					
typedef struct		{
						unsigned char						*data;
					} light_map_map_texture_alpha_type;

//
// uv layers
//

#define uv_layer_normal							0
#define uv_layer_light_map						1

//
// normals
//

#define normal_vector_scale						300.0f

//
// import modes
//

#define import_mode_new							0
#define import_mode_replace						1
#define import_mode_replace_all					2

//
// auto generate mode buttons
//

#define map_auto_generate_button_previous		0
#define map_auto_generate_button_next			1
#define map_auto_generate_button_done			2

//
// map undos
//

#define max_map_undo_level						8

typedef struct		{
						int						count;
						unsigned char			*data;
					} map_undo_chunk_type;
					
typedef struct		{
						map_undo_chunk_type		mesh,liquid,
												spot,scenery,node,
												light,sound,particle,
												selection;
					} map_undo_type;

//
// selections
//

#define select_max_item							5120

typedef struct		{
                        int						type,main_idx,sub_idx;
					} select_item_type;

//
// view picker
//

typedef struct		{
						int						col;
						short					type,main_idx,sub_idx;
						bool					hit;
					} view_picker_type;

//
// editor draw sorting
//

#define view_mesh_sort_max_mesh						5000

typedef struct		{
						int							mesh_idx,dist;
					} view_mesh_sort_mesh_type;

typedef struct		{
						int							count;
						view_mesh_sort_mesh_type	*meshes;
					} view_mesh_sort_type;

//
// model vertex and poly masks
//

#define model_mask_max_vertex			10240
#define model_mask_max_poly				10240

#define model_mask_flag_sel				0x1
#define model_mask_flag_hide			0x2

//
// model drawing sizes
//

#define model_draw_poly_select_line_size	2.0f
#define model_draw_vertex_handle_size		8.0f

#define model_draw_normal_size				3.0f
#define model_draw_normal_len				50.0f

//
// play types
//

#define model_play_mode_stop			0
#define model_play_mode_normal			1
#define model_play_mode_blend			2
#define model_play_mode_slow			3
#define model_play_mode_prev			4
#define model_play_mode_next			5

//
// preference structures
//

typedef struct		{
						d3col					mesh_line,mesh_sel,poly_sel,poly_cull,
												background;
					} app_pref_col_type;

typedef struct		{
						int						duplicate_offset,snap_size,
												clip_distance;
						bool					free_look,auto_texture,big_texture,
												show_tangent_binormal,
												flip_horz_movement,flip_vert_movement,
												flip_horz_turn,flip_vert_turn,
												flip_forward_movement;
						char					engine_name[256];
						app_pref_col_type		col;
					} app_pref_type;

//
// state structures
//

typedef struct		{
						int						cur_item,cur_device,
												cur_intro_button_idx,cur_intro_model_idx,
												cur_hud_bitmap_idx,cur_hud_text_idx,cur_hud_bar_idx,
												cur_virtual_control_stick_idx,cur_virtual_control_button_idx,
												cur_radar_icon_idx,cur_singleplayer_option_idx,cur_multiplayer_character_idx,
												cur_menu_idx,cur_menu_item_idx,cur_chooser_idx,cur_chooser_piece_idx,
												cur_multiplayer_game_idx,cur_multiplayer_option_idx,
												cur_sound_idx,cur_particle_idx,cur_group_particle_idx,cur_ring_idx,
												cur_halo_idx,cur_mark_idx,cur_label_idx,cur_crosshair_idx,
												cur_action_idx,cur_shader_idx;
					} project_state_type;

typedef struct		{
						int						vertex_mode,select_mode,grid_mode,node_mode,
												handle_mode,view_select_idx,drag_handle_idx,
												texture_edit_idx,auto_generate_button_idx,
												cur_no_sel_piece_type,cur_group_idx,
												cur_cinema_idx,cur_cinema_action_idx,
												cur_movement_idx,cur_movement_move_idx;
						bool					map_open,auto_generate_on,
												free_look,select_add,auto_texture,select_box_on,
												show_liquid,show_normals,show_movements,
												show_node,show_object,show_lightsoundparticle;
						char					map_file_name[file_str_len];
						d3pnt					select_box_start_pnt,select_box_end_pnt;
					} map_state_type;

typedef struct		{
						int						tick,animate_idx,pose_move_idx;
					} model_state_blend_type;
					
typedef struct		{
						int						cur_item,cur_mesh_idx,cur_bone_idx,
												cur_pose_idx,cur_animate_idx,cur_hit_box_idx,
												cur_animate_pose_move_idx,cur_pose_bone_move_idx,
												cur_animate_pose_move_particle_idx,
												cur_animate_pose_move_ring_idx,
												texture_edit_idx,drag_bone_mode,select_mode,
												play_mode,magnify_z;
						bool					model_open,texture,mesh,bone,hit_box,
												normal,view_box,bone_names,sel_vertex_with_bone,
												drag_sel_on,show_mesh[max_model_mesh];
						char					model_file_name[256];
						d3pnt					shift;
						d3ang					ang;
						d3rect					drag_sel_box;
						model_state_blend_type	blend[max_model_blend_animation];
					} model_state_type;

typedef struct		{
						int						mode;
						project_state_type		proj;
						map_state_type			map;
						model_state_type		model;
					} app_state_type;

