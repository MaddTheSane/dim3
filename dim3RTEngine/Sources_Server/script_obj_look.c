/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.look object

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

extern server_type		server;
extern js_type			js;

JSValueRef js_obj_look_get_upAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_look_get_downAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_look_get_effectWeapons(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_look_set_upAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_look_set_downAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_look_set_effectWeapons(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_look_get_look_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_look_set_look_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_look_set_look_at_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_look_props[]={
							{"upAngle",					js_obj_look_get_upAngle,				js_obj_look_set_upAngle,			kJSPropertyAttributeDontDelete},
							{"downAngle",				js_obj_look_get_downAngle,				js_obj_look_set_downAngle,			kJSPropertyAttributeDontDelete},
							{"effectWeapons",			js_obj_look_get_effectWeapons,			js_obj_look_set_effectWeapons,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	obj_look_functions[]={
							{"getLook",					js_obj_look_get_look_func,				kJSPropertyAttributeDontDelete},
							{"setLook",					js_obj_look_set_look_func,				kJSPropertyAttributeDontDelete},
							{"setLookAt",				js_obj_look_set_look_at_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_look_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_look_object(void)
{
	obj_look_class=script_create_class("obj_look_class",obj_look_props,obj_look_functions);
}

void script_free_obj_look_object(void)
{
	script_free_class(obj_look_class);
}

JSObjectRef script_add_obj_look_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_look_class,"look",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_look_get_upAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->look.up_angle));
}

JSValueRef js_obj_look_get_downAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->look.down_angle));
}

JSValueRef js_obj_look_get_effectWeapons(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_bool_to_value(cx,obj->look.effect_weapons));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_look_set_upAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->look.up_angle=fabsf(script_value_to_float(cx,vp));
	
	return(TRUE);
}

bool js_obj_look_set_downAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->look.down_angle=fabsf(script_value_to_float(cx,vp));
	
	return(TRUE);
}

bool js_obj_look_set_effectWeapons(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->look.effect_weapons=script_value_to_bool(cx,vp);
	
	return(TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_obj_look_get_look_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->view_ang.x));
}

JSValueRef js_obj_look_set_look_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	obj->view_ang.x=script_value_to_float(cx,argv[0]);

	return(script_null_to_value(cx));
}

JSValueRef js_obj_look_set_look_at_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				dist,y,look_y;
	float			ang;
	obj_type		*obj,*look_obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	
	look_obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (look_obj==NULL) return(script_null_to_value(cx));
	
		// no change if within object size
		
	y=obj->pnt.y-(obj->size.y>>1);
	look_y=look_obj->pnt.y-(look_obj->size.y>>1);
	
	if (abs(y-look_y)<(look_obj->size.y>>1)) {
		obj->view_ang.x=0.0f;
		return(script_null_to_value(cx));
	}
	
		// angle to object
		
	dist=distance_2D_get(obj->pnt.x,obj->pnt.z,look_obj->pnt.x,look_obj->pnt.z);
	ang=angle_find(y,dist,look_y,0);
	
	if (ang>180.0f) ang-=360.0f;
	
	if (obj->idx!=server.player_obj_idx) ang=-ang;
	obj->view_ang.x=ang;

	return(script_null_to_value(cx));
}
