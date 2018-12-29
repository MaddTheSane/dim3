/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: JavaScript Definitions Header

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
// Javascript Engine
//

#define STDBOOL_WIN32_H

#include <JavaScriptCore/JavaScript.h>

//
// script defines
//

#define max_script_list									1024
#define max_timer_list									1024
#define max_global_list									1024

#define js_max_parent_count								3
#define js_max_recursive_count							5

//
// attach thing types
//

#define thing_type_game									0
#define thing_type_course								1
#define thing_type_object								2
#define thing_type_weapon								3
#define thing_type_projectile							4

//
// timer types
//

#define timer_mode_single								0
#define timer_mode_repeat								1
#define timer_mode_chain								2
#define timer_mode_dispose								3

//
// hit constants
//

#define sd_proj_hit_type_none							1000
#define sd_proj_hit_type_object							1001
#define sd_proj_hit_type_wall							1002
#define sd_proj_hit_type_floor							1003
#define sd_proj_hit_type_ceiling						1004
#define sd_proj_hit_type_projectile						1005
#define sd_proj_hit_type_melee							1006
#define sd_proj_hit_type_auto							1007

//
// crosshair constants
//

#define sd_weap_crosshair_type_center					1010
#define sd_weap_crosshair_type_bone_tracking			1011
#define sd_weap_crosshair_type_bone_tracking_resizing	1012
#define sd_weap_crosshair_type_barrel_tracking			1013
#define sd_weap_crosshair_type_barrel_tracking_resizing	1014

//
// camera constants
//

#define sd_camera_type_fpp								1020
#define sd_camera_type_chase							1021
#define sd_camera_type_static							1022
#define sd_camera_type_chase_static						1023

//
// input constants
//

#define sd_input_mode_fpp								1030
#define sd_input_mode_side_scroll						1031
#define sd_input_mode_top_down							1032
#define sd_input_mode_fly								1033
#define sd_input_mode_thrust							1034

//
// light constants
//

#define sd_light_type_normal							1040
#define sd_light_type_blink								1041
#define sd_light_type_glow								1042
#define sd_light_type_pulse								1043
#define sd_light_type_flicker							1044
#define sd_light_type_failing							1045

#define sd_light_direction_all							1050
#define sd_light_direction_neg_x						1051
#define sd_light_direction_pos_x						1052
#define sd_light_direction_neg_y						1053
#define sd_light_direction_pos_y						1054
#define sd_light_direction_neg_z						1055
#define sd_light_direction_pos_z						1056

//
// message constants
//

#define sd_message_to_player							1080
#define sd_message_to_object							1081
#define sd_message_to_course							1082
#define sd_message_to_game								1083

//
// ducking constants
//

#define sd_stand_standing								1090
#define sd_stand_ducking								1091
#define sd_stand_standing_up							1092
#define sd_stand_ducking_down							1093

//
// air constants
//

#define sd_air_up										1100
#define sd_air_down										1101
#define sd_air_falling									1102
#define sd_air_ground									1103

//
// liquid constants
//

#define sd_liquid_out									1110
#define sd_liquid_in									1111
#define sd_liquid_float									1112
#define sd_liquid_under									1113

//
// projection constants
//

#define sd_projection_type_fov							1130
#define sd_projection_type_frustum						1131

//
// team constants
//

#define sd_team_none									1140
#define sd_team_red										1141
#define sd_team_blue									1142
#define sd_team_common									1143

//
// skill constants
//

#define sd_skill_easy									1150
#define sd_skill_medium									1151
#define sd_skill_hard									1152

//
// spot type constants
//

#define sd_spot_type_object								1160
#define sd_spot_type_bot								1161
#define sd_spot_type_player								1162
#define sd_spot_type_spawn								1163
#define sd_spot_type_checkpoint							1164

//
// object type constants
//

#define sd_object_type_player							1170
#define sd_object_type_bot_multiplayer					1171
#define sd_object_type_bot_map							1172
#define sd_object_type_object							1173
#define sd_object_type_remote_player					1174
#define sd_object_type_remote_object					1175

//
// collision type modes
//

#define sd_collision_mode_cylinder						1180
#define sd_collision_mode_box							1181
#define sd_collision_mode_simple						1182

//
// main events constants
//

#define sd_event_construct								2000
#define sd_event_map									2001
#define sd_event_spawn									2002
#define sd_event_die									2003
#define sd_event_telefrag								2004
#define sd_event_jump									2005
#define sd_event_fall									2006
#define sd_event_land									2007
#define sd_event_collide								2008
#define sd_event_liquid									2009
#define sd_event_animation_object						2010
#define sd_event_animation_weapon						2011
#define sd_event_timer									2012
#define sd_event_wait									2013
#define sd_event_message								2014
#define sd_event_touch									2015
#define sd_event_hit									2016
#define sd_event_click									2017
#define sd_event_damage									2018
#define sd_event_crush									2019
#define sd_event_health									2020
#define sd_event_pickup									2021
#define sd_event_add_custom								2022
#define sd_event_path									2023
#define sd_event_watch									2024
#define sd_event_weapon_fire							2025
#define sd_event_weapon_select							2026
#define sd_event_projectile								2027
#define sd_event_melee									2028
#define sd_event_vehicle								2029
#define sd_event_move									2030
#define sd_event_menu									2031
#define sd_event_chooser								2032
#define sd_event_console								2033
#define sd_event_interface								2034
#define sd_event_state									2035
#define sd_event_remote									2036
#define sd_event_score									2037
#define sd_event_dispose								2038

#define event_main_id_start								sd_event_construct
#define event_main_id_end								sd_event_dispose

#define event_main_id_count								((event_main_id_end-event_main_id_start)+1)

//
// sub events constants
//

#define sd_event_map_open								3000
#define sd_event_map_close								3001

#define sd_event_spawn_init								3002
#define sd_event_spawn_reborn							3003
#define sd_event_spawn_game_reset						3004
#define sd_event_spawn_map_change						3005

#define sd_event_liquid_in								3006
#define sd_event_liquid_out								3007
#define sd_event_liquid_submerge						3008
#define sd_event_liquid_surface							3009

#define sd_event_animation_object_walk					3010
#define sd_event_animation_object_run					3011	
#define sd_event_animation_object_crawl					3012

#define sd_event_animation_object_walk_back				3013
#define sd_event_animation_object_run_back				3014
#define sd_event_animation_object_crawl_back			3015

#define sd_event_animation_object_walk_left				3016
#define sd_event_animation_object_run_left				3017
#define sd_event_animation_object_crawl_left			3018
#define sd_event_animation_object_walk_left_forward		3019
#define sd_event_animation_object_run_left_forward		3020
#define sd_event_animation_object_crawl_left_forward	3021
#define sd_event_animation_object_walk_left_back		3022
#define sd_event_animation_object_run_left_back			3023
#define sd_event_animation_object_crawl_left_back		3024

#define sd_event_animation_object_walk_right			3025
#define sd_event_animation_object_run_right				3026
#define sd_event_animation_object_crawl_right			3027
#define sd_event_animation_object_walk_right_forward	3028
#define sd_event_animation_object_run_right_forward		3029
#define sd_event_animation_object_crawl_right_forward	3030
#define sd_event_animation_object_walk_right_back		3031
#define sd_event_animation_object_run_right_back		3032
#define sd_event_animation_object_crawl_right_back		3033

#define sd_event_animation_object_turn_left				3034
#define sd_event_animation_object_turn_right			3035
#define sd_event_animation_object_turn_stop				3036

#define sd_event_animation_object_stop					3037
#define sd_event_animation_object_fall					3038
#define sd_event_animation_object_land					3039
#define sd_event_animation_object_jump					3040
#define sd_event_animation_object_duck_down				3041
#define sd_event_animation_object_ducking				3042
#define sd_event_animation_object_stand_up				3043
#define sd_event_animation_object_standing				3044

#define sd_event_message_from_script					3045
#define sd_event_message_from_course					3046
#define sd_event_message_from_key_down					3047
#define sd_event_message_from_key_up					3048

#define sd_event_animation_weapon_raise					3049
#define sd_event_animation_weapon_lower					3050
#define sd_event_animation_weapon_held					3051
#define sd_event_animation_weapon_add_ammo				3052
#define sd_event_animation_weapon_add_clip				3053
#define sd_event_animation_weapon_add_alt_ammo			3054
#define sd_event_animation_weapon_add_alt_clip			3055

#define sd_event_path_node								3056
#define sd_event_path_done								3057

#define sd_event_watch_object_near						3058
#define sd_event_watch_object_far						3059
#define sd_event_watch_object_death						3060
#define sd_event_watch_object_enter_base				3061
#define sd_event_watch_object_exit_base					3062
#define sd_event_watch_object_sound						3063
#define sd_event_watch_object_damage					3064

#define sd_event_weapon_fire_single						3065
#define sd_event_weapon_fire_up							3066
#define sd_event_weapon_fire_down						3067
#define sd_event_weapon_fire_repeat						3068
#define sd_event_weapon_fire_clip_change				3069
#define sd_event_weapon_fire_zoom_enter					3070
#define sd_event_weapon_fire_zoom_exit					3071

#define sd_event_projectile_bounce						3072
#define sd_event_projectile_reflect						3073

#define sd_event_melee_hit								3074

#define sd_event_vehicle_enter							3075
#define sd_event_vehicle_exit							3076

#define sd_event_move_done								3077
#define sd_event_move_loop								3078

#define sd_event_interface_title_done					3079
#define sd_event_interface_story_done					3080
#define sd_event_interface_cinema_done					3081

#define sd_event_state_load								3082
#define sd_event_state_load_checkpoint					3083
#define sd_event_state_save								3084
#define sd_event_state_save_checkpoint					3085

#define sd_event_remote_join							3086
#define sd_event_remote_leave							3087
#define sd_event_remote_team							3088
#define sd_event_remote_death							3089
#define sd_event_remote_suicide							3090
#define sd_event_remote_telefrag						3091

//
// script event messages
//

#define d3_jsval_type_number							0
#define d3_jsval_type_boolean							1
#define d3_jsval_type_string							2

#define max_d3_jsval_str_len							128

#define max_script_msg_data								8

//
// javascript data storage
//

typedef union		{
						float							d3_number;
						bool							d3_boolean;
						char							d3_string[max_d3_jsval_str_len];
					} d3_jsval_data_type;

//
// define structure
//
 
typedef struct		{
						int								value_int;
						char							value_str[64],name[64];
					} script_define_type;

//
// script attachments
//

typedef struct		{
						int								thing_type,
														obj_idx,weap_idx,proj_setup_idx,proj_idx;
					} script_attach_type;

//
// global structures
//
 
typedef struct		{
						int								type;
						char							name[name_str_len];
						d3_jsval_data_type				data;
					} global_type;

typedef struct		{
						global_type*					globals[max_global_list];
					} global_list_type;

//
// timer structures
//
 
typedef struct		{
						int								mode,count,freq,user_id,
														script_idx,override_proj_idx;
						char							chain_func_name[64];
						script_attach_type				attach;
					} timer_type;

typedef struct		{
						timer_type*						timers[max_timer_list];
					} timer_list_type;

//
// script structures
//

typedef struct		{
						int								count;
						bool							in_event[event_main_id_count];
					} script_recursive_type;

typedef struct		{
						JSObjectRef						func[event_main_id_count];
					} script_event_attach_list_type;

typedef struct		{
						int								main_event,sub_event,id,tick;
					} script_event_state_type;

typedef struct		{
						int								idx,data_len,parent_idx,child_idx;
						char							name[file_str_len],sub_dir[file_str_len],
														implement_name[file_str_len];
						char							*data;
						script_attach_type				attach;
						script_event_state_type			event_state;
						script_recursive_type			recursive;
						script_event_attach_list_type	event_attach_list;
						JSGlobalContextRef				cx;
						JSObjectRef						obj,global_obj;
					} script_type;

typedef struct		{
						script_type*					scripts[max_script_list];
					} script_list_type;

//
// main js engine structure
//

typedef struct		{
						int								timer_tick,
														game_script_idx,course_script_idx;
						JSContextGroupRef				cx_group;
						JSClassRef						main_empty_class;
						script_list_type				script_list;
						timer_list_type					timer_list;
						global_list_type				global_list;
					} js_type;


