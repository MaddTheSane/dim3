/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.melee object

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
#include "scripts.h"
#include "objects.h"

extern iface_type		iface;
extern server_type		server;
extern js_type			js;

JSValueRef js_obj_melee_get_strikeBoneName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_melee_get_strikePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_melee_get_radius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_melee_get_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_melee_get_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_melee_get_force(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_melee_get_fallOff(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_melee_set_strikeBoneName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_melee_set_strikePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_melee_set_radius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_melee_set_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_melee_set_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_melee_set_force(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_melee_set_fallOff(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_melee_spawn_from_object_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_melee_spawn_from_touch_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_melee_props[]={
							{"strikeBoneName",			js_obj_melee_get_strikeBoneName,				js_obj_melee_set_strikeBoneName,	kJSPropertyAttributeDontDelete},
							{"strikePoseName",			js_obj_melee_get_strikePoseName,				js_obj_melee_set_strikePoseName,	kJSPropertyAttributeDontDelete},
							{"radius",					js_obj_melee_get_radius,						js_obj_melee_set_radius,			kJSPropertyAttributeDontDelete},
							{"distance",				js_obj_melee_get_distance,						js_obj_melee_set_distance,			kJSPropertyAttributeDontDelete},
							{"damage",					js_obj_melee_get_damage,						js_obj_melee_set_damage,			kJSPropertyAttributeDontDelete},
							{"force",					js_obj_melee_get_force,							js_obj_melee_set_force,				kJSPropertyAttributeDontDelete},
							{"fallOff",					js_obj_melee_get_fallOff,						js_obj_melee_set_fallOff,			kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	obj_melee_functions[]={
							{"spawnFromObjectBone",		js_obj_melee_spawn_from_object_bone_func,		kJSPropertyAttributeDontDelete},
							{"spawnFromTouch",			js_obj_melee_spawn_from_touch_func,				kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_melee_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_melee_object(void)
{
	obj_melee_class=script_create_class("obj_melee_class",obj_melee_props,obj_melee_functions);
}

void script_free_obj_melee_object(void)
{
	script_free_class(obj_melee_class);
}

JSObjectRef script_add_obj_melee_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_melee_class,"melee",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_melee_get_strikeBoneName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
    obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_string_to_value(cx,obj->melee.strike_bone_name));
}

JSValueRef js_obj_melee_get_strikePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
    obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_string_to_value(cx,obj->melee.strike_pose_name));
}

JSValueRef js_obj_melee_get_radius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
    obj_type			*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->melee.radius));
}

JSValueRef js_obj_melee_get_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
    obj_type			*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->melee.distance));
}

JSValueRef js_obj_melee_get_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
    obj_type			*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->melee.damage));
}

JSValueRef js_obj_melee_get_force(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
    obj_type			*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->melee.force));
}

JSValueRef js_obj_melee_get_fallOff(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
    obj_type			*obj;

	obj=object_get_attach(j_obj);
	return(script_bool_to_value(cx,obj->melee.fall_off));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_melee_set_strikeBoneName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;
	
	obj=object_get_attach(j_obj);
	script_value_to_string(cx,vp,obj->melee.strike_bone_name,name_str_len);
	
	return(TRUE);
}

bool js_obj_melee_set_strikePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;
	
	obj=object_get_attach(j_obj);
	script_value_to_string(cx,vp,obj->melee.strike_pose_name,name_str_len);

	return(TRUE);
}

bool js_obj_melee_set_radius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;
	
	obj=object_get_attach(j_obj);
	obj->melee.radius=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_melee_set_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;
	
	obj=object_get_attach(j_obj);
	obj->melee.distance=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_melee_set_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;
	
	obj=object_get_attach(j_obj);
	obj->melee.damage=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_melee_set_force(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;
	
	obj=object_get_attach(j_obj);
	obj->melee.force=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_melee_set_fallOff(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;

	obj=object_get_attach(j_obj);
	obj->melee.fall_off=script_value_to_bool(cx,vp);

	return(TRUE);
}

/* =======================================================

      Spawn Melee
      
======================================================= */

JSValueRef js_obj_melee_spawn_from_object_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char				err_str[256];
    obj_type			*obj;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);

	if (!melee_script_spawn_object_model(obj,NULL,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_obj_melee_spawn_from_touch_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char				err_str[256];
    obj_type			*obj;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);

	if (!melee_script_spawn_object_touch(obj,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}
