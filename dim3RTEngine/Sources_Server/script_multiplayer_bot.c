/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: multiplayer.bot object

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
#include "objects.h"
#include "scripts.h"

extern server_type		server;
extern js_type			js;
extern setup_type		setup;

JSValueRef js_multiplayer_bot_get_skill(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_multiplayer_bot_get_from_min_max_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		multiplayer_bot_props[]={
							{"skill",				js_multiplayer_bot_get_skill,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	multiplayer_bot_functions[]={
							{"getFromMinMax",		js_multiplayer_bot_get_from_min_max_func,	kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			multiplayer_bot_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_multiplayer_bot_object(void)
{
	multiplayer_bot_class=script_create_class("multiplayer_bot_class",multiplayer_bot_props,multiplayer_bot_functions);
}

void script_free_multiplayer_bot_object(void)
{
	script_free_class(multiplayer_bot_class);
}

JSObjectRef script_add_multiplayer_bot_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,multiplayer_bot_class,"bot",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_multiplayer_bot_get_skill(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,setup.network.bot.skill));
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_multiplayer_bot_get_from_min_max_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			skill,min,max;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	min=script_value_to_float(cx,argv[0]);
	max=script_value_to_float(cx,argv[1]);
	
	skill=(float)setup.network.bot.skill;

	return(script_float_to_value(cx,min+(((max-min)*skill)/4.0f)));
}

