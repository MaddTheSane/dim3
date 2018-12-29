/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.held object

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
#include "objects.h"

extern server_type		server;
extern js_type			js;

JSValueRef js_obj_held_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_held_drop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	obj_held_functions[]={
							{"add",					js_obj_held_add_func,				kJSPropertyAttributeDontDelete},
							{"drop",				js_obj_held_drop_func,				kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_held_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_held_object(void)
{
	obj_held_class=script_create_class("obj_held_class",NULL,obj_held_functions);
}

void script_free_obj_held_object(void)
{
	script_free_class(obj_held_class);
}

JSObjectRef script_add_obj_held_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_held_class,"held",script_idx));
}

/* =======================================================

      Add/Drop Functions
      
======================================================= */

JSValueRef js_obj_held_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int			uid;
	char		name[name_str_len],
				script[file_str_len],params[param_str_len],
				err_str[256];
	obj_type	*obj;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);

	script_value_to_string(cx,argv[0],name,name_str_len);
	script_value_to_string(cx,argv[1],script,file_str_len);
	script_value_to_string(cx,argv[2],params,param_str_len);

	uid=object_held_add(obj,name,script,params,err_str);
	if (uid==-1) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_int_to_value(cx,uid));
}

JSValueRef js_obj_held_drop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int			uid,y_change;
	float		y_ang;
	char		err_str[256];
	obj_type	*obj;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);

	uid=script_value_to_int(cx,argv[0]);
	y_ang=script_value_to_float(cx,argv[1]);
	y_change=script_value_to_int(cx,argv[2]);

	if (!object_held_drop(obj,uid,y_ang,y_change,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

