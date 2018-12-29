/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.sideSpeed object

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

extern js_type			js;

JSValueRef js_obj_side_speed_get_walk(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_side_speed_get_run(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_side_speed_get_crawl(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_side_speed_get_air(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_side_speed_get_acceleration(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_side_speed_get_deceleration(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_side_speed_get_accelerationAir(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_side_speed_get_decelerationAir(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_side_speed_set_walk(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_side_speed_set_run(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_side_speed_set_crawl(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_side_speed_set_air(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_side_speed_set_acceleration(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_side_speed_set_deceleration(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_side_speed_set_accelerationAir(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_side_speed_set_decelerationAir(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		obj_side_speed_props[]={
							{"walk",					js_obj_side_speed_get_walk,				js_obj_side_speed_set_walk,					kJSPropertyAttributeDontDelete},
							{"run",						js_obj_side_speed_get_run,				js_obj_side_speed_set_run,					kJSPropertyAttributeDontDelete},
							{"crawl",					js_obj_side_speed_get_crawl,			js_obj_side_speed_set_crawl,				kJSPropertyAttributeDontDelete},
							{"air",						js_obj_side_speed_get_air,				js_obj_side_speed_set_air,					kJSPropertyAttributeDontDelete},
							{"acceleration",			js_obj_side_speed_get_acceleration,		js_obj_side_speed_set_acceleration,			kJSPropertyAttributeDontDelete},
							{"deceleration",			js_obj_side_speed_get_deceleration,		js_obj_side_speed_set_deceleration,			kJSPropertyAttributeDontDelete},
							{"accelerationAir",			js_obj_side_speed_get_accelerationAir,	js_obj_side_speed_set_accelerationAir,		kJSPropertyAttributeDontDelete},
							{"decelerationAir",			js_obj_side_speed_get_decelerationAir,	js_obj_side_speed_set_decelerationAir,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			obj_side_speed_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_side_speed_object(void)
{
	obj_side_speed_class=script_create_class("obj_side_speed_class",obj_side_speed_props,NULL);
}

void script_free_obj_side_speed_object(void)
{
	script_free_class(obj_side_speed_class);
}

JSObjectRef script_add_obj_side_speed_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_side_speed_class,"sideSpeed",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_side_speed_get_walk(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->side_move.max_walk_speed));
}

JSValueRef js_obj_side_speed_get_run(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->side_move.max_run_speed));
}

JSValueRef js_obj_side_speed_get_crawl(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->side_move.max_crawl_speed));
}

JSValueRef js_obj_side_speed_get_air(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->side_move.max_air_speed));
}

JSValueRef js_obj_side_speed_get_acceleration(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->side_move.accelerate));
}

JSValueRef js_obj_side_speed_get_deceleration(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->side_move.decelerate));
}

JSValueRef js_obj_side_speed_get_accelerationAir(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->side_move.air_accelerate));
}

JSValueRef js_obj_side_speed_get_decelerationAir(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->side_move.air_decelerate));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_side_speed_set_walk(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->side_move.max_walk_speed=fabsf(script_value_to_float(cx,vp));
	
	return(TRUE);
}

bool js_obj_side_speed_set_run(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->side_move.max_run_speed=fabsf(script_value_to_float(cx,vp));
	
	return(TRUE);
}

bool js_obj_side_speed_set_crawl(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->side_move.max_crawl_speed=fabsf(script_value_to_float(cx,vp));
	
	return(TRUE);
}

bool js_obj_side_speed_set_air(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->side_move.max_air_speed=fabsf(script_value_to_float(cx,vp));
	
	return(TRUE);
}

bool js_obj_side_speed_set_acceleration(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->side_move.accelerate=fabsf(script_value_to_float(cx,vp));
	
	return(TRUE);
}

bool js_obj_side_speed_set_deceleration(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->side_move.decelerate=fabsf(script_value_to_float(cx,vp));
	
	return(TRUE);
}

bool js_obj_side_speed_set_accelerationAir(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->side_move.air_accelerate=fabsf(script_value_to_float(cx,vp));
	
	return(TRUE);
}

bool js_obj_side_speed_set_decelerationAir(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->side_move.air_decelerate=fabsf(script_value_to_float(cx,vp));
	
	return(TRUE);
}

