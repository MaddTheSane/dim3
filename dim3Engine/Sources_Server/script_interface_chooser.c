/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.chooser object

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
#include "scripts.h"

extern server_type		server;
extern js_type			js;

JSValueRef js_interface_chooser_start_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_chooser_showMesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_chooser_hideMesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	interface_chooser_functions[]={
							{"start",				js_interface_chooser_start_func,				kJSPropertyAttributeDontDelete},
							{"showMesh",			js_interface_chooser_showMesh_func,				kJSPropertyAttributeDontDelete},
							{"hideMesh",			js_interface_chooser_hideMesh_func,				kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			interface_chooser_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_chooser_object(void)
{
	interface_chooser_class=script_create_class("interface_chooser_class",NULL,interface_chooser_functions);
}

void script_free_interface_chooser_object(void)
{
	script_free_class(interface_chooser_class);
}

JSObjectRef script_add_interface_chooser_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,interface_chooser_class,"chooser",script_idx));
}

/* =======================================================

      Start Functions
      
======================================================= */

JSValueRef js_interface_chooser_start_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,cnt;
	char			name[name_str_len],err_str[256],
					sub_txt[max_chooser_sub_txt][max_chooser_text_data_sz];

	if (!script_check_param_at_least_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

		// get name

	script_value_to_string(cx,argv[0],name,name_str_len);

		// get any substitution text

	for (n=0;n!=max_chooser_sub_txt;n++) {
		sub_txt[n][0]=0x0;
	}

	cnt=argc-1;
	if (cnt<0) cnt=0;
	if (cnt>max_chooser_sub_txt) cnt=max_chooser_sub_txt;

	for (n=0;n!=cnt;n++) {
		script_value_to_string(cx,argv[n+1],sub_txt[n],max_chooser_text_data_sz);
	}

		// start chooser

	if (!chooser_setup(name,(char*)sub_txt,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_chooser_showMesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				id;
	char			name[name_str_len];

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	id=script_value_to_int(cx,argv[0]);
	script_value_to_string(cx,argv[1],name,name_str_len);

	return(script_bool_to_value(cx,element_model_show_mesh(id,name)));
}

JSValueRef js_interface_chooser_hideMesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				id;
	char			name[name_str_len];

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	id=script_value_to_int(cx,argv[0]);
	script_value_to_string(cx,argv[1],name,name_str_len);

	return(script_bool_to_value(cx,element_model_hide_mesh(id,name)));
}

