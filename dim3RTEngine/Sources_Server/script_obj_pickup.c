/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.pickup object

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

extern server_type			server;
extern js_type				js;
extern network_setup_type	net_setup;

JSValueRef js_obj_pickup_get_objectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_pickup_get_objectName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_pickup_get_objectIsPlayer(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_pickup_get_itemId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_pickup_get_itemName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_pickup_add_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_swap_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_single_swap_multi_add_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_add_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_add_clip_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_add_alt_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_add_alt_clip_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_add_health_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_add_armor_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_add_custom_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_cancel_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_pickup_props[]={
							{"objectId",			js_obj_pickup_get_objectId,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectName",			js_obj_pickup_get_objectName,		NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectIsPlayer",		js_obj_pickup_get_objectIsPlayer,	NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"itemId",				js_obj_pickup_get_itemId,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"itemName",			js_obj_pickup_get_itemName,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	obj_pickup_functions[]={
							{"addWeapon",					js_obj_pickup_add_weapon_func,						kJSPropertyAttributeDontDelete},
							{"swapWeapon",					js_obj_pickup_swap_weapon_func,						kJSPropertyAttributeDontDelete},
							{"singleSwapMultiAddWeapon",	js_obj_pickup_single_swap_multi_add_weapon_func,	kJSPropertyAttributeDontDelete},
							{"addAmmo",						js_obj_pickup_add_ammo_func,						kJSPropertyAttributeDontDelete},
							{"addClip",						js_obj_pickup_add_clip_func,						kJSPropertyAttributeDontDelete},
							{"addAltAmmo",					js_obj_pickup_add_alt_ammo_func,					kJSPropertyAttributeDontDelete},
							{"addAltClip",					js_obj_pickup_add_alt_clip_func,					kJSPropertyAttributeDontDelete},
							{"addHealth",					js_obj_pickup_add_health_func,						kJSPropertyAttributeDontDelete},
							{"addArmor",					js_obj_pickup_add_armor_func,						kJSPropertyAttributeDontDelete},
							{"addCustom",					js_obj_pickup_add_custom_func,						kJSPropertyAttributeDontDelete},
							{"cancel",						js_obj_pickup_cancel_func,							kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_pickup_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_pickup_object(void)
{
	obj_pickup_class=script_create_class("obj_pickup_class",obj_pickup_props,obj_pickup_functions);
}

void script_free_obj_pickup_object(void)
{
	script_free_class(obj_pickup_class);
}

JSObjectRef script_add_obj_pickup_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,obj_pickup_class,"pickup",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_pickup_get_objectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->pickup.obj_idx));
}

JSValueRef js_obj_pickup_get_objectName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj,*pickup_obj;

	obj=object_get_attach(j_obj);

	pickup_obj=server.obj_list.objs[obj->pickup.obj_idx];
	if (pickup_obj==NULL) return(script_null_to_value(cx));
	
	return(script_string_to_value(cx,pickup_obj->name));
}

JSValueRef js_obj_pickup_get_objectIsPlayer(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_bool_to_value(cx,obj->pickup.obj_idx==server.player_obj_idx));
}

JSValueRef js_obj_pickup_get_itemId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_get_attach(j_obj);
	return(script_int_to_value(cx,obj->pickup.item_idx));
}

JSValueRef js_obj_pickup_get_itemName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj,*pickup_item;

	obj=object_get_attach(j_obj);

	pickup_item=server.obj_list.objs[obj->pickup.item_idx];
	if (pickup_item==NULL) return(script_null_to_value(cx));
	
	return(script_string_to_value(cx,pickup_item->name));
}

/* =======================================================

      Add Functions
      
======================================================= */

JSValueRef js_obj_pickup_add_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	weapon_type	*weap;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_bool_to_value(cx,FALSE));
	
	weap=script_find_weapon_from_name_arg(cx,obj,argv[1],exception);
	if (weap==NULL) return(script_bool_to_value(cx,FALSE));
	
    return(script_bool_to_value(cx,item_add_weapon(obj,weap)));
}

JSValueRef js_obj_pickup_swap_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	weapon_type	*weap;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_bool_to_value(cx,FALSE));
	
	weap=script_find_weapon_from_name_arg(cx,obj,argv[1],exception);
	if (weap==NULL) return(script_bool_to_value(cx,FALSE));
	
    return(script_bool_to_value(cx,item_swap_weapon(obj,weap)));
}

JSValueRef js_obj_pickup_single_swap_multi_add_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (net_setup.mode==net_mode_none) return(js_obj_pickup_swap_weapon_func(cx,func,j_obj,argc,argv,exception));
	return(js_obj_pickup_add_weapon_func(cx,func,j_obj,argc,argv,exception));
}

JSValueRef js_obj_pickup_add_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	weapon_type	*weap;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_bool_to_value(cx,FALSE));
	
	weap=script_find_weapon_from_name_arg(cx,obj,argv[1],exception);
	if (weap==NULL) return(script_bool_to_value(cx,FALSE));
	
    return(script_bool_to_value(cx,item_add_ammo(obj,weap,script_value_to_int(cx,argv[2]))));
}

JSValueRef js_obj_pickup_add_clip_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	weapon_type	*weap;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_bool_to_value(cx,FALSE));
	
	weap=script_find_weapon_from_name_arg(cx,obj,argv[1],exception);
	if (weap==NULL) return(script_bool_to_value(cx,FALSE));
	
    return(script_bool_to_value(cx,item_add_clip(obj,weap,script_value_to_int(cx,argv[2]))));
}

JSValueRef js_obj_pickup_add_alt_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	weapon_type	*weap;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_bool_to_value(cx,FALSE));
	
	weap=script_find_weapon_from_name_arg(cx,obj,argv[1],exception);
	if (weap==NULL) return(script_bool_to_value(cx,FALSE));
	
    return(script_bool_to_value(cx,item_add_alt_ammo(obj,weap,script_value_to_int(cx,argv[2]))));
}

JSValueRef js_obj_pickup_add_alt_clip_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	weapon_type	*weap;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_bool_to_value(cx,FALSE));
	
	weap=script_find_weapon_from_name_arg(cx,obj,argv[1],exception);
	if (weap==NULL) return(script_bool_to_value(cx,FALSE));
	
    return(script_bool_to_value(cx,item_add_alt_clip(obj,weap,script_value_to_int(cx,argv[2]))));
}

JSValueRef js_obj_pickup_add_health_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_bool_to_value(cx,FALSE));

	return(script_bool_to_value(cx,item_add_health(obj,script_value_to_int(cx,argv[1]))));
}

JSValueRef js_obj_pickup_add_armor_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_bool_to_value(cx,FALSE));

	return(script_bool_to_value(cx,item_add_armor(obj,script_value_to_int(cx,argv[1]))));
}
JSValueRef js_obj_pickup_add_custom_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj!=NULL) item_add_custom(obj,script_value_to_int(cx,argv[1]));
	
	return(script_null_to_value(cx));
}

/* =======================================================

      Cancel Functions
      
======================================================= */

JSValueRef js_obj_pickup_cancel_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type			*obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_get_attach(j_obj);
	obj->pickup.canceled=TRUE;
	
	return(script_null_to_value(cx));
}
