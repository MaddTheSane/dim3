/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Main Code

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
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern render_info_type		render_info;

int							gui_draw_tick,gui_background_image_idx;
char						gui_last_key;

/* =======================================================

      GUI Background
      
======================================================= */

void gui_background_load(char *background_path,char *bitmap_name)
{
	char		path[1024];

	if (!view_file_paths_bitmap_check_wide(path,background_path,bitmap_name)) return;
	gui_background_image_idx=view_images_load_single(path,TRUE,TRUE);
}

/* =======================================================

      GUI Initialize and Shutdown
      
======================================================= */

void gui_initialize(char *background_path,char *bitmap_name)
{
	int			x,y;

		// pause game
		
	game_time_pause_start();
	
		// stop playing ambients
		
	al_stop_all_looping_sources();

		// setup
		
	element_initialize();

	gui_background_image_idx=-1;
	if (bitmap_name!=NULL) gui_background_load(background_path,bitmap_name);
		
	cursor_initialize();

		// start mouse in middle of screen
		
	x=iface.scale_x>>1;
	y=iface.scale_y>>1;
	input_gui_set_position(x,y);

		// no last keypress

	gui_last_key=0x0;

		// draw tick

	gui_draw_tick=game_time_get_raw();
}

void gui_shutdown(void)
{
		// close OpenGL GUI
		
	if (gui_background_image_idx!=-1) view_images_free_single(gui_background_image_idx);
	cursor_shutdown();
	
	element_shutdown();

		// clear all movement from gui	
			
	input_clear();
	
		// restart any map ambients
		
	if (server.map_open) map_restart_ambient();
	
		// unpause game
		
	game_time_pause_end();
}

/* =======================================================

      GUI Backgrounds
      
======================================================= */

void gui_draw_background(float alpha)
{
	bitmap_type		*bitmap;

		// background color

	if (gui_background_image_idx==-1) {
		view_primitive_2D_color_quad(&iface.color.background,1.0f,0,iface.scale_x,0,iface.scale_y);
		return;
	}

		// background image

	bitmap=view_images_get_bitmap(gui_background_image_idx);
	view_primitive_2D_texture_quad(bitmap->gl_id,NULL,alpha,0,iface.scale_x,0,iface.scale_y,0.0f,1.0f,0.0f,1.0f,TRUE);
}

/* =======================================================

      GUI Draw
      
======================================================= */

void gui_draw(float background_alpha,bool cursor)
{
	int				tick;

		// gui draw sticks to 30fps

	tick=game_time_get_raw();
	if (tick<gui_draw_tick) return;

	gui_draw_tick+=(1000/30);
	
		// clear frame
		
	gl_frame_clear();
	gl_2D_view_interface();

		// draw items

	gui_draw_background(background_alpha);
	element_draw(TRUE);
	if ((cursor) && (app.state==as_active)) cursor_draw();

		// end frame

	gl_frame_swap();
}

void gui_draw_message(char *txt)
{
	int				x,y,lx,rx,ty,by,high;
	d3col			col;
	
	gl_frame_clear();
	gl_2D_view_interface();

		// background

	gui_draw_background(1.0f);

		// messages

	element_draw(FALSE);
	
		// get band size
		
	high=(gl_text_get_char_height(iface.font.text_size_large)+10)>>1;
	
	y=iface.scale_y>>1;
	ty=y-high;
	by=y+high;
	
	lx=0;
	rx=iface.scale_x;
	
		// draw the band
		
	view_primitive_2D_color_quad(&iface.color.control.header,1.0f,lx,rx,ty,by);
		
	x=iface.scale_x>>1;
	y=iface.scale_y>>1;
	
	col.r=col.g=col.b=0.0f;
	
	gl_text_start(font_interface_index,iface.font.text_size_large,FALSE);
	gl_text_draw(x,y,txt,tx_center,TRUE,&col,1.0f);
	gl_text_end();
	
	gl_frame_swap();
}

/* =======================================================

      GUI Clicking
      
======================================================= */

int gui_click(void)
{
	int				x,y;

		// no clicking if paused

	if (app.state!=as_active) return(-1);

		// get click location

	input_gui_get_position(&x,&y);

		// check scroll wheel for
		// tables and sliders

	if (input_mouse_gui_wheel_up()) {
		element_scroll_wheel(x,y,TRUE);
		return(-1);
	}

	if (input_mouse_gui_wheel_down()) {
		element_scroll_wheel(x,y,FALSE);
		return(-1);
	}
	
		// button down

	if (input_gui_is_click_down()) {
		element_click_down(x,y);
		return(-1);
	}

		// button up

	return(element_click_up(x,y));
}

/* =======================================================

      GUI Keyboard
      
======================================================= */

int gui_keyboard(void)
{
	char		ch;

		// no keys if paused

	if (app.state!=as_active) return(-1);

		// key press?

	ch=input_get_text_input_key();
	if (ch==0x0) {
		gui_last_key=0x0;
		return(-1);
	}

		// same as last key?

	if (tolower(gui_last_key)==tolower(ch)) return(-1);
	gui_last_key=ch;

		// send to elements

	return(element_key(ch));
}
