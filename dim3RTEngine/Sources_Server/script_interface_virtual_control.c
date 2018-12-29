/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.virtualControl object

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

extern iface_type		iface;
extern js_type			js;

JSValueRef js_interface_virtual_control_showAll_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_virtual_control_hideAll_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_virtual_control_moveStick_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_virtual_control_showStick_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_virtual_control_hideStick_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_virtual_control_moveButton_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_virtual_control_showButton_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_virtual_control_hideButton_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	interface_virtual_control_functions[]={
							{"showAll",				js_interface_virtual_control_showAll_func,		kJSPropertyAttributeDontDelete},
							{"hideAll",				js_interface_virtual_control_hideAll_func,		kJSPropertyAttributeDontDelete},
							{"moveStick",			js_interface_virtual_control_moveStick_func,	kJSPropertyAttributeDontDelete},
							{"showStick",			js_interface_virtual_control_showStick_func,	kJSPropertyAttributeDontDelete},
							{"hideStick",			js_interface_virtual_control_hideStick_func,	kJSPropertyAttributeDontDelete},
							{"moveButton",			js_interface_virtual_control_moveButton_func,	kJSPropertyAttributeDontDelete},
							{"showButton",			js_interface_virtual_control_showButton_func,	kJSPropertyAttributeDontDelete},
							{"hideButton",			js_interface_virtual_control_hideButton_func,	kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			interface_virtual_control_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_virtual_control_object(void)
{
	interface_virtual_control_class=script_create_class("interface_virtual_control_class",NULL,interface_virtual_control_functions);
}

void script_free_interface_virtual_control_object(void)
{
	script_free_class(interface_virtual_control_class);
}

JSObjectRef script_add_interface_virtual_control_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,interface_virtual_control_class,"virtualControl",script_idx));
}

/* =======================================================

      Virtual Control Functions
      
======================================================= */

JSValueRef js_interface_virtual_control_showAll_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	for (n=0;n!=max_virtual_stick;n++) {
		if (iface.virtual_control.sticks[n].on) iface.virtual_control.sticks[n].show=TRUE;
	}
	
	for (n=0;n!=max_virtual_button;n++) {
		if (iface.virtual_control.buttons[n].on) iface.virtual_control.buttons[n].show=TRUE;
	}

	return(script_null_to_value(cx));
}

JSValueRef js_interface_virtual_control_hideAll_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));

	for (n=0;n!=max_virtual_stick;n++) {
		if (iface.virtual_control.sticks[n].on) iface.virtual_control.sticks[n].show=FALSE;
	}
	
	for (n=0;n!=max_virtual_button;n++) {
		if (iface.virtual_control.buttons[n].on) iface.virtual_control.buttons[n].show=FALSE;
	}

	return(script_null_to_value(cx));
}

JSValueRef js_interface_virtual_control_moveStick_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	d3pnt			pnt;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	idx=script_value_to_int(cx,argv[0]);
	if ((idx<0) || (idx>=max_virtual_stick)) return(script_null_to_value(cx));
	
	script_value_to_2D_point(cx,argv[1],&pnt);
	
	iface.virtual_control.sticks[idx].pnt.x=pnt.x;
	iface.virtual_control.sticks[idx].pnt.y=pnt.y;

	return(script_null_to_value(cx));
}

JSValueRef js_interface_virtual_control_showStick_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	idx=script_value_to_int(cx,argv[0]);
	if ((idx<0) || (idx>=max_virtual_stick)) return(script_null_to_value(cx));
	
	iface.virtual_control.sticks[idx].show=TRUE;
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_virtual_control_hideStick_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	idx=script_value_to_int(cx,argv[0]);
	if ((idx<0) || (idx>=max_virtual_stick)) return(script_null_to_value(cx));
	
	iface.virtual_control.sticks[idx].show=FALSE;

	return(script_null_to_value(cx));
}

JSValueRef js_interface_virtual_control_moveButton_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	d3pnt			pnt;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	idx=script_value_to_int(cx,argv[0]);
	if ((idx<0) || (idx>=max_virtual_button)) return(script_null_to_value(cx));
	
	script_value_to_2D_point(cx,argv[1],&pnt);
	
	iface.virtual_control.buttons[idx].pnt.x=pnt.x;
	iface.virtual_control.buttons[idx].pnt.y=pnt.y;

	return(script_null_to_value(cx));
}

JSValueRef js_interface_virtual_control_showButton_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	idx=script_value_to_int(cx,argv[0]);
	if ((idx<0) || (idx>=max_virtual_button)) return(script_null_to_value(cx));
	
	iface.virtual_control.buttons[idx].show=TRUE;

	return(script_null_to_value(cx));
}

JSValueRef js_interface_virtual_control_hideButton_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	idx=script_value_to_int(cx,argv[0]);
	if ((idx<0) || (idx>=max_virtual_button)) return(script_null_to_value(cx));
	
	iface.virtual_control.buttons[idx].show=FALSE;

	return(script_null_to_value(cx));
}




