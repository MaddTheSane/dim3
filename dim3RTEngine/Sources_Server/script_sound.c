/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: sound object

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
#include "network.h"
#include "scripts.h"
#include "objects.h"

extern app_type				app;
extern iface_type			iface;
extern js_type				js;
extern setup_type			setup;
extern server_type			server;
extern network_setup_type	net_setup;

JSValueRef js_sound_play_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_play_at_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_play_global_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_play_global_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_start_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_stop_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_set_music_looping_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_fade_in_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_fade_out_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_fade_out_fade_in_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_cross_fade_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	sound_functions[]={
							{"play",				js_sound_play_func,						kJSPropertyAttributeDontDelete},
							{"playAtObject",		js_sound_play_at_object_func,			kJSPropertyAttributeDontDelete},
							{"playGlobal",			js_sound_play_global_func,				kJSPropertyAttributeDontDelete},
							{"playGlobalPlayer",	js_sound_play_global_player_func,		kJSPropertyAttributeDontDelete},
							{"startMusic",			js_sound_start_music_func,				kJSPropertyAttributeDontDelete},
							{"stopMusic",			js_sound_stop_music_func,				kJSPropertyAttributeDontDelete},
							{"setMusicLooping",		js_sound_set_music_looping_func,		kJSPropertyAttributeDontDelete},
							{"fadeInMusic",			js_sound_fade_in_music_func,			kJSPropertyAttributeDontDelete},
							{"fadeOutMusic",		js_sound_fade_out_music_func,			kJSPropertyAttributeDontDelete},
							{"fadeOutFadeInMusic",	js_sound_fade_out_fade_in_music_func,	kJSPropertyAttributeDontDelete},
							{"crossFadeMusic",		js_sound_cross_fade_music_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			sound_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_global_sound_object(void)
{
	sound_class=script_create_class("sound_class",NULL,sound_functions);
}

void script_free_global_sound_object(void)
{
	script_free_class(sound_class);
}

JSObjectRef script_add_global_sound_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,sound_class,"sound",script_idx));
}

/* =======================================================

      Sound Exceptions
      
======================================================= */

JSValueRef js_sound_name_exception(JSContextRef cx,char *name)
{
	char			err_str[256];

	sprintf(err_str,"Named sound does not exist: %s",name);
	return(script_create_exception(cx,err_str));
}

JSValueRef js_sound_music_name_exception(JSContextRef cx,char *name)
{
	char			err_str[256];

	sprintf(err_str,"Named music does not exist or could not be played: %s",name);
	return(script_create_exception(cx,err_str));
}

/* =======================================================

      Main Sound Play
      
======================================================= */

void script_sound_play(JSContextRef cx,JSObjectRef j_obj,char *name,d3pnt *pt,float pitch,bool global,bool atplayer,JSValueRef *exception)
{
	int				buffer_idx,sound_obj_idx;
	bool			player;
	obj_type		*obj;
	
		// check if this is player

	if (atplayer) {
		player=TRUE;
		sound_obj_idx=server.player_obj_idx;
	}
	else {
		sound_obj_idx=script_get_attached_object_uid(j_obj);
		if (sound_obj_idx==-1) {
			player=FALSE;
		}
		else {
			obj=server.obj_list.objs[sound_obj_idx];
			player=(obj->idx==server.player_obj_idx);
		}
	}
	
		// turn off global if you aren't the player

	if (!player) global=FALSE;
	
		// play sound
		
	if (!app.dedicated_host) {
		buffer_idx=al_find_buffer(name);
		if (buffer_idx==-1) {
			*exception=js_sound_name_exception(cx,name);
			return;
		}

		al_play_source(buffer_idx,pt,pitch,FALSE,FALSE,global,player);
	}
	
		// run sound watches

	if (sound_obj_idx!=-1) object_watch_sound_alert(pt,sound_obj_idx,name);
	
		// detect if sound should be remoted
		// we check to see if the script had any object attached
		// (or parented to an object), as the object tells if the
		// sound is networkable
		
	if ((net_setup.mode!=net_mode_none) && (sound_obj_idx!=-1)) {
		obj=server.obj_list.objs[sound_obj_idx];
		if (object_networkable(obj)) net_client_send_sound(obj,pt,pitch,name);
	}
}

/* =======================================================

      Sound Functions
      
======================================================= */

JSValueRef js_sound_play_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			pitch;
	char			name[name_str_len];
	d3pnt			pnt;

	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));

	script_value_to_string(cx,argv[0],name,name_str_len);
	script_value_to_point(cx,argv[1],&pnt);
	pitch=script_value_to_float(cx,argv[2]);

	script_sound_play(cx,j_obj,name,&pnt,pitch,FALSE,FALSE,exception);

	return(script_null_to_value(cx));
}

JSValueRef js_sound_play_at_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			pitch;
	char			name[name_str_len];
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[1],exception);
	if (obj==NULL) return(FALSE);
	
	script_value_to_string(cx,argv[0],name,name_str_len);
	pitch=script_value_to_float(cx,argv[2]);

	script_sound_play(cx,j_obj,name,&obj->pnt,pitch,FALSE,FALSE,exception);
	
	return(script_null_to_value(cx));
}

JSValueRef js_sound_play_global_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			pitch;
	char			name[name_str_len];
	d3pnt			pt;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[0],name,name_str_len);
	pitch=script_value_to_float(cx,argv[1]);
	
	pt.x=pt.y=pt.z=0;

	script_sound_play(cx,j_obj,name,&pt,pitch,TRUE,FALSE,exception);
	
	return(script_null_to_value(cx));
}

JSValueRef js_sound_play_global_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			pitch;
	char			name[name_str_len];
	d3pnt			pt;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[0],name,name_str_len);
	pitch=script_value_to_float(cx,argv[1]);
	
	pt.x=pt.y=pt.z=0;

	script_sound_play(cx,j_obj,name,&pt,pitch,TRUE,TRUE,exception);
	
	return(script_null_to_value(cx));
}

/* =======================================================

      Music Functions
      
======================================================= */

JSValueRef js_sound_start_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len],err_str[256];

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	if ((setup.music_on) && (!app.dedicated_host)) {
	
		script_value_to_string(cx,argv[0],name,name_str_len);

		if (!al_music_play(name,err_str)) {
			*exception=script_create_exception(cx,err_str);
		}
	}

	return(script_null_to_value(cx));
}

JSValueRef js_sound_stop_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	if ((setup.music_on) && (!app.dedicated_host)) al_music_stop();

	return(script_null_to_value(cx));
}

JSValueRef js_sound_set_music_looping_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	if ((setup.music_on) && (!app.dedicated_host)) al_music_set_loop(script_value_to_bool(cx,argv[0]));

	return(script_null_to_value(cx));
}

JSValueRef js_sound_fade_in_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				msec;
	char			name[name_str_len],err_str[256];
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	if ((setup.music_on) && (!app.dedicated_host)) {

		script_value_to_string(cx,argv[0],name,name_str_len);
		msec=script_value_to_int(cx,argv[1]);

		if (!al_music_fade_in(name,msec,err_str)) {
			*exception=script_create_exception(cx,err_str);
		}
	}

	return(script_null_to_value(cx));
}

JSValueRef js_sound_fade_out_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					msec;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	if ((setup.music_on) && (!app.dedicated_host)) {

		msec=script_value_to_int(cx,argv[0]);
		al_music_fade_out(msec);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_sound_fade_out_fade_in_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					fade_out_msec,fade_in_msec;
	char				name[name_str_len],err_str[256];
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	if ((setup.music_on) && (!app.dedicated_host)) {

		script_value_to_string(cx,argv[0],name,name_str_len);
		fade_out_msec=script_value_to_int(cx,argv[1]);
		fade_in_msec=script_value_to_int(cx,argv[2]);

		if (!al_music_fade_out_fade_in(name,fade_out_msec,fade_in_msec,err_str)) {
			*exception=script_create_exception(cx,err_str);
		}
	}

	return(script_null_to_value(cx));
}

JSValueRef js_sound_cross_fade_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					cross_msec;
	char				name[name_str_len],err_str[256];
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	if ((setup.music_on) && (!app.dedicated_host)) {

		script_value_to_string(cx,argv[0],name,name_str_len);
		cross_msec=script_value_to_int(cx,argv[1]);

		if (!al_music_cross_fade(name,cross_msec,err_str)) {
			*exception=script_create_exception(cx,err_str);
		}
	}

	return(script_null_to_value(cx));

}
