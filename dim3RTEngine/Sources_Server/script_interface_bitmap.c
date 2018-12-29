/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.bitmap object

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

extern iface_type		iface;
extern js_type			js;

JSValueRef js_interface_bitmap_show_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_bitmap_hide_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_bitmap_hide_all_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_bitmap_move_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_bitmap_move_relative_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_bitmap_resize_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_bitmap_set_alpha_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_bitmap_set_repeat_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_bitmap_set_flash_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_bitmap_set_rotate_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_bitmap_start_fade_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	interface_bitmap_functions[]={
							{"show",				js_interface_bitmap_show_func,				kJSPropertyAttributeDontDelete},
							{"hide",				js_interface_bitmap_hide_func,				kJSPropertyAttributeDontDelete},
							{"hideAll",				js_interface_bitmap_hide_all_func,			kJSPropertyAttributeDontDelete},
							{"move",				js_interface_bitmap_move_func,				kJSPropertyAttributeDontDelete},
							{"moveRelative",		js_interface_bitmap_move_relative_func,		kJSPropertyAttributeDontDelete},
							{"resize",				js_interface_bitmap_resize_func,			kJSPropertyAttributeDontDelete},
							{"setAlpha",			js_interface_bitmap_set_alpha_func,			kJSPropertyAttributeDontDelete},
							{"setRepeatCount",		js_interface_bitmap_set_repeat_count_func,	kJSPropertyAttributeDontDelete},
							{"setFlash",			js_interface_bitmap_set_flash_func,			kJSPropertyAttributeDontDelete},
							{"setRotate",			js_interface_bitmap_set_rotate_func,		kJSPropertyAttributeDontDelete},
							{"startFade",			js_interface_bitmap_start_fade_func,		kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			interface_bitmap_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_bitmap_object(void)
{
	interface_bitmap_class=script_create_class("interface_bitmap_class",NULL,interface_bitmap_functions);
}

void script_free_interface_bitmap_object(void)
{
	script_free_class(interface_bitmap_class);
}

JSObjectRef script_add_interface_bitmap_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,interface_bitmap_class,"bitmap",script_idx));
}

/* =======================================================

      Bitmap Functions
      
======================================================= */

JSValueRef js_interface_bitmap_show_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	iface_bitmap_type			*bitmap;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	bitmap=script_find_bitmap_from_name(cx,argv[0],exception);
	if (bitmap!=NULL) {
		bitmap->show=TRUE;
		bitmap->show_tick=game_time_get();		// use to synch animations with shows
	}

	return(script_null_to_value(cx));
}

JSValueRef js_interface_bitmap_hide_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	iface_bitmap_type			*bitmap;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	bitmap=script_find_bitmap_from_name(cx,argv[0],exception);
	if (bitmap!=NULL) bitmap->show=FALSE;
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_bitmap_hide_all_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	iface_bitmaps_hide_all(&iface);
	return(script_null_to_value(cx));
}

JSValueRef js_interface_bitmap_move_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	iface_bitmap_type			*bitmap;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	bitmap=script_find_bitmap_from_name(cx,argv[0],exception);
	if (bitmap!=NULL) script_value_to_2D_point(cx,argv[1],&bitmap->pnt);
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_bitmap_move_relative_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3pnt						pnt;
	iface_bitmap_type			*bitmap;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	bitmap=script_find_bitmap_from_name(cx,argv[0],exception);
	if (bitmap!=NULL) {
		script_value_to_2D_point(cx,argv[1],&pnt);
		bitmap->pnt.x+=pnt.x;
		bitmap->pnt.y+=pnt.y;
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_bitmap_resize_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	iface_bitmap_type			*bitmap;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	bitmap=script_find_bitmap_from_name(cx,argv[0],exception);
	if (bitmap!=NULL) {
		bitmap->size.x=script_value_to_int(cx,argv[1]);
		bitmap->size.y=script_value_to_int(cx,argv[2]);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_interface_bitmap_set_alpha_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	iface_bitmap_type			*bitmap;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	bitmap=script_find_bitmap_from_name(cx,argv[0],exception);
	if (bitmap!=NULL) bitmap->alpha=script_value_to_float(cx,argv[1]);
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_bitmap_set_repeat_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	iface_bitmap_type			*bitmap;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	bitmap=script_find_bitmap_from_name(cx,argv[0],exception);
	if (bitmap!=NULL) bitmap->repeat.count=script_value_to_int(cx,argv[1]);
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_bitmap_set_flash_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	iface_bitmap_type			*bitmap;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	bitmap=script_find_bitmap_from_name(cx,argv[0],exception);
	if (bitmap!=NULL) bitmap->flash=script_value_to_bool(cx,argv[1]);
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_bitmap_set_rotate_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	iface_bitmap_type			*bitmap;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	bitmap=script_find_bitmap_from_name(cx,argv[0],exception);
	if (bitmap!=NULL) bitmap->rot=script_value_to_float(cx,argv[1]);
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_bitmap_start_fade_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	iface_bitmap_type			*bitmap;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	bitmap=script_find_bitmap_from_name(cx,argv[0],exception);
	if (bitmap!=NULL) {
		bitmap->show=TRUE;
		bitmap->fade.on=TRUE;
		bitmap->fade.start_tick=game_time_get();
	}

	return(script_null_to_value(cx));
}
