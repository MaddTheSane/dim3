/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.polygon object

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

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern js_type				js;

JSValueRef js_map_polygon_find_faced_by_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_polygon_get_distance_to_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_polygon_get_hit_point_to_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_polygon_get_normal_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_polygon_get_dot_product_to_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	map_polygon_functions[]={
							{"findFacedByObject",		js_map_polygon_find_faced_by_object_func,		kJSPropertyAttributeDontDelete},
							{"getDistanceToObject",		js_map_polygon_get_distance_to_object_func,		kJSPropertyAttributeDontDelete},
							{"getHitPointToObject",		js_map_polygon_get_hit_point_to_object_func,	kJSPropertyAttributeDontDelete},
							{"getNormal",				js_map_polygon_get_normal_func,					kJSPropertyAttributeDontDelete},
							{"getDotProductToObject",	js_map_polygon_get_dot_product_to_object_func,	kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			map_polygon_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_polygon_object(void)
{
	map_polygon_class=script_create_class("map_polygon_class",NULL,map_polygon_functions);
}

void script_free_map_polygon_object(void)
{
	script_free_class(map_polygon_class);
}

JSObjectRef script_add_map_polygon_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,map_polygon_class,"polygon",script_idx));
}

/* =======================================================

      Map Functions
      
======================================================= */

JSValueRef js_map_polygon_find_faced_by_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,collide_polygon_find_faced_by_object(obj)));
}

JSValueRef js_map_polygon_get_distance_to_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int			poly_uid;
	obj_type	*obj;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	poly_uid=script_value_to_int(cx,argv[0]);

	obj=script_find_obj_from_uid_arg(cx,argv[1],exception);
	if (obj==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,collide_polygon_distance_to_object(poly_uid,obj)));
}

JSValueRef js_map_polygon_get_hit_point_to_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int			poly_uid;
	d3pnt		pt;
	obj_type	*obj;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	poly_uid=script_value_to_int(cx,argv[0]);

	obj=script_find_obj_from_uid_arg(cx,argv[1],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	collide_polygon_hit_point_to_object(poly_uid,obj,&pt);

	return(script_point_to_value(cx,&pt));
}

JSValueRef js_map_polygon_get_normal_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int			poly_uid;
	d3vct		normal;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	poly_uid=script_value_to_int(cx,argv[0]);
	
	collide_polygon_get_normal(poly_uid,&normal);
	
	return(script_vector_to_value(cx,&normal));
}

JSValueRef js_map_polygon_get_dot_product_to_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int			poly_uid;
	float		dot_product;
	obj_type	*obj;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	poly_uid=script_value_to_int(cx,argv[0]);

	obj=script_find_obj_from_uid_arg(cx,argv[1],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	dot_product=collide_polygon_dot_product_to_object(poly_uid,obj);

	return(script_float_to_value(cx,dot_product));
}
