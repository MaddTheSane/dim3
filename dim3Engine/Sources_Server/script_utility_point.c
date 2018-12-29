/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: utility.point object

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

extern iface_type		iface;
extern js_type			js;

extern void view_script_transform_3D_to_2D(d3pnt *pnt);

JSValueRef js_utility_point_equal_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_utility_point_angle_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_utility_point_distance_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_utility_point_transform_3D_to_2D_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	utility_point_functions[]={
							{"equal",				js_utility_point_equal_func,				kJSPropertyAttributeDontDelete},
							{"angleTo",				js_utility_point_angle_to_func,				kJSPropertyAttributeDontDelete},
							{"distanceTo",			js_utility_point_distance_to_func,			kJSPropertyAttributeDontDelete},
							{"transform3Dto2D",		js_utility_point_transform_3D_to_2D_func,	kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			utility_point_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_utility_point_object(void)
{
	utility_point_class=script_create_class("utility_point_class",NULL,utility_point_functions);
}

void script_free_utility_point_object(void)
{
	script_free_class(utility_point_class);
}

JSObjectRef script_add_utility_point_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,utility_point_class,"point",script_idx));
}

/* =======================================================

      Point Functions
      
======================================================= */

JSValueRef js_utility_point_equal_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				slop;
	d3pnt			pnt,to_pnt;

	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));

	script_value_to_point(cx,argv[0],&pnt);
	script_value_to_point(cx,argv[1],&to_pnt);
	slop=script_value_to_int(cx,argv[2]);

	return(script_bool_to_value(cx,!((pnt.x<(to_pnt.x-slop)) || (pnt.x>(to_pnt.x+slop)) || (pnt.y<(to_pnt.y-slop)) || (pnt.y>(to_pnt.y+slop)) || (pnt.z<(to_pnt.z-slop)) || (pnt.z>(to_pnt.z+slop)))));
}

JSValueRef js_utility_point_angle_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			ang;
	d3pnt			pnt,to_pnt;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	script_value_to_2D_point(cx,argv[0],&pnt);
	script_value_to_2D_point(cx,argv[1],&to_pnt);

	ang=angle_find(pnt.x,pnt.z,to_pnt.x,to_pnt.z);
	return(script_float_to_value(cx,ang));
}

JSValueRef js_utility_point_distance_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				dist;
	d3pnt			pnt,to_pnt;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	script_value_to_point(cx,argv[0],&pnt);
	script_value_to_point(cx,argv[1],&to_pnt);

	dist=distance_get(pnt.x,pnt.y,pnt.z,to_pnt.x,to_pnt.y,to_pnt.z);
	return(script_int_to_value(cx,dist));
}

JSValueRef js_utility_point_transform_3D_to_2D_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3pnt			pnt;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	script_value_to_point(cx,argv[0],&pnt);
	view_script_transform_3D_to_2D(&pnt);

	return(script_point_to_value(cx,&pnt));
}



