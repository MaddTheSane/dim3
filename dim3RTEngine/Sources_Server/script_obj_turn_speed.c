/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.turnSpeed object

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

extern js_type			js;

JSValueRef js_obj_turn_speed_get_facingWalk(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_turn_speed_get_motionWalk(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_turn_speed_get_facingRun(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_turn_speed_get_motionRun(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_turn_speed_get_facingCrawl(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_turn_speed_get_motionCrawl(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_turn_speed_get_facingAir(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_turn_speed_get_motionAir(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_turn_speed_get_key(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_turn_speed_get_topDownAngleOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_turn_speed_set_facingWalk(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_turn_speed_set_motionWalk(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_turn_speed_set_facingRun(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_turn_speed_set_motionRun(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_turn_speed_set_facingCrawl(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_turn_speed_set_motionCrawl(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_turn_speed_set_facingAir(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_turn_speed_set_motionAir(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_turn_speed_set_key(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_turn_speed_set_topDownAngleOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		obj_turn_speed_props[]={
							{"facingWalk",				js_obj_turn_speed_get_facingWalk,			js_obj_turn_speed_set_facingWalk,			kJSPropertyAttributeDontDelete},
							{"motionWalk",				js_obj_turn_speed_get_motionWalk,			js_obj_turn_speed_set_motionWalk,			kJSPropertyAttributeDontDelete},
							{"facingRun",				js_obj_turn_speed_get_facingRun,			js_obj_turn_speed_set_facingRun,			kJSPropertyAttributeDontDelete},
							{"motionRun",				js_obj_turn_speed_get_motionRun,			js_obj_turn_speed_set_motionRun,			kJSPropertyAttributeDontDelete},
							{"facingCrawl",				js_obj_turn_speed_get_facingCrawl,			js_obj_turn_speed_set_facingCrawl,			kJSPropertyAttributeDontDelete},
							{"motionCrawl",				js_obj_turn_speed_get_motionCrawl,			js_obj_turn_speed_set_motionCrawl,			kJSPropertyAttributeDontDelete},
							{"facingAir",				js_obj_turn_speed_get_facingAir,			js_obj_turn_speed_set_facingAir,			kJSPropertyAttributeDontDelete},
							{"motionAir",				js_obj_turn_speed_get_motionAir,			js_obj_turn_speed_set_motionAir,			kJSPropertyAttributeDontDelete},
							{"key",						js_obj_turn_speed_get_key,					js_obj_turn_speed_set_key,					kJSPropertyAttributeDontDelete},
							{"topDownAngleOffset",		js_obj_turn_speed_get_topDownAngleOffset,	js_obj_turn_speed_set_topDownAngleOffset,	kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			obj_turn_speed_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_turn_speed_object(void)
{
	obj_turn_speed_class=script_create_class("obj_turn_speed_class",obj_turn_speed_props,NULL);
}

void script_free_obj_turn_speed_object(void)
{
	script_free_class(obj_turn_speed_class);
}

JSObjectRef script_add_obj_turn_speed_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_turn_speed_class,"turnSpeed",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_turn_speed_get_facingWalk(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->turn.walk_speed));
}

JSValueRef js_obj_turn_speed_get_motionWalk(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->turn.walk_motion_speed));
}

JSValueRef js_obj_turn_speed_get_facingRun(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->turn.run_speed));
}

JSValueRef js_obj_turn_speed_get_motionRun(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->turn.run_motion_speed));
}

JSValueRef js_obj_turn_speed_get_facingCrawl(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->turn.crawl_speed));
}

JSValueRef js_obj_turn_speed_get_motionCrawl(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->turn.crawl_motion_speed));
}

JSValueRef js_obj_turn_speed_get_facingAir(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->turn.air_speed));
}

JSValueRef js_obj_turn_speed_get_motionAir(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->turn.air_motion_speed));
}

JSValueRef js_obj_turn_speed_get_key(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->turn.key_speed));
}

JSValueRef js_obj_turn_speed_get_topDownAngleOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->turn.top_down_ang_offset));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_turn_speed_set_facingWalk(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->turn.walk_speed=fabsf(script_value_to_float(cx,vp));

	return(TRUE);
}

bool js_obj_turn_speed_set_motionWalk(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->turn.walk_motion_speed=fabsf(script_value_to_float(cx,vp));

	return(TRUE);
}

bool js_obj_turn_speed_set_facingRun(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->turn.run_speed=fabsf(script_value_to_float(cx,vp));

	return(TRUE);
}

bool js_obj_turn_speed_set_motionRun(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->turn.run_motion_speed=fabsf(script_value_to_float(cx,vp));

	return(TRUE);
}

bool js_obj_turn_speed_set_facingCrawl(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->turn.crawl_speed=fabsf(script_value_to_float(cx,vp));

	return(TRUE);
}

bool js_obj_turn_speed_set_motionCrawl(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->turn.crawl_motion_speed=fabsf(script_value_to_float(cx,vp));

	return(TRUE);
}

bool js_obj_turn_speed_set_facingAir(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->turn.air_speed=fabsf(script_value_to_float(cx,vp));

	return(TRUE);
}

bool js_obj_turn_speed_set_motionAir(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->turn.air_motion_speed=fabsf(script_value_to_float(cx,vp));

	return(TRUE);
}

bool js_obj_turn_speed_set_key(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->turn.key_speed=fabsf(script_value_to_float(cx,vp));

	return(TRUE);
}

bool js_obj_turn_speed_set_topDownAngleOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->turn.top_down_ang_offset=fabsf(script_value_to_float(cx,vp));

	return(TRUE);
}

