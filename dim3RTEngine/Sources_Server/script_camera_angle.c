/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.angle object

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

JSValueRef js_camera_angle_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_camera_angle_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_camera_angle_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_camera_angle_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_camera_angle_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_camera_angle_set_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_camera_angle_turn_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		camera_angle_props[]={
							{"x",					js_camera_angle_get_x,		js_camera_angle_set_x,		kJSPropertyAttributeDontDelete},
							{"y",					js_camera_angle_get_y,		js_camera_angle_set_y,		kJSPropertyAttributeDontDelete},
							{"z",					js_camera_angle_get_z,		js_camera_angle_set_z,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	camera_angle_functions[]={
							{"turn",				js_camera_angle_turn_func,				kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			camera_angle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_camera_angle_object(void)
{
	camera_angle_class=script_create_class("camera_angle_class",camera_angle_props,camera_angle_functions);
}

void script_free_camera_angle_object(void)
{
	script_free_class(camera_angle_class);
}

JSObjectRef script_add_camera_angle_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,camera_angle_class,"angle",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_camera_angle_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_float_to_value(cx,map.camera.ang_offset.x));
}

JSValueRef js_camera_angle_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_float_to_value(cx,map.camera.ang_offset.y));
}

JSValueRef js_camera_angle_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_float_to_value(cx,map.camera.ang_offset.z));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_camera_angle_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.camera.ang_offset.x=script_value_to_float(cx,vp);
	return(TRUE);
}

bool js_camera_angle_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.camera.ang_offset.y=script_value_to_float(cx,vp);
	return(TRUE);
}

bool js_camera_angle_set_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.camera.ang_offset.z=script_value_to_float(cx,vp);
	return(TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_camera_angle_turn_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3ang			ang;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
		
	script_value_to_angle(cx,argv[0],&ang);
	camera_auto_turn_set_angle_offset(&ang,script_value_to_int(cx,argv[1]));

	return(script_null_to_value(cx));
}
