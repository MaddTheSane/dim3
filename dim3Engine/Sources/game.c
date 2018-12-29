/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Game Start/Stop

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
#include "scripts.h"
#include "objects.h"

extern app_type				app;
extern map_type				map;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern void net_host_game_end(void);

/* =======================================================

      Game Start and End
      
======================================================= */

bool game_start(bool in_file_load,int skill,int option_flags,int simple_save_idx,char *err_str)
{
		// pause time
		
	game_time_pause_start();

	progress_initialize(NULL);
	
		// reset random numbers
		
	random_reset();
	
		// start progress
	
	console_add_system("Starting Game");

		// start server

	progress_update();
		
	if (!server_game_start(in_file_load,skill,option_flags,simple_save_idx,err_str)) {
		progress_shutdown();
		return(FALSE);
	}

		// start view
		
	if (!app.dedicated_host) {

		progress_update();

		if (!view_game_start(err_str)) {
			progress_shutdown();
			return(FALSE);
		}
	}

		// game in running state
		
	server.game_open=TRUE;
	server.next_state=gs_running;

	progress_shutdown();
	
	game_time_pause_end();
	
	return(TRUE);
}

void game_end(void)
{
	console_add_system("Closing Game");

		// close any network joins or hosting
	
	switch (net_setup.mode) {
		case net_mode_client:
			net_client_send_remote_remove(server.obj_list.objs[server.player_obj_idx]);
			net_client_end_message_queue();
			net_client_join_host_end();
			break;
		case net_mode_host:
			net_host_game_end();
			break;
	}

		// stop view
		
	if (!app.dedicated_host) view_game_stop();

		// stop server
		
	server_game_stop();

		// game closed
		
	server.game_open=FALSE;
}

/* =======================================================

      Game Reset
      
======================================================= */

bool game_host_reset(char *err_str)
{
	int							n;
	obj_type					*obj,*player_obj;
	network_request_game_reset	reset;
	
		// switch to next map

	net_setup.host.current_map_idx++;
	if (net_setup.host.current_map_idx>=setup.network.map_list.count) net_setup.host.current_map_idx=0;

	strcpy(map.info.name,setup.network.map_list.maps[net_setup.host.current_map_idx].name);
	map.info.player_start_name[0]=0x0;

	if (!map_rebuild_changes(err_str)) {
		game_end();
		return(FALSE);
	}

		// all objects will be re-spawned when the new map
		// loads.  We setup the next spawn to be a game
		// reset and clear the scores
		
		// remotes get hidden (until the next update)
		// the respawn just to update any lists or items
		// before the client sends in it's next packet
		
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
	
		switch (obj->type) {
		
			case object_type_player:
			case object_type_bot_multiplayer:
				object_score_reset(obj);
				obj->next_spawn_sub_event=sd_event_spawn_game_reset;
				break;
				
			case object_type_remote_player:
				object_score_reset(obj);
				obj->next_spawn_sub_event=sd_event_spawn_game_reset;
				obj->hidden=TRUE;
				break;
				
			case object_type_remote_object:
				obj->next_spawn_sub_event=sd_event_spawn_game_reset;
				obj->hidden=TRUE;
				break;
				
		}
	}

		// signal remotes to reset

	strcpy(reset.map_name,map.info.name);

	player_obj=server.obj_list.objs[server.player_obj_idx];
	net_host_player_send_message_to_clients_all(NULL,net_action_request_game_reset,(unsigned char*)&reset,sizeof(network_request_game_reset));

	return(TRUE);
}



