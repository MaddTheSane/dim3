/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Menus

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

extern app_type				app;
extern server_type			server;
extern map_type				map;
extern view_type			view;
extern iface_type			iface;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

/* =======================================================

      Start and Stop Menu
      
======================================================= */

void menu_draw_start(char *name)
{
	int				sub_idx;

		// if not hosting, pause

	if (net_setup.mode!=net_mode_host) game_time_pause_start();

		// clear input and stop sounds

	input_clear();
	al_stop_all_looping_sources();
	
		// go into pause menu

	view.menu.fade_in=TRUE;
	view.menu.fade_out=FALSE;
	view.menu.active=FALSE;

	view.menu.fade_start_tick=game_time_get_raw();

	view.menu.menu_idx=0;
	view.menu.item_idx=-1;
	view.menu.mouse_down=FALSE;
	view.menu.key_down=FALSE;

		// any sub menu?

	if (name!=NULL) {
		sub_idx=iface_menu_find_idx(&iface,name);
		if (sub_idx!=-1) view.menu.menu_idx=sub_idx;
	}
	
		// always flip off scores
		// if they are on
		
	view.score.on=FALSE;
	
		// setup cursor

	cursor_initialize();
	input_gui_set_position((iface.scale_x>>1),(iface.scale_y>>1));

		// remember last mouse

	input_gui_get_hilite_position(&view.menu.mouse_pnt.x,&view.menu.mouse_pnt.y);
}

void menu_draw_end(bool fade)
{
		// leave menu

	view.menu.fade_in=FALSE;
	view.menu.fade_out=fade;
	view.menu.active=FALSE;

	view.menu.fade_start_tick=game_time_get_raw();

	cursor_shutdown();

		// clear input and restart sounds

	input_clear();
	if (server.map_open) map_restart_ambient();
	
		// if not hosting, unpause

	if (net_setup.mode!=net_mode_host) game_time_pause_end();
}

/* =======================================================

      Find Menu Item
      
======================================================= */

int menu_find_item(iface_menu_type *menu,int kx,int ky)
{
	int						n,x,y,high,half_high,lft,rgt;
	iface_menu_item_type	*item;
	
	high=gl_text_get_char_height(iface.font.text_size_large)+10;
	half_high=high>>1;

	x=iface.scale_x>>1;
	y=(iface.scale_y-((high+5)*menu->nitem))>>1;

	lft=x-(iface.scale_x>>2);
	rgt=x+(iface.scale_x>>2);

	item=menu->items;

	for (n=0;n!=menu->nitem;n++) {
			
		if (!((net_setup.mode!=net_mode_none) && (item->multiplayer_disable))) {
			if ((kx>=lft) && (kx<=rgt) && (ky>=(y-half_high)) && (ky<=(y+half_high))) return(n);
		}
			
		y+=(high+5);
		item++;
	}

	return(-1);
}

/* =======================================================

      Menu Selection and Input
      
======================================================= */

bool menu_select(void)
{
	int						sub_idx;
	iface_menu_type			*menu;
	iface_menu_item_type	*item;
	
		// find clicked item
		
	menu=&iface.menu_list.menus[view.menu.menu_idx];
	item=&menu->items[view.menu.item_idx];
		
		// going into sub-menu
			
	if (item->sub_menu[0]!=0x0) {
		sub_idx=iface_menu_find_idx(&iface,item->sub_menu);
		if (sub_idx==-1) return(FALSE);
			
		view.menu.menu_idx=sub_idx;
		view.menu.item_idx=0;
		return(FALSE);
	}
	
		// exit menus
		
	menu_draw_end(FALSE);
			
		// quit item
		
	if (item->quit) {

		if (app.editor_override.on) {			// if in editor override, just quit
			app.loop_quit=TRUE;
		}
		else {
			server.next_state=gs_intro;
		}

		return(TRUE);
	}
	
		// event items
		
	scripts_post_event_console(js.game_script_idx,-1,sd_event_menu,0,item->item_id);
	
	return(TRUE);
}

void menu_previous_item(void)
{
	iface_menu_type			*menu;
		
	menu=&iface.menu_list.menus[view.menu.menu_idx];
	
	view.menu.item_idx--;
	if (view.menu.item_idx<0) view.menu.item_idx=menu->nitem-1;
}

void menu_next_item(void)
{
	iface_menu_type			*menu;
		
	menu=&iface.menu_list.menus[view.menu.menu_idx];
	
	view.menu.item_idx++;
	if (view.menu.item_idx>=menu->nitem) view.menu.item_idx=0;
}

void menu_input(void)
{
	int					x,y,item_idx;
	bool				prev,next,rtn;
	iface_menu_type		*menu;

		// turn on/off menu

	if (input_action_get_state_single(nc_menu)) {
	
		if ((view.menu.fade_in) || (view.menu.fade_out)) return;
		
		if (!view.menu.active) {
			menu_draw_start(NULL);
		}
		else {
			menu_draw_end(TRUE);
		}

		return;
	}

		// active menu input

	if (!view.menu.active) return;

	menu=&iface.menu_list.menus[view.menu.menu_idx];

		// if the mouse moves, we can
		// change the hilite selection
		// so it doesn't interfere with keys

		// if item idx==-1, then we haven't
		// gotten any selection yet

	input_gui_get_hilite_position(&x,&y);

	if ((view.menu.item_idx==-1) || ((x!=view.menu.mouse_pnt.x) && (y!=view.menu.mouse_pnt.y))) {
		view.menu.mouse_pnt.x=x;
		view.menu.mouse_pnt.y=y;

		item_idx=menu_find_item(menu,x,y);
		if (item_idx==-1) {
			if (view.menu.item_idx==-1) view.menu.item_idx=0;
		}
		else {
			view.menu.item_idx=item_idx;
		}
	}

		// check key presses

	if (input_get_keyboard_escape()) {
		menu_draw_end(TRUE);
		return;
	}

	prev=input_get_keyboard_prev();
	next=input_get_keyboard_next();
	rtn=input_get_keyboard_return();

	if ((!prev) && (!next) && (!rtn)) {
		view.menu.key_down=FALSE;
	}
	else {
		if (!view.menu.key_down) {
			view.menu.key_down=TRUE;

			if (rtn) {
				menu_select();
				return;
			}
			
			if (prev) {
				menu_previous_item();
				return;
			}
			
			if (next) {
				menu_next_item();
				return;
			}
		}
	}

		// only click on mouse ups so we don't get
		// the mouse down travelling through to the game

	if (input_gui_is_click_down()) {
		view.menu.mouse_down=TRUE;
		return;
	}
	
	if (view.menu.mouse_down) {
		view.menu.mouse_down=FALSE;

		input_gui_get_position(&x,&y);
		item_idx=menu_find_item(menu,x,y);

		if (item_idx!=-1) {
			view.menu.item_idx=item_idx;
			menu_select();
		}
	}
}

/* =======================================================

      Menu Drawing
      
======================================================= */

void menu_draw(void)
{
	int						n,raw_tick,x,y,lft,rgt,top,bot,my,
							high,half_high;
	float					alpha,draw_alpha;
	d3col					back_col,back_col2;
	iface_menu_type			*menu;
	iface_menu_item_type	*item;

	if ((!view.menu.fade_in) && (!view.menu.fade_out) && (!view.menu.active)) return;
	
	gl_2D_view_interface();

		// get current menu

	menu=&iface.menu_list.menus[view.menu.menu_idx];

		// get the alpha

	raw_tick=game_time_get_raw();

	alpha=1.0f;

	if (view.menu.fade_in) {
		raw_tick-=view.menu.fade_start_tick;
		if (raw_tick>=300) {
			view.menu.fade_in=FALSE;
			view.menu.active=TRUE;
		}
		else {
			alpha=((float)raw_tick)/300.0f;
		}
	}
	
	if (view.menu.fade_out) {
		raw_tick-=view.menu.fade_start_tick;
		if (raw_tick>=300) {
			view.menu.fade_out=FALSE;
			return;
		}
		else {
			alpha=1.0f-(((float)raw_tick)/300.0f);
		}
	}

		// get height

	high=gl_text_get_char_height(iface.font.text_size_large)+10;
	half_high=high>>1;

		// draw the background

	lft=(iface.scale_x>>2)-5;
	rgt=iface.scale_x-((iface.scale_x>>2)-5);

	top=((iface.scale_y-((high+5)*menu->nitem))>>1)-((high>>1)+5);
	bot=top+(((high+5)*menu->nitem)+5);

	view_primitive_2D_color_quad(&iface.color.dialog.background,alpha,lft,rgt,top,bot);
	
	glLineWidth(2.0f);
	view_primitive_2D_line_quad(&iface.color.dialog.outline,alpha,lft,rgt,top,bot);
	glLineWidth(1.0f);

		// draw the menus
		
	x=iface.scale_x>>1;
	y=(iface.scale_y-((high+5)*menu->nitem))>>1;
	
	lft=x-(iface.scale_x>>2);
	rgt=x+(iface.scale_x>>2);
	
	item=menu->items;

	for (n=0;n!=menu->nitem;n++) {
	
			// colors

		if ((net_setup.mode!=net_mode_none) && (item->multiplayer_disable)) {
			draw_alpha=alpha*0.2f;
		}
		else {
			draw_alpha=alpha;
			
			if (n==view.menu.item_idx) {
				memmove(&back_col,&iface.color.menu.mouse_over,sizeof(d3col));
			}
			else {
				memmove(&back_col,&iface.color.menu.background,sizeof(d3col));
			}
		}
		
			// background
			
		back_col2.r=back_col.r*element_gradient_factor_background;
		back_col2.g=back_col.g*element_gradient_factor_background;
		back_col2.b=back_col.b*element_gradient_factor_background;
		
		top=y-half_high;
		bot=y+half_high;
		
		my=(top+bot)>>1;
	
		view_primitive_2D_color_poly(lft,top,&back_col2,rgt,top,&back_col2,rgt,my,&back_col,lft,my,&back_col,draw_alpha);
		view_primitive_2D_color_poly(lft,my,&back_col,rgt,my,&back_col,rgt,bot,&back_col2,lft,bot,&back_col2,draw_alpha);
		
		view_primitive_2D_line_quad(&iface.color.menu.outline,draw_alpha,lft,rgt,top,bot);

			// text
			
		gl_text_start(font_interface_index,iface.font.text_size_large,FALSE);
		gl_text_draw(x,y,item->data,tx_center,TRUE,&iface.color.menu.text,draw_alpha);
		gl_text_end();

		y+=(high+5);
		item++;
	}

		// cursor

	if ((view.menu.active) && (app.state==as_active)) cursor_draw();
}


