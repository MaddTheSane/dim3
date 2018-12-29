/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Save/Load State

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

extern server_type		server;
extern js_type			js;

extern bool game_file_add_chunk(void *data,int count,int sz);
extern char* game_file_get_chunk(void *data);

/* =======================================================

      Setup Script States for Save
      
======================================================= */

bool script_state_save_single(int script_idx,bool checkpoint,char *err_str)
{
	int						n,count,prop_name_len,prop_value_len;
	char					prop_name[256];
	char					*prop_value;
	script_type				*script;
	JSPropertyNameArrayRef	js_names;
	JSStringRef				js_prop_name,js_prop_json;
	JSValueRef				js_prop_value;

	if (script_idx==-1) return(TRUE);

		// send save state event

	scripts_post_event(script_idx,-1,sd_event_state,(checkpoint?sd_event_state_save_checkpoint:sd_event_state_save),0,err_str);

		// get the script

	script=js.script_list.scripts[script_idx];

		// run through the properities

	js_names=JSObjectCopyPropertyNames(script->cx,script->global_obj);
	count=JSPropertyNameArrayGetCount(js_names);

	for (n=0;n!=count;n++) {

			// get the property name and value

		js_prop_name=JSPropertyNameArrayGetNameAtIndex(js_names,n);
		js_prop_value=JSObjectGetProperty(script->cx,script->global_obj,js_prop_name,NULL);

			// skip all properties that are functions

		if (js_prop_value!=NULL) {
			if (JSValueIsObject(script->cx,js_prop_value)) {
				if (JSObjectIsFunction(script->cx,(JSObjectRef)js_prop_value)) continue;
			}
		}

			// skip all API objects or DIM3 defines

		JSStringGetUTF8CString(js_prop_name,prop_name,256);
		prop_name[255]=0x0;

		if (script_is_prop_global_object(prop_name)) continue;
		if (script_is_prop_define(prop_name)) continue;

			// process the property with JSON

		js_prop_json=JSValueCreateJSONString(script->cx,js_prop_value,0,NULL);
		if (js_prop_json==NULL) continue;
		
		prop_value_len=JSStringGetMaximumUTF8CStringSize(js_prop_json);
		prop_value=(char*)malloc(prop_value_len);
		if (prop_value==NULL) {
			strcpy(err_str,"Out of Memory");
			JSStringRelease(js_prop_json);
			return(FALSE);
		}

		prop_value_len=JSStringGetUTF8CString(js_prop_json,prop_value,prop_value_len);
		JSStringRelease(js_prop_json);
		
			// save property, we have to write a length
			// with each of these because they can be of any size

		prop_name_len=strlen(prop_name)+1;

		game_file_add_chunk(&prop_name_len,1,sizeof(int));
		game_file_add_chunk(prop_name,1,prop_name_len);

		game_file_add_chunk(&prop_value_len,1,sizeof(int));
		game_file_add_chunk(prop_value,1,prop_value_len);

			// free the property value JSON string

		free(prop_value);
	}

	JSPropertyNameArrayRelease(js_names);

		// end each block with a 0 prop name length

	prop_name_len=0;
	game_file_add_chunk(&prop_name_len,1,sizeof(int));

	return(TRUE);
}

bool script_state_save(bool checkpoint,char *err_str)
{
	int				n,k,i;
	obj_type		*obj;
	weapon_type		*weap;
	proj_setup_type	*proj_setup;

	if (!script_state_save_single(js.game_script_idx,checkpoint,err_str)) return(FALSE);
	if (!script_state_save_single(js.course_script_idx,checkpoint,err_str)) return(FALSE);
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (!script_state_save_single(obj->script_idx,checkpoint,err_str)) return(FALSE);

		for (k=0;k!=max_weap_list;k++) {
			weap=obj->weap_list.weaps[k];
			if (weap==NULL) continue;
			
			if (!script_state_save_single(weap->script_idx,checkpoint,err_str)) return(FALSE);
		
			for (i=0;i!=max_proj_setup_list;i++) {
				proj_setup=weap->proj_setup_list.proj_setups[i];
				if (proj_setup==NULL) continue;
				
				if (!script_state_save_single(proj_setup->script_idx,checkpoint,err_str)) return(FALSE);
			}
		}
	}

	return(TRUE);
}

/* =======================================================

      Setup Script States after Load
      
======================================================= */

bool script_state_load_single(int script_idx,bool checkpoint,char *err_str)
{
	int				prop_name_len,prop_value_len;
	char			prop_name[256];
	char			*prop_value;
	script_type		*script;
	JSStringRef		js_prop_name,js_prop_json;
	JSValueRef		js_prop_value;

	if (script_idx==-1) return(TRUE);

		// get the script

	script=js.script_list.scripts[script_idx];

		// run through the properties to replace
		// a 0 property name length means the end
		// of properties for this script

	while (TRUE) {

			// get the prop name and JSON value

		game_file_get_chunk(&prop_name_len);
		if (prop_name_len==0) break;

		game_file_get_chunk(prop_name);

		game_file_get_chunk(&prop_value_len);
		prop_value=(char*)malloc(prop_value_len);
		if (prop_value==NULL) {
			strcpy(err_str,"Out of Memory");
			return(FALSE);
		}

		game_file_get_chunk(prop_value);

			// process the property

		js_prop_name=JSStringCreateWithUTF8CString(prop_name);

		js_prop_json=JSStringCreateWithUTF8CString(prop_value);
		js_prop_value=JSValueMakeFromJSONString(script->cx,js_prop_json);
		JSStringRelease(js_prop_json);

		JSObjectSetProperty(script->cx,script->global_obj,js_prop_name,js_prop_value,0,NULL);
	
		JSStringRelease(js_prop_name);
	}

		// send load event to script

	scripts_post_event(script_idx,-1,sd_event_state,(checkpoint?sd_event_state_load_checkpoint:sd_event_state_load),0,err_str);

	return(TRUE);
}

bool script_state_load(bool checkpoint,char *err_str)
{
	int				n,k,i;
	obj_type		*obj;
	weapon_type		*weap;
	proj_setup_type	*proj_setup;
	proj_type		*proj;

		// load the game and course

	if (!script_state_load_single(js.game_script_idx,checkpoint,err_str)) return(FALSE);
	if (!script_state_load_single(js.course_script_idx,checkpoint,err_str)) return(FALSE);

		// load the objects

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (!script_state_load_single(obj->script_idx,checkpoint,err_str)) return(FALSE);

		for (k=0;k!=max_weap_list;k++) {
			weap=obj->weap_list.weaps[k];
			if (weap==NULL) continue;
			
			if (!script_state_load_single(weap->script_idx,checkpoint,err_str)) return(FALSE);
		
			for (i=0;i!=max_proj_setup_list;i++) {
				proj_setup=weap->proj_setup_list.proj_setups[i];
				if (proj_setup==NULL) continue;
				
				if (!script_state_load_single(proj_setup->script_idx,checkpoint,err_str)) return(FALSE);
			}
		}
	}

		// reattach any projectiles
		// to their projectile setup scripts

	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (!proj->on) continue;

		obj=server.obj_list.objs[proj->obj_idx];
		weap=obj->weap_list.weaps[proj->weap_idx];
		proj_setup=weap->proj_setup_list.proj_setups[proj->proj_setup_idx];

		proj->script_idx=proj_setup->script_idx;
	}

	return(TRUE);
}


