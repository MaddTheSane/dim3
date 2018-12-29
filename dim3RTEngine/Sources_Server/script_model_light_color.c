/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.lightColor object

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

JSValueRef js_model_light_color_get_red(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_light_color_get_green(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_light_color_get_blue(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_model_light_color_set_red(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_model_light_color_set_green(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_model_light_color_set_blue(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

extern js_type			js;

JSStaticValue 		model_light_color_props[]={
							{"red",					js_model_light_color_get_red,			js_model_light_color_set_red,		kJSPropertyAttributeDontDelete},
							{"green",				js_model_light_color_get_green,			js_model_light_color_set_green,		kJSPropertyAttributeDontDelete},
							{"blue",				js_model_light_color_get_blue,			js_model_light_color_set_blue,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			model_light_color_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_light_color_object(void)
{
	model_light_color_class=script_create_class("model_light_color_class",model_light_color_props,NULL);
}

void script_free_model_light_color_object(void)
{
	script_free_class(model_light_color_class);
}

JSObjectRef script_add_model_light_color_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,model_light_color_class,"lightColor",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_model_light_color_get_red(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];
	
	return(script_float_to_value(cx,light->col.r));
}

JSValueRef js_model_light_color_get_green(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];

	return(script_float_to_value(cx,light->col.g));
}

JSValueRef js_model_light_color_get_blue(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];
	
	return(script_float_to_value(cx,light->col.b));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_model_light_color_set_red(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];

	light->col.r=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_model_light_color_set_green(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];

	light->col.g=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_model_light_color_set_blue(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw(j_obj);
	light=&draw->lights[draw->script_light_idx];

	light->col.b=script_value_to_float(cx,vp);
	
	return(TRUE);
}

