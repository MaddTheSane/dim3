/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.setting object

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
#include "scripts.h"

extern map_type				map;
extern server_type			server;
extern iface_type			iface;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

JSValueRef js_map_setting_get_gravity(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_setting_get_resistance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_setting_get_multiplayer(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_setting_get_multiplayerType(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_setting_get_botSkill(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_map_setting_set_gravity(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_map_setting_set_resistance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_map_get_parameter_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_set_ambient_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_clear_ambient_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_get_time_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		map_setting_props[]={
							{"gravity",				js_map_setting_get_gravity,				js_map_setting_set_gravity,			kJSPropertyAttributeDontDelete},
							{"resistance",			js_map_setting_get_resistance,			js_map_setting_set_resistance,		kJSPropertyAttributeDontDelete},
							{"multiplayer",			js_map_setting_get_multiplayer,			NULL,								kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"multiplayerType",		js_map_setting_get_multiplayerType,		NULL,								kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"botSkill",			js_map_setting_get_botSkill,			NULL,								kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	map_setting_functions[]={
							{"getParameter",		js_map_get_parameter_func,				kJSPropertyAttributeDontDelete},
							{"setAmbient",			js_map_set_ambient_func,				kJSPropertyAttributeDontDelete},
							{"clearAmbient",		js_map_clear_ambient_func,				kJSPropertyAttributeDontDelete},
							{"getTime",				js_map_get_time_func,					kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			map_setting_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_setting_object(void)
{
	map_setting_class=script_create_class("map_setting_class",map_setting_props,map_setting_functions);
}

void script_free_map_setting_object(void)
{
	script_free_class(map_setting_class);
}

JSObjectRef script_add_map_setting_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,map_setting_class,"setting",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_map_setting_get_gravity(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_float_to_value(cx,map.physics.gravity));
}

JSValueRef js_map_setting_get_resistance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_float_to_value(cx,map.physics.resistance));
}

JSValueRef js_map_setting_get_multiplayer(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_bool_to_value(cx,(net_setup.mode!=net_mode_none)));
}

JSValueRef js_map_setting_get_multiplayerType(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	if (net_setup.mode==net_mode_none) return(script_null_to_value(cx));

	return(script_string_to_value(cx,iface.multiplayer.game_list.games[net_setup.game_idx].name));
}

JSValueRef js_map_setting_get_botSkill(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,setup.network.bot.skill));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_map_setting_set_gravity(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.physics.gravity=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_map_setting_set_resistance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.physics.resistance=script_value_to_float(cx,vp);
	
	return(TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_map_get_parameter_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
    char			*c,str[256];
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
    idx=script_value_to_int(cx,argv[0]);
    if (idx<0) idx=0;
    
    c=map.settings.params;
    while (idx!=0) {
        c=strchr(c,'|');
        if (c==NULL) break;
        c++;
        idx--;
    }
    
    if (c==NULL) return(script_null_to_value(cx));
	
	strncpy(str,c,256);
	str[255]=0x0;
    
    c=strchr(str,'|');
    if (c!=NULL) *c=0x0;
    
    return(script_string_to_value(cx,str));
}

JSValueRef js_map_set_ambient_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			pitch;
	char			name[name_str_len];
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[0],name,name_str_len);
	pitch=script_value_to_float(cx,argv[1]);
	
	map_set_ambient(name,pitch);
	
	return(script_null_to_value(cx));
}

JSValueRef js_map_clear_ambient_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	map_clear_ambient();
	return(script_null_to_value(cx));
}

JSValueRef js_map_get_time_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
    return(script_int_to_value(cx,(game_time_get()-server.time.map_start_tick)));
}

