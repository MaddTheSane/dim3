/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Main Loop

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
#include "network.h"
#include "objects.h"
#include "scripts.h"

extern app_type				app;
extern map_type				map;
extern server_type			server;
extern view_type			view;
extern setup_type			setup;
extern network_setup_type	net_setup;

/* =======================================================

      Main Game Loop Run
      
======================================================= */

bool loop_view_input()
{
		// view input

	view_loop_input();

		// if state has changed to intro,
		// then we must be exiting a running
		// game
			
	if (server.next_state==gs_intro) {
		map_end();
		game_end();
		return(FALSE);
	}
	
		// any other state change
		// means an early exit
		
	return(server.next_state==gs_running);
}

void loop_server_run(void)
{
		// networking

	switch (net_setup.mode) {

		case net_mode_client:
			if (!net_client_process_messages()) return;
			break;

		case net_mode_host:
			net_host_process_messages();
			break;

	}
	
		// run server

	timers_run();
	server_run();
	
		// sending network updates
		// only clients send group synchs
		// and latency pings
		
	switch (net_setup.mode) {
	
		case net_mode_client:
			remote_network_send_updates();
			remote_network_send_group_synch();
			break;
			
		case net_mode_host:
			net_host_player_send_updates();
			break;
	}
	
		// check for score limits
		
	score_limit_check_scores();
}

void loop_view_run()
{
		// draw the view

	view_run();
	view_loop_draw();
	
		// calculate fps

	view_calculate_fps();
}

/* =======================================================

      App Activate/Deactivate
      
======================================================= */

	// phone/pad versions react differently
	// to these events by going saving
	// the state and turning off the game

#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)

void SDL_iOSEvent_WillTerminate(void)
{
	app.loop_quit=true;
}

void SDL_iOSEvent_DidReceiveMemoryWarning(void)
{
	// nothing to do as resigning always cleans everything up
}

void SDL_iOSEvent_WillResignActive(void)
{
	app.state=as_suspended;

	input_clear();
	input_mouse_pause();
		
	SDL_PauseAudio(1);
}

void SDL_iOSEvent_DidEnterBackground(void)
{
	if (server.state!=gs_intro) {
		if (server.map_open) map_end();
		if (server.game_open) game_end();
	}
}

void SDL_iOSEvent_WillEnterForeground(void)
{
	if (server.state!=gs_intro) {
		server.state=gs_intro;
		intro_open();
	}
}

void SDL_iOSEvent_DidBecomeActive(void)
{
	input_clear();
	input_mouse_resume();
	
	SDL_PauseAudio(0);
	
	app.state=as_active;
}

int loop_event_callback(void *userdata,SDL_Event *event)
{

	switch (event->type) {

		case SDL_APP_TERMINATING:
			SDL_iOSEvent_WillTerminate();
			return(0);
			
		case SDL_APP_LOWMEMORY:
			SDL_iOSEvent_DidReceiveMemoryWarning();
			return(0);
			
		case SDL_APP_WILLENTERBACKGROUND:
			SDL_iOSEvent_WillResignActive();
			return(0);
			
		case SDL_APP_DIDENTERBACKGROUND:
			SDL_iOSEvent_DidEnterBackground();
			return(0);
			
		case SDL_APP_WILLENTERFOREGROUND:
			SDL_iOSEvent_WillEnterForeground();
			return(0);
			
		case SDL_APP_DIDENTERFOREGROUND:
			SDL_iOSEvent_DidBecomeActive();
			return(0);

	}
	
	return(1);
}

void loop_app_active(void)
{
	// loop_app_active is just blank here
}

#else

	// regular computers just
	// pause the game

void loop_app_active(void)
{
		// only windowed versions can go inactive

	if (!gl_in_window_mode()) return;

		// going inactive?

	if (app.state==as_active) {
		if (input_app_active()) return;

		app.state=as_inactive;

		input_clear();
		input_mouse_pause();
		
		SDL_PauseAudio(1);

			// don't pause if this game is a host

		if (net_setup.mode!=net_mode_host) game_time_pause_start();

		return;
	}

		// becoming active?

	if (!input_app_active()) return;

	app.state=as_active;
	
	input_clear();
	input_mouse_resume();
	
	SDL_PauseAudio(0);

		// host games weren't paused

	if (net_setup.mode!=net_mode_host) game_time_pause_end();
}

#endif

/* =======================================================

      State Loops
      
======================================================= */

void loop_state_run(void)
{
	switch (server.state) {
	
		case gs_running:
			if (loop_view_input()) {
				loop_server_run();
				loop_view_run();
			}
			return;

		case gs_intro:
			intro_run();
			return;

		case gs_singleplayer_option:
			singleplayer_option_run();
			return;

		case gs_simple_save_pick:
			simple_save_pick_run();
			return;
			
		case gs_setup_game:
			setup_game_run();
			return;
			
		case gs_join:
			join_run();
			return;

		case gs_host:
			host_run();
			return;
			
		case gs_file:
			file_run();
			return;
			
		case gs_chooser:
			chooser_run();
			return;
			
		case gs_title:
			title_run();
			return;

		case gs_load_pause:
			load_pause_run();
			return;
			
		case gs_error:
			error_run();
			return;

		case gs_score_limit:
			score_limit_run();
			return;
			
	}
}

void loop_state_last_close(void)
{
	view_clear_fps();
	
	switch (server.state) {

		case gs_intro:
			intro_close();
			return;

		case gs_singleplayer_option:
			singleplayer_option_close();
			return;

		case gs_simple_save_pick:
			simple_save_pick_close();
			return;

		case gs_setup_game:
			setup_game_close();
			return;
			
		case gs_join:
			join_close();
			return;

		case gs_host:
			host_close();
			return;
			
		case gs_file:
			file_close();
			return;

		case gs_chooser:
			chooser_close();
			return;
			
		case gs_title:
			title_close();
			return;

		case gs_load_pause:
			load_pause_close();
			return;
			
		case gs_error:
			error_close();
			return;

		case gs_score_limit:
			score_limit_close();
			return;

	}
}

void loop_state_next_open(void)
{
	switch (server.state) {

		case gs_intro:
			intro_open();
			return;

		case gs_singleplayer_option:
			singleplayer_option_open();
			return;

		case gs_simple_save_pick:
			simple_save_pick_open();
			return;

		case gs_setup_game:
			setup_game_open();
			return;
			
		case gs_join:
			join_open();
			return;

		case gs_host:
			host_open();
			return;
			
		case gs_file:
			file_open();
			return;

		case gs_chooser:
			chooser_open();
			return;
			
		case gs_title:
			title_open();
			return;

		case gs_load_pause:
			load_pause_open();
			return;
			
		case gs_error:
			error_open();
			return;

		case gs_score_limit:
			score_limit_open();
			return;
	}
}

/* =======================================================

      Main Loop
      
======================================================= */

bool loop_main(char *err_str)
{
		// iOS suspended state
		
	if (app.state==as_suspended) {
		input_event_pump();
		usleep(1000);
		return(TRUE);
	}

		// pump the input
		// if there's an activation change, handle it
		
	if (input_event_pump()) {
		loop_app_active();
	}
	
	if (app.state==as_suspended) return(TRUE);
	
		// calculate timing
		
	game_time_calculate();
	
		// clear map changes
		// and checkpoints
	
	map_clear_changes();
	game_checkpoint_clear();

		// run proper game state
		
	server.next_state=server.state;
	
	loop_state_run();
	
		// switching states
		
	if (server.state!=server.next_state) {
		loop_state_last_close();
		
		server.last_state=server.state;
		server.state=server.next_state;

		loop_state_next_open();
	}
	
		// map and checkpoint changes
		// we need to check for state changes within
		// the map change (like media launching)
		
	if (server.state==gs_running) {
		if (map_need_rebuild()) {
		
			server.next_state=gs_running;
			
			if (!map_rebuild_changes(err_str)) return(FALSE);			// bad map changes is a fatal error
			
			if (server.next_state!=gs_running) {
				server.last_state=server.state;
				server.state=server.next_state;
				loop_state_next_open();
			}
			
		}

			// checkpoint changes

		game_checkpoint_run();
	}
	
	return(TRUE);
}

/* =======================================================

      Dedicated Loop
      
======================================================= */

bool loop_main_dedicated(char *err_str)
{
		// calculate timing
		
	game_time_calculate();
	
		// dedicated hosts can only
		// run servers or wait on score
		// limits (that reset game to new map)
		
	switch(server.state) {

			// regular server running

		case gs_running:
			loop_server_run();
			break;

			// score limit wait

		case gs_score_limit:
			if (game_time_get_raw()>score_limit_get_resume_tick()) {
				server.next_state=gs_running;
				if (!game_host_reset(err_str)) return(FALSE);
			}
			break;

	}

		// since this is just a host
		// let's sleep a bit so it doesn't
		// consume all the CPU (10 milliseconds)

	usleep(1000);
	
	return(TRUE);
}

