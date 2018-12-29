/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.spin object

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

JSValueRef js_model_spin_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_spin_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_spin_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_model_spin_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_model_spin_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_model_spin_set_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

extern js_type			js;

JSStaticValue 		model_spin_props[]={
							{"x",					js_model_spin_get_x,					js_model_spin_set_x,		kJSPropertyAttributeDontDelete},
							{"y",					js_model_spin_get_y,					js_model_spin_set_y,		kJSPropertyAttributeDontDelete},
							{"z",					js_model_spin_get_z,					js_model_spin_set_z,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			model_spin_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_spin_object(void)
{
	model_spin_class=script_create_class("model_spin_class",model_spin_props,NULL);
}

void script_free_model_spin_object(void)
{
	script_free_class(model_spin_class);
}

JSObjectRef script_add_model_spin_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,model_spin_class,"spin",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_model_spin_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj);
	return(script_float_to_value(cx,draw->spin.x));
}

JSValueRef js_model_spin_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj);
	return(script_float_to_value(cx,draw->spin.y));
}

JSValueRef js_model_spin_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj);
	return(script_float_to_value(cx,draw->spin.z));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_model_spin_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw		*draw;
	
	draw=script_find_model_draw(j_obj);

	draw->spin.x=script_value_to_float(cx,vp);
	draw->spin_tick=game_time_get();
	
	return(TRUE);
}

bool js_model_spin_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw		*draw;
	
	draw=script_find_model_draw(j_obj);

	draw->spin.y=script_value_to_float(cx,vp);
	draw->spin_tick=game_time_get();
	
	return(TRUE);
}

bool js_model_spin_set_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw		*draw;
	
	draw=script_find_model_draw(j_obj);

	draw->spin.z=script_value_to_float(cx,vp);
	draw->spin_tick=game_time_get();
	
	return(TRUE);
}

