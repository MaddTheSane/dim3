/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.hitPosition object

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

JSValueRef js_obj_hit_position_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_hit_position_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_hit_position_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);

JSStaticValue 		obj_hit_position_props[]={
							{"x",					js_obj_hit_position_get_x,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"y",					js_obj_hit_position_get_y,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"z",					js_obj_hit_position_get_z,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			obj_hit_position_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_hit_position_object(void)
{
	obj_hit_position_class=script_create_class("obj_hit_position_class",obj_hit_position_props,NULL);
}

void script_free_obj_hit_position_object(void)
{
	script_free_class(obj_hit_position_class);
}

JSObjectRef script_add_obj_hit_position_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_hit_position_class,"hitPosition",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_hit_position_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type			*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->hit.pnt.x));
}

JSValueRef js_obj_hit_position_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type			*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->hit.pnt.y));
}

JSValueRef js_obj_hit_position_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type			*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->hit.pnt.z));
}

