/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.kickback object

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

JSValueRef js_weap_kickback_get_size(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_kickback_set_size(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_kickback_kick_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		weap_kickback_props[]={
							{"size",				js_weap_kickback_get_size,				js_weap_kickback_set_size,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	weap_kickback_functions[]={
							{"kick",				js_weap_kickback_kick_func,				kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			weap_kickback_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_kickback_object(void)
{
	weap_kickback_class=script_create_class("weap_kickback_class",weap_kickback_props,weap_kickback_functions);
}

void script_free_weap_kickback_object(void)
{
	script_free_class(weap_kickback_class);
}

JSObjectRef script_add_weap_kickback_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,weap_kickback_class,"kickback",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_kickback_get_size(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_int_to_value(cx,weap->kickback.size));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_kickback_set_size(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->kickback.size=script_value_to_int(cx,vp);

	return(TRUE);
}

/* =======================================================

      Kickback Functions
      
======================================================= */

JSValueRef js_weap_kickback_kick_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type			*obj;
	weapon_type			*weap;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	weap=weapon_get_attach(j_obj);
	
	weapon_kickback(obj,weap);

	return(script_null_to_value(cx));
}

