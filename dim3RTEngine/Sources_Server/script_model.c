/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model object

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
extern setup_type		setup;

JSValueRef js_model_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_get_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_get_bounce(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_get_alpha(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_get_resize(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_get_faceForward(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_model_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_model_set_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_model_set_bounce(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_model_set_alpha(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_model_set_resize(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_model_set_faceForward(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		model_props[]={
							{"on",					js_model_get_on,					js_model_set_on,			kJSPropertyAttributeDontDelete},
							{"name",				js_model_get_name,					js_model_set_name,			kJSPropertyAttributeDontDelete},
							{"bounce",				js_model_get_bounce,				js_model_set_bounce,		kJSPropertyAttributeDontDelete},
							{"alpha",				js_model_get_alpha,					js_model_set_alpha,			kJSPropertyAttributeDontDelete},
							{"resize",				js_model_get_resize,				js_model_set_resize,		kJSPropertyAttributeDontDelete},
							{"faceForward",			js_model_get_faceForward,			js_model_set_faceForward,	kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			model_class;

/* =======================================================

      Create Model
      
======================================================= */

void script_init_model_object(void)
{
	model_class=script_create_class("model_class",model_props,NULL);
}

void script_free_model_object(void)
{
	script_free_class(model_class);
}

JSObjectRef script_add_model_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
    return(script_create_child_object(cx,parent_obj,model_class,"model",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_model_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj);
	return(script_bool_to_value(cx,draw->on));
}

JSValueRef js_model_get_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj);
	return(script_string_to_value(cx,draw->name));
}

JSValueRef js_model_get_bounce(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj);
	return(script_bool_to_value(cx,draw->bounce));
}

JSValueRef js_model_get_alpha(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj);
	return(script_float_to_value(cx,draw->alpha));
}

JSValueRef js_model_get_resize(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj);
	return(script_float_to_value(cx,draw->resize));
}

JSValueRef js_model_get_faceForward(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj);
	return(script_bool_to_value(cx,draw->face_forward));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_model_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj);
	draw->on=script_value_to_bool(cx,vp);
	
	return(TRUE);
}

bool js_model_set_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw		*draw;
	
	if (!script_in_construct(j_obj)) {
		*exception=script_create_exception(cx,"Can only set model name in the construct event");
		return(TRUE);
	}

	draw=script_find_model_draw(j_obj);
	script_value_to_string(cx,vp,draw->name,name_str_len);
	
	return(TRUE);
}

bool js_model_set_bounce(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj);
	draw->bounce=script_value_to_bool(cx,vp);
	
	return(TRUE);
}

bool js_model_set_alpha(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj);
	
	draw->alpha=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_model_set_resize(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj);
	draw->resize=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_model_set_faceForward(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj);
	draw->face_forward=script_value_to_bool(cx,vp);
	
	return(TRUE);
}

