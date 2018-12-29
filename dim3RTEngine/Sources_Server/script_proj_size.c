/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.size object

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

JSValueRef js_proj_size_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_size_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_size_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_size_get_weight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_proj_size_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_proj_size_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_proj_size_set_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_proj_size_set_weight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		proj_size_props[]={
							{"x",					js_proj_size_get_x,					js_proj_size_set_x,			kJSPropertyAttributeDontDelete},
							{"y",					js_proj_size_get_y,					js_proj_size_set_y,			kJSPropertyAttributeDontDelete},
							{"z",					js_proj_size_get_z,					js_proj_size_set_z,			kJSPropertyAttributeDontDelete},
							{"weight",				js_proj_size_get_weight,			js_proj_size_set_weight,	kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			proj_size_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_size_object(void)
{
	proj_size_class=script_create_class("proj_size_class",proj_size_props,NULL);
}

void script_free_proj_size_object(void)
{
	script_free_class(proj_size_class);
}

JSObjectRef script_add_proj_size_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,proj_size_class,"size",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_proj_size_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,proj_setup->size.x));
}

JSValueRef js_proj_size_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,proj_setup->size.y));
}

JSValueRef js_proj_size_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,proj_setup->size.z));
}

JSValueRef js_proj_size_get_weight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,proj_setup->size.weight));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_proj_size_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup!=NULL) proj_setup->size.x=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_proj_size_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup!=NULL) proj_setup->size.y=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_proj_size_set_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup!=NULL) proj_setup->size.z=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_proj_size_set_weight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup!=NULL) proj_setup->size.weight=script_value_to_int(cx,vp);

	return(TRUE);
}

