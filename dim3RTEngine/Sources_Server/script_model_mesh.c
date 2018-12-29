/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.mesh object

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

JSValueRef js_model_mesh_show_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_mesh_show_all_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_mesh_show_only_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_mesh_hide_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_mesh_hide_all_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

extern js_type			js;

JSStaticFunction	model_mesh_functions[]={
							{"showMesh",			js_model_mesh_show_mesh_func,				kJSPropertyAttributeDontDelete},
							{"showAllMesh",			js_model_mesh_show_all_mesh_func,			kJSPropertyAttributeDontDelete},
							{"showOnlyMesh",		js_model_mesh_show_only_mesh_func,			kJSPropertyAttributeDontDelete},
							{"hideMesh",			js_model_mesh_hide_mesh_func,				kJSPropertyAttributeDontDelete},
							{"hideAllMesh",			js_model_mesh_hide_all_mesh_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			model_mesh_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_mesh_object(void)
{
	model_mesh_class=script_create_class("model_mesh_class",NULL,model_mesh_functions);
}

void script_free_model_mesh_object(void)
{
	script_free_class(model_mesh_class);
}

JSObjectRef script_add_model_mesh_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,model_mesh_class,"mesh",script_idx));
}

/* =======================================================

      Mesh Exceptions
      
======================================================= */

JSValueRef js_model_mesh_name_exception(JSContextRef cx,char *name)
{
	char			err_str[256];

	sprintf(err_str,"Named mesh does not exist: %s",name);
	return(script_create_exception(cx,err_str));
}

/* =======================================================

      Model Mesh Functions
      
======================================================= */

JSValueRef js_model_mesh_show_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw(j_obj);

	script_value_to_string(cx,argv[0],name,name_str_len);
	if (!model_show_mesh(draw,name)) {
		*exception=js_model_mesh_name_exception(cx,name);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_model_mesh_show_all_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw(j_obj);
	model_show_all_mesh(draw);

	return(script_null_to_value(cx));
}

JSValueRef js_model_mesh_show_only_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));

	draw=script_find_model_draw(j_obj);

	script_value_to_string(cx,argv[0],name,name_str_len);
	if (!model_show_only_mesh(draw,name)) {
		*exception=js_model_mesh_name_exception(cx,name);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_model_mesh_hide_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw(j_obj);

	script_value_to_string(cx,argv[0],name,name_str_len);
	if (!model_hide_mesh(draw,name)) {
		*exception=js_model_mesh_name_exception(cx,name);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_model_mesh_hide_all_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw(j_obj);
	model_hide_all_mesh(draw);

	return(script_null_to_value(cx));
}

