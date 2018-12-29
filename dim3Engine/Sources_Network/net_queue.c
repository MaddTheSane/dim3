/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Network Queues

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

/* =======================================================

      Initialize and Shutdown Queues
      
======================================================= */

bool net_queue_initialize(net_queue_type *queue)
{
	int				sz;

		// create queue
		
	queue->count=0;
	queue->msgs=NULL;
	
	sz=net_queue_max_message*sizeof(net_queue_msg_type);

	queue->msgs=(net_queue_msg_type*)malloc(sz);
	if (queue->msgs==NULL) return(FALSE);
	
	bzero(queue->msgs,sz);
	
		// create queue lock

	queue->lock=SDL_CreateMutex();

	return(TRUE);
}

void net_queue_shutdown(net_queue_type *queue)
{
	if (queue->msgs==NULL) return;
	
	free(queue->msgs);
	SDL_DestroyMutex(queue->lock);
}

/* =======================================================

      Feed Data Into Queue
      
======================================================= */

bool net_queue_feed(d3socket sock,net_queue_type *queue)
{
	bool				err;
	net_queue_msg_type	*queue_msg;

	SDL_mutexV(queue->lock);
	
		// read data until there is no more or
		// the queue is full

	err=FALSE;
		
	while (net_receive_ready(sock)) {
	
		if (queue->count>=net_queue_max_message) break;
	
			// get the next message
		queue_msg=queue->msgs+queue->count;

		if (!net_recvfrom_mesage(sock,&queue_msg->addr,&queue_msg->action,queue_msg->msg,&queue_msg->msg_len)) {
			err=TRUE;			// socket has closed, error out
			break;
		}

			// any messages?

		if (queue_msg->action==-1) break;
	
			// got a new message!

		queue->count++;
	}
	
	SDL_mutexV(queue->lock);
	
	return(!err);
}

/* =======================================================

      Push Message On Queue
      
======================================================= */

bool net_queue_push_message(net_queue_type *queue,int action,unsigned char *msg_data,int msg_len)
{
	net_queue_msg_type	*queue_msg;

	SDL_mutexP(queue->lock);
	
		// room for another message?
		
	if (queue->count>=net_queue_max_message) {
		SDL_mutexV(queue->lock);
		return(FALSE);
	}
	
		// put message on queue
		
	queue_msg=queue->msgs+queue->count;

	queue_msg->addr.ip=0;
	queue_msg->addr.port=0;

	queue_msg->action=action;

	queue_msg->msg_len=msg_len;
	if (msg_data!=NULL) memmove(queue_msg->msg,msg_data,msg_len);
		
		// another message in queue

	queue->count++;

	SDL_mutexV(queue->lock);

	return(TRUE);
}

/* =======================================================

      Get Next Message From Queue
      
======================================================= */

bool net_queue_check_message(net_queue_type *queue,net_queue_msg_type *msg)
{
	SDL_mutexP(queue->lock);
	
		// any messages any queue?
		
	if (queue->count==0) {
		SDL_mutexV(queue->lock);
		return(FALSE);
	}

		// FIFO, use first message

	memmove(msg,queue->msgs,sizeof(net_queue_msg_type));
	
		// remove retreived mesage
		
	if (queue->count>1) memmove(queue->msgs,(queue->msgs+1),((queue->count-1)*sizeof(net_queue_msg_type)));
	queue->count--;
		
	SDL_mutexV(queue->lock);
		
	return(TRUE);
}

