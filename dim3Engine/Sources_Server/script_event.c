/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: *.event object

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

extern server_type		server;
extern js_type			js;
extern iface_type		iface;

JSValueRef js_event_start_timer_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_clear_timer_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_start_wait_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_start_wait_random_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_clear_wait_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_chain_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_clear_chain_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_call_object_by_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_call_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_call_course_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_call_game_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_call_held_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_call_spawn_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	event_functions[]={
							{"startTimer",					js_event_start_timer_func,						kJSPropertyAttributeDontDelete},
							{"clearTimer",					js_event_clear_timer_func,						kJSPropertyAttributeDontDelete},
							{"startWait",					js_event_start_wait_func,						kJSPropertyAttributeDontDelete},
							{"startWaitRandom",				js_event_start_wait_random_func,				kJSPropertyAttributeDontDelete},
							{"clearWait",					js_event_clear_wait_func,						kJSPropertyAttributeDontDelete},
							{"chain",						js_event_chain_func,							kJSPropertyAttributeDontDelete},
							{"clearChain",					js_event_clear_chain_func,						kJSPropertyAttributeDontDelete},
							{"callObjectById",				js_event_call_object_by_id_func,				kJSPropertyAttributeDontDelete},
							{"callPlayer",					js_event_call_player_func,						kJSPropertyAttributeDontDelete},
							{"callCourse",					js_event_call_course_func,						kJSPropertyAttributeDontDelete},
							{"callGame",					js_event_call_game_func,						kJSPropertyAttributeDontDelete},
							{"callHeldWeapon",				js_event_call_held_weapon_func,					kJSPropertyAttributeDontDelete},
							{"callSpawnWeapon",				js_event_call_spawn_weapon_func,				kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			event_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_event_object(void)
{
	event_class=script_create_class("event_class",NULL,event_functions);
}

void script_free_event_object(void)
{
	script_free_class(event_class);
}

JSObjectRef script_add_event_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,event_class,"event",script_idx));
}

/* =======================================================

      Start/Clear Timer Functions
      
======================================================= */

JSValueRef js_event_start_timer_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				script_idx;
	char			err_str[256];

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));

	script_idx=(int)JSObjectGetPrivate(j_obj);

	if (!timers_add(script_idx,script_value_to_int(cx,argv[0]),script_value_to_int(cx,argv[1]),NULL,timer_mode_repeat,err_str)) {
		*exception=script_create_exception(cx,err_str);
		return(script_bool_to_value(cx,FALSE));
	}

	return(script_bool_to_value(cx,TRUE));
}

JSValueRef js_event_clear_timer_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				script_idx;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));

	script_idx=(int)JSObjectGetPrivate(j_obj);
	
	timers_trigger_dispose(script_idx,timer_mode_repeat);
    return(script_null_to_value(cx));
}

/* =======================================================

      Start/Clear Wait Functions
      
======================================================= */

JSValueRef js_event_start_wait_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				script_idx;
	char			err_str[256];

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	script_idx=(int)JSObjectGetPrivate(j_obj);
	
	if (!timers_add(script_idx,script_value_to_int(cx,argv[0]),script_value_to_int(cx,argv[1]),NULL,timer_mode_single,err_str)) {
		*exception=script_create_exception(cx,err_str);
		return(script_bool_to_value(cx,FALSE));
	}

	return(script_bool_to_value(cx,TRUE));
}

JSValueRef js_event_start_wait_random_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				min,max,tick,script_idx;
	char			err_str[256];
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	script_idx=(int)JSObjectGetPrivate(j_obj);
	
	min=script_value_to_int(cx,argv[0]);
	max=script_value_to_int(cx,argv[1]);
	tick=random_int(abs(max-min))+min;
	
	if (!timers_add(script_idx,tick,script_value_to_int(cx,argv[2]),NULL,timer_mode_single,err_str)) {
		*exception=script_create_exception(cx,err_str);
		return(script_bool_to_value(cx,FALSE));
	}

	return(script_bool_to_value(cx,TRUE));
}

JSValueRef js_event_clear_wait_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				script_idx;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	script_idx=(int)JSObjectGetPrivate(j_obj);
	
	timers_trigger_dispose(script_idx,timer_mode_single);
    return(script_null_to_value(cx));
}

/* =======================================================

      Chain Functions
      
======================================================= */

JSValueRef js_event_chain_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				script_idx;
	char			chain_func_name[64],err_str[256];
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
	script_idx=(int)JSObjectGetPrivate(j_obj);
	script_value_to_string(cx,argv[1],chain_func_name,64);
	
	if (!timers_add(script_idx,script_value_to_int(cx,argv[0]),0,chain_func_name,timer_mode_chain,err_str)) {
		*exception=script_create_exception(cx,err_str);
		return(script_bool_to_value(cx,FALSE));
	}

	return(script_bool_to_value(cx,TRUE));
}

JSValueRef js_event_clear_chain_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				script_idx;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));

	script_idx=(int)JSObjectGetPrivate(j_obj);
	
	timers_trigger_dispose(script_idx,timer_mode_chain);
    return(script_null_to_value(cx));
}

/* =======================================================

      Direct Function Call
      
======================================================= */

JSValueRef js_event_call_object_by_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,arg_count;
	char			func_name[64],err_str[256];
	JSValueRef		rval,args[20];
	obj_type		*obj;

	if (!script_check_param_at_least_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
		// get arguments

	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	script_value_to_string(cx,argv[1],func_name,64);

	arg_count=argc-2;
	if (arg_count<0) arg_count=0;
	if (arg_count>20) arg_count=20;

	for (n=0;n!=arg_count;n++) {
		args[n]=argv[n+2];
	}

		// call function

	rval=scripts_direct_call(script_get_index(j_obj),obj->script_idx,-1,func_name,arg_count,args,err_str);
	if (rval==NULL) {
		*exception=script_create_exception(cx,err_str);
		return(script_null_to_value(cx));
	}
	
	return(rval);
}

JSValueRef js_event_call_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,arg_count;
	char			func_name[64],err_str[256];
	JSValueRef		rval,args[20];
	obj_type		*obj;

	if (!script_check_param_at_least_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
		// get arguments

	obj=server.obj_list.objs[server.player_obj_idx];

	script_value_to_string(cx,argv[0],func_name,64);

	arg_count=argc-1;
	if (arg_count<0) arg_count=0;
	if (arg_count>20) arg_count=20;

	for (n=0;n!=arg_count;n++) {
		args[n]=argv[n+1];
	}

		// call function

	rval=scripts_direct_call(script_get_index(j_obj),obj->script_idx,-1,func_name,arg_count,args,err_str);
	if (rval==NULL) {
		*exception=script_create_exception(cx,err_str);
		return(script_null_to_value(cx));
	}
	
	return(rval);
}

JSValueRef js_event_call_course_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,arg_count;
	char			func_name[64],err_str[256];
	JSValueRef		rval,args[20];

	if (!script_check_param_at_least_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));

		// any course script?

	if (js.course_script_idx==-1) {
		*exception=script_create_exception(cx,"There is no active course script");
		return(script_null_to_value(cx));
	}
	
		// get arguments

	script_value_to_string(cx,argv[0],func_name,64);

	arg_count=argc-1;
	if (arg_count<0) arg_count=0;
	if (arg_count>20) arg_count=20;

	for (n=0;n!=arg_count;n++) {
		args[n]=argv[n+1];
	}

		// call function

	rval=scripts_direct_call(script_get_index(j_obj),js.course_script_idx,-1,func_name,arg_count,args,err_str);
	if (rval==NULL) {
		*exception=script_create_exception(cx,err_str);
		return(script_null_to_value(cx));
	}
	
	return(rval);
}

JSValueRef js_event_call_game_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,arg_count;
	char			func_name[64],err_str[256];
	JSValueRef		rval,args[20];

	if (!script_check_param_at_least_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
		// get arguments

	script_value_to_string(cx,argv[0],func_name,64);

	arg_count=argc-1;
	if (arg_count<0) arg_count=0;
	if (arg_count>20) arg_count=20;

	for (n=0;n!=arg_count;n++) {
		args[n]=argv[n+1];
	}

		// call function

	rval=scripts_direct_call(script_get_index(j_obj),js.game_script_idx,-1,func_name,arg_count,args,err_str);
	if (rval==NULL) {
		*exception=script_create_exception(cx,err_str);
		return(script_null_to_value(cx));
	}
	
	return(rval);
}

JSValueRef js_event_call_held_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,arg_count,script_idx;
	char			func_name[64],err_str[256];
	JSValueRef		rval,args[20];
	obj_type		*obj;
	weapon_type		*weap;
	script_type		*script;

	if (!script_check_param_at_least_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));

		// find held weapon

	script_idx=(int)JSObjectGetPrivate(j_obj);
	script=js.script_list.scripts[script_idx];
	
	if (script->attach.thing_type!=thing_type_object) {
		*exception=script_create_exception(cx,"Not an object script");
		return(script_null_to_value(cx));
	}

	obj=object_get_attach(j_obj);
	weap=weapon_find_current(obj);
	if (weap==NULL) {
		*exception=script_create_exception(cx,"No held weapon");
		return(script_null_to_value(cx));
	}

		// get arguments

	script_value_to_string(cx,argv[0],func_name,64);

	arg_count=argc-1;
	if (arg_count<0) arg_count=0;
	if (arg_count>20) arg_count=20;

	for (n=0;n!=arg_count;n++) {
		args[n]=argv[n+1];
	}

		// call function

	rval=scripts_direct_call(script_get_index(j_obj),weap->script_idx,-1,func_name,arg_count,args,err_str);
	if (rval==NULL) {
		*exception=script_create_exception(cx,err_str);
		return(script_null_to_value(cx));
	}

	return(rval);
}

JSValueRef js_event_call_spawn_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,arg_count,script_idx;
	char			func_name[64],err_str[256];
	JSValueRef		rval,args[20];
	obj_type		*obj;
	weapon_type		*weap;
	proj_type		*proj;
	script_type		*script;

	if (!script_check_param_at_least_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));

		// find weapon that fired
		// this projectile

	script_idx=(int)JSObjectGetPrivate(j_obj);
	script=js.script_list.scripts[script_idx];
	
	if (script->attach.thing_type!=thing_type_projectile) {
		*exception=script_create_exception(cx,"Not an projectile script");
		return(script_null_to_value(cx));
	}

	proj=server.proj_list.projs[script->attach.proj_idx];
	if (!proj->on) {
		*exception=script_create_exception(cx,"Could not find projectile");
		return(script_null_to_value(cx));
	}

	obj=server.obj_list.objs[script->attach.obj_idx];
	weap=obj->weap_list.weaps[script->attach.weap_idx];
	if (weap==NULL) {
		*exception=script_create_exception(cx,"Could not find weapon");
		return(script_null_to_value(cx));
	}

		// get arguments

	script_value_to_string(cx,argv[0],func_name,64);

	arg_count=argc-1;
	if (arg_count<0) arg_count=0;
	if (arg_count>20) arg_count=20;

	for (n=0;n!=arg_count;n++) {
		args[n]=argv[n+1];
	}

		// call function

	rval=scripts_direct_call(script_get_index(j_obj),weap->script_idx,-1,func_name,arg_count,args,err_str);
	if (rval==NULL) {
		*exception=script_create_exception(cx,err_str);
		return(script_null_to_value(cx));
	}

	return(rval);
}
