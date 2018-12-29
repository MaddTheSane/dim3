/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Multiplayer XML

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
	#include "dim3baseutility.h"
#endif

extern file_path_setup_type		file_path_setup;

/* =======================================================

      Read Multiplayer XML
      
======================================================= */

void iface_read_settings_multiplayer(iface_type *iface)
{
	int							cnt,multiplayer_head_tag,settings_tag,
								games_head_tag,game_tag,tag,
								options_head_tag,option_tag,
								character_head_tag,character_item_tag,
								bot_head_tag,bot_tag,news_tag;
	char						path[1024];
	iface_mp_game_type			*mp_game;
	iface_mp_option_type		*mp_option;
	iface_mp_character_type		*mp_character;
	iface_mp_bot_type			*mp_bot;

	file_paths_data(&file_path_setup,path,"Settings","Multiplayer","xml");
	if (!xml_open_file(path)) return;
	
	multiplayer_head_tag=xml_findrootchild("Multiplayer");
    if (multiplayer_head_tag==-1) {
		xml_close_file();
		return;
	}

		// settings
		
    settings_tag=xml_findfirstchild("Settings",multiplayer_head_tag);
    if (settings_tag!=-1) {
		iface->multiplayer.on=xml_get_attribute_boolean(settings_tag,"on");
		iface->multiplayer.local_only=xml_get_attribute_boolean(settings_tag,"local_only");
	}

		// network games

	games_head_tag=xml_findfirstchild("Games",multiplayer_head_tag);
	if (games_head_tag!=-1) {
		
		iface->multiplayer.game_list.ngame=0;
		
		game_tag=xml_findfirstchild("Game",games_head_tag);
		while (game_tag!=-1) {
		
			mp_game=&iface->multiplayer.game_list.games[iface->multiplayer.game_list.ngame];
		
			xml_get_attribute_text(game_tag,"type",mp_game->name,name_str_len);
			mp_game->use_teams=xml_get_attribute_boolean(game_tag,"use_teams");
			mp_game->monsters=xml_get_attribute_boolean(game_tag,"monsters");
			
			tag=xml_findfirstchild("Script",game_tag);
			if (tag!=-1) {
				xml_get_attribute_text(tag,"player",mp_game->script.player_script,file_str_len);
				xml_get_attribute_text(tag,"bot",mp_game->script.bot_script,file_str_len);
			}
			
			tag=xml_findfirstchild("Spawn",game_tag);
			if (tag!=-1) {
				xml_get_attribute_text(tag,"spot_name",mp_game->spawn.spot_name,name_str_len);
				mp_game->spawn.force_team_spot=xml_get_attribute_boolean(tag,"force_team_spot");
			}
			
			tag=xml_findfirstchild("Score",game_tag);
			if (tag!=-1) {
				mp_game->score.kill=xml_get_attribute_int(tag,"kill");
				mp_game->score.death=xml_get_attribute_int(tag,"death");
				mp_game->score.suicide=xml_get_attribute_int(tag,"suicide");
				mp_game->score.goal=xml_get_attribute_int(tag,"goal");
			}
			
			iface->multiplayer.game_list.ngame++;
			if (iface->multiplayer.game_list.ngame==max_net_game) break;

			game_tag=xml_findnextchild(game_tag);
		}
	}

		// network options

	options_head_tag=xml_findfirstchild("Options",multiplayer_head_tag);
	if (options_head_tag!=-1) {
		
		iface->multiplayer.option_list.noption=0;
		
		option_tag=xml_findfirstchild("Option",options_head_tag);
		while (option_tag!=-1) {
			
			mp_option=&iface->multiplayer.option_list.options[iface->multiplayer.option_list.noption];
	
			xml_get_attribute_text(option_tag,"name",mp_option->name,name_str_len);
			xml_get_attribute_text(option_tag,"description",mp_option->descript,64);
			
			iface->multiplayer.option_list.noption++;
			if (iface->multiplayer.option_list.noption==max_net_option) break;

			option_tag=xml_findnextchild(option_tag);
		}
	}

		// characters

    character_head_tag=xml_findfirstchild("Characters",multiplayer_head_tag);
    if (character_head_tag!=-1) {

		iface->multiplayer.character_list.ncharacter=0;
		character_item_tag=xml_findfirstchild("Character",character_head_tag);
		
		while (character_item_tag!=-1) {
			mp_character=&iface->multiplayer.character_list.characters[iface->multiplayer.character_list.ncharacter];
			
			xml_get_attribute_text(character_item_tag,"name",mp_character->name,name_str_len);
			xml_get_attribute_text(character_item_tag,"model",mp_character->model_name,name_str_len);
			xml_get_attribute_text(character_item_tag,"parameter",mp_character->param,name_str_len);
			mp_character->interface_resize=xml_get_attribute_float_default(character_item_tag,"interface_resize",1.0f);
			xml_get_attribute_3_coord_int(character_item_tag,"interface_offset",&mp_character->interface_offset.x,&mp_character->interface_offset.y,&mp_character->interface_offset.z);
			
			iface->multiplayer.character_list.ncharacter++;
			character_item_tag=xml_findnextchild(character_item_tag);
		}
	}
	
		// bot names
	
    bot_head_tag=xml_findfirstchild("Bots",multiplayer_head_tag);
    if (bot_head_tag!=-1) {

		iface->multiplayer.bot_list.on=!xml_get_attribute_boolean(bot_head_tag,"hide");
	
		cnt=0;
		bot_tag=xml_findfirstchild("Bot",bot_head_tag);
		
		while (bot_tag!=-1) {
			mp_bot=&iface->multiplayer.bot_list.bots[cnt];

			xml_get_attribute_text(bot_tag,"name",mp_bot->name,name_str_len);

			cnt++;
			bot_tag=xml_findnextchild(bot_tag);
		}
	}
	
		// news
		
    news_tag=xml_findfirstchild("News",multiplayer_head_tag);
    if (news_tag!=-1) {
		xml_get_attribute_text(news_tag,"host",iface->multiplayer.news.host,64);
		iface->multiplayer.news.port=xml_get_attribute_int_default(news_tag,"port",80);
		xml_get_attribute_text(news_tag,"url",iface->multiplayer.news.url,256);
	}

	xml_close_file();
}

/* =======================================================

      Write Multiplayer XML
      
======================================================= */

bool iface_write_settings_multiplayer(iface_type *iface,char *err_str)
{
	int							n;
	char						path[1024];
	bool						ok;
	iface_mp_game_type			*mp_game;
	iface_mp_option_type		*mp_option;
	iface_mp_character_type		*mp_character;
	iface_mp_bot_type			*mp_bot;
	
		// start new file
		
	xml_new_file();

		// multiplayer

	xml_add_tagstart("Multiplayer");
	xml_add_tagend(FALSE);

		// settings

	xml_add_tagstart("Settings");
	xml_add_attribute_boolean("on",iface->multiplayer.on);
	xml_add_attribute_boolean("local_only",iface->multiplayer.local_only);
	xml_add_tagend(TRUE);

		// network games

	xml_add_tagstart("Games");
	xml_add_tagend(FALSE);
	
	mp_game=iface->multiplayer.game_list.games;

	for (n=0;n!=iface->multiplayer.game_list.ngame;n++) {
	
		xml_add_tagstart("Game");
		xml_add_attribute_text("type",mp_game->name);
		xml_add_attribute_boolean("use_teams",mp_game->use_teams);
		xml_add_attribute_boolean("monsters",mp_game->monsters);
		xml_add_tagend(FALSE);
		
		xml_add_tagstart("Script");
		xml_add_attribute_text("player",mp_game->script.player_script);
		xml_add_attribute_text("bot",mp_game->script.bot_script);
		xml_add_tagend(TRUE);
			
		xml_add_tagstart("Spawn");
		xml_add_attribute_text("spot_name",mp_game->spawn.spot_name);
		xml_add_attribute_boolean("force_team_spot",mp_game->spawn.force_team_spot);
		xml_add_tagend(TRUE);
			
		xml_add_tagstart("Score");
		xml_add_attribute_int("kill",mp_game->score.kill);
		xml_add_attribute_int("death",mp_game->score.death);
		xml_add_attribute_int("suicide",mp_game->score.suicide);
		xml_add_attribute_int("goal",mp_game->score.goal);
		xml_add_tagend(TRUE);

		xml_add_tagclose("Game");
		
		mp_game++;
	}

	xml_add_tagclose("Games");

		// network options

	xml_add_tagstart("Options");
	xml_add_tagend(FALSE);

	mp_option=iface->multiplayer.option_list.options;

	for (n=0;n!=iface->multiplayer.option_list.noption;n++) {
		xml_add_tagstart("Option");
		xml_add_attribute_text("name",mp_option->name);
		xml_add_attribute_text("description",mp_option->descript);
		xml_add_tagend(TRUE);

		mp_option++;
	}

	xml_add_tagclose("Options");

		// characters

	xml_add_tagstart("Characters");
	xml_add_tagend(FALSE);

	mp_character=iface->multiplayer.character_list.characters;

	for (n=0;n!=iface->multiplayer.character_list.ncharacter;n++) {
		xml_add_tagstart("Character");
		xml_add_attribute_text("name",mp_character->name);
		xml_add_attribute_text("model",mp_character->model_name);
		xml_add_attribute_text("parameter",mp_character->param);
		xml_add_attribute_float("interface_resize",mp_character->interface_resize);
		xml_add_attribute_3_coord_int("interface_offset",mp_character->interface_offset.x,mp_character->interface_offset.y,mp_character->interface_offset.z);
		xml_add_tagend(TRUE);

		mp_character++;
	}

	xml_add_tagclose("Characters");
	
		// bot names

	xml_add_tagstart("Bots");
	xml_add_attribute_boolean("hide",!iface->multiplayer.bot_list.on);
	xml_add_tagend(FALSE);

	mp_bot=iface->multiplayer.bot_list.bots;

	for (n=0;n!=max_net_bot;n++) {
		if (mp_bot->name[0]==0x0) continue;

		xml_add_tagstart("Bot");
		xml_add_attribute_text("name",mp_bot->name);
		xml_add_tagend(TRUE);

		mp_bot++;
	}

	xml_add_tagclose("Bots");

		// news
		
	xml_add_tagstart("News");
	xml_add_attribute_text("host",iface->multiplayer.news.host);
	xml_add_attribute_int("port",iface->multiplayer.news.port);
	xml_add_attribute_text("url",iface->multiplayer.news.url);
	xml_add_tagend(TRUE);

	xml_add_tagclose("Multiplayer");

        // write the xml
		
	file_paths_data(&file_path_setup,path,"Settings","Multiplayer","xml");
		
	ok=xml_save_file(path,err_str);
    xml_close_file();
	
	return(ok);
}
