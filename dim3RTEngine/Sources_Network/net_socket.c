/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Network Socket Code

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

/* =======================================================

      Socket Open/Close
      
======================================================= */

d3socket net_open_udp_socket(void)
{
	return(socket(AF_INET,SOCK_DGRAM,0));
}

void net_close_socket(d3socket *sock)
{
		// shutdown any more sending or receiving

#ifndef D3_OS_WINDOWS
	shutdown(*sock,SHUT_RDWR);
#else
	shutdown(*sock,SD_BOTH);
#endif

		// close socket

	closesocket(*sock);
	
	*sock=D3_NULL_SOCKET;
}

/* =======================================================

      Socket Options
      
======================================================= */

void net_socket_blocking(d3socket sock,bool blocking)
{
#ifndef D3_OS_WINDOWS

	long		opt;
	
	opt=fcntl(sock,F_GETFL,NULL);
	
	if (blocking) {
		opt&=(~O_NONBLOCK);
	}
	else {
		opt|=O_NONBLOCK;
	}
	
	fcntl(sock,F_SETFL,opt);

#else
	u_long		opt;

	opt=blocking?0:1;
	ioctlsocket(sock,FIONBIO,&opt);
#endif
}

void net_socket_enable_broadcast(d3socket sock)
{
	int				val;

#ifndef D3_OS_WINDOWS
	val=1;
	setsockopt(sock,SOL_SOCKET,SO_BROADCAST,&val,sizeof(int));
#else
	val=TRUE;
	setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(const char*)&val,sizeof(BOOL));
#endif
}

/* =======================================================

      Network IP Resolves
      
======================================================= */

bool net_ip_to_address(char *ip,unsigned long *ip_addr,char *err_str)
{
	unsigned long		nip_addr;

	nip_addr=inet_addr(ip);
	if (nip_addr==INADDR_NONE) {
		sprintf(err_str,"Networking: Could not create address for %s",ip);
		return(FALSE);
	}

	*ip_addr=ntohl(nip_addr);
	return(TRUE);
}

/* =======================================================

      Network Binds
      
======================================================= */

bool net_bind_any(d3socket sock,int port,char *err_str)
{
	int					err;
	struct sockaddr_in	addr;
	
		// setup address
		
	memset(&addr,0x0,sizeof(struct sockaddr_in));
		
	addr.sin_family=AF_INET;
	addr.sin_port=htons((short)port);
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	
		// bind socket
		
	err=bind(sock,(struct sockaddr*)&addr,sizeof(struct sockaddr_in));
	if (err<0) {
		sprintf(err_str,"Networking: Could not bind to port INADDR_ANY:%d",port);
		return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      Network Status Utilities
      
======================================================= */
	
bool net_receive_ready(d3socket sock)
{
	fd_set					fd;
	struct timeval			timeout;
	
	timeout.tv_sec=0;
	timeout.tv_usec=0;
	
	FD_ZERO(&fd);
	FD_SET(sock,&fd);
	
#ifndef D3_OS_WINDOWS
	select((sock+1),&fd,NULL,NULL,&timeout);
#else
	select(0,&fd,NULL,NULL,&timeout);
#endif
	
	return(FD_ISSET(sock,&fd));
}

bool net_send_ready(d3socket sock)
{
	fd_set					fd;
	struct timeval			timeout;
	
	timeout.tv_sec=0;
	timeout.tv_usec=0;
	
	FD_ZERO(&fd);
	FD_SET(sock,&fd);
	
#ifndef D3_OS_WINDOWS
	select((sock+1),NULL,&fd,NULL,&timeout);
#else
	select(0,NULL,&fd,NULL,&timeout);
#endif

	return(FD_ISSET(sock,&fd));
}

/* =======================================================

      Network Send and Recv Utilities
      
======================================================= */

bool net_recvfrom_mesage(d3socket sock,net_address_type *addr,int *action,unsigned char *msg,int *msg_len)
{
	int						len;
	unsigned char			data[net_max_msg_size];
	network_header			*head;
	socklen_t				addr_in_len;
	struct sockaddr_in		addr_in;

		// read next datagram
		
	addr_in_len=sizeof(addr_in);
	len=(int)recvfrom(sock,data,net_max_msg_size,0,(struct sockaddr*)&addr_in,&addr_in_len);
	
		// check for errors
		
	if (len<0) {
	
			// ignore would block errors as we
			// are almost always out of blocking
		
#ifndef D3_OS_WINDOWS
		if (errno==EWOULDBLOCK) {
#else
		if (WSAGetLastError()==WSAEWOULDBLOCK) {
#endif
			*action=-1;
			return(TRUE);
		}
		
			// other errors mean socket has closed
		
		return(FALSE);
	}
	
		// setup return address
		
	if (addr!=NULL) {
		addr->ip=ntohl(addr_in.sin_addr.s_addr);
		addr->port=(int)ntohs(addr_in.sin_port);
	}

		// no data?
		
	if (len==0) {
		*action=-1;
		return(TRUE);
	}
	
		// get header and data
		
	head=(network_header*)data;
	
	*action=(int)ntohs(head->action);
	
	len=(int)ntohs(head->len);
	if (len>net_max_msg_size) len=net_max_msg_size;
	
	memmove(msg,(data+sizeof(network_header)),len);

	if (msg_len!=NULL) *msg_len=len;

	return(TRUE);
}

bool net_sendto_msg(d3socket sock,net_address_type *addr,int action,unsigned char *msg,int msg_len)
{
	int						send_sz;
	unsigned char			data[net_max_msg_size];
	network_header			*head;
	struct sockaddr_in		addr_in;
	
		// the header
		
	head=(network_header*)data;

	head->action=htons((short)action);
	head->len=htons((short)msg_len);
	
		// the data

	if (msg_len!=0) memmove((data+sizeof(network_header)),msg,msg_len);

		// send message
		
	addr_in.sin_family=AF_INET;
	addr_in.sin_port=htons((short)addr->port);
	addr_in.sin_addr.s_addr=htonl(addr->ip);
	
	send_sz=sizeof(network_header)+msg_len;
		
	return(sendto(sock,data,send_sz,0,(struct sockaddr*)&addr_in,sizeof(struct sockaddr_in))==send_sz);
}

