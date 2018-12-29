/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.screen object

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

extern js_type			js;
extern view_type		view;
extern setup_type		setup;

JSValueRef js_interface_screen_get_width(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_interface_screen_get_height(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_interface_screen_shader_start_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_screen_shader_start_timed_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_screen_shader_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		interface_screen_props[]={
							{"width",				js_interface_screen_get_width,		NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"height",				js_interface_screen_get_height,		NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	interface_screen_functions[]={
							{"shaderStart",			js_interface_screen_shader_start_func,				kJSPropertyAttributeDontDelete},
							{"shaderStartTimed",	js_interface_screen_shader_start_timed_func,		kJSPropertyAttributeDontDelete},
							{"shaderStop",			js_interface_screen_shader_stop_func,				kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			interface_screen_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_screen_object(void)
{
	interface_screen_class=script_create_class("interface_screen_class",interface_screen_props,interface_screen_functions);
}

void script_free_interface_screen_object(void)
{
	script_free_class(interface_screen_class);
}

JSObjectRef script_add_interface_screen_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,interface_screen_class,"screen",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_interface_screen_get_width(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,view.screen.x_sz));
}

JSValueRef js_interface_screen_get_height(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,view.screen.y_sz));
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_interface_screen_shader_start_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	bool			ok;
	char			shader_name[name_str_len],err_str[256];
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	script_value_to_string(cx,argv[0],shader_name,name_str_len);
	ok=gl_fs_shader_start(shader_name,-1,err_str);
	if (err_str[0]!=0x0) *exception=script_create_exception(cx,err_str);
	
	return(script_bool_to_value(cx,ok));
}

JSValueRef js_interface_screen_shader_start_timed_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	bool			ok;
	char			shader_name[name_str_len],err_str[256];
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	script_value_to_string(cx,argv[0],shader_name,name_str_len);
	ok=gl_fs_shader_start(shader_name,script_value_to_int(cx,argv[1]),err_str);
	if (err_str[0]!=0x0) *exception=script_create_exception(cx,err_str);
	
	return(script_bool_to_value(cx,ok));
}

JSValueRef js_interface_screen_shader_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));

	gl_fs_shader_end();

	return(script_null_to_value(cx));
}
