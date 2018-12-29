/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.angle object

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

extern void object_setup_motion(obj_type *obj,float ang,float speed);

JSValueRef js_obj_angle_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_angle_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_angle_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_angle_rotate_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_angle_rotateTo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_angle_rotateToObject_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_angle_rotateToPlayer_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_angle_props[]={
							{"x",					js_obj_angle_get_x,					NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"y",					js_obj_angle_get_y,					NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"z",					js_obj_angle_get_z,					NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	obj_angle_functions[]={
							{"rotate",				js_obj_angle_rotate_func,				kJSPropertyAttributeDontDelete},
							{"rotateTo",			js_obj_angle_rotateTo_func,				kJSPropertyAttributeDontDelete},
							{"rotateToObject",		js_obj_angle_rotateToObject_func,		kJSPropertyAttributeDontDelete},
							{"rotateToPlayer",		js_obj_angle_rotateToPlayer_func,		kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_angle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_angle_object(void)
{
	obj_angle_class=script_create_class("obj_angle_class",obj_angle_props,obj_angle_functions);
}

void script_free_obj_angle_object(void)
{
	script_free_class(obj_angle_class);
}

JSObjectRef script_add_obj_angle_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_angle_class,"angle",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_angle_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->ang.x));
}

JSValueRef js_obj_angle_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->ang.y));
}

JSValueRef js_obj_angle_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->ang.z));
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_obj_angle_rotate_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			ang_add;
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);

	ang_add=script_value_to_float(cx,argv[0]);
	obj->ang.y=angle_add(obj->ang.y,ang_add);

	return(script_null_to_value(cx));
}

JSValueRef js_obj_angle_rotateTo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	obj->ang.y=script_value_to_float(cx,argv[0]);

	return(script_null_to_value(cx));
}

JSValueRef js_obj_angle_rotateToObject_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj,*obj2;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
		// objects
	
	obj=object_get_attach(j_obj);
	
	obj2=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj2==NULL) return(script_null_to_value(cx));
	
		// rotate to
		// only do x/z if this object is flying
		
	obj->ang.y=angle_find(obj->pnt.x,obj->pnt.z,obj2->pnt.x,obj2->pnt.z);
		
	if (obj->fly) {
		obj->ang.x=angle_find(obj->pnt.y,obj->pnt.z,obj2->pnt.y,obj2->pnt.z);
		obj->ang.z=angle_find(obj->pnt.x,obj->pnt.y,obj2->pnt.x,obj2->pnt.y);
	}
		
	return(script_null_to_value(cx));
}

JSValueRef js_obj_angle_rotateToPlayer_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj,*obj2;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
		// objects
	
	obj=object_get_attach(j_obj);
	obj2=server.obj_list.objs[server.player_obj_idx];
	
		// rotate to
		// only do x/z if this object is flying
		
	obj->ang.y=angle_find(obj->pnt.x,obj->pnt.z,obj2->pnt.x,obj2->pnt.z);
		
	if (obj->fly) {
		obj->ang.x=angle_find(obj->pnt.y,obj->pnt.z,obj2->pnt.y,obj2->pnt.z);
		obj->ang.z=angle_find(obj->pnt.x,obj->pnt.y,obj2->pnt.x,obj2->pnt.y);
	}
		
	return(script_null_to_value(cx));
}

