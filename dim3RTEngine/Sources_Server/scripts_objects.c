/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Object Creation

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

extern js_type			js;
extern iface_type		iface;

/* =======================================================

      Create dim3 Script Classes
      
======================================================= */

JSClassRef script_create_class(const char *name,const JSStaticValue *props,const JSStaticFunction *funcs)
{
	JSClassDefinition		def;

	bzero(&def,sizeof(JSClassDefinition));

	def.version=0;
	def.attributes=kJSClassAttributeNone;
	def.className=name;
	def.parentClass=NULL;
	def.staticValues=props;
	def.staticFunctions=funcs;
	def.initialize=NULL;
	def.finalize=NULL;
	def.hasProperty=NULL;
	def.getProperty=NULL;
	def.setProperty=NULL;
	def.deleteProperty=NULL;
	def.getPropertyNames=NULL;
	def.callAsFunction=NULL;
	def.callAsConstructor=NULL;
	def.hasInstance=NULL;
	def.convertToType=NULL;

	return(JSClassCreate(&def));
}

void script_free_class(JSClassRef cls)
{
	JSClassRelease(cls);
}

/* =======================================================

      Initialize/Release Classes
      
======================================================= */

void script_initialize_classes(void)
{
		// main empty class
		
	js.main_empty_class=script_create_class("main_empty_class",NULL,NULL);

		// constructors

	script_init_global_point_angle_vector_object();
	script_init_global_color_object();
	
		// global classes
		
	script_init_global_script_object();

	script_init_global_map_object();
	script_init_map_info_object();
	script_init_map_setting_object();
	script_init_map_action_object();
	script_init_map_fog_object();
	script_init_map_fog_color_object();
	script_init_map_group_object();
	script_init_map_light_color_object();
	script_init_map_movement_object();
	script_init_map_node_object();
	script_init_map_object_object();
	script_init_map_spot_object();
	script_init_map_light_object();
	script_init_map_polygon_object();

	script_init_global_singleplayer_object();
	script_init_singleplayer_setting_object();
	script_init_singleplayer_high_score_object();

	script_init_global_multiplayer_object();
	script_init_multiplayer_bot_object();
	script_init_multiplayer_score_object();
	script_init_multiplayer_setting_object();

	script_init_global_camera_object();
	script_init_camera_angle_object();
	script_init_camera_chase_object();
	script_init_camera_chase_track_angle_object();
	script_init_camera_chase_slop_object();
	script_init_camera_plane_object();
	script_init_camera_position_object();
	script_init_camera_setting_object();
	script_init_camera_state_object();
	script_init_camera_static_position_object();

	script_init_global_interface_object();
	script_init_interface_bar_object();
	script_init_interface_bitmap_object();
	script_init_interface_console_object();
	script_init_interface_interaction_object();
	script_init_interface_radar_object();
	script_init_interface_screen_object();
	script_init_interface_text_object();
	script_init_interface_virtual_control_object();

	script_init_global_data_object();
	script_init_global_sound_object();
	script_init_global_spawn_object();

	script_init_global_utility_object();
	script_init_utility_angle_object();
	script_init_utility_point_object();
	script_init_utility_random_object();

		// script classes
		
	script_init_game_setting_object();

	script_init_model_object();
	script_init_model_animation_object();
	script_init_model_bone_object();
	script_init_model_fill_object();
	script_init_model_halo_object();
	script_init_model_light_object();
	script_init_model_light_color_object();
	script_init_model_mesh_object();
	script_init_model_offset_object();
	script_init_model_rotate_object();
	script_init_model_spin_object();

	script_init_event_object();

	script_init_obj_setting_object();
	script_init_obj_position_object();
	script_init_obj_angle_object();
	script_init_obj_lock_object();
	script_init_obj_status_object();
	script_init_obj_motion_vector_object();
	script_init_obj_motion_angle_object();
	script_init_obj_forward_speed_object();
	script_init_obj_side_speed_object();
	script_init_obj_vert_speed_object();
	script_init_obj_turn_speed_object();
	script_init_obj_object_speed_object();
	script_init_obj_thrust_object();
	script_init_obj_look_object();
	script_init_obj_sight_object();
	script_init_obj_health_object();
	script_init_obj_size_object();
	script_init_obj_vehicle_object();
	script_init_obj_radar_object();
	script_init_obj_score_object();
	script_init_obj_touch_object();
	script_init_obj_touch_position_object();
	script_init_obj_touch_angle_object();
	script_init_obj_hit_object();
	script_init_obj_hit_position_object();
	script_init_obj_hit_angle_object();
	script_init_obj_click_object();
	script_init_obj_held_object();
	script_init_obj_pickup_object();
	script_init_obj_label_object();
	script_init_obj_watch_object();
	script_init_obj_watch_position_object();
	script_init_obj_watch_angle_object();
	script_init_obj_weapon_object();
	script_init_obj_weapon_fire_object();
	script_init_obj_melee_object();

	script_init_weap_setting_object();
	script_init_weap_hand_object();
	script_init_weap_hand_position_object();
	script_init_weap_hand_angle_object();
	script_init_weap_projectile_object();
	script_init_weap_crosshair_object();
	script_init_weap_crosshair_color_object();
	script_init_weap_crosshair_empty_color_object();
	script_init_weap_crosshair_pickup_color_object();
	script_init_weap_ammo_object();
	script_init_weap_alt_ammo_object();
	script_init_weap_kickback_object();
	script_init_weap_recoil_object();
	script_init_weap_melee_object();
	script_init_weap_fire_object();
	script_init_weap_dual_object();
	script_init_weap_target_object();
	script_init_weap_target_color_object();
	script_init_weap_zoom_object();

	script_init_proj_setting_object();
	script_init_proj_position_object();
	script_init_proj_origin_object();
	script_init_proj_motion_vector_object();
	script_init_proj_motion_angle_object();
	script_init_proj_speed_object();
	script_init_proj_hit_object();
	script_init_proj_action_object();
	script_init_proj_push_object();
	script_init_proj_size_object();
	script_init_proj_melee_object();
}

void script_release_classes(void)
{
		// main class
		
	script_free_class(js.main_empty_class);

		// constructors

	script_free_global_point_angle_vector_object();
	script_free_global_color_object();

		// global classes

	script_free_global_script_object();
	
	script_free_global_map_object();
	script_free_map_info_object();
	script_free_map_setting_object();
	script_free_map_action_object();
	script_free_map_fog_object();
	script_free_map_fog_color_object();
	script_free_map_group_object();
	script_free_map_light_color_object();
	script_free_map_movement_object();
	script_free_map_node_object();
	script_free_map_object_object();
	script_free_map_spot_object();
	script_free_map_light_object();
	script_free_map_polygon_object();

	script_free_global_singleplayer_object();
	script_free_singleplayer_setting_object();
	script_free_singleplayer_high_score_object();

	script_free_global_multiplayer_object();
	script_free_multiplayer_bot_object();
	script_free_multiplayer_score_object();
	script_free_multiplayer_setting_object();

	script_free_global_camera_object();
	script_free_camera_angle_object();
	script_free_camera_chase_object();
	script_free_camera_chase_track_angle_object();
	script_free_camera_chase_slop_object();
	script_free_camera_plane_object();
	script_free_camera_position_object();
	script_free_camera_setting_object();
	script_free_camera_state_object();
	script_free_camera_static_position_object();

	script_free_global_interface_object();
	script_free_interface_bar_object();
	script_free_interface_bitmap_object();
	script_free_interface_console_object();
	script_free_interface_interaction_object();
	script_free_interface_radar_object();
	script_free_interface_screen_object();
	script_free_interface_text_object();
	script_free_interface_virtual_control_object();

	script_free_global_data_object();
	script_free_global_sound_object();
	script_free_global_spawn_object();

	script_free_global_utility_object();
	script_free_utility_angle_object();
	script_free_utility_point_object();
	script_free_utility_random_object();

		// script classes
		
	script_free_game_setting_object();

	script_free_model_object();
	script_free_model_animation_object();
	script_free_model_bone_object();
	script_free_model_fill_object();
	script_free_model_halo_object();
	script_free_model_light_object();
	script_free_model_light_color_object();
	script_free_model_mesh_object();
	script_free_model_offset_object();
	script_free_model_rotate_object();
	script_free_model_spin_object();

	script_free_event_object();

	script_free_obj_setting_object();
	script_free_obj_position_object();
	script_free_obj_angle_object();
	script_free_obj_lock_object();
	script_free_obj_status_object();
	script_free_obj_motion_vector_object();
	script_free_obj_motion_angle_object();
	script_free_obj_forward_speed_object();
	script_free_obj_side_speed_object();
	script_free_obj_vert_speed_object();
	script_free_obj_turn_speed_object();
	script_free_obj_object_speed_object();
	script_free_obj_thrust_object();
	script_free_obj_look_object();
	script_free_obj_sight_object();
	script_free_obj_health_object();
	script_free_obj_size_object();
	script_free_obj_vehicle_object();
	script_free_obj_radar_object();
	script_free_obj_score_object();
	script_free_obj_touch_object();
	script_free_obj_touch_position_object();
	script_free_obj_touch_angle_object();
	script_free_obj_hit_object();
	script_free_obj_hit_position_object();
	script_free_obj_hit_angle_object();
	script_free_obj_click_object();
	script_free_obj_held_object();
	script_free_obj_pickup_object();
	script_free_obj_label_object();
	script_free_obj_watch_object();
	script_free_obj_watch_position_object();
	script_free_obj_watch_angle_object();
	script_free_obj_weapon_object();
	script_free_obj_weapon_fire_object();
	script_free_obj_melee_object();

	script_free_weap_setting_object();
	script_free_weap_hand_object();
	script_free_weap_hand_position_object();
	script_free_weap_hand_angle_object();
	script_free_weap_projectile_object();
	script_free_weap_crosshair_object();
	script_free_weap_crosshair_color_object();
	script_free_weap_crosshair_empty_color_object();
	script_free_weap_crosshair_pickup_color_object();
	script_free_weap_ammo_object();
	script_free_weap_alt_ammo_object();
	script_free_weap_kickback_object();
	script_free_weap_recoil_object();
	script_free_weap_melee_object();
	script_free_weap_fire_object();
	script_free_weap_dual_object();
	script_free_weap_target_object();
	script_free_weap_target_color_object();
	script_free_weap_zoom_object();

	script_free_proj_setting_object();
	script_free_proj_position_object();
	script_free_proj_origin_object();
	script_free_proj_motion_vector_object();
	script_free_proj_motion_angle_object();
	script_free_proj_speed_object();
	script_free_proj_hit_object();
	script_free_proj_action_object();
	script_free_proj_push_object();
	script_free_proj_size_object();
	script_free_proj_melee_object();
}

/* =======================================================

      Add Global Objects to Script
      
======================================================= */

bool script_add_global_object(script_type *script,char *err_str)
{
	JSObjectRef			j_parent_obj,j_sub_obj,j_sub_obj_2;

		// dim3 object

	j_parent_obj=script->global_obj;

		// constructors

	script_add_global_point_angle_vector_object(script->cx,j_parent_obj,script->idx);
	script_add_global_color_object(script->cx,j_parent_obj,script->idx);
	
		// script object
		
	script_add_global_script_object(script->cx,j_parent_obj,script->idx);

		// map object

	j_sub_obj=script_add_global_map_object(script->cx,j_parent_obj,script->idx);
	script_add_map_info_object(script->cx,j_sub_obj,script->idx);
	script_add_map_setting_object(script->cx,j_sub_obj,script->idx);
	script_add_map_action_object(script->cx,j_sub_obj,script->idx);
	script_add_map_light_color_object(script->cx,j_sub_obj,script->idx);
	script_add_map_fog_object(script->cx,j_sub_obj,script->idx);
	script_add_map_fog_color_object(script->cx,j_sub_obj,script->idx);
	script_add_map_spot_object(script->cx,j_sub_obj,script->idx);
	script_add_map_node_object(script->cx,j_sub_obj,script->idx);
	script_add_map_object_object(script->cx,j_sub_obj,script->idx);
	script_add_map_group_object(script->cx,j_sub_obj,script->idx);
	script_add_map_movement_object(script->cx,j_sub_obj,script->idx);
	script_add_map_light_object(script->cx,j_sub_obj,script->idx);
	script_add_map_polygon_object(script->cx,j_sub_obj,script->idx);

		// single object
		
	j_sub_obj=script_add_global_singleplayer_object(script->cx,j_parent_obj,script->idx);
	script_add_singleplayer_setting_object(script->cx,j_sub_obj,script->idx);
	script_add_singleplayer_high_score_object(script->cx,j_sub_obj,script->idx);

		// multiplayer object
		
	j_sub_obj=script_add_global_multiplayer_object(script->cx,j_parent_obj,script->idx);
	script_add_multiplayer_setting_object(script->cx,j_sub_obj,script->idx);
	script_add_multiplayer_bot_object(script->cx,j_sub_obj,script->idx);
	script_add_multiplayer_score_object(script->cx,j_sub_obj,script->idx);

		// camera object
		
	j_sub_obj=script_add_global_camera_object(script->cx,j_parent_obj,script->idx);
	script_add_camera_setting_object(script->cx,j_sub_obj,script->idx);
	script_add_camera_position_object(script->cx,j_sub_obj,script->idx);
	script_add_camera_angle_object(script->cx,j_sub_obj,script->idx);
	j_sub_obj_2=script_add_camera_chase_object(script->cx,j_sub_obj,script->idx);
	script_add_camera_chase_track_angle_object(script->cx,j_sub_obj_2,script->idx);
	script_add_camera_chase_slop_object(script->cx,j_sub_obj_2,script->idx);
	script_add_camera_static_position_object(script->cx,j_sub_obj,script->idx);
	script_add_camera_plane_object(script->cx,j_sub_obj,script->idx);
	script_add_camera_state_object(script->cx,j_sub_obj,script->idx);

		// interface object
		
	j_sub_obj=script_add_global_interface_object(script->cx,j_parent_obj,script->idx);
	script_add_interface_screen_object(script->cx,j_sub_obj,script->idx);
	script_add_interface_console_object(script->cx,j_sub_obj,script->idx);
	script_add_interface_text_object(script->cx,j_sub_obj,script->idx);
	script_add_interface_bitmap_object(script->cx,j_sub_obj,script->idx);
	script_add_interface_bar_object(script->cx,j_sub_obj,script->idx);
	script_add_interface_radar_object(script->cx,j_sub_obj,script->idx);
	script_add_interface_interaction_object(script->cx,j_sub_obj,script->idx);
	script_add_interface_virtual_control_object(script->cx,j_sub_obj,script->idx);

		// data, sound, and spawn objects
		
	script_add_global_data_object(script->cx,j_parent_obj,script->idx);
	script_add_global_sound_object(script->cx,j_parent_obj,script->idx);
	script_add_global_spawn_object(script->cx,j_parent_obj,script->idx);

		// utility object
		
	j_sub_obj=script_add_global_utility_object(script->cx,j_parent_obj,script->idx);
	script_add_utility_angle_object(script->cx,j_sub_obj,script->idx);
	script_add_utility_point_object(script->cx,j_sub_obj,script->idx);
	script_add_utility_random_object(script->cx,j_sub_obj,script->idx);

	return(TRUE);
}

bool script_is_prop_global_object(char *name)
{
		// build in objects

	if (strcmp(name,"script")==0) return(TRUE);
	if (strcmp(name,"camera")==0) return(TRUE);
	if (strcmp(name,"data")==0) return(TRUE);
	if (strcmp(name,"iface")==0) return(TRUE);
	if (strcmp(name,"map")==0) return(TRUE);
	if (strcmp(name,"sound")==0) return(TRUE);
	if (strcmp(name,"spawn")==0) return(TRUE);
	if (strcmp(name,"utility")==0) return(TRUE);
	if (strcmp(name,"singleplayer")==0) return(TRUE);
	if (strcmp(name,"multiplayer")==0) return(TRUE);

		// build in types

	if (strcmp(name,"Point")==0) return(TRUE);
	if (strcmp(name,"Angle")==0) return(TRUE);
	if (strcmp(name,"Vector")==0) return(TRUE);
	if (strcmp(name,"Color")==0) return(TRUE);

	return(FALSE);
}

/* =======================================================

      Script Property and Function Utilities
      
======================================================= */

void script_set_single_property(JSContextRef cx,JSObjectRef j_obj,const char *prop_name,JSValueRef vp,int flags)
{
	JSStringRef			j_prop_name;

	j_prop_name=JSStringCreateWithUTF8CString(prop_name);
	JSObjectSetProperty(cx,j_obj,j_prop_name,vp,flags,NULL);
	JSStringRelease(j_prop_name);
}

JSValueRef script_get_single_property(JSContextRef cx,JSObjectRef j_obj,const char *prop_name)
{
	JSStringRef			j_prop_name;
	JSValueRef			vp;

	j_prop_name=JSStringCreateWithUTF8CString(prop_name);
	vp=JSObjectGetProperty(cx,j_obj,j_prop_name,NULL);
	JSStringRelease(j_prop_name);

	return(vp);
}

JSValueRef script_get_single_property_with_has_check(JSContextRef cx,JSObjectRef j_obj,const char *prop_name)
{
	JSStringRef			j_prop_name;
	JSValueRef			vp;

	j_prop_name=JSStringCreateWithUTF8CString(prop_name);
	
	if (!JSObjectHasProperty(cx,j_obj,j_prop_name)) {
		JSStringRelease(j_prop_name);
		return(NULL);
	}

	vp=JSObjectGetProperty(cx,j_obj,j_prop_name,NULL);
	
	JSStringRelease(j_prop_name);

	return(vp);
}

JSObjectRef script_get_single_function(JSContextRef cx,JSObjectRef j_obj,const char *func_name)
{
	JSValueRef			vp;
	
	vp=script_get_single_property(cx,j_obj,func_name);
	if (vp==NULL) return(NULL);
	if (JSValueIsNull(cx,vp)) return(NULL);
	if (!JSValueIsObject(cx,vp)) return(NULL);
	if (!JSObjectIsFunction(cx,(JSObjectRef)vp)) return(NULL);
	
	return((JSObjectRef)vp);
}

/* =======================================================

      Script Param and Call Time Checking
      
======================================================= */

bool script_check_param_count(JSContextRef cx,JSObjectRef func,int argc,int need_argc,JSValueRef *exception)
{
	char				func_name[64],err_str[256];
	JSValueRef			vp;
	
		// write params?
		
	if (argc==need_argc) return(TRUE);
	
		// get function name
	
	strcpy(func_name,"Unknown");
		
	if (func!=NULL) {
		vp=script_get_single_property(cx,func,"name");
		if (vp!=NULL) script_value_to_string(cx,vp,func_name,64);
	}
	
	sprintf(err_str,"Function '%s' requires %d parameters, found %d",func_name,need_argc,argc);
	*exception=script_create_exception(cx,err_str);

	return(FALSE);
}

bool script_check_param_at_least_count(JSContextRef cx,JSObjectRef func,int argc,int need_argc,JSValueRef *exception)
{
	char				func_name[64],err_str[256];
	JSValueRef			vp;
	
		// write params?
		
	if (argc>=need_argc) return(TRUE);
	
		// get function name
		
	vp=script_get_single_property(cx,func,"name");
	if (vp==NULL) {
		strcpy(func_name,"Unknown");
	}
	else {
		script_value_to_string(cx,vp,func_name,64);
	}
	
	sprintf(err_str,"Function '%s' requires at least %d parameters, found %d",func_name,argc,need_argc);
	
	*exception=script_create_exception(cx,err_str);

	return(FALSE);
}

bool script_check_fail_in_construct(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,JSValueRef *exception)
{
	char				func_name[64],err_str[256];
	JSValueRef			vp;

	if (!script_in_construct(j_obj)) return(TRUE);
	
	vp=script_get_single_property(cx,func,"name");
	if (vp==NULL) {
		strcpy(func_name,"Unknown");
	}
	else {
		script_value_to_string(cx,vp,func_name,64);
	}
	
	sprintf(err_str,"Function '%s' can not be called in a construct event, move call to spawn event",func_name);
	*exception=script_create_exception(cx,err_str);

	return(FALSE);
}

/* =======================================================

      Create dim3 Script Objects
      
======================================================= */

void script_create_main_object_add_model_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
    JSObjectRef			j_obj;

	j_obj=script_add_model_object(cx,parent_obj,script_idx);
	
	script_add_model_offset_object(cx,j_obj,script_idx);
	script_add_model_rotate_object(cx,j_obj,script_idx);
	script_add_model_spin_object(cx,j_obj,script_idx);
	script_add_model_light_object(cx,j_obj,script_idx);
	script_add_model_light_color_object(cx,j_obj,script_idx);
	script_add_model_halo_object(cx,j_obj,script_idx);
	script_add_model_animation_object(cx,j_obj,script_idx);
	script_add_model_mesh_object(cx,j_obj,script_idx);
	script_add_model_bone_object(cx,j_obj,script_idx);
	script_add_model_fill_object(cx,j_obj,script_idx);
}

JSObjectRef script_create_main_object(JSContextRef cx,int script_idx)
{
	script_type			*script;
	JSObjectRef			j_obj,j_sub_obj;

	j_obj=JSObjectMake(cx,js.main_empty_class,(void*)script_idx);
	if (j_obj==NULL) return(NULL);
		
		// add sub-objects

	script=js.script_list.scripts[script_idx];

	switch (script->attach.thing_type) {

		case thing_type_game:
			script_add_game_setting_object(cx,j_obj,script_idx);
			script_add_event_object(cx,j_obj,script_idx);
			break;

		case thing_type_course:
			script_add_event_object(cx,j_obj,script_idx);
			break;

		case thing_type_object:
			script_add_obj_setting_object(cx,j_obj,script_idx);
			script_add_event_object(cx,j_obj,script_idx);
			script_add_obj_position_object(cx,j_obj,script_idx);
			script_add_obj_angle_object(cx,j_obj,script_idx);
			script_add_obj_lock_object(cx,j_obj,script_idx);
			script_add_obj_status_object(cx,j_obj,script_idx);
			script_add_obj_motion_vector_object(cx,j_obj,script_idx);
			script_add_obj_motion_angle_object(cx,j_obj,script_idx);
			script_add_obj_forward_speed_object(cx,j_obj,script_idx);
			script_add_obj_side_speed_object(cx,j_obj,script_idx);
			script_add_obj_vert_speed_object(cx,j_obj,script_idx);
			script_add_obj_turn_speed_object(cx,j_obj,script_idx);
			script_add_obj_object_speed_object(cx,j_obj,script_idx);
			script_add_obj_thrust_object(cx,j_obj,script_idx);
			script_add_obj_look_object(cx,j_obj,script_idx);
			script_add_obj_sight_object(cx,j_obj,script_idx);
			script_add_obj_health_object(cx,j_obj,script_idx);
			script_add_obj_size_object(cx,j_obj,script_idx);
			script_add_obj_vehicle_object(cx,j_obj,script_idx);
			script_add_obj_radar_object(cx,j_obj,script_idx);
			script_add_obj_score_object(cx,j_obj,script_idx);
			script_add_obj_touch_object(cx,j_obj,script_idx);
			script_add_obj_touch_position_object(cx,j_obj,script_idx);
			script_add_obj_touch_angle_object(cx,j_obj,script_idx);
			script_add_obj_hit_object(cx,j_obj,script_idx);
			script_add_obj_hit_position_object(cx,j_obj,script_idx);
			script_add_obj_hit_angle_object(cx,j_obj,script_idx);
			script_add_obj_click_object(cx,j_obj,script_idx);
			script_add_obj_held_object(cx,j_obj,script_idx);
			script_add_obj_pickup_object(cx,j_obj,script_idx);
			script_add_obj_label_object(cx,j_obj,script_idx);
			script_add_obj_watch_object(cx,j_obj,script_idx);
			script_add_obj_watch_position_object(cx,j_obj,script_idx);
			script_add_obj_watch_angle_object(cx,j_obj,script_idx);
			script_add_obj_weapon_object(cx,j_obj,script_idx);
			script_add_obj_weapon_fire_object(cx,j_obj,script_idx);
			script_add_obj_melee_object(cx,j_obj,script_idx);
			script_create_main_object_add_model_object(cx,j_obj,script_idx);
			break;

		case thing_type_weapon:
			script_add_weap_setting_object(cx,j_obj,script_idx);
			script_add_event_object(cx,j_obj,script_idx);
			script_add_weap_hand_object(cx,j_obj,script_idx);
			script_add_weap_hand_position_object(cx,j_obj,script_idx);
			script_add_weap_hand_angle_object(cx,j_obj,script_idx);
			script_add_weap_projectile_object(cx,j_obj,script_idx);
			j_sub_obj=script_add_weap_crosshair_object(cx,j_obj,script_idx);
			script_add_weap_crosshair_color_object(cx,j_sub_obj,script_idx);
			script_add_weap_crosshair_empty_color_object(cx,j_sub_obj,script_idx);
			script_add_weap_crosshair_pickup_color_object(cx,j_sub_obj,script_idx);
			script_add_weap_kickback_object(cx,j_obj,script_idx);
			script_add_weap_recoil_object(cx,j_obj,script_idx);
			script_add_weap_ammo_object(cx,j_obj,script_idx);
			script_add_weap_alt_ammo_object(cx,j_obj,script_idx);
			script_add_weap_melee_object(cx,j_obj,script_idx);
			script_add_weap_fire_object(cx,j_obj,script_idx);
			script_add_weap_dual_object(cx,j_obj,script_idx);
			j_sub_obj=script_add_weap_target_object(cx,j_obj,script_idx);
			script_add_weap_target_color_object(cx,j_sub_obj,script_idx);
			script_add_weap_zoom_object(cx,j_obj,script_idx);
			script_create_main_object_add_model_object(cx,j_obj,script_idx);
			break;

		case thing_type_projectile:
			script_add_proj_setting_object(cx,j_obj,script_idx);
			script_add_event_object(cx,j_obj,script_idx);
			script_add_proj_position_object(cx,j_obj,script_idx);
			script_add_proj_origin_object(cx,j_obj,script_idx);
			script_add_proj_motion_vector_object(cx,j_obj,script_idx);
			script_add_proj_motion_angle_object(cx,j_obj,script_idx);
			script_add_proj_speed_object(cx,j_obj,script_idx);
			script_add_proj_hit_object(cx,j_obj,script_idx);
			script_add_proj_action_object(cx,j_obj,script_idx);
			script_add_proj_push_object(cx,j_obj,script_idx);
			script_add_proj_size_object(cx,j_obj,script_idx);
			script_add_proj_melee_object(cx,j_obj,script_idx);
			script_create_main_object_add_model_object(cx,j_obj,script_idx);

	}
	
	return(j_obj);
}

JSObjectRef script_create_child_object(JSContextRef cx,JSObjectRef parent_obj,JSClassRef cls,const char *name,int script_idx)
{
	JSObjectRef			j_obj;
	JSValueRef			j_obj_vp;
	
	j_obj=JSObjectMake(cx,cls,(void*)script_idx);
	j_obj_vp=JSValueToObject(cx,j_obj,NULL);

	script_set_single_property(cx,parent_obj,name,j_obj_vp,(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));

	return(j_obj);
}

