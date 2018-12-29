/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Setup

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

#define setup_pane_video					0
#define setup_pane_audio					1
#define setup_pane_control					2
#define setup_pane_action					3
#define setup_pane_player					4
#define setup_pane_debug					5

#define ctrl_screen_gl_size_id				0
#define ctrl_fsaa_id						1
#define ctrl_decal_on_id					2
#define ctrl_shadow_on_id					3
#define ctrl_gamma_id						4

#define ctrl_sound_volume_id				30
#define ctrl_music_on_id					31
#define ctrl_music_volume_id				32

#define ctrl_always_run_id					40
#define ctrl_toggle_run_id					41
#define ctrl_invert_look_id					42
#define ctrl_auto_aim_id					43
#define ctrl_mouse_speed_id					44
#define ctrl_mouse_accel_id					45
#define ctrl_mouse_smooth_id				46
#define ctrl_joystick_speed_id				47
#define ctrl_joystick_accel_id				48

#define ctrl_action_id						50

#define ctrl_network_name_id				60
#define ctrl_network_show_names_id			61
#define ctrl_character_id					62
#define ctrl_character_model_id				63

#define ctrl_debug_engine_windowed_id		70
#define ctrl_debug_editor_windowed_id		71
#define ctrl_debug_no_hud_id				72
#define ctrl_debug_no_draw_weapon_id		73
#define ctrl_debug_metrics_on_id			74
#define ctrl_debug_debug_on_id				75
#define ctrl_debug_ignore_fps_lock_id		76

#define setup_game_frame_id					80
#define setup_game_tab_id					81

#define setup_action_set_button_id			90
#define setup_action_clear_button_id		91

#define setup_game_ok_button_id				100
#define setup_game_cancel_button_id			101
#define setup_game_default_button_id		102

extern bool view_reset_display(char *err_str);
extern int setup_find_action_in_setup(int action_idx);
extern bool setup_xml_reset(void);

extern app_type				app;
extern render_info_type		render_info;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;

int							setup_tab_value,setup_action_scroll_pos,
							setup_tab_index[6],
							setup_key_control_to_action_index_list[ncontrol];

char						setup_tab_list[6][name_str_len],
							setup_screen_size_list[max_screen_size+1][32],
							setup_fsaa_mode_list[][32]=setup_fsaa_mode_list_def,
							setup_control_names[][32]=control_names,
							setup_action_list[ncontrol+1][128],
							setup_character_list[max_character+1][128];
							
bool						setup_action_set_flag,
							setup_in_game,setup_close_save_flag;
setup_type					setup_backup;

/* =======================================================

      Character Table
      
======================================================= */

void setup_network_fill_character_table(void)
{
	int							n;
	char						*c;

	c=(char*)setup_character_list;
	
	for (n=0;n!=iface.multiplayer.character_list.ncharacter;n++) {
		strcpy(c,iface.multiplayer.character_list.characters[n].name);
		c+=128;
	}
	
	*c=0x0;

	element_set_table_data(ctrl_character_id,FALSE,(char*)setup_character_list);
}

/* =======================================================

      Setup Panes
      
======================================================= */

void setup_game_video_pane(void)
{
	int			n,idx,wid,high,
				fx,fy,x,y,control_y_add,control_y_sz;
	bool		no_res_combo;
	
#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)
	no_res_combo=TRUE;
#else
	no_res_combo=(iface.setup.no_resolution_switch) && (!setup.window);
#endif
	
	control_y_add=element_get_control_separation_high();
	control_y_sz=control_y_add*3;
	if (!no_res_combo) control_y_sz+=control_y_add;
	if (gl_check_fsaa_ok()) control_y_sz+=control_y_add;

	element_get_frame_inner_space(setup_game_frame_id,&fx,&fy,&wid,&high);
	x=fx+(int)(((float)wid)*0.4f);
	y=(fy+((high-control_y_sz)/2))+control_y_add;
	
		// setup screen size list
		
	idx=0;
	strcpy(setup_screen_size_list[0],"Default");
		
	for (n=0;n!=render_info.nscreen_size;n++) {
	
		wid=render_info.screen_sizes[n].wid;
		high=render_info.screen_sizes[n].high;
		
		if ((wid==setup.screen_wid) && (high==setup.screen_high)) idx=n+1;
		
		if (gl_is_size_widescreen(wid,high)) {
			sprintf(setup_screen_size_list[n+1],"%dx%d Widescreen",wid,high);
		}
		else {
			sprintf(setup_screen_size_list[n+1],"%dx%d",wid,high);
		}
	}
	
		// build the controls
		
	if (!no_res_combo) {
		element_combo_add("Screen Size",(char*)setup_screen_size_list,idx,ctrl_screen_gl_size_id,x,y,TRUE);
		y+=control_y_add;
	}

	if (gl_check_fsaa_ok()) {
		element_combo_add("Full-Screen Anti-Aliasing",(char*)setup_fsaa_mode_list,setup.fsaa_mode,ctrl_fsaa_id,x,y,TRUE);
		y+=control_y_add;
	}
	
	element_checkbox_add("Decals",setup.decal_on,ctrl_decal_on_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Shadows",setup.shadow_on,ctrl_shadow_on_id,x,y,TRUE);
	y+=control_y_add;

	element_slider_add("Gamma",setup.gamma,-0.5f,0.5f,ctrl_gamma_id,x,y,TRUE);
}

void setup_game_audio_pane(void)
{
	int			fx,fy,wid,high,
				x,y,control_y_add,control_y_sz;
	
	control_y_add=element_get_control_separation_high();
	control_y_sz=control_y_add*2;
	
	element_get_frame_inner_space(setup_game_frame_id,&fx,&fy,&wid,&high);
	x=fx+(int)(((float)wid)*0.4f);
	y=(fy+((high-control_y_sz)/2))+control_y_add;
	
	element_slider_add("Sound Volume",setup.sound_volume,0.0f,1.0f,ctrl_sound_volume_id,x,y,TRUE);
	y+=control_y_add;

	element_checkbox_add("Music",setup.music_on,ctrl_music_on_id,x,y,TRUE);
	y+=control_y_add;
	element_slider_add("Music Volume",setup.music_volume,0.0f,1.0f,ctrl_music_volume_id,x,y,TRUE);
}

bool setup_game_control_pane_visible(void)
{
	if (iface.setup.allow_run) return(TRUE);
	if (iface.setup.allow_auto_aim) return(TRUE);
	if (input_check_joystick_ok()) return(TRUE);
#if !defined(D3_OS_IPHONE) && !defined(D3_OS_ANDRIOD)
	return(TRUE);
#else
	return(FALSE);
#endif
}

void setup_game_control_pane(void)
{
	int			fx,fy,wid,high,
				x,y,control_y_add,control_y_sz;
	
		// get size of controls

	control_y_add=element_get_control_separation_high();
	control_y_sz=0;
	if (iface.setup.allow_run) control_y_sz+=(control_y_add*2);
	if (iface.setup.allow_auto_aim) control_y_sz+=control_y_add;
#if !defined(D3_OS_IPHONE) && !defined(D3_OS_ANDRIOD)
	control_y_sz+=(control_y_add*4);
#endif
	if (input_check_joystick_ok()) control_y_sz+=(control_y_add*2);

	element_get_frame_inner_space(setup_game_frame_id,&fx,&fy,&wid,&high);
	x=fx+(int)(((float)wid)*0.4f);
	y=(fy+((high-control_y_sz)/2))+control_y_add;

		// add the controls

	if (iface.setup.allow_run) {
		element_checkbox_add("Always Run",setup.always_run,ctrl_always_run_id,x,y,TRUE);
		y+=control_y_add;
		element_checkbox_add("Toggle Run",setup.toggle_run,ctrl_toggle_run_id,x,y,TRUE);
		y+=control_y_add;
	}

#if !defined(D3_OS_IPHONE) && !defined(D3_OS_ANDRIOD)
	element_checkbox_add("Invert Look",setup.invert_look,ctrl_invert_look_id,x,y,TRUE);
	y+=control_y_add;
#endif
	if (iface.setup.allow_auto_aim) {
		element_checkbox_add("Auto Aim",setup.auto_aim,ctrl_auto_aim_id,x,y,TRUE);
		y+=control_y_add;
	}

#if !defined(D3_OS_IPHONE) && !defined(D3_OS_ANDRIOD)
	element_checkbox_add("Mouse Smoothing",setup.mouse_smooth,ctrl_mouse_smooth_id,x,y,TRUE);
	y+=control_y_add;
	element_slider_add("Mouse Speed",setup.mouse.speed,0.0f,0.1f,ctrl_mouse_speed_id,x,y,TRUE);
	y+=control_y_add;
	element_slider_add("Mouse Acceleration",setup.mouse.acceleration,0.0f,1.0f,ctrl_mouse_accel_id,x,y,TRUE);
	y+=control_y_add;
#endif

	if (input_check_joystick_ok()) {
		element_slider_add("Joystick Speed",setup.joystick.speed,0.0f,0.1f,ctrl_joystick_speed_id,x,y,TRUE);
		y+=control_y_add;
		element_slider_add("Joystick Acceleration",setup.joystick.acceleration,0.0f,1.0f,ctrl_joystick_accel_id,x,y,TRUE);
	}
}

void setup_game_action_pane(void)
{
	int					n,k,list_cnt,idx,
						fx,fy,wid,high,
						x,y,table_high,padding;
	element_column_type	cols[2];
	
	element_get_frame_inner_space(setup_game_frame_id,&fx,&fy,&wid,&high);

	padding=element_get_padding();

	table_high=high-(element_get_button_high()+padding);
	
		// setup action list
		
	list_cnt=0;
	
	for (n=0;n!=ncontrol;n++) {
	
			// is item showing
			
		if (!iface.action_display_list.action_displays[n].show) continue;
		
			// hook action index to control index
			
		setup_key_control_to_action_index_list[list_cnt]=n;
	
			// action
			
		strcpy(setup_action_list[list_cnt],iface.action_display_list.action_displays[n].display_name);
		strcat(setup_action_list[list_cnt],"\t");
		
			// attachments
			
		idx=setup_find_action_in_setup(n);
		if (idx!=-1) {
			for (k=0;k!=max_setup_action_attach;k++) {
				if (setup.action_list.actions[idx].attach[k][0]!=0x0) {
					strcat(setup_action_list[list_cnt],setup.action_list.actions[idx].attach[k]);
					strcat(setup_action_list[list_cnt],"  ");
				}
			}
		}
		
		list_cnt++;
	}
	
	setup_action_list[list_cnt][0]=0x0;

	strcpy(cols[0].name,"Action");
	cols[0].percent_size=0.50f;
	strcpy(cols[1].name,"Keys");
	cols[1].percent_size=0.50f;

	element_table_add(cols,(char*)setup_action_list,ctrl_action_id,2,fx,fy,wid,table_high,FALSE,element_table_bitmap_none);
	
		// action buttons
		
	x=fx+(int)(((float)wid)*0.5f);
	y=(fy+table_high)+padding;
	
	wid=element_get_button_long_wid();
	high=element_get_button_high();
	
	element_button_text_add("Set Action",setup_action_set_button_id,(x-(padding/2)),y,wid,high,element_pos_right,element_pos_top);
	element_enable(setup_action_set_button_id,FALSE);
	
	element_button_text_add("Clear Action",setup_action_clear_button_id,(x+(padding/2)),y,wid,high,element_pos_left,element_pos_top);
	element_enable(setup_action_clear_button_id,FALSE);
}

void setup_game_player_pane(void)
{
	int							fx,fy,x,y,wid,high,padding,
								table_wid,table_high,control_y_add,control_y_sz;
	element_column_type			cols[1];
	iface_mp_character_type		*mp_character;

	padding=element_get_padding();

	control_y_add=element_get_control_separation_high();
	control_y_sz=control_y_add*2;

	element_get_frame_inner_space(setup_game_frame_id,&fx,&fy,&wid,&high);
	
	if (iface.multiplayer.character_list.ncharacter!=0) {
		x=fx+(int)(((float)wid)*0.24f);
		y=(fy+control_y_add)+padding;
	}
	else {
		x=fx+(int)(((float)wid)*0.38f);
		y=(fy+((high-control_y_sz)/2))+control_y_add;
	}
	
		// names and colors
		
	element_text_field_add("Name",setup.network.name,name_str_len,ctrl_network_name_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Show Names",setup.network.show_names,ctrl_network_show_names_id,x,y,TRUE);
	
		// is there a character control?

	if (iface.multiplayer.character_list.ncharacter==0) return;
	
		// character table
		
	y+=padding;

	table_wid=(int)(((float)wid)*0.8f);
	table_high=high-(y-fy);

	strcpy(cols[0].name,"Characters");
	cols[0].percent_size=1.0f;

	element_table_add(cols,NULL,ctrl_character_id,1,fx,y,table_wid,table_high,FALSE,element_table_bitmap_none);

		// fill and select table

	setup_network_fill_character_table();

	element_set_value(ctrl_character_id,setup.network.character_idx);
	element_make_selection_visible(ctrl_character_id);
	
		// character model

	x=fx+(int)(((float)wid)*0.87f);
	y=fy+(int)(((float)high)*0.9f);
	
	mp_character=&iface.multiplayer.character_list.characters[setup.network.character_idx];
	element_model_add(mp_character->model_name,"Idle",mp_character->interface_resize,&mp_character->interface_offset,NULL,ctrl_character_model_id,x,y);
}

void setup_game_debug_pane(void)
{
	int			fx,fy,wid,high,
				x,y,control_y_add,control_y_sz;
	
	control_y_add=element_get_control_separation_high();
	control_y_sz=control_y_add*7;
	
	element_get_frame_inner_space(setup_game_frame_id,&fx,&fy,&wid,&high);
	x=fx+(int)(((float)wid)*0.5f);
	y=(fy+((high-control_y_sz)/2))+control_y_add;
	
	element_checkbox_add("Engine Windowed Mode",setup.window,ctrl_debug_engine_windowed_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Editor Windowed Run Mode",setup.window_editor,ctrl_debug_editor_windowed_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("No HUD",setup.no_hud,ctrl_debug_no_hud_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("No Draw Weapon",setup.no_draw_weapon,ctrl_debug_no_draw_weapon_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Display Metrics",setup.metrics_on,ctrl_debug_metrics_on_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Debug Mode",setup.debug_on,ctrl_debug_debug_on_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Ignore FPS Lock",setup.ignore_fps_lock,ctrl_debug_ignore_fps_lock_id,x,y,TRUE);
}

void setup_game_create_pane(void)
{
	int							fx,fy,wid,high,margin,
								ntab;
	element_frame_button_type	butts[3]={{setup_game_default_button_id,"Default",FALSE},{setup_game_cancel_button_id,"Cancel",TRUE},{setup_game_ok_button_id,"OK",TRUE}};
	
	element_clear();
	
		// the frame

	ntab=0;

	if ((!setup_in_game) && (iface.setup.game_video)) {
		strcpy(setup_tab_list[ntab],"Video");
		setup_tab_index[ntab]=setup_pane_video;
		ntab++;
	}
	if (iface.setup.game_audio) {
		strcpy(setup_tab_list[ntab],"Audio");
		setup_tab_index[ntab]=setup_pane_audio;
		ntab++;
	}
	if ((iface.setup.game_control) && (setup_game_control_pane_visible())) {
		strcpy(setup_tab_list[ntab],"Control");
		setup_tab_index[ntab]=setup_pane_control;
		ntab++;
	}
	if (iface.setup.game_action) {
		strcpy(setup_tab_list[ntab],"Actions");
		setup_tab_index[ntab]=setup_pane_action;
		ntab++;
	}
	if ((iface.setup.game_player) && (iface.multiplayer.on)) {
		strcpy(setup_tab_list[ntab],"Multiplayer");
		setup_tab_index[ntab]=setup_pane_player;
		ntab++;
	}
	if (iface.setup.game_debug) {
		strcpy(setup_tab_list[ntab],"Debug");
		setup_tab_index[ntab]=setup_pane_debug;
		ntab++;
	}

	margin=element_get_margin();

	fx=margin;
	fy=margin;
	wid=iface.scale_x-(margin*2);
	high=iface.scale_y-(margin*2);
	
	element_frame_add("Setup",setup_game_frame_id,fx,fy,wid,high,setup_game_tab_id,ntab,(char*)setup_tab_list,3,butts);
	element_set_value(setup_game_tab_id,setup_tab_value);
	
		// specific pane controls
		
	switch (setup_tab_index[element_get_value(setup_game_tab_id)]) {
		case setup_pane_video:
			setup_game_video_pane();
			break;
		case setup_pane_audio:
			setup_game_audio_pane();
			break;
		case setup_pane_control:
			setup_game_control_pane();
			break;
		case setup_pane_action:
			setup_game_action_pane();
			break;
		case setup_pane_player:
			setup_game_player_pane();
			break;
		case setup_pane_debug:
			setup_game_debug_pane();
			break;
	}
}

/* =======================================================

      Setup Actions
      
======================================================= */

int setup_game_action_find_free_attach(setup_action_type *action)
{
	int					n;
	
	for (n=0;n!=max_setup_action_attach;n++) {
		if (action->attach[n][0]==0x0) return(n);
	}
	
	return(-1);
}
	
void setup_game_action_enable_buttons(void)
{
	int					ctrl_idx,action_idx;
	setup_action_type	*action;
	
	element_enable(setup_action_set_button_id,FALSE);
	element_enable(setup_action_clear_button_id,FALSE);
	
		// get current action
		
	ctrl_idx=element_get_value(ctrl_action_id);
	if (ctrl_idx==-1) return;
	
	action_idx=setup_find_action_in_setup(setup_key_control_to_action_index_list[ctrl_idx]);
	
		// clear enabled if selection
		
	element_enable(setup_action_clear_button_id,TRUE);
	
		// set enabled if empty attachment
		
	action=&setup.action_list.actions[action_idx];
	if (setup_game_action_find_free_attach(action)!=-1) element_enable(setup_action_set_button_id,TRUE);
}

void setup_game_action_clear(void)
{
	int					n,ctrl_idx,action_idx;
	setup_action_type	*action;
	
		// clear attachments
		
	ctrl_idx=element_get_value(ctrl_action_id);
	if (ctrl_idx==-1) return;
	
	action_idx=setup_find_action_in_setup(setup_key_control_to_action_index_list[ctrl_idx]);
	action=&setup.action_list.actions[action_idx];
	
	for (n=0;n!=max_setup_action_attach;n++) {
		action->attach[n][0]=0x0;
	}
	
		// rebuild list
		
	setup_game_create_pane();
	element_set_value(ctrl_action_id,ctrl_idx);
	setup_game_action_enable_buttons();
}

void setup_game_action_wait_for_input(void)
{
	int					n,ctrl_idx,action_idx;
	char				name[32];
	bool				already_set;
	setup_action_type	*action;
	
		// any input?
		
	if (!input_set_key_wait(name)) return;
	
		// set current action
		
	ctrl_idx=element_get_value(ctrl_action_id);
	if (ctrl_idx==-1) return;
	
	action_idx=setup_find_action_in_setup(setup_key_control_to_action_index_list[ctrl_idx]);
	
	if (action_idx==-1) {					// action doesn't exit, so create new one
		action_idx=setup.action_list.naction;
		strcpy(setup.action_list.actions[action_idx].name,setup_control_names[setup_key_control_to_action_index_list[ctrl_idx]]);
		setup.action_list.naction++;
	}
	
	action=&setup.action_list.actions[action_idx];
	
		// already picked?
		
	already_set=FALSE;
	
	for (n=0;n!=max_setup_action_attach;n++) {
		if (strcasecmp(action->attach[n],name)==0) already_set=TRUE;
	}

		// if not add
	
	if (!already_set) {
		n=setup_game_action_find_free_attach(action);
		if (n!=-1) strcpy(action->attach[n],name);
	}
	
		// don't let set key get back to interface
	
	input_clear();
	
		// rebuild list
		
	setup_game_create_pane();
	element_set_value(ctrl_action_id,ctrl_idx);
	element_set_scroll_position(ctrl_action_id,setup_action_scroll_pos);
	setup_game_action_enable_buttons();
	
		// got action
		
	setup_action_set_flag=FALSE;
}

/* =======================================================

      Setup Game Operations
      
======================================================= */

void setup_game_open(void)
{
		// setup gui
		
	gui_initialize("Bitmaps/Backgrounds","main");

		// remember if we came here
		// from within the game

	setup_in_game=(server.last_state==gs_running);
	
		// waiting for an action flag
		
	setup_action_set_flag=FALSE;
	
		// save/restore flag
		
	setup_close_save_flag=FALSE;
	
		// start with first tab
		
	setup_tab_value=0;
	setup_game_create_pane();
	
		// save setup
		
	memmove(&setup_backup,&setup,sizeof(setup_type));
}

void setup_game_close(void)
{
	char			err_str[256];
	bool			display_reset;

	gui_shutdown();
	
		// is it a restore?
		
	if (!setup_close_save_flag) {
		memmove(&setup,&setup_backup,sizeof(setup_type));
		return;
	}
	
		// save

		// fix control and sound changes
		
	setup_to_input();
	
	al_set_volume(setup.sound_volume);
	al_music_set_volume(setup.music_volume);
	al_music_set_state(setup.music_on);
	
		// need to reset SDL?
		// can't do it in game (not possible, anyway, but just in case)
		
	if (!setup_in_game) {
		display_reset=(setup_backup.screen_wid!=setup.screen_wid);
		display_reset=display_reset || (setup_backup.screen_high!=setup.screen_high);
		display_reset=display_reset || (setup_backup.fsaa_mode!=setup.fsaa_mode);
	
		if (display_reset) {
			if (!view_reset_display(err_str)) {
				app.loop_quit=TRUE;			// fatal error resetting display
				return;
			}

		}
	}
	
		// write setup
		
	setup_xml_write();
}

void setup_game_done(void)
{
		// continue game or intro

	if (setup_in_game) {
		server.next_state=gs_running;
	}
	else {
		server.next_state=gs_intro;
	}
}

void setup_game_default(void)
{
	setup_xml_reset();
	setup_game_create_pane();
}

/* =======================================================

      Setup Game Input
      
======================================================= */

void setup_game_click(void)
{
	int							id,idx;
	iface_mp_character_type		*mp_character;

	id=-1;

		// keyboard

	if (input_get_keyboard_escape()) id=setup_game_cancel_button_id;
	if (input_get_keyboard_return()) id=setup_game_ok_button_id;

	if (id==-1) id=gui_keyboard();

		// clicking

	if (id==-1) {
		id=gui_click();
		if (id!=-1) hud_click();
	}
		
	if (id==-1) return;

		// handle click
	
	switch (id) {
	
			// tab
			
		case setup_game_tab_id:
			setup_tab_value=element_get_value(setup_game_tab_id);
			setup_game_create_pane();
			return;
			
			// buttons
			
		case setup_game_ok_button_id:
			setup_close_save_flag=TRUE;
			setup_game_done();
			return;
			
		case setup_game_cancel_button_id:
			setup_close_save_flag=FALSE;
			setup_game_done();
			return;

		case setup_game_default_button_id:
			setup_game_default();
			return;
			
		case setup_action_set_button_id:
			setup_action_scroll_pos=element_get_scroll_position(ctrl_action_id);
			setup_action_set_flag=TRUE;
			input_clear();
			input_set_key_start();
			return;
			
		case setup_action_clear_button_id:
			setup_action_scroll_pos=element_get_scroll_position(ctrl_action_id);
			setup_game_action_clear();
			element_set_scroll_position(ctrl_action_id,setup_action_scroll_pos);
			return;
	
			// video pane
			
		case ctrl_screen_gl_size_id:
			idx=element_get_value(ctrl_screen_gl_size_id);
			if (idx==0) {
				setup.screen_wid=setup.screen_high=-1;
			}
			else {
				setup.screen_wid=render_info.screen_sizes[idx-1].wid;
				setup.screen_high=render_info.screen_sizes[idx-1].high;
			}
			break;
			
		case ctrl_decal_on_id:
			setup.decal_on=element_get_value(ctrl_decal_on_id);
			break;

		case ctrl_shadow_on_id:
			setup.shadow_on=element_get_value(ctrl_shadow_on_id);
			break;
			
		case ctrl_fsaa_id:
			setup.fsaa_mode=element_get_value(ctrl_fsaa_id);
			break;
			
		case ctrl_gamma_id:
			setup.gamma=element_get_slider_value(ctrl_gamma_id);
			break;
			
			// audio pane
			
		case ctrl_sound_volume_id:
			setup.sound_volume=element_get_slider_value(ctrl_sound_volume_id);
			break;

		case ctrl_music_on_id:
			setup.music_on=element_get_value(ctrl_music_on_id);
			break;
			
		case ctrl_music_volume_id:
			setup.music_volume=element_get_slider_value(ctrl_music_volume_id);
			break;
			
			// mouse pane
			
		case ctrl_always_run_id:
			setup.always_run=element_get_value(ctrl_always_run_id);
			break;
			
		case ctrl_toggle_run_id:
			setup.toggle_run=element_get_value(ctrl_toggle_run_id);
			break;
			
		case ctrl_invert_look_id:
			setup.invert_look=element_get_value(ctrl_invert_look_id);
			break;

		case ctrl_auto_aim_id:
			setup.auto_aim=element_get_value(ctrl_auto_aim_id);
			break;
			
		case ctrl_mouse_smooth_id:
			setup.mouse_smooth=element_get_value(ctrl_mouse_smooth_id);
			break;
			
		case ctrl_mouse_speed_id:
			setup.mouse.speed=element_get_slider_value(ctrl_mouse_speed_id);
			break;
			
		case ctrl_mouse_accel_id:
			setup.mouse.acceleration=element_get_slider_value(ctrl_mouse_accel_id);
			break;
			
		case ctrl_joystick_speed_id:
			setup.joystick.speed=element_get_slider_value(ctrl_joystick_speed_id);
			break;

		case ctrl_joystick_accel_id:
			setup.joystick.acceleration=element_get_slider_value(ctrl_joystick_accel_id);
			break;
		
			// action pane
			
		case ctrl_action_id:
			setup_game_action_enable_buttons();
			break;

			// player pane

		case ctrl_network_name_id:
			element_get_value_string(ctrl_network_name_id,setup.network.name);
			break;

		case ctrl_network_show_names_id:
			setup.network.show_names=element_get_value(ctrl_network_show_names_id);
			break;

		case ctrl_character_id:
			setup.network.character_idx=element_get_value(ctrl_character_id);
			mp_character=&iface.multiplayer.character_list.characters[setup.network.character_idx];
			element_replace_model(ctrl_character_model_id,mp_character->model_name,"Idle",mp_character->interface_resize,&mp_character->interface_offset,NULL);
			break;

			// debug pane

		case ctrl_debug_engine_windowed_id:
			setup.window=element_get_value(ctrl_debug_engine_windowed_id);
			break;

		case ctrl_debug_editor_windowed_id:
			setup.window_editor=element_get_value(ctrl_debug_editor_windowed_id);
			break;

		case ctrl_debug_no_hud_id:
			setup.no_hud=element_get_value(ctrl_debug_no_hud_id);
			break;

		case ctrl_debug_no_draw_weapon_id:
			setup.no_draw_weapon=element_get_value(ctrl_debug_no_draw_weapon_id);
			break;

		case ctrl_debug_metrics_on_id:
			setup.metrics_on=element_get_value(ctrl_debug_metrics_on_id);
			break;

		case ctrl_debug_debug_on_id:
			setup.debug_on=element_get_value(ctrl_debug_debug_on_id);
			break;

		case ctrl_debug_ignore_fps_lock_id:
			setup.ignore_fps_lock=element_get_value(ctrl_debug_ignore_fps_lock_id);
			break;

	}
}
	
/* =======================================================

      Run Game Setup
      
======================================================= */

void setup_game_run(void)
{
		// if input wait is on
		
	if (setup_action_set_flag) {
		gui_draw_message("Press a Key or Click a Button");
		setup_game_action_wait_for_input();
		return;
	}
	
		// draw UI
		
	gui_draw(1.0f,TRUE);

	setup_game_click();
}

