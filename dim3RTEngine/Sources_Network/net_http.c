/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Network HTTP Reader

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

      Connect to Webserver
      
======================================================= */

d3socket net_get_http_file_connect(char *host_name,int port,int secs,char *err_str)
{
	int					err,count;
	bool				in_progress,connect_ok;
	char				*ip;
	d3socket			sock;
	struct hostent		*hent;
	struct sockaddr_in	addr;
	fd_set				fd;
	struct timeval		timeout;

		// get IP address from host name
		
	hent=gethostbyname(host_name);
	if (hent==NULL) {
		sprintf(err_str,"Could not resolve host name %s",host_name);
		return(D3_NULL_SOCKET);
	}
	
	ip=inet_ntoa(*(struct in_addr*)(hent->h_addr_list[0]));
	
		// get host address
		
	memset(&addr,0x0,sizeof(struct sockaddr_in));
		
	addr.sin_family=AF_INET;
	addr.sin_port=htons((short)port);
	addr.sin_addr.s_addr=inet_addr(ip);
	if (addr.sin_addr.s_addr==INADDR_NONE) {
		sprintf(err_str,"Networking: Could not create address for %s",ip);
		return(D3_NULL_SOCKET);
	}

		// create socket and put in non-blocking
		
	sock=socket(AF_INET,SOCK_STREAM,0);
	if (sock==D3_NULL_SOCKET) {
		strcpy(err_str,"Unable to create socket");
		return(D3_NULL_SOCKET);
	}

	net_socket_blocking(sock,FALSE);
	
		// try to connect
	
	err=connect(sock,(struct sockaddr*)&addr,sizeof(struct sockaddr_in));
	if (err<0) {

#ifndef D3_OS_WINDOWS
		in_progress=(errno==EINPROGRESS);
#else
		in_progress=(WSAGetLastError()==WSAEWOULDBLOCK);
#endif

		if (!in_progress) {
			net_close_socket(&sock);
			sprintf(err_str,"Networking: Could not connect to %s:%d",ip,port);
			return(D3_NULL_SOCKET);
		}
	}

		// if we can write to socket,
		// then we've connected

	count=secs*1000;
	connect_ok=FALSE;
	
	while (count>0) {

		timeout.tv_sec=0;
		timeout.tv_usec=0;

		FD_ZERO(&fd);
		FD_SET(sock,&fd);

		#ifndef D3_OS_WINDOWS
			select((sock+1),NULL,&fd,NULL,&timeout);
		#else
			select(0,NULL,&fd,NULL,&timeout);
		#endif

		if (FD_ISSET(sock,&fd)) {
			connect_ok=TRUE;
			break;
		}

		usleep(1000);
		count--;
	}

		// return connection state

	if (!connect_ok) {
		net_close_socket(&sock);
		sprintf(err_str,"Networking: No connection to %s:%d",ip,port);
		return(D3_NULL_SOCKET);
	}

		// put socket back into blocking mode

	net_socket_blocking(sock,TRUE);
	
	return(sock);
}

/* =======================================================

      Send and Receive Data
      
======================================================= */

bool net_get_http_file_send(d3socket sock,char *host_name,char *url)
{
	int				len,sent_len;
	char			http[1024];

		// send the get

	strcpy(http,"GET ");
	strcat(http,url);
	strcat(http," HTTP/1.1\r\n");
	strcat(http,"Host: ");
	strcat(http,host_name);
	strcat(http,"\r\n");
	strcat(http,"User-Agent: dim3\r\n");
	strcat(http,"Accept: text/plain\r\n");
	strcat(http,"Connection: close\r\n");
	strcat(http,"\r\n");

	len=strlen(http);

		// send the data
			
	sent_len=send(sock,http,len,0);
	return(sent_len==len);
}

char* net_get_http_file_get(d3socket sock,char *err_str)
{
	int					max_len,rcv_size,rbyte,content_offset,content_length;
	bool				ok;
	char				*c,*data,*content_data;
	char				str[256];
	fd_set				fd;
	struct timeval		timeout;

		// retrieve the data
		// only get up to 10K

	max_len=10*1024;			

	data=malloc(max_len+1);
	if (data==NULL) {
		strcpy(err_str,"Out of memory");
		return(NULL);
	}

	rcv_size=0;
	ok=FALSE;
	content_offset=content_length=-1;

	while (TRUE) {

			// any data to get?

		timeout.tv_sec=0;
		timeout.tv_usec=0;
	
		FD_ZERO(&fd);
		FD_SET(sock,&fd);
	
		#ifndef D3_OS_WINDOWS
			select((sock+1),&fd,NULL,NULL,&timeout);
		#else
			select(0,&fd,NULL,NULL,&timeout);
		#endif
	
		if (!FD_ISSET(sock,&fd)) {
			usleep(1000);
			continue;
		}

			// grab data

		rbyte=recv(sock,(data+rcv_size),(max_len-rcv_size),0);
		if (rbyte<=0) break;

		rcv_size+=rbyte;
		if (rcv_size>=max_len) break;

		*(data+rcv_size)=0x0;
		
			// check for 200 OK
		
		if (!ok) {
			c=strchr(data,'\n');
			if (c!=NULL) {
				strncpy(str,data,256);
				str[255]=0x0;
				
				c=strchr(str,'\n');
				if (c!=NULL) {
					*c=0x0;
					if (strstr(str,"200 OK")==NULL) {
						strcpy(err_str,"File not found");
						free(data);
						return(NULL);
					}
					
					ok=TRUE;
				}
			}
		}
				
			// check for content length

		if (content_offset==-1) {
			c=strstr(data,"\r\n\r\n");
			if (c!=NULL) content_offset=(int)((c+4)-data);
		}

		if ((content_offset!=-1) && (content_length==-1)) {
			c=strstr(data,"Content-Length: ");
			if (c!=NULL) {
				c+=16;
				strncpy(str,c,32);
				str[31]=0x0;
				c=strchr(str,'\r');
				if (c!=NULL) {
					*c=0x0;
					content_length=atoi(str);

						// check for wild content lengths

					if ((content_length<0) || (content_length>(max_len-content_offset))) {
						strcpy(err_str,"Unable to retrieve file");
						free(data);
						return(NULL);
					}
						
				}
			}
		}

			// are we at content length?

		if ((content_offset!=-1) && (content_length!=-1)) {
			if (rcv_size>=(content_offset+content_length)) break;
		}

		usleep(1000);
	}
	
	*(data+rcv_size)=0x0;

		// if we didn't get a content length,
		// determine it from size

	if (content_length==-1) {
		content_length=rcv_size-content_offset;
		if (content_length<=0) {
			strcpy(err_str,"Unable to retrieve file");
			free(data);
			return(NULL);
		}
	}

		// get the content data

	content_data=malloc(content_length+1);
	if (content_data==NULL) {
		strcpy(err_str,"Unable to retrieve file");
		free(data);
		return(NULL);
	}

	memmove(content_data,(data+content_offset),content_length);
	content_data[content_length]=0x0;
	
	free(data);

		// return content

	return(content_data);
}

/* =======================================================

      Network HTTP File
      
======================================================= */

char* net_get_http_file(char *host_name,int port,char *url,char *err_str)
{
	char			*content_data;
	d3socket		sock;

		// connect to HTTP server

	sock=net_get_http_file_connect(host_name,port,5,err_str);
	if (sock==D3_NULL_SOCKET) return(NULL);

		// send the get

	if (!net_get_http_file_send(sock,host_name,url)) {
		net_close_socket(&sock);
		strcpy(err_str,"Unable to retrieve file");
		return(NULL);
	}

		// retrieve the data
	
	content_data=net_get_http_file_get(sock,err_str);
	
		// close the socket

	net_close_socket(&sock);

	return(content_data);
}
