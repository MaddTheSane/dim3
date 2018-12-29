/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: spawn object

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
#include "objects.h"
#include "scripts.h"

extern iface_type		iface;
extern server_type		server;
extern js_type			js;

JSValueRef js_spawn_particle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_particle_moving_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_particle_line_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_ring_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_ring_line_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_flash_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_lightning_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_ray_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_ray_team_color_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_shake_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_push_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	spawn_functions[]={
							{"particle",			js_spawn_particle_func,				kJSPropertyAttributeDontDelete},
							{"particleMoving",		js_spawn_particle_moving_func,		kJSPropertyAttributeDontDelete},
							{"particleLine",		js_spawn_particle_line_func,		kJSPropertyAttributeDontDelete},
							{"ring",				js_spawn_ring_func,					kJSPropertyAttributeDontDelete},
							{"ringLine",			js_spawn_ring_line_func,			kJSPropertyAttributeDontDelete},
							{"flash",				js_spawn_flash_func,				kJSPropertyAttributeDontDelete},
							{"lightning",			js_spawn_lightning_func,			kJSPropertyAttributeDontDelete},
							{"ray",					js_spawn_ray_func,					kJSPropertyAttributeDontDelete},
							{"rayTeamColor",		js_spawn_ray_team_color_func,		kJSPropertyAttributeDontDelete},
							{"shake",				js_spawn_shake_func,				kJSPropertyAttributeDontDelete},
							{"push",				js_spawn_push_func,					kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			spawn_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_global_spawn_object(void)
{
	spawn_class=script_create_class("spawn_class",NULL,spawn_functions);
}

void script_free_global_spawn_object(void)
{
	script_free_class(spawn_class);
}

JSObjectRef script_add_global_spawn_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,spawn_class,"spawn",script_idx));
}

/* =======================================================

      Spawn Exceptions
      
======================================================= */

JSValueRef js_particle_name_exception(JSContextRef cx,char *name)
{
	char			err_str[256];

	sprintf(err_str,"Named particle does not exist: %s",name);
	return(script_create_exception(cx,err_str));
}

JSValueRef js_ring_name_exception(JSContextRef cx,char *name)
{
	char			err_str[256];

	sprintf(err_str,"Named ring does not exist: %s",name);
	return(script_create_exception(cx,err_str));
}

/* =======================================================

      Spawn Particle
      
======================================================= */

JSValueRef js_spawn_particle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	char			name[name_str_len];
	d3pnt			pnt;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	script_value_to_point(cx,argv[0],&pnt);
	script_value_to_string(cx,argv[1],name,name_str_len);
	
	idx=particle_find_index(name);
	if (idx==-1) {
		*exception=js_particle_name_exception(cx,name);
	}
	else {
		script_bool_to_value(cx,particle_spawn(idx,script_get_attached_object_uid(j_obj),&pnt,NULL,NULL));
	}

	return(script_null_to_value(cx));
}

JSValueRef js_spawn_particle_moving_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	char			name[name_str_len];
	d3pnt			pnt;
	particle_motion	motion;

	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	motion.bone_idx=-1;
	script_value_to_point(cx,argv[0],&pnt);
	script_value_to_vector(cx,argv[1],&motion.vct);
	script_value_to_string(cx,argv[2],name,name_str_len);
	
	idx=particle_find_index(name);
	if (idx==-1) {
		*exception=js_particle_name_exception(cx,name);
	}
	else {
		script_bool_to_value(cx,particle_spawn(idx,script_get_attached_object_uid(j_obj),&pnt,NULL,&motion));
	}

	return(script_null_to_value(cx));
}

JSValueRef js_spawn_particle_line_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx,count;
	char			name[name_str_len];
	d3pnt			start_pnt,end_pnt;

	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	
	script_value_to_point(cx,argv[0],&start_pnt);
	script_value_to_point(cx,argv[1],&end_pnt);
	count=script_value_to_int(cx,argv[2]);
	script_value_to_string(cx,argv[3],name,name_str_len);
	
	idx=particle_find_index(name);
	if (idx==-1) {
		*exception=js_particle_name_exception(cx,name);
	}
	else {
		script_bool_to_value(cx,particle_line_spawn(idx,script_get_attached_object_uid(j_obj),&start_pnt,&end_pnt,count));
	}

	return(script_null_to_value(cx));
}

/* =======================================================

      Spawn Ring
      
======================================================= */

JSValueRef js_spawn_ring_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	char			name[name_str_len];
	d3pnt			pnt;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	script_value_to_point(cx,argv[0],&pnt);
	script_value_to_string(cx,argv[1],name,name_str_len);
	
	idx=ring_find_index(name);
	if (idx==-1) {
		*exception=js_ring_name_exception(cx,name);
	}
	else {
		script_bool_to_value(cx,ring_spawn(idx,script_get_attached_object_uid(j_obj),&pnt,NULL));
	}

	return(script_null_to_value(cx));
}

JSValueRef js_spawn_ring_line_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx,count;
	char			name[name_str_len];
	d3pnt			start_pnt,end_pnt;

	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	
	script_value_to_point(cx,argv[0],&start_pnt);
	script_value_to_point(cx,argv[1],&end_pnt);
	count=script_value_to_int(cx,argv[2]);
	script_value_to_string(cx,argv[3],name,name_str_len);
	
	idx=ring_find_index(name);
	if (idx==-1) {
		*exception=js_ring_name_exception(cx,name);
	}
	else {
		script_bool_to_value(cx,ring_line_spawn(idx,script_get_attached_object_uid(j_obj),&start_pnt,&end_pnt,count));
	}

	return(script_null_to_value(cx));
}

/* =======================================================

      Spawn Flash
      
======================================================= */

JSValueRef js_spawn_flash_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				intensity,flash_msec,fade_msec;
	d3pnt			pnt;
	d3col			col;

	if (!script_check_param_count(cx,func,argc,5,exception)) return(script_null_to_value(cx));
	
	script_value_to_point(cx,argv[0],&pnt);
	script_value_to_color(cx,argv[1],&col);
	intensity=script_value_to_int(cx,argv[2]);
	flash_msec=script_value_to_int(cx,argv[3]);
	fade_msec=script_value_to_int(cx,argv[4]);

	script_bool_to_value(cx,effect_spawn_flash(&pnt,&col,intensity,1.0f,flash_msec,fade_msec));
    
	return(script_null_to_value(cx));
}

/* =======================================================

      Spawn Lightning
      
======================================================= */

JSValueRef js_spawn_lightning_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				wid,life_msec;
	float			varient;
	d3pnt			start_pnt,end_pnt;
	d3col			col;

	if (!script_check_param_count(cx,func,argc,6,exception)) return(script_null_to_value(cx));

	script_value_to_point(cx,argv[0],&start_pnt);
	script_value_to_point(cx,argv[1],&end_pnt);
	wid=script_value_to_int(cx,argv[2]);
	varient=script_value_to_float(cx,argv[3]);
	script_value_to_color(cx,argv[4],&col);
	life_msec=script_value_to_int(cx,argv[5]);

	script_bool_to_value(cx,effect_spawn_lightning(&start_pnt,&end_pnt,wid,varient,&col,life_msec));
    
	return(script_null_to_value(cx));
}

/* =======================================================

      Spawn Ray
      
======================================================= */

JSValueRef js_spawn_ray_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				wid,life_msec;
	d3pnt			start_pnt,end_pnt;
	d3col			col;

	if (!script_check_param_count(cx,func,argc,5,exception)) return(script_null_to_value(cx));
	
	script_value_to_point(cx,argv[0],&start_pnt);
	script_value_to_point(cx,argv[1],&end_pnt);
	wid=script_value_to_int(cx,argv[2]);
	script_value_to_color(cx,argv[3],&col);
	life_msec=script_value_to_int(cx,argv[4]);

	script_bool_to_value(cx,effect_spawn_ray(&start_pnt,&end_pnt,wid,&col,life_msec));
    
	return(script_null_to_value(cx));
}

JSValueRef js_spawn_ray_team_color_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				wid,life_msec,obj_idx;
	d3pnt			start_pnt,end_pnt;
	d3col			col;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	
	script_value_to_point(cx,argv[0],&start_pnt);
	script_value_to_point(cx,argv[1],&end_pnt);
	wid=script_value_to_int(cx,argv[2]);
	life_msec=script_value_to_int(cx,argv[3]);

		// team color

	col.r=col.g=col.b=1.0f;

	obj_idx=script_get_attached_object_uid(j_obj);
	if (obj_idx!=-1) {
		obj=server.obj_list.objs[obj_idx];
		if (obj!=NULL) object_get_tint(obj,&col);
	}

	script_bool_to_value(cx,effect_spawn_ray(&start_pnt,&end_pnt,wid,&col,life_msec));
    
	return(script_null_to_value(cx));
}

/* =======================================================

      Spawn Shake
      
======================================================= */

JSValueRef js_spawn_shake_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				dist,sz,life_msec;
	d3pnt			pnt;

	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	
	script_value_to_point(cx,argv[0],&pnt);
	dist=script_value_to_int(cx,argv[1]);
	sz=script_value_to_int(cx,argv[2]);
	life_msec=script_value_to_int(cx,argv[3]);
	
	script_bool_to_value(cx,effect_spawn_shake(&pnt,dist,sz,life_msec));
    
	return(script_null_to_value(cx));
}

/* =======================================================

      Spawn Push
      
======================================================= */

JSValueRef js_spawn_push_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				radius,force;
	d3pnt			push_pnt;

	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	script_value_to_point(cx,argv[0],&push_pnt);
	radius=script_value_to_int(cx,argv[1]);
	force=script_value_to_int(cx,argv[2]);

	collide_objects_push(&push_pnt,radius,force);
	return(script_bool_to_value(cx,TRUE));
}

