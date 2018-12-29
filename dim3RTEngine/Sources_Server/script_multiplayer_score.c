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

JSValueRef js_multiplayer_score_get_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_multiplayer_score_get_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	multiplayer_score_functions[]={
							{"getObject",			js_multiplayer_score_get_object_func,		kJSPropertyAttributeDontDelete},
							{"getTeam",				js_multiplayer_score_get_team_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			multiplayer_score_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_multiplayer_score_object(void)
{
	multiplayer_score_class=script_create_class("multiplayer_score_class",NULL,multiplayer_score_functions);
}

void script_free_multiplayer_score_object(void)
{
	script_free_class(multiplayer_score_class);
}

JSObjectRef script_add_multiplayer_score_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,multiplayer_score_class,"score",script_idx));
}

/* =======================================================

      Score Functions
      
======================================================= */

JSValueRef js_multiplayer_score_get_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(FALSE);

	return(script_int_to_value(cx,obj->score.score));
}

JSValueRef js_multiplayer_score_get_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,score,team_idx;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	team_idx=script_value_to_int(cx,argv[0])-sd_team_none;

		// add up team scores

	score=0;

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if ((obj->type==object_type_player) || (obj->type==object_type_remote_player) || (obj->type==object_type_bot_multiplayer)) {
			if (obj->team_idx==team_idx) score+=obj->score.score;
		}
	}

		// return team score

	return(script_int_to_value(cx,score));
}

