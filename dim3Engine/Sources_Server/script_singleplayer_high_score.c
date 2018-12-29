/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: multiplayer.score object

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

JSValueRef js_singleplayer_high_score_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	singleplayer_high_score_functions[]={
							{"add",			js_singleplayer_high_score_add_func,		kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			singleplayer_high_score_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_singleplayer_high_score_object(void)
{
	singleplayer_high_score_class=script_create_class("singleplayer_high_score_class",NULL,singleplayer_high_score_functions);
}

void script_free_singleplayer_high_score_object(void)
{
	script_free_class(singleplayer_high_score_class);
}

JSObjectRef script_add_singleplayer_high_score_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,singleplayer_high_score_class,"highScore",script_idx));
}

/* =======================================================

      High Score Functions
      
======================================================= */

JSValueRef js_singleplayer_high_score_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	/* supergumba -- implement

	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(FALSE);

	return(script_int_to_value(cx,obj->score.score));
	*/

	return(script_null_to_value(cx));
}

