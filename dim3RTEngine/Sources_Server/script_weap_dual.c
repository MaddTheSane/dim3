/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.dual object

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

extern js_type				js;

JSValueRef js_weap_dual_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_dual_get_active(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_dual_get_handOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_dual_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_dual_set_active(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_dual_set_handOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_dual_switch_hand_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		weap_dual_props[]={
							{"on",					js_weap_dual_get_on,					js_weap_dual_set_on,			kJSPropertyAttributeDontDelete},
							{"active",				js_weap_dual_get_active,				js_weap_dual_set_active,		kJSPropertyAttributeDontDelete},
							{"handOffset",			js_weap_dual_get_handOffset,			js_weap_dual_set_handOffset,	kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	weap_dual_functions[]={
							{"switchHand",			js_weap_dual_switch_hand_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			weap_dual_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_dual_object(void)
{
	weap_dual_class=script_create_class("weap_dual_class",weap_dual_props,weap_dual_functions);
}

void script_free_weap_dual_object(void)
{
	script_free_class(weap_dual_class);
}

JSObjectRef script_add_weap_dual_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,weap_dual_class,"dual",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_dual_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_bool_to_value(cx,weap->dual.on));
}

JSValueRef js_weap_dual_get_active(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_bool_to_value(cx,weap->dual.active));
}

JSValueRef js_weap_dual_get_handOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_int_to_value(cx,weap->dual.hand_offset));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_dual_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->dual.on=script_value_to_bool(cx,vp);

	return(TRUE);
}

bool js_weap_dual_set_active(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->dual.active=script_value_to_bool(cx,vp);

	return(TRUE);
}

bool js_weap_dual_set_handOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->dual.hand_offset=script_value_to_int(cx,vp);

	return(TRUE);
}

/* =======================================================

      Dual Functions
      
======================================================= */

JSValueRef js_weap_dual_switch_hand_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	weapon_type		*weap;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	weap=weapon_get_attach(j_obj);
	
	if ((!weap->dual.on) && (!weap->dual.active)) {
		weap->dual.in_dual=FALSE;
	}
	else {
		weap->dual.in_dual=script_value_to_bool(cx,argv[0]);
	}
	
	return(script_null_to_value(cx));
}

