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
// view maximums
//

#define max_light_spot										128	

#define join_info_max_players								24				// needs to reflect host_max_remote_count in network_defs.h

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
						bool								pinged,unreachable;
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
						bool								on;
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

typedef struct		{
						GLint								dim3ProjectionMatrix,
															dim3Vertex,
															dim3VertexUV,
															dim3VertexColor,
															dim3SimpleColor;
					} shader_cached_var_loc;

typedef struct		{
						char								name[64],vertex_name[64],fragment_name[64];
						GLuint								vertex_obj,fragment_obj,program_obj;
						shader_cached_var_loc				var_locs;
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
// view screen
//

typedef struct		{
						int									x_sz,y_sz;
						bool								wide;
					} view_screen_type;


//
// view rendering structure
//

typedef struct		{
						int									under_liquid_idx,z_adjust;
						float								fov;
						bool								flip;
						d3pnt								pnt;
						d3ang								ang;
					} view_render_camera_type;
	
typedef struct		{
						int									count,in_view_count;
						halo_draw_type						halos[max_light_spot];
					} view_render_halo_type;

typedef struct		{
						int									cull_idx;
						bool								force_camera_obj;
						view_render_camera_type				camera;
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


