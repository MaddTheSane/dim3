/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Application Debugging

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

extern render_info_type		render_info;
extern map_type				map;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

char						object_type_str[][32]={"Player","Remote","Bot Multiplayer","Bot Map","Object"},
							bind_type_str[][16]={"Game","Map","Remote"},
							effect_type_str[][16]={"Flash","Particle","Lightning","Ray","Globe","Shake"};

/* =======================================================

      Debugging Dump Routines
      
======================================================= */

void debug_dump_header(FILE *file,char *str)
{
	fwrite("\r\n### ",1,6,file);
	fwrite(str,1,strlen(str),file);
	fwrite(" ###\r\n\r\n",1,8,file);
}

void debug_dump_info_str(FILE *file,char *title,char *value)
{
	fwrite(title,1,strlen(title),file);
	fwrite(": ",1,2,file);
	fwrite(value,1,strlen(value),file);
	fwrite("\r\n",1,2,file);
}

void debug_dump_info_int(FILE *file,char *title,int value)
{
	char		str[256];

	sprintf(str,"%d",value);

	fwrite(title,1,strlen(title),file);
	fwrite(": ",1,2,file);
	fwrite(str,1,strlen(str),file);
	fwrite("\r\n",1,2,file);
}

void debug_dump_info_enable(FILE *file,char *title,bool enabled)
{
	fwrite(title,1,strlen(title),file);
	fwrite(": ",1,2,file);
	if (enabled) {
		fwrite("Enabled",1,7,file);
	}
	else {
		fwrite("Disabled",1,8,file);
	}
	fwrite("\r\n",1,2,file);
}

void debug_info_return(FILE *file)
{
	fwrite("\r\n",1,2,file);
}

void debug_info_table_str(FILE *file,char *title,int fieldsz)
{
	int			strsz;
	char		str[256];
	
	strsz=strlen(title);
	
	if (strsz>=fieldsz) {
		strcpy(str,title);
	}
	else {
		memset(str,0x20,256);
		memmove(str,title,strsz);
	}
	
	str[fieldsz]=0x0;

	fwrite(str,1,strlen(str),file);
}

void debug_info_table_int(FILE *file,int value,int fieldsz)
{
	char		str[256];
	
	sprintf(str,"%d",value);
	debug_info_table_str(file,str,fieldsz);
}

void debug_info_table_tag(FILE *file,unsigned long tag,int fieldsz)
{
	char		str[256];
	
	memmove(str,&tag,4);
	str[4]=0x0;
	
	debug_info_table_str(file,str,fieldsz);
}

/* =======================================================

      Debug Dump
      
======================================================= */

void debug_dump(void)
{
	int					n,k,t,nvertex,npoly;
	char				str[256],path[1024];
	char				timer_type_str[][32]={"Single","Repeat","Chain","Dispose"};
	obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;
	proj_setup_type		*proj_setup;
	effect_type			*effect;
	model_type			*mdl;
	script_type			*script,*script_parent;
	timer_type			*timer;
	FILE				*file;

		// start output file

	file_paths_app_data(&file_path_setup,path,"Debug","info","txt");
	
	file=fopen(path,"w");
	if (file==NULL) return;
	
		// app
		
	debug_dump_header(file,"App");

		// engine

	debug_dump_header(file,"Engine");

	debug_dump_info_str(file,"Engine",dim3_version);
	debug_dump_info_str(file,"Project",iface.project.name);

	debug_dump_info_str(file,"GPU",render_info.name);
	sprintf(str,"%d,%d @ %d",render_info.desktop.wid,render_info.desktop.high,render_info.monitor_refresh_rate);
	debug_dump_info_str(file,"Screen",str);
	debug_dump_info_int(file,"Max Texture Size",render_info.texture_max_size);
	
	debug_dump_info_enable(file,"FBO",gl_check_frame_buffer_ok());
	debug_dump_info_enable(file,"NPOT Textures",gl_check_npot_textures_ok());
	debug_dump_info_enable(file,"FSAA",gl_check_fsaa_ok());

	debug_dump_info_str(file,"Extensions",render_info.ext_string);

#ifdef D3_OS_WINDOWS
	debug_dump_info_str(file,"WGL Extensions",(char*)wglGetExtensionsStringARB(wglGetCurrentDC()));
#endif

		// timing

	debug_dump_header(file,"Timing");

	debug_dump_info_int(file,"Raw Tick",game_time_get_raw());
	debug_dump_info_int(file,"Game Tick",game_time_get());

	debug_dump_info_int(file,"Server Run Tick (Game)",server.time.run_tick);
	debug_dump_info_int(file,"Server Network Update Tick (Game)",server.time.network_update_tick);
	debug_dump_info_int(file,"Server Network Group Synch Tick (Game)",server.time.network_group_synch_tick);

	debug_dump_info_int(file,"View Input Tick (Raw)",view.time.input_tick);
	debug_dump_info_int(file,"View Draw Tick (Raw)",view.time.draw_tick);
	debug_dump_info_int(file,"View Run Tick (Game)",view.time.run_tick);
	debug_dump_info_int(file,"View Frame Draw Time",view.time.draw_time);

	debug_dump_info_int(file,"Map Start Tick (Game)",map.start_game_tick);
	debug_dump_info_int(file,"JS Timer Tick (Game)",js.timer_tick);

		// map

	debug_dump_header(file,"Map");

	debug_dump_info_str(file,"Name",map.info.name);
	debug_dump_info_str(file,"Author",map.info.author);

	debug_dump_info_int(file,"Mesh Count",map.mesh.nmesh);
	debug_dump_info_int(file,"Liquid Count",map.liquid.nliquid);
	debug_dump_info_int(file,"Spot Count",map.nspot);
	debug_dump_info_int(file,"Scenery Count",map.nscenery);
	debug_dump_info_int(file,"Node Count",map.nnode);

		// objects

	debug_dump_header(file,"Objects");

	debug_info_table_str(file,"Index",6);
	debug_info_table_str(file,"Name",25);
	debug_info_table_str(file,"Type",15);
	debug_info_table_str(file,"Model",25);
	debug_info_table_str(file,"Script",30);
	debug_info_table_str(file,"Binding",10);
	debug_info_return(file);
	debug_info_table_str(file,"-----",6);
	debug_info_table_str(file,"------------------------",25);
	debug_info_table_str(file,"--------------",15);
	debug_info_table_str(file,"------------------------",25);
	debug_info_table_str(file,"-----------------------------",30);
	debug_info_table_str(file,"---------",10);
	debug_info_return(file);

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		debug_info_table_int(file,obj->idx,6);
		debug_info_table_str(file,obj->name,25);
		debug_info_table_str(file,object_type_str[obj->type],15);
		
		if (obj->draw.model_idx==-1) {
			debug_info_table_str(file,"*",25);
		}
		else {
			debug_info_table_str(file,server.model_list.models[obj->draw.model_idx]->name,25);
		}
		
		if (!obj->scenery.on) {
			debug_info_table_str(file,js.script_list.scripts[obj->script_idx]->name,30);
		}
		else {
			debug_info_table_str(file,"*",30);
		}
		
		debug_info_table_str(file,bind_type_str[obj->bind],10);
		debug_info_return(file);
	}

		// weapons

	debug_dump_header(file,"Weapons");

	debug_info_table_str(file,"Index",6);
	debug_info_table_str(file,"Object",25);
	debug_info_table_str(file,"Name",25);
	debug_info_table_str(file,"Model",25);
	debug_info_table_str(file,"Script",30);
	debug_info_return(file);
	debug_info_table_str(file,"-----",6);
	debug_info_table_str(file,"------------------------",25);
	debug_info_table_str(file,"------------------------",25);
	debug_info_table_str(file,"------------------------",25);
	debug_info_table_str(file,"-----------------------------",30);
	debug_info_return(file);

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (object_count_weapons(obj)==0) continue;

		for (k=0;k!=max_weap_list;k++) {
			weap=obj->weap_list.weaps[k];
			if (weap==NULL) continue;

			debug_info_table_int(file,weap->idx,6);
			debug_info_table_str(file,obj->name,25);
			debug_info_table_str(file,weap->name,25);
			if (weap->draw.model_idx==-1) {
				debug_info_table_str(file,"*",25);
			}
			else {
				debug_info_table_str(file,server.model_list.models[weap->draw.model_idx]->name,25);
			}
			debug_info_table_str(file,js.script_list.scripts[weap->script_idx]->name,30);
			debug_info_return(file);
		}
	}

		// projectile setups

	debug_dump_header(file,"Projectile Setups");

	debug_info_table_str(file,"Index",6);
	debug_info_table_str(file,"Object",25);
	debug_info_table_str(file,"Weapon",25);
	debug_info_table_str(file,"Name",25);
	debug_info_table_str(file,"Model",25);
	debug_info_table_str(file,"Script",30);
	debug_info_return(file);
	debug_info_table_str(file,"-----",6);
	debug_info_table_str(file,"------------------------",25);
	debug_info_table_str(file,"------------------------",25);
	debug_info_table_str(file,"------------------------",25);
	debug_info_table_str(file,"------------------------",25);
	debug_info_table_str(file,"-----------------------------",30);
	debug_info_return(file);

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (object_count_weapons(obj)==0) continue;

		for (k=0;k!=max_weap_list;k++) {
			weap=obj->weap_list.weaps[k];
			if (weap==NULL) continue;

			if (weapon_count_projectile_setups(weap)==0) continue;

			for (t=0;t!=max_proj_setup_list;t++) {
				proj_setup=weap->proj_setup_list.proj_setups[t];
				if (proj_setup==NULL) continue;

				debug_info_table_int(file,proj_setup->idx,6);
				debug_info_table_str(file,obj->name,25);
				debug_info_table_str(file,weap->name,25);
				debug_info_table_str(file,proj_setup->name,25);
				if (proj_setup->draw.model_idx==-1) {
					debug_info_table_str(file,"*",25);
				}
				else {
					debug_info_table_str(file,server.model_list.models[proj_setup->draw.model_idx]->name,25);
				}
				debug_info_table_str(file,js.script_list.scripts[proj_setup->script_idx]->name,30);
				debug_info_return(file);
			}
		}
	}

		// projectiles

	debug_dump_header(file,"Projectiles");

	debug_info_table_str(file,"Name",25);
	debug_info_table_str(file,"Object",25);
	debug_info_table_str(file,"Weapon",25);
	debug_info_return(file);
	debug_info_table_str(file,"------------------------",25);
	debug_info_table_str(file,"------------------------",25);
	debug_info_table_str(file,"------------------------",25);
	debug_info_return(file);
	
	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (!proj->on) continue;

		obj=server.obj_list.objs[proj->obj_idx];
		weap=obj->weap_list.weaps[proj->weap_idx];
		proj_setup=weap->proj_setup_list.proj_setups[proj->proj_setup_idx];
		
		debug_info_table_str(file,proj_setup->name,25);
		debug_info_table_str(file,obj->name,25);
		debug_info_table_str(file,weap->name,25);
		debug_info_return(file);
	}

		// effects

	debug_dump_header(file,"Effects");
	
	debug_info_table_str(file,"Type",20);
	debug_info_table_str(file,"Life Tick",10);
	debug_info_return(file);
	debug_info_table_str(file,"------------------",20);
	debug_info_table_str(file,"---------",10);
	debug_info_return(file);
	
	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (!effect->on) continue;

		debug_info_table_str(file,effect_type_str[effect->effecttype],20);
		debug_info_table_int(file,effect->life_tick,10);
		debug_info_return(file);
	}
	
		// models

	debug_dump_header(file,"Models");
	
	debug_info_table_str(file,"Name",25);
	debug_info_table_str(file,"Vertexes",10);
	debug_info_table_str(file,"Polygons",10);
	debug_info_table_str(file,"Ref Count",10);
	debug_info_return(file);
	debug_info_table_str(file,"-----------------------",25);
	debug_info_table_str(file,"---------",10);
	debug_info_table_str(file,"---------",10);
	debug_info_table_str(file,"---------",10);
	debug_info_return(file);

	for (n=0;n!=max_model_list;n++) {
		mdl=server.model_list.models[n];
		if (mdl==NULL) continue;

		nvertex=npoly=0;

		for (k=0;k!=mdl->nmesh;k++) {
			nvertex+=mdl->meshes[k].nvertex;
			npoly+=mdl->meshes[k].npoly;
		}

		debug_info_table_str(file,mdl->name,25);
		debug_info_table_int(file,nvertex,10);
		debug_info_table_int(file,npoly,10);
		debug_info_table_int(file,mdl->load.reference_count,10);
		debug_info_return(file);
	}

		// scripts
		
	debug_dump_header(file,"Scripts");
	
	debug_info_table_str(file,"Name",35);
	debug_info_table_str(file,"Parent",35);
	debug_info_table_str(file,"Attached Events",35);
	debug_info_return(file);
	debug_info_table_str(file,"----------------------------------",35);
	debug_info_table_str(file,"----------------------------------",35);
	debug_info_table_str(file,"----------------------------------",35);
	debug_info_return(file);
	
	for (n=0;n!=max_script_list;n++) {
		script=js.script_list.scripts[n];
		if (script==NULL) continue;

		debug_info_table_str(file,script->name,35);
		
		if (script->parent_idx==-1) {
			debug_info_table_str(file,"*",35);
		}
		else {
			script_parent=js.script_list.scripts[script->parent_idx];
			debug_info_table_str(file,script_parent->name,35);
		}

		debug_info_table_str(file,script->name,35);
		
		for (k=0;k!=event_main_id_count;k++) {
			if (script->event_attach_list.func[k]!=NULL) {
				script_get_define_for_event((event_main_id_start+k),str);
				fwrite(str,1,strlen(str),file);
				fwrite(" ",1,1,file);
			}
		}
		
		debug_info_return(file);
	}

		// timers

	debug_dump_header(file,"Timers");
	
	debug_info_table_str(file,"Script",35);
	debug_info_table_str(file,"Count",10);
	debug_info_table_str(file,"Frequency",10);
	debug_info_table_str(file,"Type",10);
	debug_info_return(file);
	debug_info_table_str(file,"----------------------------------",35);
	debug_info_table_str(file,"---------",10);
	debug_info_table_str(file,"---------",10);
	debug_info_table_str(file,"---------",10);
	debug_info_return(file);
	
	for (n=0;n!=max_timer_list;n++) {
		timer=js.timer_list.timers[n];
		if (timer==NULL) continue;

		script=js.script_list.scripts[timer->script_idx];
		debug_info_table_str(file,script->name,35);
		debug_info_table_int(file,timer->count,10);
		debug_info_table_int(file,timer->freq,10);
		debug_info_table_str(file,timer_type_str[timer->mode==timer_mode_repeat],10);
		debug_info_return(file);
	}

		// finish

	fclose(file);

		// console message

	snprintf(str,256,"Debug Info: %s",path);
	str[255]=0x0;

	console_add_system(str);
}

/* =======================================================

      Debug ScreenShot
      
======================================================= */

void debug_screenshot(void)
{
	char				path[1024],file_name[256],str[256];
	struct tm			*tm;
	time_t				curtime;
	
	curtime=time(NULL);
	tm=localtime(&curtime);

	sprintf(file_name,"%.4d%.2d%.2d_%.2d%.2d%.2d",(tm->tm_year+1900),(tm->tm_mon+1),tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	file_paths_app_data(&file_path_setup,path,"Screenshots",file_name,"png");
	
	if (!gl_screen_shot(0,0,view.screen.x_sz,view.screen.y_sz,FALSE,path)) {
		console_add_system("Unable to save screenshot");
		return;
	}

	snprintf(str,256,"Screenshot: %s",path);
	str[255]=0x0;

	console_add_system(str);
}

/* =======================================================

      Debug Input
      
======================================================= */

void debug_input(void)
{
		// screenshot always works
		
	if (input_action_get_state_single(nc_debug_screenshot)) {
		debug_screenshot();
		return;
	}
	
		// others require debug mode to be on
		
	if (!iface.setup.game_debug) return;
	
	if (input_action_get_state_single(nc_debug_status)) {
		debug_dump();
		return;
	}
}

/* =======================================================

      Debug Console Map Change
      
======================================================= */

bool debug_change_map(char *name)
{
	int							n;
	char						str[256];
	bool						map_ok;
	file_path_directory_type	*fpd;

		// verify the map
		
	map_ok=FALSE;
	
	fpd=file_paths_read_directory_data(&file_path_setup,"Maps","xml");
	
	for (n=0;n!=fpd->nfile;n++) {
		if (strcasecmp(fpd->files[n].file_name,name)==0) {
			map_ok=TRUE;
			break;
		}
	}

	file_paths_close_directory(fpd);
	
		// map exists?
		
	if (!map_ok) {
		sprintf(str,"map '%s' does not exist",name);
		console_add_error(str);
		return(FALSE);
	}
	
		// set map
		
	strncpy(map.info.name,name,name_str_len);
	strcpy(map.info.player_start_name,"Start");
	
	server.map_change.on=TRUE;
	server.map_change.skip_media=TRUE;
	server.map_change.player_restart=FALSE;
	
	return(TRUE);
}
