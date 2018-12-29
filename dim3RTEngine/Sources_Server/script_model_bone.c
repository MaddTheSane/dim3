/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.bone object

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

JSValueRef js_model_bone_find_offset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_bone_find_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_bone_get_brightness_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_bone_set_dynamic_rotate_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_bone_set_dynamic_move_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_bone_set_dynamic_resize_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	model_bone_functions[]={
							{"findOffset",			js_model_bone_find_offset_func,				kJSPropertyAttributeDontDelete},
							{"findPosition",		js_model_bone_find_position_func,			kJSPropertyAttributeDontDelete},
							{"getBrightness",		js_model_bone_get_brightness_func,			kJSPropertyAttributeDontDelete},
							{"setDynamicRotate",	js_model_bone_set_dynamic_rotate_func,		kJSPropertyAttributeDontDelete},
							{"setDynamicMove",		js_model_bone_set_dynamic_move_func,		kJSPropertyAttributeDontDelete},
							{"setDynamicResize",	js_model_bone_set_dynamic_resize_func,		kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			model_bone_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_bone_object(void)
{
	model_bone_class=script_create_class("model_bone_class",NULL,model_bone_functions);
}

void script_free_model_bone_object(void)
{
	script_free_class(model_bone_class);
}

JSObjectRef script_add_model_bone_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,model_bone_class,"bone",script_idx));
}

/* =======================================================

      Setup Draw For Bone Functions
      
======================================================= */

model_draw* script_bone_function_setup(JSContextRef cx,JSObjectRef j_obj,JSValueRef *exception)
{
	int					script_idx;
	obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;
	script_type			*script;
	
	script_idx=(int)JSObjectGetPrivate(j_obj);
	script=js.script_list.scripts[script_idx];
	
	switch (script->attach.thing_type) {
	
		case thing_type_object:
			obj=server.obj_list.objs[script->attach.obj_idx];
			model_draw_setup_object(obj);
			return(&obj->draw);
			
		case thing_type_weapon:
			obj=server.obj_list.objs[script->attach.obj_idx];
			weap=obj->weap_list.weaps[script->attach.weap_idx];
			model_draw_setup_weapon(obj,weap,FALSE,FALSE);
			if (weap->dual.in_dual) return(&weap->draw_dual);
			return(&weap->draw);
			
		case thing_type_projectile:
			if (script->attach.proj_idx==-1) break;			// this happens when we are on a projectile setup, not a projectile
			proj=server.proj_list.projs[script->attach.proj_idx];
			model_draw_setup_projectile(proj);
			return(&proj->draw);
			
	}
	
	*exception=script_create_exception(cx,"There is no model to get bone positions from");
	return(NULL);
}

/* =======================================================

      Bone Exceptions
      
======================================================= */

JSValueRef js_model_bone_name_exception(JSContextRef cx,char *pose_name,char *bone_name)
{
	char			err_str[256];

	sprintf(err_str,"Named pose (%s) or bone (%s) does not exist",pose_name,bone_name);
	return(script_create_exception(cx,err_str));
}

/* =======================================================

      Model Bone Functions
      
======================================================= */

JSValueRef js_model_bone_find_offset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char				pose_name[name_str_len],bone_name[name_str_len];
	d3pnt				pnt;
	model_draw			*draw;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
		// get proper draw setup
		
	draw=script_bone_function_setup(cx,j_obj,exception);
	if (draw==NULL) return(script_null_to_value(cx));
	
		// get bone offset
		
	script_value_to_string(cx,argv[0],pose_name,name_str_len);
	script_value_to_string(cx,argv[1],bone_name,name_str_len);
	
	if (!model_find_bone_offset(draw,pose_name,bone_name,&pnt)) {
		*exception=js_model_bone_name_exception(cx,pose_name,bone_name);
		return(script_null_to_value(cx));
	}
	
	return(script_point_to_value(cx,&pnt));
}

JSValueRef js_model_bone_find_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char				pose_name[name_str_len],bone_name[name_str_len];
	d3pnt				pnt;
	model_draw			*draw;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
		// get proper draw setup
		
	draw=script_bone_function_setup(cx,j_obj,exception);
	if (draw==NULL) return(script_null_to_value(cx));
	
		// get bone position
	
	script_value_to_string(cx,argv[0],pose_name,name_str_len);
	script_value_to_string(cx,argv[1],bone_name,name_str_len);
	
	if (!model_find_bone_position(draw,pose_name,bone_name,&pnt)) {
		*exception=js_model_bone_name_exception(cx,pose_name,bone_name);
		return(script_null_to_value(cx));
	}
	
	return(script_point_to_value(cx,&pnt));
}

JSValueRef js_model_bone_get_brightness_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			pose_name[name_str_len],bone_name[name_str_len];
	float			bright;
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
		// get proper draw setup
		
	draw=script_bone_function_setup(cx,j_obj,exception);
	if (draw==NULL) return(script_null_to_value(cx));
	
		// get bone light

	script_value_to_string(cx,argv[0],pose_name,name_str_len);
	script_value_to_string(cx,argv[1],bone_name,name_str_len);
	
	if (!model_get_bone_brightness(draw,pose_name,bone_name,&bright)) {
		*exception=js_model_bone_name_exception(cx,pose_name,bone_name);
		return(script_null_to_value(cx));
	}
	
	return(script_float_to_value(cx,bright));
}

/* =======================================================

      Dynamic Bone Functions
      
======================================================= */

JSValueRef js_model_bone_set_dynamic_rotate_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			bone_name[name_str_len],err_str[256];
	d3ang			ang;
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
		// get proper draw setup
		
	draw=script_bone_function_setup(cx,j_obj,exception);
	if (draw==NULL) return(script_null_to_value(cx));

		// create dynamic bone

	script_value_to_string(cx,argv[0],bone_name,name_str_len);
	ang.x=script_value_to_float(cx,argv[1]);
	ang.z=script_value_to_float(cx,argv[2]);
	ang.y=script_value_to_float(cx,argv[3]);

	if (!model_dynamic_bone_set_rotate(draw,bone_name,&ang,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_model_bone_set_dynamic_move_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			bone_name[name_str_len],err_str[256];
	d3pnt			pnt;
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
		// get proper draw setup
		
	draw=script_bone_function_setup(cx,j_obj,exception);
	if (draw==NULL) return(script_null_to_value(cx));

		// create dynamic bone

	script_value_to_string(cx,argv[0],bone_name,name_str_len);
	pnt.x=script_value_to_int(cx,argv[1]);
	pnt.z=script_value_to_int(cx,argv[2]);
	pnt.y=script_value_to_int(cx,argv[3]);

	if (!model_dynamic_bone_set_move(draw,bone_name,&pnt,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_model_bone_set_dynamic_resize_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			bone_name[name_str_len],err_str[256];
	float			resize;
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	if (!script_check_fail_in_construct(cx,func,j_obj,exception)) return(script_null_to_value(cx));
	
		// get proper draw setup
		
	draw=script_bone_function_setup(cx,j_obj,exception);
	if (draw==NULL) return(script_null_to_value(cx));

		// create dynamic bone

	script_value_to_string(cx,argv[0],bone_name,name_str_len);
	resize=script_value_to_float(cx,argv[1]);

	if (!model_dynamic_bone_set_resize(draw,bone_name,resize,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}
