/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.info object

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

JSValueRef js_map_info_get_file(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_info_get_title(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_info_get_author(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);

JSStaticValue 		map_info_props[]={
							{"file",				js_map_info_get_file,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"title",				js_map_info_get_title,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"author",				js_map_info_get_author,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			map_info_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_info_object(void)
{
	map_info_class=script_create_class("map_info_class",map_info_props,NULL);
}

void script_free_map_info_object(void)
{
	script_free_class(map_info_class);
}

JSObjectRef script_add_map_info_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,map_info_class,"info",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_map_info_get_file(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_string_to_value(cx,map.info.name));
}

JSValueRef js_map_info_get_title(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_string_to_value(cx,map.info.title));
}

JSValueRef js_map_info_get_author(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_string_to_value(cx,map.info.author));
}


