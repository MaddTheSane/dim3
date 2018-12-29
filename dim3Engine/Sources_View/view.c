/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View MainLine

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
#include "objects.h"

view_type					view;
view_render_type			view_camera_render,view_node_render;
render_info_type			render_info;

extern map_type				map;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

extern void game_file_initialize(void);
extern void menu_input(void);
extern void file_input(void);
extern void debug_input(void);
extern void view_draw(void);
extern void chat_clear_messages(void);
extern bool shadow_initialize(void);
extern void shadow_shutdown(void);
extern void menu_draw(void);
extern bool cocoa_is_ipad(void);

extern int loop_event_callback(void *userdata,SDL_Event *event);

/* =======================================================

      View Memory
      
======================================================= */

bool view_memory_allocate(void)
{
		// initialize pointers
		
	view.images=NULL;
	view.rain_draws=NULL;
	view.chat.lines=NULL;
	view.console.lines=NULL;
	
		// view pointers

	view.images=(view_image_type*)malloc(max_view_image*sizeof(view_image_type));
	if (view.images==NULL) return(FALSE);

	view.rain_draws=(rain_draw_type*)malloc(max_rain_density*sizeof(rain_draw_type));
	if (view.rain_draws==NULL) return(FALSE);
	
	view.chat.lines=(view_chat_line_type*)malloc(max_view_chat_lines*sizeof(view_chat_line_type));
	if (view.chat.lines==NULL) return(FALSE);
	
	view.console.lines=(view_console_line_type*)malloc(max_view_console_lines*sizeof(view_console_line_type));
	if (view.console.lines==NULL) return(FALSE);

		// clear pointers

	bzero(view.images,(max_view_image*sizeof(view_image_type)));
	bzero(view.rain_draws,(max_rain_density*sizeof(rain_draw_type)));
	bzero(view.chat.lines,(max_view_chat_lines*sizeof(view_chat_line_type)));
	bzero(view.console.lines,(max_view_console_lines*sizeof(view_console_line_type)));

		// no utility VBOs yet

	view_initialize_utility_vertex_object();
			
	return(TRUE);
}

void view_memory_release(void)
{
	if (view.images!=NULL) free(view.images);
	if (view.rain_draws!=NULL) free(view.rain_draws);
	if (view.chat.lines!=NULL) free(view.chat.lines);
	if (view.console.lines!=NULL) free(view.console.lines);
}

/* =======================================================

      Screen Size List
      
======================================================= */

void view_create_screen_size_list(void)
{
	int				n,k,i,nmode;
	float			ratio;
	bool			hit;
	SDL_DisplayMode	mode;
	
	nmode=SDL_GetNumDisplayModes(0);
	
		// if no modes, then 640x480 is the only mode
		
	if (nmode==0) {
		render_info.screen_sizes[0].wid=640;
		render_info.screen_sizes[0].high=480;
		render_info.nscreen_size=1;
		return;
	}
	
		// create screen list
		
	k=0;
	
	for (n=(nmode-1);n>=0;n--) {
	
		SDL_GetDisplayMode(0,n,&mode);

			// knock out any less than 640x480 or when height >= width

		if (mode.w<640) continue;
		if (mode.h<480) continue;
		if (mode.h>=mode.w) continue;

			// is this screen already in list?

		hit=FALSE;

		for (i=0;i!=k;i++) {
			if ((render_info.screen_sizes[i].wid==mode.w) && (render_info.screen_sizes[i].high==mode.h)) {
				hit=TRUE;
				break;
			}
		}

			// add to list if ratio is equal or
			// better than 4:3 (0.75) and not less
			// then 1:85:1 (0.54)

		if (!hit) {
			ratio=(float)mode.h/(float)mode.w;
			if ((ratio>=0.54) && (ratio<=0.75f)) {
				render_info.screen_sizes[k].wid=mode.w;
				render_info.screen_sizes[k].high=mode.h;

				k++;
				if (k>=max_screen_size) break;
			}
		}
	}
	
	render_info.nscreen_size=k;
}

/* =======================================================

      View Initialize and Shutdown Display
      
======================================================= */

bool view_initialize_display(char *err_str)
{
	int				n;
	bool			ok;

		// is screen size legal?
		// if not, go back to default
		
	if (setup.screen_wid!=-1) {
		ok=FALSE;
			
		for (n=0;n!=render_info.nscreen_size;n++) {
			if ((render_info.screen_sizes[n].wid==setup.screen_wid) && (render_info.screen_sizes[n].high==setup.screen_high)) {
				ok=TRUE;
				break;
			}
		}
		
		if (!ok) setup.screen_wid=setup.screen_high=-1;
	}

		// start openGL
		
	if (!gl_initialize(setup.screen_wid,setup.screen_high,setup.fsaa_mode,err_str)) {
		view_memory_release();
		SDL_Quit();
		return(FALSE);
	}

		// fix some OpenGL settings if not supported by card

	if (!gl_check_fsaa_ok()) setup.fsaa_mode=fsaa_mode_none;

		// utility vbo for a couple things
		// like primitives and bitmaps

	view_create_utility_vertex_object();

		// shadows

	if (!shadow_initialize()) {
		strcpy(err_str,"Out of Memory");
		gl_shutdown();
		SDL_Quit();
		return(FALSE);
	}
	
		// start the shaders
		
	gl_shader_initialize();

	if (!gl_simple_shader_initialize(err_str)) {
		gl_shutdown();
		SDL_Quit();
		return(FALSE);
	}

	if (!gl_core_shader_initialize(err_str)) {
		gl_shutdown();
		SDL_Quit();
		return(FALSE);
	}
		
	if (!gl_user_shader_initialize(err_str)) {
		gl_shutdown();
		SDL_Quit();
		return(FALSE);
	}

		// initialize text
	
	gl_text_initialize();
	
		// back renderer
		
	gl_back_render_initialize();

	return(TRUE);
}

void view_shutdown_display(void)
{
	view_dispose_utility_vertex_object();

	shadow_shutdown();
	gl_simple_shader_shutdown();
	gl_core_shader_shutdown();
	gl_user_shader_shutdown();
	gl_back_render_shutdown();
	gl_text_shutdown();
	gl_shutdown();
}

bool view_reset_display(char *err_str)
{
	view_shutdown_display();
	return(view_initialize_display(err_str));
}

/* =======================================================

      View Initialize and Shutdown Main
      
======================================================= */

bool view_initialize(char *err_str)
{
	int						tick;
	SDL_DisplayMode			sdl_mode;

		// clear view structure

	memset(&view,0x0,sizeof(view_type));

		// allocate memory
		
	if (!view_memory_allocate()) {
		view_memory_release();
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

		// start SDL

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK|SDL_INIT_NOPARACHUTE)==-1) {
		view_memory_release();
		sprintf(err_str,"SDL: Could not init (Error: %s)\n",SDL_GetError());
		return(FALSE);
	}
	
		// fix resolution if no switch is on
		// mobile platforms always turn off resolution switching
		
#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)
	iface.setup.no_resolution_switch=TRUE;
#endif

	if ((iface.setup.no_resolution_switch) && (!setup.window)) {
		setup.screen_wid=setup.screen_high=-1;
	}

		// determine and cache the
		// device type

#ifdef D3_OS_IPHONE
	if (cocoa_is_ipad()) {
		view.device_type=device_type_pad;
	}
	else {
		view.device_type=device_type_phone;
	}
#else
	#ifdef D3_OS_ANDRIOD
		view.device_type=device_type_phone;
	#else
		view.device_type=device_type_pc;
	#endif
#endif

		// get desktop screen size
		
	SDL_GetDesktopDisplayMode(0,&sdl_mode);
	render_info.desktop.wid=sdl_mode.w;
	render_info.desktop.high=sdl_mode.h;

		// create screen sizes
		
	view_create_screen_size_list();
	
		// gl initialize

	if (!view_initialize_display(err_str)) return(FALSE);

		// sound initialize
	
	if (!al_initialize(err_str)) {
		view_shutdown_display();
		view_memory_release();
		SDL_Quit();
		return(FALSE);
	}

	al_set_volume(setup.sound_volume);
	al_music_set_volume(setup.music_volume);
	al_music_set_state(setup.music_on);
	
		// if iOS, then make sure to
		// setup event capture so we can respond to
		// correct events
		
#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)
	SDL_SetEventFilter(loop_event_callback,0);
#endif
	
		// intialize the view bitmap lists
		
	view_images_initialize();
	
		// load the sounds
		
	al_load_all_xml_sounds();
	
		// connect the input
		
	input_initialize(gl_in_window_mode());
	setup_to_input();
	
		// hook up current render
		// to camera render -- some routines like face
		// forward need this to be valid
		
	view.render=&view_camera_render;

		// states

	view.menu.active=FALSE;
	view.score.on=FALSE;
	
	chat_clear_messages();
	
		// preloaded models
		
	model_preload_start();
	
		// draw and input timing
		// we use raw timing so input and
		// view can work through pauses
		
	tick=game_time_get_raw();

	view.time.input_tick=tick;
	view.time.draw_tick=tick;
	
	view.time.draw_time=1000/render_info.monitor_refresh_rate;
	
	return(TRUE);
}

void view_shutdown(void)
{
		// clear all preloaded models
		
	model_preload_free();
	
		// shutdown view

	input_shutdown();
	al_shutdown();
	view_images_shutdown();
	view_shutdown_display();

		// shutdown SDL
		
	SDL_Quit();
	
		// release memory

	view_memory_release();
}

/* =======================================================

      View Game Start and Stop
      
======================================================= */

bool view_game_start(char *err_str)
{
		// cameras and globals

	progress_update();
		
    camera_initialize();
	game_file_initialize();
	
		// load images for hud bitmaps, radar, particles,
		// rings, halos, marks, crosshairs and remote icons
	
	progress_update();
	view_images_cached_load();

	progress_update();

		// precalculate particles

	particle_precalculate_all();
	
		// clear chat messages
		
	chat_clear_messages();

		// turn off any checkpoint HUD items

	hud_checkpoint_show(FALSE);

	return(TRUE);
}

void view_game_stop(void)
{
		// stop any music
	
	al_music_stop();
	
		// free images for hud bitmaps, radar, particles,
		// rings, halos, marks, crosshairs and remote icons
	
	view_images_cached_free();
}

/* =======================================================

      Reset Timing when Loading Saved Game
      
======================================================= */

void view_game_reset_timing(void)
{
	int			tick;
	
	tick=game_time_get_raw();

	view.time.input_tick=tick;
	view.time.draw_tick=tick;
}

/* =======================================================

      View Wide Check
      
======================================================= */

bool view_file_paths_bitmap_check_wide(char *path,char *dir,char *name)
{
	char			name2[256];
	struct stat		sb;
	
		// check for wide
		
	if (view.screen.wide) {
		sprintf(name2,"%s_wide",name);
		file_paths_data(&file_path_setup,path,dir,name2,"png");
		if (stat(path,&sb)!=-1) return(TRUE);
	}
	
		// fall back to standard
		
	file_paths_data(&file_path_setup,path,dir,name,"png");
	return(stat(path,&sb)!=-1);
}

/* =======================================================

      View Loop Input
      
======================================================= */

void view_loop_input(void)
{
	int				raw_tick;

		// time to check input?
		// use raw ticks so it works through pauses
		
	raw_tick=game_time_get_raw();
	if (raw_tick<view.time.input_tick) return;

	view.time.input_tick=raw_tick+input_tick_rate;

		// player input
	
	player_get_input();
	
		// system input
	
	console_input();
	menu_input();
	file_input();
	debug_input();
}

/* =======================================================

      Normal Playing View Draw
      
======================================================= */

void view_loop_draw(void)
{
	int			raw_tick,tick,y_add;

		// time for view draw?
		// use raw ticks so it works through pauses
		
	raw_tick=game_time_get_raw();
	if ((!setup.ignore_fps_lock) && (raw_tick<view.time.draw_tick)) return;

	view.time.draw_tick=raw_tick+view.time.draw_time;

		// texture setup
	
	tick=game_time_get();

	map_setup_animated_textures(&map,tick);
	map_mesh_poly_run_shifts(&map,tick);

		// start frame
		
	gl_frame_clear(TRUE);
	gl_shader_frame_start();
	
		// squish for open console
		
	if (view.console.on) {
		#ifndef D3_ROTATE_VIEW
			y_add=(int)(((float)view.screen.y_sz)*console_screen_percent);
			gl_set_viewport(0,y_add,view.screen.x_sz,(view.screen.y_sz-y_add));
		#else
			y_add=(int)(((float)view.screen.y_sz)*console_screen_percent);
			gl_set_viewport(y_add,0,(view.screen.y_sz-y_add),view.screen.x_sz);
		#endif
	}
	
		// draw view

	view_draw();
	
	hud_draw();
	radar_draw();
	network_draw();
	metrics_draw();

		// virtual controls
		
#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)
	virtual_control_draw();
#endif

		// menu

	menu_draw();

		// restore if console open
		// and draw console
		
	if (view.console.on) {
		#ifndef D3_ROTATE_VIEW
			gl_set_viewport(0,0,view.screen.x_sz,view.screen.y_sz);
		#else
			gl_set_viewport(0,0,view.screen.y_sz,view.screen.x_sz);
		#endif
		console_draw();
	}

		// swap frame buffers
	
	gl_frame_swap();

	view.fps.count++;
}

/* =======================================================

      View Draw for Screen Captures
      
======================================================= */

void view_capture_draw(char *path)
{
	gl_frame_clear(FALSE);
	gl_shader_frame_start();

	view_draw();

	gl_screen_shot(0,0,view.screen.x_sz,view.screen.y_sz,TRUE,path);
}

