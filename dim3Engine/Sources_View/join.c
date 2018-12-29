/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Network Join

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

#define join_pane_hosts					0
#define join_pane_custom_host			1
#define join_pane_news					2

#define join_frame_id					0
#define join_tab_id						1

#define join_button_join_id				10
#define join_button_cancel_id			11
#define join_button_rescan_id			12

#define join_lan_table_id				20
#define join_wan_table_id				21

#define join_custom_ip_id				22

#define join_map_bitmap_id				30
#define join_host_status_id				31
#define join_game_name_id				32
#define join_map_name_id				33
#define join_player_count_id			34
#define join_ping_id					35

#define join_status_id					40

#define join_news_id					50

#define join_mode_wan_lan				0
#define join_mode_lan					1
#define join_mode_lan_error				2

extern map_type				map;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

int							join_mode,join_tab_value,join_thread_lan_start_tick;
char						*join_table_data,*join_news;
bool						join_thread_lan_started,
							join_thread_wan_started,
							join_wan_updated,
							join_thread_lan_trigger_quit;
SDL_Thread					*join_lan_thread,*join_wan_thread;

join_server_host_list_type	*join_lan_list,*join_wan_list;
			
/* =======================================================

      Fill Table Join List
      
======================================================= */

void join_fill_table_with_host_list(join_server_host_list_type *list,int table_id)
{
	int						n,sz;
	char					*c,*row_data;
	join_server_host_type	*host;
	
		// create row data
		
	sz=(list->count+1)*128;
	
	row_data=malloc(sz);
	bzero(row_data,sz);
	
		// build table
	
	host=list->hosts;
	c=row_data;
	
	for (n=0;n!=list->count;n++) {
		snprintf(c,128,"%s [%s]",host->name,host->ip);
		c[127]=0x0;
		
		c+=128;
		host++;
	}

	element_set_table_data(table_id,FALSE,row_data);

	free(row_data);
}

/* =======================================================

      Join Host Information
      
======================================================= */

void join_update_host_information(int table_id)
{
	int						idx;
	char					str[256],path[1024];
	join_server_host_type	*host;

		// turn off other table
		
	if (table_id==join_lan_table_id) {
		element_set_value(join_wan_table_id,-1);
	}
	else {
		element_set_value(join_lan_table_id,-1);
	}
	
		// get selection
		
	idx=element_get_value(table_id);

		// nothing selected

	if (idx==-1) {
		element_set_bitmap(join_map_bitmap_id,NULL);
		element_enable(join_button_join_id,FALSE);
		element_text_change(join_host_status_id,"No Selection");
		element_text_change(join_game_name_id,"");
		element_text_change(join_map_name_id,"");
		element_text_change(join_player_count_id,"");
		element_text_change(join_ping_id,"");
		return;
	}

		// get host
		
	if (table_id==join_lan_table_id) {
		host=&join_lan_list->hosts[idx];
	}
	else {
		host=&join_wan_list->hosts[idx];
	}
	
		// pinging

	if (host->pinging) {
		element_set_bitmap(join_map_bitmap_id,NULL);
		element_enable(join_button_join_id,FALSE);
		element_text_change(join_host_status_id,"Pinging...");
		element_text_change(join_game_name_id,"");
		element_text_change(join_map_name_id,"");
		element_text_change(join_player_count_id,"");
		element_text_change(join_ping_id,"");
		return;
	}

		// selected item but bad read

	if (host->unreachable) {
		element_enable(join_map_bitmap_id,FALSE);
		element_text_change(join_host_status_id,"Unreachable");
		element_text_change(join_game_name_id,"");
		element_text_change(join_map_name_id,"");
		element_text_change(join_player_count_id,"");
		element_text_change(join_ping_id,"");
		element_text_change(join_ping_id,"");
		return;
	}

		// good item

	file_paths_data(&file_path_setup,path,"Bitmaps/Icons_Map",host->map_name,"png");
	element_set_bitmap(join_map_bitmap_id,path);

	if (host->player_list.count<host->player_list.max_count) {
		element_text_change(join_host_status_id,"OK");
	}
	else {
		element_text_change(join_host_status_id,"Full");
	}

	element_text_change(join_game_name_id,host->game_name);
	element_text_change(join_map_name_id,host->map_name);

	sprintf(str,"%d/%d",host->player_list.count,host->player_list.max_count);
	element_text_change(join_player_count_id,str);

	sprintf(str,"%d msec",host->ping_msec);
	element_text_change(join_ping_id,str);

	element_enable(join_button_join_id,TRUE);
}

/* =======================================================

      Add or Update Host Table
      
======================================================= */

bool join_ping_thread_update_host(join_server_host_type *host,int start_tick,network_reply_info *reply_info)
{
	int						n,cnt;
	
	host->pinging=FALSE;
	
		// errors have NULL reply info
		
	if (reply_info==NULL) {
		host->unreachable=TRUE;
		return(FALSE);
	}

		// only add if same project

	if (strcasecmp(reply_info->proj_name,iface.project.name)!=0) return(FALSE);

		// host

	strcpy(host->name,reply_info->host_name);
	strcpy(host->ip,reply_info->host_ip_resolve);

	strcpy(host->game_name,reply_info->game_name);
	strcpy(host->map_name,reply_info->map_name);

	host->score_limit=(signed short)ntohs(reply_info->score_limit);
	host->respawn_secs=(signed short)ntohs(reply_info->respawn_secs);
	host->game_reset_secs=(signed short)ntohs(reply_info->game_reset_secs);
	host->option_flags=ntohl(reply_info->option_flags);

		// players

	host->player_list.count=(signed short)ntohs(reply_info->player_list.count);
	host->player_list.max_count=(signed short)ntohs(reply_info->player_list.max_count);

	cnt=host->player_list.count;
	if (cnt>join_info_max_players) cnt=join_info_max_players;
	if (cnt<0) cnt=0;

	for (n=0;n!=cnt;n++) {
		strcpy(host->player_list.players[n].name,reply_info->player_list.players[n].name);
		host->player_list.players[n].bot=(((int)ntohs(reply_info->player_list.players[n].bot))!=0);
		host->player_list.players[n].score=(int)ntohs(reply_info->player_list.players[n].score);
	}

		// ping time

	host->ping_msec=time_get()-start_tick;

	host->unreachable=FALSE;

	return(TRUE);
}

bool join_ping_thread_add_host(join_server_host_list_type *list,int start_tick,network_reply_info *reply_info)
{
	join_server_host_type	*host;

		// only add if same project

	if (strcasecmp(reply_info->proj_name,iface.project.name)!=0) return(FALSE);

		// add to list

	host=&list->hosts[list->count];

		// update

	if (join_ping_thread_update_host(host,start_tick,reply_info)) {
		list->count++;
		return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Join Ping LAN Hosts
	  
	  This is done by a thread that gathers up
	  all the hosts found in a local broadcast
      
======================================================= */

int join_ping_thread_lan(void *arg)
{
	int					max_tick,action;
	unsigned char		msg[net_max_msg_size];
	bool				good_reply;
	net_address_type	addr;
	d3socket			broadcast_sock;

		// create a socket and send out a broadcast
		// to all addresses on this subnet, looking for
		// replies from a dim3 server

	broadcast_sock=net_open_udp_socket();
	if (broadcast_sock==D3_NULL_SOCKET) return(0);

	net_socket_enable_broadcast(broadcast_sock);
	
	addr.ip=INADDR_BROADCAST;
	addr.port=net_port_host;
	
	if (!net_sendto_msg(broadcast_sock,&addr,net_action_request_info,NULL,0)) {
		net_close_socket(&broadcast_sock);
		return(0);
	}
	
		// now wait for any replies
	
	join_thread_lan_start_tick=time_get();
	max_tick=client_query_timeout_wait_msec;
	
		// non-blocking socket
		
	net_socket_blocking(broadcast_sock,FALSE);
	
	while (((join_thread_lan_start_tick+max_tick)>time_get()) && (!join_thread_lan_trigger_quit)) {

			// any replies?
			
		good_reply=FALSE;
		
		if (net_recvfrom_mesage(broadcast_sock,NULL,&action,msg,NULL)) {
			if (action==net_action_reply_info) {
				good_reply=join_ping_thread_add_host(join_lan_list,join_thread_lan_start_tick,(network_reply_info*)msg);
				if (good_reply) join_fill_table_with_host_list(join_lan_list,join_lan_table_id);
			}
		}
		
			// if no good reply, then sleep for a bit
			
		if (!good_reply) {
			usleep(100000);
		}
	}
	
	net_close_socket(&broadcast_sock);

	element_table_busy(join_lan_table_id,FALSE);
	element_enable(join_button_rescan_id,TRUE);

	return(0);
}

void join_ping_thread_lan_start(void)
{
		// empty join list
		
	join_thread_lan_trigger_quit=FALSE;
	
		// clear table and set
		// as busy

	join_lan_list->count=0;

	element_set_table_data(join_lan_table_id,FALSE,NULL);
	element_table_busy(join_lan_table_id,TRUE);

		// start pinging hosts
		
	join_thread_lan_started=TRUE;

	join_lan_thread=SDL_CreateThread(join_ping_thread_lan,"join_lan",NULL);
}

void join_ping_thread_lan_end(void)
{
	if (!join_thread_lan_started) return;
	
	join_thread_lan_trigger_quit=TRUE;
	
	SDL_WaitThread(join_lan_thread,NULL);
	
	join_thread_lan_started=FALSE;
}

/* =======================================================

      Join Ping WAN Hosts
	  
	  This is also done in a thread so the UI
	  continues to function but only done as
	  WAN hosts are clicked
      
======================================================= */

bool join_ping_thread_wan_internal(join_server_host_type *host,int msec,unsigned char *msg)
{
	int						action;
	unsigned long			nip_addr;
	bool					got_reply;
	net_address_type		addr,recv_addr;
	d3socket				sock;
	
		// get socket, translate IP
		// and call host
		
	sock=net_open_udp_socket();
	if (sock==D3_NULL_SOCKET) return(FALSE);
	
	nip_addr=inet_addr(host->ip);
	if (nip_addr==INADDR_NONE) return(FALSE);
	
	addr.ip=ntohl(nip_addr);
	addr.port=net_port_host;

	if (!net_sendto_msg(sock,&addr,net_action_request_info,NULL,0)) {
		net_close_socket(&sock);
		return(FALSE);
	}

		// non-blocking socket
		
	net_socket_blocking(sock,FALSE);

		// get the reply
	
	got_reply=FALSE;
	
	while ((msec+client_query_timeout_wait_msec)>time_get()) {
		if (net_recvfrom_mesage(sock,&recv_addr,&action,msg,NULL)) {
			if ((recv_addr.ip==addr.ip) && (action==net_action_reply_info)) {
				got_reply=TRUE;
				break;
			}
		}
		usleep(100000);
	}
	
		// close socket
		
	net_close_socket(&sock);
	
	return(got_reply);
}

int join_ping_thread_wan(void *arg)
{
	int						idx,msec;
	unsigned char			msg[net_max_msg_size];
	join_server_host_type	*host;
	
	idx=(int)arg;
	host=&join_wan_list->hosts[idx];
	
		// ping the host
		
	msec=time_get();
	
	if (join_ping_thread_wan_internal(host,msec,msg)) {
		join_ping_thread_update_host(host,msec,(network_reply_info*)msg);
	}
	else {
		join_ping_thread_update_host(host,msec,NULL);
	}
	
		// can't update OpenGL in thread
		// so we trigger for later
		
	join_wan_updated=TRUE;
	
	return(0);
}

void join_ping_wan_host(void)
{
	int						idx;
	join_server_host_type	*host;
		
		// blank if no selection
		
	idx=element_get_value(join_wan_table_id);
	if (idx==-1) {
		join_update_host_information(join_wan_table_id);
		return;
	}
	
	host=&join_wan_list->hosts[idx];
	
		// setup as pinging

	host->pinging=TRUE;
	join_update_host_information(join_wan_table_id);
	
	element_table_busy(join_wan_table_id,TRUE);
	
		// start pinging single WAN host
		
	join_thread_wan_started=TRUE;
	join_wan_updated=FALSE;

	join_wan_thread=SDL_CreateThread(join_ping_thread_wan,"join_wan",(void*)idx);
}

void join_ping_thread_wan_end(void)
{
	if (!join_thread_wan_started) return;
	
	SDL_WaitThread(join_wan_thread,NULL);
	
	join_thread_wan_started=FALSE;
}

void join_ping_wan_host_update(void)
{
	int						idx;
	join_server_host_type	*host;

	idx=element_get_value(join_wan_table_id);
	if (idx==-1) return;

	host=&join_wan_list->hosts[idx];

	host->pinging=FALSE;
	join_update_host_information(join_wan_table_id);
	
	element_table_busy(join_wan_table_id,FALSE);
}

void join_ping_custom_host(join_server_host_type *host)
{
	int						msec;
	unsigned char			msg[net_max_msg_size];

	strcpy(host->ip,setup.network.custom_host_ip);
		
	msec=time_get();
	
	if (join_ping_thread_wan_internal(host,msec,msg)) {
		join_ping_thread_update_host(host,msec,(network_reply_info*)msg);
	}
	else {
		join_ping_thread_update_host(host,msec,NULL);
	}
}

/* =======================================================

      Join Panes
      
======================================================= */

void join_news_pane(void)
{
	int			x,y,wid,high;
	
		// show and hide proper elements
		
	element_hide(join_button_rescan_id,TRUE);
	element_enable(join_button_join_id,FALSE);
	element_text_change(join_status_id,"");

		// news
		
	element_get_frame_inner_space(join_frame_id,&x,&y,&wid,&high);
	element_text_box_add(join_news,join_news_id,x,y,wid,high,FALSE);
}

void join_add_info_controls(void)
{
	int				fx,fy,wid,high,
					mx,tx,ty,half_wid,pic_high,
					padding,margin;

	padding=element_get_padding();
	margin=element_get_margin();

	element_get_frame_inner_space(join_frame_id,&fx,&fy,&wid,&high);
	
		// join bitmap

	half_wid=(wid>>1)-padding;
	mx=((fx+wid)-half_wid)-padding;

		// picture

	pic_high=(half_wid*3)/4;
	ty=fy+((high-(pic_high+margin+iface.font.text_size_medium+(element_get_control_high()*4)))>>1);

	element_bitmap_add(NULL,join_map_bitmap_id,fx,ty,half_wid,pic_high,TRUE);
	
	tx=fx+(half_wid>>2);
	ty+=(pic_high+margin+iface.font.text_size_medium);

	element_text_add("Status:",-1,tx,ty,iface.font.text_size_medium,tx_right,&iface.color.control.text,FALSE);
	element_text_add("",join_host_status_id,(tx+10),ty,iface.font.text_size_medium,tx_left,&iface.color.control.text,FALSE);

	ty+=element_get_control_high();
	element_text_add("Game:",-1,tx,ty,iface.font.text_size_medium,tx_right,&iface.color.control.text,FALSE);
	element_text_add("",join_game_name_id,(tx+10),ty,iface.font.text_size_medium,tx_left,&iface.color.control.text,FALSE);

	ty+=element_get_control_high();
	element_text_add("Map:",-1,tx,ty,iface.font.text_size_medium,tx_right,&iface.color.control.text,FALSE);
	element_text_add("",join_map_name_id,(tx+10),ty,iface.font.text_size_medium,tx_left,&iface.color.control.text,FALSE);

	ty+=element_get_control_high();
	element_text_add("Players:",-1,tx,ty,iface.font.text_size_medium,tx_right,&iface.color.control.text,FALSE);
	element_text_add("",join_player_count_id,(tx+10),ty,iface.font.text_size_medium,tx_left,&iface.color.control.text,FALSE);
	
	ty+=element_get_control_high();
	element_text_add("Ping:",-1,tx,ty,iface.font.text_size_medium,tx_right,&iface.color.control.text,FALSE);
	element_text_add("",join_ping_id,(tx+10),ty,iface.font.text_size_medium,tx_left,&iface.color.control.text,FALSE);
}

void join_custom_host_pane(void)
{
	int						fx,fy,x,y,wid,high;

		// show and hide proper elements
		
	element_hide(join_button_rescan_id,TRUE);
	element_enable(join_button_join_id,(setup.network.custom_host_ip[0]!=0x0));
	element_text_change(join_status_id,"");

	element_get_frame_inner_space(join_frame_id,&fx,&fy,&wid,&high);

		// IP address

	x=fx+((wid>>1)-(element_get_control_long_wid()>>1));
	y=fy+(high>>1);

	element_text_field_add("IP",setup.network.custom_host_ip,64,join_custom_ip_id,x,y,TRUE);
}

void join_hosts_pane(void)
{
	int						fx,fy,y,wid,high,table_high,
							mx,half_wid,padding,margin;
	char					str[1024],nstr[32];
	element_column_type		cols[4];

		// show and hide proper elements
		
	element_hide(join_button_rescan_id,FALSE);
	element_enable(join_button_rescan_id,FALSE);
	element_enable(join_button_join_id,FALSE);
	element_text_change(join_status_id,"");
	
	padding=element_get_padding();
	margin=element_get_margin();

	element_get_frame_inner_space(join_frame_id,&fx,&fy,&wid,&high);
	
		// info controls

	join_add_info_controls();

		// lan list

	half_wid=(wid>>1)-padding;
	mx=((fx+wid)-half_wid)-padding;
	
	if (join_mode==join_mode_lan) {
		table_high=high-padding;
	}
	else {
		table_high=((high-padding)/2);
	}

	strcpy(cols[0].name,"Local Hosts");
	cols[0].percent_size=1.0f;
	
	y=fy;
	element_table_add(cols,NULL,join_lan_table_id,1,(mx+padding),y,half_wid,table_high,FALSE,element_table_bitmap_none);
	
	y+=(table_high+padding);

		// wan list
	
	if (join_mode==join_mode_wan_lan) {
		strcpy(cols[0].name,"Internet Hosts");
		cols[0].percent_size=1.0f;

		element_table_add(cols,NULL,join_wan_table_id,1,(mx+padding),y,half_wid,table_high,FALSE,element_table_bitmap_none);

		join_fill_table_with_host_list(join_wan_list,join_wan_table_id);
	}

		// any wan error state

	else {
		if (join_mode==join_mode_lan_error) {
			strcpy(str,"No internet hosts available.\nCould not retrieve host data from:\n");
			strcat(str,iface.multiplayer.news.host);
			if (iface.multiplayer.news.port!=80) {
				sprintf(nstr,":%d",iface.multiplayer.news.port);
				strcat(str,nstr);
			}
			strcat(str,iface.multiplayer.news.url);

			element_text_box_add(str,-1,(mx+padding),y,half_wid,table_high,TRUE);
		}
	}

		// start the local broadcast
		// LAN thread
		
	join_ping_thread_lan_start();
}

void join_create_pane(void)
{
	int							fx,fy,x,y,wid,high,margin,padding;
	char						tab_list[][32]={"Hosts","Custom Host","News"};
	element_frame_button_type	butts[3]={{join_button_rescan_id,"Rescan",FALSE},{join_button_cancel_id,"Cancel",TRUE},{join_button_join_id,"Join",TRUE}};
	
		// turn off any scanning threads
		
	join_ping_thread_lan_end();
	join_ping_thread_wan_end();

		// controls

	element_clear();

		// frame
		
	margin=element_get_margin();
	padding=element_get_padding();

	fx=margin;
	fy=margin;
	wid=iface.scale_x-(margin*2);
	high=iface.scale_y-(margin*2);
	
	if (join_mode==join_mode_wan_lan) {
		element_frame_add("Join Game",join_frame_id,fx,fy,wid,high,join_tab_id,3,(char*)tab_list,3,butts);
		element_set_value(join_tab_id,join_tab_value);
	}
	else {
		element_frame_add("Join Game",join_frame_id,fx,fy,wid,high,join_tab_id,0,NULL,3,butts);
	}
	
		// status
		
	x=(element_get_x_position(join_button_rescan_id)+element_get_button_short_wid())+padding;
	y=(element_get_y_position(join_button_rescan_id)+element_get_button_high())-((element_get_button_high()-gl_text_get_char_height(iface.font.text_size_small))/2);
	
	element_text_add("",join_status_id,x,y,iface.font.text_size_small,tx_left,NULL,FALSE);

		// specific pane controls
		
	if (join_mode!=join_mode_wan_lan) {
		join_hosts_pane();
		return;
	}
		
	switch (element_get_value(join_tab_id)) {
		case join_pane_hosts:
			join_hosts_pane();
			break;
		case join_pane_custom_host:
			join_custom_host_pane();
			break;
		case join_pane_news:
			join_news_pane();
			break;
	}
}

/* =======================================================

      Join Operations
      
======================================================= */

void join_open(void)
{
		// news data

	join_news=(char*)malloc(max_join_server_news_len);
	join_news[0]=0x0;

		// host lists

	join_lan_list=(join_server_host_list_type*)malloc(sizeof(join_server_host_list_type));
	join_wan_list=(join_server_host_list_type*)malloc(sizeof(join_server_host_list_type));

	join_lan_list->count=0;
	join_wan_list->count=0;

		// get the project hash

	net_create_project_hash();

		// if internet hosts, then get
		// news and master host list

	join_mode=iface.multiplayer.local_only?join_mode_lan:join_mode_wan_lan;

	if (join_mode==join_mode_wan_lan) {
		if (!net_load_news(join_wan_list,join_news)) join_mode=join_mode_lan_error;
	}

		// setup gui
		
	gui_initialize("Bitmaps/Backgrounds","main");
	
		// start with first tab
		
	join_tab_value=join_pane_hosts;
	join_thread_lan_started=FALSE;
	join_thread_wan_started=FALSE;
	join_wan_updated=FALSE;
	
		// create panes
		
	join_create_pane();
}

void join_close(void)
{
		// stop any pinging

	join_ping_thread_lan_end();
	join_ping_thread_lan_end();

		// free lists

	free(join_news);
	free(join_lan_list);
	free(join_wan_list);

		// shutdown gui
	
	gui_shutdown();
}

/* =======================================================

      Enabling
	        
======================================================= */

void join_activity_start(void)
{
		// stop any pinging
		
	join_ping_thread_lan_end();
	join_ping_thread_wan_end();
		
		// disable buttons and redraw
		
	element_enable(join_button_join_id,FALSE);
	element_enable(join_button_cancel_id,FALSE);
	
	gui_draw(1.0f,FALSE);
}

void join_activity_complete(bool single,char *msg)
{
	element_enable(join_button_join_id,single);
	element_enable(join_button_cancel_id,TRUE);
	
	if (!single) {
		element_set_value(join_lan_table_id,-1);
		element_set_value(join_wan_table_id,-1);
	}

	if (msg!=NULL) element_text_change(join_status_id,msg);
	
	gui_draw(1.0f,FALSE);
}

/* =======================================================

      Join Game
      
======================================================= */

void join_game(void)
{
	int								idx,net_uid,tick_offset;
	char							deny_reason[64],err_str[256];
	obj_type						*player_obj;
	join_server_host_type			*host;
	join_server_host_type			custom_host;

		// get selected host

	if (element_get_value(join_tab_id)==join_pane_custom_host) {
		join_ping_custom_host(&custom_host);
		host=&custom_host;
		
		if (host->unreachable) {
			error_setup("Unable to Join Game: Host is Unreachable","Network Game Canceled");
			server.next_state=gs_error;
			return;
		}
	}
	else {

		idx=element_get_value(join_lan_table_id);
		if (idx!=-1) {
			host=&join_lan_list->hosts[idx];
		}
		else {
			if (join_mode!=join_mode_wan_lan) return;
			
			idx=element_get_value(join_wan_table_id);
			host=&join_wan_list->hosts[idx];
		}
	
	}

		// reject if server is full

	if (host->player_list.count>=host->player_list.max_count) {
		error_setup("Unable to Join Game: Server is Full","Network Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// set the network flags
	
	if (!net_ip_to_address(host->ip,&net_setup.client.host_addr.ip,err_str)) {
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}
	
	net_setup.client.host_addr.port=net_port_host;

	net_setup.mode=net_mode_client;
	net_setup.client.latency=0;
	net_setup.score_limit=host->score_limit;
	net_setup.respawn_secs=host->respawn_secs;
	net_setup.game_reset_secs=host->game_reset_secs;
	net_setup.option_flags=host->option_flags;

		// setup game type and map

	net_setup.game_idx=net_client_find_game(host->game_name);
	if (net_setup.game_idx==-1) {
		sprintf(err_str,"Could not find game type: %s",host->game_name);
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}
	
	map.info.name[0]=0x0;
	strcpy(map.info.host_name,host->map_name);
	
		// start game
	
	if (!game_start(FALSE,skill_medium,0,0,err_str)) {
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}
	
		// start the map
		
	if (!map_start(FALSE,TRUE,err_str)) {
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// get player

	player_obj=server.obj_list.objs[server.player_obj_idx];
							
		// attempt to join

	net_uid=net_client_join_host_start(player_obj,&tick_offset,deny_reason);
	if (net_uid==-1) {
		map_end();
		game_end();
		sprintf(err_str,"Unable to Join Game: %s",deny_reason);
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}

	player_obj->remote.net_uid=net_uid;
	
		// start client network thread

	if (!net_client_start_message_queue(err_str)) {
		net_client_send_remote_remove(server.obj_list.objs[server.player_obj_idx]);
		net_client_join_host_end();
		map_end();
		game_end();
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// request synchs of objects
		// (sends remote_adds) and map groups

	net_client_request_object_synch(player_obj);
	net_client_request_group_synch(player_obj);
	
		// game is running
	
	server.next_state=gs_running;
}

/* =======================================================

      Join Input
      
======================================================= */

void join_click(void)
{
	int			id;
	
	id=-1;

		// keyboard

	if (input_get_keyboard_escape()) id=join_button_cancel_id;
	if (input_get_keyboard_return()) {
		if (element_get_enable(join_button_join_id)) id=join_button_join_id;
	}

	if (id==-1) id=gui_keyboard();

		// clicking

	if (id==-1) {
		id=gui_click();
		if (id!=-1) hud_click();
	}
		
	if (id==-1) return;

		// handle click

	switch (id) {
			
		case join_tab_id:
			join_tab_value=element_get_value(join_tab_id);
			join_create_pane();
			return;
	
		case join_button_join_id:
			join_game();
			break;
			
		case join_button_cancel_id:
			server.next_state=gs_intro;
			break;
			
		case join_button_rescan_id:
			element_enable(join_button_rescan_id,FALSE);
			join_ping_thread_lan_end();
			join_ping_thread_wan_end();
			join_ping_thread_lan_start();
			break;

		case join_lan_table_id:
			join_update_host_information(join_lan_table_id);
			break;

		case join_wan_table_id:
			join_ping_wan_host();
			break;

		case join_custom_ip_id:
			element_get_value_string(join_custom_ip_id,setup.network.custom_host_ip);
			setup_xml_write();
			element_enable(join_button_join_id,TRUE);
			break;

	}
}
	
/* =======================================================

      Run Join
      
======================================================= */

void join_run(void)
{
		// wan thread finishes can't draw from
		// thread so we trigger than here
		
	if (join_wan_updated) {
		join_ping_wan_host_update();
		join_wan_updated=FALSE;
	}
	
		// draw and click
		
	gui_draw(1.0f,TRUE);
	join_click();
}

