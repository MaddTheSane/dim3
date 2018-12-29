/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Interface Functions

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
// main app
//

extern bool main_app_initialize(void);
extern void main_app_shutdown(void);
extern bool main_app_quit(void);

//
// main window
//

extern void main_wind_initialize(void);
extern void main_wind_shutdown(void);
extern void main_wind_menu_create(void);
extern void main_wind_menu_dispose(void);
extern void main_wind_menu_update(void);
extern bool main_wind_menu_event_run(int cmd);
extern void main_wind_set_title(void);
extern bool main_wind_switch_mode(int mode);
extern void main_wind_gl_setup(void);
extern void main_wind_timer(void);
extern void main_wind_draw(void);
extern void main_wind_draw_no_swap(void);
extern void main_wind_click(d3pnt *pnt,bool double_click);
extern void main_wind_scroll_wheel(d3pnt *pnt,int delta);
extern void main_wind_mouse_move(d3pnt *pnt);
extern bool main_wind_cursor(void);
extern void main_wind_key(char ch);
extern void main_wind_resize(void);

//
// setup
//

extern bool setup_xml_read(void);
extern bool setup_xml_write(void);

//
// menus
//

extern void project_menu_create(void);
extern void project_menu_dispose(void);
extern void project_menu_update(void);
extern bool project_menu_quit_save(void);
extern bool project_menu_event_run(int cmd);

extern void map_menu_create(void);
extern void map_menu_dispose(void);
extern void map_menu_update(void);
extern bool map_menu_event_run(int cmd);

extern void model_menu_create(void);
extern void model_menu_dispose(void);
extern void model_menu_update(void);
extern bool model_menu_event_run(int cmd);

//
// property utilities
//

extern void property_string_get_values(int value_type,void *value,char *str);
extern void property_string_get_title(int value_type,int i_min,int i_max,char *title);
extern void property_string_set_values(int value_type,void *value,int value_len,int i_min,int i_max,char *str);

extern bool property_chord_get_values(int value_type,void *value,char *str_x,char *str_y,char *str_z);
extern void property_chord_get_title(int value_type,char *title);
extern void property_chord_set_values(int value_type,void *value,char *str_x,char *str_y,char *str_z);

extern int property_pick_get_list_count(char *list_ptr,int list_item_sz,int list_name_offset);

extern int property_pick_file_fill_list(char *search_path,char *extension,char *required_file_name);
extern char* property_pick_file_get_list(void);
extern void property_pick_file(char *title,char *search_path,char *extension,char *required_file_name,char *file_name);

extern void property_get_parameter(int idx,char *param_list,char *str);
extern void property_set_parameter(int idx,char *param_list,char *str);

extern void dialog_property_string_run(int value_type,void *value,int value_len,int i_min,int i_max);
extern void dialog_property_chord_run(int value_type,void *value);

//
// text
//

extern void text_initialize(void);
extern void text_shutdown(void);
extern int text_width(float txt_size,char *str);
extern void text_draw(int x,int y,float txt_size,d3col *col,char *str);
extern void text_draw_center(int x,int y,float txt_size,d3col *col,char *str);
extern void text_draw_right(int x,int y,float txt_size,d3col *col,char *str);

//
// progress
//

extern void progress_start(char *title,int count);
extern void progress_end(void);
extern void progress_next(void);
extern void progress_next_title(char *title);

//
// list palette
//

extern void list_palette_initialize(void);
extern void list_palette_shutdown(void);
extern void list_palette_list_initialize(list_palette_type *list,char *title,bool left,bool never_hide_picker,bool never_open);
extern void list_palette_list_shutdown(list_palette_type *list);
extern int list_palette_width(list_palette_type *list);
extern void list_palette_total_box(list_palette_type *list,d3rect *box);
extern void list_palette_set_title(list_palette_type *list,char *title_0,char *name_0,char *title_1,char *name_1,char *title_2,char *name_2);
extern int list_palette_get_level(list_palette_type *list);
extern void list_palette_set_level(list_palette_type *list,int level);
extern bool list_palette_is_open(list_palette_type *list);
extern void list_palette_add_header(list_palette_type *list,int id,char *name);
extern void list_palette_add_header_count(list_palette_type *list,int id,char *name,int count);
extern void list_palette_add_header_button(list_palette_type *list,int id,char *name,int button_type);
extern void list_palette_add_item(list_palette_type *list,int id,int idx,char *name,bool selected,bool disabled);
extern void list_palette_add_color(list_palette_type *list,int id,int idx,d3col *col_ptr,bool selected,bool disabled);
extern void list_palette_add_string_selectable(list_palette_type *list,int id,char *name,char *str_ptr,int str_len,bool selected,bool disabled);
extern void list_palette_add_string_selectable_button(list_palette_type *list,int id,int button_type,int button_id,char *name,bool selected,bool disabled);
extern void list_palette_add_string_selectable_moveable_button(list_palette_type *list,int id,int button_type,int button_id,int button_up_id,int button_down_id,char *name,bool selected,bool disabled);
extern void list_palette_add_string(list_palette_type *list,int id,char *name,char *str_ptr,int str_len,bool disabled);
extern void list_palette_add_parameter(list_palette_type *list,int id,char *name,char *params,int param_idx,bool disabled);
extern void list_palette_add_na_blank(list_palette_type *list,int id,char *name);
extern void list_palette_add_int(list_palette_type *list,int id,char *name,int *int_ptr,bool disabled);
extern void list_palette_add_float(list_palette_type *list,int id,char *name,float *float_ptr,bool disabled);
extern void list_palette_add_checkbox(list_palette_type *list,int id,char *name,bool *bool_ptr,bool disabled);
extern void list_palette_add_pick_color(list_palette_type *list,int id,char *name,d3col *col_ptr,bool disabled);
extern void list_palette_add_point(list_palette_type *list,int id,char *name,d3pnt *pnt_ptr,bool disabled);
extern void list_palette_add_angle(list_palette_type *list,int id,char *name,d3ang *ang_ptr,bool disabled);
extern void list_palette_add_vector(list_palette_type *list,int id,char *name,d3vct *vct_ptr,bool disabled);
extern void list_palette_add_normal_vector(list_palette_type *list,int id,char *name,d3vct *vct_ptr,bool disabled);
extern void list_palette_add_uv(list_palette_type *list,int id,char *name,d3uv *uv_ptr,bool disabled);
extern void list_palette_add_picker_list_int(list_palette_type *list,int id,char *name,char *list_ptr,int list_count,int list_item_sz,int list_name_offset,bool include_none,bool sort,int *int_ptr,bool disabled);
extern void list_palette_add_picker_list_string(list_palette_type *list,int id,char *name,char *list_ptr,int list_count,int list_item_sz,int list_name_offset,bool include_none,bool sort,char *str_ptr,bool disabled);
extern void list_palette_add_picker_file(list_palette_type *list,int id,int button_type,int button_id,char *name,char *search_path,char *extension,char *required_file_name,char *str_ptr,bool disabled);
extern void list_palette_delete_all_items(list_palette_type *list);
extern void list_palette_start_picking_mode(list_palette_type *list,char *title,char *list_ptr,int list_count,int list_item_sz,int list_name_offset,bool include_none,bool file_list,bool sort,int *idx_ptr,char *name_ptr);
extern void list_palette_sort_mark_start(list_palette_type *list,list_palette_pane_type *pane);
extern void list_palette_sort(list_palette_type *list,list_palette_pane_type *pane);
extern void list_palette_draw(list_palette_type *list);
extern void list_palette_scroll_wheel(list_palette_type *list,d3pnt *pnt,int move);
extern void list_palette_scroll_item_into_view(list_palette_type *list,int item_id,int item_idx);
extern bool list_palette_click(list_palette_type *list,d3pnt *pnt,bool double_click);

//
// tool tips
//

extern void tool_tip_initialize(void);
extern void tool_tip_setup(d3pnt *pnt,char *str,bool right);
extern void tool_tip_clear(void);
extern void tool_tip_draw(void);

//
// tool palette
//

extern void tool_palette_initialize(tool_palette_type *tool_palette,int count,tool_palette_tool_type *tools);
extern void tool_palette_shutdown(tool_palette_type *tool_palette);
extern void tool_palette_box(d3rect *box);
extern void tool_palette_set_state(tool_palette_type *tool_palette,int idx,bool selected,bool disabled);
extern void tool_palette_draw(tool_palette_type *tool_palette);
extern int tool_palette_click(tool_palette_type *tool_palette,d3pnt *pnt);
extern void tool_palette_mouse_move(tool_palette_type *tool_palette,d3pnt *pnt);

//
// text palette
//

extern void texture_palette_draw(void);
extern void texture_palette_click(d3pnt *pnt,bool dbl_click);

//
// texture editing
//

extern void texture_edit_switch(int texture_idx);
extern void texture_edit_draw(void);
extern bool texture_edit_cursor(void);
extern void texture_edit_scroll_wheel(int delta);
extern bool texture_edit_click(d3pnt *pnt,bool double_click);

//
// import text decoder
//

extern int textdecode_count_linestarts(void);
extern void textdecode_linestarts(void);
extern bool textdecode_open(char *path,char p_piece_break);
extern void textdecode_close(void);
extern int textdecode_count(void);
extern int textdecode_find(int str_line,char *txt);
extern void textdecode_get_line(int i,char *txt);
extern void textdecode_get_piece(int i,int k,char *txt);

//
// tool palettes
//

extern void project_tool_palette_initialize(void);
extern void project_tool_palette_shutdown(void);
extern void project_tool_palette_set_state(void);
extern void project_tool_palette_click(int idx);
extern void project_tool_default(void);

extern void map_tool_palette_initialize(void);
extern void map_tool_palette_shutdown(void);
extern void map_tool_palette_set_state(void);
extern void map_tool_palette_click(int idx);
extern void map_tool_default(void);

extern void model_tool_palette_initialize(void);
extern void model_tool_palette_shutdown(void);
extern void model_tool_palette_set_state(void);
extern void model_tool_palette_click(int idx);
extern void model_tool_default(void);

//
// texture palette
//

extern int map_texture_palette_per_page_count(void);
extern int map_texture_palette_pixel_size(void);
extern int map_texture_palette_page_list_count(void);
extern int map_texture_palette_page_list_width(void);
extern void map_texture_palette_box(d3rect *box);
extern bool map_texture_palette_get_disabled_state(void);
extern int map_texture_palette_get_selected_texture(void);
extern void map_texture_palette_put_selected_texture(int txt_idx);
extern void map_texture_palette_reset(void);

extern int model_texture_palette_per_page_count(void);
extern int model_texture_palette_pixel_size(void);
extern int model_texture_palette_page_list_count(void);
extern int model_texture_palette_page_list_width(void);
extern void model_texture_palette_box(d3rect *box);
extern bool model_texture_palette_get_disabled_state(void);
extern int model_texture_palette_get_selected_texture(void);
extern void model_texture_palette_put_selected_texture(int txt_idx);
extern void model_texture_palette_reset(void);

//
// file list palette
//

extern void file_palette_initialize(void);
extern void file_palette_shutdown(void);
extern void file_palette_fill(void);
extern void file_palette_draw(void);
extern void file_palette_scroll_wheel(d3pnt *pnt,int move);
extern void file_palette_click(d3pnt *pnt,bool double_click);

//
// project property palettes
//

extern void project_palette_initialize(void);
extern void project_palette_shutdown(void);
extern void project_palette_draw(void);
extern void project_palette_reset(void);
extern void project_palette_scroll_wheel(d3pnt *pnt,int move);
extern bool project_palette_delete(void);
extern void project_palette_click(d3pnt *pnt,bool double_click);

extern void project_palette_fill_main(void);
extern void project_palette_fill_project(void);
extern void project_palette_fill_color(void);
extern void project_palette_fill_devices(void);
extern void project_palette_fill_logo(void);
extern void project_palette_fill_title_page(void);
extern void project_palette_fill_singleplayer(void);
extern void project_palette_fill_multiplayer(void);
extern void project_palette_fill_setup(void);
extern void project_palette_fill_progress(void);
extern void project_palette_fill_hud(void);
extern void project_palette_fill_virtual_control(void);
extern void project_palette_fill_radar(void);
extern void project_palette_fill_menus(void);
extern void project_palette_fill_choosers(void);
extern void project_palette_fill_sounds(void);
extern void project_palette_fill_particles(void);
extern void project_palette_fill_rings(void);
extern void project_palette_fill_halos(void);
extern void project_palette_fill_marks(void);
extern void project_palette_fill_labels(void);
extern void project_palette_fill_crosshairs(void);
extern void project_palette_fill_actions(void);
extern void project_palette_fill_shaders(void);
extern void project_palette_fill_preload_models(void);
extern void project_palette_fill_preferences(void);

extern void project_palette_click_main(bool double_click);
extern void project_palette_click_project(bool double_click);
extern void project_palette_click_color(bool double_click);
extern void project_palette_click_devices(bool double_click);
extern void project_palette_click_logo(bool double_click);
extern void project_palette_click_title_page(bool double_click);
extern void project_palette_click_singleplayer(bool double_click);
extern void project_palette_click_multiplayer(bool double_click);
extern void project_palette_click_setup(bool double_click);
extern void project_palette_click_progress(bool double_click);
extern void project_palette_click_hud(bool double_click);
extern void project_palette_click_virtual_control(bool double_click);
extern void project_palette_click_radar(bool double_click);
extern void project_palette_click_menus(bool double_click);
extern void project_palette_click_choosers(bool double_click);
extern void project_palette_click_sounds(bool double_click);
extern void project_palette_click_particles(bool double_click);
extern void project_palette_click_rings(bool double_click);
extern void project_palette_click_halos(bool double_click);
extern void project_palette_click_marks(bool double_click);
extern void project_palette_click_labels(bool double_click);
extern void project_palette_click_crosshairs(bool double_click);
extern void project_palette_click_actions(bool double_click);
extern void project_palette_click_shaders(bool double_click);
extern void project_palette_click_preload_models(bool double_click);
extern void project_palette_click_preferences(bool double_click);

extern void project_palette_fill_title_page_button(int intro_button_idx);
extern void project_palette_fill_title_page_model(int intro_model_idx);
extern void project_palette_fill_hud_bitmap(int hud_bitmap_idx);
extern void project_palette_fill_hud_text(int hud_text_idx);
extern void project_palette_fill_hud_bar(int hud_bar_idx);
extern void project_palette_fill_virtual_control_stick(int virtual_control_stick_idx);
extern void project_palette_fill_virtual_control_button(int virtual_control_button_idx);
extern void project_palette_fill_radar_icon(int radar_icon_idx);
extern void project_palette_fill_singleplayer_option(int singleplayer_option_idx);
extern void project_palette_fill_multiplayer_character(int multiplayer_character_idx);
extern void project_palette_fill_multiplayer_game(int multiplayer_game_idx);
extern void project_palette_fill_multiplayer_option(int multiplayer_option_idx);
extern void project_palette_fill_menu(int menu_idx);
extern void project_palette_fill_chooser(int chooser_idx);
extern void project_palette_fill_sound(int sound_idx);
extern void project_palette_fill_particle(int particle_idx);
extern void project_palette_fill_particle_group(int particle_idx);
extern void project_palette_fill_ring(int ring_idx);
extern void project_palette_fill_halo(int halo_idx);
extern void project_palette_fill_mark(int mark_idx);
extern void project_palette_fill_label(int label_idx);
extern void project_palette_fill_crosshair(int crosshair_idx);
extern void project_palette_fill_action(int action_idx);
extern void project_palette_fill_shader(int shader_idx);

extern void project_palette_click_title_page_button(int intro_button_idx,bool double_click);
extern void project_palette_click_title_page_model(int intro_model_idx,bool double_click);
extern void project_palette_click_hud_bitmap(int hud_bitmap_idx,bool double_click);
extern void project_palette_click_hud_text(int hud_text_idx,bool double_click);
extern void project_palette_click_hud_bar(int hud_bar_idx,bool double_click);
extern void project_palette_click_virtual_control_stick(int virtual_control_stick_idx,bool double_click);
extern void project_palette_click_virtual_control_button(int virtual_control_button_idx,bool double_click);
extern void project_palette_click_radar_icon(int radar_icon_idx,bool double_click);
extern void project_palette_click_singleplayer_option(int option_option_idx,bool double_click);
extern void project_palette_click_multiplayer_character(int multiplayer_character_idx,bool double_click);
extern void project_palette_click_multiplayer_game(int game_game_idx,bool double_click);
extern void project_palette_click_multiplayer_option(int option_option_idx,bool double_click);
extern void project_palette_click_menu(int menu_idx,bool double_click);
extern void project_palette_click_chooser(int chooser_idx,bool double_click);
extern void project_palette_click_sound(int sound_idx,bool double_click);
extern void project_palette_click_particle(int particle_idx,bool double_click);
extern void project_palette_click_particle_group(int particle_idx,bool double_click);
extern void project_palette_click_ring(int ring_idx,bool double_click);
extern void project_palette_click_halo(int halo_idx,bool double_click);
extern void project_palette_click_mark(int mark_idx,bool double_click);
extern void project_palette_click_label(int label_idx,bool double_click);
extern void project_palette_click_crosshair(int crosshair_idx,bool double_click);
extern void project_palette_click_action(int action_idx,bool double_click);
extern void project_palette_click_shader(int shader_idx,bool double_click);

extern void project_palette_fill_menu_item(int menu_idx,int menu_item_idx);
extern void project_palette_fill_chooser_piece(int chooser_idx,int chooser_item_idx);
extern void project_palette_fill_group_particle(int particle_idx,int group_particle_idx);

extern void project_palette_click_menu_item(int menu_idx,int menu_item_idx,bool double_click);
extern void project_palette_click_chooser_piece(int chooser_idx,int chooser_item_idx,bool double_click);
extern void project_palette_click_group_particle(int particle_idx,int group_particle_idx,bool double_click);

//
// map property palettes
//

extern void map_palette_fill_texture(int texture_idx);
extern void map_palette_fill_main(void);
extern void map_palette_fill_map(void);
extern void map_palette_fill_camera(void);
extern void map_palette_fill_light_media(void);
extern void map_palette_fill_sky_weather(void);
extern void map_palette_fill_cinema(int cinema_idx);
extern void map_palette_fill_group(int group_idx);
extern void map_palette_fill_movement(int movement_idx);
extern void map_palette_fill_mesh(int mesh_idx,int poly_idx);
extern void map_palette_fill_liquid(int liq_idx);
extern void map_palette_fill_spot(int spot_idx);
extern void map_palette_fill_light(int light_idx);
extern void map_palette_fill_sound(int sound_idx);
extern void map_palette_fill_particle(int particle_idx);
extern void map_palette_fill_scenery(int scenery_idx);
extern void map_palette_fill_node(int node_idx);

extern void map_palette_click_texture(int texture_idx,int id,bool double_click);
extern void map_palette_click_main(bool double_click);
extern void map_palette_click_map(bool double_click);
extern void map_palette_click_camera(bool double_click);
extern void map_palette_click_light_media(bool double_click);
extern void map_palette_click_sky_weather(bool double_click);
extern void map_palette_click_cinema(bool double_click);
extern void map_palette_click_group(bool double_click);
extern void map_palette_click_movement(bool double_click);
extern void map_palette_click_mesh(int mesh_idx,int poly_idx,bool double_click);
extern void map_palette_click_liquid(int liq_idx,bool double_click);
extern void map_palette_click_spot(int spot_idx,bool double_click);
extern void map_palette_click_light(int light_idx,bool double_click);
extern void map_palette_click_sound(int sound_idx,bool double_click);
extern void map_palette_click_particle(int particle_idx,bool double_click);
extern void map_palette_click_scenery(int scenery_idx,bool double_click);
extern void map_palette_click_node(int node_idx,bool double_click);

extern void map_palette_initialize(void);
extern void map_palette_shutdown(void);
extern void map_palette_draw(void);
extern void map_palette_reset(void);
extern void map_palette_scroll_into_view(int item_id,int item_idx);
extern void map_palette_scroll_wheel(d3pnt *pnt,int move);
extern bool map_palette_delete(void);
extern void map_palette_click(d3pnt *pnt,bool double_click);

extern void map_palette_fill_cinema_action(int cinema_idx,int action_idx);
extern void map_palette_fill_movement_move(int movement_idx,int move_idx);

extern void map_palette_click_cinema_action(int cinema_idx,int action_idx,bool double_click);
extern void map_palette_click_movement_move(int movement_idx,int move_idx,bool double_click);

//
// property palette
//

extern void model_palette_initialize(void);
extern void model_palette_shutdown(void);
extern void model_palette_draw(void);
extern void model_palette_reset(void);
extern void model_palette_scroll_into_view(int item_type,int item_idx);
extern void model_palette_scroll_wheel(d3pnt *pnt,int move);
extern bool model_palette_delete(void);
extern void model_palette_click(d3pnt *pnt,bool double_click);

extern void model_palette_fill_texture(int texture_idx);
extern void model_palette_fill_main(void);
extern void model_palette_fill_model(void);
extern void model_palette_fill_mesh(int mesh_idx);
extern void model_palette_fill_animation(int animate_idx);
extern void model_palette_fill_pose(int pose_idx);
extern void model_palette_fill_bone(int bone_idx,int pose_idx);
extern void model_palette_fill_hit_box(int hit_box_idx);

extern void model_palette_click_texture(int texture_idx,int id,bool double_click);
extern void model_palette_click_main(bool double_click);
extern void model_palette_click_model(bool double_click);
extern void model_palette_click_mesh(int mesh_idx,bool double_click);
extern void model_palette_click_animation(int animation_idx,bool double_click);
extern void model_palette_click_pose(int pose_idx,bool double_click);
extern void model_palette_click_bone(int bone_idx,int pose_idx,bool double_click);
extern void model_palette_click_hit_box(int hit_box_idx,bool double_click);

extern void model_palette_fill_animate_pose_move(int animate_idx,int pose_move_idx);
extern void model_palette_fill_pose_bone_move(int pose_idx,int bone_move_idx);

extern void model_palette_click_animate_pose_move(int animate_idx,int pose_move_idx,bool double_click);
extern void model_palette_click_pose_bone_move(int pose_idx,int bone_move_idx,bool double_click);

extern void model_palette_fill_animate_pose_move_particle(int animate_idx,int pose_move_idx,int particle_idx);
extern void model_palette_fill_animate_pose_move_ring(int animate_idx,int pose_move_idx,int ring_idx);

extern void model_palette_click_animate_pose_move_particle(int animate_idx,int pose_move_idx,int particle_idx,bool double_click);
extern void model_palette_click_animate_pose_move_ring(int animate_idx,int pose_move_idx,int ring_idx,bool double_click);

//
// dos
//

extern bool file_new_map(void);
extern bool file_open_map(char *file_name);
extern void file_save_map(void);
extern bool file_close_map(void);

//
// map undo
//

extern void map_undo_initialize(void);
extern void map_undo_clear(void);
extern void map_undo_push(void);
extern void map_undo_pull(void);


//
// launching
//

extern void launch_engine(void);
extern void launch_map_script_editor(void);
extern void launch_spot_script_editor(spot_type *spot);

//
// light maps
//

extern void light_map_texture_map_folder_name(char *map_name);
extern float light_map_ray_trace_triangle(d3pnt *spt,d3vct *vct,int *x,int *y,int *z);
extern bool light_maps_create(void);
extern void light_maps_clear(void);

//
// liquid reflection maps
//

extern bool liquid_reflection_maps_create(void);

//
// selection routines
//

extern void select_clear(void);
extern void select_add(int type,int main_idx,int sub_idx);
extern int select_count(void);
extern void select_get(int sel_idx,int *type,int *main_idx,int *sub_idx);
extern void select_switch(int sel_idx,int type,int main_idx,int sub_idx);
extern int select_find(int type,int main_idx,int sub_idx);
extern bool select_check(int type,int main_idx,int sub_idx);
extern bool select_has_type(int type);
extern void select_flip(int type,int main_idx,int sub_idx);
extern void select_remove_type(int type);
extern void select_add_group(int group_idx);
extern void select_add_movement(int movement_idx);
extern void select_add_cinema(int cinema_idx);
extern void select_delete_move_index(int type,int main_idx,int sub_idx);
extern void select_sort(void);
extern void select_duplicate_clear(void);
extern void select_duplicate_add(int type,int main_idx,int sub_idx);
extern void select_duplicate_backup(void);
extern void select_duplicate_copy(void);

//
// selection move routines
//

extern void select_get_extent(d3pnt *min,d3pnt *max);
extern void select_get_center(d3pnt *mid);
extern void select_get_angle(d3ang *ang);

//
// groups and movements
//

extern int group_count(int group_idx);
extern void group_set(int group_idx);
extern void group_clear(int group_idx,bool delete_shift);
extern int map_shift_movement_move(int movement_idx,int move_idx,int dir);

//
// piece routines
//

extern void piece_duplicate(void);
extern void piece_delete_face(void);
extern void piece_delete(void);
extern void piece_select_more(void);
extern void piece_tesselate(bool mesh);
extern void piece_distort(void);
extern void piece_resize(void);
extern void piece_force_grid(void);
extern void piece_flip(bool flip_x,bool flip_y,bool flip_z);
extern void piece_rotate(d3ang *ang);
extern void piece_free_rotate(void);
extern void piece_move(int move_x,int move_y,int move_z);
extern void piece_mesh_select_all_poly(void);
extern void piece_rotate_uvs(void);
extern void piece_flip_uvs(bool flip_u,bool flip_v);
extern void piece_reset_uvs(bool poly_only);
extern void piece_whole_uvs(bool poly_only);
extern void piece_single_uvs(bool poly_only);
extern void piece_mesh_recalc_normals(bool poly_only);
extern void piece_mesh_poly_invert_normals(map_mesh_poly_type *poly);
extern void piece_mesh_invert_normals(bool poly_only);
extern void piece_mesh_set_normals_in_out(bool out);
extern void piece_poly_hole(void);
extern void piece_liquid_reset_size(map_liquid_type *liq);
extern void piece_key(char ch);

//
// piece create routines
//

extern bool piece_create_texture_ok(void);
extern void piece_create_get_spot(d3pnt *pnt);
extern void piece_create_spot(void);
extern void piece_create_scenery(void);
extern void piece_create_light(void);
extern void piece_create_sound(void);
extern void piece_create_particle(void);
extern void piece_create_node(void);
extern void piece_create_liquid(void);
extern void piece_create_cinema(void);
extern void piece_create_group(void);
extern void piece_create_movement(void);

//
// piece mesh routines
//

extern void piece_add_obj_mesh(void);
extern void piece_add_obj_mesh_uv(void);
extern void piece_add_height_map_mesh(void);
extern void piece_add_grid_mesh(void);
extern void piece_add_polygon_mesh(void);
extern void piece_combine_mesh(void);
extern void piece_split_mesh(void);
extern void map_mesh_move_all(int x,int y,int z);
extern void map_mesh_center_all(void);
extern void map_mesh_reset_uv_all(void);

//
// map import routines
//

extern bool map_import_obj(char *path,char *err_str);

//
// node routines
//

extern bool node_link_click(int node_idx);
extern int node_duplicate_and_drag(int org_node_idx);
extern void node_path_rebuild(void);

//
// map view general
//

extern bool view_initialize(void);
extern void view_shutdown(void);
extern void view_setup_default_views(void);
extern void view_split_horizontal(void);
extern void view_split_vertical(void);
extern void view_split_remove(void);
extern void view_center_all(bool reset_ang);
extern void view_get_pixel_box(editor_view_type *view,d3rect *box);
extern void view_set_viewport_box(d3rect *box,bool erase,bool use_background);
extern void view_set_viewport(editor_view_type *view,bool erase,bool use_background);
extern float view_get_lookat_x_angle(editor_view_type *view);
extern void view_set_2D_projection(editor_view_type *view);
extern void view_set_3D_projection(editor_view_type *view,int near_z,int far_z,int near_z_offset);
extern void map_view_setup_project_point(void);
extern float map_view_project_point(editor_view_type *view,d3pnt *pnt);
extern bool map_view_project_point_in_z(d3pnt *pnt);
extern float map_view_project_get_depth(editor_view_type *view,d3pnt *pnt);
extern float map_view_project_get_depth_f(editor_view_type *view,d3fpnt *pnt);
extern editor_view_type* view_get_current_view(void);
extern bool view_point_in_view(editor_view_type *view,d3pnt *pnt);
extern bool map_view_cursor(d3pnt *pnt);
extern void view_key(char ch);
extern void view_get_position(d3pnt *pnt);
extern void view_set_position(d3pnt *pnt);
extern void view_set_position_y_shift(d3pnt *pnt,int y_shift);
extern void view_move_position(d3pnt *pnt);
extern void view_get_angle(d3ang *ang);
extern void view_set_angle(d3ang *ang);
extern void view_turn_angle(d3ang *ang);
extern void view_perspective_ortho(bool on);
extern void view_cull(bool on);
extern void view_clip(bool on);
extern void map_view_goto_select(void);
extern void map_view_goto_select_all(void);
extern void map_view_calculate_bounds(d3pnt *min_pnt,d3pnt *max_pnt);
extern void map_view_calculate_center(d3pnt *center_pnt);
extern void map_view_goto_map_center(editor_view_type *view);
extern void map_view_goto_map_center_all(void);
extern void map_view_reset_uv_layers(void);
extern void view_flip_clip(void);
extern int view_get_uv_layer(void);
extern void view_set_uv_layer(int uv_layer);
extern void view_no_rot(bool on);
extern void view_face_front(void);
extern void view_face_left(void);
extern void view_face_right(void);
extern void view_face_back(void);
extern void view_face_top(void);
extern void view_face_bottom(void);
extern void map_view_select_view(d3pnt *pnt);
extern void map_view_scroll_wheel(d3pnt *pnt,int delta);
extern bool map_view_click(d3pnt *pnt,bool double_click);
extern bool view_cull_poly(editor_view_type *view,map_mesh_type *mesh,map_mesh_poly_type *poly,int mn,int mk);
extern bool view_clip_poly(editor_view_type *view,map_mesh_poly_type *poly);
extern bool view_clip_liquid(editor_view_type *view,map_liquid_type *liq);
extern bool view_clip_point(editor_view_type *view,d3pnt *pnt);
extern void view_draw_create_mesh_sort_list(editor_view_type *view);
extern void view_draw_meshes_opaque(editor_view_type *view);
extern void view_draw_meshes_transparent(editor_view_type *view);
extern void view_draw_liquids(editor_view_type *view,bool opaque);
extern void view_draw_meshes_line(editor_view_type *view,bool opaque);
extern void view_draw_meshes_normals(editor_view_type *view);
extern void map_view_draw(void);

//
// auto-generate
//

extern void view_map_auto_generate_start(void);
extern bool map_view_auto_generate_cursor(d3pnt *pnt);
extern void map_view_auto_generate_scroll_wheel(d3pnt *pnt,int delta);
extern bool map_view_auto_generate_click(d3pnt *pnt,bool double_click);
extern void map_view_auto_generate_draw(void);

extern void view_map_auto_generate_textures(void);

extern void model_auto_generate_textures(void);

//
// view VBO
//

extern void view_vbo_mesh_rebuild(int mesh_idx);
extern void view_vbo_mesh_initialize(int mesh_idx);
extern void view_vbo_mesh_free(int mesh_idx);
extern void view_vbo_map_initialize(void);
extern void view_vbo_map_free(void);

//
// view draw piece
//

extern void view_cull_setup_frustum_clipping_planes(void);
extern bool view_cull_boundbox_in_frustum(d3pnt *min,d3pnt *max);
extern bool view_clip_poly(editor_view_type *view,map_mesh_poly_type *poly);
extern bool view_clip_liquid(editor_view_type *view,map_liquid_type *liq);
extern void map_view_draw_select_mesh_get_grow_handles(int mesh_idx,d3pnt *pts);
extern void map_view_draw_select_liquid_get_grow_handles(int liquid_idx,d3pnt *pts);
extern void map_view_draw_view(editor_view_type *view);
extern void map_view_draw_select(editor_view_type *view);

//
// view picking
//

extern bool view_pick_list_start(editor_view_type *view,bool in_2D,int count);
extern void view_pick_list_end(editor_view_type *view,d3pnt *pnt,int *type,int *main_idx,int *sub_idx);
extern bool view_pick_list_multiple_setup(editor_view_type *view);
extern int view_pick_list_multiple_pick(editor_view_type *view,d3pnt *start_pnt,d3pnt *end_pnt,view_picker_type *pick_list);
extern void view_pick_list_multiple_end(void);
extern void view_pick_list_add(int type,int main_idx,int sub_idx);
extern void view_pick_list_add_cube(d3pnt *v_pnts,int type,int main_idx,int sub_idx);
extern void view_pick_list_add_handle(d3pnt *pnt,int type,int main_idx,int sub_idx);
extern void view_pick_list_add_2D_handle(d3pnt *pnt,int type,int main_idx,int sub_idx);

//
// view click
//

extern int view_get_grid(void);
extern void view_click_grid(d3pnt *pt);
extern void view_force_grid(int mesh_idx,bool pos_only);
extern bool view_click_snap(int mesh_idx,int vertex_idx,int liquid_idx,d3pnt *pt);
extern bool view_click_snap_poly(int mesh_idx,int poly_idx,d3pnt *pt);
extern void view_click_snap_mesh(d3pnt *old_dpt,d3pnt *mpt);
extern void view_click_piece_map_pick_start(editor_view_type *view);
extern void view_click_piece(editor_view_type *view,d3pnt *pt,bool double_click);
extern void view_click_fix_cascade_size(int mesh_idx);
extern bool view_click_drag_mesh_handle(editor_view_type *view,d3pnt *pt);
extern bool view_click_drag_mesh(editor_view_type *view,d3pnt *pt);
extern bool view_click_drag_mesh_poly(editor_view_type *view,d3pnt *pt);
extern bool view_click_drag_vertex(editor_view_type *view,d3pnt *pt);
extern bool view_click_drag_texture_uv(editor_view_type *view,d3pnt *pt,bool entire_mesh);
extern bool view_click_drag_liquid_vertex(editor_view_type *view,d3pnt *pt);
extern bool view_click_drag_liquid(editor_view_type *view,d3pnt *pt);
extern bool view_click_drag_item(editor_view_type *view,d3pnt *pt);
extern bool view_click_box_select(editor_view_type *view,d3pnt *pt);

//
// view draw handle
//

extern bool view_handle_create_rot_handle(editor_view_type *view,d3pnt *pnt,d3ang *ang,d3pnt *center_pnt,d3pnt *hand_pnt);
extern void view_get_sprite_vertexes(d3pnt *pnt,d3ang *ang,d3pnt *v_pnts);

//
// view movement
//

extern void view_mouse_get_scroll_horizontal_axis(editor_view_type *view,d3pnt *pnt,int dist);
extern void view_mouse_get_scroll_vertical_axis(editor_view_type *view,d3pnt *pnt,int dist);
extern void view_mouse_scroll_movement(editor_view_type *view,d3pnt *pnt);
extern void view_mouse_forward_movement(editor_view_type *view,d3pnt *pnt);
extern void view_key_forward_movement(editor_view_type *view,int dir);
extern void view_key_side_movement(editor_view_type *view,int dir);
extern void view_key_vert_movement(editor_view_type *view,int dir);
extern void view_mouse_get_forward_axis(editor_view_type *view,d3pnt *pnt,int dist);
extern void view_mouse_turn(editor_view_type *view,d3pnt *pnt);

//
// view models
//

extern void map_view_models_start(void);
extern void map_view_models_close(void);
extern void map_view_models_reset(void);
extern void map_view_model_get_size(char *model_name,d3pnt *size);
extern bool map_view_model_draw(editor_view_type *view,d3pnt *pnt,d3ang *ang,char *name,float resize,int *texture_frame,int frame_count);
extern void map_view_model_cube_vertexes(char *name,d3pnt *pnt,d3ang *ang,float resize,d3pnt *v_pnts);

//
// map dialogs
//

extern bool dialog_new_map_run(char *file_name);
extern bool dialog_file_open_run(char *title,char *search_path,char *extension,char *required_file_name,char *file_name);
extern bool dialog_scale_run(d3fpnt *scale);
extern bool dialog_distort_run(d3pnt *mov_pnt);
extern bool dialog_reposition_run(d3pnt *min,d3pnt *max);
extern bool dialog_free_rotate_run(d3ang *ang);
extern bool dialog_grid_run(d3pnt *divs,bool *sides);
extern bool dialog_obj_import_run(int *import_mode,int *scale_axis,int *scale_unit,bool *force_grid);
extern bool dialog_height_import_run(int *div_cnt,int *size,int *high);

//
// model files
//

extern bool model_file_new(void);
extern bool model_file_open(char *file_name);
extern bool model_file_save(void);
extern bool model_file_close(void);
extern void model_file_import_mesh_obj(bool replace);
extern void model_file_insert_mesh_dim3_model(void);
extern void model_file_import_animations(void);

//
// model undo
//

extern void model_undo_initialize(void);
extern void model_undo_clear(void);
extern void model_undo_set_bone_move(int pose_idx,int bone_idx);
extern void model_undo_run(void);

//
// model pieces
//

extern void model_piece_add_mesh(void);
extern void model_piece_duplicate_mesh(int mesh_idx);
extern void model_piece_delete_mesh(int mesh_idx);
extern void model_piece_add_bone(void);
extern void model_piece_delete_bone(int bone_idx);
extern void model_piece_add_pose(void);
extern void model_piece_duplicate_pose(int pose_idx);
extern void model_piece_delete_pose(int pose_idx);
extern int model_shift_pose_index(int pose_idx,int dir);
extern void model_piece_add_animate(void);
extern void model_piece_duplicate_animate(int animate_idx);
extern void model_piece_delete_animate(int animate_idx);
extern void model_piece_add_animation_pose_move_particle(int animate_idx,int animate_pose_move_idx);
extern void model_piece_delete_animation_pose_move_particle(int animate_idx,int animate_pose_move_idx,int idx);
extern void model_piece_add_animation_pose_move_ring(int animate_idx,int animate_pose_move_idx);
extern void model_piece_delete_animation_pose_move_ring(int animate_idx,int animate_pose_move_idx,int idx);
extern int model_shift_animation_pose_move(int animate_idx,int pose_move_idx,int dir);
extern void model_piece_add_hit_box(void);
extern void model_piece_delete_hit_box(int hit_box_idx);

//
// model importing
//

extern bool model_import_obj(char *path,bool replace,bool *found_normals,char *err_str);
extern void model_insert_mesh(char *file_name);
extern void model_insert_animations(char *file_name);

extern int model_texture_count(void);
extern bool model_texture_exists(char *material_name);
extern int model_texture_pick(char *material_name,char *err_str);

//
// model clicking and drawing
//

extern void model_play(int play_mode);
extern void model_view_draw(void);
extern void model_wind_get_box(d3rect *box);
extern void model_wind_click(d3pnt *pnt,bool double_click);
extern void model_draw(int mesh_idx);
extern void model_draw_mesh(int mesh_idx);
extern void model_draw_bones(int sel_bone_idx);
extern void model_draw_bone_names(int sel_bone_idx);
extern void model_draw_bones_get_handle_rot(int bone_idx,d3ang *rot);
extern float model_draw_bones_drag_handle_offset(void);
extern void model_draw_bones_drag_handle_calc(d3fpnt *bone_pnt,d3vct *vct,d3ang *ang,d3fpnt *hand_pnt);
extern void model_draw_selected_vertexes(int mesh_idx);
extern void model_draw_selected_poly(int mesh_idx);
extern void model_draw_box_view(void);
extern void model_draw_box_hit_boxes(void);
extern void model_draw_axis(void);
extern void model_draw_normals(int mesh_idx);
extern void model_draw_gl_setup(int z_offset);
extern void model_draw_gl_setup_2D(void);
extern void model_draw_setup_project_point(void);
extern float model_draw_project_point(d3fpnt *pnt,d3pnt *pnt2d);
extern bool model_draw_project_point_in_z(d3fpnt *pnt);
extern float model_draw_project_get_depth(d3fpnt *pnt);
extern void model_draw_setup_bones_vertexes(int mesh_idx);
extern void model_draw_setup_pose(int pose_idx);
extern void model_draw_wind_pose(int mesh_idx,int pose_idx);
extern void model_draw_wind(int mesh_idx);

//
// model vertex masks
//

extern bool model_vertex_mask_initialize(void);
extern void model_vertex_mask_shutdown(void);
extern void model_vertex_mask_clear_sel(int mesh_idx);
extern void model_vertex_mask_set_sel(int mesh_idx,int vertex_idx,bool value);
extern void model_vertex_mask_set_sel_all(int mesh_idx);
extern bool model_vertex_mask_check_sel_any(int mesh_idx);
extern int model_vertex_mask_count_sel(int mesh_idx);
extern int model_vertex_mask_get_first_sel(int mesh_idx);
extern bool model_vertex_mask_check_sel(int mesh_idx,int vertex_idx);
extern void model_vertex_mask_clear_hide(int mesh_idx);
extern void model_vertex_mask_set_hide(int mesh_idx,int vertex_idx,bool value);
extern bool model_vertex_mask_check_hide(int mesh_idx,int vertex_idx);
extern void model_vertex_mask_hide_set_sel_vertexes(int mesh_idx);
extern void model_vertex_mask_hide_set_non_sel_vertexes(int mesh_idx);
extern void model_vertex_mask_hide_show_all_vertexes(int mesh_idx);
extern bool model_vertex_mask_check_hide_poly(int mesh_idx,model_poly_type *poly);
extern void model_vertex_mask_set_sel_bone(int mesh_idx,int bone_idx);
extern void model_vertex_mask_set_sel_has_bone(int mesh_idx);
extern void model_vertex_mask_set_sel_no_bone(int mesh_idx);
extern void model_vertex_mask_set_sel_near_bone(int mesh_idx,int bone_idx,float percentage);
extern void model_vertex_mask_set_sel_vertex_to_bone(int mesh_idx,int major_bone_idx,int minor_bone_idx,float factor);
extern void model_vertex_mask_set_sel_poly_mask(int mesh_idx);
extern void model_vertex_mask_set_sel_texture(int mesh_idx,int txt_idx);

//
// model poly masks
//

extern bool model_poly_mask_initialize(void);
extern void model_poly_mask_shutdown(void);
extern void model_poly_mask_clear_sel(int mesh_idx);
extern void model_poly_mask_set_sel(int mesh_idx,int poly_idx,bool value);
extern bool model_poly_mask_check_sel(int mesh_idx,int poly_idx);
extern int model_poly_mask_count_sel(int mesh_idx);
extern int model_poly_mask_get_first_sel(int mesh_idx);
extern void model_poly_mask_set_sel_texture(int mesh_idx,int txt_idx);
extern void model_poly_mask_clear_hide(int mesh_idx);
extern void model_poly_mask_set_hide(int mesh_idx,int poly_idx,bool value);
extern bool model_poly_mask_check_hide(int mesh_idx,int poly_idx);
extern model_poly_type* model_poly_mask_get_single_select(int mesh_idx);
extern void model_poly_mask_select_more(int mesh_idx);

//
// model vertex and poly calcs
//

extern void model_vertex_find_center_sel_vertexes(int mesh_idx,int *p_cx,int *p_cy,int *p_cz);
extern void model_vertex_invert_normals(int mesh_idx);
extern void model_vertex_set_normals(int mesh_idx);
extern void model_vertex_set_normals_in_out(int mesh_idx,bool out);
extern void model_vertex_clear_bone_attachments_sel_vertexes(int mesh_idx);
extern void model_vertex_auto_bone_attachments(int mesh_idx);
extern void model_vertex_delete_sel_vertex(int mesh_idx);
extern void model_vertex_delete_unused_vertexes(int mesh_idx);
extern void model_vertex_delete_sel_poly(int mesh_idx);
extern void model_vertex_collapse_selected(int mesh_idx);
extern void model_vertex_collapse_similar(int mesh_idx);
extern void model_polygon_make_quad(int mesh_idx);
extern void model_polygon_tessellate(int mesh_idx,bool sel_only);
extern void model_bone_attach_duplicate(int mesh_idx);
extern void model_clean_up(void);

//
// model picking
//

extern bool model_pick_list_start(int count);
extern void model_pick_list_end(d3pnt *pnt,int *idx);
extern void model_pick_list_add_poly(int idx,int ptsz,d3pnt *v_pnts);

//
// model dialogs
//

extern bool dialog_new_model_run(char *file_name);
extern bool dialog_file_open_run(char *dialog_name,char *search_path,char *extension,char *required_file_name,char *file_name);
extern int dialog_import_finish_run(void);
extern void dialog_set_vertex_bone_set_bone_combo(int id,int bone_idx);
extern int dialog_set_vertex_bone_get_bone_combo(int id);
extern bool dialog_set_vertex_bone_run(int *major_bone_idx,int *minor_bone_idx,float *factor);
extern bool dialog_bone_attach_duplicate_run(int *from_bone_idx,int *to_bone_idx,int *vertex_slop);
extern bool dialog_animation_scale_time_run(int animate_idx);
extern bool dialog_play_blend_animation_run(void);
extern bool dialog_set_normal_run(d3vct *normal);
