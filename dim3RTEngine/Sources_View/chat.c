/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Chat Routines

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

extern app_type				app;
extern view_type			view;
extern iface_type			iface;

/* =======================================================

      Chat
      
======================================================= */

void chat_clear_messages(void)
{
	view.chat.nline=0;
	
	view.chat.type_on=FALSE;
	view.chat.type_str[0]=0x0;
}

void chat_add_message(char *name,char *str,d3col *col)
{
	int					idx;
	view_chat_line_type	*line;

		// if a dedicated host,
		// then just stdout chats

	if (app.dedicated_host) {
		fprintf(stdout,"%s: %s\n",name,str);
		return;
	}
	
		// need to remove a line?
		
	if (view.chat.nline>=max_view_chat_lines) {
		memmove(&view.chat.lines[0],&view.chat.lines[1],(sizeof(view_chat_line_type)*(max_view_chat_lines-1)));
		idx=view.chat.nline-1;
	}
	else {
		idx=view.chat.nline;
		view.chat.nline++;
	}
	
		// set line
		
	line=&view.chat.lines[idx];
	
	strcpy(line->name,name);
	strcpy(line->str,str);
	memmove(&line->col,col,sizeof(d3col));
	
		// last addition tick
		
	view.chat.remove_tick=game_time_get()+(iface.chat.last_add_life_sec*1000);
}

void chat_time_out(void)
{
	int				tick;

	tick=game_time_get();
	if ((view.chat.nline==0) || (view.chat.remove_tick>tick)) return;
	
	memmove(&view.chat.lines[0],&view.chat.lines[1],(sizeof(view_chat_line_type)*(max_view_chat_lines-1)));
	view.chat.nline--;
	
	view.chat.remove_tick=tick+(iface.chat.next_life_sec*1000);
}
