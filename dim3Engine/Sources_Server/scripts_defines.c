/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Defines

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

#include "scripts.h"

extern js_type				js;
extern setup_type			setup;
extern file_path_setup_type	file_path_setup;

script_define_type			*script_user_defines;

script_define_type			script_dim3_defines[]={

									// constants
									
								{sd_proj_hit_type_none,							"",		"DIM3_PROJ_HIT_TYPE_NONE"},
								{sd_proj_hit_type_object,						"",		"DIM3_PROJ_HIT_TYPE_OBJECT"},
								{sd_proj_hit_type_wall,							"",		"DIM3_PROJ_HIT_TYPE_WALL"},
								{sd_proj_hit_type_floor,						"",		"DIM3_PROJ_HIT_TYPE_FLOOR"},
								{sd_proj_hit_type_ceiling,						"",		"DIM3_PROJ_HIT_TYPE_CEILING"},
								{sd_proj_hit_type_projectile,					"",		"DIM3_PROJ_HIT_TYPE_PROJECTILE"},
								{sd_proj_hit_type_melee,						"",		"DIM3_PROJ_HIT_TYPE_MELEE"},
								{sd_proj_hit_type_auto,							"",		"DIM3_PROJ_HIT_TYPE_AUTO"},
								
								{sd_weap_crosshair_type_center,					"",		"DIM3_WEAPON_CROSSHAIR_TYPE_CENTER"},
								{sd_weap_crosshair_type_bone_tracking,			"",		"DIM3_WEAPON_CROSSHAIR_TYPE_BONE_TRACKING"},
								{sd_weap_crosshair_type_bone_tracking_resizing,	"",		"DIM3_WEAPON_CROSSHAIR_TYPE_BONE_TRACKING_RESIZING"},
								{sd_weap_crosshair_type_barrel_tracking,		"",		"DIM3_WEAPON_CROSSHAIR_TYPE_BARREL_TRACKING"},
								{sd_weap_crosshair_type_barrel_tracking_resizing,"",	"DIM3_WEAPON_CROSSHAIR_TYPE_BARREL_TRACKING_RESIZING"},

								{sd_camera_type_fpp,							"",		"DIM3_CAMERA_TYPE_FPP"},
								{sd_camera_type_chase,							"",		"DIM3_CAMERA_TYPE_CHASE"},
								{sd_camera_type_static,							"",		"DIM3_CAMERA_TYPE_STATIC"},
								{sd_camera_type_chase_static,					"",		"DIM3_CAMERA_TYPE_CHASE_STATIC"},
								
								{sd_input_mode_fpp,								"",		"DIM3_INPUT_MODE_FPP"},
								{sd_input_mode_side_scroll,						"",		"DIM3_INPUT_MODE_SIDE_SCROLL"},
								{sd_input_mode_top_down,						"",		"DIM3_INPUT_MODE_TOP_DOWN"},
								{sd_input_mode_fly,								"",		"DIM3_INPUT_MODE_FLY"},
								{sd_input_mode_thrust,							"",		"DIM3_INPUT_MODE_THRUST"},

								{sd_light_type_normal,							"",		"DIM3_LIGHT_TYPE_NORMAL"},
								{sd_light_type_blink,							"",		"DIM3_LIGHT_TYPE_BLINK"},
								{sd_light_type_glow,							"",		"DIM3_LIGHT_TYPE_GLOW"},
								{sd_light_type_pulse,							"",		"DIM3_LIGHT_TYPE_PULSE"},
								{sd_light_type_flicker,							"",		"DIM3_LIGHT_TYPE_FLICKER"},
								{sd_light_type_failing,							"",		"DIM3_LIGHT_TYPE_FAILING"},

								{sd_light_direction_all,						"",		"DIM3_LIGHT_DIRECTION_ALL"},
								{sd_light_direction_neg_x,						"",		"DIM3_LIGHT_DIRECTION_NEG_X"},
								{sd_light_direction_pos_x,						"",		"DIM3_LIGHT_DIRECTION_POS_X"},
								{sd_light_direction_neg_y,						"",		"DIM3_LIGHT_DIRECTION_NEG_Y"},
								{sd_light_direction_pos_y,						"",		"DIM3_LIGHT_DIRECTION_POS_Y"},
								{sd_light_direction_neg_z,						"",		"DIM3_LIGHT_DIRECTION_NEG_Z"},
								{sd_light_direction_pos_z,						"",		"DIM3_LIGHT_DIRECTION_POS_Z"},
								
								{sd_message_to_player,							"",		"DIM3_MESSAGE_TO_PLAYER"},
								{sd_message_to_object,							"",		"DIM3_MESSAGE_TO_OBJECT"},
								{sd_message_to_course,							"",		"DIM3_MESSAGE_TO_COURSE"},
								{sd_message_to_game,							"",		"DIM3_MESSAGE_TO_GAME"},
								
								{sd_stand_standing,								"",		"DIM3_STAND_STANDING"},
								{sd_stand_ducking,								"",		"DIM3_STAND_DUCKING"},
								{sd_stand_standing_up,							"",		"DIM3_STAND_STANDING_UP"},
								{sd_stand_ducking_down,							"",		"DIM3_STAND_DUCKING_DOWN"},
								
								{sd_air_up,										"",		"DIM3_AIR_UP"},
								{sd_air_down,									"",		"DIM3_AIR_DOWN"},
								{sd_air_falling,								"",		"DIM3_AIR_FALLING"},
								{sd_air_ground,									"",		"DIM3_AIR_GROUND"},

								{sd_liquid_out,									"",		"DIM3_LIQUID_OUT"},
								{sd_liquid_in,									"",		"DIM3_LIQUID_IN"},
								{sd_liquid_float,								"",		"DIM3_LIQUID_FLOAT"},
								{sd_liquid_under,								"",		"DIM3_LIQUID_UNDER"},
								
								{sd_projection_type_fov,						"",		"DIM3_PROJECTION_TYPE_FOV"},
								{sd_projection_type_frustum,					"",		"DIM3_PROJECTION_TYPE_FRUSTUM"},

								{sd_team_none,									"",		"DIM3_TEAM_NONE"},
								{sd_team_red,									"",		"DIM3_TEAM_RED"},
								{sd_team_blue,									"",		"DIM3_TEAM_BLUE"},
								{sd_team_common,								"",		"DIM3_TEAM_COMMON"},

								{sd_skill_easy,									"",		"DIM3_SKILL_EASY"},
								{sd_skill_medium,								"",		"DIM3_SKILL_MEDIUM"},
								{sd_skill_hard,									"",		"DIM3_SKILL_HARD"},
								
								{sd_spot_type_object,							"",		"DIM3_SPOT_TYPE_OBJECT"},
								{sd_spot_type_bot,								"",		"DIM3_SPOT_TYPE_BOT"},
								{sd_spot_type_player,							"",		"DIM3_SPOT_TYPE_PLAYER"},
								{sd_spot_type_spawn,							"",		"DIM3_SPOT_TYPE_SPAWN"},
								{sd_spot_type_checkpoint,						"",		"DIM3_SPOT_TYPE_CHECKPOINT"},
								
								{sd_object_type_player,							"",		"DIM3_OBJECT_TYPE_PLAYER"},
								{sd_object_type_bot_multiplayer,				"",		"DIM3_OBJECT_TYPE_BOT_MULTIPLAYER"},
								{sd_object_type_bot_map,						"",		"DIM3_OBJECT_TYPE_BOT_MAP"},
								{sd_object_type_object,							"",		"DIM3_OBJECT_TYPE_OBJECT"},
								{sd_object_type_remote_player,					"",		"DIM3_OBJECT_TYPE_REMOTE_PLAYER"},
								{sd_object_type_remote_object,					"",		"DIM3_OBJECT_TYPE_REMOTE_OBJECT"},
								
								{sd_collision_mode_cylinder,					"",		"DIM3_COLLISION_MODE_CYLINDER"},
								{sd_collision_mode_box,							"",		"DIM3_COLLISION_MODE_BOX"},
								{sd_collision_mode_simple,						"",		"DIM3_COLLISION_MODE_SIMPLE"},

									// main events
									
								{sd_event_construct,							"",		"DIM3_EVENT_CONSTRUCT"},
								{sd_event_spawn,								"",		"DIM3_EVENT_SPAWN"},
								{sd_event_die,									"",		"DIM3_EVENT_DIE"},
								{sd_event_telefrag,								"",		"DIM3_EVENT_TELEFRAG"},
								{sd_event_jump,									"",		"DIM3_EVENT_JUMP"},
								{sd_event_fall,									"",		"DIM3_EVENT_FALL"},
								{sd_event_land,									"",		"DIM3_EVENT_LAND"},
								{sd_event_collide,								"",		"DIM3_EVENT_COLLIDE"},
								{sd_event_liquid,								"",		"DIM3_EVENT_LIQUID"},
								{sd_event_map,									"",		"DIM3_EVENT_MAP"},
								{sd_event_animation_object,						"",		"DIM3_EVENT_ANIMATION_OBJECT"},
								{sd_event_animation_weapon,						"",		"DIM3_EVENT_ANIMATION_WEAPON"},
								{sd_event_timer,								"",		"DIM3_EVENT_TIMER"},
								{sd_event_wait,									"",		"DIM3_EVENT_WAIT"},
								{sd_event_message,								"",		"DIM3_EVENT_MESSAGE"},
								{sd_event_touch,								"",		"DIM3_EVENT_TOUCH"},
								{sd_event_hit,									"",		"DIM3_EVENT_HIT"},
								{sd_event_click,								"",		"DIM3_EVENT_CLICK"},
								{sd_event_damage,								"",		"DIM3_EVENT_DAMAGE"},
								{sd_event_crush,								"",		"DIM3_EVENT_CRUSH"},
								{sd_event_health,								"",		"DIM3_EVENT_HEALTH"},
								{sd_event_pickup,								"",		"DIM3_EVENT_PICKUP"},
								{sd_event_add_custom,							"",		"DIM3_EVENT_ADD_CUSTOM"},
								{sd_event_path,									"",		"DIM3_EVENT_PATH"},
								{sd_event_watch,								"",		"DIM3_EVENT_WATCH"},
								{sd_event_weapon_fire,							"",		"DIM3_EVENT_WEAPON_FIRE"},
								{sd_event_weapon_select,						"",		"DIM3_EVENT_WEAPON_SELECT"},
								{sd_event_projectile,							"",		"DIM3_EVENT_PROJECTILE"},
								{sd_event_melee,								"",		"DIM3_EVENT_MELEE"},
								{sd_event_vehicle,								"",		"DIM3_EVENT_VEHICLE"},
								{sd_event_move,									"",		"DIM3_EVENT_MOVE"},
								{sd_event_menu,									"",		"DIM3_EVENT_MENU"},
								{sd_event_chooser,								"",		"DIM3_EVENT_CHOOSER"},
								{sd_event_console,								"",		"DIM3_EVENT_CONSOLE"},
								{sd_event_interface,							"",		"DIM3_EVENT_INTERFACE"},
								{sd_event_state,								"",		"DIM3_EVENT_STATE"},
								{sd_event_remote,								"",		"DIM3_EVENT_REMOTE"},
								{sd_event_score,								"",		"DIM3_EVENT_SCORE"},
								{sd_event_dispose,								"",		"DIM3_EVENT_DISPOSE"},

									// sub events

								{sd_event_spawn_init,							"",		"DIM3_EVENT_SPAWN_INIT"},
								{sd_event_spawn_reborn,							"",		"DIM3_EVENT_SPAWN_REBORN"},
								{sd_event_spawn_game_reset,						"",		"DIM3_EVENT_SPAWN_GAME_RESET"},
								{sd_event_spawn_map_change,						"",		"DIM3_EVENT_SPAWN_MAP_CHANGE"},

								{sd_event_liquid_in,							"",		"DIM3_EVENT_LIQUID_IN"},
								{sd_event_liquid_out,							"",		"DIM3_EVENT_LIQUID_OUT"},
								{sd_event_liquid_submerge,						"",		"DIM3_EVENT_LIQUID_SUBMERGE"},
								{sd_event_liquid_surface,						"",		"DIM3_EVENT_LIQUID_SURFACE"},
								
								{sd_event_map_open,								"",		"DIM3_EVENT_MAP_OPEN"},
								{sd_event_map_close,							"",		"DIM3_EVENT_MAP_CLOSE"},
								
								{sd_event_animation_object_walk,				"",		"DIM3_EVENT_ANIMATION_OBJECT_WALK"},
								{sd_event_animation_object_run,					"",		"DIM3_EVENT_ANIMATION_OBJECT_RUN"},
								{sd_event_animation_object_crawl,				"",		"DIM3_EVENT_ANIMATION_OBJECT_CRAWL"},
								{sd_event_animation_object_walk_back,			"",		"DIM3_EVENT_ANIMATION_OBJECT_WALK_BACK"},
								{sd_event_animation_object_run_back,			"",		"DIM3_EVENT_ANIMATION_OBJECT_RUN_BACK"},
								{sd_event_animation_object_crawl_back,			"",		"DIM3_EVENT_ANIMATION_OBJECT_CRAWL_BACK"},

								{sd_event_animation_object_walk_left,			"",		"DIM3_EVENT_ANIMATION_OBJECT_WALK_LEFT"},
								{sd_event_animation_object_run_left,			"",		"DIM3_EVENT_ANIMATION_OBJECT_RUN_LEFT"},
								{sd_event_animation_object_crawl_left,			"",		"DIM3_EVENT_ANIMATION_OBJECT_CRAWL_LEFT"},
								{sd_event_animation_object_walk_left_forward,	"",		"DIM3_EVENT_ANIMATION_OBJECT_WALK_LEFT_FORWARD"},
								{sd_event_animation_object_run_left_forward,	"",		"DIM3_EVENT_ANIMATION_OBJECT_RUN_LEFT_FORWARD"},
								{sd_event_animation_object_crawl_left_forward,	"",		"DIM3_EVENT_ANIMATION_OBJECT_CRAWL_LEFT_FORWARD"},
								{sd_event_animation_object_walk_left_back,		"",		"DIM3_EVENT_ANIMATION_OBJECT_WALK_LEFT_BACK"},
								{sd_event_animation_object_run_left_back,		"",		"DIM3_EVENT_ANIMATION_OBJECT_RUN_LEFT_BACK"},
								{sd_event_animation_object_crawl_left_back,		"",		"DIM3_EVENT_ANIMATION_OBJECT_CRAWL_LEFT_BACK"},

								{sd_event_animation_object_walk_right,			"",		"DIM3_EVENT_ANIMATION_OBJECT_WALK_RIGHT"},
								{sd_event_animation_object_run_right,			"",		"DIM3_EVENT_ANIMATION_OBJECT_RUN_RIGHT"},
								{sd_event_animation_object_crawl_right,			"",		"DIM3_EVENT_ANIMATION_OBJECT_CRAWL_RIGHT"},
								{sd_event_animation_object_walk_right_forward,	"",		"DIM3_EVENT_ANIMATION_OBJECT_WALK_RIGHT_FORWARD"},
								{sd_event_animation_object_run_right_forward,	"",		"DIM3_EVENT_ANIMATION_OBJECT_RUN_RIGHT_FORWARD"},
								{sd_event_animation_object_crawl_right_forward,	"",		"DIM3_EVENT_ANIMATION_OBJECT_CRAWL_RIGHT_FORWARD"},
								{sd_event_animation_object_walk_right_back,		"",		"DIM3_EVENT_ANIMATION_OBJECT_WALK_RIGHT_BACK"},
								{sd_event_animation_object_run_right_back,		"",		"DIM3_EVENT_ANIMATION_OBJECT_RUN_RIGHT_BACK"},
								{sd_event_animation_object_crawl_right_back,	"",		"DIM3_EVENT_ANIMATION_OBJECT_CRAWL_RIGHT_BACK"},

								{sd_event_animation_object_turn_left,			"",		"DIM3_EVENT_ANIMATION_OBJECT_TURN_LEFT"},
								{sd_event_animation_object_turn_right,			"",		"DIM3_EVENT_ANIMATION_OBJECT_TURN_RIGHT"},
								{sd_event_animation_object_turn_stop,			"",		"DIM3_EVENT_ANIMATION_OBJECT_TURN_STOP"},

								{sd_event_animation_object_stop,				"",		"DIM3_EVENT_ANIMATION_OBJECT_STOP"},
								{sd_event_animation_object_fall,				"",		"DIM3_EVENT_ANIMATION_OBJECT_FALL"},
								{sd_event_animation_object_land,				"",		"DIM3_EVENT_ANIMATION_OBJECT_LAND"},
								{sd_event_animation_object_jump,				"",		"DIM3_EVENT_ANIMATION_OBJECT_JUMP"},
								{sd_event_animation_object_duck_down,			"",		"DIM3_EVENT_ANIMATION_OBJECT_DUCK_DOWN"},
								{sd_event_animation_object_ducking,				"",		"DIM3_EVENT_ANIMATION_OBJECT_DUCKING"},
								{sd_event_animation_object_stand_up,			"",		"DIM3_EVENT_ANIMATION_OBJECT_STAND_UP"},
								{sd_event_animation_object_standing,			"",		"DIM3_EVENT_ANIMATION_OBJECT_STANDING"},
								
								{sd_event_message_from_script,					"",		"DIM3_EVENT_MESSAGE_FROM_SCRIPT"},
								{sd_event_message_from_course,					"",		"DIM3_EVENT_MESSAGE_FROM_COURSE"},
								{sd_event_message_from_key_down,				"",		"DIM3_EVENT_MESSAGE_FROM_KEY_DOWN"},
								{sd_event_message_from_key_up,					"",		"DIM3_EVENT_MESSAGE_FROM_KEY_UP"},
								
								{sd_event_animation_weapon_raise,				"",		"DIM3_EVENT_ANIMATION_WEAPON_RAISE"},
								{sd_event_animation_weapon_lower,				"",		"DIM3_EVENT_ANIMATION_WEAPON_LOWER"},
								{sd_event_animation_weapon_held,				"",		"DIM3_EVENT_ANIMATION_WEAPON_HELD"},
								{sd_event_animation_weapon_add_ammo,			"",		"DIM3_EVENT_ANIMATION_WEAPON_ADD_AMMO"},
								{sd_event_animation_weapon_add_clip,			"",		"DIM3_EVENT_ANIMATION_WEAPON_ADD_CLIP"},
								{sd_event_animation_weapon_add_alt_ammo,		"",		"DIM3_EVENT_ANIMATION_WEAPON_ADD_ALT_AMMO"},
								{sd_event_animation_weapon_add_alt_clip,		"",		"DIM3_EVENT_ANIMATION_WEAPON_ADD_ALT_CLIP"},
								
								{sd_event_path_node,							"",		"DIM3_EVENT_PATH_NODE"},
								{sd_event_path_done,							"",		"DIM3_EVENT_PATH_DONE"},
								
								{sd_event_watch_object_near,					"",		"DIM3_EVENT_WATCH_OBJECT_NEAR"},
								{sd_event_watch_object_far,						"",		"DIM3_EVENT_WATCH_OBJECT_FAR"},
								{sd_event_watch_object_death,					"",		"DIM3_EVENT_WATCH_OBJECT_DEATH"},
								{sd_event_watch_object_enter_base,				"",		"DIM3_EVENT_WATCH_OBJECT_ENTER_BASE"},
								{sd_event_watch_object_exit_base,				"",		"DIM3_EVENT_WATCH_OBJECT_EXIT_BASE"},
								{sd_event_watch_object_sound,					"",		"DIM3_EVENT_WATCH_OBJECT_SOUND"},
								{sd_event_watch_object_damage,					"",		"DIM3_EVENT_WATCH_OBJECT_DAMAGE"},
								
								{sd_event_weapon_fire_single,					"",		"DIM3_EVENT_WEAPON_FIRE_SINGLE"},
								{sd_event_weapon_fire_up,						"",		"DIM3_EVENT_WEAPON_FIRE_UP"},
								{sd_event_weapon_fire_down,						"",		"DIM3_EVENT_WEAPON_FIRE_DOWN"},
								{sd_event_weapon_fire_repeat,					"",		"DIM3_EVENT_WEAPON_FIRE_REPEAT"},
								{sd_event_weapon_fire_clip_change,				"",		"DIM3_EVENT_WEAPON_FIRE_CLIP_CHANGE"},
								{sd_event_weapon_fire_zoom_enter,				"",		"DIM3_EVENT_WEAPON_FIRE_ZOOM_ENTER"},
								{sd_event_weapon_fire_zoom_exit,				"",		"DIM3_EVENT_WEAPON_FIRE_ZOOM_EXIT"},

								{sd_event_projectile_bounce,					"",		"DIM3_EVENT_PROJECTILE_BOUNCE"},
								{sd_event_projectile_reflect,					"",		"DIM3_EVENT_PROJECTILE_REFLECT"},

								{sd_event_melee_hit,							"",		"DIM3_EVENT_MELEE_HIT"},
								
								{sd_event_vehicle_enter,						"",		"DIM3_EVENT_VEHICLE_ENTER"},
								{sd_event_vehicle_exit,							"",		"DIM3_EVENT_VEHICLE_EXIT"},
								
								{sd_event_move_done,							"",		"DIM3_EVENT_MOVE_DONE"},
								{sd_event_move_loop,							"",		"DIM3_EVENT_MOVE_LOOP"},

								{sd_event_interface_title_done,					"",		"DIM3_EVENT_INTERFACE_TITLE_DONE"},
								{sd_event_interface_story_done,					"",		"DIM3_EVENT_INTERFACE_STORY_DONE"},
								{sd_event_interface_cinema_done,				"",		"DIM3_EVENT_INTERFACE_CINEMA_DONE"},

								{sd_event_state_load,							"",		"DIM3_EVENT_STATE_LOAD"},
								{sd_event_state_load_checkpoint,				"",		"DIM3_EVENT_STATE_LOAD_CHECKPOINT"},
								{sd_event_state_save,							"",		"DIM3_EVENT_STATE_SAVE"},
								{sd_event_state_save_checkpoint,				"",		"DIM3_EVENT_STATE_SAVE_CHECKPOINT"},

								{sd_event_remote_join,							"",		"DIM3_EVENT_REMOTE_JOIN"},
								{sd_event_remote_leave,							"",		"DIM3_EVENT_REMOTE_LEAVE"},
								{sd_event_remote_team,							"",		"DIM3_EVENT_REMOTE_TEAM"},
								{sd_event_remote_death,							"",		"DIM3_EVENT_REMOTE_DEATH"},
								{sd_event_remote_suicide,						"",		"DIM3_EVENT_REMOTE_SUICIDE"},
								{sd_event_remote_telefrag,						"",		"DIM3_EVENT_REMOTE_TELEFRAG"},
								
								{-1,"",""}};

/* =======================================================

      Add Defines to Scripts
      
======================================================= */

bool script_defines_is_number(char *str,bool *is_float)
{
	int			n,len;
	char		*c,ch;

	*is_float=FALSE;

	len=strlen(str);
	if (len==0) return(FALSE);

	c=str;

	for (n=0;n!=len;n++) {
		ch=*c++;
		if ((ch>='0') && (ch<='9')) continue;
		if (ch=='.') {
			*is_float=TRUE;
			continue;
		}
		
		return(FALSE);
	}

	return(TRUE);
}

void script_defines_create_constants_set(script_type *script,script_define_type *defines,bool user_defined)
{
	int					i;
	float				f;
	bool				is_float;
	script_define_type	*define;
	
		// run through the defines
		
	define=defines;
		
	while (TRUE) {
		if (define->value_int==-1) break;

			// determine how to set property
			// built in defines are always numbers, but
			// users can be strings or numbers

		if (!user_defined) {
			script_set_single_property(script->cx,script->global_obj,define->name,script_int_to_value(script->cx,define->value_int),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
		}
		else {
			if (script_defines_is_number(define->value_str,&is_float)) {
				if (!is_float) {
					i=atoi(define->value_str);
					script_set_single_property(script->cx,script->global_obj,define->name,script_int_to_value(script->cx,i),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
				}
				else {
					f=(float)atof(define->value_str);
					script_set_single_property(script->cx,script->global_obj,define->name,script_float_to_value(script->cx,f),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
				}
			}
			else {
				script_set_single_property(script->cx,script->global_obj,define->name,script_string_to_value(script->cx,define->value_str),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
			}
		}
		
		define++;
	}
}

void script_defines_create_constants(script_type *script)
{
	script_defines_create_constants_set(script,script_dim3_defines,FALSE);
	if (script_user_defines!=NULL) script_defines_create_constants_set(script,script_user_defines,TRUE);
}

/* =======================================================

      Check Defines
      
======================================================= */

bool script_is_prop_define(char *name)
{
	script_define_type	*define;

		// built in defines

	define=script_dim3_defines;
		
	while (TRUE) {
		if (define->value_int==-1) break;
		if (strcmp(define->name,name)==0) return(TRUE);
		define++;
	}

		// user defines

	if (script_user_defines==NULL) return(FALSE);

	define=script_user_defines;
		
	while (TRUE) {
		if (define->value_int==-1) break;
		if (strcmp(define->name,name)==0) return(TRUE);
		define++;
	}

	return(FALSE);
}

void script_get_define_for_event(int event,char *name)
{
	script_define_type	*define;

	name[0]=0x0;
	
	define=script_dim3_defines;
		
	while (TRUE) {
		if (define->value_int==-1) break;
		if (define->value_int==event) {
			strcpy(name,define->name);
			return;
		}
		define++;
	}
}

/* =======================================================

      User Defines
      
======================================================= */

bool script_load_user_defines_line(char *c,char *name,char *value_str)
{
	char			*c2,*c3,line[256];
	
	strncpy(line,c,256);
	line[255]=0x0;
	
	c2=strchr(line,'\r');
	if (c2!=NULL) *c2=0x0;
	c2=strchr(line,'\n');
	if (c2!=NULL) *c2=0x0;
	
	if (line[0]==0x0) return(FALSE);
		
		// is there a name value?
			
	c2=strrchr(line,'\t');
	if (c2==NULL) return(FALSE);
		
		// get name, value
		
	strncpy(name,line,64);
	name[63]=0x0;
	
	c3=strchr(name,'\t');
	if (c3!=NULL) *c3=0x0;
			
	strncpy(value_str,(c2+1),64);
	value_str[63]=0x0;
	
		// are they legal?
		
	return((name[0]!=0x0) && (value_str[0]!=0x0));
}

void script_load_user_defines(void)
{
    int					cnt,sz;
    char				path[1024],tmp_name[64],tmp_value[64];
	char				*c,*data;
	FILE				*file;
	struct stat			sb;
	script_define_type	*define;

	script_user_defines=NULL;

		// read defines file

	file_paths_data(&file_path_setup,path,"Scripts/Global","Defines","txt");
	
	file=fopen(path,"rb");
	if (file==NULL) return;
		
	stat(path,&sb);
	sz=sb.st_size;
	
	data=malloc(sz+1);
	if (data==NULL) return;
	
	fread(data,1,sz,file);
	fclose(file);
	
	data[sz]=0x0;

		// count returns

	cnt=0;
	c=data;
	
	while (TRUE) {
		if (script_load_user_defines_line(c,tmp_name,tmp_value)) cnt++;

		c=strchr(c,'\n');
		if (c==NULL) break;

		c++;
	}
	
		// create defines

	script_user_defines=malloc((cnt+1)*sizeof(script_define_type));
	if (script_user_defines==NULL) {
		free(data);
		return;
	}
	
	c=data;
	define=script_user_defines;
	
	while (TRUE) {
	
		if (script_load_user_defines_line(c,define->name,define->value_str)) {
			define->value_int=0;
			define++;
		}
			
		c=strchr(c,'\n');
		if (c==NULL) break;
		
		c++;
	}

	define->value_int=-1;

		// free file

	free(data);
}

void script_free_user_defines(void)
{
	if (script_user_defines!=NULL) free(script_user_defines);
}

