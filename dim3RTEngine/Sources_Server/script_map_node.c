/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.node object

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

#include "scripts.h"
#include "objects.h"

extern iface_type		iface;
extern map_type			map;
extern server_type		server;
extern js_type			js;

JSValueRef js_map_node_find_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_node_find_random_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_node_find_random_skip_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_node_find_random_forward_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_node_find_random_forward_skip_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_node_find_nearest_to_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_node_find_nearest_names_in_path_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_node_find_nearest_unheld_weapon_in_path_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_node_find_nearest_held_weapon_ammo_in_path_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_node_find_nearest_armor_in_path_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_node_next_in_path_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_node_get_adjacent_nodes_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_node_get_name_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_node_get_distance_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_node_get_angle_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_node_get_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	map_node_functions[]={
							{"find",							js_map_node_find_func,									kJSPropertyAttributeDontDelete},
							{"findRandom",						js_map_node_find_random_func,							kJSPropertyAttributeDontDelete},
							{"findRandomSkipId",				js_map_node_find_random_skip_id_func,					kJSPropertyAttributeDontDelete},
							{"findRandomForward",				js_map_node_find_random_forward_func,					kJSPropertyAttributeDontDelete},
							{"findRandomForwardSkipId",			js_map_node_find_random_forward_skip_id_func,			kJSPropertyAttributeDontDelete},
							{"findNearestToObject",				js_map_node_find_nearest_to_object_func,				kJSPropertyAttributeDontDelete},
							{"findNearestNamesInPath",			js_map_node_find_nearest_names_in_path_func,			kJSPropertyAttributeDontDelete},
							{"findNearestUnheldWeaponInPath",	js_map_node_find_nearest_unheld_weapon_in_path_func,	kJSPropertyAttributeDontDelete},
							{"findNearestHeldWeaponAmmoInPath",	js_map_node_find_nearest_held_weapon_ammo_in_path_func,	kJSPropertyAttributeDontDelete},
							{"findNearestArmorInPath",			js_map_node_find_nearest_armor_in_path_func,			kJSPropertyAttributeDontDelete},
							{"nextInPath",						js_map_node_next_in_path_func,							kJSPropertyAttributeDontDelete},
							{"getAdjacentNodes",				js_map_node_get_adjacent_nodes_func,					kJSPropertyAttributeDontDelete},
							{"getName",							js_map_node_get_name_func,								kJSPropertyAttributeDontDelete},
							{"getDistance",						js_map_node_get_distance_func,							kJSPropertyAttributeDontDelete},
							{"getAngleTo",						js_map_node_get_angle_to_func,							kJSPropertyAttributeDontDelete},
							{"getPosition",						js_map_node_get_position_func,							kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			map_node_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_node_object(void)
{
	map_node_class=script_create_class("map_node_class",NULL,map_node_functions);
}

void script_free_map_node_object(void)
{
	script_free_class(map_node_class);
}

JSObjectRef script_add_map_node_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,map_node_class,"node",script_idx));
}

/* =======================================================

      Find Node Functions
      
======================================================= */

JSValueRef js_map_node_find_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	char			name[name_str_len];
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[0],name,name_str_len);
	idx=map_find_node(&map,name);
	return(script_int_to_value(cx,idx));
}

JSValueRef js_map_node_find_random_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	char			name[name_str_len];
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[0],name,name_str_len);
	idx=map_find_random_node(&map,name,-1);
	return(script_int_to_value(cx,idx));
}

JSValueRef js_map_node_find_random_skip_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx,skip_idx;
	char			name[name_str_len];
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[0],name,name_str_len);
	skip_idx=script_value_to_int(cx,argv[1]);

	idx=map_find_random_node(&map,name,skip_idx);
	return(script_int_to_value(cx,idx));
}

JSValueRef js_map_node_find_random_forward_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	char			name[name_str_len];
	d3pnt			pnt;
	d3ang			ang;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[0],name,name_str_len);
	script_value_to_point(cx,argv[1],&pnt);
	script_value_to_angle(cx,argv[2],&ang);

	idx=map_find_random_forward_node(&map,name,-1,&pnt,&ang);
	return(script_int_to_value(cx,idx));
}

JSValueRef js_map_node_find_random_forward_skip_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx,skip_idx;
	char			name[name_str_len];
	d3pnt			pnt;
	d3ang			ang;
	
	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[0],name,name_str_len);
	script_value_to_point(cx,argv[1],&pnt);
	script_value_to_angle(cx,argv[2],&ang);
	skip_idx=script_value_to_int(cx,argv[3]);

	idx=map_find_random_forward_node(&map,name,skip_idx,&pnt,&ang);
	return(script_int_to_value(cx,idx));
}

/* =======================================================

      Find Nearest Node Functions
      
======================================================= */

JSValueRef js_map_node_find_nearest_to_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));
	
	idx=map_find_nearest_node_by_point(&map,&obj->pnt);
	return(script_int_to_value(cx,idx));
}

JSValueRef js_map_node_find_nearest_names_in_path_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,idx,from_idx,k,len,d,dist;
	char			name[name_str_len];
	JSObjectRef		array_obj;
	JSValueRef		vp;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
		// from node
		
	from_idx=script_find_node_idx_from_idx_arg(cx,argv[0],exception);
	if (from_idx==-1) return(script_null_to_value(cx));
	
		// check if array is OK
		
	if (!JSValueIsArray(cx,argv[1])) {
		*exception=script_create_exception(cx,"Second parameter to findNearestNamesInPath needs to be an array");
		return(script_null_to_value(cx));
	}

		// get length

	array_obj=JSValueToObject(cx,argv[1],exception);
	if (array_obj==NULL) return(script_null_to_value(cx));
	
	vp=script_get_single_property(cx,array_obj,"length");
	len=script_value_to_int(cx,vp);
	
		// check all names
		
	idx=-1;
	dist=-1;
		
	for (n=0;n!=len;n++) {
		vp=JSObjectGetPropertyAtIndex(cx,array_obj,n,NULL);
		script_value_to_string(cx,vp,name,name_str_len);
	
		k=map_find_nearest_node_in_path(&map,from_idx,name,&d);
		if (k==-1) continue;
		
		if ((dist==-1) || (d<dist)) {
			dist=d;
			idx=k;
		}
	}
	
	return(script_int_to_value(cx,idx));
}

JSValueRef js_map_node_find_nearest_unheld_weapon_in_path_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx,from_idx;
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
		// from node and object
		
	from_idx=script_find_node_idx_from_idx_arg(cx,argv[0],exception);
	if (from_idx==-1) return(script_null_to_value(cx));

	obj=script_find_obj_from_uid_arg(cx,argv[1],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	idx=object_find_neareset_unheld_weapon_node(obj,from_idx);
	return(script_int_to_value(cx,idx));
}

JSValueRef js_map_node_find_nearest_held_weapon_ammo_in_path_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx,from_idx;
	bool			empty_only;
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
		// from node and object
		
	from_idx=script_find_node_idx_from_idx_arg(cx,argv[0],exception);
	if (from_idx==-1) return(script_null_to_value(cx));

	obj=script_find_obj_from_uid_arg(cx,argv[1],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	empty_only=script_value_to_bool(cx,argv[2]);

	idx=object_find_neareset_held_weapon_ammo_node(obj,from_idx,empty_only);
	return(script_int_to_value(cx,idx));
}

JSValueRef js_map_node_find_nearest_armor_in_path_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx,from_idx;
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
		// from node and object
		
	from_idx=script_find_node_idx_from_idx_arg(cx,argv[0],exception);
	if (from_idx==-1) return(script_null_to_value(cx));

	obj=script_find_obj_from_uid_arg(cx,argv[1],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	idx=object_find_neareset_armor_node(obj,from_idx);
	return(script_int_to_value(cx,idx));
}

/* =======================================================

      Find Node Path Functions
      
======================================================= */

JSValueRef js_map_node_next_in_path_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx,from_idx,to_idx;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	from_idx=script_find_node_idx_from_idx_arg(cx,argv[0],exception);
	if (from_idx==-1) return(script_null_to_value(cx));

	to_idx=script_find_node_idx_from_idx_arg(cx,argv[1],exception);
	if (to_idx==-1) return(script_null_to_value(cx));
	
	idx=map_find_next_node_in_path(&map,from_idx,to_idx);
	return(script_int_to_value(cx,idx));
}

JSValueRef js_map_node_get_adjacent_nodes_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,cnt,link[max_node_link];
	node_type		*node;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	node=script_find_node_from_idx_arg(cx,argv[0],exception);
	if (node==NULL) return(script_null_to_value(cx));

	cnt=0;

	for (n=0;n!=max_node_link;n++) {
		if (node->link[n]!=-1) {
			link[cnt++]=(int)node->link[n];
		}
	}

	return(script_int_array_to_value(cx,cnt,link));
}

/* =======================================================

      Node Name
      
======================================================= */

JSValueRef js_map_node_get_name_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	node_type		*node;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
		// node
		
	node=script_find_node_from_idx_arg(cx,argv[0],exception);
	if (node==NULL) return(script_null_to_value(cx));
	
		// get position
		
	return(script_string_to_value(cx,node->name));
}

/* =======================================================

      Node Distance and Angle To
      
======================================================= */

JSValueRef js_map_node_get_distance_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3pnt			pnt;
	node_type		*node;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
		
	node=script_find_node_from_idx_arg(cx,argv[0],exception);
	if (node==NULL) return(script_null_to_value(cx));
	
	script_value_to_point(cx,argv[1],&pnt);
	return(script_int_to_value(cx,distance_get(node->pnt.x,node->pnt.y,node->pnt.z,pnt.x,pnt.y,pnt.z)));
}

JSValueRef js_map_node_get_angle_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				x,z,y;
	d3ang			ang;
	node_type		*node;
	
	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	
		// node
		
	node=script_find_node_from_idx_arg(cx,argv[0],exception);
	if (node==NULL) return(script_null_to_value(cx));

		// x,z,y
		
	x=script_value_to_int(cx,argv[1]);
	z=script_value_to_int(cx,argv[2]);
	y=script_value_to_int(cx,argv[3]);
	
		// get angles to
		
	ang.x=angle_find(y,z,node->pnt.y,node->pnt.z);
	ang.y=angle_find(x,z,node->pnt.x,node->pnt.z);
	ang.z=angle_find(x,y,node->pnt.x,node->pnt.y);
		
	return(script_angle_to_value(cx,&ang));
}

/* =======================================================

      Node Position and Angle
      
======================================================= */

JSValueRef js_map_node_get_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	node_type		*node;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
		// node
		
	node=script_find_node_from_idx_arg(cx,argv[0],exception);
	if (node==NULL) return(script_null_to_value(cx));
	
		// get position
		
	return(script_point_to_value(cx,&node->pnt));
}

