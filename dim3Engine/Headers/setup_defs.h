/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Setup XML Definitions Header

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
// setup sizes
//

#define max_setup_action							128
#define max_setup_action_attach						4

#define max_setup_network_host						32

#define max_setup_network_map						32
#define max_setup_network_option					32

//
// setup values
//

// texture quality mode, mipmap mode, and mipmap mode all defined in baseutility.h

#define fsaa_mode_none								0
#define fsaa_mode_2x								1
#define fsaa_mode_4x								2
#define fsaa_mode_8x								3

#define setup_fsaa_mode_list_def					{"None","2x","4x","8x",""}

//
// setup paths
//

typedef struct		{
						char						dim3_name[256],dim3_base_path[1024],dim3_data_path[1024];
					} setup_path_type;
					
//
// setup actions
//

typedef struct		{
						char						name[32],attach[max_setup_action_attach][32];
					} setup_action_type;
					
typedef struct		{
						int							naction;
						setup_action_type			actions[max_setup_action];
					} setup_action_list_type;
					
//
// setup mouse/joystick axis
//

typedef struct		{
						float						speed,acceleration;
					} setup_axis_type;

//
// setup network
//

typedef struct		{
						int								count,skill;
					} setup_network_bot_type;

typedef struct		{
						char							name[name_str_len];
					} setup_network_map_type;

typedef struct		{
						int								count;
						setup_network_map_type			maps[max_setup_network_map];
					} setup_network_map_list_type;
					
typedef struct		{
						char							name[name_str_len];
					} setup_network_option_type;

typedef struct		{
						int								count;
						setup_network_option_type		options[max_setup_network_option];
					} setup_network_option_list_type;
					
typedef struct		{
						int								game_type,score_limit,
														respawn_secs,game_reset_secs,
														character_idx,tint_color_idx;
						char							name[name_str_len],
														custom_host_ip[64];
						bool							show_names,map_rotation;
						setup_network_bot_type			bot;
						setup_network_map_list_type		map_list;
						setup_network_option_list_type	option_list;
					} setup_network_type;

//
// setup struct
//
					
typedef struct		{
						int								screen_wid,screen_high,
														screen_rtl_wid,screen_rtl_high,
														fsaa_mode;
						float							gamma,sound_volume,music_volume;
						bool							decal_on,lightmap_on,shadow_on,
														always_run,toggle_run,invert_look,mouse_smooth,
														music_on,auto_aim,window,window_editor,
														no_hud,no_draw_weapon,metrics_on,debug_on,
														ignore_fps_lock,screen_rtl_full_window;
						setup_path_type					path;
						setup_axis_type					mouse,joystick;
						setup_action_list_type			action_list;
						setup_network_type				network;
					} setup_type;
