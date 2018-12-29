/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.sight object

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

extern iface_type		iface;
extern server_type		server;
extern js_type			js;

JSValueRef js_obj_sight_get_sideFieldAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_sight_get_lookFieldAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_sight_get_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_sight_set_sideFieldAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_sight_set_lookFieldAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_sight_set_sideFieldDivision(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_sight_set_lookFieldDivision(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_sight_set_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_sight_test_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_sight_test_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_sight_props[]={
							{"sideFieldAngle",		js_obj_sight_get_sideFieldAngle,	js_obj_sight_set_sideFieldAngle,		kJSPropertyAttributeDontDelete},
							{"lookFieldAngle",		js_obj_sight_get_lookFieldAngle,	js_obj_sight_set_lookFieldAngle,		kJSPropertyAttributeDontDelete},
							{"distance",			js_obj_sight_get_distance,			js_obj_sight_set_distance,				kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	obj_sight_functions[]={
							{"testObject",			js_obj_sight_test_object_func,		kJSPropertyAttributeDontDelete},
							{"testPlayer",			js_obj_sight_test_player_func,		kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_sight_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_sight_object(void)
{
	obj_sight_class=script_create_class("obj_sight_class",obj_sight_props,obj_sight_functions);
}

void script_free_obj_sight_object(void)
{
	script_free_class(obj_sight_class);
}

JSObjectRef script_add_obj_sight_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_sight_class,"sight",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_sight_get_sideFieldAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->sight.side_angle));
}

JSValueRef js_obj_sight_get_lookFieldAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->sight.look_angle));
}

JSValueRef js_obj_sight_get_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->sight.distance));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_sight_set_sideFieldAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->sight.side_angle=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_obj_sight_set_lookFieldAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->sight.look_angle=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_obj_sight_set_sideFieldDivision(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
// supergumba -- depreciated
	return(TRUE);
}

bool js_obj_sight_set_lookFieldDivision(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
// supergumba -- depreciated
	return(TRUE);
}

bool js_obj_sight_set_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->sight.distance=script_value_to_int(cx,vp);
	
	return(TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_obj_sight_test_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				id;
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);

	id=script_value_to_int(cx,argv[0]);
	return(script_bool_to_value(cx,object_sight_test_object(obj,id)));
}

JSValueRef js_obj_sight_test_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	return(script_bool_to_value(cx,object_sight_test_object(obj,server.player_obj_idx)));
}
