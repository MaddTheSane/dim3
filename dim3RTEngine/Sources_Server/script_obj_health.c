/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.health object

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

#include "scripts.h"
#include "objects.h"

extern js_type			js;

JSValueRef js_obj_health_get_maximum(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_health_get_start(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_health_get_current(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_health_get_recoverTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_health_get_recoverAmount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_health_get_fallDamageMinimumHeight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_health_get_fallDamageFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_health_get_factor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_health_get_armor_maximum(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_health_get_armor_start(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_health_get_armor_current(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_health_set_maximum(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_health_set_start(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_health_set_recoverTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_health_set_recoverAmount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_health_set_fallDamageMinimumHeight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_health_set_fallDamageFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_health_set_factor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_health_set_armor_maximum(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_health_set_armor_start(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_health_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_health_remove_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_health_remove_from_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_health_damage_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_health_damage_from_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_health_reset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_health_props[]={
							{"maximum",					js_obj_health_get_maximum,					js_obj_health_set_maximum,					kJSPropertyAttributeDontDelete},
							{"start",					js_obj_health_get_start,					js_obj_health_set_start,					kJSPropertyAttributeDontDelete},
							{"current",					js_obj_health_get_current,					NULL,										kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"recoverTick",				js_obj_health_get_recoverTick,				js_obj_health_set_recoverTick,				kJSPropertyAttributeDontDelete},
							{"recoverAmount",			js_obj_health_get_recoverAmount,			js_obj_health_set_recoverAmount,			kJSPropertyAttributeDontDelete},
							{"fallDamageMinimumHeight",	js_obj_health_get_fallDamageMinimumHeight,	js_obj_health_set_fallDamageMinimumHeight,	kJSPropertyAttributeDontDelete},
							{"fallDamageFactor",		js_obj_health_get_fallDamageFactor,			js_obj_health_set_fallDamageFactor,			kJSPropertyAttributeDontDelete},
							{"factor",					js_obj_health_get_factor,					js_obj_health_set_factor,					kJSPropertyAttributeDontDelete},
							{"armorMaximum",			js_obj_health_get_armor_maximum,			js_obj_health_set_armor_maximum,			kJSPropertyAttributeDontDelete},
							{"armorStart",				js_obj_health_get_armor_start,				js_obj_health_set_armor_start,				kJSPropertyAttributeDontDelete},
							{"armorCurrent",			js_obj_health_get_armor_current,			NULL,										kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	obj_health_functions[]={
							{"add",						js_obj_health_add_func,						kJSPropertyAttributeDontDelete},
							{"remove",					js_obj_health_remove_func,					kJSPropertyAttributeDontDelete},
							{"removeFromObject",		js_obj_health_remove_from_object_func,		kJSPropertyAttributeDontDelete},
							{"damage",					js_obj_health_damage_func,					kJSPropertyAttributeDontDelete},
							{"damageFromObject",		js_obj_health_damage_from_object_func,		kJSPropertyAttributeDontDelete},
							{"reset",					js_obj_health_reset_func,					kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_health_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_health_object(void)
{
	obj_health_class=script_create_class("obj_health_class",obj_health_props,obj_health_functions);
}

void script_free_obj_health_object(void)
{
	script_free_class(obj_health_class);
}

JSObjectRef script_add_obj_health_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_health_class,"health",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_health_get_maximum(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->status.health.max_value));
}

JSValueRef js_obj_health_get_start(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->status.health.start_value));
}

JSValueRef js_obj_health_get_current(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->status.health.value));
}

JSValueRef js_obj_health_get_recoverTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->status.health.recover_tick));
}

JSValueRef js_obj_health_get_recoverAmount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->status.health.recover_amount));
}

JSValueRef js_obj_health_get_fallDamageMinimumHeight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->fall.damage_minimum_height));
}

JSValueRef js_obj_health_get_fallDamageFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->fall.damage_factor));
}

JSValueRef js_obj_health_get_factor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->status.health.factor));
}

JSValueRef js_obj_health_get_armor_maximum(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->status.armor.max_value));
}

JSValueRef js_obj_health_get_armor_start(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->status.armor.start_value));
}

JSValueRef js_obj_health_get_armor_current(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->status.armor.value));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_health_set_maximum(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->status.health.max_value=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_health_set_start(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->status.health.start_value=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_health_set_recoverTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->status.health.recover_tick=script_value_to_int(cx,vp);
	obj->status.health.recover_count=0;			// restart recover count
	
	return(TRUE);
}

bool js_obj_health_set_recoverAmount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->status.health.recover_amount=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_health_set_fallDamageMinimumHeight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->fall.damage_minimum_height=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_health_set_fallDamageFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->fall.damage_factor=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_obj_health_set_factor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->status.health.factor=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_obj_health_set_armor_maximum(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->status.armor.max_value=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_health_set_armor_start(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->status.armor.start_value=script_value_to_int(cx,vp);
	
	return(TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_obj_health_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	object_health_add(obj,script_value_to_int(cx,argv[0]));

	return(script_null_to_value(cx));
}

JSValueRef js_obj_health_remove_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	
	obj->damage_obj_idx=-1;			// self-inflicted damage
	object_health_remove(obj,script_value_to_int(cx,argv[0]));

	return(script_null_to_value(cx));
}

JSValueRef js_obj_health_remove_from_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	
	obj->damage_obj_idx=script_value_to_int(cx,argv[1]);
	object_health_remove(obj,script_value_to_int(cx,argv[0]));

	return(script_null_to_value(cx));
}

JSValueRef js_obj_health_damage_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	object_damage(obj,NULL,NULL,NULL,NULL,script_value_to_int(cx,argv[0]));

	return(script_null_to_value(cx));
}

JSValueRef js_obj_health_damage_from_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj,*damage_obj;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	
	damage_obj=script_find_obj_from_uid_arg(cx,argv[1],exception);
	if (damage_obj==NULL) return(script_null_to_value(cx));

	object_damage(obj,damage_obj,NULL,NULL,NULL,script_value_to_int(cx,argv[0]));

	return(script_null_to_value(cx));
}

JSValueRef js_obj_health_reset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	obj_health		*health;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
    health=&obj->status.health;
	
	health->value=health->max_value;

	return(script_null_to_value(cx));
}


