/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Window Routines

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

map_type						map;
model_type						model;
file_path_setup_type			file_path_setup;
iface_type						iface;
app_state_type					state;

extern tool_palette_type		project_tool_palette,map_tool_palette,model_tool_palette;
extern list_palette_type		file_palette,project_palette,map_palette,model_palette;

/* =======================================================

      Initialize and Shutdown
	        
======================================================= */

void main_wind_initialize(void)
{
		// initializations
		
	text_initialize();

	project_tool_palette_initialize();
	map_tool_palette_initialize();
	model_tool_palette_initialize();

	list_palette_initialize();
	file_palette_initialize();
	project_palette_initialize();
	map_palette_initialize();
	model_palette_initialize();

	tool_tip_initialize();

		// map setup

	view_initialize();

		// model setup

	model_vertex_mask_initialize();
	model_poly_mask_initialize();
   
        // misc setup

	state.map.vertex_mode=vertex_mode_none;
	state.map.select_mode=select_mode_mesh;
	state.map.grid_mode=grid_mode_small;
	state.map.handle_mode=handle_mode_rotate;
	
	state.map.free_look=TRUE;
	state.map.select_add=FALSE;
	
	state.map.drag_handle_idx=-1;

		// set title

	main_wind_set_title();
}

void main_wind_shutdown(void)
{
	view_shutdown();

	model_vertex_mask_shutdown();
	model_poly_mask_shutdown();
	
	project_palette_shutdown();
	map_palette_shutdown();
	model_palette_shutdown();

	file_palette_shutdown();
	list_palette_shutdown();
	
	project_tool_palette_shutdown();
	map_tool_palette_shutdown();
	model_tool_palette_shutdown();
	
	text_shutdown();
}

/* =======================================================

      Menus
      
======================================================= */

void main_wind_menu_create(void)
{
	switch (state.mode) {

		case app_mode_project:
			project_menu_create();
			project_menu_update();
			break;
			
		case app_mode_map:
			map_menu_create();
			map_menu_update();
			break;
			
		case app_mode_model:
			model_menu_create();
			model_menu_update();
			break;

	}
}

void main_wind_menu_dispose(void)
{
	switch (state.mode) {
	
		case app_mode_project:
			project_menu_dispose();
			break;

		case app_mode_map:
			map_menu_dispose();
			break;
			
		case app_mode_model:
			model_menu_dispose();
			break;

	}
}

void main_wind_menu_update(void)
{
	switch (state.mode) {

		case app_mode_project:
			project_menu_update();
			break;
			
		case app_mode_map:
			map_menu_update();
			break;
			
		case app_mode_model:
			model_menu_update();
			break;

	}
}

bool main_wind_menu_event_run(int cmd)
{
	switch (state.mode) {

		case app_mode_project:
			return(project_menu_event_run(cmd));
			
		case app_mode_map:
			return(map_menu_event_run(cmd));
			
		case app_mode_model:
			return(model_menu_event_run(cmd));

	}
	
	return(FALSE);
}

/* =======================================================

      Window Title
      
======================================================= */

void main_wind_set_title(void)
{
	char			title[256];

	title[0]=0x0;

	switch (state.mode) {

		case app_mode_map:
			sprintf(title,"dim3Editor - %s - %s",iface.project.name,state.map.map_file_name);
			break;
			
		case app_mode_model:
			sprintf(title,"dim3Editor - %s - %s",iface.project.name,state.model.model_file_name);
			break;

	}

	if (title[0]==0x0) sprintf(title,"dim3Editor - %s",iface.project.name);

	os_set_title_window(title);
}

/* =======================================================

      Change Mode
      
======================================================= */

bool main_wind_switch_mode(int mode)
{
	char			err_str[256];
	
	if (state.mode==mode) return(TRUE);

		// if old mode was
		// model, turn off animation timer

	if (state.mode==app_mode_model) os_end_timer();

		// save anything we are
		// moving away from

	switch (state.mode) {
	
		case app_mode_project:
			iface_write(&iface,err_str);
			break;

		case app_mode_map:
			if (!file_close_map()) return(FALSE);
			break;

		case app_mode_model:
			if (!model_file_close()) return(FALSE);
			break;
	
	}

		// turn on animation timer for
		// model mode

	if (mode==app_mode_model) os_start_timer();

		// need to rebuild menu
		
	main_wind_menu_dispose();
	state.mode=mode;
	main_wind_menu_create();

	return(TRUE);
}

/* =======================================================

      GL Setup
      
======================================================= */

void main_wind_gl_setup(void)
{
		// setup

	glClearColor(1.0f,1.0f,1.0f,0.0f);

	glEnable(GL_SMOOTH);
	glDisable(GL_DITHER);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	
	glHint(GL_TEXTURE_COMPRESSION_HINT,GL_NICEST);
	glHint(GL_GENERATE_MIPMAP_HINT,GL_NICEST);

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	
	glEnable(GL_DEPTH_TEST);

 	glEnableClientState(GL_VERTEX_ARRAY);
}

/* =======================================================

      Timers
      
======================================================= */

void main_wind_timer(void)
{
	if (state.mode!=app_mode_model) return;
	if (!state.model.model_open) return;
	if (state.model.play_mode==model_play_mode_stop) return;
	
	main_wind_draw();
}

/* =======================================================

      Drawing
      
======================================================= */

void main_wind_clear_viewport(void)
{
	int				wid,high;
	d3rect			wbox;

	os_get_window_box(&wbox);

	wid=wbox.rx-wbox.lx;
	high=wbox.by-wbox.ty;

	glScissor(0,0,wid,high);
	glViewport(0,0,wid,high);
}

void main_wind_draw_project(void)
{
	project_tool_palette_set_state();
	tool_palette_draw(&project_tool_palette);
	project_palette_draw();
}

void main_wind_draw_map(void)
{
	if (state.map.map_open) {
		if (state.map.texture_edit_idx!=-1) {
			texture_edit_draw();
		}
		else {
			if (state.map.auto_generate_on) {
				map_view_auto_generate_draw();
			}
			else {
				map_view_draw();
			}
		}
	}
	
	map_tool_palette_set_state();
	tool_palette_draw(&map_tool_palette);

	if (!state.map.auto_generate_on) texture_palette_draw();
	
	map_palette_draw();
}

void main_wind_draw_model(void)
{
	if (state.model.texture_edit_idx!=-1) {
		texture_edit_draw();
	}
	else {
		model_view_draw();
	}

	model_tool_palette_set_state();
	tool_palette_draw(&model_tool_palette);

	texture_palette_draw();

	model_palette_draw();
}

void main_wind_draw_no_swap(void)
{
		// clear draw buffer

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	main_wind_clear_viewport();

	glClearColor(0.75f,0.75f,0.75f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// draw window

	switch (state.mode) {
		case app_mode_project:
			main_wind_draw_project();
			break;
		case app_mode_map:
			main_wind_draw_map();
			break;
		case app_mode_model:
			main_wind_draw_model();
			break;
	}

		// all views have file palette
		// and tool tips

	file_palette_draw();
	tool_tip_draw();
}

void main_wind_draw(void)
{
	main_wind_draw_no_swap();
	os_swap_gl_buffer();
}

/* =======================================================

      Clicking
      
======================================================= */

void main_wind_click_project(d3pnt *pnt,bool double_click)
{
	int					idx;
	d3rect				tbox;

		// tool palette

	tool_palette_box(&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		project_tool_palette_set_state();
		idx=tool_palette_click(&project_tool_palette,pnt);
		if (idx!=-1) project_tool_palette_click(idx);
		return;
	}

		// property palette
		
	list_palette_total_box(&project_palette,&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		project_palette_click(pnt,double_click);
		return;
	}
}

void main_wind_click_map(d3pnt *pnt,bool double_click)
{
	int					idx;
	d3rect				tbox;
	
		// tool palette

	tool_palette_box(&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		map_tool_palette_set_state();
		idx=tool_palette_click(&map_tool_palette,pnt);
		if (idx!=-1) map_tool_palette_click(idx);
		return;
	}

		// texture palette
		
	if (!state.map.auto_generate_on) {
		map_texture_palette_box(&tbox);

		if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
			texture_palette_click(pnt,double_click);
			return;
		}
	}

		// property palette
		
	list_palette_total_box(&map_palette,&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		map_palette_click(pnt,double_click);
		return;
	}

		// view clicks

	if (!state.map.map_open) return;

	if (state.map.texture_edit_idx!=-1) {
		texture_edit_click(pnt,double_click);
	}
	else {
		if (state.map.auto_generate_on) {
			map_view_auto_generate_click(pnt,double_click);
		}
		else {
			map_view_click(pnt,double_click);
		}
	}
}

void main_wind_click_model(d3pnt *pnt,bool double_click)
{
	int					idx,old_play_mode;
	d3rect				tbox;

		// tool palette

	tool_palette_box(&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		model_tool_palette_set_state();
		idx=tool_palette_click(&model_tool_palette,pnt);
		if (idx!=-1) model_tool_palette_click(idx);
		return;
	}

		// texture palette
		
	model_texture_palette_box(&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		texture_palette_click(pnt,double_click);
		return;
	}

		// property palette
		
	list_palette_total_box(&model_palette,&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		model_palette_click(pnt,double_click);
		return;
	}

		// model clicks
		// turn off animation as it glitches
		// up the win32 timers

	if (!state.model.model_open) return;

	old_play_mode=state.model.play_mode;
	state.model.play_mode=model_play_mode_stop;

	if (state.model.texture_edit_idx!=-1) {
		texture_edit_click(pnt,double_click);
	}
	else {
		model_wind_click(pnt,double_click);
	}

	state.model.play_mode=old_play_mode;
}

void main_wind_click(d3pnt *pnt,bool double_click)
{
	d3rect				tbox;

		// file palettes
	
	list_palette_total_box(&file_palette,&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		file_palette_click(pnt,double_click);
		return;
	}

		// other clicks

	switch (state.mode) {
		case app_mode_project:
			main_wind_click_project(pnt,double_click);
			break;
		case app_mode_map:
			main_wind_click_map(pnt,double_click);
			break;
		case app_mode_model:
			main_wind_click_model(pnt,double_click);
			break;
	}
}

/* =======================================================

      Scroll Wheel
      
======================================================= */

void main_wind_scroll_wheel_project(d3pnt *pnt,int delta)
{
	d3rect				tbox;

		// scroll wheel project palette

	list_palette_total_box(&project_palette,&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		project_palette_scroll_wheel(pnt,delta);
		return;
	}
}

void main_wind_scroll_wheel_map(d3pnt *pnt,int delta)
{
	d3rect				tbox;

		// scroll wheel map palette

	list_palette_total_box(&map_palette,&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		map_palette_scroll_wheel(pnt,delta);
		return;
	}

		// scroll wheel in view

	if (state.map.texture_edit_idx!=-1) {
		texture_edit_scroll_wheel(delta);
	}
	else {
		if (state.map.auto_generate_on) {
			map_view_auto_generate_scroll_wheel(pnt,delta);
		}
		else {
			map_view_scroll_wheel(pnt,delta);
		}
	}
}

void main_wind_scroll_wheel_model(d3pnt *pnt,int delta)
{
	d3rect				tbox;

		// scroll wheel model palette

	list_palette_total_box(&model_palette,&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		model_palette_scroll_wheel(pnt,delta);
		return;
	}

		// scroll wheel in model

	if (state.model.texture_edit_idx!=-1) {
		texture_edit_scroll_wheel(delta);
	}
	else {
		state.model.magnify_z+=(delta*20);
		main_wind_draw();
	}
}

void main_wind_scroll_wheel(d3pnt *pnt,int delta)
{
	d3rect				tbox;
	
		// scroll wheel in file palette

	list_palette_total_box(&file_palette,&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		file_palette_scroll_wheel(pnt,delta);
		return;
	}

		// other movements

	switch (state.mode) {
		case app_mode_project:
			main_wind_scroll_wheel_project(pnt,delta);
			break;
		case app_mode_map:
			main_wind_scroll_wheel_map(pnt,delta);
			break;
		case app_mode_model:
			main_wind_scroll_wheel_model(pnt,delta);
			break;
	}
}

/* =======================================================

      Mouse Movement
      
======================================================= */

void main_wind_mouse_move(d3pnt *pnt)
{
	switch (state.mode) {

		case app_mode_project:
			tool_palette_mouse_move(&project_tool_palette,pnt);
			break;

		case app_mode_map:
			tool_palette_mouse_move(&map_tool_palette,pnt);
			break;

		case app_mode_model:
			tool_palette_mouse_move(&model_tool_palette,pnt);
			break;
	}
}

/* =======================================================

      Cursors
      
======================================================= */

bool main_wind_cursor_map(void)
{
	d3pnt			pnt;
	
	os_get_cursor(&pnt);

	if (state.map.texture_edit_idx!=-1) return(texture_edit_cursor());
	if (state.map.auto_generate_on) return(map_view_auto_generate_cursor(&pnt));
	return(map_view_cursor(&pnt));
}

bool main_wind_cursor_model(void)
{
		// texture editing

	if (state.model.texture_edit_idx==-1) return(texture_edit_cursor());

		// model cursors

	if (os_key_space_down()) {
		os_set_hand_cursor();
		return(TRUE);
	}
	if (os_key_command_down()) {
		os_set_hand_cursor();
		return(TRUE);
	}
	if (os_key_option_down()) {
		os_set_resize_cursor();
		return(TRUE);
	}
	if (os_key_shift_down()) {
		os_set_add_cursor();
		return(TRUE);
	}
	if (os_key_control_down()) {
		os_set_subtract_cursor();
		return(TRUE);
	}
	
	os_set_arrow_cursor();
	return(TRUE);
}

bool main_wind_cursor(void)
{
	switch (state.mode) {

		case app_mode_project:
			return(FALSE);

		case app_mode_map:
			return(main_wind_cursor_map());

		case app_mode_model:
			return(main_wind_cursor_model());

	}

	return(FALSE);
}

/* =======================================================

      Keys
      
======================================================= */

void main_wind_key_map(char ch)
{
	editor_view_type		*view;
	
		// special movement keys
		
	if ((ch=='w') || (ch=='W')) {
		view=view_get_current_view();
		view_key_forward_movement(view,-1);
		return;
	}

	if ((ch=='s') || (ch=='S')) {
		view=view_get_current_view();
		view_key_forward_movement(view,1);
		return;
	}
	
	if ((ch=='a') || (ch=='A')) {
		view=view_get_current_view();
		view_key_side_movement(view,-1);
		return;
	}

	if ((ch=='d') || (ch=='D')) {
		view=view_get_current_view();
		view_key_side_movement(view,1);
		return;
	}

	if ((ch=='q') || (ch=='Q')) {
		view=view_get_current_view();
		view_key_vert_movement(view,-1);
		return;
	}
	
	if ((ch=='e') || (ch=='E')) {
		view=view_get_current_view();
		view_key_vert_movement(view,1);
		return;
	}
	
		// panel keys
		
	view_key(ch);
}

void main_wind_key_model(char ch)
{
		// esc key deselects
		
	if (ch==D3_KEY_ESC) {
		model_vertex_mask_clear_sel(state.model.cur_mesh_idx);
		main_wind_draw();
		return;
	}

		// check for deletes
		// on selected item tree

	if ((ch==D3_KEY_BACKSPACE) || (ch==D3_KEY_DELETE)) {
		if (model_palette_delete()) {
			main_wind_draw();
			return;
		}
	}
}

void main_wind_key(char ch)
{
	switch (state.mode) {

		case app_mode_map:
			main_wind_key_map(ch);
			break;

		case app_mode_model:
			main_wind_key_model(ch);
			break;

	}
}

/* =======================================================

      Resize Setup
      
======================================================= */

void main_wind_resize(void)
{
}














