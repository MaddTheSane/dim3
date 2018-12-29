/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.objectSpeed object

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

JSValueRef js_obj_object_speed_get_jumpHeight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_object_speed_get_duckAdd(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_object_speed_get_duckChange(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_object_speed_get_bumpHeight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_object_speed_get_bumpSmoothFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_object_speed_get_bounceFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_object_speed_get_noSlide(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_object_speed_set_jumpHeight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_object_speed_set_duckAdd(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_object_speed_set_duckChange(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_object_speed_set_bumpHeight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_object_speed_set_bumpSmoothFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_object_speed_set_bounceFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_object_speed_set_noSlide(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		obj_object_speed_props[]={
							{"jumpHeight",				js_obj_object_speed_get_jumpHeight,			js_obj_object_speed_set_jumpHeight,			kJSPropertyAttributeDontDelete},
							{"duckAdd",					js_obj_object_speed_get_duckAdd,			js_obj_object_speed_set_duckAdd,			kJSPropertyAttributeDontDelete},
							{"duckChange",				js_obj_object_speed_get_duckChange,			js_obj_object_speed_set_duckChange,			kJSPropertyAttributeDontDelete},
							{"bumpHeight",				js_obj_object_speed_get_bumpHeight,			js_obj_object_speed_set_bumpHeight,			kJSPropertyAttributeDontDelete},
							{"bumpSmoothFactor",		js_obj_object_speed_get_bumpSmoothFactor,	js_obj_object_speed_set_bumpSmoothFactor,	kJSPropertyAttributeDontDelete},
							{"bounceFactor",			js_obj_object_speed_get_bounceFactor,		js_obj_object_speed_set_bounceFactor,		kJSPropertyAttributeDontDelete},
							{"noSlide",					js_obj_object_speed_get_noSlide,			js_obj_object_speed_set_noSlide,			kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			obj_object_speed_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_object_speed_object(void)
{
	obj_object_speed_class=script_create_class("obj_object_speed_class",obj_object_speed_props,NULL);
}

void script_free_obj_object_speed_object(void)
{
	script_free_class(obj_object_speed_class);
}

JSObjectRef script_add_obj_object_speed_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_object_speed_class,"objectSpeed",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_object_speed_get_jumpHeight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->jump.y_add));
}

JSValueRef js_obj_object_speed_get_duckAdd(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->duck.y_add));
}

JSValueRef js_obj_object_speed_get_duckChange(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->duck.y_change));
}

JSValueRef js_obj_object_speed_get_bumpHeight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->bump.high));
}

JSValueRef js_obj_object_speed_get_bumpSmoothFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->bump.smooth_factor));
}

JSValueRef js_obj_object_speed_get_bounceFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->bounce.factor));
}

JSValueRef js_obj_object_speed_get_noSlide(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_bool_to_value(cx,obj->no_slide));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_object_speed_set_jumpHeight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->jump.y_add=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_object_speed_set_duckAdd(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->duck.y_add=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_object_speed_set_duckChange(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->duck.y_change=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_object_speed_set_bumpHeight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->bump.high=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_object_speed_set_bumpSmoothFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->bump.smooth_factor=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_obj_object_speed_set_bounceFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->bounce.factor=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_obj_object_speed_set_noSlide(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->no_slide=script_value_to_bool(cx,vp);
	
	return(TRUE);
}


