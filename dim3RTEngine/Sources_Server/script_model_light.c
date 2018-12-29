/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.light object

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

JSValueRef js_model_light_get_index(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_light_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_light_get_type(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_light_get_direction(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_light_get_intensity(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_light_get_exponent(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_model_light_set_index(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_model_light_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_model_light_set_type(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_model_light_set_direction(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_model_light_set_intensity(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_model_light_set_exponent(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

extern js_type			js;

JSStaticValue 		model_light_props[]={
							{"index",				js_model_light_get_index,				js_model_light_set_index,			kJSPropertyAttributeDontDelete},
							{"on",					js_model_light_get_on,					js_model_light_set_on,				kJSPropertyAttributeDontDelete},
							{"type",				js_model_light_get_type,				js_model_light_set_type,			kJSPropertyAttributeDontDelete},
							{"direction",			js_model_light_get_direction,			js_model_light_set_direction,		kJSPropertyAttributeDontDelete},
							{"intensity",			js_model_light_get_intensity,			js_model_light_set_intensity,		kJSPropertyAttributeDontDelete},
							{"exponent",			js_model_light_get_exponent,			js_model_light_set_exponent,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSClassRef			model_light_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_light_object(void)
{
	model_light_class=script_create_class("model_light_class",model_light_props,NULL);
}

void script_free_model_light_object(void)
{
	script_free_class(model_light_class);
}

JSObjectRef script_add_model_light_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,model_light_class,"light",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_model_light_get_index(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];
	
	return(script_int_to_value(cx,draw->script_light_idx));
}

JSValueRef js_model_light_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];

	return(script_bool_to_value(cx,light->on));
}

JSValueRef js_model_light_get_type(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];
	
	return(script_int_to_value(cx,light->type-sd_light_type_normal));
}

JSValueRef js_model_light_get_direction(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];

	return(script_int_to_value(cx,light->direction+sd_light_direction_all));
}

JSValueRef js_model_light_get_intensity(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];
	
	return(script_int_to_value(cx,light->intensity));
}

JSValueRef js_model_light_get_exponent(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];
	
	return(script_float_to_value(cx,light->exponent));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_model_light_set_index(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];
	
	draw->script_light_idx=script_value_to_int(cx,vp);
	if ((draw->script_light_idx<0) || (draw->script_light_idx>=max_model_light)) draw->script_light_idx=0;
	
	return(TRUE);
}

bool js_model_light_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];
	
	light->on=script_value_to_bool(cx,vp);
	
	return(TRUE);
}

bool js_model_light_set_type(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];
	
	light->type=script_value_to_int(cx,vp)-sd_light_type_normal;
	
	return(TRUE);
}

bool js_model_light_set_direction(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];
	
	light->direction=script_value_to_int(cx,vp)-sd_light_direction_all;
	
	return(TRUE);
}

bool js_model_light_set_intensity(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];

	light->intensity=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_model_light_set_exponent(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];
	
	light->exponent=script_value_to_float(cx,vp);
	
	return(TRUE);
}

