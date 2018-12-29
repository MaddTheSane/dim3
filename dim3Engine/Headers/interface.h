/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Interface Functions Header

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
// app
//

extern bool app_start(char *err_str);
extern void app_end(void);
extern bool app_run_intro(char *err_str);
extern bool app_run_editor_launch(char *err_str);
extern bool app_run_dedicated_host(char *err_str);

//
// main loop
//

extern bool loop_main(char *err_str);
extern bool loop_main_dedicated(char *err_str);

//
// game
//

extern bool game_start(bool in_file_load,int skill,int option_flags,int simple_save_idx,char *err_str);
extern void game_end(void);
extern bool game_host_reset(char *err_str);
extern bool game_file_reload_ok(void);
extern bool game_file_reload(char *err_str);

//
// maps
//

extern void map_start_finish(bool skip_media);
extern bool map_start(bool in_file_load,bool skip_media,char *err_str);
extern void map_end(void);
extern void map_clear_changes(void);
extern bool map_need_rebuild(void);
extern bool map_rebuild_changes(char *err_str);
extern void map_start_ambient(void);
extern void map_end_ambient(void);
extern void map_clear_ambient(void);
extern void map_set_ambient(char *name,float pitch);

//
// server
//

extern bool server_initialize(char *err_str);
extern void server_shutdown(void);
extern bool server_game_start(bool in_file_load,int skill,int option_flags,int simple_save_idx,char *err_str);
extern void server_game_stop(void);

extern void server_run(void);

//
// view
//

extern bool view_initialize(char *err_str);
extern void view_shutdown(void);
extern bool view_game_start(char *err_str);
extern void view_game_stop(void);

extern bool view_file_paths_bitmap_check_wide(char *path,char *dir,char *name);

extern void view_loop_input(void);
extern void view_run(void);
extern void view_loop_draw(void);
extern void view_clear_fps(void);
extern void view_calculate_fps(void);

extern void view_game_reset_timing(void);

extern bool view_map_vbo_initialize(void);
extern void view_map_vbo_release(void);
extern void view_map_vbo_rebuild(void);

//
// timing
//

extern void game_time_initialize(void);
extern void game_time_calculate(void);
extern void game_time_reset(int tick);
extern int game_time_get_raw(void);
extern int game_time_get(void);
extern float game_time_fequency_second_get(int start_tick);
extern void game_time_pause_start(void);
extern void game_time_pause_end(void);

//
// save state
//

extern bool game_file_save(bool no_progress,bool suspend_save,char *err_str);
extern bool game_file_load(char *file_name,bool resume_load,char *err_str);
extern void game_file_suspend(void);
extern void game_file_resume(void);
extern void game_checkpoint_clear(void);
extern void game_checkpoint_set(int checkpoint_spot_idx);
extern void game_checkpoint_run(void);

//
// meshes
//

extern void mesh_triggers(obj_type *obj);

//
// cameras
//

extern void camera_initialize(void);
extern void camera_map_setup(void);
extern void camera_connect(obj_type *obj);
extern void camera_get_angle_from(d3pnt *pnt,d3ang *ang);
extern int camera_check_liquid(obj_type *obj,d3pnt *pnt);
extern void camera_save(void);
extern void camera_restore(void);
extern void camera_auto_turn_set_angle_offset(d3ang *ang,int life_msec);
extern void camera_auto_turn_set_chase_angle(d3ang *ang,int life_msec);
extern void camera_animate_start(float fov,float aspect_ration,int msec);
extern void camera_server_run(void);
extern void camera_view_draw_run(void);
extern void camera_fpp_calc_position(void);
extern void camera_chase_connect(void);
extern float camera_chase_run_slop(float ang,float goal,float slop,float chase_track_speed);
extern void camera_chase_run(void);
extern void camera_chase_calc_position(void);
extern void camera_chase_static_calc_position(void);
extern void camera_static_connect(void);
extern void camera_static_calc_position(void);
extern bool camera_walk_to_node_by_index_setup(int from_idx,int to_idx,int msec,int event_id,bool open_doors,bool in_freeze,char *err_str);
extern bool camera_walk_to_node_setup(char *start_node,char *end_node,int msec,int event_id,bool open_doors,bool in_freeze,char *err_str);
extern void camera_static_run(void);

//
// halos
//

extern void halo_draw_clear(void);
extern void halo_draw_add(d3pnt *pnt,int obj_idx,int halo_idx);
extern bool halo_draw_setup_cull(iface_halo_type *halo,int obj_idx,d3pnt *pnt,int *p_pixel_sz,float *p_alpha);
extern void halo_draw_setup(void);
extern void halo_draw_render(void);

//
// gui
//

extern void gui_initialize(char *background_path,char *bitmap_name);
extern void gui_shutdown(void);
extern void gui_draw_background(float alpha);
extern void gui_draw(float background_alpha,bool cursor);
extern void gui_draw_message(char *txt);
extern int gui_click(void);
extern int gui_keyboard(void);

//
// consoles
//

extern void console_initialize(void);
extern void console_remove_line(void);
extern void console_add_line(char *txt,d3col *col);
extern void console_add(char *txt);
extern void console_add_system(char *txt);
extern void console_add_error(char *txt);
extern void console_input(void);
extern void console_draw(void);
extern void console_input(void);

//
// cursors
//

extern void cursor_initialize(void);
extern void cursor_shutdown(void);
extern void cursor_draw(void);

//
// elements
//

extern void element_initialize(void);
extern void element_shutdown(void);
extern void element_clear(void);

extern int element_get_control_separation_high(void);
extern int element_get_padding(void);
extern int element_get_margin(void);
extern int element_get_tab_control_high(void);
extern int element_get_control_short_wid(void);
extern int element_get_control_long_wid(void);
extern int element_get_control_high(void);
extern void element_get_tab_button_bottom_left(int *x,int *y);
extern void element_get_tab_button_bottom_right(int *x,int *y);
extern void element_get_frame_button_bottom_right(int fx,int fy,int wid,int high,int *x,int *y);
extern int element_get_button_short_wid(void);
extern int element_get_button_long_wid(void);
extern int element_get_button_high(void);
extern int element_get_control_scroll_size(void);
extern int element_get_frame_title_high(void);

extern void element_button_text_add(char *name,int id,int x,int y,int wid,int high,int x_pos,int y_pos);
extern void element_button_bitmap_add(char *path,char *path2,int id,int x,int y,int wid,int high,int x_pos,int y_pos);
extern void element_button_box_add(int id,int x,int y,int wid,int high,int x_pos,int y_pos);
extern void element_bitmap_add(char *path,int id,int x,int y,int wid,int high,bool framed);
extern void element_text_add(char *str,int id,int x,int y,int size,int just,d3col *col,bool selectable);
extern void element_text_field_add(char *str,char *value_str,int max_value_str_sz,int id,int x,int y,bool selectable);
extern void element_number_add(char *str,int value,int id,int x,int y,int min,int max);
extern void element_checkbox_add(char *str,int value,int id,int x,int y,bool selectable);
extern void element_combo_add(char *str,char *combo_data,int value,int id,int x,int y,bool selectable);
extern void element_slider_add(char *str,float value,float value_min,float value_max,int id,int x,int y,bool selectable);
extern void element_table_add(element_column_type* cols,char *row_data,int id,int ncolumn,int x,int y,int wid,int high,bool checkbox,int bitmap_mode);
extern void element_tab_add(int id,int x,int y,int wid,int high,int tab_count,char *tabs);
extern void element_text_box_add(char *data,int id,int x,int y,int wid,int high,bool error_display);
extern void element_info_field_add(char *str,char *value_str,int id,int x,int y);
extern void element_model_add(char *name,char *animate,float resize,d3pnt *offset,d3ang *rot,int id,int x,int y);
extern void element_count_add(char *path,char *disable_path,int id,int x,int y,int wid,int high,int bitmap_add,bool horizontal,int wrap_count,int count,int max_count);
extern void element_frame_add(char *title,int id,int x,int y,int wid,int high,int tab_id,int tab_count,char *tabs,int button_count,element_frame_button_type *buttons);

extern int element_get_selected(void);
extern int element_get_value(int id);
extern float element_get_slider_value(int id);
extern void element_get_value_string(int id,char *str);
extern void element_set_value(int id,int value);
extern void element_set_slider_value(int id,float value);
extern void element_set_value_string(int id,char *str);
extern void element_enable(int id,bool enabled);
extern bool element_get_enable(int id);
extern void element_hide(int id,bool hide);
extern void element_table_busy(int id,bool busy);
extern void element_text_change(int id,char *str);
extern void element_set_table_data(int id,bool sort,char *row_data);
extern void element_set_bitmap(int id,char *path);
extern void element_enable_table_checkboxes(int id,bool enable);
extern void element_set_table_checkbox(int id,int idx,bool on);
extern bool element_get_table_checkbox(int id,int idx);
extern bool element_has_table_check(int id);
extern void element_get_table_column_data(int id,int row_idx,int col_idx,char *str,int str_len);
extern bool element_replace_model(int id,char *name,char *animate,float resize,d3pnt *offset,d3ang *rot);
extern bool element_model_show_mesh(int id,char *name);
extern bool element_model_hide_mesh(int id,char *name);

extern int element_get_scroll_position(int id);
extern void element_set_scroll_position(int id,int pos);
extern void element_make_selection_visible(int id);
extern int element_get_x_position(int id);
extern int element_get_y_position(int id);
extern void element_get_frame_inner_space(int id,int *x,int *y,int *wid,int *high);

extern int element_draw_item_under_cursor(void);
extern void element_draw(bool cursor_hilite);
extern void element_click_down(int x,int y);
extern int element_click_up(int x,int y);
extern void element_scroll_wheel(int x,int y,bool up);
extern int element_key(char ch);

//
// progress
//

extern void progress_initialize(char *map_name);
extern void progress_shutdown(void);
extern void progress_update(void);

//
// hud
//

extern void hud_texts_fps(char *data);
extern void hud_checkpoint_show(bool show);
extern void hud_click(void);
extern void hud_draw(void);
extern void network_draw(void);
extern void metrics_draw(void);

//
// virtual controls
//

extern void virtual_control_draw(void);

//
// radar
//

extern void radar_draw(void);

//
// crosshairs
//

extern void crosshair_show_alt(obj_type *obj);
extern bool crosshair_get_location(obj_type *obj,weapon_type *weap,int *kx,int *ky,int *hit_obj_idx,int *dist);
extern void crosshair_setup(obj_type *obj,weapon_type *weap);
extern void crosshair_draw(obj_type *obj,weapon_type *weap);
extern void zoom_setup(obj_type *obj,weapon_type *weap);
extern void zoom_draw(obj_type *obj,weapon_type *weap);

//
// collisions
//

extern void collide_setup(void);

extern bool collide_contact_is_wall_hit(poly_pointer_type *hit_poly);
extern bool collide_object_to_map(obj_type *obj,d3pnt *motion);
extern bool collide_object_to_map_bump(obj_type *obj,d3pnt *motion,int *bump_y_move);
extern bool collide_object_to_mesh(obj_type *obj,int mesh_idx);
extern bool collide_object_to_object(obj_type *obj,d3pnt *motion,obj_type *chk_obj,bool skip_pickup);
extern bool collide_object_to_sphere(d3pnt *sphere_pnt,int radius,obj_type *obj);
extern bool collide_object_check_obscured(obj_type *obj);

extern bool collide_projectile_to_map(proj_type *proj,d3pnt *motion);
extern bool collide_projectile_to_sphere(d3pnt *sphere_pnt,int radius,proj_type *proj);

extern void collide_objects_push(d3pnt *push_pnt,int radius,int force);

extern int collide_polygon_find_faced_by_object(obj_type *obj);
extern int collide_polygon_distance_to_object(int poly_uid,obj_type *obj);
extern void collide_polygon_hit_point_to_object(int poly_uid,obj_type *obj,d3pnt *pt);
extern void collide_polygon_get_normal(int poly_uid,d3vct *normal);
extern float collide_polygon_dot_product_to_object(int poly_uid,obj_type *obj);

extern int find_poly_nearest_stand(d3pnt *pnt,int my,bool ignore_higher);
extern void pin_build_trig_table(void);
extern int pin_downward_movement_point(d3pnt *pnt,int my,poly_pointer_type *stand_poly);
extern int pin_downward_movement_obj(obj_type *obj,bool set_obj_contact,int my);
extern int pin_downward_movement_proj(proj_type *proj,int my);
extern int pin_upward_movement_point(d3pnt *pnt,int my,poly_pointer_type *head_poly);
extern int pin_upward_movement_obj(obj_type *obj,int my);
extern int pin_upward_movement_proj(proj_type *proj,int my);
extern bool map_stand_crush_object(obj_type *obj);
extern bool map_stand_check_object(obj_type *obj);

//
// ray tracing
//

extern bool ray_trace_initialize(char *err_str);
extern void ray_trace_shutdown(void);
extern bool ray_trace_triangle_blocking_f(d3fpnt *spt,d3vct *vct,d3fpnt *tpt_0,d3fpnt *tpt_1,d3fpnt *tpt_2);
extern ray_trace_check_item_type* ray_trace_get_last_item_list(int *item_count);
extern void ray_push(d3pnt *pt,d3ang *ang,int dist);
extern void ray_push_to_end(d3pnt *pt,d3pnt *ept,int dist);
extern bool ray_trace_map_by_angle(d3pnt *spt,d3ang *ang,int dist,ray_trace_contact_type *contact);
extern bool ray_trace_map_by_point(d3pnt *spt,d3pnt *ept,ray_trace_contact_type *contact);
extern void ray_trace_map_by_point_array(int cnt,d3pnt *bounds_min,d3pnt *bounds_max,d3pnt *spts,d3pnt *epts,ray_trace_contact_type *base_contact,ray_trace_contact_type *contacts);
extern void ray_trace_shadow_to_mesh_poly(int cnt,d3vct *vct,d3fpnt *spt,d3fpnt *hpt,bool *hits,int mesh_idx,int poly_idx);

//
// effects
//

extern bool effect_initialize_list(void);
extern void effect_free_list(void);
extern int effect_count_list(void);
extern int effect_spawn(int effecttype,d3pnt *pt,int life_tick);
extern void effect_dispose(void);
extern void effect_object_bone_attach_particle_dispose(int obj_idx);
extern void effect_weapon_bone_attach_particle_dispose(int obj_idx,int weap_idx);
extern void effect_projectile_bone_attach_particle_dispose(int proj_idx);
extern bool effect_spawn_flash(d3pnt *pt,d3col *col,int intensity,float exponent,int flash_msec,int fade_msec);
extern bool effect_spawn_lightning(d3pnt *start_pt,d3pnt *end_pt,int wid,float varient,d3col *col,int life_msec);
extern bool effect_spawn_ray(d3pnt *start_pt,d3pnt *end_pt,int wid,d3col *col,int life_msec);
extern bool effect_spawn_shake(d3pnt *pt,int distance,int size,int life_msec);
extern void effect_draw_lightning(effect_type *effect);
extern void effect_draw_ray(effect_type *effect,int count);
extern void effect_image_animate_get_uv(int tick,int image_offset,iface_image_animation_type *animate,float *gx,float *gy,float *g_size);
extern void effect_draw_get_bound_box(effect_type *effect,d3pnt *min,d3pnt *max);
extern void effect_draw(void);
extern iface_particle_type* particle_find(char *name);
extern int particle_find_index(char *name);
extern void particle_precalculate(iface_particle_type *particle);
extern void particle_precalculate_all(void);
extern bool particle_spawn(int particle_idx,int obj_idx,d3pnt *pt,particle_rotate *rot,particle_motion *motion);
extern bool particle_line_spawn(int particle_idx,int obj_idx,d3pnt *start_pt,d3pnt *end_pt,int count);
extern bool particle_chain(effect_type *effect);
extern float particle_get_gravity(iface_particle_type *particle,int count);
extern void particle_map_initialize(void);
extern void particle_map_run(void);
extern void particle_draw(effect_type *effect,int count,int image_offset);
extern iface_ring_type* ring_find(char *name);
extern int ring_find_index(char *name);
extern bool ring_spawn(int ring_idx,int obj_idx,d3pnt *pt,d3ang *ang);
extern bool ring_line_spawn(int ring_idx,int obj_idx,d3pnt *start_pt,d3pnt *end_pt,int count);
extern void ring_draw(effect_type *effect,int count,int image_offset);
extern int mark_find(char *name);
extern bool decal_initialize_list(void);
extern void decal_free_list(void);
extern int decal_count_list(void);
extern void decal_move_with_mesh(int mesh_idx,d3pnt *motion);
extern void decal_rotate_with_mesh(int mesh_idx,float y);
extern void decal_add(int obj_idx,d3pnt *pnt,poly_pointer_type *poly_ptr,int mark_idx,int sz,float alpha);
extern void decal_dispose(void);

//
// groups
//

extern bool group_move_start(int group_idx,int movement_idx,int movement_move_idx,d3pnt *mov,d3ang *rot,int count,int user_id,int obj_idx,bool main_move);
extern void group_move_freeze(int group_idx,bool freeze);
extern bool group_move_frozen(int group_idx);
extern bool group_move_object_stand(int group_idx,int stand_mesh_idx);
extern void group_moves_run(bool run_events);
extern void group_moves_rebuild(void);
extern void group_moves_synch_with_client(int group_idx,network_reply_group_synch *synch);
extern void group_moves_synch_with_host(network_reply_group_synch *synch);

extern void group_show(int group_idx,bool show);
extern void group_solid(int group_idx,bool solid);
extern void group_texture(int group_idx,int index);
extern void group_texture_shift(int group_idx,float x_shift,float y_shift);

//
// movements
//

extern void map_movements_auto_open(void);
extern bool map_movement_next_move(int movement_idx,int move_idx,int obj_idx);
extern void map_movements_script_start(int obj_idx,int movement_idx,bool reverse);
extern void map_movements_script_start_or_thaw(int obj_idx,int movement_idx);
extern void map_movements_script_freeze(int movement_idx);
extern void map_movements_script_thaw(int movement_idx);
extern bool map_movements_script_is_looping(int movement_idx);

//
// gui screens
//

extern void menu_draw_start(char *name);

extern void intro_open(void);
extern void intro_close(void);
extern void intro_start_game(int skill,int option_flags,char *map_name,int simple_save_idx);
extern bool intro_click_has_singleplayer_options(int simple_save_idx);
extern void intro_run(void);

extern bool singleplayer_map_pick_on(int simple_save_idx);
extern bool singleplayer_option_on(int simple_save_idx,int idx);
extern int singleplayer_option_count(int simple_save_idx);
extern void singleplayer_option_open(void);
extern void singleplayer_option_close(void);
extern void singleplayer_option_run(void);

extern void simple_save_pick_open(void);
extern void simple_save_pick_close(void);
extern void simple_save_pick_run(void);

extern void setup_game_open(void);
extern void setup_game_close(void);
extern void setup_game_run(void);

extern void join_open(void);
extern void join_close(void);
extern void join_run(void);

extern void host_open(void);
extern void host_close(void);
extern void host_run(void);
extern void host_game_setup(void);

extern void file_open(void);
extern void file_close(void);
extern void file_setup(bool is_save);
extern void file_run(void);

extern void chooser_open(void);
extern void chooser_close(void);
extern bool chooser_setup(char *name,char *sub_txt,char *err_str);
extern void chooser_run(void);

extern void title_open(void);
extern void title_close(void);
extern bool title_setup(int exit_state,char *dir,char *name,char *sound_name,int life_tick,int event_id,char *err_str);
extern void title_run(void);

extern void load_pause_open(void);
extern void load_pause_close(void);
extern void load_pause_setup(char *map_name,bool skip_media);
extern void load_pause_run(void);

extern bool cinema_start(char *name,int event_id,char *err_str);
extern void cinema_run(void);

extern void error_open(void);
extern void error_close(void);
extern void error_setup(char *err_str,char *err_str_2);
extern void error_run(void);

extern void score_limit_check_scores(void);
extern void score_limit_open(void);
extern void score_limit_close(void);
extern int score_limit_get_resume_time(void);
extern int score_limit_get_resume_tick(void);
extern void score_limit_run(void);

//
// setup xml
//

extern void setup_xml_default(void);
extern int setup_find_control_for_name(char *name);
extern int setup_find_action_in_setup(int action_idx);
extern void setup_to_input(void);
extern void setup_xml_read_key_int(int setup_tag,char *name,int *value);
extern void setup_xml_read_key_float(int setup_tag,char *name,float *value);
extern void setup_xml_read_key_boolean(int setup_tag,char *name,bool *value);
extern bool setup_xml_read(void);
extern void setup_xml_write_key_int(char *name,int value);
extern void setup_xml_write_key_float(char *name,float value);
extern void setup_xml_write_key_boolean(char *name,bool value);
extern bool setup_xml_write(void);
extern void setup_restore(void);

//
// sounds
//

extern bool al_initialize(char *err_str);
extern void al_shutdown(void);
extern void al_set_volume(float sound_volume);

extern int al_open_buffer(char *name,char *path,int min_dist,int max_dist);
extern void al_close_buffer(int buffer_idx);
extern void al_close_all_buffers(void);
extern void al_load_all_xml_sounds(void);

extern int al_find_buffer(char *name);
extern int al_get_buffer_max_dist(int buffer_idx);

extern void al_set_listener(d3pnt *pnt,float ang_y);
extern int al_distance_to_listener(d3pnt *pnt);

extern int al_play_source(int buffer_idx,d3pnt *pnt,float pitch,bool loop,bool ambient,bool global,bool player);
extern void al_stop_source(int source_idx);

extern bool al_music_initialize(char *err_str);
extern void al_music_shutdown(void);
extern void al_music_map_pre_cache(void);
extern bool al_music_play(char *name,char *err_str);
extern void al_music_stop(void);
extern void al_music_set_loop(bool loop);
extern bool al_music_playing(void);
extern bool al_music_playing_is_name(char *name);
extern void al_music_set_volume(float music_volume);
extern void al_music_set_state(bool music_on);
extern bool al_music_fade_in(char *name,int msec,char *err_str);
extern void al_music_fade_out(int msec);
extern bool al_music_fade_out_fade_in(char *name,int fade_out_msec,int fade_in_msec,char *err_str);
extern bool al_music_cross_fade(char *name,int cross_msec,char *err_str);
extern void al_music_run(void);

extern void al_stop_all_sources(void);
extern void al_stop_all_looping_sources(void);

extern void al_ambient_list_clear(void);
extern void al_ambient_list_add(int buffer_idx,d3pnt *pnt,float pitch);
extern void al_ambients_run(void);

extern void map_restart_ambient(void);

//
// input
//

extern void input_initialize(bool in_window);
extern void input_shutdown(void);
extern void input_clear(void);
extern void input_set_key_start(void);
extern bool input_set_key_wait(char *name);
extern bool input_app_active(void);
extern bool input_event_pump(void);

extern void input_action_clear(void);
extern void input_action_attach(char *attach_name,int action_index);
extern bool input_check_action_same_attachment(int action_1_index,int action_2_index);
extern void input_action_set_touch_trigger_state(int action_idx,bool down,bool auto_up);
extern bool input_action_get_state(int action_index);
extern bool input_action_get_state_range(int start_action_index,int end_action_index);
extern bool input_action_get_state_single(int action_index);

extern void input_mouse_initialize(void);
extern void input_mouse_shutdown(void);
extern void input_mouse_clear(void);
extern void input_mouse_pause(void);
extern void input_mouse_resume(void);
extern void input_event_mouse_button(int button,bool down);
extern void input_event_mouse_motion(int x,int y);
extern void input_event_mouse_wheel(int y);
extern void input_get_mouse_movement(float *x,float *y);
extern bool input_get_mouse_button(int button);
extern void input_mouse_wheel_reset(void);
extern void input_mouse_gui_set_position(int x,int y);
extern void input_mouse_gui_get_position(int *x,int *y);
extern void input_mouse_gui_get_hilite_position(int *x,int *y);
extern bool input_mouse_gui_is_click_down(void);
extern bool input_mouse_gui_wheel_up(void);
extern bool input_mouse_gui_wheel_down(void);

extern void input_gui_set_position(int x,int y);
extern void input_gui_get_position(int *x,int *y);
extern void input_gui_get_hilite_position(int *x,int *y);
extern bool input_gui_is_click_down(void);
extern void input_gui_wait_click_up(void);

extern void input_keyboard_clear(void);
extern void input_event_key(int key_idx,bool down);
extern bool input_get_keyboard_escape(void);
extern bool input_get_keyboard_return(void);
extern bool input_get_keyboard_prev(void);
extern bool input_get_keyboard_next(void);
extern bool input_get_keyboard_key(int key_idx);
extern void input_clear_text_input(void);
extern char input_get_text_input_key(void);

extern bool input_joystick_initialize(void);
extern void input_joystick_shutdown(void);
extern bool input_check_joystick_ok(void);
extern void input_joystick_clear(void);
extern void input_event_joystick_button(int button,bool down);
extern float input_joystick_get_axis(int axis);
extern bool input_joystick_get_axis_as_button_min(int axis);
extern bool input_joystick_get_axis_as_button_max(int axis);
extern bool input_joystick_get_button(int button);

extern void input_touch_initialize(void);
extern void input_touch_shutdown(void);
extern bool input_touch_check_ok(void);
extern void input_touch_clear(void);
extern bool input_touch_get_state(int idx);
extern void input_touch_get_point(int idx,int *x,int *y);
extern void input_touch_event_up(int finger_id);
extern void input_touch_event_down(int touch_id,int finger_id,float x,float y);
extern void input_touch_event_move(int touch_id,int finger_id,float x,float y);
extern float input_touch_get_axis(int axis);
extern bool input_touch_get_axis_as_button_min(int axis);
extern bool input_touch_get_axis_as_button_max(int axis);
extern void input_touch_gui_get_position(int *x,int *y);
extern void input_touch_gui_get_hilite_position(int *x,int *y);
extern bool input_touch_gui_is_click_down(void);

//
// gl init and shutdown
//

extern bool gl_in_window_mode(void);
extern void gl_setup_context(void);
extern bool gl_initialize(int screen_wid,int screen_high,int fsaa_mode,char *err_str);
extern void gl_shutdown(void);

//
// gl drawables
//

extern bool gl_is_size_widescreen(int wid,int high);
extern bool gl_screen_shot(int lft_x,int top_y,int wid,int high,bool thumbnail,char *path);

//
// gl settings
//

extern bool gl_check_initialize(char *err_str);
extern bool gl_check_frame_buffer_ok(void);
extern bool gl_check_fsaa_ok(void);
extern bool gl_check_texture_generate_mipmaps_ok(void);
extern bool gl_check_npot_textures_ok(void);

//
// lights
//

extern void gl_lights_compile(int tick);
extern int gl_light_get_averaged_shadow_light(d3pnt *pnt,d3pnt *size,d3pnt *light_pnt);
extern void gl_lights_calc_ambient_color(d3col *col);
extern void gl_lights_calc_diffuse_vector(d3pnt *pnt,int count,int *indexes,d3vct *vct);
extern void gl_lights_calc_color(float x,float y,float z,float *cf);
extern void gl_lights_calc_color_light_cache_byte(int count,int *indexes,bool skip_light_map,float x,float y,float z,unsigned char *cp);
extern void gl_lights_calc_color_light_cache_float(int count,int *indexes,bool skip_light_map,float x,float y,float z,float *cp);
extern void gl_lights_build_mesh_glsl_light_list(map_mesh_type *mesh,view_glsl_light_list_type *light_list);
extern void gl_lights_build_poly_glsl_light_list(map_mesh_type *mesh,map_mesh_poly_type *poly,view_glsl_light_list_type *light_list);
extern void gl_lights_build_liquid_glsl_light_list(map_liquid_type *liq,view_glsl_light_list_type *light_list);
extern void gl_lights_build_model_glsl_light_list(model_type *mdl,model_draw *draw,view_glsl_light_list_type *light_list);

//
// vbos
//

extern void view_create_mesh_liquid_vertex_object(map_vbo_type *vbo,int vertex_count,int vertex_stride,int index_count);
extern void view_dispose_mesh_liquid_vertex_object(map_vbo_type *vbo);
extern void view_bind_mesh_liquid_vertex_object(map_vbo_type *vbo);
extern unsigned char* view_map_mesh_liquid_vertex_object(map_vbo_type *vbo);
extern void view_unmap_mesh_liquid_vertex_object(void);
extern void view_unbind_mesh_liquid_vertex_object(void);
extern void view_bind_mesh_liquid_index_object(map_vbo_type *vbo);
extern unsigned short* view_map_mesh_liquid_index_object(void);
extern void view_unmap_mesh_liquid_index_object(void);
extern void view_unbind_mesh_liquid_index_object(void);

extern void view_create_model_vertex_object(model_draw *draw);
extern void view_dispose_model_vertex_object(model_draw *draw);
extern void view_bind_model_vertex_object(model_draw *draw,int mesh_idx);
extern unsigned char* view_map_model_vertex_object(void);
extern void view_unmap_model_vertex_object(void);
extern void view_unbind_model_vertex_object(void);
extern void view_bind_model_shadow_vertex_object(model_draw *draw,int mesh_idx);
extern unsigned char* view_map_model_shadow_vertex_object(void);
extern void view_unmap_model_shadow_vertex_object(void);
extern void view_unbind_model_shadow_vertex_object(void);

extern void view_create_sky_vertex_object(int vertex_mem_sz);
extern void view_dispose_sky_vertex_object(void);
extern void view_bind_sky_vertex_object(void);
extern unsigned char* view_map_sky_vertex_object(void);
extern void view_unmap_sky_vertex_object(void);
extern void view_unbind_sky_vertex_object(void);

extern void view_create_fog_vertex_object(int vertex_mem_sz);
extern void view_dispose_fog_vertex_object(void);
extern void view_bind_fog_vertex_object(void);
extern unsigned char* view_map_fog_vertex_object(void);
extern void view_unmap_fog_vertex_object(void);
extern void view_unbind_fog_vertex_object(void);

extern void view_create_rain_vertex_object(int vertex_mem_sz);
extern void view_dispose_rain_vertex_object(void);
extern void view_bind_rain_vertex_object(void);
extern unsigned char* view_map_rain_vertex_object(void);
extern void view_unmap_rain_vertex_object(void);
extern void view_unbind_rain_vertex_object(void);

extern void view_clear_effect_vertex_object(effect_type *effect);
extern void view_create_effect_vertex_object(effect_type *effect,int vertex_mem_sz,int index_mem_sz);
extern void view_dispose_effect_vertex_object(effect_type *effect);
extern void view_bind_effect_vertex_object(effect_type *effect);
extern unsigned char* view_map_effect_vertex_object(void);
extern void view_unmap_effect_vertex_object(void);
extern void view_unbind_effect_vertex_object(void);
extern void view_bind_effect_index_object(effect_type *effect);
extern unsigned short* view_map_effect_index_object(void);
extern void view_unmap_effect_index_object(void);
extern void view_unbind_effect_index_object(void);

extern void view_create_text_vertex_object(void);
extern void view_dispose_text_vertex_object(void);
extern void view_bind_text_vertex_object(void);
extern unsigned char* view_map_text_vertex_object(void);
extern void view_unmap_text_vertex_object(void);
extern void view_unbind_text_vertex_object(void);

extern void view_initialize_utility_vertex_object(void);
extern void view_create_utility_vertex_object(void);
extern void view_dispose_utility_vertex_object(void);
extern void view_bind_utility_vertex_object(void);
extern unsigned char* view_map_utility_vertex_object(void);
extern void view_unmap_utility_vertex_object(void);
extern void view_unbind_utility_vertex_object(void);

extern void view_primitive_2D_tint_screen(d3col *col,float alpha);
extern void view_primitive_2D_color_poly(int x0,int y0,d3col *col0,int x1,int y1,d3col *col1,int x2,int y2,d3col *col2,int x3,int y3,d3col *col3,float alpha);
extern void view_primitive_2D_color_quad(d3col *col,float alpha,int lft,int rgt,int top,int bot);
extern void view_primitive_2D_color_trig(d3col *col,float alpha,int lft,int rgt,int top,int bot,int dir);
extern void view_primitive_2D_color_arc(d3col *out_col,d3col *in_col,float alpha,int lft,int rgt,int top,int bot,float start_perc,float end_perc);
extern void view_primitive_2D_line(d3col *col,float alpha,int x0,int y0,int x1,int y1);
extern void view_primitive_2D_line_poly(d3col *col,float alpha,int x0,int y0,int x1,int y1,int x2,int y2,int x3,int y3);
extern void view_primitive_2D_line_quad(d3col *col,float alpha,int lft,int rgt,int top,int bot);
extern void view_primitive_2D_line_trig(d3col *col,float alpha,int lft,int rgt,int top,int bot,int dir);
extern void view_primitive_3D_line(d3col *col,float alpha,int x0,int y0,int z0,int x1,int y1,int z1);
extern void view_primitive_3D_line_cube(d3col *col,float alpha,int *px,int *py,int *pz);
extern void view_primitive_2D_texture_quad(GLuint gl_id,d3col *col,float alpha,int lft,int rgt,int top,int bot,float gx,float gx2,float gy,float gy2,bool clamp);
extern void view_primitive_2D_texture_quad_rot(GLuint gl_id,d3col *col,float alpha,int lft,int rgt,int top,int bot,float ang,float gx,float gx2,float gy,float gy2);

//
// shaders
//

extern void gl_shader_initialize(void);

extern void gl_shader_code_clear(shader_type *shader);
extern bool gl_shader_code_compile(shader_type *shader,char *vertex_data,char *fragment_data,char *err_str);
extern void gl_shader_code_shutdown(shader_type *shader);

extern void gl_shader_attach_map(void);
extern void gl_shader_attach_model(model_type *mdl);

extern void gl_shader_set_scene_variables(shader_type *shader);

extern void gl_shader_set_draw_matrix_variables(shader_type *shader);
extern void gl_shader_set_light_variables(shader_type *shader,int core_shader_group,bool is_core,view_glsl_light_list_type *light_list);
extern void gl_shader_set_diffuse_variables(shader_type *shader,view_glsl_light_list_type *light_list);
extern void gl_shader_set_poly_variables(shader_type *shader,float alpha);
extern void gl_shader_hilite_override(shader_type *shader,view_glsl_light_list_type *light_list);
extern void gl_shader_set_texture(shader_type *shader,int core_shader_group,texture_type *texture,int txt_idx,int lmap_txt_idx,int frame);
extern void gl_shader_texture_override(GLuint gl_id,float alpha);

extern void gl_shader_frame_start(void);
extern void gl_shader_force_matrix_resets(void);

//
// shader execution
//

extern void gl_shader_draw_execute_simple_color_set_color(d3col *col,float alpha);
extern void gl_shader_draw_execute_simple_color_start(int vertex_size,int vertex_offset,d3col *col,float alpha);
extern void gl_shader_draw_execute_simple_color_end(void);

extern void gl_shader_draw_execute_simple_gradient_start(int vertex_size,int vertex_offset,int color_offset);
extern void gl_shader_draw_execute_simple_gradient_end(void);

extern void gl_shader_draw_execute_simple_black_start(int vertex_size,int vertex_offset,float alpha);
extern void gl_shader_draw_execute_simple_black_end(void);

extern void gl_shader_draw_execute_simple_bitmap_set_color(d3col *col,float alpha);
extern void gl_shader_draw_execute_simple_bitmap_set_texture(unsigned long gl_id);
extern void gl_shader_draw_execute_simple_bitmap_start(int vertex_size,int vertex_offset,int uv_offset,int stride,d3col *col,float alpha);
extern void gl_shader_draw_execute_simple_bitmap_end(void);

extern void gl_shader_draw_execute_reset_cached_offsets(void);

extern void gl_shader_draw_execute_map_start(texture_type *texture,int txt_idx,int frame,int lmap_txt_idx,float alpha,int vertex_offset,int uv_offset,int lmap_uv_offset,int tangent_offset,int normal_offset,int stride,view_glsl_light_list_type *light_list);
extern void gl_shader_draw_execute_map_end(texture_type *texture,view_glsl_light_list_type *light_list);

extern void gl_shader_draw_execute_liquid_start(texture_type *texture,int txt_idx,int frame,int lmap_txt_idx,float alpha,int vertex_offset,int uv_offset,int lmap_uv_offset,int tangent_offset,int normal_offset,int stride,view_glsl_light_list_type *light_list);
extern void gl_shader_draw_execute_liquid_end(texture_type *texture,view_glsl_light_list_type *light_list);

extern void gl_shader_draw_execute_model_start(texture_type *texture,int txt_idx,int frame,float alpha,int vertex_offset,int uv_offset,int tangent_offset,int normal_offset,int stride,view_glsl_light_list_type *light_list);
extern void gl_shader_draw_execute_model_end(texture_type *texture,view_glsl_light_list_type *light_list);

//
// core shaders
//

extern bool gl_core_shader_initialize(char *err_str);
extern void gl_core_shader_shutdown(void);
extern void gl_core_shader_build_generic_precision_defines(char *buf);
extern shader_type* gl_core_shader_find_ptr(int nlight,int core_shader_group,texture_type *texture);

//
// simple shaders
//

extern bool gl_simple_shader_initialize(char *err_str);
extern void gl_simple_shader_shutdown(void);

//
// user shaders
//

extern bool gl_user_shader_initialize(char *err_str);
extern void gl_user_shader_shutdown(void);
extern int gl_user_shader_find(char *name);

//
// full screen shaders
//

extern void gl_fs_shader_map_start(void);
extern void gl_fs_shader_map_end(void);
extern bool gl_fs_shader_start(char *shader_name,int life_msec,char *err_str);
extern void gl_fs_shader_end(void);
extern void gl_fs_shader_render_begin(void);
extern void gl_fs_shader_render_finish(void);

//
// back renderers
//

extern void gl_back_render_initialize(void);
extern void gl_back_render_shutdown(void);
extern void gl_back_render_map_start(void);
extern void gl_back_render_map_end(void);
extern void gl_back_render_frame_start(void);
extern bool gl_back_render_get_texture(char *node_name,GLuint *txt_id,float *alpha);

//
// opengl es patches
//

extern bool glu_patch_gluProject(float objx,float objy,float objz,float modelMatrix[16],float projMatrix[16],int viewport[4],float *winx,float *winy,float *winz);

//
// view
//

extern void gl_frame_clear(bool in_view);
extern void gl_frame_swap(void);

extern void gl_set_viewport(int x,int y,int wid,int high);
extern void gl_save_viewport(void);
extern void gl_restore_viewport(void);

extern void gl_3D_view(void);
extern void gl_3D_rotate(d3pnt *pnt,d3ang *ang);
extern void gl_2D_view_screen(void);
extern void gl_2D_view_interface(void);
extern void gl_3D_view_interface_model(void);

extern void gl_2D_scissor_start(int lx,int rx,int ty,int by);
extern void gl_2D_scissor_end(void);

extern bool gl_project_in_view_z(d3pnt *pnt);
extern bool gl_project_in_view_z_f(d3fpnt *f_pnt);
extern void gl_project_point(d3pnt *pnt);
extern void gl_project_point_f(d3pnt *pnt,d3fpnt *f_pnt);
extern float gl_project_get_depth(d3pnt *pnt);
extern float gl_project_get_depth_f(d3fpnt *f_pnt);
extern void gl_project_to_eye_coordinates(d3vct *vct);

//
// view culling
//

extern void view_cull_setup_frustum_clipping_planes(void);
extern int view_cull_distance_to_view_center(int x,int y,int z);
extern bool view_cull_mesh(map_mesh_type *mesh);
extern bool view_cull_liquid(map_liquid_type *liq);
extern bool view_cull_model(model_draw *draw);
extern bool view_cull_model_shadow(model_draw *draw);
extern bool view_cull_effect(effect_type *effect,d3pnt *center_pnt);
extern bool view_cull_halo(d3pnt *pnt);
extern void view_cull_draw_list_mesh_poly(void);

//
// map obscuring
//

extern bool view_obscure_initialize(void);
extern void view_obscure_release(void);
extern void view_obscure_run(void);

//
// textures
//

extern void gl_texture_initialize(void);
extern void gl_texture_shutdown(void);
extern void gl_texture_bind(int unit,GLuint txt_id);
extern void gl_texture_clear(int unit);

//
// text
//

extern void gl_text_initialize(void);
extern void gl_text_shutdown(void);
extern int gl_text_get_char_height(int text_size);
extern int gl_text_get_string_width(int text_font,int text_size,bool monospaced,char *str);
extern void gl_text_start(int text_font,int text_size,bool monospaced);
extern void gl_text_end(void);
extern void gl_text_draw(int x,int y,char *txt,int just,bool vcenter,d3col *col,float alpha);

//
// images
//

extern void view_images_initialize(void);
extern void view_images_shutdown(void);
extern int view_images_load_single(char *path,bool npot,bool simple);
extern void view_images_free_single(int idx);
extern void view_images_cached_load(void);
extern void view_images_cached_free(void);
extern bool view_images_is_empty(int idx);
extern bitmap_type* view_images_get_bitmap(int idx);
extern unsigned long view_images_get_gl_id(int idx);

//
// map rendering
//

extern void render_map_mesh_opaque(void);
extern void render_map_mesh_transparent(void);

//
// liquids
//

extern float liquid_tide_get_high(map_liquid_type *liq);
extern int liquid_wave_get_divisions(map_liquid_type *liq);
extern void render_map_liquid_opaque(void);
extern void render_map_liquid_transparent(void);

//
// shadows
//

extern bool shadow_get_volume(d3pnt *pnt,int high,d3pnt *light_pnt,int light_intensity,d3pnt *min,d3pnt *max);
extern void shadow_render_model_setup_lights(model_draw *draw);
extern void shadow_render_model(model_draw *draw);

//
// skies, backgrounds, fogs, rain
//

extern void sky_draw_init(void);
extern void sky_draw_release(void);
extern void sky_draw_background(void);
extern void sky_draw(void);

extern bool fog_solid_on(void);
extern void fog_draw_init(void);
extern void fog_draw_release(void);
extern void fog_draw_textured(void);

extern void rain_draw_init(void);
extern void rain_draw_release(void);
extern void rain_draw(void);

extern void view_draw_tint_start(d3col *col,float alpha,int fade_in_msec,int life_msec,int fade_out_msec);

//
// misc rendering
//

extern void draw_weapon_hand_setup(obj_type *obj,weapon_type *weap);
extern void draw_weapon_hand(obj_type *obj,weapon_type *weap);
extern void decal_render(void);

extern void particle_draw_position(effect_type *effect,int count,d3pnt *pnt);
extern void ring_draw_position(effect_type *effect,int count,d3pnt *pnt);

//
// chats
//

extern void chat_add_message(obj_type *obj,char *str,d3col *col);

//
// tints
//

extern void view_draw_liquid_tint(int liquid_idx);
extern void view_draw_effect_tint(void);

//
// fades
//

extern void view_fade_start(void);
extern void view_fade_cinema_fade_in_start(int msec);
extern void view_fade_cinema_fade_out_start(int msec);
extern void view_fade_cancel(void);
extern void view_fade_draw(void);

//
// debug
//

extern bool debug_change_map(char *name);


