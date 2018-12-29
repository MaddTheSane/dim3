/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Client Functions Header

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

//
// network
//

extern void net_initialize(void);
extern void net_shutdown(void);

extern void net_create_project_hash(void);
extern int net_get_project_hash(void);
extern bool net_load_news(join_server_host_list_type *join_host_list,char *news);

extern void net_get_host_name(char *name);
extern void net_get_host_ip(char *ip_name,char *ip_resolve);

extern bool net_ip_local(char *ip);
extern void net_ip_local_broadcast(char *ip,char *broad_ip);

//
// sockets
//

extern inline d3socket net_open_udp_socket(void);
extern inline void net_close_socket(d3socket *sock);
extern inline void net_socket_blocking(d3socket sock,bool blocking);
extern inline void net_socket_enable_broadcast(d3socket sock);
extern bool net_ip_to_address(char *ip,unsigned long *ip_addr,char *err_str);
extern bool net_bind_any(d3socket sock,int port,char *err_str);
extern bool net_receive_ready(d3socket sock);
extern bool net_send_ready(d3socket sock);
extern bool net_recvfrom_mesage(d3socket sock,net_address_type *addr,int *action,unsigned char *msg,int *msg_len);
extern bool net_sendto_msg(d3socket sock,net_address_type *addr,int action,unsigned char *msg,int msg_len);

//
// http reads
//

extern char* net_get_http_file(char *host_name,int port,char *url,char *err_str);

//
// read queues
//

extern bool net_queue_initialize(net_queue_type *queue);
extern void net_queue_shutdown(net_queue_type *queue);
extern bool net_queue_feed(d3socket sock,net_queue_type *queue);
extern bool net_queue_push_message(net_queue_type *queue,int action,unsigned char *msg_data,int msg_len);
extern bool net_queue_check_message(net_queue_type *queue,net_queue_msg_type *msg);

//
// hosting
//

extern bool net_host_initialize(char *err_str);
extern void net_host_shutdown(void);

extern void net_host_game_setup(void);
extern bool net_host_game_start(char *err_str);
extern void net_host_game_end(void);

extern bool net_host_join_local_player(char *err_str);
extern void net_host_join_multiplayer_bots(void);

extern int net_host_player_remote_thread(void *arg);

extern void net_host_process_messages(void);

//
// host player lists
//

extern void net_host_player_initialize(void);
extern void net_host_player_shutdown(void);

extern int net_host_player_find_net_uid(int net_uid);
extern int net_host_player_find_net_addr(net_address_type *addr);

extern bool net_host_player_add_ok(char *name,char *deny_reason);
extern int net_host_player_add(net_address_type *addr,bool local,char *name,char *draw_name,int tint_color_idx);
extern int net_host_player_add_bot(obj_type *obj);
extern void net_host_player_remove_by_uid(int net_uid);
extern void net_host_player_create_info_player_list(network_reply_info_player_list *player_list);
extern void net_host_player_remote_route_msg(net_queue_msg_type *msg);

extern void net_host_player_send_pickup(obj_type *obj,obj_type *item_obj);
extern void net_host_player_send_updates(void);

extern void net_host_player_send_message_to_client(net_address_type *addr,int action,unsigned char *msg,int msg_len);
extern void net_host_player_send_message_to_clients_all(net_address_type *skip_addr,int action,unsigned char *msg,int msg_len);

//
// client host pinging and joining
//

extern int net_client_join_host_start(obj_type *obj,int *tick_offset,char *deny_reason);
extern void net_client_join_host_end(void);

extern int net_client_find_game(char *game_name);

//
// client receiving messages
//

extern bool net_client_start_message_queue(char *err_str);
extern void net_client_end_message_queue(void);
extern bool net_client_process_messages(void);

//
// client sending messages
//

extern void net_client_send_remote_remove(obj_type *obj);
extern void net_client_request_object_synch(obj_type *obj);
extern void net_client_request_group_synch(obj_type *obj);
extern void net_client_send_death(obj_type *obj,bool telefrag);
extern void net_client_send_remote_update(obj_type *obj);
extern void net_client_send_chat(obj_type *obj,char *str);
extern void net_client_send_sound(obj_type *obj,d3pnt *pnt,float pitch,char *name);
extern void net_client_send_projectile_add(obj_type *obj,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang);
extern void net_client_send_hitscan_add(obj_type *obj,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang);
extern void net_client_send_melee_add(obj_type *obj,char *weap_name,int radius,int distance,int damage,int force,d3pnt *pt,d3ang *ang);
extern void net_client_send_click(obj_type *clicking_obj,obj_type *clicked_obj);
