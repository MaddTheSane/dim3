/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: External Header

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
// version
//

#define map_current_version									3

//
// map sizes
//

#define map_max_size										1000000			// maximum size of map on x/z units

//
// map maximums
//

#define max_mesh											8192			// maximum number of meshes in a map
#define max_liquid											256				// maximum number of liquids in a a map

#define max_map_texture										256				// maximum number of textures in a map

#define max_map_scenery										1024			// maximum number of ambient scenery
#define max_map_scenery_model_texture_frame					8				// maximum number of changable texture frames

#define max_map_light										256				// maximum number of ambient lights
#define max_map_sound										256				// maximum number of ambient sounds
#define max_map_particle									256				// maximum number of ambient particles

#define max_spot											512				// maximum number of spot in a map
#define max_node											512				// maximum number of nodes in a map
#define max_node_link										16				// maximum number of connected nodes

#define max_sort_poly										2048			// maximum number of transparent polys in a single scene

#define max_editor_view										8

#define max_music_preload									5

//
// lights
//

#define max_map_light_cache_index							32
#define max_map_mesh_size_lighting_small					50000

//
// strings
//

#define param_str_len										256				// length of parameter string

//
// physics
//

#define floor_slop											128				// how close to floor to be "standing" on it

#define gravity_start_power									0				// starting gravity
#define gravity_factor										5000			// factor to divide gravity out by when combing with weight

#define proj_reflect_slope_max_y							0.5f			// how big a slope before bounces become reflects
#define proj_bounce_min_speed								10.0f			// minimum speed before bounces cancel

#define slope_angle_to_slope								0.0065f			// multiply by this to get the slope from an angle

//
// lights
//

#define lt_normal											0
#define lt_blink											1
#define lt_glow												2
#define lt_pulse											3
#define lt_flicker											4
#define lt_failing											5

#define ld_all												0
#define ld_neg_x											1
#define ld_pos_x											2
#define ld_neg_y											3
#define ld_pos_y											4
#define ld_neg_z											5
#define ld_pos_z											6

//
// media types
//

#define mi_none												0
#define mi_chooser											1
#define mi_title											2
#define mi_cinema											3

//
// sky types
//

#define st_dome_panoramic									0
#define st_dome_hemisphere									1
#define st_cube												2

//
// camera modes
//

#define cv_fpp												0
#define cv_chase											1
#define cv_static											2
#define cv_chase_static										3

//
// input modes
//

#define im_fpp												0
#define im_side_scroll										1
#define im_top_down											2
#define im_fly												3
#define im_thrust											4

//
// group types
//

#define group_type_mesh										0
#define group_type_liquid									1

//
// spot types
//

#define spot_type_object									0
#define spot_type_bot										1
#define spot_type_player									2
#define spot_type_spawn										3
#define spot_type_checkpoint								4

//
// skills
//

#define skill_easy											0
#define skill_medium										1
#define skill_hard											2

//
// spawn play types
//

#define spawn_always										0
#define spawn_single_player_only							1
#define spawn_multiplayer_only								2

//
// mesh hide types
//

#define mesh_hide_mode_never								0
#define mesh_hide_mode_single_player						1
#define mesh_hide_mode_multiplayer							2

//
// cinema types
//

#define cinema_action_none									0
#define cinema_action_place									1
#define cinema_action_move									2
#define cinema_action_stop									3
#define cinema_action_show									4
#define cinema_action_show_fade								5
#define cinema_action_hide									6
#define cinema_action_fade_in								7
#define cinema_action_fade_out								8

#define cinema_actor_camera									0
#define cinema_actor_player									1
#define cinema_actor_object									2
#define cinema_actor_movement								3
#define cinema_actor_particle								4
#define cinema_actor_sound									5
#define cinema_actor_hud_text								6
#define cinema_actor_hud_bitmap								7

//
// shared vbo and light cache
//

typedef struct		{
						int									vertex_count,vertex_mem_sz,vertex_stride,
															index_count,index_mem_sz;
						GLuint								vertex,index;
					} map_vbo_type;

typedef struct		{
						int									count,
															indexes[max_map_light_cache_index];
					} map_light_cache_type;

//
// mesh structures
//

typedef struct		{
						bool								flat,wall_like;
						d3pnt								min,max,mid;
					} map_mesh_poly_box_type;

typedef struct		{
						int									lx,rx,lz,rz,
															l_ty,l_by,r_ty,r_by;
					} map_mesh_poly_line_type;

typedef struct		{
						float								y,ang_y,move_x,move_z;
					} map_mesh_poly_slope_type;
					
typedef struct		{
						float								ka,kb,kc,kd;
					} map_mesh_poly_plane_type;

typedef struct		{
						bool								climbable,never_cull,obscuring;
					} map_mesh_poly_flag_type;

typedef struct		{
						int									vertex_offset,decal_stencil_idx,
															txt_frame_offset;
						bool								culled[2],bump_ok,shadow_ok,
															shift_on,transparent_on,glow_on;
						d3uv								shift_offset;
					} map_mesh_poly_draw_type;

typedef struct		{
						d3uv								uvs[8];
					} map_mesh_poly_uv_type;

typedef struct		{
						int									index_offset;
					} map_mesh_poly_vbo_type;

typedef struct		{
						int									txt_idx,lmap_txt_idx,ptsz,v[8];
						char								camera[name_str_len];
						d3uv								shift;
						tangent_space_type					tangent_space;
						map_mesh_poly_uv_type				main_uv,lmap_uv;
						map_mesh_poly_box_type				box;
						map_mesh_poly_line_type				line;
						map_mesh_poly_slope_type			slope;
						map_mesh_poly_plane_type			plane;
						map_mesh_poly_flag_type				flag;
						map_mesh_poly_draw_type				draw;
						map_mesh_poly_vbo_type				vbo;
					} map_mesh_poly_type;

typedef struct		{
						int									wall_count,floor_count,obscure_count;
						short								*wall_idxs,*floor_idxs,*obscure_idxs;
					} map_mesh_poly_list;
					
typedef struct		{
						d3pnt								min,max,mid;
					} map_mesh_box_type;

typedef struct		{
						bool								on,pass_through,moveable,
															hilite,simple_collision,
															lock_uv,lock_move,cascade_size,
															never_obscure,never_cull,rot_independent,
															no_light_map,skip_light_map_trace,
															no_halo_obscure;
					} map_mesh_flag_type;

typedef struct		{
						int									entry_id,exit_id,base_team;
						bool								entry_on,exit_on,base_on,map_change_on;
						char								map_name[file_str_len],
															map_spot_name[name_str_len];
					} map_mesh_message_type;

typedef struct		{
						bool								shiftable,poly_has_camera,shadow_ok,
															has_obscure_poly,lighting_small;
					} map_mesh_precalc_flag_type;

typedef struct		{
						bool								moved,cur_ambient_only,
															has_opaque,has_transparent,has_glow;
					} map_mesh_draw_type;

typedef struct		{
						d3pnt								*vertexes;
						tangent_space_type					*tangent_spaces;
					} map_mesh_copy_type;

typedef struct		{
						char								obj_name[name_str_len],
															group_name[name_str_len];
					} map_mesh_import_type;

typedef struct		{
						int									rtl_mesh_id,
															nvertex,npoly,group_idx,
															hide_mode,harm;
						d3pnt								rot_off;
						d3pnt								*vertexes;
						map_mesh_poly_type					*polys;
						map_mesh_poly_list					poly_list;
						map_mesh_box_type					box;
						map_mesh_flag_type					flag;
						map_mesh_precalc_flag_type			precalc_flag;
						map_mesh_message_type				msg;
						map_mesh_draw_type					draw;
						map_mesh_copy_type					copy;
						map_mesh_import_type				import;
						map_light_cache_type				light_cache;
						map_vbo_type						vbo;
					} map_mesh_type;

//
// liquid structures
//

typedef struct		{
						d3uv								offset,size;
					} map_liquid_uv_type;

typedef struct		{
						int									in_harm,drown_harm,drown_tick;
					} map_liquid_harm_type;

typedef struct		{
						int									rate,high;
						float								uv_shift;
					} map_liquid_tide_type;

typedef struct		{
						int									length,high,period_msec;
						bool								on,dir_north_south;
					} map_liquid_wave_type;

typedef struct		{
						bool								lock_uv,never_obscure,never_cull,
															no_draw;
					} map_liquid_flag_type;

typedef struct		{
						int									buffer_idx;
						char								sound_name[name_str_len];
					} map_liquid_ambient_type;
					
typedef struct		{
						int									texture_size,
															x_refract_factor,z_refract_factor;
						float								alpha;
						d3col								no_hit_col;
					} map_liquid_reflection_type;

typedef struct		{
						int									txt_idx,stamp_size;
						bool								on;
						d3uv								shift;
					} map_liquid_overlay_type;

typedef struct		{
						int									y,lft,rgt,top,bot;
						bool								copied;
					} map_liquid_copy_type;

typedef struct		{
						int									rtl_mesh_id,
															y,depth,lft,rgt,top,bot,group_idx,
															txt_idx,lmap_txt_idx;
						float								speed_alter,tint_alpha;
						char								camera[name_str_len];
						d3uv								shift;
						d3col								col;
						map_liquid_uv_type					main_uv,lmap_uv;
						map_liquid_harm_type				harm;
						map_liquid_tide_type				tide;
						map_liquid_wave_type				wave;
						map_liquid_flag_type				flag;
						map_liquid_ambient_type				ambient;
						map_liquid_reflection_type			reflect;
						map_liquid_overlay_type				overlay;
						map_liquid_copy_type				copy;
						map_light_cache_type				light_cache;
						map_vbo_type						vbo;
					} map_liquid_type;

//
// mesh and liquid collections
//

typedef struct		{
						int									nmesh,vbo_vertex_count;
						map_mesh_type						*meshes;
					} map_mesh_collection_type;

typedef struct		{
						int									nliquid;
						map_liquid_type						*liquids;
					} map_liquid_collection_type;

//
// map sort lists
//

typedef struct		{
						int									mesh_idx,poly_idx;
						float								dist;
					} map_poly_sort_poly_type;

typedef struct		{
						int									count;
						map_poly_sort_poly_type				*polys;
					} map_poly_sort_type;

//
// scenery, light, sound and particle structures
//
					
typedef struct		{
						int									texture_frame[max_map_scenery_model_texture_frame];
						float								resize;
						char								model_name[name_str_len],animation_name[name_str_len];
						bool								contact_object_on,contact_projectile_on,
															contact_hit_box,face_forward,
															shadow;
						d3pnt								pnt;
						d3ang								ang;
					} map_scenery_type;

typedef struct		{
						int									type,direction,intensity,
															halo_idx;
						float								exponent;
						bool								on,light_map;
						char								halo_name[name_str_len];
						d3col								col;
					} map_light_setting_type;

typedef struct		{
						int									rtl_light_id;
						char								name[name_str_len];
						d3pnt								pnt;
						map_light_setting_type				setting;
					} map_light_type;
					
typedef struct		{
						int									buffer_idx;
                        float								pitch;
						char								name[name_str_len];
						bool								on;
						d3pnt								pnt;
					} map_sound_type;
					
typedef struct		{
                        int									spawn_tick,slop_tick,next_spawn_tick,
															particle_idx;
						char								name[name_str_len];
						bool								on,single_spawn,first_spawn;
						d3pnt								pnt;
						map_light_setting_type				light_setting;
					} map_particle_type;
                    
//
// script spots
//

typedef struct		{
						int									nearest_node_idx,spawned_obj_idx;
						bool								random_hit;
					} spot_lookup_type;

typedef struct		{
						int									radius;
						bool								used;
					} spot_checkpoint_type;

typedef struct		{
						int									type,skill,spawn;
						char								name[name_str_len],
															script[name_str_len],
															display_model[name_str_len],
															params[param_str_len];
						d3pnt								pnt;
						d3ang								ang;
						spot_lookup_type					lookup;
						spot_checkpoint_type				checkpoint;
					} spot_type;

//
// nodes
//

typedef struct		{
						bool								render;
						GLuint								txt_id;
					} node_back_render_type;

typedef struct		{
						int									idx,event_id;
						short								link[max_node_link],
															path_hint[max_node];
						float								alpha;
						char								name[name_str_len];
						bool								follow_camera;
						d3pnt								pnt;
						d3ang								ang;
						node_back_render_type				back_render;
					} node_type;

//
// group structure
//

typedef struct		{
						int									type,idx;
					} group_unit_type;

typedef struct		{
						int									count,user_id,attach_obj_idx,
															movement_idx,movement_move_idx;
						bool								on,was_moved,freeze,main_move;
						d3fpnt								f_mov_add,f_mov_accum_add;
						d3pnt								center_pnt,cuml_mov_add;
						d3ang								rot_add,cuml_rot_add;
					} group_run_type;

typedef struct		{
						int									unit_count;
						char								name[name_str_len];
						group_run_type						run;
						group_unit_type						*unit_list;
					} group_type;
					
typedef struct		{
						int									ngroup;
						group_type							*groups;
					} map_group_collection_type;

//
// movement structures
//
 
typedef struct		{
						int									msec,user_id,sound_buffer_idx;
						char								sound_name[name_str_len];
						float								sound_pitch;
						d3pnt								mov;
						d3ang								rot;
					} movement_move_type;

typedef struct		{
						bool								started,opened,reverse;
						d3pnt								auto_open_pnt;
					} movement_run_type;

typedef struct		{
						int									nmove,group_idx,reverse_group_idx,
															auto_open_distance;
						char								name[name_str_len];
						bool								auto_start,auto_open,auto_open_stand,
															auto_open_never_close,loop;
						movement_run_type					run;
						movement_move_type					*moves;
					} movement_type;

typedef struct		{
						int									nmovement;
						movement_type						*movements;
					} map_movement_collection_type;

//
// map media and music
//

typedef struct		{
						int									type,event_id;
						char								name[name_str_len],title_sound_name[name_str_len];
					} map_media_type;
					
typedef struct		{
						int									fade_msec;
						char								name[name_str_len],
															preload_name[max_music_preload][name_str_len];
					} map_music_type;
					
//
// map info and sky structures
//

typedef struct		{
						char								name[name_str_len],
															title[name_str_len],
															author[name_str_len],
															host_name[name_str_len],
															player_start_name[name_str_len],
															load_path[1024];
					} map_info_type;

typedef struct		{
						char								params[param_str_len];
					} map_settings_type;

typedef struct		{
						float								gravity,gravity_max_power,gravity_max_speed,
															resistance,slope_min_ang,slope_max_ang,
															slope_max_speed,slope_min_gravity;
					} map_physics_type;

typedef struct		{
						bool								map_picker;
					} map_singleplayer_type;

typedef struct		{
						char								game_list[256];
					} map_multiplayer_type;

typedef struct		{
						int									model,shadow,effect;
					} map_optimize_obscure_dist_type;

typedef struct		{
						int									shadow_poly_min_area;
						float								cull_angle;
						bool								never_cull,ray_trace_obscure,
															shadow_floor_only;
						map_optimize_obscure_dist_type		obscure_dist;
					} map_optimize_type;					

typedef struct		{
						float								sound_pitch;
						char								sound_name[name_str_len];
						d3col								light_color;
					} map_ambient_type;					

typedef struct		{
						int									density,radius,height,speed,
															line_width,line_length,slant_add,
															slant_time_msec,slant_change_msec;
						float								alpha;
						bool								on,reset;
						d3col								start_color,end_color;
					} map_rain_type;					

typedef struct		{
						int									fill;
						d3uv								size,scroll_factor,clip;
					} map_background_layer_type;					
					
typedef struct		{
						bool								on;
						map_background_layer_type			front,middle,back;
					} map_background_type;					
					
typedef struct		{
						int									type,radius,dome_y,
															fill,bottom_fill,north_fill,south_fill,east_fill,west_fill;
						float								txt_fact;
						bool								on,dome_mirror;
					} map_sky_type;
					
typedef struct		{
						int									count,inner_radius,outer_radius,
															high,drop,texture_idx;
						float								speed,alpha;
						bool								on,use_solid_color;
						d3uv								txt_fact;
						d3col								col;
					} map_fog_type;

//
// map camera
//

typedef struct		{
						int									near_z,far_z,near_z_offset;
						float								fov,aspect_ratio;
					} map_camera_plane_type;

typedef struct		{
						int									distance;
						float								track_speed;
						d3ang								track_ang,slop;
					} map_camera_setup_chase_type;

typedef struct		{
						bool								follow;
						char								attach_node[name_str_len];
					} map_camera_setup_static_type;

typedef struct		{
						int									camera_mode,input_mode;
						d3pnt								pnt_offset;
						d3ang								ang_offset;
						map_camera_plane_type				plane;
						map_camera_setup_chase_type			chase;
						map_camera_setup_static_type		c_static;
					} map_camera_type;

//
// map cinema structures
//

typedef struct		{
						int									action,actor_type,
															start_msec,end_msec;
						bool								move_reverse;
						char								actor_name[name_str_len],
															animation_name[name_str_len],
															next_animation_name[name_str_len],
															node_name[name_str_len],
															text_str[256];
					} map_cinema_action_type;

typedef struct		{
						int									naction,len_msec;
						bool								freeze_input,show_hud,no_cancel;
						char								name[name_str_len];
						map_cinema_action_type				*actions;
					} map_cinema_type;

typedef struct		{
						int									ncinema;
						map_cinema_type						*cinemas;
					} map_cinema_collection_type;

//
// map light maps and editor setup
//

typedef struct		{
						int									quality,size;
						float								diffuse_boost;
						bool								use_normals,skip_glows;
					} map_light_map_type;
					
typedef struct		{
						int									view_near_dist,view_far_dist;
						bool								link_always_start;
					} map_editor_setup_type;

//
// map editor view structures
//

typedef struct		{
						float								lft,rgt,top,bot;
					} editor_view_box_type;
					
typedef struct		{
						int									uv_layer;
						bool								ortho,cull,clip,no_rot;
						d3pnt								pnt;
						d3ang								ang;
						editor_view_box_type				box;
					} editor_view_type;

typedef struct		{
						int									count;
						editor_view_type					views[max_editor_view];
					} editor_views_type;

//
// main map structure
//

typedef struct		{

						int									nlight,nsound,nparticle,
															nspot,nnode,nscenery,
															start_game_tick;
													
						map_info_type						info;

						map_settings_type					settings;
						map_singleplayer_type				singleplayer;
						map_multiplayer_type				multiplayer;
						
						map_ambient_type					ambient;
						map_background_type					background;
						map_sky_type						sky;
						map_fog_type						fog;
						map_rain_type						rain;

						map_physics_type					physics;
						
						map_optimize_type					optimize;
						map_camera_type						camera;
						map_media_type						media;
						map_music_type						music;

						map_light_map_type					light_map;
						map_editor_setup_type				editor_setup;
						editor_views_type					editor_views;
						
						texture_type						*textures;
						
						map_scenery_type					*sceneries;
						map_light_type						*lights;
						map_sound_type						*sounds;
						map_particle_type					*particles;
						spot_type							*spots;
						node_type							*nodes;
						
						map_mesh_collection_type			mesh;
						map_liquid_collection_type			liquid;

						map_group_collection_type			group;
						map_movement_collection_type		movement;
						map_cinema_collection_type			cinema;
						
					} map_type;

//
// functions
//

extern bool map_new(map_type *map,char *name);
extern bool map_open(map_type *map,char *name);
extern bool map_reload(map_type *map);
extern bool map_save(map_type *map,char *err_str);
extern void map_close(map_type *map);

extern void map_textures_read_setup(map_type *map);
extern void map_textures_read_texture(map_type *map,int txt_idx);
extern void map_textures_close_texture(map_type *map,int txt_idx);
extern void map_refresh_textures(map_type *map);

extern void map_prepare_mesh_poly(map_type *map,map_mesh_type *mesh,map_mesh_poly_type *poly);
extern void map_prepare_mesh_box(map_mesh_type *mesh);
extern void map_prepare(map_type *map);
extern void map_move(map_type *map,d3pnt *mov_pnt);
extern void map_center(map_type *map);

extern int map_count_texture_frames(map_type *map,int txt);
extern void map_setup_animated_textures(map_type *map,int tick);

extern bool map_host_load_info(char *map_name,char *info_name,bool *singleplayer_map_picker,char *game_list);

extern bool map_create_vertex_lists(map_type *map);
extern void map_dispose_vertex_lists(map_type *map);

extern int map_count_spot(map_type *map,char *name,int type);
extern int map_find_spot(map_type *map,char *name,int type);
extern void map_find_random_spot_clear(map_type *map,char *name,int type);
extern int map_find_random_spot(map_type *map,char *name,int type);

extern int map_find_light(map_type *map,char *name);

extern int map_node_to_node_distance(map_type *map,int from_idx,int to_idx);
extern int map_find_node(map_type *map,char *name);
extern int map_find_nearest_node_by_point(map_type *map,d3pnt *pnt);
extern int map_find_nearest_node_in_path(map_type *map,int from_idx,char *name,int *dist);
extern int map_find_random_node(map_type *map,char *name,int skip_idx);
extern int map_find_random_forward_node(map_type *map,char *name,int skip_idx,d3pnt *pnt,d3ang *ang);
extern int map_find_next_node_in_path(map_type *map,int from_idx,int to_idx);

extern int map_mesh_add(map_type *map);
extern bool map_mesh_delete(map_type *map,int mesh_idx);
extern bool map_mesh_set_vertex_count(map_type *map,int mesh_idx,int nvertex);
extern bool map_mesh_set_poly_count(map_type *map,int mesh_idx,int npoly);
extern int map_mesh_duplicate(map_type *map,int mesh_idx);
extern int map_mesh_count_total_poly(map_type *map);
extern int map_mesh_add_poly(map_type *map,int mesh_idx,int ptsz,int *x,int *y,int *z,float *gx,float *gy,int txt_idx);
extern int map_mesh_add_duplicate_internal_poly(map_type *map,int mesh_idx,int poly_idx,d3pnt *mov_pnt);
extern bool map_mesh_delete_poly(map_type *map,int mesh_idx,int poly_idx);
extern bool map_mesh_delete_unused_vertexes(map_type *map,int mesh_idx);
extern void map_mesh_calculate_extent(map_type *map,int mesh_idx,d3pnt *min,d3pnt *max);
extern void map_mesh_calculate_center(map_type *map,int mesh_idx,d3pnt *mpt);
extern void map_mesh_poly_calculate_extent(map_type *map,int mesh_idx,int poly_idx,d3pnt *min,d3pnt *max);
extern void map_mesh_poly_calculate_center(map_type *map,int mesh_idx,int poly_idx,d3pnt *mpt);
extern void map_mesh_calculate_uv_center(map_type *map,int mesh_idx,float *gx,float *gy);

extern int map_mesh_find(map_type *map,d3pnt *pnt);
extern int map_mesh_find_closest(map_type *map,d3pnt *pnt);
extern int map_mesh_find_always(map_type *map,d3pnt *pnt);
extern int map_mesh_calculate_distance(map_mesh_type *mesh,d3pnt *pnt);

extern int map_mesh_combine(map_type *map,int mesh_1_idx,int mesh_2_idx);
extern void map_mesh_move_rotate_copy_reset(map_type *map,int mesh_idx);
extern void map_mesh_move_rotate_copy(map_type *map,int mesh_idx,d3pnt *center_pnt,d3pnt *move_pnt,d3ang *rot_ang);
extern void map_mesh_move(map_type *map,int mesh_idx,d3pnt *mov_pnt);
extern void map_mesh_resize(map_type *map,int mesh_idx,d3pnt *min,d3pnt *max);
extern void map_mesh_flip(map_type *map,int mesh_idx,bool flip_x,bool flip_y,bool flip_z);
extern void map_mesh_rotate(map_type *map,int mesh_idx,d3pnt *center_pnt,d3ang *rot_ang);
extern void map_mesh_skew(map_type *map,int mesh_idx,int axis,int dir,int size);
extern bool map_mesh_tesselate(map_type *map,int mesh_idx);
extern int map_mesh_poly_punch_hole(map_type *map,int mesh_idx,int poly_idx,d3pnt *extrude_pnt,bool extrude_close,int extrude_normal_mode);
extern bool map_mesh_poly_tesselate(map_type *map,int mesh_idx,int poly_idx);
extern void map_mesh_poly_run_shifts(map_type *map,int tick);
extern void map_mesh_get_poly_uv_as_box(map_type *map,int mesh_idx,int poly_idx,bool light_map_uv,float *x_txtoff,float *y_txtoff,float *x_txtfact,float *y_txtfact);
extern void map_mesh_set_poly_uv_as_box(map_type *map,int mesh_idx,int poly_idx,bool light_map_uv,float x_txtoff,float y_txtoff,float x_txtfact,float y_txtfact);
extern void map_mesh_rotate_poly_uv(map_type *map,int mesh_idx,int poly_idx,int rot_ang);
extern void map_mesh_flip_poly_uv(map_type *map,int mesh_idx,int poly_idx,bool flip_u,bool flip_v);
extern void map_mesh_reset_poly_uv(map_type *map,int mesh_idx,int poly_idx);
extern void map_mesh_reset_uv(map_type *map,int mesh_idx);
extern void map_mesh_whole_poly_uv(map_type *map,int mesh_idx,int poly_idx);
extern void map_mesh_whole_uv(map_type *map,int mesh_idx);
extern void map_mesh_poly_single_uv(map_type *map,int mesh_idx,int poly_idx);
extern void map_mesh_poly_multipy_uv(map_type *map,int mesh_idx,int poly_idx,float x,float y);
extern void map_mesh_single_uv(map_type *map,int mesh_idx);
extern void map_mesh_sort_polys(map_type *map);

extern int map_liquid_add(map_type *map);
extern bool map_liquid_delete(map_type *map,int liquid_idx);
extern int map_liquid_duplicate(map_type *map,int liquid_idx);
extern void map_liquid_calculate_center(map_type *map,int liquid_idx,d3pnt *pt);
extern int map_liquid_calculate_distance(map_liquid_type *liq,d3pnt *pnt);

extern void map_liquid_move(map_type *map,int liquid_idx,d3pnt *mov_pnt);
extern void map_liquid_move_copy(map_type *map,int liquid_idx,d3pnt *mov_pt);
extern void map_liquid_reset_uv(map_type *map,int liquid_idx);

extern int map_group_add(map_type *map);
extern bool map_group_delete(map_type *map,int group_idx);
extern bool map_group_create_unit_list(map_type *map);
extern void map_group_dispose_unit_list(map_type *map);
extern void map_group_get_center(map_type *map,int group_idx,d3pnt *center_pnt);

extern int map_movement_add(map_type *map);
extern bool map_movement_delete(map_type *map,int movement_idx);
extern int map_movement_move_add(map_type *map,int movement_idx);
extern bool map_movement_move_delete(map_type *map,int movement_idx,int move_idx);
extern int map_movement_find(map_type *map,char *name);

extern bool map_add_texture_frame(map_type *map,int txt,char *bitmap_name);
extern bool map_delete_texture_frame(map_type *map,int txt);
extern bool map_replace_texture(map_type *map,int txt,char *bitmap_name);
extern bool map_delete_texture(map_type *map,int start_txt,int end_txt);

extern void map_flip_normals_mesh_poly(map_type *map,map_mesh_type *mesh,int poly_idx);
extern void map_recalc_normals_mesh_poly(map_type *map,map_mesh_type *mesh,int poly_idx,int normal_mode,bool only_tangent);
extern void map_recalc_normals_mesh(map_type *map,map_mesh_type *mesh,int normal_mode,bool only_tangent);
extern void map_recalc_normals(map_type *map,bool only_tangent);

extern int map_cinema_add(map_type *map);
extern bool map_cinema_delete(map_type *map,int cinema_idx);
extern int map_cinema_add_action(map_type *map,int cinema_idx);
extern bool map_cinema_delete_action(map_type *map,int cinema_idx,int action_idx);
