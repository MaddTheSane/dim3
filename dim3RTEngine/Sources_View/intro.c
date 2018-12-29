/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Intro

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

// intro elements

#define intro_button_game_new_id				0
#define intro_button_game_load_id				1
#define intro_button_game_setup_id				2

#define intro_button_multiplayer_host_id		10
#define intro_button_multiplayer_join_id		11

#define intro_button_credit_id					20
#define intro_button_quit_id					21

extern app_type				app;
extern server_type			server;
extern map_type				map;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

int							intro_simple_save_idx;
bool						intro_esc_down;
bitmap_type					intro_bitmap;

bool test=FALSE;

/* =======================================================

      Intro UI Utilities
      
======================================================= */

void intro_open_add_button(iface_intro_button_type *btn,char *name,int id)
{
	int			x,y;
	char		sel_name[256],path[1024],path2[1024];

	if (!btn->on) return;

		// check for overrides

	x=btn->x;
	y=btn->y;

#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)

	if (btn->mobile_hide) return;

	if (btn->mobile_x!=-1) {
		x=btn->mobile_x;
		y=btn->mobile_y;
	}

#endif

		// get name and position

	sprintf(sel_name,"%s_selected",name);

	file_paths_data(&file_path_setup,path,"Bitmaps/UI_Elements",name,"png");
	file_paths_data(&file_path_setup,path2,"Bitmaps/UI_Elements",sel_name,"png");
	element_button_bitmap_add(path,path2,id,x,y,btn->wid,btn->high,element_pos_left,element_pos_top);
	
	btn->element_id=id;
}

/* =======================================================

      Intro Score Formats
      
======================================================= */

void intro_format_score(int score,char *str)
{
	int			n,k,comma_add,cnt,len,
				min,sec;
	char		str2[256];
	
	if (iface.intro.score.format==intro_score_format_number) {
		sprintf(str2,"%d",score);
		
		k=0;
		
		len=strlen(str2);
		comma_add=len/3;
		if ((len%3)==0) comma_add--;
		
		if (comma_add<=0) {
			strcpy(str,str2);
			return;
		}
		
		cnt=0;
		k=len+comma_add;
		
		str[k--]=0x0;
		
		for (n=(len-1);n>=0;n--) {
			str[k--]=str2[n];
			cnt++;
			if ((cnt==3) && (k>0)) {
				str[k--]=',';
				cnt=0;
			}
		}
	
		return;
	}
	
	if (iface.intro.score.format==intro_score_format_time) {
	
		min=score/(1000*60);
		sec=(score%(1000*60))/1000;
		
		sprintf(str,"%d:%.2d",min,sec);

		return;
	}
	
	sprintf(str,"%d",score);
}

/* =======================================================

      Intro Open and Close
      
======================================================= */

void intro_open(void)
{
	int								n,x,y,yadd,wid,num_wid;
	bool							start_music;
	char							str[256],err_str[256];
	iface_intro_model_type			*intro_model;
	iface_score_type				*score;

		// intro UI
		
	gui_initialize("Bitmaps/Backgrounds","main");

		// models

	intro_model=iface.intro.model_list.models;

	for (n=0;n!=iface.intro.model_list.nmodel;n++) {
		element_model_add(intro_model->model_name,intro_model->animate_name,intro_model->resize,NULL,&intro_model->rot,-1,intro_model->x,intro_model->y);
		intro_model++;
	}

		// buttons
		
	intro_open_add_button(&iface.intro.button_game_new,"button_game_new",intro_button_game_new_id);
	intro_open_add_button(&iface.intro.button_game_load,"button_game_load",intro_button_game_load_id);
	intro_open_add_button(&iface.intro.button_game_setup,"button_game_setup",intro_button_game_setup_id);
	intro_open_add_button(&iface.intro.button_multiplayer_host,"button_multiplayer_host",intro_button_multiplayer_host_id);
	intro_open_add_button(&iface.intro.button_multiplayer_join,"button_multiplayer_join",intro_button_multiplayer_join_id);
	intro_open_add_button(&iface.intro.button_credit,"button_credit",intro_button_credit_id);
	intro_open_add_button(&iface.intro.button_quit,"button_quit",intro_button_quit_id);
		
		// high scores

	if (iface.intro.score.on) {
	
		score_xml_read(&iface);
		
		x=iface.intro.score.x;
		y=iface.intro.score.y;

		wid=iface.intro.score.wid;
		yadd=iface.intro.score.high/max_score_count;
		
		num_wid=iface.intro.score.text_size*3;

		score=iface.score_list.scores;
		
		for (n=0;n!=max_score_count;n++) {
		
			sprintf(str,"%d.",(n+1));
			element_text_add(str,-1,x,y,iface.intro.score.text_size,tx_right,&iface.intro.score.col,FALSE);
			
			element_text_add(score->name,-1,(x+num_wid),y,iface.intro.score.text_size,tx_left,&iface.intro.score.col,FALSE);

			intro_format_score(score->score,str);
			element_text_add(str,-1,(x+wid),y,iface.intro.score.text_size,tx_right,&iface.intro.score.col,FALSE);

			y+=yadd;
			score++;
		}
	}
	
		// default to first simple save

	intro_simple_save_idx=0;

		// in intro state
	
	intro_esc_down=FALSE;
	
		// intro music
		
	if ((iface.intro.music[0]!=0x0) && (setup.music_on)) {
	
			// previously playing music?
			
		start_music=TRUE;
		
		if (al_music_playing()) {
			if (al_music_playing_is_name(iface.intro.music)) {		// same music, keep playing
				start_music=FALSE;
			}
			else {
				start_music=TRUE;
				al_music_stop();
			}
		}
		
		if (start_music) {
			al_music_set_loop(TRUE);
			al_music_play(iface.intro.music,err_str);
		}
	}
}

void intro_close(void)
{
		// stop music
		
	al_music_stop();
	
		// shutdown UI
		
	gui_shutdown();
}

/* =======================================================

      Start a Game
      
======================================================= */

void intro_start_game(int skill,int option_flags,char *map_name,int simple_save_idx)
{
	char			err_str[256];

	al_music_stop();
	
		// goto running state
		
	server.next_state=gs_running;

		// not in networking

	net_setup.mode=net_mode_none;

		// start game

	if (!game_start(FALSE,skill,option_flags,simple_save_idx,err_str)) {
		error_setup(err_str,"Game Start Canceled");
		server.next_state=gs_error;
		return;
	}
	
		// check for map override
		
	if (map_name!=NULL) {
		strcpy(map.info.name,map_name);
	}
	
		// open map

	if (!map_start(FALSE,FALSE,err_str)) {
		error_setup(err_str,"Game Start Canceled");
		server.next_state=gs_error;
		return;
	}
}

/* =======================================================

      Intro Actions
      
======================================================= */

bool intro_click_has_singleplayer_options(int simple_save_idx)
{
	if (iface.singleplayer.skill) return(TRUE);
	if (singleplayer_map_pick_on(simple_save_idx)) return(TRUE);
	return(singleplayer_option_count(simple_save_idx)!=0);
}

void intro_click_game(int simple_save_idx)
{
		// if we have simple save
		// on, then jump to the simple save
		// interface

	if (iface.project.simple_save) {
		server.next_state=gs_simple_save_pick;
		return;
	}

		// if we have any options, then
		// go to options interface

	if (intro_click_has_singleplayer_options(simple_save_idx)) {
		intro_simple_save_idx=simple_save_idx;
		server.next_state=gs_singleplayer_option;
		return;
	}

		// otherwise, just start the
		// game

	intro_start_game(skill_medium,0,NULL,simple_save_idx);
}

void intro_click_load(void)
{
	net_setup.mode=net_mode_none;

	file_setup(FALSE);
	server.next_state=gs_file;
}

/* =======================================================

      Intro Input
      
======================================================= */

void intro_click(void)
{
	int				id;
	char			err_str[256];
	
		// element being clicked?
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
		// regular button clicks
		
	switch (id) {
	
		case intro_button_game_new_id:
			intro_click_game(0);
			break;

		case intro_button_game_load_id:
			intro_click_load();
			break;

		case intro_button_game_setup_id:
			server.next_state=gs_setup_game;
			break;
			
			// multiplayer buttons

		case intro_button_multiplayer_host_id:
			server.next_state=gs_host;
			break;

		case intro_button_multiplayer_join_id:
			server.next_state=gs_join;
			break;

			// credit and quit
			
		case intro_button_credit_id:
			title_setup(gs_intro,"Bitmaps/Backgrounds","credit",NULL,-1,-1,err_str);
			break;

		case intro_button_quit_id:
			intro_close();
			app.loop_quit=TRUE;
			break;
	}
}

void intro_key(void)
{
		// check for esc
		
	if (!input_get_keyboard_escape()) {
		intro_esc_down=FALSE;
		return;
	}
	
	if (intro_esc_down) return;
	
	hud_click();
	
	intro_esc_down=TRUE;

		// escape quits game
		
	intro_close();
	app.loop_quit=TRUE;
}

/* =======================================================

      Run Intro Page
      
======================================================= */

void intro_run(void)
{
	gui_draw(1.0f,TRUE);
	intro_click();
	intro_key();
}


