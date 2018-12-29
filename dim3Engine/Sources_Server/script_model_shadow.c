/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.shadow object

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

JSValueRef js_model_shadow_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_model_shadow_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

extern js_type			js;

JSStaticValue 		model_shadow_props[]={
							{"on",					js_model_shadow_get_on,				js_model_shadow_set_on,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			model_shadow_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_shadow_object(void)
{
	model_shadow_class=script_create_class("model_shadow_class",model_shadow_props,NULL);
}

void script_free_model_shadow_object(void)
{
	script_free_class(model_shadow_class);
}

JSObjectRef script_add_model_shadow_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,model_shadow_class,"shadow",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_model_shadow_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_shadow	*shadow;

	draw=script_find_model_draw(j_obj);
	shadow=&draw->shadow;
	
	return(script_bool_to_value(cx,shadow->on));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_model_shadow_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_shadow	*shadow;
	
	if (!script_in_construct(j_obj)) {
		*exception=script_create_exception(cx,"Can only set model shadow in the construct event");
		return(TRUE);
	}

	draw=script_find_model_draw(j_obj);
	shadow=&draw->shadow;

	shadow->on=script_value_to_bool(cx,vp);
	
	return(TRUE);
}

