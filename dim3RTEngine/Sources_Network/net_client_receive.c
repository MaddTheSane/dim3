/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Client Receive Message from Host

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

extern map_type				map;
extern server_type			server;
extern js_type				js;
extern setup_type			setup;

extern bool					join_thread_quit;

int net_client_receive_thread(void *arg);			// forward reference

d3socket					client_socket;
bool						client_complete;
net_queue_type				client_queue;
SDL_Thread					*client_thread;

/* =======================================================

      Client Message Queue
      
======================================================= */

bool net_client_start_message_queue(char *err_str)
{
		// create cache

	if (!net_queue_initialize(&client_queue)) {
		strcpy(err_str,"Networking: Out of memory");
		return(FALSE);
	}
	
		// start receive thread
		
	client_thread=SDL_CreateThread(net_client_receive_thread,"client",NULL);
	if (client_thread!=NULL) return(TRUE);
	
		// could not start thread
		
	net_queue_shutdown(&client_queue);
		
	strcpy(err_str,"Networking: Could not start client receive thread");
	return(FALSE);
}

void net_client_end_message_queue(void)
{
		// shutdown message handler
		
	client_complete=FALSE;
	SDL_WaitThread(client_thread,NULL);

		// free queue
		
	net_queue_shutdown(&client_queue);
}

int net_client_receive_thread(void *arg)
{
	bool				client_err;
	
	client_err=FALSE;
	client_complete=TRUE;
		
	while (client_complete) {

			// feed the queues from the socket
			// if there's an error, break out of loop
			// and cancel network game
			
		if (!net_queue_feed(client_socket,&client_queue)) {
			client_err=TRUE;
			break;
		}
	
			// sleep so thread doesn't spike processor
			// as it only reads when data is ready
			
		usleep(10000);
	}

		// if there was an error, put a exit on
		// the queue as if it came from the host
		
	if (client_err) net_queue_push_message(&client_queue,net_action_request_host_exit,NULL,0);
	
		// exit thread
	
	return(0);
}

/* =======================================================

      Client Process Queue
      
======================================================= */

bool net_client_process_messages(void)
{
	int						count;
	net_queue_msg_type		msg;
	
	count=0;
	
	while (count<client_message_per_loop_count) {
	
			// check for messages

		if (!net_queue_check_message(&client_queue,&msg)) return(TRUE);
		
			// if at score limit, only accept reset messages
			// or game exits
			
		if (server.state==gs_score_limit) {
			if ((msg.action!=net_action_request_game_reset) && (msg.action!=net_action_request_host_exit)) continue;
		}
		
			// run message
			
		if (!remote_route_message(&msg)) return(FALSE);
	}
	
	return(TRUE);
}
