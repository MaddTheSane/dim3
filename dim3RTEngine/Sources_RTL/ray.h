//
// dim3RTL
// by Brian Barnes (bbarnes@klinksoftware.com)
// free software offered as is for any use
//

//
// types
//

#include "ray_types.h"

//
// data
//

#define ray_max_scene								32
#define ray_max_scene_light							128
#define ray_max_scene_mesh							5120

#define ray_max_material							1024

//
// ray tracing
//

#define ray_max_ray_bounce							8
#define ray_max_light_per_mesh						8
#define ray_max_tint_per_pixel						64

#define ray_max_mesh_pack_list						ray_max_scene_mesh
#define ray_max_mesh_poly_pack_list					10240

//
// rendering
//
// note: max_slice_count MUST BE A SQRT!
//

#define ray_render_max_slice_count					(24*24)
#define ray_render_max_thread_count					32

//
// worker thread mode
//

#define ray_thread_mode_suspend						0
#define ray_thread_mode_rendering					1
#define ray_thread_mode_shutdown					2

//
// mipmaps
//

#define ray_mipmap_max_level						8
#define ray_mipmap_min_dimension					32
#define ray_mipmap_distance_exponent				0.7f

//
// math defines
//

#define ray_TRIG_PI									3.14159265358979f
#define ray_ANG_to_RAD								(float)(ray_TRIG_PI/180.0f)
#define ray_RAD_to_ANG								(float)(180.0f/ray_TRIG_PI)

//
// bounds struct
//

typedef struct		{
                        ray_point_type					min,max,mid;
                    } ray_bound_type;

//
// scene eye
//
// this contains the eye position in a scene object.
//

typedef struct		{
						float							plane_dist,max_dist;
						ray_point_type					pnt;
						ray_matrix_type					matrix;
					} ray_eye_type;

//
// packed mesh-poly list
//
// these are two structures used to pack arrays of meshes or arrays
// of meshes or polys, used to optimize what the render has to look at
//
// the mesh list is just a list of mesh indexes
//
// the mesh-poly list is formated like this:
//
// [mesh index][poly count][poly 1][poly 2][...][mesh index][etc]
//

typedef struct		{
						int								idx;
						int								list[ray_max_mesh_pack_list];
					} ray_mesh_pack_list;

typedef struct		{
						int								idx;
						int								list[ray_max_mesh_poly_pack_list];
					} ray_mesh_poly_pack_list;

//
// scene meshes
//
// these are the meshes inside of each scene object.  They contain
// vertex, uv, normal, tangent, and triangle lists.
//
// each mesh/poly has a parallel main render and slice structures
// that are used to store linked lists containing a reduced number
// of meshes and polys for certain circumstances
//

typedef struct		{
						int								count;
						ray_point_type					*vertexes;
					} ray_vertex_block;

typedef struct		{
						int								count;
						ray_uv_type						*uvs;
					} ray_uv_block;

typedef struct		{
						int								count;
						ray_vector_type					*normals;
					} ray_normal_block;

typedef struct		{
						int								count;
						ray_vector_type					*tangents;
					} ray_tangent_block;

typedef struct		{
						int								vertex,uv,
														normal,tangent;
					} ray_polygon_index_type;

typedef struct		{
						ray_polygon_index_type			idxs[3];
						ray_vector_type					v1,v2;
					} ray_trig_type;

typedef struct		{
						int								count;
						ray_trig_type					*trigs;
					} ray_trig_block;

typedef struct		{
						int								next_link_poly_idx;
					} ray_poly_slice_type;

typedef struct		{
						int								material_idx,nvertex,
														mm_level;
						ray_color_type					col;
						ray_vector_type					surface_normal;
						ray_polygon_index_type			idxs[8];
						ray_trig_block					trig_block;
						ray_plane_type					plane;
						ray_bound_type					bound;
						ray_poly_slice_type				slice[ray_render_max_slice_count];
					} ray_poly_type;

typedef struct		{
						int								count;
						ray_poly_type					*polys;
					} ray_poly_block;

typedef struct		{
						int								idx;
						ray_mesh_poly_pack_list			mesh_poly_pack_block_list;			// a list of all the mesh/polys that could block parent mesh from this light
					} ray_mesh_light_collide_type;

typedef struct		{
						int								count;
						ray_mesh_light_collide_type		lights[ray_max_light_per_mesh];		// a list of lights that could possibly fall onto this mesh
					} ray_mesh_light_collide_list;

typedef struct		{
						int								id;
						bool							hidden;
						unsigned long					flags;
						ray_vertex_block				vertex_block;
						ray_uv_block					uv_block;
						ray_normal_block				normal_block;
						ray_tangent_block				tangent_block;
						ray_poly_block					poly_block;
						ray_bound_type					bound;
						ray_mesh_light_collide_list		light_collide;
					} ray_mesh_type;

typedef struct		{
						int								count,next_id;
						ray_mesh_type*					meshes[ray_max_scene_mesh];
					} ray_mesh_list;

//
// scene lights
//
// these contain the lights in a scene.
//

typedef struct		{
						float							cos_sweep;
						bool							on;
						ray_vector_type					vct;
					} ray_light_direction_type;

typedef struct		{
						int								id;
						float							intensity,exponent;
						bool							hidden;
						ray_point_type					pnt;
						ray_color_type					col;
						ray_light_direction_type		direction;
						ray_bound_type					bound;
						ray_mesh_poly_pack_list			mesh_poly_pack_collide_list;
					} ray_light_type;

typedef struct		{
						int								count,next_id;
						ray_light_type*					lights[ray_max_scene_light];
					} ray_light_list;

//
// scene rendering and slices
//
// these structures contain information needed to render a scene, like a list of
// non-culled meshes for the view, and the slices, which are chunks of the view
// that the rendering threads pick up and render into
//
// there's a number a pre-calced lists, like all the non-culled polys in each slice,
// or all the possible blocking polys for lights
//

typedef struct		{
						int										mesh_idx,poly_idx;
					} ray_scene_slice_likey_block_type;

typedef struct		{
						int										idx;
						ray_2d_point_type						pixel_start,pixel_end;
						ray_scene_slice_likey_block_type		likely_block[ray_max_scene_light];
						ray_mesh_poly_pack_list					mesh_pack_list;
					} ray_scene_slice_type;

typedef struct		{
						bool									shutdown_done;
						void									*parent_scene;					// this is a pointer back to the parent structure, need by threading
						ray_thread								thread;
					} ray_scene_thread_type;

typedef struct		{
						int										next_slice_idx,thread_done_count;
						ray_mutex								scene_lock,thread_lock;			// thread_lock only needed for pthread con waits
						ray_cond								thread_cond;					// thread_cond only needed for pthread con waits
						ray_scene_thread_type					*threads;						// initially ray_render_max_thread_count
						ray_scene_slice_type					*slices;						// initially ray_render_max_slice_count
						ray_mesh_pack_list						view_mesh_pack_list;
					} ray_scene_render_type;

//
// the scene
//
// the scene is the main rendering object for a openRL operation.  Scenes
// contain a drawing buffer/target, lights, meshes, and eye position
//

typedef union		{
						unsigned long					gl_id;
					} ray_attachment_type;

typedef struct		{
						int								target,format,
														wid,high;
						unsigned long					*data;
					} ray_scene_buffer_type;

typedef struct		{
						int								id,target,thread_mode;
						ray_attachment_type				attachment;
						ray_eye_type					eye;
						ray_color_type					ambient_col;
						ray_light_list					light_list;
						ray_mesh_list					mesh_list;
						ray_scene_buffer_type			buffer;
						ray_scene_render_type			render;
					} ray_scene_type;

typedef struct		{
						int								count,next_id,
														thread_mode;
						ray_scene_type*					scenes[ray_max_scene];
					} ray_scene_list;

//
// materials
//
// materials are global to all scenes within the openRL, and can be used
// by any scene
//
// note that unlike other lists, materials does not have a count.  it uses
// null to find an empty spot.  This is to preserve the indexes stored within
// items like meshes
//

typedef struct		{
						unsigned char					*color,*normal,
														*specular,*glow;
					} ray_material_mipmap_data_type;

typedef struct		{
						int								wid,high;
						float							wid_scale,high_scale;
						ray_material_mipmap_data_type	data;
					} ray_material_mipmap_type;

typedef struct		{
						int								count;
						ray_material_mipmap_type		mipmaps[ray_mipmap_max_level];
					} ray_material_mipmap_list;

typedef struct		{
						int								id,wid,high,
														alpha_type;
						float							shine_factor,glow_factor,
														refract_factor;
						bool							no_alpha;
						ray_material_mipmap_list		mipmap_list;
					} ray_material_type;

typedef struct		{
						int								next_id;
						ray_material_type*				materials[ray_max_material];
					} ray_material_list;

//
// rendering pixel data
//

typedef struct		{
						ray_vector_type					normal,tangent,binormal;
					} ray_material_pixel_surface_type;
typedef struct		{
						bool							on;
						ray_color_type					rgb;
					} ray_material_pixel_col_type;

typedef struct		{
						float							shine_factor;
						ray_material_pixel_surface_type	surface;
						ray_material_pixel_col_type		color,normal,specular,glow;
					} ray_material_pixel_type;

//
// global ray
//
// this structure is the global structure that contains all the structures
// within an dim3RTL engine
//

typedef struct		{
						int								slice_count,
														thread_count;
					} ray_settings_type;

typedef struct		{
						ray_settings_type				settings;
						ray_scene_list					scene_list;
						ray_material_list				material_list;
					} ray_global_type;

//
// internal structures
//

typedef struct		{
						int								mesh_idx,poly_idx;
					} ray_collision_skip_type;

typedef struct		{
						int								count;
						ray_collision_skip_type			skips[ray_max_ray_bounce];
					} ray_collision_skip_block;

typedef struct		{
						float							t;
						ray_color_type					col;
					} ray_collision_tint_type;

typedef struct		{
						int								count;
						ray_collision_tint_type			tints[ray_max_tint_per_pixel];
					} ray_collision_tint_block;

typedef struct		{
						int								mesh_idx,poly_idx,trig_idx;
						float							max_t,t,u,v;
						bool							in_bounce,only_pass_through;
						ray_collision_skip_block		skip_block;
						ray_collision_tint_block		tint_block;
					} ray_collision_type;

//
// internal interfaces
//

extern unsigned long ray_create_ulong_color_from_float(ray_color_type *col);
extern unsigned long ray_create_ulong_color_from_float_no_alpha(ray_color_type *col);
extern unsigned long ray_create_ulong_color_from_float_no_alpha_clamp(ray_color_type *col);
extern void ray_create_float_color_from_ulong(unsigned long ul,ray_color_type *rgb);
extern void ray_create_float_color_from_ulong_no_alpha(unsigned long ul,ray_color_type *rgb);
extern unsigned long ray_create_ulong_black(void);

extern inline float ray_vector_normalize(ray_vector_type *v);
extern inline void ray_vector_create_from_points(ray_vector_type *v,ray_point_type *p1,ray_point_type *p2);
extern inline void ray_vector_scalar_multiply(ray_vector_type *sm,ray_vector_type *v,float f);
extern inline void ray_vector_add(ray_vector_type *v,ray_vector_type *v1,ray_vector_type *v2);
extern inline void ray_vector_cross_product(ray_vector_type *cp,ray_vector_type *v1,ray_vector_type *v2);
extern inline float ray_vector_dot_product(ray_vector_type *v1,ray_vector_type *v2);
extern inline void ray_vector_find_line_point_for_T(ray_point_type *p,ray_vector_type *v,float t,ray_point_type *lp);

extern void ray_to_bound(ray_point_type *p,ray_vector_type *v,ray_bound_type *bnd);
extern bool ray_bound_bound_collision(ray_bound_type *bnd_1,ray_bound_type *bnd_2);
extern bool ray_bound_ray_collision(ray_point_type *p,ray_vector_type *v,ray_bound_type *bnd);

extern bool ray_plane_ray_collision(ray_point_type *p,ray_vector_type *nv,float nv_dist,ray_plane_type *plane);

extern float ray_distance_between_points(ray_point_type *p1,ray_point_type *p2);
extern unsigned char* ray_bitmap_reduction(int factor,int wid,int high,unsigned char *data);

extern void ray_scene_3D_to_2D_point(ray_scene_type *scene,ray_point_type *pnt_3d,ray_2d_point_type *pnt_2d);

extern void ray_scene_clear_threads(ray_scene_type *scene);
extern void ray_scene_wait_shutdown_threads(ray_scene_type *scene);
extern void ray_scene_resume_threads(ray_scene_type *scene,int mode);
extern void ray_scene_release_threads(ray_scene_type *scene);
extern bool ray_scene_create_threads(ray_scene_type *scene);
extern bool ray_scene_create_mutexes(ray_scene_type *scene);
extern void ray_scene_release_mutexes(ray_scene_type *scene);

extern void ray_precalc_mesh_bounds(ray_mesh_type *mesh);
extern void ray_precalc_polygon_bounds(ray_mesh_type *mesh,ray_poly_type *poly);
extern void ray_precalc_polygon_normal(ray_mesh_type *mesh,ray_poly_type *poly);
extern void ray_precalc_polygon_plane(ray_mesh_type *mesh,ray_poly_type *poly);
extern void ray_precalc_light_bounds(ray_light_type *light);
extern void ray_precalc_triangle_vectors(ray_mesh_type *mesh,ray_trig_type *trig);
extern void ray_precalc_render_scene_setup(ray_scene_type *scene);
extern void ray_precalc_render_scene_slice_setup(ray_scene_type *scene,ray_scene_slice_type *slice);

extern void ray_get_material_pixel(ray_scene_type *scene,ray_point_type *eye_pnt,ray_point_type *trig_pnt,ray_collision_type *collision,ray_material_pixel_type *pixel);
extern void ray_get_material_color(ray_scene_type *scene,ray_point_type *eye_pnt,ray_point_type *trig_pnt,ray_collision_type *collision,ray_color_type *col);
extern float ray_get_material_alpha(ray_scene_type *scene,ray_point_type *eye_pnt,ray_point_type *trig_pnt,ray_collision_type *collision);
extern void ray_get_material_normal(ray_scene_type *scene,ray_point_type *eye_pnt,ray_point_type *trig_pnt,ray_collision_type *collision,ray_vector_type *normal);

extern int ray_scene_get_index(int sceneId);
extern int ray_material_get_index(int materialId);

extern void ray_render_clear_threads(ray_scene_type *scene);
extern void ray_render_stall(ray_scene_type *scene);

#ifndef WIN32
	extern void* ray_render_thread(void *arg);
#else
	extern unsigned __stdcall ray_render_thread(void *arg);
#endif


