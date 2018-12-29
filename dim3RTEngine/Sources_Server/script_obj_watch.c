/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.watch object

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
#include "objects.h"

extern server_type		server;
extern js_type			js;

JSValueRef js_obj_watch_get_objectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_watch_get_objectName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_watch_get_objectIsPlayer(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_watch_get_objectIsRemote(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_watch_get_objectIsBot(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_watch_get_objectIsMapBot(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_watch_get_objectIsPlayerRemoteBot(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_watch_get_objectIsPlayerRemoteBotMapBot(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_watch_get_objectTeam(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_watch_get_baseTeam(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_watch_get_soundName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_watch_start_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_watch_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_watch_set_restrict_sight_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_watch_clear_restrict_sight_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_watch_props[]={
							{"objectId",						js_obj_watch_get_objectId,							NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectName",						js_obj_watch_get_objectName,						NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectIsPlayer",					js_obj_watch_get_objectIsPlayer,					NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectIsRemote",					js_obj_watch_get_objectIsRemote,					NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectIsBot",						js_obj_watch_get_objectIsBot,						NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectIsMapBot",					js_obj_watch_get_objectIsMapBot,					NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectIsPlayerRemoteBot",			js_obj_watch_get_objectIsPlayerRemoteBot,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectIsPlayerRemoteBotMapBot",	js_obj_watch_get_objectIsPlayerRemoteBotMapBot,		NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectTeam",						js_obj_watch_get_objectTeam,						NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"baseTeam",						js_obj_watch_get_baseTeam,							NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"soundName",						js_obj_watch_get_soundName,							NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	obj_watch_functions[]={
							{"start",							js_obj_watch_start_func,					kJSPropertyAttributeDontDelete},
							{"stop",							js_obj_watch_stop_func,						kJSPropertyAttributeDontDelete},
							{"setRestrictSight",				js_obj_watch_set_restrict_sight_func,		kJSPropertyAttributeDontDelete},
							{"clearRestrictSight",				js_obj_watch_clear_restrict_sight_func,		kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_watch_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_watch_object(void)
{
	obj_watch_class=script_create_class("obj_watch_class",obj_watch_props,obj_watch_functions);
}

void script_free_obj_watch_object(void)
{
	script_free_class(obj_watch_class);
}

JSObjectRef script_add_obj_watch_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_watch_class,"watch",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_watch_get_objectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->watch.obj_idx));
}

JSValueRef js_obj_watch_get_objectName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj,*watch_obj;

	obj=object_get_attach(j_obj);
	
	watch_obj=server.obj_list.objs[obj->watch.obj_idx];
	if (watch_obj==NULL) return(script_null_to_value(cx));

	return(script_string_to_value(cx,watch_obj->name));
}

JSValueRef js_obj_watch_get_objectIsPlayer(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_bool_to_value(cx,obj->watch.obj_idx==server.player_obj_idx));
}

JSValueRef js_obj_watch_get_objectIsRemote(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj,*watch_obj;

	obj=object_get_attach(j_obj);
	
	watch_obj=server.obj_list.objs[obj->watch.obj_idx];
	if (watch_obj==NULL) return(script_bool_to_value(cx,FALSE));
	
	return(script_bool_to_value(cx,watch_obj->type==object_type_remote_player));
}

JSValueRef js_obj_watch_get_objectIsBot(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj,*watch_obj;

	obj=object_get_attach(j_obj);
	
	watch_obj=server.obj_list.objs[obj->watch.obj_idx];
	if (watch_obj==NULL) return(script_bool_to_value(cx,FALSE));
	
	return(script_bool_to_value(cx,(watch_obj->type==object_type_bot_multiplayer)));
}

JSValueRef js_obj_watch_get_objectIsMapBot(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj,*watch_obj;

	obj=object_get_attach(j_obj);
	
	watch_obj=server.obj_list.objs[obj->watch.obj_idx];
	if (watch_obj==NULL) return(script_bool_to_value(cx,FALSE));
	
	return(script_bool_to_value(cx,(watch_obj->type==object_type_bot_map)));
}

JSValueRef js_obj_watch_get_objectIsPlayerRemoteBot(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj,*watch_obj;

	obj=object_get_attach(j_obj);
	
	if (obj->watch.obj_idx==server.player_obj_idx) return(script_bool_to_value(cx,TRUE));

	watch_obj=server.obj_list.objs[obj->watch.obj_idx];
	if (watch_obj==NULL) return(script_bool_to_value(cx,FALSE));
	
	return(script_bool_to_value(cx,(watch_obj->type==object_type_remote_player) || (watch_obj->type==object_type_bot_multiplayer)));
}

JSValueRef js_obj_watch_get_objectIsPlayerRemoteBotMapBot(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj,*watch_obj;

	obj=object_get_attach(j_obj);
	
	if (obj->watch.obj_idx==server.player_obj_idx) return(script_bool_to_value(cx,TRUE));

	watch_obj=server.obj_list.objs[obj->watch.obj_idx];
	if (watch_obj==NULL) return(script_bool_to_value(cx,FALSE));
	
	return(script_bool_to_value(cx,(watch_obj->type==object_type_remote_player) || (watch_obj->type==object_type_bot_multiplayer) || (watch_obj->type==object_type_bot_map)));
}

JSValueRef js_obj_watch_get_objectTeam(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj,*watch_obj;

	obj=object_get_attach(j_obj);
	
	watch_obj=server.obj_list.objs[obj->watch.obj_idx];
	if (watch_obj==NULL) return(script_int_to_value(cx,sd_team_none));
	
	return(script_int_to_value(cx,watch_obj->team_idx+sd_team_none));
}

JSValueRef js_obj_watch_get_baseTeam(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->watch.base_team+sd_team_none));
}

JSValueRef js_obj_watch_get_soundName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_string_to_value(cx,obj->watch.sound_name));
}

/* =======================================================

      Start/Stop/Filter Functions
      
======================================================= */

JSValueRef js_obj_watch_start_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	
	obj->watch.on=TRUE;
	obj->watch.dist=script_value_to_int(cx,argv[0]);
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_watch_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	object_clear_watch(&obj->watch);

	return(script_null_to_value(cx));
}

/* =======================================================

      Sight Restriction Functions
      
======================================================= */

JSValueRef js_obj_watch_set_restrict_sight_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	
	obj->watch.watch_restrict.on=TRUE;
	obj->watch.watch_restrict.ang=script_value_to_float(cx,argv[0]);
	obj->watch.watch_restrict.ray_trace=script_value_to_bool(cx,argv[1]);
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_watch_clear_restrict_sight_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	
	obj->watch.watch_restrict.on=FALSE;
	
	return(script_null_to_value(cx));
}
