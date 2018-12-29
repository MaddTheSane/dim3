/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: script object

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
#include "objects.h"
#include "scripts.h"

extern server_type		server;
extern js_type			js;

JSValueRef js_script_implements_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_script_attach_event_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_script_call_parent_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_script_get_parent_variable_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_script_call_parent_function_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_script_get_child_variable_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_script_call_child_function_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	script_functions[]={
							{"implements",			js_script_implements_func,				kJSPropertyAttributeDontDelete},
							{"attachEvent",			js_script_attach_event_func,			kJSPropertyAttributeDontDelete},
							{"callParent",			js_script_call_parent_func,				kJSPropertyAttributeDontDelete},
							{"getParentVariable",	js_script_get_parent_variable_func,		kJSPropertyAttributeDontDelete},
							{"callParentFunction",	js_script_call_parent_function_func,	kJSPropertyAttributeDontDelete},
							{"getChildVariable",	js_script_get_child_variable_func,		kJSPropertyAttributeDontDelete},
							{"callChildFunction",	js_script_call_child_function_func,		kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			script_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_global_script_object(void)
{
	script_class=script_create_class("script_class",NULL,script_functions);
}

void script_free_global_script_object(void)
{
	script_free_class(script_class);
}

JSObjectRef script_add_global_script_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,script_class,"script",script_idx));
}

/* =======================================================

      Script Functions
      
======================================================= */

JSValueRef js_script_implements_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				script_idx;
	char			name[file_str_len],err_str[256];
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	if (script_in_event(j_obj)) {
		*exception=script_create_exception(cx,"Can not set implemented script inside an event");
		return(script_null_to_value(cx));
	}
	
	script_idx=(int)JSObjectGetPrivate(j_obj);
	script_value_to_string(cx,argv[0],name,file_str_len);

	if (!scripts_set_implement(script_idx,name,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_script_attach_event_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				script_idx,main_event;
	char			func_name[256],err_str[256];
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	if (script_in_event(j_obj)) {
		*exception=script_create_exception(cx,"Can not attach an event inside an event");
		return(script_null_to_value(cx));
	}
	
		// special check for actually functions instead of names
		
	if (!JSValueIsString(cx,argv[1])) {
		*exception=script_create_exception(cx,"Function parameter is not a string");
		return(script_null_to_value(cx));
	}

		// attach event
		
	script_idx=(int)JSObjectGetPrivate(j_obj);
	main_event=script_value_to_int(cx,argv[0]);
	script_value_to_string(cx,argv[1],func_name,256);

	if (!scripts_setup_event_attach(script_idx,main_event,func_name,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_script_call_parent_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				script_idx;
	char			err_str[256];
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));

	if (!script_in_event(j_obj)) {
		*exception=script_create_exception(cx,"Can not call parent outside an event");
		return(script_null_to_value(cx));
	}
	
	script_idx=(int)JSObjectGetPrivate(j_obj);

	if (!scripts_post_event_call_parent(script_idx,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_script_get_parent_variable_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				script_idx;
	char			prop_name[256],err_str[256];
	JSValueRef		rval;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	script_idx=(int)JSObjectGetPrivate(j_obj);
	script_value_to_string(cx,argv[0],prop_name,256);

	rval=scripts_get_parent_variable(script_idx,prop_name,err_str);
	if (rval==NULL) {
		*exception=script_create_exception(cx,err_str);
		return(script_null_to_value(cx));
	}

	return(rval);
}

JSValueRef js_script_call_parent_function_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,script_idx,arg_count;
	char			func_name[256],err_str[256];
	JSValueRef		rval,args[20];
	
	if (!script_check_param_at_least_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	script_idx=(int)JSObjectGetPrivate(j_obj);
	script_value_to_string(cx,argv[0],func_name,256);

	arg_count=argc-1;
	if (arg_count<0) arg_count=0;
	if (arg_count>20) arg_count=20;

	for (n=0;n!=arg_count;n++) {
		args[n]=argv[n+1];
	}

	rval=scripts_call_parent_function(script_idx,func_name,arg_count,args,err_str);
	if (rval==NULL) {
		*exception=script_create_exception(cx,err_str);
		return(script_null_to_value(cx));
	}

	return(rval);
}

JSValueRef js_script_get_child_variable_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				script_idx;
	char			prop_name[256],err_str[256];
	JSValueRef		rval;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	script_idx=(int)JSObjectGetPrivate(j_obj);
	script_value_to_string(cx,argv[0],prop_name,256);

	rval=scripts_get_child_variable(script_idx,prop_name,err_str);
	if (rval==NULL) {
		*exception=script_create_exception(cx,err_str);
		return(script_null_to_value(cx));
	}

	return(rval);
}

JSValueRef js_script_call_child_function_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,script_idx,arg_count;
	char			func_name[256],err_str[256];
	JSValueRef		rval,args[20];
	
	if (!script_check_param_at_least_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	script_idx=(int)JSObjectGetPrivate(j_obj);
	script_value_to_string(cx,argv[0],func_name,256);

	arg_count=argc-1;
	if (arg_count<0) arg_count=0;
	if (arg_count>20) arg_count=20;

	for (n=0;n!=arg_count;n++) {
		args[n]=argv[n+1];
	}

	rval=scripts_call_child_function(script_idx,func_name,arg_count,args,err_str);
	if (rval==NULL) {
		*exception=script_create_exception(cx,err_str);
		return(script_null_to_value(cx));
	}

	return(rval);
}
