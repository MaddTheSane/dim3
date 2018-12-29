/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.size object

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
extern js_type			js;

JSValueRef js_obj_size_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_size_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_size_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_size_get_eyeOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_size_get_clickDistance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_size_get_nodeTouchDistance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_size_get_weight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_size_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_size_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_size_set_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_size_set_eyeOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_size_set_clickDistance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_size_set_nodeTouchDistance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_size_set_weight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_size_grow_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_size_grow_over_time_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_size_grow_over_time_change_size_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_size_grow_over_time_change_offset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_size_props[]={
							{"x",					js_obj_size_get_x,					js_obj_size_set_x,					kJSPropertyAttributeDontDelete},
							{"y",					js_obj_size_get_y,					js_obj_size_set_y,					kJSPropertyAttributeDontDelete},
							{"z",					js_obj_size_get_z,					js_obj_size_set_z,					kJSPropertyAttributeDontDelete},
							{"eyeOffset",			js_obj_size_get_eyeOffset,			js_obj_size_set_eyeOffset,			kJSPropertyAttributeDontDelete},
							{"clickDistance",		js_obj_size_get_clickDistance,		js_obj_size_set_clickDistance,		kJSPropertyAttributeDontDelete},
							{"nodeTouchDistance",	js_obj_size_get_nodeTouchDistance,	js_obj_size_set_nodeTouchDistance,	kJSPropertyAttributeDontDelete},
							{"weight",				js_obj_size_get_weight,				js_obj_size_set_weight,				kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	obj_size_functions[]={
							{"growTo",						js_obj_size_grow_to_func,						kJSPropertyAttributeDontDelete},
							{"growOverTime",				js_obj_size_grow_over_time_func,				kJSPropertyAttributeDontDelete},
							{"growOverTimeChangeSize",		js_obj_size_grow_over_time_change_size_func,	kJSPropertyAttributeDontDelete},
							{"growOverTimeChangeOffset",	js_obj_size_grow_over_time_change_offset_func,	kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_size_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_size_object(void)
{
	obj_size_class=script_create_class("obj_size_class",obj_size_props,obj_size_functions);
}

void script_free_obj_size_object(void)
{
	script_free_class(obj_size_class);
}

JSObjectRef script_add_obj_size_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_size_class,"size",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_size_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->size.x));
}

JSValueRef js_obj_size_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->size.y));
}

JSValueRef js_obj_size_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->size.z));
}

JSValueRef js_obj_size_get_eyeOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->size.eye_offset));
}

JSValueRef js_obj_size_get_clickDistance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->click.distance));
}

JSValueRef js_obj_size_get_nodeTouchDistance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->size.node_touch_dist));
}

JSValueRef js_obj_size_get_weight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->size.weight));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_size_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->size.x=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_obj_size_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->size.y=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_obj_size_set_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->size.z=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_obj_size_set_eyeOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->size.eye_offset=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_obj_size_set_clickDistance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->click.distance=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_obj_size_set_nodeTouchDistance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->size.node_touch_dist=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_obj_size_set_weight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->size.weight=script_value_to_int(cx,vp);

	return(TRUE);
}

/* =======================================================

      Function
      
======================================================= */

JSValueRef js_obj_size_grow_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			resize;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	resize=script_value_to_float(cx,argv[0]);
	
	obj=object_get_attach(j_obj);
	object_grow_direct(obj,resize);
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_size_grow_over_time_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				msec;
	float			resize;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	resize=script_value_to_float(cx,argv[0]);
	msec=script_value_to_int(cx,argv[1]);
	
	obj=object_get_attach(j_obj);
	object_grow_start(obj,msec,resize,NULL,NULL);
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_size_grow_over_time_change_size_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				msec;
	float			resize;
	d3pnt			size;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
		
	resize=script_value_to_float(cx,argv[0]);
	script_value_to_point(cx,argv[1],&size);
	msec=script_value_to_int(cx,argv[2]);
	
	obj=object_get_attach(j_obj);
	object_grow_start(obj,msec,resize,&size,NULL);

	return(script_null_to_value(cx));
}

JSValueRef js_obj_size_grow_over_time_change_offset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				msec;
	float			resize;
	d3pnt			offset;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
		
	resize=script_value_to_float(cx,argv[0]);
	script_value_to_point(cx,argv[1],&offset);
	msec=script_value_to_int(cx,argv[2]);
	
	obj=object_get_attach(j_obj);
	object_grow_start(obj,msec,resize,NULL,&offset);

	return(script_null_to_value(cx));
}
