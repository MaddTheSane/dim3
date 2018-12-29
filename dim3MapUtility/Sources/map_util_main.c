/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Main Routines

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

extern file_path_setup_type	file_path_setup;

/* =======================================================

      New Maps
      
======================================================= */

bool map_new(map_type *map,char *name)
{
	int				n;

		// info
		
	strcpy(map->info.name,name);
	map->info.title[0]=0x0;
	map->info.author[0]=0x0;
	
	file_paths_data_default(&file_path_setup,map->info.load_path,"Maps",map->info.name,"xml");
	
		// physics
		
	map->physics.gravity=1;
	map->physics.gravity_max_power=32.0f;
	map->physics.gravity_max_speed=400.0f;
	map->physics.resistance=1.0f;
	map->physics.slope_min_ang=25.0f;
	map->physics.slope_max_ang=45.0f;
	map->physics.slope_max_speed=250.0f;
	map->physics.slope_min_gravity=200.0f;
	
		// settings

	map->settings.params[0]=0x0;

	map->singleplayer.map_picker=TRUE;
	map->multiplayer.game_list[0]=0x0;
	
		// optimization
		
	map->optimize.never_cull=FALSE;
	map->optimize.cull_angle=0.0f;
	map->optimize.ray_trace_obscure=FALSE;
	map->optimize.obscure_dist.model=0;
	map->optimize.obscure_dist.shadow=0;
	map->optimize.obscure_dist.effect=0;
	map->optimize.shadow_poly_min_area=0;
	map->optimize.shadow_floor_only=FALSE;

		// light maps

	map->light_map.quality=2;
	map->light_map.size=1;
	map->light_map.use_normals=FALSE;
	map->light_map.skip_glows=FALSE;
	map->light_map.diffuse_boost=0.0f;
	
		// editor setup

	map->editor_setup.view_near_dist=400;
	map->editor_setup.view_far_dist=300000;
	map->editor_setup.link_always_start=TRUE;

		// camera

	map->camera.camera_mode=cv_fpp;
	map->camera.input_mode=im_fpp;
	
	map->camera.pnt_offset.x=0;
	map->camera.pnt_offset.y=0;
	map->camera.pnt_offset.z=0;

	map->camera.ang_offset.x=0.0f;
	map->camera.ang_offset.y=0.0f;
	map->camera.ang_offset.z=0.0f;
    
    map->camera.chase.distance=8000;
	map->camera.chase.track_speed=1.0f;

  	map->camera.chase.track_ang.x=0.0f;
	map->camera.chase.track_ang.y=0.0f;
	map->camera.chase.track_ang.z=0.0f;
  
	map->camera.chase.slop.x=0.0f;
	map->camera.chase.slop.y=0.0f;
	map->camera.chase.slop.z=0.0f;
    
    map->camera.c_static.follow=TRUE;
	map->camera.c_static.attach_node[0]=0x0;

	map->camera.plane.fov=60.0f;
	map->camera.plane.aspect_ratio=1.0f;
	map->camera.plane.near_z=400;
	map->camera.plane.far_z=300000;
	map->camera.plane.near_z_offset=-400;
	
		// media
		
	map->media.type=mi_none;
	map->media.event_id=-1;
	map->media.name[0]=0x0;
	map->media.title_sound_name[0]=0x0;
	
		// music
		
	map->music.fade_msec=0;
	map->music.name[0]=0x0;

	for (n=0;n!=max_music_preload;n++) {
		map->music.preload_name[n][0]=0x0;
	}
	
		// ambients
		
	map->ambient.light_color.r=map->ambient.light_color.g=map->ambient.light_color.b=1.0f;
	map->ambient.sound_name[0]=0x0;
	map->ambient.sound_pitch=1.0f;
	
		// rain
		
	map->rain.on=FALSE;
	map->rain.density=200;
	map->rain.radius=40000;
	map->rain.height=15000;
	map->rain.speed=35;
	map->rain.line_length=1500;
	map->rain.line_width=1;
	map->rain.slant_add=20;
	map->rain.slant_time_msec=8000;
	map->rain.slant_change_msec=1000;
	map->rain.alpha=0.3f;
	map->rain.start_color.r=map->rain.start_color.g=map->rain.start_color.b=1.0f;
	map->rain.end_color.r=map->rain.end_color.g=0.2f;
	map->rain.end_color.b=1.0f;
    
		// background
		
	map->background.on=FALSE;
	map->background.front.fill=-1;
	map->background.front.size.x=1.0f;
	map->background.front.size.y=1.0f;
	map->background.front.scroll_factor.x=0.0f;
	map->background.front.scroll_factor.y=0.0f;
	map->background.front.clip.x=0.0f;
	map->background.front.clip.y=1.0f;
	map->background.middle.fill=-1;
	map->background.middle.size.x=1.0f;
	map->background.middle.size.y=1.0f;
	map->background.middle.scroll_factor.x=0.0f;
	map->background.middle.scroll_factor.y=0.0f;
	map->background.middle.clip.x=0.0f;
	map->background.middle.clip.y=1.0f;
	map->background.back.fill=-1;
	map->background.back.size.x=1.0f;
	map->background.back.size.y=1.0f;
	map->background.back.scroll_factor.x=0.0f;
	map->background.back.scroll_factor.y=0.0f;
	map->background.back.clip.x=0.0f;
	map->background.back.clip.y=1.0f;
	
		// sky
		
	map->sky.on=FALSE;
	map->sky.type=st_cube;
    map->sky.fill=-1;
    map->sky.bottom_fill=-1;
    map->sky.north_fill=-1;
    map->sky.south_fill=-1;
    map->sky.east_fill=-1;
    map->sky.west_fill=-1;
    map->sky.radius=43000;
	map->sky.dome_y=0;
	map->sky.dome_mirror=FALSE;
	map->sky.txt_fact=1.0f;
	
		// fog
		
	map->fog.on=FALSE;
	map->fog.count=30;
	map->fog.outer_radius=150000;
	map->fog.inner_radius=75000;
	map->fog.high=22000;
	map->fog.drop=7500;
	map->fog.texture_idx=0;
	map->fog.speed=0.001f;
	map->fog.txt_fact.x=8.0f;
	map->fog.txt_fact.y=1.0f;
	map->fog.col.r=0.5;
	map->fog.col.g=0.5;
	map->fog.col.b=0.5;
	map->fog.alpha=0.05f;
	map->fog.use_solid_color=TRUE;

		// editor setup

	map->editor_views.count=0;
		
		// pieces
	
	map->nspot=0;
	map->nnode=0;
	map->nscenery=0;
	map->nlight=0;
	map->nsound=0;
	map->nparticle=0;

		// meshes and liquids

	map->mesh.nmesh=0;
	map->mesh.meshes=NULL;

	map->liquid.nliquid=0;
	map->liquid.liquids=NULL;
		
		// groups, movements, cinemas
		
	map->group.ngroup=0;
	map->group.groups=NULL;
	
	map->movement.nmovement=0;
	map->movement.movements=NULL;

	map->cinema.ncinema=0;
	map->cinema.cinemas=NULL;
	
		// memory
		
	map->textures=(texture_type*)malloc(max_map_texture*sizeof(texture_type));
	if (map->textures==NULL) return(FALSE);
	
	map->spots=(spot_type*)malloc(max_spot*sizeof(spot_type));
	if (map->spots==NULL) return(FALSE);
	
	map->nodes=(node_type*)malloc(max_node*sizeof(node_type));
	if (map->nodes==NULL) return(FALSE);
	
	map->sceneries=(map_scenery_type*)malloc(max_map_scenery*sizeof(map_scenery_type));
	if (map->sceneries==NULL) return(FALSE);
	
	map->lights=(map_light_type*)malloc(max_map_light*sizeof(map_light_type));
	if (map->lights==NULL) return(FALSE);
	
	map->sounds=(map_sound_type*)malloc(max_map_sound*sizeof(map_sound_type));
	if (map->sounds==NULL) return(FALSE);
	
	map->particles=(map_particle_type*)malloc(max_map_particle*sizeof(map_particle_type));
	if (map->particles==NULL) return(FALSE);

		// zero memory
		
	bzero(map->textures,(max_map_texture*sizeof(texture_type)));
	bzero(map->spots,(max_spot*sizeof(spot_type)));
	bzero(map->nodes,(max_node*sizeof(node_type)));
	bzero(map->sceneries,(max_map_scenery*sizeof(map_scenery_type)));
	bzero(map->lights,(max_map_light*sizeof(map_light_type)));
	bzero(map->sounds,(max_map_sound*sizeof(map_sound_type)));
	bzero(map->particles,(max_map_particle*sizeof(map_particle_type)));
	
		// bitmaps
		
	map_textures_new(map);
		
	return(TRUE);
}

/* =======================================================

      Open Maps
      
======================================================= */

bool map_open(map_type *map,char *name)
{
	if (!map_new(map,name)) return(FALSE);
	
	strcpy(map->info.name,name);
	file_paths_data(&file_path_setup,map->info.load_path,"Maps",map->info.name,"xml");
	
	if (!read_map_xml(map)) return(FALSE);

	map_find_random_spot_clear(map,NULL,-1);
	
	return(TRUE);
}

/* =======================================================

      Reload Maps
      
======================================================= */

bool map_reload(map_type *map)
{
	if (!read_map_xml(map)) return(FALSE);

	return(TRUE);
}

/* =======================================================

      Save Maps
      
======================================================= */

bool map_save(map_type *map,char *err_str)
{
    return(write_map_xml(map,err_str));
}

/* =======================================================

      Close Maps
      
======================================================= */

void map_close(map_type *map)
{
	int					n;

		// bitmaps
		
	map_textures_close(map);

		// meshes and liquids

	if (map->mesh.meshes!=NULL) {
		for (n=0;n!=map->mesh.nmesh;n++) {
			if (map->mesh.meshes[n].polys!=NULL) free(map->mesh.meshes[n].polys);
		}

		free(map->mesh.meshes);
		map->mesh.nmesh=0;
		map->mesh.meshes=NULL;
	}

	if (map->liquid.liquids!=NULL) {
		free(map->liquid.liquids);
		map->liquid.nliquid=0;
		map->liquid.liquids=NULL;
	}
	
		// groups, movements, cinemas

	if (map->group.groups!=NULL) {
		free(map->group.groups);
		map->group.ngroup=0;
		map->group.groups=NULL;
	}

	if (map->movement.movements!=NULL) {
		for (n=0;n!=map->movement.nmovement;n++) {
			if (map->movement.movements[n].moves!=NULL) free(map->movement.movements[n].moves);
		}

		free(map->movement.movements);
		map->movement.nmovement=0;
		map->movement.movements=NULL;
	}
		
	if (map->cinema.cinemas!=NULL) {
		for (n=0;n!=map->cinema.ncinema;n++) {
			if (map->cinema.cinemas[n].actions!=NULL) free(map->cinema.cinemas[n].actions);
		}

		free(map->cinema.cinemas);
		map->cinema.ncinema=0;
		map->cinema.cinemas=NULL;
	}
	
		// memory
		
	free(map->textures);
	free(map->spots);
	free(map->nodes);
	free(map->sceneries);
	free(map->lights);
	free(map->sounds);
	free(map->particles);
}

/* =======================================================

      Refresh Textures
      
======================================================= */

void map_refresh_textures(map_type *map)
{
	map_textures_close(map);
	map_textures_read_complete(map);
}

