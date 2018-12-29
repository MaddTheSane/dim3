/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.setting object

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

JSValueRef js_camera_setting_get_type(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_camera_setting_get_attachObjectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_camera_setting_set_type(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_camera_setting_attach_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		camera_setting_props[]={
							{"type",				js_camera_setting_get_type,				js_camera_setting_set_type,		kJSPropertyAttributeDontDelete},
							{"attachObjectId",		js_camera_setting_get_attachObjectId,	NULL,							kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	camera_setting_functions[]={
							{"attach",				js_camera_setting_attach_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			camera_setting_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_camera_setting_object(void)
{
	camera_setting_class=script_create_class("camera_setting_class",camera_setting_props,camera_setting_functions);
}

void script_free_camera_setting_object(void)
{
	script_free_class(camera_setting_class);
}

JSObjectRef script_add_camera_setting_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,camera_setting_class,"setting",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_camera_setting_get_type(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,map.camera.camera_mode+sd_camera_type_fpp));
}

JSValueRef js_camera_setting_get_attachObjectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,camera.obj_idx));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_camera_setting_set_type(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.camera.camera_mode=script_value_to_int(cx,vp)-sd_camera_type_fpp;
	
	return(TRUE);
}

/* =======================================================

      State Function
      
======================================================= */

JSValueRef js_camera_setting_attach_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj!=NULL) camera_connect(obj);

	return(script_null_to_value(cx));
}
