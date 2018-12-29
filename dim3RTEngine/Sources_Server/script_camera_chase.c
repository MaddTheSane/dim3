/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.chase object

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

extern map_type			map;
extern camera_type		camera;
extern js_type			js;

JSValueRef js_camera_chase_get_size(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_camera_chase_get_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_camera_chase_get_trackSpeed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_camera_chase_set_size(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_camera_chase_set_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_camera_chase_set_trackSpeed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		camera_chase_props[]={
							{"distance",			js_camera_chase_get_distance,			js_camera_chase_set_distance,		kJSPropertyAttributeDontDelete},
							{"trackSpeed",			js_camera_chase_get_trackSpeed,			js_camera_chase_set_trackSpeed,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			camera_chase_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_camera_chase_object(void)
{
	camera_chase_class=script_create_class("camera_chase_class",camera_chase_props,NULL);
}

void script_free_camera_chase_object(void)
{
	script_free_class(camera_chase_class);
}

JSObjectRef script_add_camera_chase_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,camera_chase_class,"chase",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_camera_chase_get_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,map.camera.chase.distance));
}

JSValueRef js_camera_chase_get_trackSpeed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_float_to_value(cx,map.camera.chase.track_speed));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_camera_chase_set_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.camera.chase.distance=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_camera_chase_set_trackSpeed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.camera.chase.track_speed=script_value_to_float(cx,vp);
	
	return(TRUE);
}
