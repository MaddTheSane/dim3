/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.crosshair object

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

extern js_type			js;

JSValueRef js_weap_crosshair_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_crosshair_get_type(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_crosshair_get_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_crosshair_get_minSize(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_crosshair_get_maxSize(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_crosshair_get_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_crosshair_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_crosshair_set_type(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_crosshair_set_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_crosshair_set_minSize(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_crosshair_set_maxSize(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_crosshair_set_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		weap_crosshair_props[]={
							{"on",					js_weap_crosshair_get_on,			js_weap_crosshair_set_on,			kJSPropertyAttributeDontDelete},
							{"type",				js_weap_crosshair_get_type,			js_weap_crosshair_set_type,			kJSPropertyAttributeDontDelete},
							{"name",				js_weap_crosshair_get_name,			js_weap_crosshair_set_name,			kJSPropertyAttributeDontDelete},
							{"minSize",				js_weap_crosshair_get_minSize,		js_weap_crosshair_set_minSize,		kJSPropertyAttributeDontDelete},
							{"maxSize",				js_weap_crosshair_get_maxSize,		js_weap_crosshair_set_maxSize,		kJSPropertyAttributeDontDelete},
							{"distance",			js_weap_crosshair_get_distance,		js_weap_crosshair_set_distance,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			weap_crosshair_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_crosshair_object(void)
{
	weap_crosshair_class=script_create_class("weap_crosshair_class",weap_crosshair_props,NULL);
}

void script_free_weap_crosshair_object(void)
{
	script_free_class(weap_crosshair_class);
}

JSObjectRef script_add_weap_crosshair_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,weap_crosshair_class,"crosshair",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_crosshair_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_bool_to_value(cx,weap->crosshair.on));
}

JSValueRef js_weap_crosshair_get_type(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_int_to_value(cx,weap->crosshair.type+sd_weap_crosshair_type_center));
}

JSValueRef js_weap_crosshair_get_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_string_to_value(cx,weap->crosshair.fire_name));
}

JSValueRef js_weap_crosshair_get_minSize(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_int_to_value(cx,weap->crosshair.min_size));
}

JSValueRef js_weap_crosshair_get_maxSize(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_int_to_value(cx,weap->crosshair.max_size));
}

JSValueRef js_weap_crosshair_get_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_int_to_value(cx,weap->crosshair.distance));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_crosshair_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->crosshair.on=script_value_to_bool(cx,vp);

	return(TRUE);
}

bool js_weap_crosshair_set_type(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->crosshair.type=script_value_to_int(cx,vp)-sd_weap_crosshair_type_center;

	return(TRUE);
}

bool js_weap_crosshair_set_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	script_value_to_string(cx,vp,weap->crosshair.fire_name,name_str_len);
	weapon_attach_fire_crosshair(weap);

	return(TRUE);
}

bool js_weap_crosshair_set_minSize(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->crosshair.min_size=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_weap_crosshair_set_maxSize(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->crosshair.max_size=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_weap_crosshair_set_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->crosshair.distance=script_value_to_int(cx,vp);

	return(TRUE);
}



