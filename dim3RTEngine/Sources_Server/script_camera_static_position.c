/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.staticPosition object

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

extern iface_type		iface;
extern map_type			map;
extern camera_type		camera;
extern js_type			js;

JSValueRef js_camera_static_position_get_follow(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_camera_static_position_set_follow(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_camera_static_position_move_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_camera_static_position_move_to_spot_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_camera_static_position_walk_to_node_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		camera_static_position_props[]={
							{"follow",				js_camera_static_position_get_follow,			js_camera_static_position_set_follow,			kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	camera_static_position_functions[]={
							{"move",				js_camera_static_position_move_func,			kJSPropertyAttributeDontDelete},
							{"moveToSpot",			js_camera_static_position_move_to_spot_func,	kJSPropertyAttributeDontDelete},
							{"walkToNode",			js_camera_static_position_walk_to_node_func,	kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			camera_static_position_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_camera_static_position_object(void)
{
	camera_static_position_class=script_create_class("camera_static_position_class",camera_static_position_props,camera_static_position_functions);
}

void script_free_camera_static_position_object(void)
{
	script_free_class(camera_static_position_class);
}

JSObjectRef script_add_camera_static_position_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,camera_static_position_class,"staticPosition",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_camera_static_position_get_follow(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_bool_to_value(cx,map.camera.c_static.follow));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_camera_static_position_set_follow(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.camera.c_static.follow=script_value_to_bool(cx,vp);
	
	return(TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_camera_static_position_move_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	script_value_to_point(cx,argv[0],&camera.cur_pos.pnt);
	
	return(script_null_to_value(cx));
}

JSValueRef js_camera_static_position_move_to_spot_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int			idx;
	spot_type	*spot;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	idx=script_value_to_int(cx,argv[0]);
	if ((idx<0) || (idx>=map.nspot)) {
		*exception=script_create_exception(cx,"Unknown spot");
	}
	else {
		spot=&map.spots[idx];
		memmove(&camera.cur_pos.pnt,&spot->pnt,sizeof(d3pnt));
	}

	return(script_null_to_value(cx));
}

JSValueRef js_camera_static_position_walk_to_node_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				msec,event_id;
	char			start_name[name_str_len],end_name[name_str_len],err_str[256];
	bool			open_doors,in_freeze;

	if (!script_check_param_count(cx,func,argc,6,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[0],start_name,name_str_len);
	script_value_to_string(cx,argv[1],end_name,name_str_len);

	msec=script_value_to_int(cx,argv[2]);
	event_id=script_value_to_int(cx,argv[3]);
	open_doors=script_value_to_bool(cx,argv[4]);
	in_freeze=script_value_to_bool(cx,argv[5]);
		
	if (!camera_walk_to_node_setup(start_name,end_name,msec,event_id,open_doors,in_freeze,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}
