/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Multiplayer Game

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

#define kMPGamePropertySettingsName				0
#define kMPGamePropertySettingsTeam				1
#define kMPGamePropertySettingsMonster			2
#define kMPGamePropertyScriptPlayerScript		3
#define kMPGamePropertyScriptBotScript			4
#define kMPGamePropertySpawnSpotName			5
#define kMPGamePropertySpawnForceTeam			6
#define kMPGamePropertyScoreKill				7
#define kMPGamePropertyScoreDeath				8
#define kMPGamePropertyScoreSuicide				9
#define kMPGamePropertyScoreGoal				10

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Multiplayer Game
      
======================================================= */

void project_palette_fill_multiplayer_game(int multiplayer_game_idx)
{
	iface_mp_game_type		*mp_game;

	mp_game=&iface.multiplayer.game_list.games[multiplayer_game_idx];

	list_palette_set_title(&project_palette,"Multiplayer",NULL,"Game",mp_game->name,NULL,NULL);

		// settings

	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_string(&project_palette,kMPGamePropertySettingsName,"Name",mp_game->name,name_str_len,FALSE);
	list_palette_add_checkbox(&project_palette,kMPGamePropertySettingsTeam,"Requires Teams",&mp_game->use_teams,FALSE);
	list_palette_add_checkbox(&project_palette,kMPGamePropertySettingsMonster,"Include Monsters",&mp_game->monsters,FALSE);

		// bots

	list_palette_add_header(&project_palette,0,"Scripts");
	list_palette_add_string(&project_palette,kMPGamePropertyScriptPlayerScript,"Player",mp_game->script.player_script,file_str_len,FALSE);
	list_palette_add_string(&project_palette,kMPGamePropertyScriptBotScript,"Bot",mp_game->script.bot_script,file_str_len,FALSE);
	
		// spawn

	list_palette_add_header(&project_palette,0,"Spawning");
	list_palette_add_string(&project_palette,kMPGamePropertySpawnSpotName,"Spot Name",mp_game->spawn.spot_name,name_str_len,FALSE);
	list_palette_add_checkbox(&project_palette,kMPGamePropertySpawnForceTeam,"Force Team Spots",&mp_game->spawn.force_team_spot,FALSE);
	
		// score

	list_palette_add_header(&project_palette,0,"Scoring");
	list_palette_add_int(&project_palette,kMPGamePropertyScoreKill,"Kill",&mp_game->score.kill,FALSE);
	list_palette_add_int(&project_palette,kMPGamePropertyScoreDeath,"Death",&mp_game->score.death,FALSE);
	list_palette_add_int(&project_palette,kMPGamePropertyScoreSuicide,"Suicide",&mp_game->score.suicide,FALSE);
	list_palette_add_int(&project_palette,kMPGamePropertyScoreGoal,"Goal",&mp_game->score.goal,FALSE);
}

/* =======================================================

      Property Palette Click Multiplayer Game
      
======================================================= */

void project_palette_click_multiplayer_game(int multiplayer_game_idx,bool double_click)
{
}

