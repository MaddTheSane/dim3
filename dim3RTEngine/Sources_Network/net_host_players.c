/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Host Player Lists

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

extern char team_colors[][16];

extern app_type				app;
extern map_type				map;
extern server_type			server;
extern view_type			view;
extern js_type				js;
extern network_setup_type	net_setup;

int							net_host_player_count;
net_host_player_type		net_host_players[host_max_remote_count];

SDL_mutex					*net_host_player_lock;

/* =======================================================

      Initialize Players
      
======================================================= */

void net_host_player_initialize(void)
{
	net_host_player_count=0;
	
	net_host_player_lock=SDL_CreateMutex();
}

void net_host_player_shutdown(void)
{
	SDL_DestroyMutex(net_host_player_lock);
}

/* =======================================================

      Find Players
      
======================================================= */

int net_host_player_find_net_uid(int net_uid)
{
	int						n;
	net_host_player_type	*player;
	
	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		if (player->connect.net_uid==net_uid) return(n);
		player++;
	}
	
	return(-1);
}

int net_host_player_find_net_addr(net_address_type *addr)
{
	int						n;
	net_host_player_type	*player;
	
	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		if ((player->connect.addr.ip==addr->ip) && (player->connect.addr.port==addr->port)) return(n);
		player++;
	}
	
	return(-1);
}

bool net_host_player_net_uid_to_address(int net_uid,net_address_type *addr)
{
	int						n;
	net_host_player_type	*player;
	
	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {

		if (player->connect.net_uid==net_uid) {
			addr->ip=player->connect.addr.ip;
			addr->port=player->connect.addr.port;
			return(TRUE);
		}

		player++;
	}
	
	return(FALSE);
}

/* =======================================================

      Player Add OK?
      
======================================================= */

bool net_host_player_add_ok(char *name,char *deny_reason)
{
	int						n;
	net_host_player_type	*player;

		// host full?
		
	if (net_host_player_count==host_max_remote_count) {
		strcpy(deny_reason,"Host is full");
		return(FALSE);
	}

		// lock all player operations
		
	SDL_mutexP(net_host_player_lock);

		// id in use?

	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		if (strcasecmp(player->name,name)==0) {
			sprintf(deny_reason,"Player name '%s' already in use",name);
			SDL_mutexV(net_host_player_lock);
			return(FALSE);
		}
		player++;
	}

		// unlock player operation

	SDL_mutexV(net_host_player_lock);

	return(TRUE);
}

/* =======================================================

      Add and Remove Players
      
======================================================= */

int net_host_player_add(net_address_type *addr,bool local,char *name,char *draw_name,int tint_color_idx)
{
	net_host_player_type		*player;

		// lock all player operations
		
	SDL_mutexP(net_host_player_lock);
	
		// create new player
		
	player=&net_host_players[net_host_player_count];

		// connections
		// includes a personal socket for sending to other clients

	player->connect.sock=net_open_udp_socket();
	if (player->connect.sock==D3_NULL_SOCKET) {
		SDL_mutexV(net_host_player_lock);
		return(-1);
	}

	if (addr==NULL) {
		player->connect.addr.ip=0;
		player->connect.addr.port=0;
	}
	else {
		player->connect.addr.ip=addr->ip;
		player->connect.addr.port=addr->port;
	}

	player->connect.local=local;
	player->connect.bot=FALSE;

	player->connect.net_uid=net_setup.next_net_uid;
	net_setup.next_net_uid++;

		// settings

	player->score=0;
	strcpy(player->name,name);
	strcpy(player->draw_name,draw_name);
	player->team_idx=net_team_none;
	
	player->pnt.x=player->pnt.y=player->pnt.z=0;
	
	net_host_player_count++;
	
		// unlock player operations

	SDL_mutexV(net_host_player_lock);
	
		// if dedicated, note in logs
		
	if (app.dedicated_host) fprintf(stdout,"Joined: %s\n",player->name);
	
	return(player->connect.net_uid);
}

int net_host_player_add_bot(obj_type *obj)
{
	net_host_player_type	*player;

		// lock all player operations
		
	SDL_mutexP(net_host_player_lock);

		// create new player
		
	player=&net_host_players[net_host_player_count];

		// bots have no connections

	player->connect.sock=D3_NULL_SOCKET;
	player->connect.addr.ip=0;
	player->connect.addr.port=0;
	player->connect.local=TRUE;
	player->connect.bot=TRUE;

	player->connect.net_uid=net_setup.next_net_uid;
	net_setup.next_net_uid++;

		// settings

	player->score=obj->score.score;
	
	strcpy(player->name,obj->name);
	strcpy(player->draw_name,obj->draw.name);
	
	player->team_idx=obj->team_idx;
	
	memmove(&player->pnt,&obj->pnt,sizeof(d3pnt));
	
	net_host_player_count++;

		// unlock player operations
	
	SDL_mutexV(net_host_player_lock);
	
	return(player->connect.net_uid);
}

void net_host_player_remove_by_uid(int net_uid)
{
	int						idx;
	char					name[name_str_len];
	net_host_player_type	*player;
	
		// lock all player operations
		
	SDL_mutexP(net_host_player_lock);
		
		// find player
		
	idx=net_host_player_find_net_uid(net_uid);
	if (idx==-1) {
		SDL_mutexV(net_host_player_lock);
		return;
	}
	
	player=&net_host_players[idx];

		// close the socket

	if (player->connect.sock!=D3_NULL_SOCKET) net_close_socket(&player->connect.sock);

		// remember name

	strcpy(name,player->name);

		// exit player
	
	if (idx<(net_host_player_count-1)) {
		memmove(&net_host_players[idx],&net_host_players[idx+1],(sizeof(net_host_player_type)*((net_host_player_count-idx)-1)));
	}
	
	net_host_player_count--;
	
	SDL_mutexV(net_host_player_lock);
	
		// if dedicated, note in logs
		
	if (app.dedicated_host) fprintf(stdout,"Left: %s\n",name);
}

void net_host_player_remove(network_request_remote_remove *remove)
{
	net_host_player_remove_by_uid((signed short)ntohs(remove->remove_net_uid));
}

/* =======================================================

      Count Teams
      
======================================================= */

int net_host_player_count_team(int team_idx)
{
	int						n,count;
	net_host_player_type	*player;
	
	count=0;
	
	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		if (player->team_idx==team_idx) count++;
		player++;
	}
	
	return(count);
}

/* =======================================================

      Update Player Information
      
======================================================= */

void net_host_player_update(network_request_remote_update *update)
{
	int							idx,score;
	bool						score_update;
	net_host_player_type		*player;

		// update score
		
	SDL_mutexP(net_host_player_lock);

	idx=net_host_player_find_net_uid((signed short)ntohs(update->update_net_uid));
	if (idx==-1) {
		SDL_mutexV(net_host_player_lock);
		return;
	}

	player=&net_host_players[idx];
	
		// position
		
	player->pnt.x=ntohl(update->pnt_x);
	player->pnt.y=ntohl(update->pnt_y);
	player->pnt.z=ntohl(update->pnt_z);
		
		// score

	score=(signed short)ntohs(update->score);
	
	score_update=(score!=player->score);
	player->score=score;
	
	SDL_mutexV(net_host_player_lock);
}

/* =======================================================

      Remote Click
      
======================================================= */

void net_host_player_remote_click(network_request_remote_click *click)
{
	obj_type			*clicking_obj,*clicked_obj;
	
	clicking_obj=object_find_remote_net_uid((signed short)ntohs(click->clicking_net_uid));
	if (clicking_obj==NULL) return;

	clicked_obj=object_find_remote_net_uid((signed short)ntohs(click->clicking_net_uid));
	if (clicking_obj==NULL) return;

		// run click

	object_click(clicked_obj,clicking_obj);
}

/* =======================================================

      Object Synching

	  When a player joins, it requests a object synch
	  which sends a remote add for every object on
	  the host that's not static scenery
      
======================================================= */

void net_host_player_remote_object_synch(net_address_type *addr)
{
	int							n,idx,skip_net_uid,the_type;
	char						*c;
	obj_type					*obj;
	network_request_remote_add	add;

		// find this player

	skip_net_uid=-1;

	SDL_mutexP(net_host_player_lock);

	idx=net_host_player_find_net_addr(addr);
	if (idx!=-1) skip_net_uid=net_host_players[idx].connect.net_uid;

	SDL_mutexV(net_host_player_lock);

		// send objects

	for (n=0;n!=max_obj_list;n++) {

			// get object, skip scenery

		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (obj->scenery.on) continue;
		
			// don't send self!
			
		if (obj->remote.net_uid==skip_net_uid) continue;
		
			// determine type on client
			
		the_type=object_type_remote_object;
		if ((obj->type==object_type_remote_player) || (obj->type==object_type_bot_multiplayer)) the_type=object_type_remote_player;

			// build the remote

		add.add_net_uid=htons((short)obj->remote.net_uid);
		add.type=htons((short)the_type);
		strcpy(add.name,obj->name);
		
		strcpy(add.script_name,js.script_list.scripts[obj->script_idx]->name);
		c=strrchr(add.script_name,'.');		// remove .js
		if (c!=0x0) *c=0x0;

		strcpy(add.draw_name,obj->draw.name);
		add.team_idx=htons((short)obj->team_idx);
		add.tint_color_idx=htons((short)obj->tint_color_idx);
		add.score=obj->score.score;

		net_host_player_send_message_to_client(addr,net_action_request_remote_add,(unsigned char*)&add,sizeof(network_request_remote_add));
	}
}

/* =======================================================

      Group Synching

	  Periodically, clients ask the host to
	  synch their group movements, so they don't
	  get off.  This is also used to read
	  the latency
      
======================================================= */

void net_host_player_remote_group_synch(net_address_type *addr)
{
	int								n;
	network_reply_group_synch		reply_synch;

		// send a synch for all groups that were
		// ever moved

	for (n=0;n!=map.group.ngroup;n++) {

		if (map.group.groups[n].run.was_moved) {
			group_moves_synch_with_client(n,&reply_synch);
			net_host_player_send_message_to_client(addr,net_action_reply_group_synch,(unsigned char*)&reply_synch,sizeof(network_reply_group_synch));
		}
	}
}

/* =======================================================

      Build Lists for Info Replies
      
======================================================= */

void net_host_player_create_info_player_list(network_reply_info_player_list *player_list)
{
	int							n;
	net_host_player_type		*player;
	network_reply_info_player	*info_player;

		// set up the player info

	SDL_mutexP(net_host_player_lock);

	player=net_host_players;
	info_player=player_list->players;
	
	for (n=0;n!=net_host_player_count;n++) {
		strcpy(info_player->name,player->name);
		info_player->score=htons((short)player->score);
		info_player->bot=htons((short)(player->connect.bot?1:0));

		info_player++;
		player++;
	}

		// finish with the count

	player_list->count=htons((short)net_host_player_count);
	player_list->max_count=htons((short)host_max_remote_count);

	SDL_mutexV(net_host_player_lock);
}

/* =======================================================

      Route Remote Messages to Host
      
======================================================= */

void net_host_player_remote_route_msg(net_queue_msg_type *msg)
{
		// push messages to other clients
		// and respond to player specific messages

		// stat_updates are send only from host to
		// client, so ignore all of those
		
	switch (msg->action) {
	
		case net_action_request_remote_remove:
			net_host_player_remove((network_request_remote_remove*)msg->msg);
			net_host_player_send_message_to_clients_all(&msg->addr,net_action_request_remote_remove,msg->msg,msg->msg_len);
			break;
			
		case net_action_request_remote_update:
			net_host_player_update((network_request_remote_update*)msg->msg);
			net_host_player_send_message_to_clients_all(&msg->addr,net_action_request_remote_update,msg->msg,msg->msg_len);
			break;
			
		case net_action_request_remote_death:
		case net_action_request_remote_chat:
		case net_action_request_remote_sound:
		case net_action_request_remote_fire:
			net_host_player_send_message_to_clients_all(&msg->addr,msg->action,msg->msg,msg->msg_len);
			break;

		case net_action_request_remote_click:
			net_host_player_remote_click((network_request_remote_click*)msg->msg);
			break;

		case net_action_request_object_synch:
			net_host_player_remote_object_synch(&msg->addr);
			break;

		case net_action_request_group_synch:
			net_host_player_remote_group_synch(&msg->addr);
			break;

	}
		
		// pass on message to remotes on this host
			
	remote_route_message(msg);
}

/* =======================================================

      Send Pickup Stat Updates to Players
      
======================================================= */

void net_host_player_send_pickup(obj_type *obj,obj_type *item_obj)
{
	bool								ok;
	net_address_type					addr;
	network_request_remote_pickup		pickup;

		// get player

	SDL_mutexP(net_host_player_lock);
	ok=net_host_player_net_uid_to_address(obj->remote.net_uid,&addr);
	SDL_mutexV(net_host_player_lock);

	if (!ok) return;

		// build the update

	pickup.picking_net_uid=htons((short)obj->remote.net_uid);
	pickup.picked_net_uid=htons((short)item_obj->remote.net_uid);

		// send update to player

	net_host_player_send_message_to_client(&addr,net_action_request_remote_pickup,(unsigned char*)&pickup,sizeof(network_request_remote_pickup));
}

/* =======================================================

      Send Bot 
      
======================================================= */

void net_host_player_send_updates(void)
{
	int								n,tick;
	bool							chat_on;
	obj_type						*obj;
	network_request_remote_update	update;

		// time for an update

	tick=game_time_get();
	if (tick<server.time.network_update_tick) return;

	server.time.network_update_tick=tick+client_communication_update_msec_rate;

		// update all objects on this
		// server that aren't scenery
		// or remotes
		// (this includes the player on
		// the host)

	if (net_setup.mode==net_mode_host) {

		for (n=0;n!=max_obj_list;n++) {
			obj=server.obj_list.objs[n];
			if (obj==NULL) continue;

			if (obj->scenery.on) continue;
			if (obj->type==object_type_remote_player) continue;
			
			chat_on=FALSE;
			if (!app.dedicated_host) {
				if (n==server.player_obj_idx) chat_on=view.chat.type_on;
			}

			remote_update_pack(obj,chat_on,&update);
			net_host_player_send_message_to_clients_all(NULL,net_action_request_remote_update,(unsigned char*)&update,sizeof(network_request_remote_update));
		}
	}
}

/* =======================================================

      Send Messages to Single or All Players
      
======================================================= */

void net_host_player_send_message_to_client(net_address_type *addr,int action,unsigned char *msg,int msg_len)
{
	int						idx;
	net_host_player_type	*player;

	SDL_mutexP(net_host_player_lock);

		// find player

	idx=net_host_player_find_net_addr(addr);
	if (idx==-1) {
		SDL_mutexV(net_host_player_lock);
		return;
	}

	player=&net_host_players[idx];

		// send message

	net_sendto_msg(player->connect.sock,&player->connect.addr,action,msg,msg_len);

	SDL_mutexV(net_host_player_lock);
}

void net_host_player_send_message_to_clients_all(net_address_type *skip_addr,int action,unsigned char *msg,int msg_len)
{
	int						n,skip_idx;
	net_host_player_type	*player;
	
	SDL_mutexP(net_host_player_lock);

		// find player to skip

	skip_idx=-1;
	if (skip_addr!=NULL) skip_idx=net_host_player_find_net_addr(skip_addr);
	
		// send to others
	
	for (n=0;n!=net_host_player_count;n++) {

			// any skip addresses

		if (n==skip_idx) continue;

			// skip any local players
			// or bots

		player=&net_host_players[n];
		if (player->connect.local) continue;
		if (player->connect.bot) continue;

			// send to network
		
		net_sendto_msg(player->connect.sock,&player->connect.addr,action,msg,msg_len);
	}
	
	SDL_mutexV(net_host_player_lock);
}
