/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.lock object

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

extern map_type			map;
extern server_type		server;
extern js_type			js;

JSValueRef js_obj_lock_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_lock_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_lock_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_lock_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_lock_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_lock_set_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		obj_lock_props[]={
							{"x",					js_obj_lock_get_x,					js_obj_lock_set_x,		kJSPropertyAttributeDontDelete},
							{"y",					js_obj_lock_get_y,					js_obj_lock_set_y,		kJSPropertyAttributeDontDelete},
							{"z",					js_obj_lock_get_z,					js_obj_lock_set_z,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSClassRef			obj_lock_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_lock_object(void)
{
	obj_lock_class=script_create_class("obj_lock_class",obj_lock_props,NULL);
}

void script_free_obj_lock_object(void)
{
	script_free_class(obj_lock_class);
}

JSObjectRef script_add_obj_lock_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_lock_class,"lock",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_lock_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_bool_to_value(cx,obj->lock.x));
}

JSValueRef js_obj_lock_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_bool_to_value(cx,obj->lock.y));
}

JSValueRef js_obj_lock_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_bool_to_value(cx,obj->lock.z));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_lock_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->lock.x=script_value_to_bool(cx,vp);
	
	return(TRUE);
}

bool js_obj_lock_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->lock.y=script_value_to_bool(cx,vp);
	
	return(TRUE);
}

bool js_obj_lock_set_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->lock.z=script_value_to_bool(cx,vp);
	
	return(TRUE);
}

