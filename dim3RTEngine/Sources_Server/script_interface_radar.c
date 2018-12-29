/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.radar object

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
extern setup_type		setup;

JSValueRef js_interface_radar_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_interface_radar_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_interface_radar_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_interface_radar_get_displayRadius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_interface_radar_get_viewRadius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_interface_radar_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_interface_radar_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_interface_radar_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_interface_radar_set_displayRadius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_interface_radar_set_viewRadius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		interface_radar_props[]={
							{"on",					js_interface_radar_get_on,				js_interface_radar_set_on,				kJSPropertyAttributeDontDelete},
							{"x",					js_interface_radar_get_x,				js_interface_radar_set_x,				kJSPropertyAttributeDontDelete},
							{"y",					js_interface_radar_get_y,				js_interface_radar_set_y,				kJSPropertyAttributeDontDelete},
							{"displayRadius",		js_interface_radar_get_displayRadius,	js_interface_radar_set_displayRadius,	kJSPropertyAttributeDontDelete},
							{"viewRadius",			js_interface_radar_get_viewRadius,		js_interface_radar_set_viewRadius,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			interface_radar_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_radar_object(void)
{
	interface_radar_class=script_create_class("interface_radar_class",interface_radar_props,NULL);
}

void script_free_interface_radar_object(void)
{
	script_free_class(interface_radar_class);
}

JSObjectRef script_add_interface_radar_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,interface_radar_class,"radar",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_interface_radar_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_bool_to_value(cx,iface.radar.on));
}

JSValueRef js_interface_radar_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,iface.radar.pnt.x));
}

JSValueRef js_interface_radar_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,iface.radar.pnt.y));
}

JSValueRef js_interface_radar_get_displayRadius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,iface.radar.display_radius));
}

JSValueRef js_interface_radar_get_viewRadius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,iface.radar.view_radius));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_interface_radar_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	iface.radar.on=script_value_to_bool(cx,vp);
	
	return(TRUE);
}

bool js_interface_radar_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	iface.radar.pnt.x=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_interface_radar_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	iface.radar.pnt.y=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_interface_radar_set_displayRadius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	iface.radar.display_radius=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_interface_radar_set_viewRadius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	iface.radar.view_radius=script_value_to_int(cx,vp);
	
	return(TRUE);
}

