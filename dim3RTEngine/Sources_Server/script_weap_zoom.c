/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.zoom object

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

JSValueRef js_weap_zoom_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_zoom_get_active(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_zoom_get_fovMinimum(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_zoom_get_fovMaximum(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_zoom_get_fovSteps(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_zoom_get_turnFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_zoom_get_crawlTurnFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_zoom_get_lookFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_zoom_get_swayFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_zoom_get_crawlSwayFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_zoom_get_maskName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_zoom_get_showWeapon(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_zoom_get_tick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_zoom_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_zoom_set_fovMinimum(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_zoom_set_fovMaximum(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_zoom_set_fovSteps(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_zoom_set_turnFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_zoom_set_crawlTurnFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_zoom_set_lookFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_zoom_set_swayFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_zoom_set_crawlSwayFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_zoom_set_maskName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_zoom_set_showWeapon(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_zoom_set_tick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_zoom_enter_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_zoom_exit_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_zoom_goto_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		weap_zoom_props[]={
							{"on",					js_weap_zoom_get_on,						js_weap_zoom_set_on,				kJSPropertyAttributeDontDelete},
							{"active",				js_weap_zoom_get_active,					NULL,								kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"fovMinimum",			js_weap_zoom_get_fovMinimum,				js_weap_zoom_set_fovMinimum,		kJSPropertyAttributeDontDelete},
							{"fovMaximum",			js_weap_zoom_get_fovMaximum,				js_weap_zoom_set_fovMaximum,		kJSPropertyAttributeDontDelete},
							{"fovSteps",			js_weap_zoom_get_fovSteps,					js_weap_zoom_set_fovSteps,			kJSPropertyAttributeDontDelete},
							{"turnFactor",			js_weap_zoom_get_turnFactor,				js_weap_zoom_set_turnFactor,		kJSPropertyAttributeDontDelete},
							{"crawlTurnFactor",		js_weap_zoom_get_crawlTurnFactor,			js_weap_zoom_set_crawlTurnFactor,	kJSPropertyAttributeDontDelete},
							{"lookFactor",			js_weap_zoom_get_lookFactor,				js_weap_zoom_set_lookFactor,		kJSPropertyAttributeDontDelete},
							{"swayFactor",			js_weap_zoom_get_swayFactor,				js_weap_zoom_set_swayFactor,		kJSPropertyAttributeDontDelete},
							{"crawlSwayFactor",		js_weap_zoom_get_crawlSwayFactor,			js_weap_zoom_set_crawlSwayFactor,	kJSPropertyAttributeDontDelete},
							{"maskName",			js_weap_zoom_get_maskName,					js_weap_zoom_set_maskName,			kJSPropertyAttributeDontDelete},
							{"showWeapon",			js_weap_zoom_get_showWeapon,				js_weap_zoom_set_showWeapon,		kJSPropertyAttributeDontDelete},
							{"tick",				js_weap_zoom_get_tick,						js_weap_zoom_set_tick,				kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	weap_zoom_functions[]={
							{"enter",				js_weap_zoom_enter_func,					kJSPropertyAttributeDontDelete},
							{"exit",				js_weap_zoom_exit_func,						kJSPropertyAttributeDontDelete},
							{"goto",				js_weap_zoom_goto_func,						kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			weap_zoom_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_zoom_object(void)
{
	weap_zoom_class=script_create_class("weap_zoom_class",weap_zoom_props,weap_zoom_functions);
}

void script_free_weap_zoom_object(void)
{
	script_free_class(weap_zoom_class);
}

JSObjectRef script_add_weap_zoom_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,weap_zoom_class,"zoom",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_zoom_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_bool_to_value(cx,weap->zoom.on));
}

JSValueRef js_weap_zoom_get_active(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_bool_to_value(cx,weap->zoom.mode!=zoom_mode_off));
}

JSValueRef js_weap_zoom_get_fovMinimum(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->zoom.fov_min));
}

JSValueRef js_weap_zoom_get_fovMaximum(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->zoom.fov_max));
}

JSValueRef js_weap_zoom_get_fovSteps(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_int_to_value(cx,weap->zoom.step_count));
}

JSValueRef js_weap_zoom_get_turnFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->zoom.turn_factor));
}

JSValueRef js_weap_zoom_get_crawlTurnFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->zoom.crawl_turn_factor));
}

JSValueRef js_weap_zoom_get_lookFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->zoom.look_factor));
}

JSValueRef js_weap_zoom_get_swayFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->zoom.sway_factor));
}

JSValueRef js_weap_zoom_get_crawlSwayFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_float_to_value(cx,weap->zoom.crawl_sway_factor));
}

JSValueRef js_weap_zoom_get_maskName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_string_to_value(cx,weap->zoom.mask_name));
}

JSValueRef js_weap_zoom_get_showWeapon(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_bool_to_value(cx,weap->zoom.show_weapon));
}

JSValueRef js_weap_zoom_get_tick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_get_attach(j_obj);
	return(script_int_to_value(cx,weap->zoom.tick));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_zoom_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->zoom.on=script_value_to_bool(cx,vp);

	return(TRUE);
}

bool js_weap_zoom_set_fovMinimum(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->zoom.fov_min=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_zoom_set_fovMaximum(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->zoom.fov_max=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_zoom_set_fovSteps(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->zoom.step_count=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_weap_zoom_set_turnFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->zoom.turn_factor=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_zoom_set_crawlTurnFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->zoom.crawl_turn_factor=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_zoom_set_lookFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->zoom.look_factor=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_zoom_set_swayFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->zoom.sway_factor=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_zoom_set_crawlSwayFactor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->zoom.crawl_sway_factor=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_weap_zoom_set_maskName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	script_value_to_string(cx,vp,weap->zoom.mask_name,name_str_len);
	weapon_attach_zoom_mask(weap);

	return(TRUE);
}

bool js_weap_zoom_set_showWeapon(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->zoom.show_weapon=script_value_to_bool(cx,vp);

	return(TRUE);
}

bool js_weap_zoom_set_tick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_get_attach(j_obj);
	weap->zoom.tick=script_value_to_int(cx,vp);

	return(TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_weap_zoom_enter_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	weap=weapon_get_attach(j_obj);

	weapon_zoom_enter(obj,weap);

	return(script_null_to_value(cx));
}

JSValueRef js_weap_zoom_exit_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	weap=weapon_get_attach(j_obj);

	weapon_zoom_exit(obj,weap);

	return(script_null_to_value(cx));
}

JSValueRef js_weap_zoom_goto_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				step;
	obj_type		*obj;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	obj=object_get_attach(j_obj);
	weap=weapon_get_attach(j_obj);

	step=script_value_to_int(cx,argv[1]);
	if (step<0) step=0;
	if (step>(weap->zoom.step_count-1)) step=weap->zoom.step_count-1;

	weap->zoom.current_step=step;

	return(script_null_to_value(cx));
}


