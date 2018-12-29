/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: App Start/Stop

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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "objects.h"
#include "network.h"

extern iface_type			iface;
extern server_type			server;
extern map_type				map;
extern setup_type			setup;
extern network_setup_type	net_setup;

app_type					app;
file_path_setup_type		file_path_setup;

/* =======================================================

      App Start and End
      
======================================================= */

bool app_start(char *err_str)
{
		// initialize timing

	game_time_initialize();
	
		// physics ray tracing
		
	if (!ray_trace_initialize(err_str)) return(FALSE);
	
		// OS network initialization
		
	net_initialize();
	
		// read setup preferences
		
	setup_xml_read();
	
		// client network defaults
		
	net_setup.mode=net_mode_none;
	
		// initialize server
		
	if (!server_initialize(err_str)) return(FALSE);

		// initialize view
		// if not running in dedicated host mode
	
	if (!app.dedicated_host) {

		if (!view_initialize(err_str)) {
			server_shutdown();
			return(FALSE);
		}

		console_initialize();
	}

	return(TRUE);
}

void app_end(void)
{
	if (server.map_open) map_end();
	if (server.game_open) game_end();
	
		// shut down app
		
	console_add_system("Closing App");
	
		// shutdown view
		// and ray tracing
		
	if (!app.dedicated_host) view_shutdown();
	
		// physics ray tracing
		
	ray_trace_shutdown();

		// shutdown server
		
	server_shutdown();
	
		// OS network shutdown
		
	net_shutdown();
}

/* =======================================================

      App Start and End
      
======================================================= */

bool app_run_intro(char *err_str)
{
		// start in intro or title media
		// we hard-set the state here as there
		// is no previous state
		
		// is there a title?

	if (iface.logo.name[0]!=0x0) {
		if (!title_setup(gs_intro,"Titles",iface.logo.name,iface.logo.sound,iface.logo.life_msec,-1,err_str)) return(FALSE);
		title_open();
		return(TRUE);
	}

		// if not go right to intro

	server.state=gs_intro;

	intro_open();

	return(TRUE);
}
	
bool app_run_editor_launch(char *err_str)
{
		// launch directly into map
		
	server.state=gs_running;
	net_setup.mode=net_mode_none;
		
	if (!game_start(FALSE,skill_medium,0,0,err_str)) return(FALSE);
	
	if (!map_start(FALSE,TRUE,err_str)) {
		game_end();
		return(FALSE);
	}
	
	return(TRUE);
}

bool app_run_dedicated_host(char *err_str)
{
	char			str[256];

		// launch directly into hosting
		// setup hosting flags and IPs
		
	host_game_setup();
	net_host_game_setup();

	net_setup.mode=net_mode_host;
	net_setup.client.latency=0;
	net_setup.client.host_addr.ip=0;
	net_setup.client.host_addr.port=0;
	
	net_create_project_hash();

		// setup map
		
	map.info.name[0]=0x0;
	strcpy(map.info.host_name,setup.network.map_list.maps[net_setup.host.current_map_idx].name);
	
		// start game
	
	if (!game_start(FALSE,skill_medium,0,0,err_str)) {
		net_host_game_end();
		return(FALSE);
	}
	
		// add any multiplayer bots
		
	if (!game_multiplayer_bots_create(err_str)) {
		game_end();
		net_host_game_end();
		return(FALSE);
	}
	
		// start the map
		
	if (!map_start(FALSE,TRUE,err_str)) {
		game_end();
		net_host_game_end();
		return(FALSE);
	}

		// start hosting

	if (!net_host_game_start(err_str)) {
		map_end();
		game_end();
		net_host_game_end();
		return(FALSE);
	}

		// dedicated hosting, no local
		// player to add, only add
		// multiplayer bots to host

	net_host_join_multiplayer_bots();

		// game is running

	sprintf(str,"Running on %s...",net_setup.host.ip_resolve);
	console_add(str);
	
	server.next_state=gs_running;

	return(TRUE);
}

