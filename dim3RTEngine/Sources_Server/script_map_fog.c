/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.fog object

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

#include "scripts.h"

extern map_type			map;
extern js_type			js;

JSValueRef js_map_fog_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_fog_get_count(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_fog_get_outerRadius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_fog_get_innerRadius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_fog_get_high(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_fog_get_drop(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_fog_get_textureIndex(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_fog_get_speed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_fog_get_textureXFact(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_fog_get_textureYFact(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_fog_get_alpha(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_fog_get_useSolidColor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_map_fog_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_map_fog_set_count(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_map_fog_set_outerRadius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_map_fog_set_innerRadius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_map_fog_set_high(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_map_fog_set_drop(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_map_fog_set_textureIndex(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_map_fog_set_speed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_map_fog_set_textureXFact(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_map_fog_set_textureYFact(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_map_fog_set_alpha(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_map_fog_set_useSolidColor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		map_fog_props[]={
							{"on",					js_map_fog_get_on,					js_map_fog_set_on,					kJSPropertyAttributeDontDelete},
							{"layerCount",			js_map_fog_get_count,				js_map_fog_set_count,				kJSPropertyAttributeDontDelete},
							{"outerRadius",			js_map_fog_get_outerRadius,			js_map_fog_set_outerRadius,			kJSPropertyAttributeDontDelete},
							{"innerRadius",			js_map_fog_get_innerRadius,			js_map_fog_set_innerRadius,			kJSPropertyAttributeDontDelete},
							{"height",				js_map_fog_get_high,				js_map_fog_set_high,				kJSPropertyAttributeDontDelete},
							{"drop",				js_map_fog_get_drop,				js_map_fog_set_drop,				kJSPropertyAttributeDontDelete},
							{"textureIndex",		js_map_fog_get_textureIndex,		js_map_fog_set_textureIndex,		kJSPropertyAttributeDontDelete},
							{"speed",				js_map_fog_get_speed,				js_map_fog_set_speed,				kJSPropertyAttributeDontDelete},
							{"textureXFact",		js_map_fog_get_textureXFact,		js_map_fog_set_textureXFact,		kJSPropertyAttributeDontDelete},
							{"textureYFact",		js_map_fog_get_textureYFact,		js_map_fog_set_textureYFact,		kJSPropertyAttributeDontDelete},
							{"alpha",				js_map_fog_get_alpha,				js_map_fog_set_alpha,				kJSPropertyAttributeDontDelete},
							{"useSolidColor",		js_map_fog_get_useSolidColor,		js_map_fog_set_useSolidColor,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			map_fog_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_fog_object(void)
{
	map_fog_class=script_create_class("map_fog_class",map_fog_props,NULL);
}

void script_free_map_fog_object(void)
{
	script_free_class(map_fog_class);
}

JSObjectRef script_add_map_fog_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,map_fog_class,"fog",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_map_fog_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_bool_to_value(cx,map.fog.on));
}

JSValueRef js_map_fog_get_count(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,map.fog.count));
}

JSValueRef js_map_fog_get_outerRadius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,map.fog.outer_radius));
}

JSValueRef js_map_fog_get_innerRadius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,map.fog.inner_radius));
}

JSValueRef js_map_fog_get_high(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,map.fog.high));
}

JSValueRef js_map_fog_get_drop(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,map.fog.drop));
}

JSValueRef js_map_fog_get_textureIndex(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,map.fog.texture_idx));
}

JSValueRef js_map_fog_get_speed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_float_to_value(cx,map.fog.speed));
}

JSValueRef js_map_fog_get_textureXFact(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_float_to_value(cx,map.fog.txt_fact.x));
}

JSValueRef js_map_fog_get_textureYFact(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_float_to_value(cx,map.fog.txt_fact.y));
}

JSValueRef js_map_fog_get_alpha(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_float_to_value(cx,map.fog.alpha));
}

JSValueRef js_map_fog_get_useSolidColor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_bool_to_value(cx,map.fog.use_solid_color));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_map_fog_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.fog.on=script_value_to_bool(cx,vp);
	
	return(TRUE);
}

bool js_map_fog_set_count(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.fog.count=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_map_fog_set_outerRadius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.fog.outer_radius=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_map_fog_set_innerRadius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.fog.inner_radius=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_map_fog_set_high(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.fog.high=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_map_fog_set_drop(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.fog.drop=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_map_fog_set_textureIndex(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.fog.texture_idx=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_map_fog_set_speed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.fog.speed=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_map_fog_set_textureXFact(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.fog.txt_fact.x=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_map_fog_set_textureYFact(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.fog.txt_fact.y=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_map_fog_set_alpha(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.fog.alpha=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_map_fog_set_useSolidColor(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	map.fog.use_solid_color=script_value_to_bool(cx,vp);
	
	return(TRUE);
}


