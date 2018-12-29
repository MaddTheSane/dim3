/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.object object

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

extern iface_type		iface;
extern server_type		server;
extern js_type			js;

JSValueRef js_map_object_get_count(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_map_object_find_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_find_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_find_all_players_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_nearest_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_nearest_skip_object_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_nearest_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_nearest_player_skip_object_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_nearest_player_skip_self_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_nearest_player_skip_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_nearest_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_getAllIds_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_getAllNearIds_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_name_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_type_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_team_name_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_team_color_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_distance_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_angle_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_angle_to_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_is_facing_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_size_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_health_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_is_dead_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_is_hidden_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_is_contact_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_is_max_health_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_last_damage_object_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_was_telefrag_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_move_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_shove_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_shove_direct_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_add_goal_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_set_contact_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_set_hidden_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_set_model_light_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_set_model_halo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_set_model_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_spawn_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_remove_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		map_object_props[]={
							{"count",						js_map_object_get_count,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	map_object_functions[]={
							{"find",						js_map_object_find_func,							kJSPropertyAttributeDontDelete},
							{"findPlayer",					js_map_object_find_player_func,						kJSPropertyAttributeDontDelete},
							{"findAllPlayers",				js_map_object_find_all_players_func,				kJSPropertyAttributeDontDelete},
							{"nearest",						js_map_object_nearest_func,							kJSPropertyAttributeDontDelete},
							{"nearestSkipObjectId",			js_map_object_nearest_skip_object_id_func,			kJSPropertyAttributeDontDelete},
							{"nearestPlayer",				js_map_object_nearest_player_func,					kJSPropertyAttributeDontDelete},
							{"nearestPlayerSkipObjectId",	js_map_object_nearest_player_skip_object_id_func,	kJSPropertyAttributeDontDelete},
							{"nearestPlayerSkipSelf",		js_map_object_nearest_player_skip_self_func,		kJSPropertyAttributeDontDelete},
							{"nearestPlayerSkipTeam",		js_map_object_nearest_player_skip_team_func,		kJSPropertyAttributeDontDelete},
							{"nearestTeam",					js_map_object_nearest_team_func,					kJSPropertyAttributeDontDelete},
							{"getAllIds",					js_map_object_getAllIds_func,						kJSPropertyAttributeDontDelete},
							{"getAllNearIds",				js_map_object_getAllNearIds_func,					kJSPropertyAttributeDontDelete},
							{"getObject",					js_map_object_get_object_func,						kJSPropertyAttributeDontDelete},
							{"getName",						js_map_object_get_name_func,						kJSPropertyAttributeDontDelete},
							{"getType",						js_map_object_get_type_func,						kJSPropertyAttributeDontDelete},
							{"getTeam",						js_map_object_get_team_func,						kJSPropertyAttributeDontDelete},
							{"getTeamName",					js_map_object_get_team_name_func,					kJSPropertyAttributeDontDelete},
							{"getTeamColor",				js_map_object_get_team_color_func,					kJSPropertyAttributeDontDelete},
							{"getDistance",					js_map_object_get_distance_func,					kJSPropertyAttributeDontDelete},
							{"getAngleTo",					js_map_object_get_angle_to_func,					kJSPropertyAttributeDontDelete},
							{"getAngleToId",				js_map_object_get_angle_to_id_func,					kJSPropertyAttributeDontDelete},
							{"isFacingId",					js_map_object_is_facing_id_func,					kJSPropertyAttributeDontDelete},
							{"getPosition",					js_map_object_get_position_func,					kJSPropertyAttributeDontDelete},
							{"getAngle",					js_map_object_get_angle_func,						kJSPropertyAttributeDontDelete},
							{"getSize",						js_map_object_get_size_func,						kJSPropertyAttributeDontDelete},
							{"getHealth",					js_map_object_get_health_func,						kJSPropertyAttributeDontDelete},
							{"isDead",						js_map_object_is_dead_func,							kJSPropertyAttributeDontDelete},
							{"isHidden",					js_map_object_is_hidden_func,						kJSPropertyAttributeDontDelete},
							{"isContact",					js_map_object_is_contact_func,						kJSPropertyAttributeDontDelete},
							{"isMaxHealth",					js_map_object_is_max_health_func,					kJSPropertyAttributeDontDelete},
							{"getLastDamageObjectId",		js_map_object_get_last_damage_object_id_func,		kJSPropertyAttributeDontDelete},
							{"wasTelefrag",					js_map_object_was_telefrag_func,					kJSPropertyAttributeDontDelete},
							{"moveTo",						js_map_object_move_to_func,							kJSPropertyAttributeDontDelete},
							{"shove",						js_map_object_shove_func,							kJSPropertyAttributeDontDelete},
							{"shoveDirect",					js_map_object_shove_direct_func,					kJSPropertyAttributeDontDelete},
							{"addGoal",						js_map_object_add_goal_func,						kJSPropertyAttributeDontDelete},
							{"setContact",					js_map_object_set_contact_func,						kJSPropertyAttributeDontDelete},
							{"setHidden",					js_map_object_set_hidden_func,						kJSPropertyAttributeDontDelete},
							{"setModelLight",				js_map_object_set_model_light_func,					kJSPropertyAttributeDontDelete},
							{"setModelHalo",				js_map_object_set_model_halo_func,					kJSPropertyAttributeDontDelete},
							{"setModelMesh",				js_map_object_set_model_mesh_func,					kJSPropertyAttributeDontDelete},
							{"spawn",						js_map_object_spawn_func,							kJSPropertyAttributeDontDelete},
							{"remove",						js_map_object_remove_func,							kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			map_object_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_object_object(void)
{
	map_object_class=script_create_class("map_object_class",map_object_props,map_object_functions);
}

void script_free_map_object_object(void)
{
	script_free_class(map_object_class);
}

JSObjectRef script_add_map_object_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,map_object_class,"object",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_map_object_get_count(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,object_script_count()));
}

/* =======================================================

      Find Object Function
      
======================================================= */

JSValueRef js_map_object_find_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char				name[name_str_len];
	obj_type			*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[0],name,name_str_len);
	
	obj=object_find_name(name);
	if (obj==NULL) return(script_int_to_value(cx,-1));

	return(script_int_to_value(cx,obj->idx));
}

/* =======================================================

      Find Player Object Function
      
======================================================= */

JSValueRef js_map_object_find_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,server.player_obj_idx));
}

JSValueRef js_map_object_find_all_players_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int			n,cnt,uids[max_obj_list];
	obj_type	*obj;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	cnt=0;

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if ((obj->type==object_type_player) || (obj->type==object_type_remote_player) || (obj->type==object_type_bot_multiplayer)) uids[cnt++]=obj->idx;
	}

	if (cnt==0) return(script_null_to_value(cx));

	return(script_int_array_to_value(cx,cnt,uids));
}

/* =======================================================

      Find Nearest Object Functions
      
======================================================= */

JSValueRef js_map_object_nearest_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					type,min_dist,max_dist;
	char				*name_ptr,name[name_str_len];
	float				ang,ang_sweep;
	d3pnt				pnt;
	obj_type			*obj;
	
	name_ptr=NULL;
	type=-1;
	ang=-1;
	ang_sweep=360;
	
	if (!script_check_param_count(cx,func,argc,7,exception)) return(script_null_to_value(cx));
	
	script_value_to_point(cx,argv[0],&pnt);
	if (!script_is_value_null(cx,argv[1])) {
		script_value_to_string(cx,argv[1],name,name_str_len);
		name_ptr=name;
	}
	if (!script_is_value_null(cx,argv[2])) type=script_value_to_int(cx,argv[2])-sd_object_type_player;
	if (!script_is_value_null(cx,argv[3])) ang=script_value_to_float(cx,argv[3]);
	if (!script_is_value_null(cx,argv[4])) ang_sweep=script_value_to_float(cx,argv[4]);
	min_dist=script_value_to_int(cx,argv[5]);
	max_dist=script_value_to_int(cx,argv[6]);
	
		// find object

	obj=object_find_nearest(&pnt,name_ptr,type,-1,ang,ang_sweep,min_dist,max_dist,FALSE,-1);
	if (obj==NULL) return(script_int_to_value(cx,-1));
	
	return(script_int_to_value(cx,obj->idx));
}

JSValueRef js_map_object_nearest_skip_object_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					type,min_dist,max_dist;
	char				*name_ptr,name[name_str_len];
	float				ang,ang_sweep;
	d3pnt				pnt;
	obj_type			*obj;

	name_ptr=NULL;
	type=-1;
	ang=-1;
	ang_sweep=360;
	
	if (!script_check_param_count(cx,func,argc,8,exception)) return(script_null_to_value(cx));

	script_value_to_point(cx,argv[0],&pnt);
	if (!script_is_value_null(cx,argv[1])) {
		script_value_to_string(cx,argv[1],name,name_str_len);
		name_ptr=name;
	}
	if (!script_is_value_null(cx,argv[2])) type=script_value_to_int(cx,argv[2])-sd_object_type_player;
	if (!script_is_value_null(cx,argv[3])) ang=script_value_to_float(cx,argv[3]);
	if (!script_is_value_null(cx,argv[4])) ang_sweep=script_value_to_float(cx,argv[4]);
	min_dist=script_value_to_int(cx,argv[5]);
	max_dist=script_value_to_int(cx,argv[6]);
	
		// find object

	obj=object_find_nearest(&pnt,name_ptr,type,-1,ang,ang_sweep,min_dist,max_dist,FALSE,script_value_to_int(cx,argv[7]));
	if (obj==NULL) return(script_int_to_value(cx,-1));
	
	return(script_int_to_value(cx,obj->idx));
}

JSValueRef js_map_object_nearest_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					min_dist,max_dist;
	float				ang,ang_sweep;
	d3pnt				pnt;
	obj_type			*obj;

	ang=-1;
	ang_sweep=360;

	if (!script_check_param_count(cx,func,argc,5,exception)) return(script_null_to_value(cx));

	script_value_to_point(cx,argv[0],&pnt);
	if (!script_is_value_null(cx,argv[1])) ang=script_value_to_float(cx,argv[1]);
	if (!script_is_value_null(cx,argv[2])) ang_sweep=script_value_to_float(cx,argv[2]);
	min_dist=script_value_to_int(cx,argv[3]);
	max_dist=script_value_to_int(cx,argv[4]);
	
		// find object

	obj=object_find_nearest(&pnt,NULL,-1,-1,ang,ang_sweep,min_dist,max_dist,TRUE,-1);
	if (obj==NULL) return(script_int_to_value(cx,-1));

	return(script_int_to_value(cx,obj->idx));
}

JSValueRef js_map_object_nearest_player_skip_object_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					min_dist,max_dist;
	float				ang,ang_sweep;
	d3pnt				pnt;
	obj_type			*obj;
	
	ang=-1;
	ang_sweep=360;

	if (!script_check_param_count(cx,func,argc,6,exception)) return(script_null_to_value(cx));

	script_value_to_point(cx,argv[0],&pnt);
	if (!script_is_value_null(cx,argv[1])) ang=script_value_to_float(cx,argv[1]);
	if (!script_is_value_null(cx,argv[2])) ang_sweep=script_value_to_float(cx,argv[2]);
	min_dist=script_value_to_int(cx,argv[3]);
	max_dist=script_value_to_int(cx,argv[4]);
	
		// find object

	obj=object_find_nearest(&pnt,NULL,-1,-1,ang,ang_sweep,min_dist,max_dist,TRUE,script_value_to_int(cx,argv[5]));
	if (obj==NULL) return(script_int_to_value(cx,-1));
	
	return(script_int_to_value(cx,obj->idx));
}

JSValueRef js_map_object_nearest_player_skip_self_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					min_dist,max_dist;
	float				ang,ang_sweep;
	d3pnt				pnt;
	obj_type			*obj;
	
	ang=-1;
	ang_sweep=360;

	if (!script_check_param_count(cx,func,argc,5,exception)) return(script_null_to_value(cx));

	script_value_to_point(cx,argv[0],&pnt);
	if (!script_is_value_null(cx,argv[1])) ang=script_value_to_float(cx,argv[1]);
	if (!script_is_value_null(cx,argv[2])) ang_sweep=script_value_to_float(cx,argv[2]);
	min_dist=script_value_to_int(cx,argv[3]);
	max_dist=script_value_to_int(cx,argv[4]);
	
		// find object

	obj=object_find_nearest(&pnt,NULL,-1,-1,ang,ang_sweep,min_dist,max_dist,TRUE,script_get_attached_object_uid(j_obj));
	if (obj==NULL) return(script_int_to_value(cx,-1));

	return(script_int_to_value(cx,obj->idx));
}

JSValueRef js_map_object_nearest_player_skip_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					obj_idx,team_idx,min_dist,max_dist;
	float				ang,ang_sweep;
	d3pnt				pnt;
	obj_type			*obj;
	
	ang=-1;
	ang_sweep=360;

	if (!script_check_param_count(cx,func,argc,5,exception)) return(script_null_to_value(cx));

	script_value_to_point(cx,argv[0],&pnt);
	if (!script_is_value_null(cx,argv[1])) ang=script_value_to_float(cx,argv[1]);
	if (!script_is_value_null(cx,argv[2])) ang_sweep=script_value_to_float(cx,argv[2]);
	min_dist=script_value_to_int(cx,argv[3]);
	max_dist=script_value_to_int(cx,argv[4]);

		// get team

	team_idx=-1;

	obj_idx=script_get_attached_object_uid(j_obj);
	if (obj_idx!=-1) {
		obj=server.obj_list.objs[obj_idx];
		if (obj!=NULL) {
			switch (obj->team_idx) {
				case net_team_red:
					team_idx=net_team_blue;
					break;
				case net_team_blue:
					team_idx=net_team_red;
					break;
			}
		}
	}
	
		// find object

	obj=object_find_nearest(&pnt,NULL,-1,team_idx,ang,ang_sweep,min_dist,max_dist,TRUE,-1);
	if (obj==NULL) return(script_int_to_value(cx,-1));

	return(script_int_to_value(cx,obj->idx));
}

JSValueRef js_map_object_nearest_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					min_dist,max_dist,team_idx;
	float				ang,ang_sweep;
	d3pnt				pnt;
	obj_type			*obj;

	ang=-1;
	ang_sweep=360;

	if (!script_check_param_count(cx,func,argc,6,exception)) return(script_null_to_value(cx));

	script_value_to_point(cx,argv[0],&pnt);
	team_idx=script_value_to_int(cx,argv[1])-sd_team_none;
	if (!script_is_value_null(cx,argv[2])) ang=script_value_to_float(cx,argv[2]);
	if (!script_is_value_null(cx,argv[3])) ang_sweep=script_value_to_float(cx,argv[3]);
	min_dist=script_value_to_int(cx,argv[4]);
	max_dist=script_value_to_int(cx,argv[5]);
	
		// find object

	obj=object_find_nearest(&pnt,NULL,-1,team_idx,ang,ang_sweep,min_dist,max_dist,FALSE,-1);
	if (obj==NULL) return(script_int_to_value(cx,-1));

	return(script_int_to_value(cx,obj->idx));
}

/* =======================================================

      ID Lists
      
======================================================= */

JSValueRef js_map_object_getAllIds_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,cnt,ids[max_obj_list];
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	cnt=0;
	
	for (n=0;n!=max_obj_list;n++) {
		if (server.obj_list.objs[n]!=NULL) {
			ids[cnt++]=server.obj_list.objs[n]->idx;
		}
	}
	
	return(script_int_array_to_value(cx,cnt,ids));
}

JSValueRef js_map_object_getAllNearIds_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				min_dist,max_dist,
					cnt,ids[max_obj_list];
	d3pnt			pnt;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	script_value_to_point(cx,argv[0],&pnt);
	min_dist=script_value_to_int(cx,argv[1]);
	max_dist=script_value_to_int(cx,argv[2]);
	
	cnt=object_find_nearest_list(&pnt,min_dist,max_dist,ids);
	
	return(script_int_array_to_value(cx,cnt,ids));
}


/* =======================================================

      Get Object Information
      
======================================================= */

JSValueRef js_map_object_get_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	script_type		*script;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	if (obj->script_idx==-1) return(script_null_to_value(cx));

	script=js.script_list.scripts[obj->script_idx];
	return((JSValueRef)script->obj);
}

JSValueRef js_map_object_get_name_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	return(script_string_to_value(cx,obj->name));
}

JSValueRef js_map_object_get_type_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	return(script_int_to_value(cx,obj->type+sd_object_type_player));
}

JSValueRef js_map_object_get_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	return(script_int_to_value(cx,obj->team_idx+sd_team_none));
}

JSValueRef js_map_object_get_team_name_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			str[32];
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));
	
	object_team_get_name(obj->team_idx,str);
	return(script_string_to_value(cx,str));
}

JSValueRef js_map_object_get_team_color_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3col			col;
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));
	
	object_team_get_tint(obj->team_idx,&col);

	return(script_color_to_value(cx,&col));
}

/* =======================================================

      Object Distance and Angle To
      
======================================================= */

JSValueRef js_map_object_get_distance_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3pnt			pnt;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));
	
	script_value_to_point(cx,argv[1],&pnt);
		
	return(script_int_to_value(cx,distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,pnt.x,pnt.y,pnt.z)));
}

JSValueRef js_map_object_get_angle_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3pnt			pnt;
	d3ang			ang;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	script_value_to_point(cx,argv[1],&pnt);
	
		// get angles to
		
	ang.x=angle_find(pnt.y,pnt.z,obj->pnt.y,obj->pnt.z);
	ang.y=angle_find(pnt.x,pnt.z,obj->pnt.x,obj->pnt.z);
	ang.z=angle_find(pnt.x,pnt.y,obj->pnt.x,obj->pnt.y);
		
	return(script_angle_to_value(cx,&ang));
}

JSValueRef js_map_object_get_angle_to_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3ang			ang;
	obj_type		*obj,*obj2;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
		// uids
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	obj2=script_find_obj_from_uid_arg(cx,argv[1],exception);
	if (obj2==NULL) return(script_null_to_value(cx));
	
		// get angles to
		
	ang.x=angle_find(obj2->pnt.y,obj2->pnt.z,obj->pnt.y,obj->pnt.z);
	ang.y=angle_find(obj2->pnt.x,obj2->pnt.z,obj->pnt.x,obj->pnt.z);
	ang.z=angle_find(obj2->pnt.x,obj2->pnt.y,obj->pnt.x,obj->pnt.y);
		
	return(script_angle_to_value(cx,&ang));
}

/* =======================================================

      Facing
      
======================================================= */

JSValueRef js_map_object_is_facing_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			ang_y,ang_dif;
	bool			cwise;
	obj_type		*obj,*obj2;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
		// uids
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	obj2=script_find_obj_from_uid_arg(cx,argv[1],exception);
	if (obj2==NULL) return(script_null_to_value(cx));
	
		// get angles to
		
	ang_y=angle_find(obj->pnt.x,obj->pnt.z,obj2->pnt.x,obj2->pnt.z);

	ang_dif=angle_dif(ang_y,angle_add(obj->ang.y,obj->face.ang.y),&cwise);
	return(script_bool_to_value(cx,ang_dif<script_value_to_float(cx,argv[2])));
}

/* =======================================================

      Object Position, Angle, Size
      
======================================================= */

JSValueRef js_map_object_get_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
		// uid
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));
	
		// get position
		
	return(script_point_to_value(cx,&obj->pnt));
}

JSValueRef js_map_object_get_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3ang			ang;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
		// uid
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));
	
		// get angle

	ang.x=angle_add(obj->ang.x,obj->face.ang.x);
	ang.y=angle_add(obj->ang.y,obj->face.ang.y);
	ang.z=angle_add(obj->ang.z,obj->face.ang.z);
		
	return(script_angle_to_value(cx,&ang));
}

JSValueRef js_map_object_get_size_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3pnt			pnt;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
		// uid
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));
	
		// get size
		
	pnt.x=obj->size.x;
	pnt.y=obj->size.y;
	pnt.z=obj->size.z;

	return(script_point_to_value(cx,&pnt));
}

JSValueRef js_map_object_get_health_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
		// uid
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));
	
		// get size
		
	return(script_int_to_value(cx,obj->status.health.value));
}

JSValueRef js_map_object_is_dead_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));
		
	return(script_bool_to_value(cx,obj->status.health.value<=0));
}

JSValueRef js_map_object_is_hidden_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));
		
	return(script_bool_to_value(cx,obj->hidden));
}

JSValueRef js_map_object_is_contact_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));
		
	return(script_bool_to_value(cx,(obj->contact.object_on) || (obj->contact.projectile_on) || (obj->contact.force_on)));
}

JSValueRef js_map_object_is_max_health_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));
		
	return(script_bool_to_value(cx,obj->status.health.value>=obj->status.health.max_value));
}

JSValueRef js_map_object_get_last_damage_object_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));
		
	return(script_int_to_value(cx,obj->damage_obj_idx));
}

JSValueRef js_map_object_was_telefrag_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));
		
	return(script_bool_to_value(cx,obj->death_telefrag));
}

/* =======================================================

      Object Moving and Shoving
      
======================================================= */

JSValueRef js_map_object_move_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3pnt			pnt;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));

	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	script_value_to_point(cx,argv[1],&pnt);

	object_set_position(obj,pnt.x,pnt.y,pnt.z,script_value_to_float(cx,argv[2]),0);
	object_telefrag_players(obj,FALSE);

	return(script_null_to_value(cx));
}

JSValueRef js_map_object_shove_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			speed;
	d3ang			ang;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));

	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	script_value_to_angle(cx,argv[1],&ang);
	speed=script_value_to_float(cx,argv[2]);

	object_shove(obj,&ang,speed);
	return(script_null_to_value(cx));
}

JSValueRef js_map_object_shove_direct_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3vct			vct;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	script_value_to_vector(cx,argv[1],&vct);

	object_shove_direct(obj,&vct);
	return(script_null_to_value(cx));
}

/* =======================================================

      Object Scoring
      
======================================================= */

JSValueRef js_map_object_add_goal_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj!=NULL) object_score_goal(obj);

	return(script_null_to_value(cx));
}

/* =======================================================

      Object Setting
      
======================================================= */

JSValueRef js_map_object_set_contact_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	bool			on;
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj!=NULL) {
		on=script_value_to_bool(cx,argv[1]);

		obj->contact.object_on=on;
		obj->contact.projectile_on=on;
		obj->contact.force_on=on;
	}

	return(script_null_to_value(cx));
}

JSValueRef js_map_object_set_hidden_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj!=NULL) obj->hidden=script_value_to_bool(cx,argv[1]);

	return(script_null_to_value(cx));
}

JSValueRef js_map_object_set_model_light_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					idx;
	obj_type			*obj;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj!=NULL) {
		idx=script_value_to_int(cx,argv[1]);
		if ((idx>=0) && (idx<max_model_light)) obj->draw.lights[idx].on=script_value_to_bool(cx,argv[2]);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_map_object_set_model_halo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					idx;
	obj_type			*obj;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj!=NULL) {
		idx=script_value_to_int(cx,argv[1]);
		if ((idx>=0) && (idx<max_model_halo)) obj->draw.halos[idx].on=script_value_to_bool(cx,argv[2]);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_map_object_set_model_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					idx;
	char				name[name_str_len];
	obj_type			*obj;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[1],name,name_str_len);
	idx=model_find_mesh_from_draw(&obj->draw,name);
	if (idx==-1) return(script_null_to_value(cx));

	if (script_value_to_bool(cx,argv[2])) {
		obj->draw.mesh_mask|=(0x1<<idx);
	}
	else {
		obj->draw.mesh_mask&=((0x1<<idx)^0xFF);
	}

	return(script_null_to_value(cx));
}

/* =======================================================

      Spawning and Removing
      
======================================================= */

JSValueRef js_map_object_spawn_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				uid;
	char			name[name_str_len],err_str[256],
					script[name_str_len],params[256];
	d3pnt			pnt;
	d3ang			ang;

	if (!script_check_param_count(cx,func,argc,5,exception)) return(script_null_to_value(cx));

		// spawn values

	script_value_to_string(cx,argv[0],name,name_str_len);
	script_value_to_string(cx,argv[1],script,name_str_len);
	script_value_to_string(cx,argv[2],params,256);
	script_value_to_point(cx,argv[3],&pnt);
	script_value_to_angle(cx,argv[4],&ang);

		// spawn

	uid=object_script_spawn(name,script,params,&pnt,&ang,FALSE,err_str);
	if (uid==-1) {
		*exception=script_create_exception(cx,err_str);
		return(script_null_to_value(cx));
	}

		// return UID

	return(script_int_to_value(cx,uid));
}

JSValueRef js_map_object_remove_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256];
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	if (!object_script_remove(obj->idx,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

