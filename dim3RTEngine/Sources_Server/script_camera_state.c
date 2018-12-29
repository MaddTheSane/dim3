/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.state object

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

JSValueRef js_camera_state_save_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_camera_state_restore_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	camera_state_functions[]={
							{"save",				js_camera_state_save_func,				kJSPropertyAttributeDontDelete},
							{"restore",				js_camera_state_restore_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			camera_state_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_camera_state_object(void)
{
	camera_state_class=script_create_class("camera_state_class",NULL,camera_state_functions);
}

void script_free_camera_state_object(void)
{
	script_free_class(camera_state_class);
}

JSObjectRef script_add_camera_state_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,camera_state_class,"state",script_idx));
}

/* =======================================================

      State Function
      
======================================================= */

JSValueRef js_camera_state_save_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	camera_save();
	return(script_null_to_value(cx));
}

JSValueRef js_camera_state_restore_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	camera_restore();
	return(script_null_to_value(cx));
}

