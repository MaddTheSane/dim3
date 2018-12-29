/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Functions Header

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
// objects
//

extern void object_initialize_list(void);
extern void object_free_list(void);
extern int object_count_list(void);
int object_count_weapons(obj_type *obj);

extern void object_team_get_name(int team_idx,char *str);
extern void object_team_get_tint(int team_idx,d3col *tint);
extern void object_get_tint(obj_type *obj,d3col *tint);

extern bool object_networkable(obj_type *obj);

extern int object_create(char *name,int type,int bind);
extern int object_start(spot_type *spot,char *name,int type,int bind,char *err_str);
extern void object_dispose_single(int idx);
extern void object_dispose_2(int bind);
extern void object_dispose_all(void);

extern int object_script_count(void);
extern int object_script_spawn(char *name,char *script,char *params,d3pnt *pnt,d3ang *ang,bool hide,char *err_str);
extern bool object_script_remove(int idx,char *err_str);

extern inline int object_get_radius(obj_type *obj);
extern void object_set_current_mesh(obj_type *obj);
extern void object_set_position(obj_type *obj,int x,int y,int z,float ang_y,float ymove);
extern void object_stop(obj_type *obj);

extern bool object_start_script(obj_type *obj,bool no_construct,char *err_str);

extern void object_clear_size(obj_size *size);
extern void object_clear_position(d3pnt *pnt);
extern void object_clear_angle(d3ang *ang);
extern void object_clear_motion(obj_motion *motion);
extern void object_clear_movement(obj_movement *move);
extern void object_clear_force(obj_force *force);
extern void object_clear_contact(obj_contact *contact);
extern void object_clear_touch(obj_touch *touch);
extern void object_clear_hit(obj_hit *hit);
extern void object_clear_pickup(obj_pickup *pickup);
extern void object_clear_watch(obj_watch *watch);
extern void object_clear_remote(obj_remote *remote);
extern void object_clear_draw(model_draw *draw);

extern void object_reset_prepare(obj_type *obj);
extern void object_reset(obj_type *obj);

extern inline obj_type* object_get_attach(JSObjectRef j_obj);
extern obj_type* object_find_remote_net_uid(int net_uid);
extern obj_type* object_find_name(char *name);
extern obj_type* object_find_nearest(d3pnt *pnt,char *name,int type,int team_idx,float ang,float ang_sweep,int min_dist,int max_dist,bool player,int skip_obj_idx);
extern int object_find_nearest_list(d3pnt *pnt,int min_dist,int max_dist,int *ids);
extern int object_count_team(int team_idx,int ignore_obj_idx);
extern void object_set_even_team(obj_type *obj);
extern int object_find_idx_click_object(obj_type *obj);
extern int object_find_idx_by_stood_on_object_idx(int stand_obj_idx);
extern bool object_sight_test_object(obj_type *obj,int test_obj_idx);
extern int object_find_neareset_unheld_weapon_node(obj_type *obj,int from_node_idx);
extern int object_find_neareset_held_weapon_ammo_node(obj_type *obj,int from_node_idx,bool empty_only);
extern int object_find_neareset_armor_node(obj_type *obj,int from_node_idx);

extern void object_attach_click_crosshair_up(obj_type *obj);
extern void object_attach_click_crosshair_down(obj_type *obj);

extern void object_multiplayer_setup(obj_type *obj);
extern void object_multiplayer_setup_model_team_texture(obj_type *obj);

extern int object_choose_spawn_spot(obj_type *obj,char *err_str);
extern bool object_spawn(obj_type *obj,char *err_str);
extern int object_get_respawn_time(obj_type *obj);
extern void object_check_respawn(obj_type *obj);
extern int game_player_create(char *err_str);
extern bool game_multiplayer_bots_create(char *err_str);
extern bool map_objects_create(char *err_str);
extern bool map_object_attach_all(char *err_str);
extern void map_object_detach_all(void);

extern void object_score_recalc_place(void);
extern void object_score_update(obj_type *obj);
extern void object_score_death(obj_type *obj);
extern void object_score_goal(obj_type *obj);
extern void object_score_reset(obj_type *obj);
extern void object_death(obj_type *obj);
extern void object_telefrag(obj_type *obj,obj_type *source_obj);
extern bool object_telefrag_players(obj_type *obj,bool check_only);
extern void object_setup_touch(obj_type *obj,obj_type *source_obj,bool stand);
extern void object_touch(obj_type *obj);
extern void object_setup_hit(obj_type *obj,obj_type *from_obj,weapon_type *from_weap,proj_type *from_proj,d3pnt *melee_hit_pt,int damage);
extern void object_click(obj_type *obj,obj_type *from_obj);
extern void object_damage(obj_type *obj,obj_type *source_obj,weapon_type *source_weap,proj_type *source_proj,d3pnt *melee_hit_pt,int damage);
extern void object_melee_hit(obj_type *obj,obj_type *melee_obj,weapon_type *melee_weap,melee_type *melee,d3pnt *sphere_pnt);
extern void object_health_add(obj_type *obj,int value);
extern void object_health_remove(obj_type *obj,int value);
extern bool object_heal(obj_type *obj,int heal);
extern void object_health_recover(obj_type *obj);
extern bool object_heal_armor(obj_type *obj,int heal);
extern void object_crush(obj_type *obj,bool auto_crush);
extern void object_mesh_harm(obj_type *obj);
extern void object_hide(obj_type *obj,bool hide);
extern void object_input_freeze(obj_type *obj,bool freeze);
extern void object_push(obj_type *obj,d3pnt *pnt,int radius,int force,bool external_force);
extern void object_shove(obj_type *obj,d3ang *ang,float speed);
extern void object_shove_direct(obj_type *obj,d3vct *vct);
extern bool object_is_targetted(obj_type *obj,d3col *col);
extern bool object_set_radar_icon(obj_type *obj,char *err_str);

extern void object_slice_setup(obj_type *obj);
extern void object_movement(obj_type *obj,obj_movement *move);
extern void object_simple_movement(obj_type *obj,obj_movement *move);
extern void object_gravity(obj_type *obj);
extern void object_fix_force(obj_type *obj);
extern void object_alter_speed(obj_type *obj,float alt_speed);
extern void object_alter_gravity(obj_type *obj,float alt_gravity);
extern void object_fix_bump_smooth(obj_type *obj);
extern void object_motion_lock(obj_type *obj,d3pnt *motion);
extern void object_move_xz_bounce(obj_type *obj);
extern void object_move_y_up(obj_type *obj,int ymove);
extern void object_move_y_fall(obj_type *obj);
extern void object_move_y_down(obj_type *obj,int ymove);
extern void object_move_y(obj_type *obj,int ymove);
extern void object_move_y_fly(obj_type *obj,int ymove);
extern void object_move_fly(obj_type *obj);
extern void object_move_swim(obj_type *obj);
extern void object_move_normal(obj_type *obj);
extern void object_move(obj_type *obj);
extern void object_move_remote(obj_type *obj);
extern void object_thrust(obj_type *obj);
extern void object_move_start(obj_type *obj);
extern void object_move_stop(obj_type *obj);

extern void object_move_with_mesh(int mesh_idx,d3pnt *motion);
extern void object_rotate_with_mesh(int mesh_idx,float rot_y);
extern bool object_push_object_allowed(obj_type *obj,obj_type *push_obj);
extern bool object_push_with_object(obj_type *obj,d3pnt *motion);
extern void object_move_with_standing_object(obj_type *obj,d3pnt *motion,bool y_only);
extern void object_rotate_with_standing_object(obj_type *obj,float rot_y);

extern float object_liquid_alter_speed(obj_type *obj);
extern void object_liquid(obj_type *obj);

extern void object_rigid_body_reset_angle(obj_type *obj);
extern void object_fly_reset_angle(obj_type *obj);

extern float object_get_turn_speed(obj_type *obj);
extern float object_get_turn_motion(obj_type *obj);
extern void object_turn(obj_type *obj);
extern void object_turn_stop(obj_type *obj);
extern void object_fix_motion(obj_type *obj);
extern void object_face(obj_type *obj);
extern void object_face_object_start(obj_type *obj,obj_type *track_obj);
extern void object_face_stop(obj_type *obj);
extern void object_player_turn(obj_type *obj);
extern void object_player_turn_direct(obj_type *obj,float ang);
extern void object_player_turn_auto_aim(obj_type *obj);
extern void object_player_look(obj_type *obj);
extern void object_player_look_stop(obj_type *obj);
extern void object_player_look_fix_zoom(obj_type *obj,weapon_type *weap);
extern float object_player_look_constrain(obj_type *obj,weapon_type *weap,float ang_x);

extern void object_post_move_animation_event(obj_type *obj,int sub_event);
extern bool object_post_move_animation_event_change(obj_type *obj,int sub_event);
extern void object_post_turn_animation_event(obj_type *obj,int sub_event);
extern bool object_post_turn_animation_event_change(obj_type *obj,int sub_event);
extern void object_event_animations(obj_type *obj);
extern void object_start_jump(obj_type *obj);
extern void object_liquid_jump(obj_type *obj);
extern void object_start_stand(obj_type *obj);
extern void object_start_duck(obj_type *obj);
extern void object_ducking(obj_type *obj);
extern void object_set_ambient(obj_type *obj,char *name,float pitch);
extern void object_change_ambient_pitch(obj_type *obj,float pitch);
extern void object_clear_ambient(obj_type *obj);
extern bool object_enter_vehicle(obj_type *obj,int vehicle_idx,char *err_str);
extern bool object_exit_vehicle(obj_type *vehicle_obj,bool ignore_errors,char *err_str);

extern bool object_auto_walk_node_setup(obj_type *obj,int from_idx,int to_idx,bool skip_event,int event_id,char *err_str);
extern bool object_auto_walk_node_name_setup(obj_type *obj,char *start_node,char *end_node,int event_id,char *err_str);
extern bool object_auto_walk_object_setup(obj_type *obj,int uid,bool turn_only,char *err_str);
extern bool object_auto_walk_player_setup(obj_type *obj,bool turn_only,char *err_str);
extern void object_auto_walk_position_setup(obj_type *obj,d3pnt *pnt);
extern bool object_auto_walk_node_resume(obj_type *obj,char *err_str);
extern bool object_auto_walk_node_reverse(obj_type *obj,char *err_str);
extern void object_auto_walk_stop(obj_type *obj);
extern void object_auto_walk_dodge_setup(obj_type *obj,float dodge_angle,int dodge_msec);
extern bool object_auto_walk_get_seek_position(obj_type *obj,d3pnt *pnt);
extern void object_auto_walk(obj_type *obj);

extern void object_watch(obj_type *obj);
extern void object_watch_damage_alert(d3pnt *pnt,obj_type *damage_obj);
extern void object_watch_death_alert(obj_type *dead_obj);
extern void object_watch_base_alert(map_mesh_type *mesh,obj_type *enter_obj,bool entry);
extern void object_watch_sound_alert(d3pnt *pnt,int sound_obj_idx,char *sound_name);

extern void object_checkpoint(obj_type *obj);

extern int object_held_add(obj_type *obj,char *name,char *script,char *params,char *err_str);
extern bool object_held_drop(obj_type *obj,int uid,float y_ang,int y_change,char *err_str);

extern void object_grow_clear(obj_type *obj);
extern void object_grow_direct(obj_type *obj,float end_resize);
extern void object_grow_start(obj_type *obj,int grow_msec,float end_resize,d3pnt *end_size,d3pnt *end_offset);
extern void object_grow_run(obj_type *obj);

//
// items
//

extern bool item_pickup(obj_type *obj,obj_type *item_obj);
extern void item_pickup_check(obj_type *obj);
extern bool item_add_weapon(obj_type *obj,weapon_type *weap);
extern bool item_swap_weapon(obj_type *obj,weapon_type *weap);
extern bool item_add_ammo(obj_type *obj,weapon_type *weap,int add_count);
extern bool item_add_clip(obj_type *obj,weapon_type *weap,int add_count);
extern bool item_add_alt_ammo(obj_type *obj,weapon_type *weap,int add_count);
extern bool item_add_alt_clip(obj_type *obj,weapon_type *weap,int add_count);
extern bool item_add_health(obj_type *obj,int add_count);
extern bool item_add_armor(obj_type *obj,int add_count);
extern void item_add_custom(obj_type *obj,int custom_id);

//
// players
//

extern void player_clear_input(void);
extern void player_command_input(obj_type *obj);
extern void player_turn_input(obj_type *obj,float mouse_x);
extern void player_look_input(obj_type *obj,float mouse_y);
extern void player_jump_duck_input(obj_type *obj);
extern void player_weapon_zoom_input(obj_type *obj,weapon_type *weap);
extern void player_weapon_change_input(obj_type *obj,weapon_type *weap);
extern void player_weapon_fire_input(obj_type *obj,weapon_type *weap);
extern void player_weapon_input(obj_type *obj);
extern void player_enter_vehicle(obj_type *obj);
extern void player_exit_vehicle(obj_type *obj);
extern void player_enter_exit_input(obj_type *obj);
extern bool player_message_input(obj_type *obj);
extern void player_get_input(void);

//
// remotes
//

extern bool remote_add(network_request_remote_add *remote_add);
extern void remote_remove(network_request_remote_remove *remove);
extern bool remote_timed_out(obj_type *obj);
extern void remote_predict_move(obj_type *obj);
extern void remote_update_pack(obj_type *obj,bool chat_on,network_request_remote_update *update);
extern void remote_update_unpack(obj_type *obj,network_request_remote_update *update);
extern bool remote_route_message(net_queue_msg_type *msg);
extern void remote_network_send_updates(void);
extern void remote_network_send_group_synch(void);

//
// weapons
//

extern int weapon_count_projectile_setups(weapon_type *weap);

extern inline weapon_type* weapon_get_attach(JSObjectRef j_obj);
extern weapon_type* weapon_find_name(obj_type *obj,char *name);
extern inline weapon_type* weapon_find_current(obj_type *obj);

extern bool weapon_start_script(obj_type *obj,weapon_type *weap,bool no_construct,char *err_str);
extern bool weapon_add(obj_type *obj,char *name,char *err_str);
extern void weapon_dispose(obj_type *obj,int idx);

extern void weapon_attach_fire_crosshair(weapon_type *weap);
extern void weapon_attach_zoom_mask(weapon_type *weap);

extern void weapon_reset_ammo(weapon_type *weap);
extern void weapon_reset_ammo_object(obj_type *obj);
extern void weapon_set(obj_type *obj,weapon_type *weap);

extern void weapon_clear_state(obj_type *obj);

extern void weapon_lower(obj_type *obj);
extern void weapon_raise(obj_type *obj);
extern void weapon_held(obj_type *obj);
extern void weapon_goto(obj_type *obj,weapon_type *weap);
extern void weapon_switch(obj_type *obj,int dir);
extern void weapon_pick(obj_type *obj,int index);

extern void weapon_target_next_object(obj_type *obj,weapon_type *weap);
extern void weapon_target_previous_object(obj_type *obj,weapon_type *weap);
extern bool weapon_target_start(obj_type *obj,weapon_type *weap,bool opponent_only);
extern bool weapon_target_end(obj_type *obj,weapon_type *weap);

extern void weapon_zoom_enter(obj_type *obj,weapon_type *weap);
extern void weapon_zoom_exit(obj_type *obj,weapon_type *weap);
extern void weapon_zoom_off(obj_type *obj,weapon_type *weap);
extern void weapon_zoom_key(obj_type *obj,weapon_type *weap);

extern void weapon_player_run_hand(void);
extern void weapon_player_hand_bounce(void);
extern int weapon_get_bounce(obj_type *obj,weapon_type *weap);

extern void weapon_setup_fire(weapon_type *weap,int method);
extern bool weapon_add_projectile(obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,d3pnt *pt,d3ang *ang);
extern bool weapon_get_projectile_position_angle_weapon_model(obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,d3ang *fire_ang,char *err_str);
extern bool weapon_get_projectile_position_angle_weapon_barrel(obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,d3ang *fire_ang,char *err_str);
extern bool weapon_get_projectile_position_angle_object_model(obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,d3ang *fire_ang,char *err_str);
extern void weapon_get_projectile_position_center(obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,d3ang *fire_ang);
extern void weapon_get_projectile_position_angle_add_offset(d3ang *ang,d3ang *off_ang);
extern bool weapon_script_fire(obj_type *obj,weapon_type *weap,int method);
extern bool weapon_script_projectile_spawn(obj_type *obj,weapon_type *weap,char *proj_name,d3pnt *pt,d3ang *ang,int count,float slop,char *err_str);
extern bool weapon_script_projectile_spawn_weapon_model(obj_type *obj,weapon_type *weap,char *proj_name,int count,float slop,d3ang *off_ang,char *err_str);
extern bool weapon_script_projectile_spawn_weapon_barrel(obj_type *obj,weapon_type *weap,char *proj_name,int count,float slop,d3ang *off_ang,char *err_str);
extern bool weapon_script_projectile_spawn_object_model(obj_type *obj,weapon_type *weap,char *proj_name,int count,float slop,d3ang *off_ang,char *err_str);
extern bool weapon_script_projectile_spawn_center(obj_type *obj,weapon_type *weap,char *proj_name,int count,float slop,d3ang *off_ang,char *err_str);
extern void weapon_player_fire_down(obj_type *obj,weapon_type *weap,int method);
extern void weapon_player_fire_repeat(obj_type *obj,weapon_type *weap);
extern void weapon_player_fire_up(obj_type *obj,weapon_type *weap,int method);

extern void weapon_kickback(obj_type *obj,weapon_type *weap);
extern void weapon_recoil_clear(weapon_type *weap);
extern void weapon_recoil_copy(weapon_type *to_weap,weapon_type *from_weap);
extern void weapon_recoil_run(obj_type *obj,weapon_type *weap);
extern void weapon_recoil_add(obj_type *obj,weapon_type *weap,d3ang *ang);
extern void weapon_recoil(obj_type *obj,weapon_type *weap);

//
// melees
//

extern void melee_add(obj_type *obj,weapon_type *weap,d3pnt *pt,d3ang *ang,melee_type *melee,int ignore_obj_idx);
extern bool melee_strike_position_weapon_model(obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,char *err_str);
extern bool melee_strike_position_object_model(obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,char *err_str);
extern bool melee_strike_position_projectile_model(obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,proj_type *proj,d3pnt *fire_pnt,char *err_str);
extern bool melee_script_spawn_weapon_model(obj_type *obj,weapon_type *weap,char *err_str);
extern bool melee_script_spawn_object_model(obj_type *obj,weapon_type *weap,char *err_str);
extern bool melee_script_spawn_object_touch(obj_type *obj,char *err_str);
extern bool melee_script_spawn_projectile_model(obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,proj_type *proj,char *err_str);

//
// projectiles
//

extern bool projectile_initialize_list(void);
extern void projectile_free_list(void);
extern int projectile_count_list(void);

extern proj_type* projectile_create(obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,bool hit_scan);

extern inline int projectile_get_radius(proj_type *proj);
extern void projectile_set_position(proj_type *proj,d3pnt *pt,d3ang *ang);
extern void projectile_set_origin(proj_type *proj);
extern void projectile_spawn_position(proj_type *proj,d3pnt *pt,d3ang *ang,obj_type *parentobj);

extern void projectile_dispose(proj_type *proj);
extern void projectile_dispose_object(obj_type *obj);
extern void projectile_dispose_all(void);

extern proj_setup_type* find_proj_setups(weapon_type *weap,char *name);

extern bool proj_setup_start_script(obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,bool no_construct,char *err_str);
extern bool proj_setup_create(obj_type *obj,weapon_type *weap,char *name,char *err_str);
extern void proj_setup_dispose(weapon_type *weap,int idx);

extern proj_setup_type* proj_setup_get_attach(JSObjectRef j_obj);
extern proj_type* proj_get_attach(JSObjectRef j_obj);

extern void projectile_slice_setup(proj_type *proj);
extern void projectile_gravity(proj_type *proj);
extern void projectile_speed(proj_type *proj);
extern void projectile_move(proj_type *proj);
extern void projectile_turn_xz_towards(proj_type *proj,obj_type *to_obj,float turn_add);
extern void projectile_thrust_y_towards(proj_type *proj,obj_type *to_obj,float thrust_add);
extern void projectile_seek(proj_type *proj,obj_type *to_obj,float turn_add,float thrust_add);
extern bool projectile_bounce(proj_type *proj,float min_ymove,float reduce,bool send_event);
extern float projectile_reflect_angle(proj_type *proj);
extern void projectile_reflect(proj_type *proj,bool send_event);
extern void projectile_reset_angle_for_flight(proj_type *proj);
extern void projectile_eject_vector(proj_type *proj,d3vct *vct);
extern void projectile_reflect_vector(proj_type *proj,d3vct *vct);

extern void projectile_hit(proj_type *proj);
extern bool projectile_hit_auto(proj_type *proj);

extern void projectile_hitscan(obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,d3pnt *pt,d3ang *ang);

//
// scenery
//

extern void scenery_create(void);
extern void scenery_start(void);

//
// models
//

extern void model_initialize_list(void);
extern void model_free_list(void);

extern model_type* model_find(char *name);
extern int model_find_index(char *name);
extern bool model_draw_initialize(model_draw *draw,char *item_type,char *item_name,char *err_str);
extern void model_draw_free(model_draw *draw);
extern void model_rebuild_draw_memory(model_draw *draw);

extern void model_preload_start(void);
extern void model_preload_free(void);

extern void model_draw_setup_object(obj_type *obj);
extern void model_draw_setup_weapon(obj_type *obj,weapon_type *weap,bool ignore_y_shifts,bool dual_hand);
extern void model_draw_setup_projectile(proj_type *proj);
extern void model_draw_setup_interface_models(model_type *mdl,model_draw *draw,int x,int y);
extern void model_get_view_min_max(model_draw *draw,d3pnt *pnt,d3pnt *min,d3pnt *max);

extern void model_texture_change_frame(model_draw *draw,int txt_idx,int frame);
extern void model_texture_change_animation(model_draw *draw,int txt_idx,bool on,bool reverse);
extern void model_get_current_animation_name(model_draw *draw,char *name);
extern int model_find_animation_from_draw(model_draw *draw,char *name);
extern bool model_start_animation(model_draw *draw,char *name,int tick);
extern void model_stop_animation(model_draw *draw);
extern void model_stop_all_animation(model_draw *draw);
extern bool model_cancel_animation(model_draw *draw,char *name);
extern bool model_change_animation(model_draw *draw,char *name,int tick);
extern bool model_interrupt_animation(model_draw *draw,char *name,int tick);
extern void model_run_animation(model_draw *draw,int tick);
extern void model_calc_animation(model_draw *draw,int tick);
extern void model_calc_draw_bones(model_draw *draw);
extern int model_get_current_pose(model_draw *draw);
extern bool model_find_bone_offset(model_draw *draw,char *pose_name,char *bone_name,d3pnt *pnt);
extern bool model_find_bone_position(model_draw *draw,char *pose_name,char *bone_name,d3pnt *pnt);
extern bool model_get_last_draw_bone_position(model_draw *draw,int bone_idx,d3pnt *pnt);
extern bool model_get_bone_brightness(model_draw *draw,char *pose_name,char *bone_name,float *bright);

extern void model_animation_effect_setup(model_type *mdl);
extern void model_animation_effect_launch(model_draw *draw,int animate_idx,int pose_idx);

extern void model_fade_clear(model_draw *draw);
extern void model_fade_start(model_draw *draw,int fade_msec,float end_alpha);
extern void model_fade_run(model_draw *draw);
extern void model_mesh_fade_start(model_draw *draw,int mesh_idx,int fade_in_msec,int fade_life_msec,int fade_out_msec);
extern void model_mesh_fade_run(model_draw *draw);

extern int model_find_mesh_from_draw(model_draw *draw,char *name);
extern bool model_show_mesh(model_draw *draw,char *name);
extern bool model_show_all_mesh(model_draw *draw);
extern bool model_show_only_mesh(model_draw *draw,char *name);
extern bool model_hide_mesh(model_draw *draw,char *name);
extern bool model_hide_all_mesh(model_draw *draw);

extern bool model_dynamic_bone_set_rotate(model_draw *draw,char *bone_name,d3ang *rot,char *err_str);
extern bool model_dynamic_bone_set_move(model_draw *draw,char *bone_name,d3pnt *mov,char *err_str);
extern bool model_dynamic_bone_set_resize(model_draw *draw,char *bone_name,float resize,char *err_str);

extern void render_model_setup(model_draw *draw,int tick);
extern void render_model_build_vertex_lists(model_draw *draw,bool always_build);

//
// labels
//

extern void label_draw_setup(void);
extern void label_draw_render(void);
