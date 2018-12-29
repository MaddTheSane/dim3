/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.motionVector object

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
#include "objects.h"

extern iface_type		iface;
extern js_type			js;

JSValueRef js_obj_motion_vector_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_motion_vector_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_motion_vector_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_motion_vector_go_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_vector_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_vector_jump_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_vector_shove_direct_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_vector_alter_speed_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_vector_alter_gravity_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_vector_walk_to_node_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_vector_walk_to_node_by_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_vector_walk_to_node_resume_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_vector_walk_to_node_reverse_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_vector_walk_to_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_vector_walk_to_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_vector_walk_to_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_vector_walk_to_dodge_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_vector_walk_to_position_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_vector_turn_to_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_vector_turn_to_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_motion_vector_props[]={
							{"x",					js_obj_motion_vector_get_x,							NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"y",					js_obj_motion_vector_get_y,							NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"z",					js_obj_motion_vector_get_z,							NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	obj_motion_vector_functions[]={
							{"go",					js_obj_motion_vector_go_func,						kJSPropertyAttributeDontDelete},
							{"stop",				js_obj_motion_vector_stop_func,						kJSPropertyAttributeDontDelete},
							{"jump",				js_obj_motion_vector_jump_func,						kJSPropertyAttributeDontDelete},
							{"shoveDirect",			js_obj_motion_vector_shove_direct_func,				kJSPropertyAttributeDontDelete},
							{"alterSpeed",			js_obj_motion_vector_alter_speed_func,				kJSPropertyAttributeDontDelete},
							{"alterGravity",		js_obj_motion_vector_alter_gravity_func,			kJSPropertyAttributeDontDelete},
							{"walkToNode",			js_obj_motion_vector_walk_to_node_func,				kJSPropertyAttributeDontDelete},
							{"walkToNodeById",		js_obj_motion_vector_walk_to_node_by_id_func,		kJSPropertyAttributeDontDelete},
							{"walkToNodeResume",	js_obj_motion_vector_walk_to_node_resume_func,		kJSPropertyAttributeDontDelete},
							{"walkToNodeReverse",	js_obj_motion_vector_walk_to_node_reverse_func,		kJSPropertyAttributeDontDelete},
							{"walkToObject",		js_obj_motion_vector_walk_to_object_func,			kJSPropertyAttributeDontDelete},
							{"walkToPlayer",		js_obj_motion_vector_walk_to_player_func,			kJSPropertyAttributeDontDelete},
							{"walkToPosition",		js_obj_motion_vector_walk_to_position_func,			kJSPropertyAttributeDontDelete},
							{"walkToDodge",			js_obj_motion_vector_walk_to_dodge_func,			kJSPropertyAttributeDontDelete},
							{"turnToObject",		js_obj_motion_vector_turn_to_object_func,			kJSPropertyAttributeDontDelete},
							{"turnToPlayer",		js_obj_motion_vector_turn_to_player_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};
	
JSClassRef			obj_motion_vector_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_motion_vector_object(void)
{
	obj_motion_vector_class=script_create_class("obj_motion_vector_class",obj_motion_vector_props,obj_motion_vector_functions);
}

void script_free_obj_motion_vector_object(void)
{
	script_free_class(obj_motion_vector_class);
}

JSObjectRef script_add_obj_motion_vector_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_motion_vector_class,"motionVector",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_motion_vector_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->motion.vct.x));
}

JSValueRef js_obj_motion_vector_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->motion.vct.y));
}

JSValueRef js_obj_motion_vector_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->motion.vct.z));
}

/* =======================================================

      Stop and Go
      
======================================================= */

JSValueRef js_obj_motion_vector_go_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	object_move_start(obj);
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_motion_vector_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	object_move_stop(obj);
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_motion_vector_jump_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	object_start_jump(obj);
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_motion_vector_shove_direct_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3vct			vct;
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
		
	script_value_to_vector(cx,argv[0],&vct);
	
	obj=object_get_attach(j_obj);
	object_shove_direct(obj,&vct);

	return(script_null_to_value(cx));
}

/* =======================================================

      Alterations
      
======================================================= */

JSValueRef js_obj_motion_vector_alter_speed_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
    object_alter_speed(obj,script_value_to_float(cx,argv[0]));
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_motion_vector_alter_gravity_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
    object_alter_gravity(obj,script_value_to_float(cx,argv[0]));
	
	return(script_null_to_value(cx));
}

/* =======================================================

      Walk To
      
======================================================= */

JSValueRef js_obj_motion_vector_walk_to_node_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			start_name[name_str_len],end_name[name_str_len],err_str[256];
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	
		// get node names
		
	script_value_to_string(cx,argv[0],start_name,name_str_len);
	script_value_to_string(cx,argv[1],end_name,name_str_len);
	
		// start walk
		
	if (!object_auto_walk_node_name_setup(obj,start_name,end_name,script_value_to_int(cx,argv[2]),err_str)) {
		*exception=script_create_exception(cx,err_str);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_motion_vector_walk_to_node_by_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				from_idx,to_idx;
	char			err_str[256];
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	from_idx=script_find_node_idx_from_idx_arg(cx,argv[0],exception);
	if (from_idx==-1) return(script_null_to_value(cx));
	
	to_idx=script_find_node_idx_from_idx_arg(cx,argv[1],exception);
	if (to_idx==-1) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	if (!object_auto_walk_node_setup(obj,from_idx,to_idx,FALSE,script_value_to_int(cx,argv[2]),err_str)) {
		*exception=script_create_exception(cx,err_str);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_motion_vector_walk_to_node_resume_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256];
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	if (!object_auto_walk_node_resume(obj,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_motion_vector_walk_to_node_reverse_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256];
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	if (!object_auto_walk_node_reverse(obj,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_obj_motion_vector_walk_to_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256];
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	if (!object_auto_walk_object_setup(obj,script_value_to_int(cx,argv[0]),FALSE,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_motion_vector_walk_to_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256];
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	if (!object_auto_walk_player_setup(obj,FALSE,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_motion_vector_walk_to_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3pnt			pnt;
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
		
	script_value_to_point(cx,argv[0],&pnt);
	
	obj=object_get_attach(j_obj);
	object_auto_walk_position_setup(obj,&pnt);

	return(script_null_to_value(cx));
}

JSValueRef js_obj_motion_vector_walk_to_dodge_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	object_auto_walk_dodge_setup(obj,script_value_to_float(cx,argv[0]),script_value_to_int(cx,argv[1]));
		
	return(script_null_to_value(cx));
}

/* =======================================================

      Turn To
      
======================================================= */

JSValueRef js_obj_motion_vector_turn_to_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256];
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	if (!object_auto_walk_object_setup(obj,script_value_to_int(cx,argv[0]),TRUE,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_motion_vector_turn_to_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256];
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	if (!object_auto_walk_player_setup(obj,TRUE,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}
	
	return(script_null_to_value(cx));
}
