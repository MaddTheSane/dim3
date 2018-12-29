/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.spot object

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

extern map_type			map;
extern js_type			js;

JSValueRef js_map_spot_get_count(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_spot_find_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_spot_get_name_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_spot_get_type_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_spot_get_script_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_spot_get_parameter_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_spot_get_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_spot_get_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		map_spot_props[]={
							{"count",				js_map_spot_get_count,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	map_spot_functions[]={
							{"find",				js_map_spot_find_func,				kJSPropertyAttributeDontDelete},
							{"getName",				js_map_spot_get_name_func,			kJSPropertyAttributeDontDelete},
							{"getType",				js_map_spot_get_type_func,			kJSPropertyAttributeDontDelete},
							{"getScript",			js_map_spot_get_script_func,		kJSPropertyAttributeDontDelete},
							{"getParameter",		js_map_spot_get_parameter_func,		kJSPropertyAttributeDontDelete},
							{"getPosition",			js_map_spot_get_position_func,		kJSPropertyAttributeDontDelete},
							{"getAngle",			js_map_spot_get_angle_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			map_spot_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_spot_object(void)
{
	map_spot_class=script_create_class("map_spot_class",map_spot_props,map_spot_functions);
}

void script_free_map_spot_object(void)
{
	script_free_class(map_spot_class);
}

JSObjectRef script_add_map_spot_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,map_spot_class,"spot",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_map_spot_get_count(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,map.nspot));
}

/* =======================================================

      Spot Finds
      
======================================================= */

JSValueRef js_map_spot_find_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int			type;
	char		name[name_str_len];
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	if ((script_is_value_null(cx,argv[0])) && (script_is_value_null(cx,argv[1]))) {
		return(script_int_to_value(cx,-1));
	}
	
	if ((!script_is_value_null(cx,argv[0])) && (!script_is_value_null(cx,argv[1]))) {
		script_value_to_string(cx,argv[0],name,name_str_len);
		type=script_value_to_int(cx,argv[1])-sd_spot_type_object;
		return(script_int_to_value(cx,map_find_spot(&map,name,type)));
	}
	
	if (!script_is_value_null(cx,argv[0])) {
		script_value_to_string(cx,argv[0],name,name_str_len);
		return(script_int_to_value(cx,map_find_spot(&map,name,-1)));
	}
		
	type=script_value_to_int(cx,argv[1])-sd_spot_type_object;
	return(script_int_to_value(cx,map_find_spot(&map,NULL,type)));
}

/* =======================================================

      Spot Info
      
======================================================= */

JSValueRef js_map_spot_get_name_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	spot_type	*spot;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	spot=script_find_spot_from_idx_arg(cx,argv[0],exception);
	if (spot==NULL) return(script_null_to_value(cx));
	
	return(script_string_to_value(cx,spot->name));
}

JSValueRef js_map_spot_get_type_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	spot_type	*spot;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	spot=script_find_spot_from_idx_arg(cx,argv[0],exception);
	if (spot==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,spot->type+sd_spot_type_object));
}

JSValueRef js_map_spot_get_script_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	spot_type	*spot;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	spot=script_find_spot_from_idx_arg(cx,argv[0],exception);
	if (spot==NULL) return(script_null_to_value(cx));
	
	return(script_string_to_value(cx,spot->script));
}

JSValueRef js_map_spot_get_parameter_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	spot_type	*spot;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	spot=script_find_spot_from_idx_arg(cx,argv[0],exception);
	if (spot==NULL) return(script_null_to_value(cx));
	
	return(script_string_to_value(cx,spot->params));
}

/* =======================================================

      Spot Position and Angle
      
======================================================= */

JSValueRef js_map_spot_get_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	spot_type   *spot;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
		// find spot
	
	spot=script_find_spot_from_idx_arg(cx,argv[0],exception);
	if (spot==NULL) return(script_null_to_value(cx));
	
		// get position
		
	return(script_point_to_value(cx,&spot->pnt));
}

JSValueRef js_map_spot_get_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	spot_type   *spot;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
		// find spot
	
	spot=script_find_spot_from_idx_arg(cx,argv[0],exception);
	if (spot==NULL) return(script_null_to_value(cx));
	
		// get position
		
	return(script_angle_to_value(cx,&spot->ang));
}

