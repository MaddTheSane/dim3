/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.action object

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

extern server_type		server;
extern js_type			js;

JSValueRef js_proj_action_get_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_action_get_collision(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_action_get_auto_hitTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_action_get_auto_bounce(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_action_get_auto_bounceMinMove(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_action_get_auto_bounceReduce(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_action_get_auto_reflect(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_proj_action_set_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_proj_action_set_collision(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_proj_action_set_auto_hitTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_proj_action_set_auto_bounce(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_proj_action_set_auto_bounceMinMove(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_proj_action_set_auto_bounceReduce(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_proj_action_set_auto_reflect(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_proj_action_rotate_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_proj_action_turn_towards_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_proj_action_seek_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_proj_action_seek_target_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_proj_action_bounce_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_proj_action_reflect_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_proj_action_stick_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_proj_action_destroy_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		proj_action_props[]={
							{"damage",				js_proj_action_get_damage,				js_proj_action_set_damage,					kJSPropertyAttributeDontDelete},
							{"collision",			js_proj_action_get_collision,			js_proj_action_set_collision,				kJSPropertyAttributeDontDelete},
							{"autoHitTick",			js_proj_action_get_auto_hitTick,		js_proj_action_set_auto_hitTick,			kJSPropertyAttributeDontDelete},
							{"autoBounce",			js_proj_action_get_auto_bounce,			js_proj_action_set_auto_bounce,				kJSPropertyAttributeDontDelete},
							{"autoBounceMinMove",	js_proj_action_get_auto_bounceMinMove,	js_proj_action_set_auto_bounceMinMove,		kJSPropertyAttributeDontDelete},
							{"autoBounceReduce",	js_proj_action_get_auto_bounceReduce,	js_proj_action_set_auto_bounceReduce,		kJSPropertyAttributeDontDelete},
							{"autoReflect",			js_proj_action_get_auto_reflect,		js_proj_action_set_auto_reflect,			kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	proj_action_functions[]={
							{"rotate",				js_proj_action_rotate_func,				kJSPropertyAttributeDontDelete},
							{"turnTowards",			js_proj_action_turn_towards_func,		kJSPropertyAttributeDontDelete},
							{"seek",				js_proj_action_seek_func,				kJSPropertyAttributeDontDelete},
							{"seekTarget",			js_proj_action_seek_target_func,		kJSPropertyAttributeDontDelete},
							{"bounce",				js_proj_action_bounce_func,				kJSPropertyAttributeDontDelete},
							{"reflect",				js_proj_action_reflect_func,			kJSPropertyAttributeDontDelete},
							{"stick",				js_proj_action_stick_func,				kJSPropertyAttributeDontDelete},
							{"destroy",				js_proj_action_destroy_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			proj_action_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_action_object(void)
{
	proj_action_class=script_create_class("proj_action_class",proj_action_props,proj_action_functions);
}

void script_free_proj_action_object(void)
{
	script_free_class(proj_action_class);
}

JSObjectRef script_add_proj_action_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,proj_action_class,"action",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_proj_action_get_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,proj_setup->damage));
}

JSValueRef js_proj_action_get_collision(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_bool_to_value(cx,proj_setup->collision));
}

JSValueRef js_proj_action_get_auto_hitTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,proj_setup->action.hit_tick));
}

JSValueRef js_proj_action_get_auto_bounce(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_bool_to_value(cx,proj_setup->action.bounce));
}

JSValueRef js_proj_action_get_auto_bounceMinMove(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_float_to_value(cx,proj_setup->action.bounce_min_move));
}

JSValueRef js_proj_action_get_auto_bounceReduce(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_float_to_value(cx,proj_setup->action.bounce_reduce));
}

JSValueRef js_proj_action_get_auto_reflect(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_bool_to_value(cx,proj_setup->action.reflect));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_proj_action_set_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup!=NULL) proj_setup->damage=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_proj_action_set_collision(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup!=NULL) proj_setup->collision=script_value_to_bool(cx,vp);

	return(TRUE);
}

bool js_proj_action_set_auto_hitTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup!=NULL) proj_setup->action.hit_tick=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_proj_action_set_auto_bounce(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup!=NULL) proj_setup->action.bounce=script_value_to_bool(cx,vp);

	return(TRUE);
}

bool js_proj_action_set_auto_bounceMinMove(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup!=NULL) proj_setup->action.bounce_min_move=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_proj_action_set_auto_bounceReduce(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup!=NULL) proj_setup->action.bounce_reduce=script_value_to_float(cx,vp);

	return(TRUE);
}

bool js_proj_action_set_auto_reflect(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup!=NULL) proj_setup->action.reflect=script_value_to_bool(cx,vp);

	return(TRUE);
}

/* =======================================================

      Projectile Changes
      
======================================================= */

JSValueRef js_proj_action_rotate_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	proj_type			*proj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));
		
	proj->ang.y=proj->motion.ang.y=script_value_to_float(cx,argv[0]);
	
	return(script_null_to_value(cx));
}

JSValueRef js_proj_action_turn_towards_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	proj_type			*proj;
	obj_type			*to_obj;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));
		
	to_obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (to_obj==NULL) return(script_null_to_value(cx));
	
	projectile_turn_xz_towards(proj,to_obj,script_value_to_float(cx,argv[1]));
	
	return(script_null_to_value(cx));
}

JSValueRef js_proj_action_seek_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	proj_type			*proj;
	obj_type			*to_obj;

	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));
		
	to_obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (to_obj==NULL) return(script_null_to_value(cx));
	
	projectile_seek(proj,to_obj,script_value_to_float(cx,argv[1]),script_value_to_float(cx,argv[2]));
	
	return(script_null_to_value(cx));
}

JSValueRef js_proj_action_seek_target_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type			*to_obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	proj_type			*proj;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));
	
	weap=weapon_get_attach(j_obj);

	proj_setup=weap->proj_setup_list.proj_setups[proj->proj_setup_idx];
	if (proj_setup==NULL) return(script_null_to_value(cx));
		
	to_obj=server.obj_list.objs[weap->target.obj_idx];
	if (to_obj==NULL) return(script_null_to_value(cx));
	
	projectile_seek(proj,to_obj,script_value_to_float(cx,argv[0]),script_value_to_float(cx,argv[1]));
	
	return(script_null_to_value(cx));
}

JSValueRef js_proj_action_bounce_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    float				min_ymove,reduce;
	proj_type			*proj;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));
    
    min_ymove=script_value_to_float(cx,argv[0]);
	reduce=script_value_to_float(cx,argv[1]);
	
	projectile_bounce(proj,min_ymove,reduce,FALSE);
    
	return(script_null_to_value(cx));
}

JSValueRef js_proj_action_reflect_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	proj_type			*proj;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));
	
	projectile_reflect(proj,FALSE);
	     
	return(script_null_to_value(cx));
}

JSValueRef js_proj_action_stick_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	proj_type			*proj;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));
    
    proj->stick=TRUE;
    proj->ang.x=0;
    
	return(script_null_to_value(cx));
}

JSValueRef js_proj_action_destroy_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	proj_type			*proj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));
	
	proj->script_dispose=TRUE;
    
	return(script_null_to_value(cx));
}
