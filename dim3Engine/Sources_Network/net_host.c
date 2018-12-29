/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Main Host Code

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
#include "network.h"
#include "objects.h"

extern app_type				app;
extern map_type				map;
extern server_type			server;
extern js_type				js;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;

d3socket					host_socket;
bool						host_complete,host_start_shutdown;
net_queue_type				host_queue;
char						host_err_str[256];
SDL_Thread					*host_thread;

int net_host_thread(void *arg);		// forward reference for main thread

/* =======================================================

      Initialize and Shutdown Host Networking
      
======================================================= */

bool net_host_initialize(char *err_str)
{
		// create cache

	if (!net_queue_initialize(&host_queue)) {
		strcpy(err_str,"Networking: Out of memory");
		return(FALSE);
	}

		// begin listener thread
		
	host_complete=FALSE;
	host_start_shutdown=FALSE;
	host_socket=D3_NULL_SOCKET;
	
	host_thread=SDL_CreateThread(net_host_thread,"host",NULL);
	if (host_thread==NULL) {
		net_queue_shutdown(&host_queue);
		strcpy(err_str,"Networking: Could not start host listener thread");
		return(FALSE);
	}
	
		// wait for listenter thread to get into accept
		
	while (TRUE) {
		if (host_complete) break;
		usleep(100000);
	}
	
	if (host_err_str[0]!=0x0) {
		net_queue_shutdown(&host_queue);
		strcpy(err_str,host_err_str);
		return(FALSE);
	}

		// now listening on separate thread
		
	return(TRUE);
}

void net_host_shutdown(void)
{
		// did host never start?

	if (host_socket==D3_NULL_SOCKET) return;
	
		// trigger the shutdown and wait
		
	host_start_shutdown=TRUE;
	SDL_WaitThread(host_thread,NULL);
	
		// shutdown socket and free queue
		
	net_close_socket(&host_socket);
	net_queue_shutdown(&host_queue);
}

/* =======================================================

      Join Local Player to Host
      
======================================================= */

bool net_host_join_local_player(char *err_str)
{
	char						*c;
	obj_type					*player_obj;
	network_request_remote_add	add;

		// get local player

	player_obj=server.obj_list.objs[server.player_obj_idx];

		// join directly to host

	player_obj->remote.net_uid=net_host_player_add(NULL,TRUE,player_obj->name,player_obj->draw.name,player_obj->tint_color_idx);
	if (player_obj->remote.net_uid==-1) {
		strcpy(err_str,"Unable to add player");
		return(FALSE);
	}

		// send all other players on host the new player for remote add

	add.type=htons((short)object_type_remote_player);
	strcpy(add.name,player_obj->name);
	
	strcpy(add.script_name,js.script_list.scripts[player_obj->script_idx]->name);
	c=strrchr(add.script_name,'.');		// remove .js
	if (c!=0x0) *c=0x0;
	
	strcpy(add.draw_name,player_obj->draw.name);
	add.team_idx=htons((short)net_team_none);
	add.tint_color_idx=htons((short)player_obj->tint_color_idx);
	add.score=0;

	net_host_player_send_message_to_clients_all(NULL,net_action_request_remote_add,(unsigned char*)&add,sizeof(network_request_remote_add));

	return(TRUE);
}

/* =======================================================

      Join Multiplayer Bots to a Host
      
======================================================= */

void net_host_join_multiplayer_bots(void)
{
	int				n;
	char			deny_reason[256],err_str[256];
	obj_type		*obj;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

			// can this bot join the game?

		if (obj->type!=object_type_bot_multiplayer) continue;
		if (!net_host_player_add_ok(obj->name,deny_reason)) {
			snprintf(err_str,256,"%s: %s",obj->name,deny_reason);
			err_str[255]=0x0;
			console_add(err_str);
			continue;
		}

			// add bot

		obj->remote.net_uid=net_host_player_add_bot(obj);
	}
}

/* =======================================================

      Host Info Reply
      
======================================================= */

void net_host_info_request(net_address_type *addr)
{
	network_reply_info		info;
	
	strcpy(info.host_name,net_setup.host.name);
	strcpy(info.host_ip_resolve,net_setup.host.ip_resolve);

	strcpy(info.proj_name,iface.project.name);
	strcpy(info.game_name,iface.multiplayer.game_list.games[net_setup.game_idx].name);
	strcpy(info.map_name,map.info.name);

	info.score_limit=htons((short)net_setup.score_limit);
	info.respawn_secs=htons((short)net_setup.respawn_secs);
	info.game_reset_secs=htons((short)net_setup.game_reset_secs);
	info.option_flags=htonl(net_setup.option_flags);

	net_host_player_create_info_player_list(&info.player_list);

	net_sendto_msg(host_socket,addr,net_action_reply_info,(unsigned char*)&info,sizeof(network_reply_info));
}

/* =======================================================

      Join Host Reply
      
======================================================= */

bool net_host_join_request_ok(network_request_join *request_join,network_reply_join *reply_join)
{
		// refuse join if hash is different

	if (htonl(request_join->hash)!=net_get_project_hash()) {
		sprintf(reply_join->deny_reason,"Client game data has been modified");
		return(FALSE);
	}

		// refuse join if versions are different

	if (strncmp(request_join->vers,dim3_version,name_str_len)!=0) {
		sprintf(reply_join->deny_reason,"Client and host version differ (%s; %s)",request_join->vers,dim3_version);
		return(FALSE);
	}

		// name conflict or host full?

	return(net_host_player_add_ok(request_join->name,reply_join->deny_reason));
}

int net_host_join_request(net_address_type *addr,network_request_join *request_join)
{
	int							net_uid,
								tint_color_idx;
	char						*c;
	obj_type					*obj;
	network_reply_join			reply_join;
	network_request_remote_add	add;
	
		// check if join is OK
	
	net_uid=-1;

	reply_join.deny_reason[0]=0x0;

		// create remote UID when
		// adding this new remote player

	if (net_host_join_request_ok(request_join,&reply_join)) {
		tint_color_idx=htons((short)request_join->tint_color_idx);
		net_uid=net_host_player_add(addr,FALSE,request_join->name,request_join->draw_name,tint_color_idx);
	}

		// construct the reply
	
	reply_join.join_net_uid=htons((short)net_uid);

	reply_join.team_idx=htons((short)net_team_none);
	reply_join.map_tick=htonl(game_time_get()-map.start_game_tick);

		// build a remote add request for other
		// clients in the game

		// so we can add a remote, we start with a fake
		// team (none), after we add, we can call the game
		// rules and reset it

	add.add_net_uid=htons((short)net_uid);
	add.type=htons((short)object_type_remote_player);
	strncpy(add.name,request_join->name,name_str_len);
	
	strncpy(add.script_name,request_join->script_name,name_str_len);
	c=strrchr(add.script_name,'.');		// remove .js
	if (c!=0x0) *c=0x0;

	strncpy(add.draw_name,request_join->draw_name,name_str_len);
	add.team_idx=htons((short)net_team_none);
	add.tint_color_idx=htons((short)tint_color_idx);
	add.score=0;

	add.name[name_str_len-1]=0x0;
	add.script_name[name_str_len-1]=0x0;
	add.draw_name[name_str_len-1]=0x0;

		// create the remote object

	if (!remote_add(&add)) {
		net_uid=-1;
	}

		// run team rule

	if (net_uid!=-1) {
		obj=object_find_remote_net_uid(net_uid);
		object_multiplayer_setup(obj);

			// reset team in reply and remote add

		reply_join.team_idx=htons((short)obj->team_idx);
		add.team_idx=htons((short)obj->team_idx);
	}
	
		// send reply back to client

	if (!net_sendto_msg(host_socket,addr,net_action_reply_join,(unsigned char*)&reply_join,sizeof(network_reply_join))) {
		if (net_uid!=-1) net_host_player_remove_by_uid(net_uid);
		return(FALSE);
	}
	
		// if no player uid, then player was rejected
		
	if (net_uid==-1) return(FALSE);
	
		// send all other players on host the new player for remote add

	net_host_player_send_message_to_clients_all(addr,net_action_request_remote_add,(unsigned char*)&add,sizeof(network_request_remote_add));
	
	return(net_uid);
}

/* =======================================================

      Host Process Messages
      
======================================================= */

void net_host_process_messages(void)
{
	int						count;
	net_queue_msg_type		msg;
	
	count=0;
	
	while (count<host_message_per_loop_count) {
	
			// check for messages

		if (!net_queue_check_message(&host_queue,&msg)) return;

			// reply to all info request
			
		if (msg.action==net_action_request_info) {
			net_host_info_request(&msg.addr);
			continue;
		}

			// reply to all join requests

		if (msg.action==net_action_request_join) {
			net_host_join_request(&msg.addr,(network_request_join*)msg.msg);
			continue;
		}
		
			// all other requests are routed to
			// proper remotes and back to other players
			// if necessary

		net_host_player_remote_route_msg(&msg);
	}
}

/* =======================================================

      Host Networking Main Thread
      
======================================================= */

int net_host_thread(void *arg)
{
		// use host err_str to flag if errors occured
		
	host_err_str[0]=0x0;
	
		// create host socket
		
	host_socket=net_open_udp_socket();
	if (host_socket==D3_NULL_SOCKET) {
		strcpy(host_err_str,"Hosting: Unable to open socket");
		host_complete=TRUE;
		return(0);
	}
	
		// we'll let the socket block for us
	
	net_socket_blocking(host_socket,TRUE);
		
		// bind -- use any IP on this machine to
		// get traffic

	if (!net_bind_any(host_socket,net_port_host,host_err_str)) {
		net_close_socket(&host_socket);
		host_complete=TRUE;
		return(0);
	}

		// host is OK, free thread to run independantly
		
	host_complete=TRUE;

		// begin waiting for messages
	
	while (TRUE) {
	
			// exiting?
			
		if (host_start_shutdown) break;

			// feed the queues from the socket
			// if there's an error, break out of loop
			// and cancel network game
			
		if (!net_queue_feed(host_socket,&host_queue)) break;
	
			// always wait a little to not flood this thread
			
		usleep(100000);
	}
	
	return(0);
}

/* =======================================================

      Start and Stop Hosted Game
      
======================================================= */

void net_host_game_setup(void)
{
		// reset UIDs

	net_setup.next_net_uid=net_uid_constant_client_start;
	
		// resolve names to IPs
		
	if (net_setup.host.name[0]==0x0) net_get_host_name(net_setup.host.name);
	net_get_host_ip(net_setup.host.ip_name,net_setup.host.ip_resolve);
}

bool net_host_game_start(char *err_str)
{
		// start hosting

	if (!net_host_initialize(err_str)) return(FALSE);

		// initialize host player list
		
	net_host_player_initialize();

	return(TRUE);
}

void net_host_game_end(void)
{
		// inform all player of server shutdown
		
	net_host_player_send_message_to_clients_all(NULL,net_action_request_host_exit,NULL,0);

		// shutdown server

	net_host_shutdown();
	net_host_player_shutdown();
	
		// make sure hosting flags are off
		
	net_setup.mode=net_mode_none;
}

