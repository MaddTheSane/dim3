/****************************** File *********************************

Module: dim3 Model Utility
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

#define model_current_version							3

//
// definitions
//

#define max_model										64			// maximum models in a map

#define max_model_mesh									32			// maximum number of meshes
#define max_model_bone									128			// maximum number of bones
#define max_model_texture								32			// maximum number of textures in model

#define max_model_pose									256			// maximum number of poses
#define max_model_animate								128			// maximum number of animations
#define max_model_animate_pose							32			// maximum number of poses per animation

#define max_model_blend_animation						4			// maximum number of blended animations
#define max_model_light									8			// maximum number of lights in a model
#define max_model_halo									8			// maximum number of halos in a model

#define max_model_animate_particle						8			// maximum number of particles for animation
#define max_model_animate_ring							8			// maximum number of rings for animation

#define max_model_hit_box								16			// maximum number of hit boxes

//
// drawing stuctures
//

typedef struct		{
						int								parent_idx;
						bool							touch;
						d3fpnt							fpnt;
						d3fpnt							parent_dist;
						d3ang							rot;
						matrix_type						rot_mat;
					} model_draw_bone_type;

typedef struct		{
						float							resize;
						d3fpnt							parent_dist_add;
						d3ang							rot_add;
					} model_draw_alter_bone_type;

typedef struct		{
                        int								idx_1,idx_2;
                        float							factor,acceleration;
					} model_draw_pose_type;
					
typedef struct		{
						float							*gl_vertex_array,
														*gl_tangent_array,
														*gl_normal_array;
						unsigned char					*poly_cull_array;
					} model_draw_mesh_array_type;

typedef struct		{
						float							enhance_factor;
						d3pnt							center;
						d3fpnt							move;
						d3ang							ang;
						d3ang							sway;
						model_draw_pose_type			poses[max_model_blend_animation];
						model_draw_bone_type			bones[max_model_bone];
						model_draw_alter_bone_type		alter_bones[max_model_bone];
						model_draw_mesh_array_type		mesh_arrays[max_model_mesh];
                    } model_draw_setup;

//
// model info structures
//		

typedef struct		{
						d3pnt							size;
						d3pnt							offset;
					} model_box_type;
					
typedef struct		{
						int								bone_idx;
						char							name[64];
						model_box_type					box;
					} model_hit_box_type;
                    
//
// model mesh structures
//

typedef struct		{
						int								poly_idx,v_idx;
						float							dist;
					} model_trans_sort_poly_type;

typedef struct		{
						int								count;
						model_trans_sort_poly_type		*polys;	
					} model_trans_sort_type;

typedef struct		{
						int								major_bone_idx,minor_bone_idx;
						float							bone_factor;
						d3pnt							pnt;
						d3vct							major_dist,minor_dist;
						tangent_space_type				tangent_space;
					} model_vertex_type;
		
typedef struct		{
						int								txt_idx,ptsz,v[8];
						float							gx[8],gy[8];
					} model_poly_type;

typedef struct		{
						int								nvertex,npoly;
						char							name[name_str_len];
						bool							no_lighting,diffuse,
														blend_add,never_cull,locked,
														rt_non_light_blocking;
						d3pnt							import_move,org_import_move;
						model_trans_sort_type			trans_sort;
 						model_vertex_type				*vertexes;
                        model_poly_type					*polys;
					} model_mesh_type;

//
// model bone structures
//

typedef struct		{
						int								parent_idx;
						char							name[name_str_len];
						d3pnt							pnt,natural_offset;
						d3ang							natural_rot;
						d3vct							parent_dist;
					} model_bone_type;

//
// model rigid body
//

typedef struct		{
						float							max_ang,reset_factor,smooth_factor;
					} model_rigid_body_axis_type;

typedef struct		{
						bool							on;
						model_rigid_body_axis_type		x,y,z;
					} model_rigid_body_type;

//
// model poses and animation structures
//

typedef struct		{
						int								bone_idx;
						d3pnt							offset;
					} model_bone_move_constraint_type;

typedef struct		{
						float							acceleration;
						bool							skip_blended;
						d3vct							rot;
						d3vct							mov;
						model_bone_move_constraint_type	constraint;
					} model_bone_move_type;
					
typedef struct		{
						char							name[64];
						model_bone_move_type			bone_moves[max_model_bone];
					} model_pose_type;

typedef struct		{
						int								bone_idx,buffer_idx;
						float							pitch;
						bool							no_position;
						char							name[name_str_len];
					} model_pose_move_sound_type;
					
typedef struct		{
						int								particle_idx,bone_idx;
						float							motion_factor;
						char							name[name_str_len];
						bool							rotate,motion,stick;
						d3pnt							slop;
					} model_particle_type;

typedef struct		{
						int								count;
						model_particle_type				particles[max_model_animate_particle];
					} model_pose_move_particle_type;

typedef struct		{
						int								ring_idx,bone_idx;
						char							name[name_str_len];
						bool							angle;
						d3pnt							slop;
					} model_ring_type;

typedef struct		{
						int								count;
						model_ring_type					rings[max_model_animate_ring];
					} model_pose_move_ring_type;

typedef struct		{
						int								mesh_idx,fade_in_msec,fade_life_msec,fade_out_msec;
					} model_pose_move_mesh_fade_type;
					
typedef struct		{
						int								bone_idx,intensity,flash_msec,fade_msec;
						float							exponent;
						d3col							col;
					} model_pose_move_flash_type;
					
typedef struct		{
						int								distance,size,life_msec;
					} model_pose_move_shake_type;
					
typedef struct		{
						int								pose_idx,msec;
						float							acceleration;
						d3vct							mov;
						d3vct							sway;
						model_pose_move_sound_type		sound;
						model_pose_move_mesh_fade_type	mesh_fade;
						model_pose_move_flash_type		flash;
						model_pose_move_shake_type		shake;
						model_pose_move_particle_type	particle;
						model_pose_move_ring_type		ring;
 					} model_pose_move_type;
					
typedef struct		{
						int								npose_move,loop_start,loop_end;
						float							enhance_factor;
						char							name[64];
						bool							loop,no_smooth,auto_stop;
						model_pose_move_type			pose_moves[max_model_animate_pose];
					} model_animate_type;

//
// model bone connections
//

typedef struct		{
						int								light_bone_idx[max_model_light],
														halo_bone_idx[max_model_halo],
														name_bone_idx;
					} model_bone_connect_type;

//
// model labels
//

typedef struct		{
						int								size,bone_idx;
						d3col							col;
					} model_label_text_type;

typedef struct		{
						int								size,bone_idx;
					} model_label_bitmap_type;

typedef struct		{
						int								wid,high,bone_idx;
						bool							border_on,background_on;
						d3col							border_col,background_col,bar_col;
					} model_label_bar_type;

typedef struct		{
						model_label_text_type			text;
						model_label_bitmap_type			bitmap;
						model_label_bar_type			bar;
					} model_label_type;

//
// model UI settings
//

typedef struct		{
						int								light_intensity;
						float							light_exponent;
						d3pnt							light_offset;
						d3col							light_col;
					} model_ui_type;

//
// model loading
//

typedef struct		{
						int								reference_count;
						bool							preloaded;
} model_load_type;

//
// model main structures
//

typedef struct		{
						int								uid,
														nmesh,nbone,npose,nanimate,nhit_box;
						bool							comulative_rotation;
						char							name[name_str_len],load_base_path[1024];
						d3pnt							center;
						model_load_type					load;
						model_box_type					view_box;
						model_bone_connect_type			bone_connect;
 						model_mesh_type					meshes[max_model_mesh];
						model_rigid_body_type			rigid_body;
						model_label_type				label;
						model_ui_type					ui;
						model_bone_type					*bones;
						model_pose_type					*poses;
						model_animate_type				*animates;
						model_hit_box_type				*hit_boxes;
						texture_type					*textures;
					} model_type;

//
// model functions
//

extern int model_mesh_add(model_type *model);
extern int model_mesh_duplicate(model_type *model,int mesh_idx);
extern bool model_mesh_delete(model_type *model,int mesh_idx);
extern bool model_mesh_set_vertex_count(model_type *model,int mesh_idx,int vertex_count);
extern bool model_mesh_set_poly_count(model_type *model,int mesh_idx,int poly_count);

extern void model_bone_create_name(model_type *model,int bone_idx);
extern int model_bone_add(model_type *model,int x,int y,int z);
extern void model_bone_delete(model_type *model,int bone_idx);
extern bool model_check_bone_duplicate_name(model_type *model,model_bone_type *bone);
extern bool model_check_bone_circular(model_type *model,model_bone_type *bone);
extern void model_bone_move(model_type *model,int bone_idx,int x,int y,int z,bool nudge_children,bool nudge_vertexes);

extern int model_pose_add(model_type *model);
extern int model_pose_duplicate(model_type *model,int pose_idx);
extern void model_pose_delete(model_type *model,int pose_idx);
extern void model_pose_clear(model_type *model,int pose_idx);
extern bool model_check_pose_in_animation(model_type *model,int pose_idx);
extern void model_pose_blend_set_all(model_type *model,int pose_idx,bool skip_blended);

extern int model_animate_add(model_type *model);
extern int model_animate_duplicate(model_type *model,int animate_idx);
extern void model_animate_delete(model_type *model,int animate_idx);
extern int model_animate_pose_insert(model_type *model,int animate_idx,int animate_pose_idx,int pose_idx);
extern void model_animate_pose_delete(model_type *model,int animate_idx,int animate_pose_idx);
extern void model_animate_set_loop_start(model_type *model,int animate_idx,int animate_pose_idx);
extern void model_animate_set_loop_end(model_type *model,int animate_idx,int animate_pose_idx);
extern int model_animate_add_particle(model_type *model,int animate_idx,int animate_pose_idx);
extern void model_animate_delete_particle(model_type *model,int animate_idx,int animate_pose_idx,int idx);
extern int model_animate_add_ring(model_type *model,int animate_idx,int animate_pose_idx);
extern void model_animate_delete_ring(model_type *model,int animate_idx,int animate_pose_idx,int idx);

extern int model_hit_box_add(model_type *model);
extern void model_hit_box_delete(model_type *model,int hit_box_idx);
extern void model_hit_box_get_center(model_type *model,int hit_box_idx,d3pnt *pnt,model_draw_setup *draw_setup,d3pnt *center_pnt);
extern void model_hit_box_get_box(model_type *model,int hit_box_idx,d3pnt *pnt,model_draw_setup *draw_setup,d3pnt *min,d3pnt *max);

extern int model_find_mesh(model_type *model,char *mesh_name);
extern int model_find_bone(model_type *model,char *bone_name);
extern int model_find_pose(model_type *model,char *pose_name);
extern int model_find_animate(model_type *model,char *animate_name);

extern void model_textures_read_texture(model_type *model,int txt_idx);
extern void model_textures_close_texture(model_type *model,int txt_idx);

extern int model_count_texture_frames(model_type *model,int txt);
extern void model_setup_animated_textures(model_type *model,int tick);
extern bool model_add_texture_frame(model_type *model,int txt,char *bitmap_name);
extern bool model_delete_texture_frame(model_type *model,int txt);

extern bool model_draw_setup_initialize(model_type *model,model_draw_setup *draw_setup,bool tangent_space);
extern void model_draw_setup_shutdown(model_type *model,model_draw_setup *draw_setup);
extern void model_draw_setup_clear(model_type *model,model_draw_setup *draw_setup);

extern void model_calculate_parents(model_type *model);

extern void model_create_draw_bones(model_type *model,model_draw_setup *draw_setup);
extern void model_create_draw_vertexes(model_type *model,int mesh_idx,model_draw_setup *draw_setup);
extern void model_create_draw_2D_vertexes(model_type *model,int mesh_idx,model_draw_setup *draw_setup,int x_offset,int y_offset,float x_resize,float y_resize);

extern void model_resize_draw_vertex(model_type *model,int mesh_idx,float resize,model_draw_setup *draw_setup);
extern void model_flip_draw_vertex(model_type *model,int mesh_idx,model_draw_setup *draw_setup);
extern void model_translate_draw_vertex(model_type *model,int mesh_idx,int x,int y,int z,model_draw_setup *draw_setup);

extern void model_create_draw_normals(model_type *model,int mesh_idx,model_draw_setup *draw_setup);
extern void model_flip_draw_normals(model_type *model,int mesh_idx,model_draw_setup *draw_setup);

extern void model_get_point_position(model_draw_setup *draw_setup,d3pnt *pnt);
extern void model_get_draw_bone_position(model_draw_setup *draw_setup,int bone_idx,d3pnt *pnt);
extern void model_calc_draw_bone_position(model_type *model,model_draw_setup *draw_setup,int pose_idx,int bone_idx,d3pnt *pnt);
extern bool model_get_generic_bone_position(model_draw_setup *draw_setup,int bone_idx,d3pnt *pnt);
extern bool model_get_light_position(model_type *model,model_draw_setup *draw_setup,int idx,d3pnt *pnt);
extern bool model_get_halo_position(model_type *model,model_draw_setup *draw_setup,int idx,d3pnt *pnt);

extern void model_get_size(model_type *model,int *x,int *y,int *z);
extern void model_get_vertex_extent(model_type *model,int mesh_idx,int *p_minx,int *p_maxx,int *p_minz,int *p_maxz,int *p_miny,int *p_maxy);
extern void model_get_vertex_extent_all(model_type *model,int *p_minx,int *p_maxx,int *p_minz,int *p_maxz,int *p_miny,int *p_maxy);
extern void model_get_view_complex_bounding_box(model_type *model,d3pnt *pnt,d3ang *ang,float resize,int *px,int *py,int *pz);
extern void model_get_view_complex_bounding_volume(model_type *model,d3pnt *pnt,d3ang *ang,float resize,d3pnt *min,d3pnt *max);
extern void model_recalc_boxes(model_type *model);

extern void model_recalc_normals(model_type *model,bool only_tangent);

extern void model_rescale_box(model_box_type *mbox,d3fpnt *scale);
extern void model_scale(model_type *model,int mesh_idx,d3fpnt *scale);
extern void model_scale_all(model_type *model,d3fpnt *scale);
extern void model_flip(model_type *model,int mesh_idx,bool flip_x,bool flip_y,bool flip_z);
extern void model_flip_all(model_type *model,bool flip_x,bool flip_y,bool flip_z);
extern void model_swap_xz(model_type *model,int mesh_idx);
extern void model_swap_xz_all(model_type *model);
extern void model_swap_yz(model_type *model,int mesh_idx);
extern void model_swap_yz_all(model_type *model);
extern void model_center_xz(model_type *model,int mesh_idx);
extern void model_center_xz_all(model_type *model);
extern void model_floor(model_type *model,int mesh_idx);
extern void model_floor_all(model_type *model);
extern void model_flip_uv(model_type *model,int mesh_idx,bool flip_u,bool flip_v);

extern bool model_new(model_type *model,char *name);
extern bool model_open(model_type *model,char *name,bool load_bitmaps);
extern bool model_save(model_type *model,char *err_str);
extern void model_close(model_type *model);

extern void model_refresh_textures(model_type *model);

extern int model_memory_size(model_type *model);

