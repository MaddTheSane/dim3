/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Setup XML

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

extern iface_type			iface;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

setup_type					setup;

/* =======================================================

      Default Preferences
      
======================================================= */

void setup_xml_default(void)
{
	setup.screen_rtl_wid=320;
	setup.screen_rtl_high=200;

	setup.full_screen=TRUE;
	setup.fsaa_mode=fsaa_mode_none;
	
	setup.sound_volume=0.6f;
	setup.music_on=TRUE;
	setup.music_volume=0.5f;
    
	setup.always_run=FALSE;
	setup.toggle_run=FALSE;
	setup.invert_look=FALSE;
	setup.mouse_smooth=TRUE;
	setup.auto_aim=FALSE;

	setup.mouse.speed=0.03f;
	setup.mouse.acceleration=0.6f;

	setup.joystick.speed=0.03f;
	setup.joystick.acceleration=0.6f;

	setup.action_list.naction=0;

	strcpy(setup.network.name,"Player");
	setup.network.character_idx=0;
	setup.network.tint_color_idx=0;
	setup.network.show_names=TRUE;
	setup.network.map_rotation=FALSE;
	
	setup.network.bot.count=0;
	setup.network.bot.skill=2;
	
	setup.network.map_list.count=0;
	setup.network.option_list.count=0;

	setup.network.game_type=0;
	setup.network.score_limit=20;
	setup.network.game_reset_secs=15;
	setup.network.respawn_secs=5;
	
	setup.no_hud=FALSE;
	setup.no_draw_weapon=FALSE;
	setup.metrics_on=FALSE;
	setup.debug_on=FALSE;
	setup.ignore_fps_lock=FALSE;
}

/* =======================================================

      Read Setup
      
======================================================= */

bool setup_xml_read_path(char *path)
{
	int							n,k,naction,
								setup_tag,actions_tag,maps_tag,options_tag,tag;
	char						tag_name[32];
	setup_action_type			*action;
	
		// read file
		
	setup_xml_default();

	if (!xml_open_file(path)) return(FALSE);
    
		// decode the file
       
    setup_tag=xml_findrootchild("Setup");
    if (setup_tag==-1) {
		xml_close_file();
		return(FALSE);
    }
	
		// keys

	xml_key_read_int(setup_tag,"Screen_Width",&setup.screen_rtl_wid);
	xml_key_read_int(setup_tag,"Screen_Height",&setup.screen_rtl_high);
	xml_key_read_boolean(setup_tag,"Full_Screen",&setup.full_screen);
	xml_key_read_int(setup_tag,"FSAA_Mode",&setup.fsaa_mode);
	xml_key_read_float(setup_tag,"Sound_Volume",&setup.sound_volume);
	xml_key_read_boolean(setup_tag,"Music_On",&setup.music_on);
	xml_key_read_float(setup_tag,"Music_Volume",&setup.music_volume);
	xml_key_read_boolean(setup_tag,"Always_Run",&setup.always_run);
	xml_key_read_boolean(setup_tag,"Toggle_Run",&setup.toggle_run);
	xml_key_read_boolean(setup_tag,"Invert_Look",&setup.invert_look);
	xml_key_read_boolean(setup_tag,"Auto_Aim",&setup.auto_aim);
	xml_key_read_boolean(setup_tag,"Mouse_Smooth",&setup.mouse_smooth);
	xml_key_read_float(setup_tag,"Mouse_Speed",&setup.mouse.speed);
	xml_key_read_float(setup_tag,"Mouse_Acceleration",&setup.mouse.acceleration);
	xml_key_read_float(setup_tag,"Joystick_Speed",&setup.joystick.speed);
	xml_key_read_float(setup_tag,"Joystick_Acceleration",&setup.joystick.acceleration);
	xml_key_read_text(setup_tag,"Network_Name",setup.network.name,name_str_len);
    xml_key_read_int(setup_tag,"Character",&setup.network.character_idx);
    xml_key_read_int(setup_tag,"Tint",&setup.network.tint_color_idx);
	xml_key_read_int(setup_tag,"Host_Bot_Count",&setup.network.bot.count);
	xml_key_read_int(setup_tag,"Host_Bot_Skill",&setup.network.bot.skill);
	xml_key_read_int(setup_tag,"Host_Game_Type",&setup.network.game_type);
	xml_key_read_int(setup_tag,"Host_Score_Limit",&setup.network.score_limit);
	xml_key_read_int(setup_tag,"Host_Game_Reset_Secs",&setup.network.game_reset_secs);
	xml_key_read_int(setup_tag,"Host_Respawn_Secs",&setup.network.respawn_secs);
	xml_key_read_boolean(setup_tag,"Network_Show_Names",&setup.network.show_names);
	xml_key_read_boolean(setup_tag,"Network_Map_Rotation",&setup.network.map_rotation);
	xml_key_read_boolean(setup_tag,"No_HUD",&setup.no_hud);
	xml_key_read_boolean(setup_tag,"No_Draw_Weapon",&setup.no_draw_weapon);
	xml_key_read_boolean(setup_tag,"Metrics_On",&setup.metrics_on);
	xml_key_read_boolean(setup_tag,"Debug_On",&setup.debug_on);
	xml_key_read_boolean(setup_tag,"Ignore_FPS_Lock",&setup.ignore_fps_lock);

		// actions

    actions_tag=xml_findfirstchild("Actions",setup_tag);
    if (actions_tag!=-1) {
	
		naction=xml_countchildren(actions_tag);
		tag=xml_findfirstchild("Action",actions_tag);
		
		setup.action_list.naction=naction;
		action=setup.action_list.actions;
		
        for (n=0;n!=naction;n++) {
			xml_get_attribute_text(tag,"name",action->name,32);
			
			for (k=0;k!=max_setup_action_attach;k++) {
				sprintf(tag_name,"attach_%d",k);
				xml_get_attribute_text(tag,tag_name,action->attach[k],32);
			}
           
			tag=xml_findnextchild(tag);
			
			action++;
        }
	}

 		// maps
		
    maps_tag=xml_findfirstchild("Maps",setup_tag);
    if (maps_tag!=-1) {
	
		setup.network.map_list.count=xml_countchildren(maps_tag);
		tag=xml_findfirstchild("Map",maps_tag);
		
        for (n=0;n!=setup.network.map_list.count;n++) {
			xml_get_attribute_text(tag,"name",setup.network.map_list.maps[n].name,name_str_len);
			tag=xml_findnextchild(tag);
 		}
	}
	
 		// options
		
    options_tag=xml_findfirstchild("Options",setup_tag);
    if (options_tag!=-1) {
	
		setup.network.option_list.count=xml_countchildren(options_tag);
		tag=xml_findfirstchild("Option",options_tag);
		
        for (n=0;n!=setup.network.option_list.count;n++) {
			xml_get_attribute_text(tag,"name",setup.network.option_list.options[n].name,name_str_len);
			tag=xml_findnextchild(tag);
 		}
	}
  
	xml_close_file();
	
	return(TRUE);
}

bool setup_xml_read(void)
{
	char				path[1024];
	
		// check user specific setup XML file.  If it exists, use that,
		// otherwise use default XML file
		
	if (!file_paths_app_data_exist(&file_path_setup,path,"Settings","Setup","xml")) {
		file_paths_data(&file_path_setup,path,"Settings","Setup","xml");
	}
	
	if (!setup_xml_read_path(path)) return(FALSE);

	return(TRUE);
}

bool setup_xml_reset(void)
{
	char				path[1024];
	
		// read directly from the default
		
	file_paths_data(&file_path_setup,path,"Settings","Setup","xml");

	return(setup_xml_read_path(path));
}

/* =======================================================

      Write Setup
      
======================================================= */

bool setup_xml_write(void)
{
	int							n,k;
	char						path[1024],tag_name[32],err_str[256];
	bool						ok;
	setup_action_type			*action;
	
		// start the setup file
		
    xml_new_file();
    
    xml_add_tagstart("Setup");
    xml_add_tagend(FALSE);
	
		// keys

    xml_key_write_int("Screen_Width",setup.screen_rtl_wid);
    xml_key_write_int("Screen_Height",setup.screen_rtl_high);
	xml_key_write_boolean("Full_Screen",setup.full_screen);
	xml_key_write_int("FSAA_Mode",setup.fsaa_mode);
	xml_key_write_float("Sound_Volume",setup.sound_volume);
	xml_key_write_boolean("Music_On",setup.music_on);
	xml_key_write_float("Music_Volume",setup.music_volume);
	xml_key_write_boolean("Always_Run",setup.always_run);
	xml_key_write_boolean("Toggle_Run",setup.toggle_run);
	xml_key_write_boolean("Invert_Look",setup.invert_look);
	xml_key_write_boolean("Auto_Aim",setup.auto_aim);
	xml_key_write_boolean("Mouse_Smooth",setup.mouse_smooth);
	xml_key_write_float("Mouse_Speed",setup.mouse.speed);
	xml_key_write_float("Mouse_Acceleration",setup.mouse.acceleration);
	xml_key_write_float("Joystick_Speed",setup.joystick.speed);
	xml_key_write_float("Joystick_Acceleration",setup.joystick.acceleration);
	xml_key_write_text("Network_Name",setup.network.name);
	xml_key_write_int("Character",setup.network.character_idx);
	xml_key_write_int("Tint",setup.network.tint_color_idx);
	xml_key_write_int("Host_Bot_Count",setup.network.bot.count);
	xml_key_write_int("Host_Bot_Skill",setup.network.bot.skill);
	xml_key_write_int("Host_Game_Type",setup.network.game_type);
	xml_key_write_int("Host_Score_Limit",setup.network.score_limit);
	xml_key_write_int("Host_Game_Reset_Secs",setup.network.game_reset_secs);
	xml_key_write_int("Host_Respawn_Secs",setup.network.respawn_secs);
	xml_key_write_boolean("Network_Show_Names",setup.network.show_names);
	xml_key_write_boolean("Network_Map_Rotation",setup.network.map_rotation);
	xml_key_write_boolean("No_HUD",setup.no_hud);
	xml_key_write_boolean("No_Draw_Weapon",setup.no_draw_weapon);
	xml_key_write_boolean("Metrics_On",setup.metrics_on);
	xml_key_write_boolean("Debug_On",setup.debug_on);
	xml_key_write_boolean("Ignore_FPS_Lock",setup.ignore_fps_lock);

		// actions

    xml_add_tagstart("Actions");
    xml_add_tagend(FALSE);
	
	action=setup.action_list.actions;
		
	for (n=0;n!=setup.action_list.naction;n++) {
		xml_add_tagstart("Action");
		xml_add_attribute_text("name",action->name);
			
		for (k=0;k!=max_setup_action_attach;k++) {
			if (action->attach[k][0]!=0x0) {
				sprintf(tag_name,"attach_%d",k);
				xml_add_attribute_text(tag_name,action->attach[k]);
			}
		}
           
	    xml_add_tagend(TRUE);
		action++;
	}

    xml_add_tagclose("Actions");
	
		// maps
		
    xml_add_tagstart("Maps");
    xml_add_tagend(FALSE);
	
	for (n=0;n!=setup.network.map_list.count;n++) {
		xml_add_tagstart("Map");
		xml_add_attribute_text("name",setup.network.map_list.maps[n].name);
		xml_add_tagend(TRUE);
	}

    xml_add_tagclose("Maps");
	
		// options
		
    xml_add_tagstart("Options");
    xml_add_tagend(FALSE);
	
	for (n=0;n!=setup.network.option_list.count;n++) {
		xml_add_tagstart("Option");
		xml_add_attribute_text("name",setup.network.option_list.options[n].name);
		xml_add_tagend(TRUE);
	}

    xml_add_tagclose("Options");
		
		// close setup
		
    xml_add_tagclose("Setup");

        // save the setup
		// always save to user specific data
		
	file_paths_app_data(&file_path_setup,path,"Settings","Setup","xml");
		
	ok=xml_save_file(path,err_str);
    xml_close_file();
	
	return(ok);
}

/* =======================================================

      Restore Setup on Bad Startup
      
======================================================= */

void setup_restore(void)
{
		// reset important defaults
		// to their simpliest form

	setup.screen_rtl_wid=320;
	setup.screen_rtl_high=200;
		
	setup.full_screen=FALSE;
	setup.fsaa_mode=fsaa_mode_none;
	
	setup.music_on=FALSE;

	setup.no_hud=FALSE;
	setup.no_draw_weapon=FALSE;
	setup.metrics_on=FALSE;
	setup.debug_on=FALSE;
	setup.ignore_fps_lock=FALSE;
	
		// save XML
		
	setup_xml_write();
}

