/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.altAmmo object

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

extern js_type				js;

JSValueRef js_weap_alt_ammo_get_clip(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_alt_ammo_get_count(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_alt_ammo_get_initCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_alt_ammo_get_maxCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_alt_ammo_get_clipCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_alt_ammo_get_initClipCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_alt_ammo_get_maxClipCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_alt_ammo_get_lastReloadTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_alt_ammo_set_clip(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_alt_ammo_set_count(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_alt_ammo_set_initCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_alt_ammo_set_maxCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_alt_ammo_set_clipCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_alt_ammo_set_initClipCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_alt_ammo_set_maxClipCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_alt_ammo_use_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_alt_ammo_add_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_alt_ammo_change_clip_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		weap_alt_ammo_props[]={
							{"clip",				js_weap_alt_ammo_get_clip,				js_weap_alt_ammo_set_clip,				kJSPropertyAttributeDontDelete},
							{"count",				js_weap_alt_ammo_get_count,				js_weap_alt_ammo_set_count,				kJSPropertyAttributeDontDelete},
							{"initCount",			js_weap_alt_ammo_get_initCount,			js_weap_alt_ammo_set_initCount,			kJSPropertyAttributeDontDelete},
							{"maxCount",			js_weap_alt_ammo_get_maxCount,			js_weap_alt_ammo_set_maxCount,			kJSPropertyAttributeDontDelete},
							{"clipCount",			js_weap_alt_ammo_get_clipCount,			js_weap_alt_ammo_set_clipCount,			kJSPropertyAttributeDontDelete},
							{"initClipCount",		js_weap_alt_ammo_get_initClipCount,		js_weap_alt_ammo_set_initClipCount,		kJSPropertyAttributeDontDelete},
							{"maxClipCount",		js_weap_alt_ammo_get_maxClipCount,		js_weap_alt_ammo_set_maxClipCount,		kJSPropertyAttributeDontDelete},
							{"lastReloadTick",		js_weap_alt_ammo_get_lastReloadTick,	NULL,									kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	weap_alt_ammo_functions[]={
							{"useAmmo",				js_weap_alt_ammo_use_ammo_func,			kJSPropertyAttributeDontDelete},
							{"addAmmo",				js_weap_alt_ammo_add_ammo_func,			kJSPropertyAttributeDontDelete},
							{"changeClip",			js_weap_alt_ammo_change_clip_func,		kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			weap_alt_ammo_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_alt_ammo_object(void)
{
	weap_alt_ammo_class=script_create_class("weap_alt_ammo_class",weap_alt_ammo_props,weap_alt_ammo_functions);
}

void script_free_weap_alt_ammo_object(void)
{
	script_free_class(weap_alt_ammo_class);
}

JSObjectRef script_add_weap_alt_ammo_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,weap_alt_ammo_class,"altAmmo",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_alt_ammo_get_clip(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;

	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;
	
	return(script_bool_to_value(cx,alt_ammo->use_clips));
}

JSValueRef js_weap_alt_ammo_get_count(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;

	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;

	if (weap->dual.in_dual) return(script_int_to_value(cx,alt_ammo->count_dual));
	
	return(script_int_to_value(cx,alt_ammo->count));
}

JSValueRef js_weap_alt_ammo_get_initCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;

	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;

	return(script_int_to_value(cx,alt_ammo->init_count));
}

JSValueRef js_weap_alt_ammo_get_maxCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;

	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;

	return(script_int_to_value(cx,alt_ammo->max_count));
}

JSValueRef js_weap_alt_ammo_get_clipCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;

	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;

	return(script_int_to_value(cx,alt_ammo->clip_count));
}

JSValueRef js_weap_alt_ammo_get_initClipCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;

	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;

	return(script_int_to_value(cx,alt_ammo->init_clip_count));
}

JSValueRef js_weap_alt_ammo_get_maxClipCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;

	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;

	return(script_int_to_value(cx,alt_ammo->max_clip_count));
}

JSValueRef js_weap_alt_ammo_get_lastReloadTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;

	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;
	
	if (weap->dual.in_dual) return(script_int_to_value(cx,alt_ammo->last_reload_dual_tick));
	
	return(script_int_to_value(cx,alt_ammo->last_reload_tick));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_alt_ammo_set_clip(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;
	
	alt_ammo->use_clips=script_value_to_bool(cx,vp);

	return(TRUE);
}

bool js_weap_alt_ammo_set_count(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;

	if (weap->dual.in_dual) {
		alt_ammo->count_dual=script_value_to_int(cx,vp);
	}
	else {
		alt_ammo->count=script_value_to_int(cx,vp);
	}

	return(TRUE);
}

bool js_weap_alt_ammo_set_initCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;

	alt_ammo->init_count=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_weap_alt_ammo_set_maxCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;

	alt_ammo->max_count=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_weap_alt_ammo_set_clipCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;

	alt_ammo->clip_count=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_weap_alt_ammo_set_initClipCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;

	alt_ammo->init_clip_count=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_weap_alt_ammo_set_maxClipCount(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;

	alt_ammo->max_clip_count=script_value_to_int(cx,vp);

	return(TRUE);
}

/* =======================================================

      Alternate Ammo Functions
      
======================================================= */

JSValueRef js_weap_alt_ammo_use_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				count;
	JSValueRef		vp;
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;

	count=script_value_to_int(cx,argv[0]);
	
	vp=script_bool_to_value(cx,FALSE);
	
	if (weap->dual.in_dual) {
		if (alt_ammo->count_dual>=count) {
			alt_ammo->count_dual-=count;
			vp=script_bool_to_value(cx,TRUE);
		}
	}
	else {
		if (alt_ammo->count>=count) {
			alt_ammo->count-=count;
			vp=script_bool_to_value(cx,TRUE);
		}
	}
	
	return(vp);
}

JSValueRef js_weap_alt_ammo_add_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				add;
	JSValueRef		vp;
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	weap=weapon_get_attach(j_obj);
	alt_ammo=&weap->alt_ammo;
	
	add=script_value_to_int(cx,argv[0]);
	
	vp=script_bool_to_value(cx,TRUE);

	if (weap->dual.in_dual) {
		if (alt_ammo->count_dual==alt_ammo->max_count) {
			vp=script_bool_to_value(cx,FALSE);
		}
		else {
			alt_ammo->count_dual+=add;
			if (alt_ammo->count_dual>alt_ammo->max_count) alt_ammo->count_dual=alt_ammo->max_count;
		}
	}
	else {
		if (alt_ammo->count==alt_ammo->max_count) {
			vp=script_bool_to_value(cx,FALSE);
		}
		else {
			alt_ammo->count+=add;
			if (alt_ammo->count>alt_ammo->max_count) alt_ammo->count=alt_ammo->max_count;
		}
	}
	
	return(vp);
}

JSValueRef js_weap_alt_ammo_change_clip_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	weapon_type		*weap;
	weap_ammo_type	*alt_ammo;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	weap=weapon_get_attach(j_obj);

	alt_ammo=&weap->alt_ammo;
	
		// is this a clip based weapon and enough clips?
		
	if ((!alt_ammo->use_clips) || (alt_ammo->clip_count==0)) {
		return(script_bool_to_value(cx,FALSE));
	}
	
		// change the clip
		
	if (weap->dual.in_dual) {
		alt_ammo->count_dual=alt_ammo->max_count;
		alt_ammo->last_reload_dual_tick=game_time_get();
	}
	else {
		alt_ammo->count=alt_ammo->max_count;
		alt_ammo->last_reload_tick=game_time_get();
	}
	
	alt_ammo->clip_count--;
	
		// alert object of clip change
		
	scripts_post_event_console(obj->script_idx,-1,sd_event_weapon_fire,sd_event_weapon_fire_clip_change,0);
	
	return(script_bool_to_value(cx,TRUE));
}


