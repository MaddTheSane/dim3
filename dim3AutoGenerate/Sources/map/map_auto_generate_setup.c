/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Auto Generation Setup

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
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

#ifdef D3_PCH
	#include "dim3autogenerate.h"
#endif

extern ag_map_state_type		ag_map_state;

/* =======================================================

      Auto Generate Memory
      
======================================================= */

bool ag_map_initialize(char *err_str)
{
	ag_map_state.rooms=NULL;

	ag_map_state.rooms=(ag_room_type*)malloc(ag_max_room*sizeof(ag_room_type));
	if (ag_map_state.rooms==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	return(TRUE);
}

void ag_map_release(void)
{
	if (ag_map_state.rooms!=NULL) free(ag_map_state.rooms);
	ag_map_state.rooms=NULL;
}

/* =======================================================

      Create Map Textures
      
======================================================= */

texture_type* ag_map_create_texture(map_type *map,int idx,char *name,bool has_glow)
{
	int					n;
	texture_type		*texture;

		// close old texture

	map_textures_close_texture(map,idx);

		// start new texture

	texture=&map->textures[idx];
	
	sprintf(texture->material_name,name);
	texture->shader_name[0]=0x0;
	texture->additive=FALSE;
	texture->compress=FALSE;
	
	if (has_glow) {
		texture->glow.rate=2000;
		texture->glow.min=0.1f;
		texture->glow.max=0.3f;
	}

	for (n=0;n!=max_texture_frame;n++) {
		texture->frames[n].name[0]=0x0;
	}
	
	sprintf(texture->frames[0].name,"%s/%s",map->info.name,texture->material_name);

	return(texture);
}

void auto_generate_map_create_texture_set(map_type *map,char *base_path)
{
	texture_type		*texture;

	ag_random_seed();

	texture=ag_map_create_texture(map,ag_texture_wall,"Wall",FALSE);
	bitmap_ag_texture_brick(&texture->frames[0],base_path,512);
	map_textures_read_texture(map,ag_texture_wall);
	
	texture=ag_map_create_texture(map,ag_texture_floor,"Floor",FALSE);
	bitmap_ag_texture_tile(&texture->frames[0],base_path,512,TRUE);
	map_textures_read_texture(map,ag_texture_floor);

	texture=ag_map_create_texture(map,ag_texture_ceiling,"Ceiling",FALSE);
	bitmap_ag_texture_tile(&texture->frames[0],base_path,512,FALSE);
	map_textures_read_texture(map,ag_texture_ceiling);

	texture=ag_map_create_texture(map,ag_texture_wall_2,"Wall 2",FALSE);
	bitmap_ag_texture_brick(&texture->frames[0],base_path,512);
	map_textures_read_texture(map,ag_texture_wall_2);
	
	texture=ag_map_create_texture(map,ag_texture_floor_2,"Floor 2",FALSE);
	bitmap_ag_texture_tile(&texture->frames[0],base_path,512,TRUE);
	map_textures_read_texture(map,ag_texture_floor_2);

	texture=ag_map_create_texture(map,ag_texture_ceiling_2,"Ceiling 2",FALSE);
	bitmap_ag_texture_tile(&texture->frames[0],base_path,512,FALSE);
	map_textures_read_texture(map,ag_texture_ceiling_2);

	texture=ag_map_create_texture(map,ag_texture_connect,"Connect",FALSE);
	bitmap_ag_texture_concrete_block(&texture->frames[0],base_path,512);
	map_textures_read_texture(map,ag_texture_connect);

	texture=ag_map_create_texture(map,ag_texture_alt_wall,"Alt Wall",FALSE);
	bitmap_ag_texture_brick(&texture->frames[0],base_path,512);
	map_textures_read_texture(map,ag_texture_alt_wall);

	texture=ag_map_create_texture(map,ag_texture_second_floor,"Second Story",FALSE);
	bitmap_ag_texture_brick_interlaced(&texture->frames[0],base_path,512);
	map_textures_read_texture(map,ag_texture_second_floor);

	texture=ag_map_create_texture(map,ag_texture_lift,"Lift",FALSE);
	bitmap_ag_texture_metal(&texture->frames[0],base_path,512,FALSE);
	map_textures_read_texture(map,ag_texture_lift);

	texture=ag_map_create_texture(map,ag_texture_door,"Door",FALSE);
	bitmap_ag_texture_metal(&texture->frames[0],base_path,512,TRUE);
	map_textures_read_texture(map,ag_texture_door);

	texture=ag_map_create_texture(map,ag_texture_stair,"Stairs",FALSE);
	bitmap_ag_texture_cement(&texture->frames[0],base_path,512,TRUE);
	map_textures_read_texture(map,ag_texture_stair);

	texture=ag_map_create_texture(map,ag_texture_decoration_pillar,"Pillar",FALSE);
	bitmap_ag_texture_cement(&texture->frames[0],base_path,512,FALSE);
	map_textures_read_texture(map,ag_texture_decoration_pillar);

	texture=ag_map_create_texture(map,ag_texture_decoration_box,"Box",FALSE);
	bitmap_ag_texture_wood(&texture->frames[0],base_path,512);
	map_textures_read_texture(map,ag_texture_decoration_box);

	texture=ag_map_create_texture(map,ag_texture_equipment,"Equipment",TRUE);
	bitmap_ag_texture_machine(&texture->frames[0],base_path,512);
	map_textures_read_texture(map,ag_texture_equipment);

	texture=ag_map_create_texture(map,ag_texture_window,"Window",FALSE);
	bitmap_ag_texture_window(&texture->frames[0],base_path,512);
	map_textures_read_texture(map,ag_texture_window);

	bitmap_ag_texture_skybox_start(TRUE);

	texture=ag_map_create_texture(map,ag_texture_skybox_top,"SkyTop",FALSE);
	bitmap_ag_texture_skybox_top(&texture->frames[0],base_path,512);
	map_textures_read_texture(map,ag_texture_skybox_top);

	texture=ag_map_create_texture(map,ag_texture_skybox_bottom,"SkyBottom",FALSE);
	bitmap_ag_texture_skybox_bottom(&texture->frames[0],base_path,512);
	map_textures_read_texture(map,ag_texture_skybox_bottom);

	texture=ag_map_create_texture(map,ag_texture_skybox_north,"SkyNorth",FALSE);
	bitmap_ag_texture_skybox_north(&texture->frames[0],base_path,512);
	map_textures_read_texture(map,ag_texture_skybox_north);

	texture=ag_map_create_texture(map,ag_texture_skybox_south,"SkySouth",FALSE);
	bitmap_ag_texture_skybox_south(&texture->frames[0],base_path,512);
	map_textures_read_texture(map,ag_texture_skybox_south);

	texture=ag_map_create_texture(map,ag_texture_skybox_east,"SkyEast",FALSE);
	bitmap_ag_texture_skybox_east(&texture->frames[0],base_path,512);
	map_textures_read_texture(map,ag_texture_skybox_east);

	texture=ag_map_create_texture(map,ag_texture_skybox_west,"SkyWest",FALSE);
	bitmap_ag_texture_skybox_west(&texture->frames[0],base_path,512);
	map_textures_read_texture(map,ag_texture_skybox_west);

	map->sky.fill=ag_texture_skybox_top;
	map->sky.bottom_fill=ag_texture_skybox_bottom;
	map->sky.north_fill=ag_texture_skybox_north;
	map->sky.south_fill=ag_texture_skybox_south;
	map->sky.east_fill=ag_texture_skybox_east;
	map->sky.west_fill=ag_texture_skybox_west;
}
