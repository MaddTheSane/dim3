/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.touch object

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

JSValueRef js_obj_touch_get_objectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_touch_get_objectName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_touch_get_objectIsPlayer(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_touch_get_objectIsRemote(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_touch_get_objectIsBot(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_touch_get_objectIsPlayerRemoteBot(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_touch_get_stand(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);

JSStaticValue 		obj_touch_props[]={
							{"objectId",					js_obj_touch_get_objectId,					NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectName",					js_obj_touch_get_objectName,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectIsPlayer",				js_obj_touch_get_objectIsPlayer,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectIsRemote",				js_obj_touch_get_objectIsRemote,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectIsBot",					js_obj_touch_get_objectIsBot,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectIsPlayerRemoteBot",		js_obj_touch_get_objectIsPlayerRemoteBot,	NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"stand",						js_obj_touch_get_stand,						NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			obj_touch_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_touch_object(void)
{
	obj_touch_class=script_create_class("obj_touch_class",obj_touch_props,NULL);
}

void script_free_obj_touch_object(void)
{
	script_free_class(obj_touch_class);
}

JSObjectRef script_add_obj_touch_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_touch_class,"touch",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_touch_get_objectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->touch.obj_idx));
}

JSValueRef js_obj_touch_get_objectName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj,*touch_obj;

	obj=object_get_attach(j_obj);
	if (obj->touch.obj_idx==-1) return(script_null_to_value(cx));
	
	touch_obj=server.obj_list.objs[obj->touch.obj_idx];
	return(script_string_to_value(cx,touch_obj->name));
}

JSValueRef js_obj_touch_get_objectIsPlayer(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	if (obj->touch.obj_idx==-1) return(script_bool_to_value(cx,FALSE));

	return(script_bool_to_value(cx,obj->touch.obj_idx==server.player_obj_idx));
}

JSValueRef js_obj_touch_get_objectIsRemote(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj,*touch_obj;

	obj=object_get_attach(j_obj);
	if (obj->touch.obj_idx==-1) return(script_bool_to_value(cx,FALSE));
	
	touch_obj=server.obj_list.objs[obj->touch.obj_idx];
	if (touch_obj==NULL) return(script_bool_to_value(cx,FALSE));

	return(script_bool_to_value(cx,touch_obj->type==object_type_remote_player));
}

JSValueRef js_obj_touch_get_objectIsBot(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj,*touch_obj;

	obj=object_get_attach(j_obj);
	if (obj->touch.obj_idx==-1) return(script_bool_to_value(cx,FALSE));
	
	touch_obj=server.obj_list.objs[obj->touch.obj_idx];
	if (touch_obj==NULL) return(script_bool_to_value(cx,FALSE));
	
	return(script_bool_to_value(cx,(touch_obj->type==object_type_bot_multiplayer)));
}

JSValueRef js_obj_touch_get_objectIsPlayerRemoteBot(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj,*touch_obj;

	obj=object_get_attach(j_obj);
	if (obj->touch.obj_idx==-1) return(script_bool_to_value(cx,FALSE));
	
	if (obj->touch.obj_idx==server.player_obj_idx) return(script_bool_to_value(cx,TRUE));
	
	touch_obj=server.obj_list.objs[obj->touch.obj_idx];
	if (touch_obj==NULL) return(script_bool_to_value(cx,FALSE));
	
	return(script_bool_to_value(cx,(touch_obj->type==object_type_remote_player) || (touch_obj->type==object_type_bot_multiplayer)));
}

JSValueRef js_obj_touch_get_stand(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	if (obj->touch.obj_idx==-1) return(script_bool_to_value(cx,FALSE));

	return(script_bool_to_value(cx,obj->touch.stand));
}







