/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.handAngle object

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

JSValueRef js_weap_hand_angle_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_hand_angle_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_hand_angle_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_hand_angle_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_hand_angle_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_hand_angle_set_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		weap_hand_angle_props[]={
							{"x",				js_weap_hand_angle_get_x,			js_weap_hand_angle_set_x,		kJSPropertyAttributeDontDelete},
							{"y",				js_weap_hand_angle_get_y,			js_weap_hand_angle_set_y,		kJSPropertyAttributeDontDelete},
							{"z",				js_weap_hand_angle_get_z,			js_weap_hand_angle_set_z,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			weap_hand_angle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_hand_angle_object(void)
{
	weap_hand_angle_class=script_create_class("weap_hand_angle_class",weap_hand_angle_props,NULL);
}

void script_free_weap_hand_angle_object(void)
{
	script_free_class(weap_hand_angle_class);
}

JSObjectRef script_add_weap_hand_angle_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,weap_hand_angle_class,"handAngle",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_hand_angle_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->hand.ang.x));
}

JSValueRef js_weap_hand_angle_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->hand.ang.y));
}

JSValueRef js_weap_hand_angle_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->hand.ang.z));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_hand_angle_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->hand.ang.x=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_hand_angle_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->hand.ang.y=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_hand_angle_set_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->hand.ang.z=script_value_to_float(cx,vp);

	return(TRUE);
}


