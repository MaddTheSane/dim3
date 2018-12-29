/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Server Definitions Header

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
// app states
//

#define as_active								0
#define as_inactive								1
#define as_suspended							2

//
// game states
//

#define gs_running								0
#define gs_intro								1
#define gs_singleplayer_option					2
#define gs_simple_save_pick						3
#define gs_setup_game							4
#define gs_join									5
#define gs_host									6
#define gs_file									7
#define gs_chooser								8
#define gs_title								9
#define gs_load_pause							10
#define gs_error								11
#define gs_score_limit							12

//
// lists
//

#define max_model_list							128

#define max_obj_list							1024
#define max_weap_list							16
#define max_proj_setup_list						8

#define max_proj_list							256

#define max_effect_list							512

//
// maximums
//

#define max_rain_density						5120
#define max_model_shader_light					4				// needs to reflect max_shader_light in view_defs.h

//
// binding types
//

#define bt_game									0
#define bt_map									1

//
// object types
//

#define object_type_player						0
#define object_type_bot_multiplayer				1
#define object_type_bot_map						2
#define object_type_object						3
#define object_type_remote_player				4
#define object_type_remote_object				5

//
// input modes
//

#define im_fpp									0
#define im_side_scroll							1
#define im_top_down								2
#define im_fly									3
#define im_thrust								4

//
// effect types
//

#define ef_flash								0
#define ef_particle								1
#define ef_ring									2
#define ef_lightning							3
#define ef_ray									4
#define ef_shake								5

//
// weapon modes
//

#define wm_held									0
#define wm_lower								1
#define wm_raise								2

//
// weapon crosshair types
//

#define ct_center								0
#define ct_bone_tracking						1
#define ct_bone_tracking_resizing				2
#define ct_barrel_tracking						3
#define ct_barrel_tracking_resizing				4

//
// ducking modes
//

#define dm_stand								0
#define dm_duck									1
#define dm_stand_up								2
#define dm_duck_down							3

//
// air modes
//

#define am_up									0
#define am_down									1
#define am_falling								2
#define am_ground								3

//
// liquid modes
//

#define lm_out									0
#define lm_in									1
#define lm_float								2
#define lm_under								3

//
// collision types
//

#define collision_mode_cylinder					0
#define collision_mode_box						1
#define collision_mode_simple					2

//
// animation modes
//

#define am_stopped								0
#define am_playing								1
#define am_resting								2

//
// auto walk modes
//

#define aw_none									0
#define aw_node									1
#define aw_object								2
#define aw_object_turn_only						3
#define aw_position								4

//
// node walking
//

#define node_pause_max_angle					45.0f
#define node_pause_min_angle					25.0f

//
// zoom modes
//

#define zoom_mode_off							0
#define zoom_mode_in							1
#define zoom_mode_on							2
#define zoom_mode_out							3

//
// ray collision values
//

#define collide_obj_ray_half_sweep				40.0f
#define collide_obj_ray_spindle_count			5
#define collide_obj_ray_count					(collide_obj_ray_spindle_count*5)
#define collide_obj_ray_spindle_size			((collide_obj_ray_half_sweep*2)/(collide_obj_ray_spindle_count-1))

//
// misc settings
//

#define pd_parent_grace							100					// how long till projectile can strike parent again
#define ci_object_click_angle					20.0f				// angle at which you can click an object

#define liquid_float_slop						0.1f				// how much object height is in float area

#define touch_retouch_msec_wait					250					// how long between touches counts as a single touch

#define map_collide_y_slop						150

#define crosshair_max_ray_trace_distance		60000

//
// model defines
//

#define max_model_dynamic_bone					4

//
// models
//
 
typedef struct		{
						int								tick,mode,
														animate_idx,animate_next_idx,pose_move_idx,
														smooth_animate_idx,smooth_pose_move_idx;
					} model_draw_animation;

typedef struct		{
						int								start_tick,tick;
						float							start_alpha,end_alpha;
						bool							on;
					} model_draw_fade;

typedef struct		{
						int								type,direction,intensity,
														rtl_light_id;
						float							exponent;
						bool							on;
						d3col							col;
					} model_draw_light;

typedef struct		{
						int								idx;
						bool							on;
					} model_draw_halo;
					
typedef struct		{
						int								bone_idx;
						float							resize;
						d3vct							mov,rot;
					} model_draw_dynamic_bone;

typedef struct		{
						int								obj_idx,weap_idx,proj_idx,
														net_uid;
						bool							net_sound,weap_in_dual;
						d3vct							motion_vct;
					} model_draw_connect;

typedef struct		{
						int								start_tick,fade_in_msec,fade_life_msec,fade_out_msec;
						float							alpha;
						bool							on;
					} model_draw_mesh_fade;

typedef struct		{
						int								frame;
						bool							opaque,transparent,glow;
					} model_draw_texture_type;

typedef struct		{
						int								rtl_mesh_id;
						bool							has_opaque,has_transparent,has_glow;
						float							alpha;
						model_draw_mesh_fade			fade;
						model_draw_texture_type			textures[max_model_texture];
					} model_draw_mesh_type;

typedef struct		{
						int								count,
														indexes[max_model_shader_light];
					} model_light_cache;

typedef struct		{
						int								frame;
						bool							animation_on,animation_reverse;
					} model_texture_type;
					
typedef struct		{
						int								model_idx,spin_tick,draw_dist,
														mesh_mask,render_mesh_mask,
														script_animation_idx,
														script_light_idx,script_halo_idx;
						char							name[name_str_len];
						float							alpha,resize,normal[3];
						bool							on,player,no_culling,
														flip_x,ui_lighting,
														bounce,face_forward,
														has_opaque,has_transparent,has_glow,
														built_vertex_list;
						d3pnt							pnt,size,center,offset;
						d3ang							rot,spin;
						model_draw_connect				connect;
						model_draw_light				lights[max_model_light];
						model_draw_halo					halos[max_model_halo];
						model_draw_animation			animations[max_model_blend_animation];
						model_draw_mesh_type			meshes[max_model_mesh];
						model_draw_dynamic_bone			dynamic_bones[max_model_dynamic_bone];
						model_draw_fade					fade;
						model_light_cache				light_cache;
						model_texture_type				textures[max_model_texture];
						model_draw_setup				setup;
					} model_draw;

//
// model list
//

typedef struct		{
						model_type*						models[max_model_list];
					} model_list_type;

//
// map polygon pointer
//

typedef struct		{
						int						mesh_idx,poly_idx;
					} poly_pointer_type;

//
// object sounds
//

typedef struct		{
						int						buffer_idx;
						char					name[name_str_len];
						float					pitch;
						bool					on;
					} obj_snd_ambient;

//
// object size
//

typedef struct		{
						int						x,z,y,
												eye_offset,weight,
												node_touch_dist;
					} obj_size;

typedef struct		{
						int						count,eye_offset_end;
						float					resize_end,resize_add,
												eye_offset_add,eye_offset_accum;
						bool					on,change_size,change_offset;
						d3pnt					size_end,offset_end;
						d3vct					size_add,size_accum,
												offset_add,offset_accum;
					} obj_grow;

//
// object hits and contacts
//

typedef struct		{
						int						value,max_value,start_value,
												recover_tick,recover_amount,recover_count;
						float					factor;
					} obj_health;

typedef struct		{
						int						liquid_under_tick,liquid_harm_count,liquid_drown_count,
												mesh_harm_count,respawn_tick;
						obj_health				health,armor;
					} obj_status;
			
typedef struct		{
						int						obj_idx,proj_idx,
												stand_obj_idx,head_obj_idx,
												liquid_idx,collision_mode;
						bool					object_on,projectile_on,force_on,
												melee,pushable;
						poly_pointer_type		hit_poly,stand_poly,head_poly;
					} obj_contact;
					
typedef struct		{
						int						obj_idx,last_obj_idx,
												last_touch_tick;
						bool					stand,last_stand;
						d3pnt					pnt;
						d3ang					ang;
					} obj_touch;
					
typedef struct		{
						int						obj_idx,weap_idx,proj_setup_idx,
												damage;
						char					hit_box_name[name_str_len];
						d3pnt					pnt;
						d3ang					ang;
					} obj_hit;
					
typedef struct		{
						int						obj_hit_box_idx,proj_hit_box_idx;
						bool					on;
					} obj_hit_box;
					
typedef struct		{
						int						distance,current_click_obj_idx,
												crosshair_up_idx,crosshair_down_idx;
						char					crosshair_up_name[name_str_len],
												crosshair_down_name[name_str_len];
						bool					on,click_crosshair_on;
					} obj_click;
					
typedef struct		{
						int						obj_idx,item_idx;
						bool					on,ignore,canceled;
					} obj_pickup;
					
typedef struct		{
						bool					on,crushable,invincible,
												in_damage;
					} obj_damage;
					
typedef struct		{
						int						method;
						char					name[name_str_len];
					} obj_weapon_fire;
					
//
// object sighting
//

typedef struct		{
						int						distance;
						float					side_angle,look_angle;
					} obj_sight;

//
// object positions
//

typedef struct		{
						int						last_y_change,rigid_body_offset_y;
						d3vct					vct;
						d3ang					ang;
					} obj_motion;

typedef struct		{
                        float					speed,
												max_crawl_speed,max_walk_speed,max_run_speed,max_air_speed,
												accelerate,decelerate,air_accelerate,air_decelerate,
												slop,seek_ang;
                        bool					moving,running,reverse,seeking;
                    } obj_movement;
					
typedef struct		{
						float					walk_speed,walk_motion_speed,
												run_speed,run_motion_speed,
												crawl_speed,crawl_motion_speed,
												air_speed,air_motion_speed,
												key_speed,top_down_ang_offset;
						bool					ignore_mouse,only_when_moving,restrict_player_turning;
						d3ang					ang_add,ang_to,fix_ang_add;
					} obj_turn;

typedef struct		{
						int						obj_idx;
						d3ang					ang;
					} obj_face;

typedef struct		{
						float					speed,max_speed;
						bool					drag;
						d3vct					vct;
					} obj_thrust;
						
typedef struct		{
						float					up_angle,down_angle,speed,
												ang_add;
						bool					effect_weapons;
					} obj_look;
					
typedef struct		{
						bool					x,y,z;
					} obj_lock;
					
typedef struct		{
						int						high,smooth_offset;
						float					smooth_factor;
						bool					on;
					} obj_bump;
					
typedef struct		{
						int						mesh_idx;
						float					factor;
					} obj_bounce;

typedef struct		{
						int						spawn_mesh_idx,
												last_stand_mesh_idx;
					} obj_mesh;

//
// jumping, ducking, climbing, falling, liquids
//

typedef struct		{
						int						mode,y_add,y_move,y_change;
						bool					on;
					} obj_duck;
					
typedef struct		{
						int						y_add;
						bool					on;
					} obj_jump;

typedef struct		{
						float					y_ang;
						bool					on,step_off_ready;
						poly_pointer_type		poly_ptr;
					} obj_climb;
					
typedef struct		{
						int						dist,damage_minimum_height;
						float					damage_factor;
						bool					started,land_event_ok;
					} obj_fall;

typedef struct		{
						int						mode,bob_y_move;
					} obj_liquid;

//
// auto-walk and watch
//
	
typedef struct		{
						int						end_tick;
						float					org_ang,dodge_ang;
						bool					on;
					} obj_auto_walk_dodge;
					
typedef struct		{
						int						mode,obj_idx,node_event_id,
												node_from_idx,node_dest_idx,
												node_seek_idx,node_last_seek_idx;
						bool					pause_for_turn,skip_event;
						d3pnt					pnt;
						obj_auto_walk_dodge		dodge;
					} obj_auto_walk;

typedef struct		{
						float					ang;
						bool					on,ray_trace;
					} obj_watch_restrict;
					
typedef struct		{
						int						dist,obj_idx,base_team;
						float					restrict_ang;
						char					obj_flags[max_obj_list],
												sound_name[name_str_len];
						bool					on;
						d3pnt					pnt;
						d3ang					ang;
						obj_watch_restrict		watch_restrict;
					} obj_watch;
		
//
// vehicles
//

typedef struct		{
						int						attach_obj_idx;
						bool					on,use_vehicles,
												in_enter,in_exit,
												offset_exit;
						d3pnt					attach_offset;
					} obj_vehicle;

//
// radar
//

typedef struct		{
						int						icon_idx,fade_start_tick;
						char					icon[name_str_len];
						bool					on,motion_only,always_visible;
					} obj_radar;

//
// forces
//

typedef struct		{
						float					gravity;
						d3vct					vct;
					} obj_force;

typedef struct		{
						int						size;
					} obj_kickback;

//
// remote players
//

typedef struct		{
						float					turn_y;
						d3pnt					move;
					} obj_remote_predict;
					
typedef struct		{
						int						net_uid,last_update;
						bool					talking;
						obj_remote_predict		predict;
					} obj_remote;

typedef struct		{
						int						kill,death,suicide,goal,
												score,place;
					} obj_score;

//
// labels
//

typedef struct		{
						bool					on;
						d3pnt					pnt;
					} obj_label_draw;

typedef struct		{
						char					str[32];
						obj_label_draw			draw;
					} obj_label_text;

typedef struct		{
						int						idx;
						obj_label_draw			draw;
					} obj_label_bitmap;

typedef struct		{
						float					value;
						obj_label_draw			draw;
					} obj_label_bar;

typedef struct		{
						obj_label_draw			draw;
					} obj_label_remote_name;

typedef struct		{
						obj_label_text			text;
						obj_label_bitmap		bitmap;
						obj_label_bar			bar;
						obj_label_remote_name	remote_name;
					} obj_label;

//
// scenery objects
//

typedef struct		{
						int						idx;
						bool					on;
					} obj_scenery;

//
// object weapons
//
 
typedef struct		{
						int						mode,swap_tick,bounce_y,
												current_idx,next_idx;
					} obj_held_weapon;
					
typedef struct		{
						int						x,y,sz,aim_obj_idx,
												alt_tick,gl_id;
						float					alpha;
						bool					on;
						d3col					color;
					} obj_crosshair_draw;
					
typedef struct		{
						int						x,y,old_camera_mode,start_tick;
						bool					on,sway_reset;
						d3ang					sway_ang,sway_next;
					} obj_zoom_draw;

//
// object misc
//

typedef struct		{
						int						mode;
						bool					freeze,respawn_freeze;
					} obj_input;

typedef struct		{
						char					name[name_str_len],script[file_str_len],
												params[param_str_len];
					} obj_spawn_spot;

typedef struct		{
						char					str[256];
					} obj_debug;

//
// weapon/project common structures
//
 
typedef struct		{
						int						radius,distance,damage,force;
						char					strike_bone_name[name_str_len],
												object_strike_bone_name[name_str_len],
												strike_pose_name[name_str_len],
												object_strike_pose_name[name_str_len];
						bool					fall_off;
					} melee_type;

//
// projectile setup/projectile common structures
//

typedef struct		{
						int						hit_tick;
						float					bounce_min_move,bounce_reduce;
						bool					bounce,reflect;
					} proj_action_type;

//
// projectile setup
//

typedef struct		{
						int						max_dist;
						bool					on;
					} proj_setup_hitscan_type;
					
typedef struct		{
						int						force;
						bool					on;
					} proj_setup_push_type;
					
typedef struct		{
						int						idx,script_idx,
												obj_idx,weap_idx,
												damage,decel_grace;
						char					name[name_str_len];
						float					speed,decel_speed,decel_min_speed,
												inherit_motion_factor;
						bool					collision,reset_angle;
						obj_size				size;
						model_draw				draw;
						melee_type				melee;
						proj_action_type		action;
						proj_setup_hitscan_type	hitscan;
						proj_setup_push_type	push;
					} proj_setup_type;

typedef struct		{
						proj_setup_type*		proj_setups[max_proj_setup_list];
					} proj_setup_list_type;

//
// weapons
//

typedef struct		{
						int						count,count_dual,
												init_count,max_count,
												clip_count,init_clip_count,max_clip_count,
												last_reload_tick,last_reload_dual_tick;
						bool					reset,use_ammo,use_clips;
					} weap_ammo_type;
					
typedef struct		{
						int						size,life_msec;
					} weap_kickback_type;

typedef struct		{
						float					look_reset,turn_reset;
						d3ang					ang,
												min_ang,max_ang,reset_ang;
					} weap_recoil_type;

typedef struct		{
						int						type,min_size,max_size,distance,fire_idx;
						char					fire_name[name_str_len],
												bone_name[name_str_len];
						bool					on;
						d3col					col,empty_col,pickup_col;
					} weap_crosshair_type;
					
typedef struct		{
						int						method,last_fire_tick,last_fire_dual_tick;
						bool					cancel;
					} weap_fire_type;

typedef struct		{
						int						distance,obj_idx;
						bool					on,opponent_only;
						d3col					col;
					} weap_target_type;
					
typedef struct		{
						int						mode,mask_idx,tick,step_count,current_step;
						float					fov_min,fov_max,sway_factor,crawl_sway_factor,
												turn_factor,crawl_turn_factor,look_factor;
						char					mask_name[name_str_len];
						bool					on,show_weapon;
					} weap_zoom_type;
					
typedef struct		{
						int						raise_tick,lower_tick,select_shift;
						float					fov_override,bounce_ang,bounce_speed;
						d3pnt					shift;
						d3ang					ang;
					} weap_hand_type;

typedef struct		{
						int						repeat_tick,next_repeat_tick;
						char					fire_pose_name[name_str_len],
												object_fire_pose_name[name_str_len],
												fire_bone_name[name_str_len],
												barrel_bone_name[name_str_len],
												object_fire_bone_name[name_str_len];
						bool					repeat_on,repeat_ok;
					} weap_proj_type;
					
typedef struct		{
						int						hand_offset;
						bool					on,active,in_dual;
					} weap_dual_type;
					
typedef struct		{
						int						idx,script_idx,obj_idx;
						char					name[name_str_len];
						bool					hidden,fail_in_liquid;
						model_draw				draw,draw_dual;
						melee_type				melee;
						weap_ammo_type			ammo,alt_ammo;
						weap_kickback_type		kickback;
						weap_recoil_type		recoil;
						weap_crosshair_type		crosshair;
						weap_fire_type			fire;
						weap_dual_type			dual;
						weap_target_type		target;
						weap_zoom_type			zoom;
						weap_hand_type			hand;
						weap_proj_type			proj;
						proj_setup_list_type	proj_setup_list;
					} weapon_type;

typedef struct		{
						weapon_type*			weaps[max_weap_list];
					} weapon_list_type;

//
// objects
//

typedef struct		{
						int						idx,script_idx,
												type,bind,next_spawn_sub_event,
												team_idx,tint_color_idx,character_idx,
												count,air_mode,damage_obj_idx,item_count,
												last_move_animation_event,last_turn_animation_event,
												last_spawn_spot_idx;
						char					name[name_str_len];
						bool					hidden,suspend,fly,slope_gravity,
												side_step,crawl,floating,single_speed,no_slide,
												open_doors,hide_all_weapons,
												find_on,in_collide_event,in_construct_event,
												quick_reverse,death_trigger,death_telefrag;
						d3pnt					pnt,org_pnt,last_pnt;
						d3ang					ang,view_ang,org_ang,last_ang;
						obj_motion				motion;
                        obj_movement			forward_move,side_move,vert_move;
						obj_lock				lock;
						obj_turn				turn;
						obj_face				face;
						obj_thrust				thrust;
						obj_mesh				mesh;
						obj_look				look;
						obj_size				size;
						obj_grow				grow;
						obj_sight				sight;
						obj_bump				bump;
						obj_bounce				bounce;
						obj_duck				duck;
						obj_jump				jump;
						obj_climb				climb;
						obj_fall				fall;
						obj_liquid				liquid;
						obj_auto_walk			auto_walk;
						obj_watch				watch;
						obj_vehicle				vehicle;
						obj_radar				radar;
						obj_force				force;
						obj_kickback			kickback;
						obj_status				status;
						obj_contact				contact;
						obj_touch				touch;
						obj_hit					hit;
						obj_hit_box				hit_box;
						obj_click				click;
						obj_pickup				pickup;
						obj_damage				damage;
						obj_input				input;
						obj_weapon_fire			weapon_fire;
						obj_held_weapon			held_weapon;
						obj_crosshair_draw		crosshair_draw;
						obj_zoom_draw			zoom_draw;
						melee_type				melee;
						model_draw				draw;
						obj_snd_ambient			ambient;
						obj_remote				remote;
						obj_score				score;
						obj_label				label;
						obj_scenery				scenery;
						obj_debug				debug;
						obj_spawn_spot			spawn_spot;
						weapon_list_type		weap_list;
					} obj_type;

typedef struct		{
						obj_type*				objs[max_obj_list];
					} obj_list_type;

//
// projectiles
//

typedef struct		{
						int						idx,script_idx,
												obj_idx,weap_idx,proj_setup_idx,
												count,parent_grace,decel_grace,
												start_tick;
						float					speed,decel_speed,decel_min_speed,gravity_add;
						bool					on,script_dispose,stick,reset_angle;
						d3pnt					pnt,org_pnt,last_pnt;
						d3ang					ang;
						obj_size				size;
						obj_motion				motion;
						obj_force				force;
						obj_contact				contact;
						model_draw				draw;
						proj_action_type		action;
					} proj_type;

typedef struct		{
						proj_type*				projs[max_proj_list];
					} proj_list_type;

//
// effects
//

typedef struct		{
						int						intensity,mid_tick;
						float					exponent;
						d3col					col;
					} flash_effect_data;

typedef struct		{
						bool					on;
						d3ang					ang;
					} particle_rotate;
	
typedef struct		{
						int						obj_idx,weap_idx,proj_idx,
												bone_idx;
						bool					weap_in_dual;
						d3vct					vct;
					} particle_motion;

typedef struct		{
						int						particle_idx,variation_idx;
						particle_rotate			rot;
						particle_motion			motion;
					} particle_effect_data;
					
typedef struct		{
						int						ring_idx;
						d3ang					ang;
					} ring_effect_data;
					
typedef struct		{
						int						wid,line_offset;
						float					varient;
						d3pnt					start_pnt,end_pnt;
						d3col					col;
					} lightning_effect_data;
					
typedef struct		{
						int						wid;
						d3pnt					start_pnt,end_pnt;
						d3col					col;
					} ray_effect_data;
					
typedef struct		{
						int						start_size,end_size;
						float					start_alpha,end_alpha;
						d3col					col;
					} globe_effect_data;
					
typedef struct		{
						int						distance;
					} shake_effect_data;
					
typedef union		{
						flash_effect_data		flash;
						particle_effect_data	particle;
						ring_effect_data		ring;
						lightning_effect_data	lightning;
						ray_effect_data			ray;
						globe_effect_data		globe;
						shake_effect_data		shake;
					} effect_data;
	
typedef struct		{
						int						effecttype,mesh_idx,size,
												start_tick,life_tick,
												rtl_mesh_id;
						bool					on;
						d3pnt					pnt;
						effect_data				data;
					} effect_type;

typedef struct		{
						effect_type*			effects[max_effect_list];
					} effect_list_type;

//
// ray trace structures
//

#define poly_ray_trace_origin_unknown					0
#define poly_ray_trace_origin_object					1
#define poly_ray_trace_origin_projectile				2
#define poly_ray_trace_origin_halo						3

#define ray_trace_face_hit_neg_z						0
#define ray_trace_face_hit_pos_z						1
#define ray_trace_face_hit_neg_x						2
#define ray_trace_face_hit_pos_x						3
#define ray_trace_face_hit_neg_y						4
#define ray_trace_face_hit_pos_y						5

#define ray_trace_max_check_item						512

#define ray_trace_check_item_object						0
#define ray_trace_check_item_projectile					1
#define ray_trace_check_item_mesh_poly					2
#define ray_trace_check_item_mesh_box					3


typedef struct		{
						int								idx,ignore_idx;
						bool							on;
					} ray_trace_obj_proj_type;

typedef struct		{
						int								origin;
						bool							hit;
						d3pnt							hpt;
						d3vct							array_vct;
						ray_trace_obj_proj_type			obj,proj;
						poly_pointer_type				poly;
					} ray_trace_contact_type;

typedef struct		{
						int								type,index,index_2;
					} ray_trace_check_item_type;

//
// map change structure
//

typedef struct		{
						bool							on,skip_media,player_restart;
					} server_map_change_type;
					
//
// timing structures
//

typedef struct		{
						int								run_tick,
														network_update_tick,
														network_group_synch_tick,
														map_start_tick;
						bool							paused;
					} server_time_type;

//
// Main Server Structure
//
 
 typedef struct		{
						int								state,next_state,last_state,
														skill,option_flags,
														simple_save_idx,player_obj_idx,
														checkpoint_spot_idx;
						bool							game_open,map_open;
						server_map_change_type			map_change;
						server_time_type				time;
						obj_list_type					obj_list;
						proj_list_type					proj_list;
						model_list_type					model_list;
						effect_list_type				effect_list;
					} server_type;


