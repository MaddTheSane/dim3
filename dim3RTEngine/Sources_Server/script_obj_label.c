/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.label object

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

#include "interface.h"
#include "scripts.h"
#include "objects.h"

extern map_type				map;
extern server_type			server;
extern iface_type			iface;
extern js_type				js;

JSValueRef js_obj_label_get_text(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_label_get_bitmap(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_label_get_bar(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_label_set_text(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_label_set_textSize(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_label_set_bitmap(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_label_set_bar(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		obj_label_props[]={
							{"text",				js_obj_label_get_text,				js_obj_label_set_text,			kJSPropertyAttributeDontDelete},
							{"bitmap",				js_obj_label_get_bitmap,			js_obj_label_set_bitmap,		kJSPropertyAttributeDontDelete},
							{"bar",					js_obj_label_get_bar,				js_obj_label_set_bar,			kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSClassRef			obj_label_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_label_object(void)
{
	obj_label_class=script_create_class("obj_label_class",obj_label_props,NULL);
}

void script_free_obj_label_object(void)
{
	script_free_class(obj_label_class);
}

JSObjectRef script_add_obj_label_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_label_class,"label",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_label_get_text(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	if (obj->label.text.str[0]==0x0) return(script_null_to_value(cx));

	return(script_string_to_value(cx,obj->label.text.str));
}

JSValueRef js_obj_label_get_bitmap(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	if (obj->label.bitmap.idx==-1) return(script_null_to_value(cx));

	return(script_string_to_value(cx,iface.label_list.labels[obj->label.bitmap.idx].name));
}

JSValueRef js_obj_label_get_bar(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_float_to_value(cx,obj->label.bar.value));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_label_set_text(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	if (script_is_value_null(cx,vp)) {
		obj->label.text.str[0]=0x0;
		return(TRUE);
	}

	script_value_to_string(cx,vp,obj->label.text.str,32);
	
	return(TRUE);
}

bool js_obj_label_set_bitmap(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	int				idx;
	char			label_name[name_str_len],err_str[256];
	obj_type		*obj;

	obj=object_get_attach(j_obj);

		// if set to NULL, then remove image

	if (script_is_value_null(cx,vp)) {
		obj->label.bitmap.idx=-1;
		return(TRUE);
	}
	
		// load new image

	script_value_to_string(cx,vp,label_name,name_str_len);
	idx=iface_label_find(&iface,label_name);
	
	if (idx==-1) {
		sprintf(err_str,"No label exists with this name: %s",label_name);
		*exception=script_create_exception(cx,err_str);
		return(FALSE);
	}
	
	obj->label.bitmap.idx=idx;

	return(TRUE);
}

bool js_obj_label_set_bar(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_get_attach(j_obj);
	obj->label.bar.value=script_value_to_float(cx,vp);
	
	return(TRUE);
}
