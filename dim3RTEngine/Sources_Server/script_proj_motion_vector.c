/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.motionVector object

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

#include "interface.h"
#include "scripts.h"
#include "objects.h"

extern js_type			js;

JSValueRef js_proj_motion_vector_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_motion_vector_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_motion_vector_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);

JSStaticValue 		proj_motion_vector_props[]={
							{"x",					js_proj_motion_vector_get_x,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"y",					js_proj_motion_vector_get_y,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"z",					js_proj_motion_vector_get_z,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			proj_motion_vector_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_motion_vector_object(void)
{
	proj_motion_vector_class=script_create_class("proj_motion_vector_class",proj_motion_vector_props,NULL);
}

void script_free_proj_motion_vector_object(void)
{
	script_free_class(proj_motion_vector_class);
}

JSObjectRef script_add_proj_motion_vector_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,proj_motion_vector_class,"motionVector",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_proj_motion_vector_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_type			*proj;

	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));

	return(script_float_to_value(cx,proj->motion.vct.x));
}

JSValueRef js_proj_motion_vector_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_type			*proj;

	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));

	return(script_float_to_value(cx,proj->motion.vct.y));
}

JSValueRef js_proj_motion_vector_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_type			*proj;

	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));

	return(script_float_to_value(cx,proj->motion.vct.z));
}

