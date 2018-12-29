/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.animation object

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
#include "objects.h"
#include "scripts.h"

JSValueRef js_model_animation_get_index(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_animation_get_currentAnimationName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_animation_get_playing(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_model_animation_set_index(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_model_animation_start_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_stop_all_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_cancel_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_change_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_interrupt_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_start_then_change_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_fade_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_cancel_attached_particles_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

extern js_type			js;

JSStaticValue 		model_animation_props[]={
							{"index",					js_model_animation_get_index,					js_model_animation_set_index,		kJSPropertyAttributeDontDelete},
							{"currentAnimationName",	js_model_animation_get_currentAnimationName,	NULL,								kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"playing",					js_model_animation_get_playing,					NULL,								kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	model_animation_functions[]={
							{"start",					js_model_animation_start_func,						kJSPropertyAttributeDontDelete},
							{"stop",					js_model_animation_stop_func,						kJSPropertyAttributeDontDelete},
							{"stopAll",					js_model_animation_stop_all_func,					kJSPropertyAttributeDontDelete},
							{"cancel",					js_model_animation_cancel_func,						kJSPropertyAttributeDontDelete},
							{"change",					js_model_animation_change_func,						kJSPropertyAttributeDontDelete},
							{"interrupt",				js_model_animation_interrupt_func,					kJSPropertyAttributeDontDelete},
							{"startThenChange",			js_model_animation_start_then_change_func,			kJSPropertyAttributeDontDelete},
							{"fade",					js_model_animation_fade_func,						kJSPropertyAttributeDontDelete},
							{"cancelAttachedParticles",	js_model_animation_cancel_attached_particles_func,	kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			model_animation_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_animation_object(void)
{
	model_animation_class=script_create_class("model_animation_class",model_animation_props,model_animation_functions);
}

void script_free_model_animation_object(void)
{
	script_free_class(model_animation_class);
}

JSObjectRef script_add_model_animation_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,model_animation_class,"animation",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_model_animation_get_index(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw			*draw;

	draw=script_find_model_draw(j_obj);
	return(script_int_to_value(cx,draw->script_animation_idx));
}

JSValueRef js_model_animation_get_currentAnimationName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	char				ani_name[64];
	model_draw			*draw;

	draw=script_find_model_draw(j_obj);

	model_get_current_animation_name(draw,ani_name);
	return(script_string_to_value(cx,ani_name));
}

JSValueRef js_model_animation_get_playing(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw				*draw;
	model_draw_animation	*draw_animation;

	draw=script_find_model_draw(j_obj);
	draw_animation=&draw->animations[draw->script_animation_idx];
	return(script_bool_to_value(cx,draw_animation->mode==am_playing));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_model_animation_set_index(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;

	draw=script_find_model_draw(j_obj);

	draw->script_animation_idx=script_value_to_int(cx,vp);
	if ((draw->script_animation_idx<0) || (draw->script_animation_idx>=max_model_blend_animation)) draw->script_animation_idx=0;

	return(TRUE);
}

/* =======================================================

      Animation Exceptions
      
======================================================= */

JSValueRef js_model_animation_name_exception(JSContextRef cx,char *name)
{
	char			err_str[256];

	sprintf(err_str,"Named animation does not exist: %s",name);
	return(script_create_exception(cx,err_str));
}

/* =======================================================

      Model Animation Functions
      
======================================================= */

JSValueRef js_model_animation_start_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw(j_obj);

	script_value_to_string(cx,argv[0],name,name_str_len);
	
	if (!model_start_animation(draw,name,game_time_get())) {
		*exception=js_model_animation_name_exception(cx,name);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_model_animation_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw(j_obj);
	model_stop_animation(draw);

	return(script_null_to_value(cx));
}

JSValueRef js_model_animation_stop_all_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw(j_obj);
	model_stop_all_animation(draw);

	return(script_null_to_value(cx));
}

JSValueRef js_model_animation_cancel_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw(j_obj);

	script_value_to_string(cx,argv[0],name,name_str_len);
	if (!model_cancel_animation(draw,name)) {
		*exception=js_model_animation_name_exception(cx,name);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_model_animation_change_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw(j_obj);

	script_value_to_string(cx,argv[0],name,name_str_len);
	if (!model_change_animation(draw,name,game_time_get())) {
		*exception=js_model_animation_name_exception(cx,name);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_model_animation_interrupt_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw(j_obj);

	script_value_to_string(cx,argv[0],name,name_str_len);
	if (!model_interrupt_animation(draw,name,game_time_get())) {
		*exception=js_model_animation_name_exception(cx,name);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_model_animation_start_then_change_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw(j_obj);

	script_value_to_string(cx,argv[0],name,name_str_len);
	if (!model_start_animation(draw,name,game_time_get())) {
		*exception=js_model_animation_name_exception(cx,name);
		return(script_null_to_value(cx));
	}

	script_value_to_string(cx,argv[1],name,name_str_len);
	if (!model_change_animation(draw,name,game_time_get())) {
		*exception=js_model_animation_name_exception(cx,name);
		return(script_null_to_value(cx));
	}

	return(script_null_to_value(cx));
}

/* =======================================================

      Fades
      
======================================================= */

JSValueRef js_model_animation_fade_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw(j_obj);
	model_fade_start(draw,script_value_to_int(cx,argv[1]),script_value_to_float(cx,argv[0]));
	
	return(script_null_to_value(cx));
}

/* =======================================================

      Effect Cancels
      
======================================================= */

JSValueRef js_model_animation_cancel_attached_particles_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					script_idx;
	script_type			*script;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	script_idx=(int)JSObjectGetPrivate(j_obj);
	script=js.script_list.scripts[script_idx];
	
		// get correct type for canceling particles
		
	switch (script->attach.thing_type) {
	
		case thing_type_object:
			effect_object_bone_attach_particle_dispose(script->attach.obj_idx);
			break;
			
		case thing_type_weapon:
			effect_weapon_bone_attach_particle_dispose(script->attach.obj_idx,script->attach.weap_idx);
			break;
			
		case thing_type_projectile:
			if (script->attach.proj_idx!=-1) effect_projectile_bone_attach_particle_dispose(script->attach.proj_idx);
			break;

	}
	
	return(script_null_to_value(cx));
}

