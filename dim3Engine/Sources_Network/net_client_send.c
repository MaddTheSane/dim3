/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Client Send Message to Host

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

extern map_type					map;
extern server_type				server;
extern view_type				view;
extern js_type					js;
extern setup_type				setup;

extern d3socket					client_socket;

extern network_setup_type		net_setup;

/* =======================================================

      Generic Send
	  
	  If we are a client, then send all messages
	  to host.  If we are on host (bot or host player),
	  then send messages to all clients
      
======================================================= */

static inline void net_client_send_generic(int action,unsigned char *msg,int msg_len)
{
	if (net_setup.mode==net_mode_client) {
		net_sendto_msg(client_socket,&net_setup.client.host_addr,action,msg,msg_len);
	}
	else {
		net_host_player_send_message_to_clients_all(NULL,action,msg,msg_len);
	}
}

/* =======================================================

      Host Leave
      
======================================================= */

void net_client_send_remote_remove(obj_type *obj)
{
	network_request_remote_remove			remove;
	
	remove.remove_net_uid=htons((short)obj->remote.net_uid);
	net_client_send_generic(net_action_request_remote_remove,(unsigned char*)&remove,sizeof(network_request_remote_remove));
}

/* =======================================================

      Host Synching
      
======================================================= */

void net_client_request_object_synch(obj_type *obj)
{
	net_client_send_generic(net_action_request_object_synch,NULL,0);
}

void net_client_request_group_synch(obj_type *obj)
{
	net_client_send_generic(net_action_request_group_synch,NULL,0);
}

/* =======================================================

      Object Update Messages
      
======================================================= */

void net_client_send_remote_update(obj_type *obj)
{
	network_request_remote_update	update;
	
		// no updates if pipe full
		
	if (!net_send_ready(client_socket)) return;
		
		// send update

	remote_update_pack(obj,view.chat.type_on,&update);
	net_client_send_generic(net_action_request_remote_update,(unsigned char*)&update,sizeof(network_request_remote_update));
}

/* =======================================================

      Object Death Messages
      
======================================================= */

void net_client_send_death(obj_type *obj,bool telefrag)
{
	int								net_uid_killer_obj;
	obj_type						*chk_obj;
	network_request_remote_death	death;
	
	death.death_net_uid=htons((short)obj->remote.net_uid);
	
		// normal deaths

	if (!telefrag) {
		net_uid_killer_obj=-1;
		
		if (obj->damage_obj_idx!=-1) {
			chk_obj=server.obj_list.objs[obj->damage_obj_idx];
			if (chk_obj!=NULL) {
				if ((chk_obj->type==object_type_player) || (chk_obj->type==object_type_remote_player)) {
					net_uid_killer_obj=chk_obj->remote.net_uid;
				}
			}
		}
		
		death.killer_net_uid=htons((short)net_uid_killer_obj);
		death.telefrag=htons(0);
	}

		// telefrag deaths

	else {
		chk_obj=server.obj_list.objs[obj->damage_obj_idx];		// only remote objects can telefrag each other, so no other checks necessary
		
		death.killer_net_uid=htons((short)chk_obj->remote.net_uid);
		death.telefrag=htons(1);
	}

	net_client_send_generic(net_action_request_remote_death,(unsigned char*)&death,sizeof(network_request_remote_death));
}

/* =======================================================

      Chats and Sound Messages
	        
======================================================= */

void net_client_send_chat(obj_type *obj,char *str)
{
	network_request_remote_chat		chat;
	
	chat.chat_net_uid=htons((short)obj->remote.net_uid);
	strcpy(chat.str,str);

	net_client_send_generic(net_action_request_remote_chat,(unsigned char*)&chat,sizeof(network_request_remote_chat));
}

void net_client_send_sound(obj_type *obj,d3pnt *pnt,float pitch,char *name)
{
	network_request_remote_sound	sound;

	sound.pnt_x=htonl(pnt->x);
	sound.pnt_y=htonl(pnt->y);
	sound.pnt_z=htonl(pnt->z);
	
	sound.fp_pitch=htonf(pitch);
	
	strcpy(sound.name,name);

	net_client_send_generic(net_action_request_remote_sound,(unsigned char*)&sound,sizeof(network_request_remote_sound));
}

/* =======================================================

      Weapon Messages
	        
======================================================= */

void net_client_send_projectile_add(obj_type *obj,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang)
{
	network_request_remote_fire		fire;
	
	fire.fire_net_uid=htons((short)obj->remote.net_uid);
	fire.fire_type=htons(net_remote_fire_type_projectile);

	fire.pt_x=htonl(pt->x);
	fire.pt_y=htonl(pt->y);
	fire.pt_z=htonl(pt->z);
	
	fire.fp_ang_x=htonf(ang->x);
	fire.fp_ang_y=htonf(ang->y);
	fire.fp_ang_z=htonf(ang->z);
	
	strcpy(fire.weap_name,weap_name);
	strcpy(fire.proj_setup_name,proj_setup_name);

	fire.radius=0;
	fire.distance=0;
	fire.damage=0;
	fire.force=0;
	
	net_client_send_generic(net_action_request_remote_fire,(unsigned char*)&fire,sizeof(network_request_remote_fire));
}

void net_client_send_hitscan_add(obj_type *obj,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang)
{
	network_request_remote_fire		fire;

	fire.fire_net_uid=htons((short)obj->remote.net_uid);
	fire.fire_type=htons(net_remote_fire_type_hit_scan);
	
	fire.pt_x=htonl(pt->x);
	fire.pt_y=htonl(pt->y);
	fire.pt_z=htonl(pt->z);
	
	fire.fp_ang_x=htonf(ang->x);
	fire.fp_ang_y=htonf(ang->y);
	fire.fp_ang_z=htonf(ang->z);
	
	strcpy(fire.weap_name,weap_name);
	strcpy(fire.proj_setup_name,proj_setup_name);

	fire.radius=0;
	fire.distance=0;
	fire.damage=0;
	fire.force=0;

	net_client_send_generic(net_action_request_remote_fire,(unsigned char*)&fire,sizeof(network_request_remote_fire));
}

void net_client_send_melee_add(obj_type *obj,char *weap_name,int radius,int distance,int damage,int force,d3pnt *pt,d3ang *ang)
{
	network_request_remote_fire		fire;
	
	fire.fire_net_uid=htons((short)obj->remote.net_uid);
	fire.fire_type=htons(net_remote_fire_type_melee);
	
	fire.pt_x=htonl(pt->x);
	fire.pt_y=htonl(pt->y);
	fire.pt_z=htonl(pt->z);
	
	fire.fp_ang_x=htonf(ang->x);
	fire.fp_ang_y=htonf(ang->y);
	fire.fp_ang_z=htonf(ang->z);
	
	strcpy(fire.weap_name,weap_name);
	fire.proj_setup_name[0]=0x0;
	
	fire.radius=htons((short)radius);
	fire.distance=htons((short)distance);
	fire.damage=htons((short)damage);
	fire.force=htons((short)force);

	net_client_send_generic(net_action_request_remote_fire,(unsigned char*)&fire,sizeof(network_request_remote_fire));
}

/* =======================================================

      Click Messages
	        
======================================================= */

void net_client_send_click(obj_type *clicking_obj,obj_type *clicked_obj)
{
	network_request_remote_click		click;
	
	click.clicking_net_uid=htons((short)clicking_obj->remote.net_uid);
	click.clicked_net_uid=htons((short)clicked_obj->remote.net_uid);
	
	net_client_send_generic(net_action_request_remote_click,(unsigned char*)&click,sizeof(network_request_remote_click));
}
