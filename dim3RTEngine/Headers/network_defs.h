/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Network Definitions Header

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
// additional network->host conversions
//

#define htonf(x)										(htonl((int)((x)*1000.0f)))
#define ntohf(x)										(((float)((signed int)ntohl(x)))/1000.0f)

//
// networking ports
//

#define net_port_host									11800

//
// low-level socket timing
//

#define socket_no_data_retry							10
#define socket_no_data_u_wait							25

//
// max message size
//

#define net_max_msg_size								3072


//
// host defines
//

#define host_message_per_loop_count						50

#define host_no_data_u_wait								25
#define host_max_network_error_reject					10

#define host_max_remote_count							24

#define host_client_timeout_msec_rate					10000

//
// client defines
//

#define client_query_timeout_wait_msec					2500
#define client_join_timeout_wait_msec					5000
#define client_message_per_loop_count					10

#define client_communication_update_msec_rate			25
#define client_communication_group_synch_msec_rate		5000

#define client_communication_timeout_msec				1000

//
// network modes
//

#define net_mode_none									0
#define net_mode_client									1
#define net_mode_host									2

//
// queue defines
//

#define net_queue_max_message							256

//
// network address
//

typedef struct		{
						int								port;
						unsigned long					ip;
					} net_address_type;

//
// network queues
//

typedef struct		{
						int								action,msg_len;
						unsigned char					msg[net_max_msg_size];
						net_address_type				addr;
					} net_queue_msg_type;

typedef struct		{
						int								count;
						net_queue_msg_type				*msgs;
						SDL_mutex						*lock;
					} net_queue_type;

//
// host player list
//

typedef struct		{
						int								net_uid;
						bool							local,bot;
						net_address_type				addr;
						d3socket						sock;
					} net_host_player_connect_type;

typedef struct		{
						int								team_idx,score;
						char							name[name_str_len],
														draw_name[name_str_len];
						d3pnt							pnt;
						net_host_player_connect_type	connect;
					} net_host_player_type;

//
// network setup structures
//

typedef struct		{
						int								current_map_idx;
						char							name[name_str_len],
														ip_name[256],ip_resolve[64];
					} network_setup_host_type;

typedef struct		{
						int								latency,latency_ping_tick;
						net_address_type				host_addr;
					} network_setup_client_type;

typedef struct		{
						int								mode,game_idx,
														score_limit,option_flags,
														respawn_secs,game_reset_secs,
														next_net_uid;
						network_setup_host_type			host;
						network_setup_client_type		client;
					} network_setup_type;

//
// special remote unqiue IDs
//

#define net_uid_constant_host							0
#define net_uid_constant_client_start					1000
#define net_uid_constant_bot_start						2000
#define net_uid_constant_map_obj_start					3000

//
// messages actions
//

#define net_action_none									0
#define net_action_request_info							1
#define net_action_reply_info							2
#define net_action_request_join							3
#define net_action_reply_join							4
#define net_action_request_game_reset					5
#define net_action_request_remote_add					6
#define net_action_request_remote_remove				7
#define net_action_request_remote_death					8
#define net_action_request_remote_update				9
#define net_action_request_remote_telefrag				10
#define net_action_request_remote_chat					11
#define net_action_request_remote_sound					12
#define net_action_request_remote_fire					13
#define net_action_request_remote_click					14
#define net_action_request_remote_pickup				15
#define net_action_request_host_exit					16
#define net_action_request_object_synch					17
#define net_action_request_group_synch					18
#define net_action_reply_group_synch					19
#define net_action_request_game_score_limit				20

//
// remote fire types
//

#define net_remote_fire_type_projectile					0
#define net_remote_fire_type_hit_scan					1
#define net_remote_fire_type_melee						2

//
// network maximums
//

#define net_max_weapon_per_remote						16

//
// team definitions
//

#define net_team_none									0
#define net_team_red									1
#define net_team_blue									2
#define net_team_common									3

//
// flags
//

#define net_update_flag_hidden							0x00000001
#define net_update_flag_no_contact_object				0x00000002
#define net_update_flag_no_contact_projectile			0x00000004
#define net_update_flag_no_contact_force				0x00000008
#define net_update_flag_clickable						0x00000010
#define net_update_flag_pickup							0x00000020
#define net_update_flag_talking							0x00000040

#define net_group_synch_flag_on							0x00000001
#define net_group_synch_flag_freeze						0x00000002
#define net_group_synch_flag_main_move					0x00000004

//
// network headers and messages
//

typedef struct		{
						short							action,len;
					} network_header;

//
// info messages
//

typedef struct		{
						short							bot,score;
						char							name[name_str_len];
					} network_reply_info_player;

typedef struct		{
						short							count,max_count;
						network_reply_info_player		players[host_max_remote_count];
					} network_reply_info_player_list;

typedef struct		{
						int								option_flags;
						short							score_limit,respawn_secs,game_reset_secs;
						char							host_name[name_str_len],host_ip_resolve[16],
														proj_name[name_str_len],
														game_name[name_str_len],
														map_name[name_str_len];
						network_reply_info_player_list	player_list;
					} network_reply_info;

//
// joining messages
//

typedef struct		{
						int								hash;
						short							tint_color_idx;
						char							name[name_str_len],
														script_name[name_str_len],
														draw_name[name_str_len],
														vers[name_str_len];
					} network_request_join;

typedef struct		{
						int								map_tick;
						short							join_net_uid,team_idx;
						char							deny_reason[64];
					} network_reply_join;

//
// game reset messages
//

typedef struct		{
						char							map_name[name_str_len];
					} network_request_game_reset;

//
// in game messages
//

typedef struct		{
						short							add_net_uid,type,score,
														team_idx,tint_color_idx;
						char							name[name_str_len],
														script_name[name_str_len],
														draw_name[name_str_len];
					} network_request_remote_add;
					
typedef struct		{
						short							remove_net_uid;
					} network_request_remote_remove;

typedef struct		{
						short							death_net_uid,killer_net_uid,
														telefrag;
					} network_request_remote_death;

typedef struct		{
						int								model_tick;
						short							model_mode,
														model_animate_idx,model_animate_next_idx,model_pose_move_idx,
														model_smooth_animate_idx,model_smooth_pose_move_idx;
					} network_request_animation;			// used as part of network_request_update

typedef struct		{
						int								fp_mov_x,fp_mov_y,fp_mov_z,
														fp_rot_x,fp_rot_y,fp_rot_z,
														fp_resize;
						short							bone_idx;
					} network_request_dynamic_bone;			// used as part of network_request_update

typedef struct		{
						short							hidden,ammo_count,clip_count,
														alt_ammo_count,alt_clip_count;
					} network_request_weapon_ammo;			// used as part of network_request_update

typedef struct		{
						int								flags,pnt_x,pnt_y,pnt_z,
														fp_ang_x,fp_ang_y,fp_ang_z,
														fp_face_ang_x,fp_face_ang_y,fp_face_ang_z,
														fp_predict_move_x,fp_predict_move_y,fp_predict_move_z,
														fp_predict_turn_y,model_mesh_mask;
						short							update_net_uid,offset_x,offset_y,offset_z,
														score,health,armor,
														last_stand_mesh_idx;
						unsigned char					model_cur_texture_frame[max_model_texture];
						network_request_animation		animation[max_model_blend_animation];
						network_request_dynamic_bone	dynamic_bones[max_model_dynamic_bone];
						network_request_weapon_ammo		ammos[net_max_weapon_per_remote];
					} network_request_remote_update;
					
typedef struct		{
						short							chat_net_uid;
						char							str[64];
					} network_request_remote_chat;
					
typedef struct		{
						int								pnt_x,pnt_y,pnt_z,fp_pitch;
						char							name[name_str_len];
					} network_request_remote_sound;

typedef struct		{
						int								pt_x,pt_y,pt_z,
														fp_ang_x,fp_ang_y,fp_ang_z;
						short							fire_net_uid,fire_type,radius,distance,damage,force;
						char							weap_name[name_str_len],proj_setup_name[name_str_len];
					} network_request_remote_fire;

typedef struct		{
						short							picking_net_uid,picked_net_uid;
					} network_request_remote_pickup;

typedef struct		{
						short							clicking_net_uid,clicked_net_uid;
					} network_request_remote_click;

//
// map synch messages
//

typedef struct		{
						int								flags,
														fp_mov_add_x,fp_mov_add_y,fp_mov_add_z,
														fp_mov_accum_add_x,fp_mov_accum_add_y,fp_mov_accum_add_z,
														cuml_mov_add_x,cuml_mov_add_y,cuml_mov_add_z,
														fp_rot_add_x,fp_rot_add_y,fp_rot_add_z,
														fp_cuml_rot_add_x,fp_cuml_rot_add_y,fp_cuml_rot_add_z;
						short							group_idx,movement_idx,movement_move_idx,
														count,user_id;
					} network_reply_group_synch;
