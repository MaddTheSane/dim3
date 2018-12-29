/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Auto Generator Private Map Defines

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

(c) 2000-2014 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// constants
//

#define ag_max_room								256

#define ag_max_position_room_try				8

#define ag_connector_type_min_x					0
#define ag_connector_type_max_x					1
#define ag_connector_type_min_z					2
#define ag_connector_type_max_z					3

#define ag_story_place_none						0
#define ag_story_place_left						1
#define ag_story_place_right					2
#define ag_story_place_top						3
#define ag_story_place_bottom					4
#define ag_story_place_all						5

//
// connection sides
//

#define ag_connect_side_top						0
#define ag_connect_side_bottom					1
#define ag_connect_side_left					2
#define ag_connect_side_right					3

//
// connection types
//

#define ag_connect_type_normal					0
#define ag_connect_type_door					1
#define ag_connect_type_arch					2
#define ag_connect_type_stairs					3

//
// stair types
//

#define ag_stair_type_one_side					0
#define ag_stair_type_middle					1
#define ag_stair_type_full						2

//
// decorations
//

#define ag_decoration_type_none					-1
#define ag_decoration_type_columns				0
#define ag_decoration_type_box_stacks			1
#define ag_decoration_type_equipment			2
#define ag_decoration_type_core					3
#define ag_decoration_type_walls				4
#define ag_decoration_type_trench				5
#define ag_decoration_type_machinery			6
#define ag_decoration_type_rubble				7

#define ag_decoration_column_config_square		0
#define ag_decoration_column_config_diamond		1
#define ag_decoration_column_config_circle		2

//
// textures
//

#define ag_texture_wall							0
#define ag_texture_floor						1
#define ag_texture_ceiling						2
#define ag_texture_wall_2						3
#define ag_texture_floor_2						4
#define ag_texture_ceiling_2					5
#define ag_texture_connect						6
#define ag_texture_alt_wall						7
#define ag_texture_second_floor					8
#define ag_texture_lift							9
#define ag_texture_door							10
#define ag_texture_stair						11
#define ag_texture_decoration_pillar			12
#define ag_texture_decoration_box				13
#define ag_texture_equipment					14
#define ag_texture_window						15

#define ag_texture_skybox_top					26
#define ag_texture_skybox_bottom				27
#define ag_texture_skybox_north					28
#define ag_texture_skybox_south					29
#define ag_texture_skybox_east					30
#define ag_texture_skybox_west					31

//
// counts, dimensions and positions
//

#define ag_room_start_count						20
#define ag_room_extra_count						20

#define ag_flat_connector_min_width				10000

#define ag_size_room_width_start				20000
#define ag_size_room_width_extra				10000

#define ag_map_bottom_start_y					20000
#define ag_size_room_high						10000
#define ag_size_floor_high						1000

#define ag_size_door_min_width					25000
#define ag_size_door_margin						400

#define ag_size_stair_length_start				25000
#define ag_size_stair_length_extra				10000
#define ag_size_stair_count						15
#define ag_size_story_stair_tread_len			1200

#define ag_size_pillar_wid						1500
#define ag_size_bump_high						400

#define ag_size_stack_box						2500
#define ag_count_stack_box_row_start			10
#define ag_count_stack_box_row_extra			10
#define ag_count_stack_box_row_min				2
#define ag_count_stack_box_high_max				3

#define ag_size_column_normal_wid				1500
#define ag_size_column_vertex_wid_start			2000
#define ag_size_column_vertex_wid_extra			1000

#define ag_size_equipment_pad_wid				8000
#define ag_size_equipment_computer_wid			7000
#define ag_size_equipment_high_start			5000
#define ag_size_equipment_high_extra			2000

#define ag_size_trench_high						800

#define ag_count_rubble_start					10
#define ag_count_rubble_extra					30
#define ag_size_rubble_start					200
#define ag_size_rubble_extra					800

#define ag_size_machinery_start					2000
#define ag_size_machinery_extra					2000

#define ag_size_light_width_start				50000
#define ag_size_light_width_extra				15000
#define ag_size_light_fixture_wid				500

#define ag_window_min_wid						10000
#define ag_window_min_high						10000

#define ag_window_random_percent				30
#define ag_window_count_start					2
#define ag_window_count_extra					3
#define ag_window_depth							3000

//
// structures
//

typedef struct		{
						int							p1_idx,p2_idx;
						bool						on;
					} ag_room_flat_type;

typedef struct		{
						ag_room_flat_type			lft,rgt,top,bot;
					} ag_room_flat_side_type;

typedef struct		{
						int							connect_type,side,
													other_room_idx;
						bool						on;
						d3pnt						min,max;
					} ag_room_connect_box_type;

typedef struct		{
						int							nvertex,mesh_idx;
						bool						outside,second_story,
													second_story_complete,
													require_top_floor,
													has_stairs,has_windows,
													has_wall_decorations;
						d3pnt						min,max;
						d3pnt						vertexes[8];
						d3vct						size;
						ag_room_flat_side_type		flat;
						ag_room_connect_box_type	connect_box;
					} ag_room_type;

typedef struct		{
						int							nroom,
													current_door_idx;
						d3pnt						start_pnt;
						ag_room_type				*rooms;
					} ag_map_state_type;

//
// functions
//

extern bool ag_map_initialize(char *err_str);
extern void ag_map_release(void);

extern void ag_map_add_connector_rooms(map_type *map);
extern void ag_map_add_room_second_story(map_type *map);
extern void ag_map_windows_add(map_type *map);
extern void ag_map_decorations_add(map_type *map);
extern void ag_map_lights_add(map_type *map);
extern void ag_map_spots_add(map_type *map);

extern void ag_map_delete_shared_polygons(map_type *map);
extern void ag_map_remove_polygons_in_box(map_type *map,int mesh_idx,d3pnt *min,d3pnt *max);

extern int ag_map_find_floor_polygon(map_type *map,int room_idx);
extern int ag_map_find_ceiling_polygon(map_type *map,int room_idx);
extern bool ag_map_is_polygon_window_target(map_type *map,int mesh_idx,int poly_idx);
extern void ag_map_get_wall_line(map_type *map,int mesh_idx,int poly_idx,d3pnt *p1,d3pnt *p2);
extern bool ag_map_room_is_leaf(int room_idx);
extern void ag_map_add_connector_room_step(map_type *map,d3pnt *min,d3pnt *max);
extern bool ag_map_room_surrounded_by_second_stories(int room_idx);

extern void ag_map_primitive_cube(map_type *map,d3pnt *min,d3pnt *max,bool rotate,int group_idx,int txt_idx);
extern void ag_map_primitive_rubble(map_type *map,d3pnt *min,d3pnt *max,int txt_idx);
extern void ag_map_primitive_random_cylinder_setup(map_type *map);
extern void ag_map_primitive_random_cylinder_y(map_type *map,d3pnt *pnt,int radius,int high,int txt_idx);

extern void ag_map_lights_add_spot(map_type *map,d3pnt *pnt);
