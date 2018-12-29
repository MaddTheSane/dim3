/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.movement object

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

extern map_type			map;
extern js_type			js;

JSValueRef js_map_movement_start_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_movement_start_reverse_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_movement_start_or_thaw_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_movement_freeze_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_movement_thaw_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_movement_is_looping_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	map_movement_functions[]={
							{"start",				js_map_movement_start_func,			kJSPropertyAttributeDontDelete},
							{"startReverse",		js_map_movement_start_reverse_func,	kJSPropertyAttributeDontDelete},
							{"startOrThaw",			js_map_movement_start_or_thaw_func,	kJSPropertyAttributeDontDelete},
							{"freeze",				js_map_movement_freeze_func,		kJSPropertyAttributeDontDelete},
							{"thaw",				js_map_movement_thaw_func,			kJSPropertyAttributeDontDelete},
							{"isLooping",			js_map_movement_is_looping_func,	kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			map_movement_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_movement_object(void)
{
	map_movement_class=script_create_class("map_movement_class",NULL,map_movement_functions);
}

void script_free_map_movement_object(void)
{
	script_free_class(map_movement_class);
}

JSObjectRef script_add_map_movement_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,map_movement_class,"movement",script_idx));
}

/* =======================================================

      Movement Function
      
======================================================= */

JSValueRef js_map_movement_start_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				script_idx,movement_idx;
	script_type		*script;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	script_idx=(int)JSObjectGetPrivate(j_obj);
	script=js.script_list.scripts[script_idx];

	movement_idx=script_find_map_movement_from_name(cx,argv[0],exception);
	if (movement_idx!=-1) map_movements_script_start(script->attach.obj_idx,movement_idx,FALSE);

	return(script_null_to_value(cx));
}

JSValueRef js_map_movement_start_reverse_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				script_idx,movement_idx;
	script_type		*script;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	script_idx=(int)JSObjectGetPrivate(j_obj);
	script=js.script_list.scripts[script_idx];
	
	movement_idx=script_find_map_movement_from_name(cx,argv[0],exception);
	if (movement_idx!=-1) map_movements_script_start(script->attach.obj_idx,movement_idx,TRUE);

	return(script_null_to_value(cx));
}

JSValueRef js_map_movement_start_or_thaw_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				script_idx,movement_idx;
	script_type		*script;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	script_idx=(int)JSObjectGetPrivate(j_obj);
	script=js.script_list.scripts[script_idx];
	
	movement_idx=script_find_map_movement_from_name(cx,argv[0],exception);
	if (movement_idx!=-1) map_movements_script_start_or_thaw(script->attach.obj_idx,movement_idx);

	return(script_null_to_value(cx));
}

JSValueRef js_map_movement_freeze_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				movement_idx;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	movement_idx=script_find_map_movement_from_name(cx,argv[0],exception);
	if (movement_idx!=-1) map_movements_script_freeze(movement_idx);

	return(script_null_to_value(cx));
}

JSValueRef js_map_movement_thaw_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				movement_idx;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	movement_idx=script_find_map_movement_from_name(cx,argv[0],exception);
	if (movement_idx!=-1) map_movements_script_thaw(movement_idx);

	return(script_null_to_value(cx));
}

JSValueRef js_map_movement_is_looping_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				movement_idx;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	movement_idx=script_find_map_movement_from_name(cx,argv[0],exception);
	if (movement_idx!=-1) return(script_bool_to_value(cx,map_movements_script_is_looping(movement_idx)));

	return(script_null_to_value(cx));
}

