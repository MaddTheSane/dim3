/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.score object

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

extern map_type			map;
extern js_type			js;

JSValueRef js_obj_score_get_kill(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_score_get_death(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_score_get_suicide(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_score_get_goal(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_score_get_score(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_score_add_goal_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_score_props[]={
							{"kill",				js_obj_score_get_kill,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"death",				js_obj_score_get_death,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"suicide",				js_obj_score_get_suicide,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"goal",				js_obj_score_get_goal,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"score",				js_obj_score_get_score,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	obj_score_functions[]={
							{"addGoal",				js_obj_score_add_goal_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_score_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_score_object(void)
{
	obj_score_class=script_create_class("obj_score_class",obj_score_props,obj_score_functions);
}

void script_free_obj_score_object(void)
{
	script_free_class(obj_score_class);
}

JSObjectRef script_add_obj_score_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_score_class,"score",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_score_get_kill(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->score.kill));
}

JSValueRef js_obj_score_get_death(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->score.death));
}

JSValueRef js_obj_score_get_suicide(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->score.suicide));
}

JSValueRef js_obj_score_get_goal(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->score.goal));
}

JSValueRef js_obj_score_get_score(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->score.score));
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_obj_score_add_goal_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	object_score_goal(obj);

	return(script_null_to_value(cx));
}




