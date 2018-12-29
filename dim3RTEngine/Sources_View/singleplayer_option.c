/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Singleplayer Option

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

#define singleplayer_option_frame_id			0

#define singleplayer_option_skill_id			10
#define singleplayer_option_map_table_id		11

#define singleplayer_option_button_ok_id		20
#define singleplayer_option_button_cancel_id	21

#define singleplayer_option_option_start_id		100

extern int					intro_simple_save_idx;

extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

char						skill_list[6][32]={"Easy","Medium","Hard",""};
char						*singleplayer_option_table_map_list;

/* =======================================================

      Singleplayer Option Map Lists
      
======================================================= */

void singleplayer_option_map_list_initialize(void)
{
	singleplayer_option_table_map_list=NULL;
}

void singleplayer_option_map_list_fill(void)
{
	int							n,nfile,sz;
	char						*c;
	char						info_name[name_str_len],game_list[256];
	bool						singleplayer_map_picker;
	file_path_directory_type	*map_pick_fpd;

		// load in all maps

	map_pick_fpd=file_paths_read_directory_data(&file_path_setup,"Maps","xml");

	nfile=map_pick_fpd->nfile;
	if (nfile<=0) {
		file_paths_close_directory(map_pick_fpd);
		return;
	}

		// data for maps

	sz=(nfile+1)*128;
	singleplayer_option_table_map_list=malloc(sz);
	bzero(singleplayer_option_table_map_list,sz);

		// load the maps

	c=singleplayer_option_table_map_list;
	
	for (n=0;n!=nfile;n++) {
		if (!map_host_load_info(map_pick_fpd->files[n].file_name,info_name,&singleplayer_map_picker,game_list)) continue;
		if (!singleplayer_map_picker) continue;

		sprintf(c,"Bitmaps/Icons_Map;%s;%s",map_pick_fpd->files[n].file_name,info_name);
		c+=128;
	}

	*c=0x0;

	element_set_table_data(singleplayer_option_map_table_id,TRUE,singleplayer_option_table_map_list);

		// close the directory scan

	file_paths_close_directory(map_pick_fpd);
}

void singleplayer_option_map_list_release(void)
{
	if (singleplayer_option_table_map_list!=NULL) free(singleplayer_option_table_map_list);
}

void singleplayer_option_map_list_get_name(int idx,char *name)
{
	char			*c;
	char			row_str[128],str[128];

	name[0]=0x0;

		// get the name from between the ;; markers
		
	element_get_table_row(singleplayer_option_map_table_id,idx,row_str);
	c=row_str;
	
	c=strchr(c,';');
	if (c!=NULL) {
		strncpy(str,(c+1),128);
		str[127]=0x0;

		c=strchr(str,';');
		if (c!=NULL) *c=0x0;

		strncpy(name,str,name_str_len);
		name[name_str_len-1]=0x0;
	}
}

/* =======================================================

      Determine if Options are On
      
======================================================= */

bool singleplayer_map_pick_on(int simple_save_idx)
{
	if (!iface.singleplayer.map_pick) return(FALSE);
	if (!iface.project.simple_save) return(TRUE);

	if (iface.singleplayer.map_pick_ss_reveal_id==-1) return(TRUE);
	return(iface.singleplayer.map_pick_ss_reveal_id<=iface.simple_save_list.saves[simple_save_idx].save_id);
}

bool singleplayer_option_on(int simple_save_idx,int idx)
{
	if (iface.singleplayer.option_list.options[idx].ss_reveal_id==-1) return(TRUE);
	return(iface.singleplayer.option_list.options[idx].ss_reveal_id<=iface.simple_save_list.saves[simple_save_idx].save_id);
}

int singleplayer_option_count(int simple_save_idx)
{
	int				n,count;

	count=0;

	for (n=0;n!=iface.singleplayer.option_list.noption;n++) {
		if (singleplayer_option_on(simple_save_idx,n)) count++;
	}

	return(count);
}

/* =======================================================

      Map Selected
      
======================================================= */

void singleplayer_option_map_selected(void)
{
	int				idx;

	if (!singleplayer_map_pick_on(intro_simple_save_idx)) return;
	
	idx=element_get_value(singleplayer_option_map_table_id);
	element_enable(singleplayer_option_button_ok_id,(idx!=-1));
}

/* =======================================================

      Singleplayer Option Open and Close
      
======================================================= */

void singleplayer_option_open(void)
{
	int							n,fx,fy,bx,by,wid,high,title_high,table_high,
								margin,padding,control_y_add;
	iface_sp_option_type		*sp_option;
	element_frame_button_type	butts[2]={{singleplayer_option_button_cancel_id,"Cancel",TRUE},{singleplayer_option_button_ok_id,"Play",TRUE}};
	element_column_type			cols[1];

	singleplayer_option_map_list_initialize();

		// intro UI
		
	gui_initialize("Bitmaps/Backgrounds","main");

		// get height

	margin=element_get_margin();
	padding=element_get_padding();
	control_y_add=element_get_control_separation_high();
	
	title_high=element_get_frame_title_high();

	if (singleplayer_map_pick_on(intro_simple_save_idx)) {
		high=iface.scale_y-(margin*2);
	}
	else {
		high=(padding*4);
		if (iface.singleplayer.skill) high+=control_y_add;
		high+=(control_y_add*singleplayer_option_count(intro_simple_save_idx));
		high+=element_get_button_high()+element_get_frame_title_high();
	}
	
		// dialog and frame

	fx=margin;
	fy=(iface.scale_y-high)/2;
	wid=iface.scale_x-(margin*2);
	
	element_frame_add("New Game",singleplayer_option_frame_id,fx,fy,wid,high,-1,0,NULL,2,butts);

		// controls

	element_get_frame_inner_space(singleplayer_option_frame_id,&fx,&fy,&wid,&high);

	bx=fx+(wid/3);
	by=fy;

	if (iface.singleplayer.skill) {
		element_combo_add("Skill",(char*)skill_list,skill_medium,singleplayer_option_skill_id,bx,(by+control_y_add),TRUE);
		by+=control_y_add;
	}

	sp_option=iface.singleplayer.option_list.options;

	for (n=0;n!=iface.singleplayer.option_list.noption;n++) {
		if (singleplayer_option_on(intro_simple_save_idx,n)) {
			element_checkbox_add(sp_option->descript,FALSE,(singleplayer_option_option_start_id+n),bx,(by+control_y_add),TRUE);
			by+=control_y_add;
		}
		sp_option++;
	}

	if ((iface.singleplayer.skill) || (iface.singleplayer.option_list.noption!=0)) by+=padding;

		// map pick

	if (singleplayer_map_pick_on(intro_simple_save_idx)) {
		strcpy(cols[0].name,"Map");
		cols[0].percent_size=1.0f;
		
		table_high=high-(by-fy);

		element_table_add(cols,NULL,singleplayer_option_map_table_id,1,fx,by,wid,table_high,FALSE,element_table_bitmap_data);
	
		singleplayer_option_map_list_fill();
		
		element_set_value(singleplayer_option_map_table_id,0);
	}
	
	singleplayer_option_map_selected();
}

void singleplayer_option_close(void)
{
		// release lists

	singleplayer_option_map_list_release();

		// shutdown UI
		
	gui_shutdown();
}

/* =======================================================

      Singleplayer Option Input
      
======================================================= */

void singleplayer_option_click(void)
{
	int				n,id,idx,skill,option_flags;
	char			map_name[name_str_len];
	
	id=-1;

		// keyboard

	if (input_get_keyboard_escape()) id=singleplayer_option_button_cancel_id;
	if (input_get_keyboard_return()) id=singleplayer_option_button_ok_id;

		// clicking

	if (id==-1) {
		id=gui_click();
		if (id!=-1) hud_click();
	}
		
	if (id==-1) return;

		// handle click
		
	switch (id) {
	
		case singleplayer_option_button_ok_id:

				// get setup

			skill=skill_medium;
			if (iface.singleplayer.skill) skill=element_get_value(singleplayer_option_skill_id);
			
			option_flags=0x0;
			for (n=0;n!=iface.singleplayer.option_list.noption;n++) {
				if (element_get_value(singleplayer_option_option_start_id+n)!=0) option_flags|=(0x1<<n);
			}
			
				// picked map?
				
			map_name[0]=0x0;
			
			if (singleplayer_map_pick_on(intro_simple_save_idx)) {
				idx=element_get_value(singleplayer_option_map_table_id);
				if (idx!=-1) singleplayer_option_map_list_get_name(idx,map_name);
			}

				// start game

			if (map_name[0]==0x0) {
				intro_start_game(skill,option_flags,NULL,intro_simple_save_idx);
			}
			else {
				intro_start_game(skill,option_flags,map_name,intro_simple_save_idx);
			}
			
			break;

		case singleplayer_option_button_cancel_id:
			server.next_state=gs_intro;
			break;

		case singleplayer_option_map_table_id:
			singleplayer_option_map_selected();
			break;

	}
}

/* =======================================================

      Run Singleplayer Option Page
      
======================================================= */

void singleplayer_option_run(void)
{
	gui_draw(1.0f,TRUE);
	singleplayer_option_click();
}


