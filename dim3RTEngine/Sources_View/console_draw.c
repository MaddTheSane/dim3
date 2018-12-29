/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Console Routines

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
#include "scripts.h"

extern app_type					app;
extern view_type				view;
extern server_type				server;
extern iface_type				iface;
extern js_type					js;
extern setup_type				setup;
extern network_setup_type		net_setup;
extern render_info_type			render_info;

char							console_input_str[max_view_console_txt_sz];

/* =======================================================

      Draw Console
      
======================================================= */

void console_draw(void)
{
	int						n,y,ty,y_add,txt_size;
	char					str[256];
	d3col					col;
	view_console_line_type	*cline;

		// get text size so we can fit
		// max_view_console_line in the console
		
		// need to convert to HUD scale
		// and deal with height factor

	y=(int)(((float)view.screen.y_sz)*(1.0f-console_screen_percent));
	ty=(y*iface.scale_y)/view.screen.y_sz;

	txt_size=((int)((float)(iface.scale_y-ty)/text_height_factor)/max_view_console_lines);

	y_add=gl_text_get_char_height(txt_size);

		// start drawing

	gl_2D_view_interface();

	glDisable(GL_BLEND);

		// line

	col.r=col.g=col.b=1.0f;
	view_primitive_2D_line(&col,1.0f,0,ty,iface.scale_x,ty);

		// setup text drawing

	gl_text_start(font_interface_index,txt_size,FALSE);

		// console lines

	y=ty+y_add;
	
	cline=view.console.lines;
	
	for (n=0;n!=view.console.nline;n++) {
		gl_text_draw(5,y,cline->txt,tx_left,FALSE,&cline->color,1.0f);
		y+=y_add;
		cline++;
	}
	
		// input line
		
	strcpy(str,"]");
	strcat(str,console_input_str);
	if (((game_time_get_raw()>>8)&0x1)==0x0) strcat(str,"_");
	
	col.r=col.g=col.b=0.8f;
	gl_text_draw(5,y,str,tx_left,FALSE,&col,1.0f);

		// version

	sprintf(str,"%s %s",iface.project.name,dim3_version);
	gl_text_draw((iface.scale_x-5),(ty+y_add),str,tx_right,FALSE,&col,1.0f);

	gl_text_end();
}

/* =======================================================

      Special Commands
      
======================================================= */

bool console_builtin_commands(void)
{
		// console quit
		
	if ((strcasecmp(console_input_str,"quit")==0) || (strcasecmp(console_input_str,"exit")==0)) {
		app.loop_quit=TRUE;
		return(TRUE);
	}
	
		// console map
		
	if ((iface.setup.game_debug) && (net_setup.mode==net_mode_none)) {
		if (strncasecmp(console_input_str,"map ",4)==0) {
			if (debug_change_map((char*)&console_input_str[4])) view.console.on=FALSE;
			return(TRUE);
		}
	}
	
	return(FALSE);
}

/* =======================================================

      Console Input
      
======================================================= */
	
void console_input(void)
{
	int				len;
	char			ch;

		// no console if no debug tab

	if (!iface.setup.game_debug) return;
	
		// if console is off, check for on key
		
	if (!view.console.on) {
		if (input_action_get_state_single(nc_console)) {
			view.console.on=TRUE;
			input_clear_text_input();
		}
		return;
	}
	
		// console or menu key exits
		
	if ((input_action_get_state_single(nc_console)) || (input_action_get_state_single(nc_menu))) {
		view.console.on=FALSE;
		return;
	}
	
		// typing
	
	ch=input_get_text_input_key();
	if (ch==0x0) return;
	
		// backspace
		
	if (ch==0x8) {
		len=strlen(console_input_str);
		if (len>0) console_input_str[len-1]=0x0;
		return;
	}
	
		// entering command
		
	if (ch==0xD) {
		console_add(console_input_str);

		if (!console_builtin_commands()) {
			scripts_post_event_console(js.game_script_idx,-1,sd_event_console,0,0);
		}

		console_input_str[0]=0x0;
		return;
	}
	
		// adding char to command
		
	if (ch<0x20) return;
		
	len=strlen(console_input_str);
	if (len>=(max_view_console_txt_sz-1)) return;
	
	console_input_str[len]=ch;
	console_input_str[len+1]=0x0;
}

