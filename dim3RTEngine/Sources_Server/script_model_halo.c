/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.halo object

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

JSValueRef js_model_halo_get_index(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_halo_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_halo_get_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_model_halo_set_index(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_model_halo_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_model_halo_set_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

extern server_type		server;
extern js_type			js;
extern iface_type		iface;

JSStaticValue 		model_halo_props[]={
							{"index",				js_model_halo_get_index,				js_model_halo_set_index,			kJSPropertyAttributeDontDelete},
							{"on",					js_model_halo_get_on,					js_model_halo_set_on,				kJSPropertyAttributeDontDelete},
							{"name",				js_model_halo_get_name,					js_model_halo_set_name,				kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			model_halo_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_halo_object(void)
{
	model_halo_class=script_create_class("model_halo_class",model_halo_props,NULL);
}

void script_free_model_halo_object(void)
{
	script_free_class(model_halo_class);
}

JSObjectRef script_add_model_halo_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,model_halo_class,"halo",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_model_halo_get_index(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw(j_obj);
	halo=&draw->halos[draw->script_halo_idx];
	
	return(script_int_to_value(cx,draw->script_halo_idx));
}

JSValueRef js_model_halo_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw(j_obj);
	halo=&draw->halos[draw->script_halo_idx];
	
	return(script_bool_to_value(cx,halo->on));
}

JSValueRef js_model_halo_get_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw(j_obj);
	halo=&draw->halos[draw->script_halo_idx];
	
	if (halo->idx==-1) return(script_null_to_value(cx));
	return(script_string_to_value(cx,iface.halo_list.halos[halo->idx].name));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_model_halo_set_index(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw(j_obj);
	halo=&draw->halos[draw->script_halo_idx];

	draw->script_halo_idx=script_value_to_int(cx,vp);
	if ((draw->script_halo_idx<0) || (draw->script_halo_idx>=max_model_halo)) draw->script_halo_idx=0;

	return(TRUE);
}

bool js_model_halo_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw(j_obj);
	halo=&draw->halos[draw->script_halo_idx];

	halo->on=script_value_to_bool(cx,vp);

	return(TRUE);
}

bool js_model_halo_set_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	int					idx;
	char				halo_name[name_str_len],err_str[256];
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw(j_obj);
	halo=&draw->halos[draw->script_halo_idx];

	script_value_to_string(cx,vp,halo_name,name_str_len);
	idx=iface_halo_find(&iface,halo_name);
	
	if (idx==-1) {
		sprintf(err_str,"No halo exists with this name: %s",halo_name);
		*exception=script_create_exception(cx,err_str);
		return(FALSE);
	}
	
	halo->idx=idx;

	return(TRUE);
}

