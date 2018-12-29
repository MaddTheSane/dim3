/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.recoil object

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

JSValueRef js_weap_recoil_get_minX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_recoil_get_maxX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_recoil_get_resetX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_recoil_get_minY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_recoil_get_maxY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_recoil_get_resetY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_recoil_get_minZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_recoil_get_maxZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_recoil_get_resetZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_recoil_get_resetLook(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_recoil_get_resetTurn(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_recoil_set_minX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_recoil_set_maxX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_recoil_set_resetX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_recoil_set_minY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_recoil_set_maxY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_recoil_set_resetY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_recoil_set_minZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_recoil_set_maxZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_recoil_set_resetZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_recoil_set_resetLook(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_recoil_set_resetTurn(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_recoil_go_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		weap_recoil_props[]={
							{"minX",				js_weap_recoil_get_minX,			js_weap_recoil_set_minX,		kJSPropertyAttributeDontDelete},
							{"maxX",				js_weap_recoil_get_maxX,			js_weap_recoil_set_maxX,		kJSPropertyAttributeDontDelete},
							{"resetX",				js_weap_recoil_get_resetX,			js_weap_recoil_set_resetX,		kJSPropertyAttributeDontDelete},
							{"minY",				js_weap_recoil_get_minY,			js_weap_recoil_set_minY,		kJSPropertyAttributeDontDelete},
							{"maxY",				js_weap_recoil_get_maxY,			js_weap_recoil_set_maxY,		kJSPropertyAttributeDontDelete},
							{"resetY",				js_weap_recoil_get_resetY,			js_weap_recoil_set_resetY,		kJSPropertyAttributeDontDelete},
							{"minZ",				js_weap_recoil_get_minZ,			js_weap_recoil_set_minZ,		kJSPropertyAttributeDontDelete},
							{"maxZ",				js_weap_recoil_get_maxZ,			js_weap_recoil_set_maxZ,		kJSPropertyAttributeDontDelete},
							{"resetZ",				js_weap_recoil_get_resetZ,			js_weap_recoil_set_resetZ,		kJSPropertyAttributeDontDelete},
							{"resetLook",			js_weap_recoil_get_resetLook,		js_weap_recoil_set_resetLook,	kJSPropertyAttributeDontDelete},
							{"resetTurn",			js_weap_recoil_get_resetTurn,		js_weap_recoil_set_resetTurn,	kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	weap_recoil_functions[]={
							{"go",					js_weap_recoil_go_func,				kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			weap_recoil_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_recoil_object(void)
{
	weap_recoil_class=script_create_class("weap_recoil_class",weap_recoil_props,weap_recoil_functions);
}

void script_free_weap_recoil_object(void)
{
	script_free_class(weap_recoil_class);
}

JSObjectRef script_add_weap_recoil_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,weap_recoil_class,"recoil",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_recoil_get_minX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->recoil.min_ang.x));
}

JSValueRef js_weap_recoil_get_maxX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->recoil.max_ang.x));
}

JSValueRef js_weap_recoil_get_resetX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->recoil.reset_ang.x));
}

JSValueRef js_weap_recoil_get_minY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->recoil.min_ang.y));
}

JSValueRef js_weap_recoil_get_maxY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->recoil.max_ang.y));
}

JSValueRef js_weap_recoil_get_resetY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->recoil.reset_ang.y));
}

JSValueRef js_weap_recoil_get_minZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->recoil.min_ang.z));
}

JSValueRef js_weap_recoil_get_maxZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->recoil.max_ang.z));
}

JSValueRef js_weap_recoil_get_resetZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->recoil.reset_ang.z));
}

JSValueRef js_weap_recoil_get_resetLook(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->recoil.look_reset));
}

JSValueRef js_weap_recoil_get_resetTurn(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->recoil.turn_reset));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_recoil_set_minX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->recoil.min_ang.x=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_recoil_set_maxX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->recoil.max_ang.x=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_recoil_set_resetX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->recoil.reset_ang.x=fabsf(script_value_to_float(cx,vp));

	return(TRUE);
}

bool js_weap_recoil_set_minY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->recoil.min_ang.y=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_recoil_set_maxY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->recoil.max_ang.y=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_recoil_set_resetY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->recoil.reset_ang.y=fabsf(script_value_to_float(cx,vp));

	return(TRUE);
}

bool js_weap_recoil_set_minZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->recoil.min_ang.z=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_recoil_set_maxZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->recoil.max_ang.z=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_recoil_set_resetZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->recoil.reset_ang.z=fabsf(script_value_to_float(cx,vp));

	return(TRUE);
}

bool js_weap_recoil_set_resetLook(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->recoil.look_reset=fabsf(script_value_to_float(cx,vp));

	return(TRUE);
}

bool js_weap_recoil_set_resetTurn(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->recoil.turn_reset=fabsf(script_value_to_float(cx,vp));

	return(TRUE);
}

/* =======================================================

      Recoil Functions
      
======================================================= */

JSValueRef js_weap_recoil_go_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type			*obj;
	weapon_type			*weap;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	weap=weapon_get_attach(j_obj);
	
	weapon_recoil(obj,weap);

	return(script_null_to_value(cx));
}

