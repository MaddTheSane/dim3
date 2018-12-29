/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Start/Stop

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "scripts.h"
#include "objects.h"

extern app_type				app;
extern map_type				map;
extern view_type			view;
extern server_type			server;
extern js_type				js;
extern setup_type			setup;
extern iface_type			iface;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

char						current_map_name[name_str_len];

extern void map_movements_initialize(void);
extern void group_move_clear_all(void);
extern void map_multiplayer_show_hide_meshes(void);

/* =======================================================

      Map Based Media and Music
      
======================================================= */

void map_media_start(map_media_type *media)
{
	char			err_str[256];
	
		// no media when launched from editor

	if (app.editor_override.on) return;

		// no media if last change was a skip change

	if (server.map_change.skip_media) {
		server.map_change.skip_media=FALSE;
		return;
	}

		// run the media type

	switch (media->type) {
	
		case mi_chooser:
			if (!chooser_setup(media->name,NULL,err_str)) {
				console_add_error(err_str);
			}
			break;
			
		case mi_title:
			if (!title_setup(gs_running,"Titles",media->name,media->title_sound_name,-1,media->event_id,err_str)) {
				console_add_error(err_str);
			}
			break;
			
		case mi_cinema:
			if (!cinema_start(media->name,media->event_id,err_str)) {
				console_add_error(err_str);
			}
			break;
		
	}
}

void map_music_start(map_music_type *music)
{
	bool			ok;
	char			err_str[256];
	
	if (!setup.music_on) return;
	if (music->name[0]==0x0) return;
	
		// stop old music
		
	if (al_music_playing()) al_music_stop();
	
		// start new music
		
	al_music_set_loop(TRUE);
	
	if (music->fade_msec==0) {
		ok=al_music_play(music->name,err_str);
	}
	else {
		ok=al_music_fade_in(music->name,music->fade_msec,err_str);
	}
	
		// report any errors
		
	if (!ok) console_add_error(err_str);
}

void map_start_finish(bool skip_media)
{
	if (!skip_media) map_media_start(&map.media);
	map_music_start(&map.music);
	if (!skip_media) view_fade_start();
}

/* =======================================================

      Cache Lookups and Map Draw Flags
      
======================================================= */

void map_lookups_setup(void)
{
	int					n;
	map_liquid_type		*liq;
	map_light_type		*lit;
	map_sound_type		*sound;
	map_particle_type	*particle;
	spot_type			*spot;
	
		// liquids sound buffer

	liq=map.liquid.liquids;
	
	for (n=0;n!=map.liquid.nliquid;n++) {
		liq->ambient.buffer_idx=al_find_buffer(liq->ambient.sound_name);
		liq++;
	}

		// map light halos

	lit=map.lights;

	for (n=0;n!=map.nlight;n++) {
		lit->setting.halo_idx=iface_halo_find(&iface,lit->setting.halo_name);
		lit++;
	}

		// map sounds sound buffer

	sound=map.sounds;
	
	for (n=0;n!=map.nsound;n++) {
		sound->buffer_idx=al_find_buffer(sound->name);
		sound++;
	}
	
		// particle halos

	particle=map.particles;
	
	for (n=0;n!=map.nparticle;n++) {
		particle->particle_idx=particle_find_index(particle->name);
		particle->light_setting.halo_idx=iface_halo_find(&iface,particle->light_setting.halo_name);
		particle++;
	}

		// spot closes node

	spot=map.spots;

	for (n=0;n!=map.nspot;n++) {
		spot->lookup.nearest_node_idx=map_find_nearest_node_by_point(&map,&spot->pnt);
		spot++;
	}
}

/* =======================================================

      Map Start and End
      
======================================================= */

bool map_start(bool in_file_load,bool skip_media,char *err_str)
{
	int				tick;
	char			txt[256];
	obj_type		*obj;

	game_time_pause_start();

		// start progress
		
	progress_initialize(map.info.name);
	
	strcpy(current_map_name,map.info.name);		// remember for close
	
		// load the map
		
	sprintf(txt,"Opening %s",map.info.name);
	console_add_system(txt);
	
	progress_update();

	if (!map_open(&map,map.info.name)) {
		progress_shutdown();
		sprintf(err_str,"Could not open map: %s",map.info.name);
		return(FALSE);
	}

		// don't run blank maps

	if ((map.mesh.nmesh==0) || (map.nspot==0)) {
		map_close(&map);
		progress_shutdown();
		sprintf(err_str,"Map contains no meshes or spots: %s",map.info.name);
		return(FALSE);
	}
	
		// attach camera and shaders
		// to map and setup any full screen
		// shaders and node-based back renders
		// finally, pre-calc any map music

	progress_update();

	camera_map_setup();
	
	if (!app.dedicated_host) al_music_map_pre_cache();

		// prepare map surfaces
	
	progress_update();

	map_prepare(&map);
	map_multiplayer_show_hide_meshes();

		// map lists

	progress_update();

	if (!map_group_create_unit_list(&map)) {
		progress_shutdown();
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}
	
		// sky, fog, rain

	progress_update();

	if (!app.dedicated_host) {
		sky_init();
		rain_draw_init();

		map.rain.reset=TRUE;
	}

		// start map ambients
		// and clear all proj, effects, decals, etc
		
	progress_update();

	if (!app.dedicated_host) {
		map_start_ambient();
		if (map.ambient.sound_name[0]!=0x0) map_set_ambient(map.ambient.sound_name,map.ambient.sound_pitch);
	}

	progress_update();

	if (!projectile_initialize_list()) {
		progress_shutdown();
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}

	if (!effect_initialize_list()) {
		progress_shutdown();
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}

	progress_update();

	particle_map_initialize();
	group_move_clear_all();

        // run the course script
		// course scripts are the only
		// scripts which are NOT required
		// so check to see if file exists

	progress_update();

	js.course_script_idx=-1;

	if (file_paths_data_exist(&file_path_setup,"Scripts/Courses",map.info.name,"js")) {
	
		js.course_script_idx=scripts_add(thing_type_course,"Courses",map.info.name,-1,-1,-1,err_str);
		if (js.course_script_idx==-1) {
			progress_shutdown();
			return(FALSE);
		}

	}
			
		// send the construct event
	
	progress_update();

	if (!scripts_post_event(js.course_script_idx,-1,sd_event_construct,0,0,err_str)) {
		progress_shutdown();
		return(FALSE);
	}

		// if not restoring an existing game,
		// create object and scenery

	progress_update();
	
	map_find_random_spot_clear(&map,NULL,-1);

	if (!in_file_load) {
		if (!map_objects_create(err_str)) {
			progress_shutdown();
			return(FALSE);
		}
	}

	progress_update();

	if (!in_file_load) {
		scenery_create();
		scenery_start();
	}

		// if not restoring a existing game,
		// spawn objects into map
		
	if (!in_file_load) {
		if (!map_object_attach_all(err_str)) {
			progress_shutdown();
			return(FALSE);
		}
	}
	
		// attach player to map

	progress_update();

	if (!app.dedicated_host) {

		player_clear_input();
		
			// connect camera to player
			// skip if we are reloading this map
			// to restore a saved game
			
		if (!in_file_load) {
			obj=server.obj_list.objs[server.player_obj_idx];
			camera_connect(obj);
		}
	}
	
		// initialize movements and lookups

	progress_update();
	
	map_movements_initialize();
	map_lookups_setup();
	
		// map start event
		// skip if we are reloading this map
		// to restore a saved game
		
	progress_update();

	if (!in_file_load) {
		scripts_post_event_console(js.game_script_idx,-1,sd_event_map,sd_event_map_open,0);
		scripts_post_event_console(js.course_script_idx,-1,sd_event_map,sd_event_map_open,0);

		if (!app.dedicated_host) scripts_post_event_console(obj->script_idx,-1,sd_event_map,sd_event_map_open,0);
	}
	
		// dim3rtl setup

	view_dim3rtl_map_mesh_start();
	view_dim3rtl_map_liquid_mesh_start();
	view_dim3rtl_map_model_mesh_start();

		// finish up
	
	progress_shutdown();
	
		// map open
		
	server.map_open=TRUE;
	
		// flag off all map changes
		
	map_clear_changes();
	
		// restart all the timing
		
	tick=game_time_get();
	
	map.start_game_tick=tick;
	server.time.run_tick=tick;
	server.time.network_update_tick=tick;
	server.time.network_group_synch_tick=tick;
	view.time.run_tick=tick;
	js.timer_tick=tick;

	if (!app.dedicated_host) view_clear_fps();

		// clear all input
		
	if (!app.dedicated_host) input_clear();
	
		// unpause game and start map timer
		
	game_time_pause_end();
	
	server.time.map_start_tick=game_time_get();

		// if not dedicated host, run
		// load pauses or map media

	if (!app.dedicated_host) {

			// if a pause start, go
			// directly to intro

		if ((iface.project.load_requires_click) && (!app.editor_override.on)) {
			load_pause_setup(map.info.name,skip_media);
		}
	
			// otherwise finish the map by running
			// media or sounds

		else {
			map_start_finish(skip_media);
		}
	}

	return(TRUE);
}

void map_end(void)
{
	obj_type		*obj;

		// stop all sounds
	
	if (!app.dedicated_host) {
		map_end_ambient();
		al_music_stop();
		al_stop_all_sources();
	}

		// pause timing
		
	game_time_pause_start();
	progress_initialize(map.info.name);
	
	console_add_system("Closing Map");
	
		// dim3rtl cleanup
	
	view_dim3rtl_map_liquid_mesh_stop();		// need to get rid of liquids first as map stop gets rid of materials
	view_dim3rtl_map_mesh_stop();
	view_dim3rtl_map_model_mesh_stop();
	
		// detach objects
		
	progress_update();
	
	map_object_detach_all();
	
		// map close event
	
	progress_update();
	
	scripts_post_event_console(js.game_script_idx,-1,sd_event_map,sd_event_map_close,0);
	scripts_post_event_console(js.course_script_idx,-1,sd_event_map,sd_event_map_close,0);

	if (!app.dedicated_host) {
		obj=server.obj_list.objs[server.player_obj_idx];
		scripts_post_event_console(obj->script_idx,-1,sd_event_map,sd_event_map_close,0);
	}

		// remove all projectiles
	
	progress_update();
	projectile_dispose_all();

		// free map bound items
		
	progress_update();
	object_dispose_2(bt_map);

		// clear all back buffers
		// release obscuring memory
		// and shutdown weather and skies

	if (!app.dedicated_host) {
		progress_update();
		sky_release();
		rain_draw_release();
	}

		// free some map lists

	progress_update();
	projectile_free_list();
	effect_free_list();

        // end course script
		
	scripts_dispose(js.course_script_idx);

		// free group lists

	map_group_dispose_unit_list(&map);
	
		// close map
	
	progress_update();
	map_close(&map);
		
		// map closed
		
	server.map_open=FALSE;
	
	progress_shutdown();
	
	game_time_pause_end();
}

/* =======================================================

      In Game Map changes
      
======================================================= */

void map_clear_changes(void)
{
	server.map_change.on=FALSE;
	server.map_change.skip_media=FALSE;
	server.map_change.player_restart=FALSE;
}

bool map_need_rebuild(void)
{
	return(server.map_change.on);
}

bool map_rebuild_changes(char *err_str)
{
	int				n,sub_event;
	obj_type		*obj;

		// if this is a restart, then check for
		// saved game to reload.  Reload games
		// are in paused mode so we need to flip it off

		// if it errors, just reload the map

	if (server.map_change.player_restart) {
		if (game_file_reload_ok()) {
			if (game_file_reload(err_str)) {
				game_time_pause_end();
				return(TRUE);
			}
		}
	}
	
		// end old map
		
	if (server.map_open) map_end();
	
		// reset all game bound objects spawn type
		// use map change if this is not a player restart
		
		// note, if this is being called from a network reset,
		// these will get over-ridden later down the line

	sub_event=server.map_change.player_restart?sd_event_spawn_init:sd_event_spawn_map_change;
		
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj!=NULL) {
			if (obj->bind==bt_game) obj->next_spawn_sub_event=sub_event;
		}
	}
		
		// start new map
	
	return(map_start(FALSE,FALSE,err_str));
}
