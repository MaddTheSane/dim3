/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Errors

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
#include "scripts.h"

#define error_frame_id				0
#define error_close_id				1

extern server_type			server;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern js_type				js;

/* =======================================================

      Error Operations
      
======================================================= */

void error_open(void)
{
	int							fx,fy,x,y,wid,high,
								margin,padding,control_y_add;
	element_frame_button_type	butts[1]={{error_close_id,"Close",TRUE}};

		// setup gui
		
	gui_initialize(NULL,NULL);
	
		// the frame

	margin=element_get_margin();
	padding=element_get_padding();
	control_y_add=element_get_control_separation_high();
	
	high=(padding*4)+(control_y_add*2)+(element_get_button_high()+element_get_frame_title_high());

	fx=margin;
	fy=(iface.scale_y-high)/2;
	wid=iface.scale_x-(margin*2);

	element_frame_add("Error",error_frame_id,fx,fy,wid,high,-1,0,NULL,1,butts);

		// the text
	
	element_get_frame_inner_space(error_frame_id,&fx,&fy,&wid,&high);


	x=fx+(wid/2);
	y=fy+control_y_add;

	element_text_add(view.error.str,-1,x,y,iface.font.text_size_small,tx_center,NULL,FALSE);
	
	y+=control_y_add;
	element_text_add(view.error.str_2,-1,x,y,iface.font.text_size_small,tx_center,NULL,FALSE);
}

void error_close(void)
{
	gui_shutdown();
}

void error_setup(char *err_str,char *err_str_2)
{
	strcpy(view.error.str,err_str);
	strcpy(view.error.str_2,err_str_2);
}

/* =======================================================

      Run Error Page
      
======================================================= */

void error_run(void)
{
	int				id;

	gui_draw(1.0f,TRUE);
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
		// always exit to intro
		
	if (id==error_close_id) server.next_state=gs_intro;
}


