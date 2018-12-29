/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.melee object

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

extern iface_type		iface;
extern server_type		server;
extern js_type			js;

JSValueRef js_proj_melee_get_strikeBoneName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_melee_get_strikePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_melee_get_radius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_melee_get_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_melee_get_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_melee_get_force(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_melee_get_fallOff(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_proj_melee_set_strikeBoneName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_proj_melee_set_strikePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_proj_melee_set_radius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_proj_melee_set_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_proj_melee_set_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_proj_melee_set_force(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_proj_melee_set_fallOff(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSValueRef js_proj_melee_spawn_from_projectile_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_proj_melee_spawn_from_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		proj_melee_props[]={
							{"strikeBoneName",			js_proj_melee_get_strikeBoneName,		js_proj_melee_set_strikeBoneName,	kJSPropertyAttributeDontDelete},
							{"strikePoseName",			js_proj_melee_get_strikePoseName,		js_proj_melee_set_strikePoseName,	kJSPropertyAttributeDontDelete},
							{"radius",					js_proj_melee_get_radius,				js_proj_melee_set_radius,			kJSPropertyAttributeDontDelete},
							{"distance",				js_proj_melee_get_distance,				js_proj_melee_set_distance,			kJSPropertyAttributeDontDelete},
							{"damage",					js_proj_melee_get_damage,				js_proj_melee_set_damage,			kJSPropertyAttributeDontDelete},
							{"force",					js_proj_melee_get_force,				js_proj_melee_set_force,			kJSPropertyAttributeDontDelete},
							{"fallOff",					js_proj_melee_get_fallOff,				js_proj_melee_set_fallOff,			kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	proj_melee_functions[]={
							{"spawnFromProjectileBone",	js_proj_melee_spawn_from_projectile_bone_func,	kJSPropertyAttributeDontDelete},
							{"spawnFromPosition",		js_proj_melee_spawn_from_position_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			proj_melee_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_melee_object(void)
{
	proj_melee_class=script_create_class("proj_melee_class",proj_melee_props,proj_melee_functions);
}

void script_free_proj_melee_object(void)
{
	script_free_class(proj_melee_class);
}

JSObjectRef script_add_proj_melee_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,proj_melee_class,"melee",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_proj_melee_get_strikeBoneName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type	*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_string_to_value(cx,proj_setup->melee.strike_bone_name));
}

JSValueRef js_proj_melee_get_strikePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_string_to_value(cx,proj_setup->melee.strike_pose_name));
}

JSValueRef js_proj_melee_get_radius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,proj_setup->melee.radius));
}

JSValueRef js_proj_melee_get_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,proj_setup->melee.distance));
}

JSValueRef js_proj_melee_get_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,proj_setup->melee.damage));
}

JSValueRef js_proj_melee_get_force(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,proj_setup->melee.force));
}

JSValueRef js_proj_melee_get_fallOff(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_bool_to_value(cx,proj_setup->melee.fall_off));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_proj_melee_set_strikeBoneName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(TRUE);

	script_value_to_string(cx,vp,proj_setup->melee.strike_bone_name,name_str_len);
	
	return(TRUE);
}

bool js_proj_melee_set_strikePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup==NULL) return(TRUE);
	
	script_value_to_string(cx,vp,proj_setup->melee.strike_pose_name,name_str_len);

	return(TRUE);
}

bool js_proj_melee_set_radius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup!=NULL) proj_setup->melee.radius=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_proj_melee_set_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup!=NULL) proj_setup->melee.distance=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_proj_melee_set_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup!=NULL) proj_setup->melee.damage=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_proj_melee_set_force(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup!=NULL) proj_setup->melee.force=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_proj_melee_set_fallOff(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach(j_obj);
	if (proj_setup!=NULL) proj_setup->melee.fall_off=script_value_to_bool(cx,vp);

	return(TRUE);
}

/* =======================================================

      Spawn Melee
      
======================================================= */

JSValueRef js_proj_melee_spawn_from_projectile_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char				err_str[256];
    obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;
	proj_setup_type		*proj_setup;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));
	
    obj=server.obj_list.objs[proj->obj_idx];
	weap=obj->weap_list.weaps[proj->weap_idx];

	proj_setup=weap->proj_setup_list.proj_setups[proj->proj_setup_idx];
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	if (!melee_script_spawn_projectile_model(obj,weap,proj_setup,proj,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_proj_melee_spawn_from_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3pnt				pnt;
    obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;
	proj_setup_type		*proj_setup;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));
	
	obj=server.obj_list.objs[proj->obj_idx];
	weap=obj->weap_list.weaps[proj->weap_idx];

	proj_setup=weap->proj_setup_list.proj_setups[proj->proj_setup_idx];
	if (proj_setup==NULL) return(script_null_to_value(cx));

	script_value_to_point(cx,argv[0],&pnt);
	
	melee_add(obj,weap,&pnt,&proj->ang,&proj_setup->melee,-1);

	return(script_null_to_value(cx));
}

