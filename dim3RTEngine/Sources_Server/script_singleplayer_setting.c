/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: singleplayer.setting object

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

extern server_type			server;
extern iface_type			iface;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

JSValueRef js_singleplayer_setting_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_singleplayer_setting_get_skill(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_singleplayer_setting_check_option_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		singleplayer_setting_props[]={
							{"on",					js_singleplayer_setting_get_on,					NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"skill",				js_singleplayer_setting_get_skill,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	singleplayer_setting_functions[]={
							{"checkOption",			js_singleplayer_setting_check_option_func,		kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			singleplayer_setting_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_singleplayer_setting_object(void)
{
	singleplayer_setting_class=script_create_class("singleplayer_setting_class",singleplayer_setting_props,singleplayer_setting_functions);
}

void script_free_singleplayer_setting_object(void)
{
	script_free_class(singleplayer_setting_class);
}

JSObjectRef script_add_singleplayer_setting_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,singleplayer_setting_class,"setting",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_singleplayer_setting_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_bool_to_value(cx,net_setup.mode==net_mode_none));
}

JSValueRef js_singleplayer_setting_get_skill(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	if (net_setup.mode!=net_mode_none) return(script_int_to_value(cx,skill_medium));

	return(script_int_to_value(cx,server.skill));
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_singleplayer_setting_check_option_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n;
	bool			on;
	char			name[name_str_len];
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
		// all options are false if in networking
		
	if (net_setup.mode!=net_mode_none) return(script_bool_to_value(cx,FALSE));

		// find if option is on

	script_value_to_string(cx,argv[0],name,name_str_len);

	for (n=0;n!=iface.singleplayer.option_list.noption;n++) {
		if (strcasecmp(name,iface.singleplayer.option_list.options[n].name)==0) {
			on=((server.option_flags&(0x1<<n))!=0x0);
			return(script_bool_to_value(cx,on));
		}
	}

	return(script_bool_to_value(cx,FALSE));
}

