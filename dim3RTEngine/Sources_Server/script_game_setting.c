/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: game.setting object

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

#include "scripts.h"

extern map_type				map;
extern server_type			server;
extern iface_type			iface;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

JSValueRef js_game_setting_get_type(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_game_setting_get_multiplayer(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_game_setting_get_skill(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_game_setting_get_simple_save_id(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);

JSStaticValue 		game_setting_props[]={
							{"type",				js_game_setting_get_type,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"multiplayer",			js_game_setting_get_multiplayer,		NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"skill",				js_game_setting_get_skill,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"simpleSaveId",		js_game_setting_get_simple_save_id,		NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			game_setting_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_game_setting_object(void)
{
	game_setting_class=script_create_class("game_setting_class",game_setting_props,NULL);
}

void script_free_game_setting_object(void)
{
	script_free_class(game_setting_class);
}

JSObjectRef script_add_game_setting_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,game_setting_class,"setting",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_game_setting_get_type(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	if (net_setup.mode==net_mode_none) return(script_null_to_value(cx));
	
	return(script_string_to_value(cx,iface.multiplayer.game_list.games[net_setup.game_idx].name));
}

JSValueRef js_game_setting_get_multiplayer(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_bool_to_value(cx,(net_setup.mode!=net_mode_none)));
}

JSValueRef js_game_setting_get_skill(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,server.skill));
}

JSValueRef js_game_setting_get_simple_save_id(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,iface.simple_save_list.saves[server.simple_save_idx].save_id));
}

