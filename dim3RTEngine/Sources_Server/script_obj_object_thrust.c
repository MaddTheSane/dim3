/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.thrust object

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
#include "objects.h"

extern js_type			js;

JSValueRef js_obj_thrust_get_speed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_thrust_get_maxSpeed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_thrust_get_drag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_thrust_set_speed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_thrust_set_maxSpeed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_thrust_set_drag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		obj_thrust_props[]={
							{"speed",					js_obj_thrust_get_speed,			js_obj_thrust_set_speed,		kJSPropertyAttributeDontDelete},
							{"maxSpeed",				js_obj_thrust_get_maxSpeed,			js_obj_thrust_set_maxSpeed,		kJSPropertyAttributeDontDelete},
							{"drag",					js_obj_thrust_get_drag,				js_obj_thrust_set_drag,			kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			obj_thrust_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_thrust_object(void)
{
	obj_thrust_class=script_create_class("obj_thrust_class",obj_thrust_props,NULL);
}

void script_free_obj_thrust_object(void)
{
	script_free_class(obj_thrust_class);
}

JSObjectRef script_add_obj_thrust_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_thrust_class,"thrust",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_thrust_get_speed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->thrust.speed));
}

JSValueRef js_obj_thrust_get_maxSpeed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->thrust.max_speed));
}

JSValueRef js_obj_thrust_get_drag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_bool_to_value(cx,obj->thrust.drag));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_thrust_set_speed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->thrust.speed=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_obj_thrust_set_maxSpeed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->thrust.max_speed=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_obj_thrust_set_drag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->thrust.drag=script_value_to_bool(cx,vp);
	
	return(TRUE);
}


