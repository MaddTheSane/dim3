/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.hit object

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

extern map_type			map;
extern server_type		server;
extern js_type			js;

JSValueRef js_proj_hit_get_type(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_hit_get_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_hit_get_id(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_hit_get_isPlayer(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_hit_get_startTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_hit_get_materialName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_hit_get_ejectVector(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_hit_get_reflectVector(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);

JSStaticValue 		proj_hit_props[]={
							{"type",				js_proj_hit_get_type,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"name",				js_proj_hit_get_name,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"id",					js_proj_hit_get_id,					NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"isPlayer",			js_proj_hit_get_isPlayer,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"startTick",			js_proj_hit_get_startTick,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"materialName",		js_proj_hit_get_materialName,		NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"ejectVector",			js_proj_hit_get_ejectVector,		NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"reflectVector",		js_proj_hit_get_reflectVector,		NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			proj_hit_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_hit_object(void)
{
	proj_hit_class=script_create_class("proj_hit_class",proj_hit_props,NULL);
}

void script_free_proj_hit_object(void)
{
	script_free_class(proj_hit_class);
}

JSObjectRef script_add_proj_hit_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,proj_hit_class,"hit",script_idx));
}

/* =======================================================

      Getters Utilities
      
======================================================= */

int js_get_proj_hit_type(proj_type *proj)
{
	map_mesh_poly_type	*poly;

		// check auto hits
		
	if (proj->action.hit_tick!=0) {
		if ((proj->start_tick+proj->action.hit_tick)<=game_time_get()) return(sd_proj_hit_type_auto);
	}

		// object or projectile hits

    if (proj->contact.obj_idx!=-1) return(sd_proj_hit_type_object);
	if (proj->contact.proj_idx!=-1) return(sd_proj_hit_type_projectile);
	if (proj->contact.melee) return(sd_proj_hit_type_melee);

		// map hits
		// we use wall like flag to determine if
		// it's a wall hit, and floor/ceiling
		// hits by proximity to projectile Y

	if (proj->contact.hit_poly.mesh_idx==-1) return(sd_proj_hit_type_none);
	
	poly=&map.mesh.meshes[proj->contact.hit_poly.mesh_idx].polys[proj->contact.hit_poly.poly_idx];
	if (poly->box.wall_like) return(sd_proj_hit_type_wall);
	
	if (proj->pnt.y>poly->box.mid.y) return(sd_proj_hit_type_ceiling);

	return(sd_proj_hit_type_floor);
}

void js_get_proj_hit_name(proj_type *proj,int hit_type,char *name)
{
	obj_type			*hit_obj;
	weapon_type			*hit_weap;
	proj_type			*hit_proj;
	proj_setup_type		*hit_proj_setup;

	name[0]=0x0;
	
	switch (hit_type) {
	
		case sd_proj_hit_type_object:
			hit_obj=server.obj_list.objs[proj->contact.obj_idx];
			if (hit_obj==NULL) return;

			strcpy(name,hit_obj->name);
			return;
		
		case sd_proj_hit_type_projectile:
			hit_proj=server.proj_list.projs[proj->contact.proj_idx];
			if (!hit_proj->on) return;

			hit_obj=server.obj_list.objs[proj->contact.obj_idx];
			if (hit_obj==NULL) return;

			hit_weap=hit_obj->weap_list.weaps[hit_proj->weap_idx];
			hit_proj_setup=hit_weap->proj_setup_list.proj_setups[hit_proj->proj_setup_idx];
			strcpy(name,hit_proj_setup->name);
			return;
		
	}
}

void js_get_proj_hit_material_name(proj_type *proj,int hit_type,char *name)
{
	int					mesh_idx;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;

		// get hit poly

	mesh_idx=proj->contact.hit_poly.mesh_idx;
	if (mesh_idx==-1) {
		name[0]=0x0;
		return;
	}

	mesh_poly=&map.mesh.meshes[mesh_idx].polys[proj->contact.hit_poly.poly_idx];
	
		// get material name

	texture=&map.textures[mesh_poly->txt_idx];
	strcpy(name,texture->material_name);
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_proj_hit_get_type(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_type			*proj;

	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));

	return(script_int_to_value(cx,js_get_proj_hit_type(proj)));
}

JSValueRef js_proj_hit_get_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	int					hit_type;
	char				hit_name[name_str_len];
	proj_type			*proj;

	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));

	hit_type=js_get_proj_hit_type(proj);
	js_get_proj_hit_name(proj,hit_type,hit_name);

	return(script_string_to_value(cx,hit_name));
}

JSValueRef js_proj_hit_get_id(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_type			*proj;

	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));

	return(script_int_to_value(cx,proj->contact.obj_idx));
}

JSValueRef js_proj_hit_get_isPlayer(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_type			*proj;

	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));

	return(script_bool_to_value(cx,proj->contact.obj_idx==server.player_obj_idx));
}

JSValueRef js_proj_hit_get_startTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_type			*proj;

	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));

	return(script_int_to_value(cx,proj->start_tick));
}

JSValueRef js_proj_hit_get_materialName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	int					hit_type;
	char				hit_name[name_str_len];
	proj_type			*proj;

	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));

	hit_type=js_get_proj_hit_type(proj);
	js_get_proj_hit_material_name(proj,hit_type,hit_name);

	return(script_string_to_value(cx,hit_name));
}

JSValueRef js_proj_hit_get_ejectVector(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	d3vct				vct;
	proj_type			*proj;

	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));

	projectile_eject_vector(proj,&vct);
	return(script_vector_to_value(cx,&vct));
}

JSValueRef js_proj_hit_get_reflectVector(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	d3vct				vct;
	proj_type			*proj;

	proj=proj_get_attach(j_obj);
	if (proj==NULL) return(script_null_to_value(cx));

	projectile_reflect_vector(proj,&vct);
	return(script_vector_to_value(cx,&vct));
}

