/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.hand object

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

JSValueRef js_weap_hand_get_raiseTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_hand_get_lowerTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_hand_get_selectShift(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_hand_get_bobSpeed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_hand_get_bobAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_hand_get_fovOverride(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_hand_set_raiseTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_hand_set_lowerTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_hand_set_selectShift(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_hand_set_bobSpeed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_hand_set_bobAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_hand_set_fovOverride(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		weap_hand_props[]={
							{"raiseTick",			js_weap_hand_get_raiseTick,				js_weap_hand_set_raiseTick,		kJSPropertyAttributeDontDelete},
							{"lowerTick",			js_weap_hand_get_lowerTick,				js_weap_hand_set_lowerTick,		kJSPropertyAttributeDontDelete},
							{"selectShift",			js_weap_hand_get_selectShift,			js_weap_hand_set_selectShift,	kJSPropertyAttributeDontDelete},
							{"bobSpeed",			js_weap_hand_get_bobSpeed,				js_weap_hand_set_bobSpeed,		kJSPropertyAttributeDontDelete},
							{"bobAngle",			js_weap_hand_get_bobAngle,				js_weap_hand_set_bobAngle,		kJSPropertyAttributeDontDelete},
							{"fovOverride",			js_weap_hand_get_fovOverride,			js_weap_hand_set_fovOverride,	kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			weap_hand_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_hand_object(void)
{
	weap_hand_class=script_create_class("weap_hand_class",weap_hand_props,NULL);
}

void script_free_weap_hand_object(void)
{
	script_free_class(weap_hand_class);
}

JSObjectRef script_add_weap_hand_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,weap_hand_class,"hand",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_hand_get_raiseTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_int_to_value(cx,weap->hand.raise_tick));
}

JSValueRef js_weap_hand_get_lowerTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_int_to_value(cx,weap->hand.lower_tick));
}

JSValueRef js_weap_hand_get_selectShift(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_int_to_value(cx,weap->hand.select_shift));
}

JSValueRef js_weap_hand_get_bobSpeed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->hand.bounce_speed));
}

JSValueRef js_weap_hand_get_bobAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->hand.bounce_ang));
}

JSValueRef js_weap_hand_get_fovOverride(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->hand.fov_override));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_hand_set_raiseTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->hand.raise_tick=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_weap_hand_set_lowerTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->hand.lower_tick=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_weap_hand_set_selectShift(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->hand.select_shift=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_weap_hand_set_bobSpeed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->hand.bounce_speed=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_hand_set_bobAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->hand.bounce_ang=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_hand_set_fovOverride(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->hand.fov_override=script_value_to_float(cx,vp);

	return(TRUE);
}


