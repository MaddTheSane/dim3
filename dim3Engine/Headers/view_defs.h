/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Definitions Header

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
// vertex objects
//

#define view_vbo_text_count									64
#define view_vbo_utility_count								128

//
// back renderers
//

#define back_render_texture_pixel_size						256

//
// view maximums
//

#define max_view_render_item								5120

#define max_light_spot										128	

#define join_info_max_players								24				// needs to reflect host_max_remote_count in network_defs.h

//
// shaders
//
// Note that the iPhone has a severe limitation
// on varrying variables in shaders, so we need to cut
// the light list in half
//

#define max_shader_custom_vars								8

#define max_core_shader_data_sz								10240

#ifdef D3_OS_IPHONE
	#define max_shader_light								2
#else
	#define max_shader_light								4
#endif

#define max_core_shader										25

#define gl_core_model_shader_light							0
#define gl_core_model_shader_light_bump						1
#define gl_core_model_shader_light_bump_spec				2
#define gl_core_model_shader_light_glow						3
#define gl_core_model_shader_light_bump_glow				4
#define gl_core_model_shader_light_bump_spec_glow			5

#define gl_core_model_shader_fog_light						6
#define gl_core_model_shader_fog_light_bump					7
#define gl_core_model_shader_fog_light_bump_spec			8
#define gl_core_model_shader_fog_light_glow					9
#define gl_core_model_shader_fog_light_bump_glow			10
#define gl_core_model_shader_fog_light_bump_spec_glow		11

#define gl_core_map_shader_light_map						12
#define gl_core_map_shader_light_map_bump					13
#define gl_core_map_shader_light_map_bump_spec				14
#define gl_core_map_shader_light_map_glow					15
#define gl_core_map_shader_light_map_bump_glow				16
#define gl_core_map_shader_light_map_bump_spec_glow			17

#define gl_core_map_shader_fog_light_map					18
#define gl_core_map_shader_fog_light_map_bump				19
#define gl_core_map_shader_fog_light_map_bump_spec			20
#define gl_core_map_shader_fog_light_map_glow				21
#define gl_core_map_shader_fog_light_map_bump_glow			22
#define gl_core_map_shader_fog_light_map_bump_spec_glow		23

#define gl_core_map_shader_liquid							24

#define gl_shader_core_index								1000

#define core_shader_group_map								0
#define core_shader_group_liquid							1
#define core_shader_group_model								2
#define core_shader_group_color								3
#define core_shader_group_bitmap							4

//
// diffuse
//

#define gl_diffuse_ambient_factor							0.75f

//
// shadows
//

#define view_shadows_map_poly_count							256
#define view_shadows_model_vertex_count						10240
#define view_shadows_infinite_light_shift					100
#define view_shadows_bounds_check_slop						100
#define view_shadows_clip_infinite_distance					100000

//
// effects
//

#define effect_lightning_min_lines							5
#define effect_lightning_max_lines							150

//
// input rate
//

#define input_tick_rate										33

//
// stencil constants
//

#define stencil_none										0
#define stencil_poly_start									1
#define stencil_poly_end									255

#define stencil_shadow										1

//
// render item type
//

#define view_render_type_none								-1
#define view_render_type_mesh								0
#define view_render_type_liquid								1
#define view_render_type_object								2
#define view_render_type_projectile							3
#define view_render_type_effect								4

//
// shader variable types
//

#define shader_var_type_int									0
#define shader_var_type_float								1
#define shader_var_type_vec3								2
#define shader_var_type_vec4								3

//
// console size
//

#define console_screen_percent								0.4f

//
// metrics
//

#define metrics_txt_sz										18

//
// screen sizes
//

#define max_screen_size										64

typedef struct		{
						int									wid,high;
					} screen_size_type;
					
//
// render info
//
 
typedef struct		{
						int									monitor_refresh_rate,
															texture_unit_count,texture_max_size,
															nscreen_size;
						char								name[64],ext_string[8192];
						screen_size_type					desktop,
															screen_sizes[max_screen_size];
					} render_info_type;

//
// image structures
//

#define max_view_image										1024

typedef struct		{
						int									rtl_material_id;
						char								path[1024];
						bitmap_type							bitmap;
					} view_image_type;

//
// lighting structures
//

typedef struct		{
						int									direction,i_intensity;
						bool								light_map;
						float								f_x,f_y,f_z,
															f_intensity,f_inv_intensity,f_exponent;
						d3pnt								pnt;
						d3vct								pnt_eye_space;
						d3col								col;
					} view_light_spot_type;

typedef struct		{
						float								boost;
						d3vct								vct;
					} view_glsl_light_list_diffuse_type;

typedef struct		{
						int									intensity;
						float								exponent;
						bool								on;
						d3pnt								pnt;
						d3col								col;
					} view_glsl_light_list_ui_light_type;

typedef struct		{
						int									nlight,light_idx[max_shader_light];
						bool								hilite;
						view_glsl_light_list_diffuse_type	diffuse;
						view_glsl_light_list_ui_light_type	ui_light;
					} view_glsl_light_list_type;

//
// join UI structures
//

#define max_join_server_host								64
#define max_join_server_news_len							5120

typedef struct		{
						int									score;
						bool								bot;
						char								name[name_str_len];
					} join_server_host_player;

typedef struct		{
						int									count,max_count;
						join_server_host_player				players[join_info_max_players];
					} join_server_host_player_list_type;

typedef struct		{
						int									ping_msec,
															score_limit,option_flags,
															respawn_secs,game_reset_secs;
						char								ip[256],name[64],
															game_name[name_str_len],map_name[name_str_len];
						bool								pinging,unreachable;
						join_server_host_player_list_type	player_list;
					} join_server_host_type;

typedef struct		{
						int									count;
						join_server_host_type				hosts[max_join_server_host];
					} join_server_host_list_type;

//
// halo structures
//
 
typedef struct		{
						int									idx,obj_idx,pixel_sz;
						float								alpha;
						bool								in_view;
						d3pnt								pnt,proj_pnt;
					} halo_draw_type;

//
// rain structures
//

typedef struct		{
						int									x,z,y,by,
															x_off,z_off;
					} rain_draw_type;

//
// chat, console and score structures
//

#define max_view_chat_lines									15
#define max_view_chat_str_len								64

#define max_view_console_lines								14
#define max_view_console_txt_sz								128

typedef struct		{
						char								name[name_str_len],str[max_view_chat_str_len];
						d3col								col;
					} view_chat_line_type;

typedef struct		{
						int									nline,remove_tick;
						char								type_str[max_view_chat_str_len];
						bool								type_on;
						view_chat_line_type					*lines;
					} view_chat_type;

typedef struct		{
						int									style;
						char								txt[max_view_console_txt_sz];
						d3col								color;
					} view_console_line_type;
					
typedef struct		{
						int									nline;
						bool								on,focus;
						view_console_line_type				*lines;
					} view_console_type;

typedef struct		{
						bool								on;
					} view_score_type;

//
// timing structures
//

typedef struct		{
						int									input_tick,draw_tick,draw_time,
															run_tick;
					} view_time_type;

//
// fps type
//

typedef struct		{
						int									tick,count,last_time;
						float								total;
						bool								first_skip;
					} view_fps_type;

//
// shader structures
//

typedef union		{
						int									i;
						float								f;
						float								vec3[3];
						float								vec4[4];
					} shader_value_type;
					
typedef struct		{
						int									var_type;
						char								name[name_str_len];
						shader_value_type					value;
					} shader_custom_var_type;

typedef struct		{
						GLint								position,color,
															intensity,invertIntensity,exponent,
															direction;
					} shader_cached_var_light_loc;
					
typedef struct		{
						GLint								dim3ProjectionMatrix,dim3ModelViewMatrix,
															dim3NormalMatrix,dim3FrequencySecond,
															dim3CameraPosition,dim3AmbientColor,
															dim3ShineFactor,dim3GlowFactor,
															dim3Alpha,dim3DiffuseVector,
															dim3FogEnd,dim3FogScale,dim3FogColor,
															dim3Vertex,dim3VertexColor,
															dim3VertexUV,dim3VertexLightMapUV,
															dim3VertexTangent,dim3VertexNormal,
															dim3SimpleColor;
						shader_cached_var_light_loc			dim3Lights[max_shader_light];
					} shader_cached_var_loc;

typedef struct		{
						int									direction;
						float								intensity,invertIntensity,exponent;
						d3fpnt								position;
						d3col								color;
					} shader_current_var_light_value;

typedef struct		{
						float								r,g,b,a;
					} shader_current_var_simple_color;

typedef struct		{
						int									vertex_offset,color_offset,
															uv_offset,lmap_uv_offset,
															tangent_offset,normal_offset;
					} shader_current_var_simple_attrib;

typedef struct		{
						int									nlight,light_idx[max_shader_light];
						float								alpha,shine_factor,glow_factor;
						d3vct								diffuse_vct;
						shader_current_var_simple_attrib	attrib;
						shader_current_var_simple_color		simple_color;
						shader_current_var_light_value		lights[max_shader_light];
					} shader_current_var_value;

typedef struct		{
						int									start_tick,current_hilite;
						char								name[64],
															vertex_name[64],fragment_name[64];
						bool								need_matrix_reset,need_frame_reset;
						GLuint								vertex_obj,fragment_obj,program_obj;
						shader_cached_var_loc				var_locs;
						shader_current_var_value			var_values;
					} shader_type;

//
// camera structure
//

typedef struct		{
						d3pnt								pnt,static_pnt;
						d3ang								ang,static_ang,chase_ang;
					} camera_current_position_type;

typedef struct		{
						int									node_seek_idx,node_dest_idx,
															total_dist,msec,event_id,
															start_tick,end_tick;
						float								speed;
						bool								on,open_doors,in_freeze;
						d3pnt								start_pnt,end_pnt;
						d3ang								start_ang,end_ang;
					} camera_auto_walk_type;

typedef struct		{
						int									end_tick;
						bool								on;
						d3ang								ang;
					} camera_auto_turn_type;

typedef struct		{
						int									start_tick,end_tick;
						float								start_fov,end_fov,
															start_aspect_ratio,end_aspect_ratio;
						bool								on;
					} camera_animate_type;

typedef struct		{
						int									obj_idx;
						camera_current_position_type		cur_pos;
						camera_auto_walk_type				auto_walk;
						camera_auto_turn_type				auto_turn_angle_offset,auto_turn_chase_angle;
						camera_animate_type					animate;
					} camera_type;

//
// view counts
//

typedef struct		{
						int									mesh,mesh_poly,
															liquid,liquid_poly,
															model,model_poly,
															shadow,shadow_poly,
															effect,obscure_percent;
					} view_count_type;

//
// view screen
//

typedef struct		{
						int									x_sz,y_sz;
						bool								wide;
					} view_screen_type;


//
// view rendering structure
//

#define view_list_item_flag_model_in_view					0x1
#define view_list_item_flag_shadow_in_view					0x2

typedef struct		{
						int									under_liquid_idx,z_adjust;
						float								fov;
						bool								flip;
						d3pnt								pnt;
						d3ang								ang;
					} view_render_camera_type;
	
typedef struct		{
						int									flag,dist;
						short								type,idx;
					} view_render_draw_list_item_type;
					
typedef struct		{
						int									count;
						view_render_draw_list_item_type		items[max_view_render_item];
					} view_render_draw_list_type;

typedef struct		{
						int									count;
						view_light_spot_type				spots[max_light_spot];
					} view_render_light_type;

typedef struct		{
						int									count,in_view_count;
						halo_draw_type						halos[max_light_spot];
					} view_render_halo_type;

typedef struct		{
						int									cull_idx;
						bool								force_camera_obj;
						view_render_camera_type				camera;
						view_render_draw_list_type			draw_list;
						view_render_light_type				light;
						view_render_halo_type				halo_draw;
					} view_render_type;

//
// UI states
//

typedef struct		{
						int									fade_start_tick,menu_idx,item_idx;
						bool								fade_in,fade_out,active,
															mouse_down,key_down;
						d3pnt								mouse_pnt;
					} view_menu_type;

typedef struct		{
						int									idx,event_id,
															start_tick,last_tick;
						bool								on;
						camera_type							camera_state;
					} view_cinema_type;

typedef struct		{
						char								str[256],str_2[256];
					} view_error_type;

//
// main view structure
//
 
typedef struct		{
						int									device_type;
						view_count_type						count;
						view_screen_type					screen;
						view_render_type					*render;
						view_time_type						time;
						view_fps_type						fps;
						view_image_type						*images;
						rain_draw_type						*rain_draws;
						view_menu_type						menu;
						view_cinema_type					cinema;
						view_chat_type						chat;
						view_console_type					console;
						view_score_type						score;
						view_error_type						error;
					} view_type;


