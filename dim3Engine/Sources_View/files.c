/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Load/Save Games

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

#define max_file_count					256

#define file_frame_id					0
#define file_info_table_id				1
#define file_info_bitmap_id				2
#define file_info_name_id				3
#define file_info_time_id				4
#define file_info_elapsed_id			5

#define file_button_save_id				10
#define file_button_load_id				11
#define file_button_delete_id			12
#define file_button_cancel_id			13

extern server_type				server;
extern iface_type				iface;
extern setup_type				setup;
extern file_path_setup_type		file_path_setup;

int								file_last_state;
char							*file_table_data;
bool							file_is_save,file_has_list;
			
/* =======================================================

      Build File List Utilities
      
======================================================= */

void file_dir_name_to_name(char *dir_name,char *file_name)
{
	char		*c;
	
	strcpy(file_name,&dir_name[18]);
	
	c=strchr(file_name,'.');
	if (c!=NULL) *c=0x0;
}

void file_dir_name_to_time(char *dir_name,char *time_str)
{
	int			hr;
	char		hour_str[8];
	bool		am;
	
		// turn hour into am-pm
		
	am=TRUE;
	memmove(hour_str,&dir_name[8],2);
	hour_str[2]=0x0;
	
	hr=atoi(hour_str);
	if (hr>=12) {
		am=FALSE;
		if (hr>12) hr-=12;
	}
	
	if (hr==0) hr=12;
	
	sprintf(hour_str,"%.2d",hr);
	
		// time string
	
	memmove(&time_str[0],&dir_name[4],2);
	time_str[2]='/';
	memmove(&time_str[3],&dir_name[6],2);
	time_str[5]='/';
	memmove(&time_str[6],&dir_name[0],4);
	time_str[10]=' ';
	memmove(&time_str[11],hour_str,2);
	time_str[13]=':';
	memmove(&time_str[14],&dir_name[10],2);
	time_str[16]=0x0;
	
	if (am) {
		strcat(time_str," AM");
	}
	else {
		strcat(time_str," PM");
	}
}

void file_dir_name_to_elapsed(char *dir_name,char *elapse_str)
{
	memmove(&elapse_str[0],&dir_name[13],2);
	elapse_str[2]=':';
	memmove(&elapse_str[3],&dir_name[15],2);
	elapse_str[5]=0x0;
}

/* =======================================================

      Build File List
      
======================================================= */

void file_build_list(void)
{
	int							n,k,idx,cnt,sz,sort_idx[max_file_count+1];
	char						name_str[64];
	file_path_directory_type	*fpd;
	
		// data for maximum number of files
		
	sz=(max_file_count+1)*128;
	
	file_table_data=malloc(sz);
	bzero(file_table_data,sz);
	
		// grab the files
		
	fpd=file_paths_read_directory_document(&file_path_setup,"Saved Games","sav");
	if (fpd==NULL) return;

		// sort the times

	cnt=0;
	
	for (n=0;n!=fpd->nfile;n++) {
	
		if (fpd->files[n].is_dir) continue;

			// find position in sort

		idx=cnt;

		for (k=0;k!=cnt;k++) {
			if (strcmp(fpd->files[n].file_name,fpd->files[sort_idx[k]].file_name)>0) {
				idx=k;
				break;
			}
		}

		if (idx==cnt) {
			sort_idx[cnt]=n;
		}
		else {
			sz=cnt-idx;
			if (sz!=0) memmove(&sort_idx[idx+1],&sort_idx[idx],(sizeof(int)*sz));
			sort_idx[idx]=n;
		}

		cnt++;
	}

		// fill in the list

	for (n=0;n!=cnt;n++) {
		idx=sort_idx[n];
			
		file_dir_name_to_name(fpd->files[idx].file_name,name_str);
		sprintf((file_table_data+(128*n)),"%s\t%s",name_str,fpd->files[idx].file_name);
	}

	file_paths_close_directory(fpd);

	file_has_list=(cnt!=0);
}

void file_close_list(void)
{
	free(file_table_data);
}

/* =======================================================

      Save File Selected
      
======================================================= */

void file_save_selected(void)
{
	int				idx;
	char			file_name[256],str[256],path[1024];
	
	idx=element_get_value(file_info_table_id);
	
		// nothing selected
		
	if (idx==-1) {

			// buttons

		if (!file_is_save) {
			element_enable(file_button_load_id,FALSE);
			element_enable(file_button_load_id,FALSE);
		}

		element_enable(file_button_delete_id,FALSE);

			// info
		
		element_set_bitmap(file_info_bitmap_id,NULL);
		element_text_change(file_info_name_id,"");
		element_text_change(file_info_time_id,"");
		element_text_change(file_info_elapsed_id,"");
		return;
	}
	
		// save game selected

	if (!file_is_save) element_enable(file_button_load_id,TRUE);
	element_enable(file_button_delete_id,TRUE);

		// info

	element_get_table_column_data(file_info_table_id,idx,1,file_name,256);

	file_paths_app_data(&file_path_setup,path,"Saved Games",NULL,NULL);
	strcat(path,"/");
	strcat(path,file_name);
	strcat(path,".png");
	element_set_bitmap(file_info_bitmap_id,path);

	file_dir_name_to_name(file_name,str);
	element_text_change(file_info_name_id,str);

	file_dir_name_to_time(file_name,str);
	element_text_change(file_info_time_id,str);

	file_dir_name_to_elapsed(file_name,str);
	element_text_change(file_info_elapsed_id,str);
}

/* =======================================================

      Save File Delete
      
======================================================= */

void file_save_delete(void)
{
	int				idx;
	char			*c,file_name[256],path[1024];
	
	idx=element_get_value(file_info_table_id);
	if (idx==-1) return;
	
		// delete save game and png
		
	element_get_table_column_data(file_info_table_id,idx,1,file_name,256);
	
	c=strrchr(file_name,'.');		// get rid of .sav
	if (c!=NULL) *c=0x0;
	
	file_paths_app_data(&file_path_setup,path,"Saved Games",file_name,"png");
	remove(path);
	
	file_paths_app_data(&file_path_setup,path,"Saved Games",file_name,"sav");
	remove(path);
	
		// reset table
		
	file_close_list();
	file_build_list();
	
	element_set_table_data(file_info_table_id,FALSE,file_table_data);
	
	element_set_value(file_info_table_id,-1);
	file_save_selected();
}

/* =======================================================

      File Operations
      
======================================================= */

void file_open(void)
{
	int							x,y,fx,fy,ty,wid,high,mx,tx,half_wid,pic_high,
								table_high,margin,padding;
	element_frame_button_type	butts_save[3]={{file_button_delete_id,"Delete",FALSE},{file_button_cancel_id,"Cancel",TRUE},{file_button_save_id,"New Save",TRUE}},
								butts_load[3]={{file_button_delete_id,"Delete",FALSE},{file_button_cancel_id,"Cancel",TRUE},{file_button_load_id,"Load",TRUE}};
	element_column_type	cols[4];
	
	file_last_state=server.last_state;
	
		// setup gui
		
	gui_initialize("Bitmaps/Backgrounds","main");
	
		// the frame

	margin=element_get_margin();
	padding=element_get_padding();

	fx=margin;
	fy=margin;
	wid=iface.scale_x-(margin*2);
	high=iface.scale_y-(margin*2);
	
	if (file_is_save) {
		element_frame_add("Save Game",file_frame_id,fx,fy,wid,high,-1,0,NULL,3,butts_save);
	}
	else {
		element_frame_add("Load Game",file_frame_id,fx,fy,wid,high,-1,0,NULL,3,butts_load);
	}

	element_get_frame_inner_space(file_frame_id,&x,&y,&wid,&table_high);

	half_wid=(wid>>1)-padding;
	mx=((x+wid)-half_wid)-padding;
	
		// make the file list
		
	file_build_list();

		// info

	pic_high=(half_wid*3)/4;
	ty=y+((table_high-(pic_high+margin+iface.font.text_size_medium+(element_get_control_high()*2)))>>1);

	element_bitmap_add(NULL,file_info_bitmap_id,x,ty,half_wid,pic_high,TRUE);

	tx=x+(half_wid>>2);
	ty+=(pic_high+margin+iface.font.text_size_medium);

	element_text_add("Map:",-1,tx,ty,iface.font.text_size_medium,tx_right,&iface.color.control.text,FALSE);
	element_text_add("",file_info_name_id,(tx+10),ty,iface.font.text_size_medium,tx_left,&iface.color.control.text,FALSE);

	ty+=element_get_control_high();
	element_text_add("Time:",-1,tx,ty,iface.font.text_size_medium,tx_right,&iface.color.control.text,FALSE);
	element_text_add("",file_info_time_id,(tx+10),ty,iface.font.text_size_medium,tx_left,&iface.color.control.text,FALSE);

	ty+=element_get_control_high();
	element_text_add("Elapsed:",-1,tx,ty,iface.font.text_size_medium,tx_right,&iface.color.control.text,FALSE);
	element_text_add("",file_info_elapsed_id,(tx+10),ty,iface.font.text_size_medium,tx_left,&iface.color.control.text,FALSE);

		// files

	strcpy(cols[0].name,"Saved Games");
	cols[0].percent_size=1.0f;

	element_table_add(cols,file_table_data,file_info_table_id,1,(mx+padding),y,half_wid,table_high,FALSE,element_table_bitmap_none);

		// if a first item, select it

	if (file_has_list) element_set_value(file_info_table_id,0);

		// enable buttons

	file_save_selected();
}

void file_close(void)
{
	gui_shutdown();
	file_close_list();
}

void file_setup(bool is_save)
{
	file_is_save=is_save;
}

/* =======================================================

      File Input
      
======================================================= */

void file_input(void)
{
	char		err_str[256];
	
	if (input_action_get_state_single(nc_save)) {
		file_setup(TRUE);
		server.next_state=gs_file;
		return;
	}
	
	if (input_action_get_state_single(nc_load)) {
		file_setup(FALSE);
		server.next_state=gs_file;
		return;
	}
	
	if (input_action_get_state_single(nc_quick_save)) {
		if (!game_file_save(FALSE,FALSE,err_str)) console_add_error(err_str);
		return;
	}
}

void file_click(void)
{
	int			id,k;
	char		err_str[256],file_name[256];

	id=-1;

		// keyboard

	if (input_get_keyboard_escape()) id=file_button_cancel_id;
	if (input_get_keyboard_return()) id=file_is_save?file_button_save_id:file_button_load_id;

		// clicking

	if (id==-1) {
		id=gui_click();
		if (id!=-1) hud_click();
	}
		
	if (id==-1) return;

		// handle click
		
	switch (id) {
	
		case file_button_save_id:
			if (!game_file_save(FALSE,FALSE,err_str)) console_add_error(err_str);
			server.next_state=gs_running;
			break;
			
		case file_button_load_id:

			k=element_get_value(file_info_table_id);
			if (k==-1) break;
			
			element_get_table_column_data(file_info_table_id,k,1,file_name,256);

			server.next_state=gs_running;

			if (game_file_load(file_name,FALSE,err_str)) {
				game_time_pause_end();
				break;
			}

			if (server.map_open) map_end();
			if (server.game_open) game_end();

			error_setup(err_str,"Game Load Canceled");
			server.next_state=gs_error;
			break;
			
		case file_button_delete_id:
			file_save_delete();
			break;
			
		case file_button_cancel_id:
			server.next_state=file_last_state;
			break;
			
		case file_info_table_id:
			file_save_selected();
			break;
			
	}
}
	
/* =======================================================

      Run File
      
======================================================= */

void file_run(void)
{
	gui_draw(1.0f,TRUE);
	file_click();
}

