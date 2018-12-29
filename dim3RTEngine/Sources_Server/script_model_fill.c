/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.fill object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

JSValueRef js_model_fill_change_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_fill_startAnimation_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_fill_stopAnimation_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	model_fill_functions[]={
							{"change",				js_model_fill_change_func,				kJSPropertyAttributeDontDelete},
							{"startAnimation",		js_model_fill_startAnimation_func,		kJSPropertyAttributeDontDelete},
							{"stopAnimation",		js_model_fill_stopAnimation_func,		kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			model_fill_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_fill_object(void)
{
	model_fill_class=script_create_class("model_fill_class",NULL,model_fill_functions);
}

void script_free_model_fill_object(void)
{
	script_free_class(model_fill_class);
}

JSObjectRef script_add_model_fill_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,model_fill_class,"fill",script_idx));
}

/* =======================================================

      Model Fill Functions
      
======================================================= */

JSValueRef js_model_fill_change_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw(j_obj);
    model_texture_change_frame(draw,script_value_to_int(cx,argv[0]),script_value_to_int(cx,argv[1]));

	return(script_null_to_value(cx));
}

JSValueRef js_model_fill_startAnimation_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw(j_obj);
	model_texture_change_animation(draw,script_value_to_int(cx,argv[0]),TRUE,(!script_value_to_bool(cx,argv[1])));
 
	return(script_null_to_value(cx));
}

JSValueRef js_model_fill_stopAnimation_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw(j_obj);
	model_texture_change_animation(draw,script_value_to_int(cx,argv[0]),FALSE,FALSE);
 
	return(script_null_to_value(cx));
}

