/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: data object

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

#include "scripts.h"

extern js_type			js;

JSValueRef js_data_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_data_sub_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_data_set_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_data_get_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	data_functions[]={
							{"add",					js_data_add_func,					kJSPropertyAttributeDontDelete},
							{"sub",					js_data_sub_func,					kJSPropertyAttributeDontDelete},
							{"set",					js_data_set_func,					kJSPropertyAttributeDontDelete},
							{"get",					js_data_get_func,					kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			data_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_global_data_object(void)
{
	data_class=script_create_class("data_class",NULL,data_functions);
}

void script_free_global_data_object(void)
{
	script_free_class(data_class);
}

JSObjectRef script_add_global_data_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,data_class,"data",script_idx));
}

/* =======================================================

      Data Functions
      
======================================================= */

JSValueRef js_data_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[0],name,name_str_len);
	script_add_global(cx,name,argv[1]);
	
	return(script_null_to_value(cx));
}

JSValueRef js_data_sub_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[0],name,name_str_len);
	script_delete_global(name);
	
	return(script_null_to_value(cx));
}

JSValueRef js_data_set_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[0],name,name_str_len);
	script_set_global(cx,name,argv[1]);

	return(script_null_to_value(cx));
}

JSValueRef js_data_get_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[0],name,name_str_len);
	return(script_get_global(cx,name));
}

