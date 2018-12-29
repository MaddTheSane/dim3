/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Network Host

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
#include "network.h"
#include "objects.h"

#define host_pane_game					0
#define host_pane_options				1
#define host_pane_info					2

#define host_frame_id					0
#define host_tab_id						1

#define host_button_host_id				10
#define host_button_cancel_id			11
#define host_game_type_id				12
#define host_table_id					13
#define host_dedicated_id				14
#define host_map_rotation_id			15
#define host_status_id					16

#define host_game_bot_count_id			20
#define host_game_bot_skill_id			21

#define host_game_score_limit_id		22
#define host_game_game_reset_secs_id	23
#define host_game_respawn_secs_id		24

#define host_game_option_base			100

extern map_type				map;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

int							host_tab_value,host_first_map_idx,
							host_file_map_count,host_table_map_count;
char						*host_file_map_list,*host_table_map_list;
char						net_game_types[max_net_game+1][name_str_len],
							bot_skill_list[6][32]={"Very Easy","Easy","Normal","Hard","Very Hard",""};

/* =======================================================

      Host Map Table Data
      
======================================================= */

bool host_check_game_type(char *game_type,char *game_list)
{
	int				idx;
	char			*c,*c2;
	char			game_token[256];

	c=game_list;

	while (TRUE) {

		if (*c==0x0) break;

			// check next token

		strcpy(game_token,c);
		c2=strchr(game_token,',');
		if (c2!=NULL) *c2=0x0;

			// remove white space

		if (game_token[0]!=0x0) {
			while (game_token[0]==' ') {
				strcpy((char*)&game_token[0],(char*)&game_token[1]);
			}
			while (TRUE) {
				idx=strlen(game_token);
				if (idx==0) break;
				if (game_token[idx-1]!=' ') break;
				
				game_token[idx-1]=0x0;
			}
		}

			// compare

		if (strcasecmp(game_type,game_token)==0) return(TRUE);

			// get next token

		c2=strchr(c,',');
		if (c2==NULL) break;

		c=c2+1;
	}

	return(FALSE);
}

void host_fill_map_table(char *game_type)
{
	int							n;
	char						*c,*c2;

	host_table_map_count=0;

	c=host_file_map_list;
	c2=host_table_map_list;

	for (n=0;n!=host_file_map_count;n++) {
		
		if (host_check_game_type(game_type,(c+(file_str_len+name_str_len)))) {
			sprintf(c2,"Bitmaps/Icons_Map;%s;%s",c,(c+file_str_len));
			c2+=128;
			
			host_table_map_count++;
		}

		c+=(file_str_len+name_str_len+256);
	}

	*c2=0x0;

	element_set_table_data(host_table_id,TRUE,host_table_map_list);
}

void host_map_list_initialize(void)
{
	int							n,nfile,sz;
	char						*c;
	char						info_name[name_str_len],game_list[256];
	bool						singleplayer_map_picker;
	file_path_directory_type	*map_pick_fpd;

	progress_initialize(NULL);

		// initial setup

	host_file_map_count=0;
	host_file_map_list=NULL;

	host_table_map_count=0;
	host_table_map_list=NULL;

		// load in all maps with the correct game type

	map_pick_fpd=file_paths_read_directory_data(&file_path_setup,"Maps","xml");

	nfile=map_pick_fpd->nfile;
	if (nfile<=0) {
		file_paths_close_directory(map_pick_fpd);
		return;
	}

		// data for maps

	progress_update();

	sz=nfile*(file_str_len+name_str_len+256);
	host_file_map_list=malloc(sz);
	bzero(host_file_map_list,sz);

	sz=(nfile+1)*128;
	host_table_map_list=malloc(sz);
	bzero(host_table_map_list,sz);

		// load the maps

	c=host_file_map_list;
	
	for (n=0;n!=nfile;n++) {

		progress_update();

			// get the map info

		if (!map_host_load_info(map_pick_fpd->files[n].file_name,info_name,&singleplayer_map_picker,game_list)) continue;

		strcpy(c,map_pick_fpd->files[n].file_name);
		c+=file_str_len;
		strcpy(c,info_name);
		c+=name_str_len;
		strcpy(c,game_list);
		c+=256;
		
		host_file_map_count++;
	}

	file_paths_close_directory(map_pick_fpd);

	progress_shutdown();
}

void host_map_list_shutdown(void)
{
	if (host_file_map_list!=NULL) free(host_file_map_list);
	if (host_table_map_list!=NULL) free(host_table_map_list);
}

/* =======================================================

      Set and Get Map List
      
======================================================= */

void host_map_list_get_name(int idx,char *name)
{
	char			*c;
	char			row_str[128];

	name[0]=0x0;

	element_get_table_row(host_table_id,idx,row_str);
	c=row_str;
		
	c=strchr(c,';');
	if (c!=NULL) {
		strcpy(name,(c+1));
		c=strchr(name,';');
		if (c!=NULL) *c=0x0;
	}
}

void host_map_list_to_table(void)
{
	int				n,k;
	char			name[256];
	bool			checked;

		// single map?

	if (!setup.network.map_rotation) {

		host_first_map_idx=-1;
		if (setup.network.map_list.count==0) return;

		for (n=0;n!=host_table_map_count;n++) {
			host_map_list_get_name(n,name);
			if (strcmp(setup.network.map_list.maps[0].name,name)==0) {
				host_first_map_idx=n;
				return;
			}
		}

		return;
	}

		// rotating maps

	host_first_map_idx=-1;

	for (n=0;n!=host_table_map_count;n++) {

			// get map name

		host_map_list_get_name(n,name);

			// find if map is in list

		checked=FALSE;

		for (k=0;k!=setup.network.map_list.count;k++) {
			if (strcasecmp(setup.network.map_list.maps[k].name,name)==0) {
				checked=TRUE;
				break;
			}
		}
	
		element_set_table_checkbox(host_table_id,n,checked);

			// always scroll to first selected map

		if (host_first_map_idx==-1) host_first_map_idx=n;
	}

	if (host_first_map_idx==-1) host_first_map_idx=0;
}

void host_map_table_to_list(void)
{
	int				n,idx,count;
	char			name[256];

	setup.network.map_list.count=0;

		// single map?

	if (!setup.network.map_rotation) {
		idx=element_get_value(host_table_id);
		if (idx==-1) return;

		host_map_list_get_name(idx,name);

		setup.network.map_list.count=1;
		strcpy(setup.network.map_list.maps[0].name,name);

		return;
	}
	
		// rotating maps

	count=0;

	for (n=0;n!=host_table_map_count;n++) {
		if (!element_get_table_checkbox(host_table_id,n)) continue;
	
		host_map_list_get_name(n,name);
		strcpy(setup.network.map_list.maps[count++].name,name);
	}

	setup.network.map_list.count=count;
}

/* =======================================================

      Host Panes
      
======================================================= */

void host_game_pane(void)
{
	int						n,fx,fy,x,y,wid,high,table_high,
							padding,control_y_add;
	element_column_type		cols[1];
	
	padding=element_get_padding();
	control_y_add=element_get_control_separation_high();
	
	element_get_frame_inner_space(host_frame_id,&fx,&fy,&wid,&high);
	
		// game type

	for (n=0;n!=iface.multiplayer.game_list.ngame;n++) {
		strcpy(net_game_types[n],iface.multiplayer.game_list.games[n].name);
	}
	net_game_types[iface.multiplayer.game_list.ngame][0]=0x0;

	x=(int)(((float)wid)*0.25f);
	y=fy+control_y_add;
	
	element_combo_add("Game Type",(char*)net_game_types,setup.network.game_type,host_game_type_id,x,y,TRUE);
	y+=control_y_add;

		// rotating maps button

	element_checkbox_add("Map Rotation",setup.network.map_rotation,host_map_rotation_id,x,y,TRUE);
	y+=padding;

		// hosts table
		
	table_high=(high-(y-fy));

	strcpy(cols[0].name,"Map");
	cols[0].percent_size=1.0f;

	element_table_add(cols,NULL,host_table_id,1,fx,y,wid,table_high,setup.network.map_rotation,element_table_bitmap_data);
	y+=(high+padding);
	
		// fill table with maps

	host_fill_map_table(iface.multiplayer.game_list.games[setup.network.game_type].name);
	host_map_list_to_table();

	element_set_value(host_table_id,host_first_map_idx);
	element_make_selection_visible(host_table_id);
}

void host_options_pane(void)
{
	int							n,k,fx,fy,wid,high,
								x,y,control_y_add,control_y_sz;
	bool						on;
	iface_mp_option_type		*mp_option;

		// panel sizes

	control_y_add=element_get_control_separation_high();
	control_y_sz=(control_y_add*(3+iface.multiplayer.option_list.noption))+element_get_padding();
	if (iface.multiplayer.bot_list.on) control_y_sz+=(control_y_add*2);
	
	element_get_frame_inner_space(host_frame_id,&fx,&fy,&wid,&high);
	x=fx+(int)(((float)wid)*0.45f);
	y=(fy+((high-control_y_sz)/2))+control_y_add;
	
		// bots

	if (iface.multiplayer.bot_list.on) {

		element_number_add("Bot Count",setup.network.bot.count,host_game_bot_count_id,x,y,0,15);
		y+=control_y_add;

		element_combo_add("Bot Skill",(char*)bot_skill_list,setup.network.bot.skill,host_game_bot_skill_id,x,y,TRUE);
		y+=control_y_add;
	}

		// score limits and spawn times

	element_number_add("Score Limit",setup.network.score_limit,host_game_score_limit_id,x,y,1,100);
	y+=control_y_add;
	
	element_number_add("Game Reset Seconds",setup.network.game_reset_secs,host_game_game_reset_secs_id,x,y,5,30);
	y+=control_y_add;
	
	element_number_add("Respawn Seconds",setup.network.respawn_secs,host_game_respawn_secs_id,x,y,0,30);
	y+=control_y_add;

		// project options

	y+=element_get_padding();

	mp_option=iface.multiplayer.option_list.options;

	for (n=0;n!=iface.multiplayer.option_list.noption;n++) {

		on=FALSE;

		for (k=0;k!=setup.network.option_list.count;k++) {
			if (strcasecmp(mp_option->name,setup.network.option_list.options[k].name)==0) {
				on=TRUE;
				break;
			}
		}

		element_checkbox_add(mp_option->descript,on,(host_game_option_base+n),x,y,TRUE);
		y+=control_y_add;

		mp_option++;
	}
}

void host_info_pane(void)
{
	int			fx,fy,wid,high,
				x,y,control_y_add,control_y_sz;
	char		str[256];
	
	control_y_add=element_get_control_separation_high();
	control_y_sz=4*control_y_add;
	
	element_get_frame_inner_space(host_frame_id,&fx,&fy,&wid,&high);
	x=fx+(int)(((float)wid)*0.45f);
	y=(fy+((high-control_y_sz)/2))+control_y_add;
	
		// type
		
	if (strcmp(net_setup.host.ip_resolve,"127.0.0.1")==0) {
		strcpy(str,"Local Machine");
	}
	else {
		if (net_ip_local(net_setup.host.ip_resolve)) {
			strcpy(str,"Local Network");
		}
		else {
			strcpy(str,"Internet");
		}
	}
		
	element_info_field_add("Type",str,-1,x,y);
	y+=control_y_add;
	
		// host name and IP
	
	element_info_field_add("Host Name",net_setup.host.name,-1,x,y);
	y+=control_y_add;
	element_info_field_add("Host IP",net_setup.host.ip_resolve,-1,x,y);
	y+=control_y_add;

	sprintf(str,"%d",net_port_host);
	element_info_field_add("Host Port",str,-1,x,y);
	y+=control_y_add;
}

void host_enable_host_button(void)
{
	int			pane;
	bool		enable;
	
	pane=element_get_value(host_tab_id);
	if (pane!=host_pane_game) return;
	
	if (!setup.network.map_rotation) {
		enable=(element_get_value(host_table_id)!=-1);
	}
	else {
		enable=element_has_table_check(host_table_id);
	}
		
	element_enable(host_button_host_id,enable);
}

void host_create_pane(void)
{
	int							fx,fy,wid,high,margin;
	char						tab_list[][32]={"Game","Options","Info"};
	element_frame_button_type	butts[2]={{host_button_cancel_id,"Cancel",TRUE},{host_button_host_id,"Host",TRUE}};
						
	element_clear();
	
		// frame
		
	margin=element_get_margin();

	fx=margin;
	fy=margin;
	wid=iface.scale_x-(margin*2);
	high=iface.scale_y-(margin*2);
	
	element_frame_add("Host Game",host_frame_id,fx,fy,wid,high,host_tab_id,3,(char*)tab_list,2,butts);
	element_set_value(host_tab_id,host_tab_value);
	
		// setup some IP information
		
	if (net_setup.host.name[0]==0x0) net_get_host_name(net_setup.host.name);
	net_get_host_ip(net_setup.host.ip_name,net_setup.host.ip_resolve);
	
		// specific pane controls
		
	switch (element_get_value(host_tab_id)) {
		case host_pane_game:
			host_game_pane();
			break;
		case host_pane_options:
			host_options_pane();
			break;
		case host_pane_info:
			host_info_pane();
			break;
	}
	
		// enable host button
		
	host_enable_host_button();
}

/* =======================================================

      Open/Close Host Game
      
======================================================= */

void host_open(void)
{
		// load the maps

	host_map_list_initialize();

		// get the project hash

	net_create_project_hash();

		// setup gui
		
	gui_initialize("Bitmaps/Backgrounds","main");

		// start with first tab
		
	host_tab_value=0;
	host_first_map_idx=-1;

	host_create_pane();
}

void host_close(void)
{
	host_map_list_shutdown();
	gui_shutdown();
}

/* =======================================================

      Host Game
      
======================================================= */

void host_game_setup(void)
{
	int				n,k;
	
		// game type
		
	net_setup.game_idx=setup.network.game_type;
	
		// game options
		
	net_setup.score_limit=setup.network.score_limit;
	net_setup.respawn_secs=setup.network.respawn_secs;
	net_setup.game_reset_secs=setup.network.game_reset_secs;
		
	net_setup.option_flags=0x0;
	
	for (n=0;n!=setup.network.option_list.count;n++) {
		for (k=0;k!=iface.multiplayer.option_list.noption;k++) {
			if (strcasecmp(setup.network.option_list.options[n].name,iface.multiplayer.option_list.options[k].name)==0) {
				net_setup.option_flags=net_setup.option_flags|(0x1<<k);
				break;
			}
		}
	}

		// starting game map

	net_setup.host.current_map_idx=0;
}

void host_game(void)
{
	char			err_str[256];
	
		// setup hosting flags and IPs
		
	net_host_game_setup();

	net_setup.mode=net_mode_host;
	net_setup.client.latency=0;
	net_setup.client.host_addr.ip=0;
	net_setup.client.host_addr.port=0;

		// setup map
		
	map.info.name[0]=0x0;
	strcpy(map.info.host_name,setup.network.map_list.maps[net_setup.host.current_map_idx].name);
	
		// start game
	
	if (!game_start(FALSE,skill_medium,0,0,err_str)) {
		net_host_game_end();
		error_setup(err_str,"Hosting Game Canceled");
		server.next_state=gs_error;
		return;
	}
	
		// add any multiplayer bots
		
	if (!game_multiplayer_bots_create(err_str)) {
		net_host_game_end();
		error_setup(err_str,"Hosting Game Canceled");
		server.next_state=gs_error;
		return;
	}
	
		// start the map
		
	if (!map_start(FALSE,TRUE,err_str)) {
		net_host_game_end();
		error_setup(err_str,"Hosting Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// start hosting

	if (!net_host_game_start(err_str)) {
		net_host_game_end();
		error_setup(err_str,"Hosting Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// add local player to host

	if (!net_host_join_local_player(err_str)) {
		net_host_game_end();
		error_setup(err_str,"Hosting Game Canceled");
		server.next_state=gs_error;
		return;
	}
	
		// add multiplayer bots to host

	net_host_join_multiplayer_bots();

		// game is running
	
	server.next_state=gs_running;
}

/* =======================================================

      Host Input
      
======================================================= */

void host_click(void)
{
	int							n,id,idx;
	iface_mp_option_type		*mp_option;

	id=-1;

		// keyboard

	if (input_get_keyboard_escape()) id=host_button_cancel_id;
	if (input_get_keyboard_return()) id=host_button_host_id;

	if (id==-1) id=gui_keyboard();

		// clicking

	if (id==-1) {
		id=gui_click();
		if (id!=-1) hud_click();
	}
		
	if (id==-1) return;

		// special option clicks
		// rebuild the options list

	if (id>=host_game_option_base) {
	
		setup.network.option_list.count=0;
		mp_option=iface.multiplayer.option_list.options;

		for (n=0;n!=iface.multiplayer.option_list.noption;n++) {

			if (element_get_value(host_game_option_base+n)!=0) {
				strcpy(setup.network.option_list.options[setup.network.option_list.count].name,mp_option->name);
				setup.network.option_list.count++;
			}

			mp_option++;
		}

		return;
	}

		// run selection

	switch (id) {

			// tab
			
		case host_tab_id:
			host_tab_value=element_get_value(host_tab_id);
			host_create_pane();
			return;

			// controls

		case host_game_type_id:
			idx=element_get_value(host_game_type_id);
			if (idx!=setup.network.game_type) {
				setup.network.game_type=idx;
				host_fill_map_table(iface.multiplayer.game_list.games[idx].name);
				host_map_list_to_table();
				element_set_value(host_table_id,host_first_map_idx);
				element_make_selection_visible(host_table_id);
				host_enable_host_button();
			}
			break;

		case host_table_id:
			host_map_table_to_list();
			element_enable(host_button_host_id,(setup.network.map_list.count!=0));
			break;

// supergumba -- not fully implemented yet
//		case host_dedicated_id:
//			setup.network.dedicated=(element_get_value(host_dedicated_id)!=0);
//			break;

		case host_map_rotation_id:
			setup.network.map_rotation=(element_get_value(host_map_rotation_id)!=0);
			host_create_pane();
			break;

		case host_game_bot_count_id:
			setup.network.bot.count=element_get_value(host_game_bot_count_id);
			break;

		case host_game_bot_skill_id:
			setup.network.bot.skill=element_get_value(host_game_bot_skill_id);
			break;

		case host_game_score_limit_id:
			setup.network.score_limit=element_get_value(host_game_score_limit_id);
			if (setup.network.score_limit<0) setup.network.score_limit=0;
			break;
			
		case host_game_game_reset_secs_id:
			setup.network.game_reset_secs=element_get_value(host_game_game_reset_secs_id);
			break;
			
		case host_game_respawn_secs_id:
			setup.network.respawn_secs=element_get_value(host_game_respawn_secs_id);
			break;

			// buttons

		case host_button_host_id:
			setup_xml_write();
			host_game_setup();
			host_game();
			break;
			
		case host_button_cancel_id:
			setup_xml_write();
			server.next_state=gs_intro;
			break;
	}
}

/* =======================================================

      Run Host
      
======================================================= */

void host_run(void)
{
	gui_draw(1.0f,TRUE);
	host_click();
}

