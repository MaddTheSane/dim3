/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Game Save/Load

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
#include "objects.h"
#include "scripts.h"

typedef struct		{
						int					tick,skill,option_flags,
											simple_save_idx,checkpoint_spot_idx,
											player_obj_idx;
						char				version[16],map_name[name_str_len];
					} file_save_header;					

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern js_type				js;
extern setup_type			setup;
extern file_path_setup_type	file_path_setup;


bool						game_file_has_suspended_save;
unsigned long				game_file_pos;
char						game_file_last_save_name[256];
unsigned char				*game_file_data;

extern void view_capture_draw(char *path);
extern void rain_reset(void);

/* =======================================================

      Initialize and Last Saves
      
======================================================= */

void game_file_initialize(void)
{
	game_file_last_save_name[0]=0x0;
	game_file_has_suspended_save=FALSE;
}

/* =======================================================

      Chunks
      
======================================================= */

bool game_file_start(void)
{
	int				n,k,t,mem_sz;
	obj_type		*obj;
	weapon_type		*weap;
	proj_setup_type	*proj_setup;

		// start with extra

	mem_sz=(64*1024);

		// add in objects, weapons, proj setups

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		mem_sz+=(sizeof(obj_type)+(2*sizeof(int)));
		mem_sz+=1024;		// script json

		for (k=0;k!=max_weap_list;k++) {
			weap=obj->weap_list.weaps[k];
			if (weap==NULL) continue;

			mem_sz+=(sizeof(weapon_type)+(2*sizeof(int)));
			mem_sz+=1024;

			for (t=0;t!=max_proj_setup_list;t++) {
				proj_setup=weap->proj_setup_list.proj_setups[t];
				if (proj_setup==NULL) continue;
				
				mem_sz+=(sizeof(proj_setup_type)+sizeof(int));
				mem_sz+=1024;
			}
		}
	}

		// projectiles, effects, spots

	mem_sz+=(projectile_count_list()*(sizeof(proj_type)+sizeof(int)))+sizeof(int);
	mem_sz+=(effect_count_list()*(sizeof(effect_type)+sizeof(int)))+sizeof(int);
	mem_sz+=(map.nspot*(sizeof(spot_type)+sizeof(int)))+sizeof(int);
	
		// HUD

	mem_sz+=sizeof(iface_bitmap_type);
	mem_sz+=sizeof(iface_text_type);
	mem_sz+=sizeof(iface_bar_type);
	mem_sz+=sizeof(iface_radar_type);
	
		// map changes
		
	mem_sz+=sizeof(map_ambient_type);					
	mem_sz+=sizeof(map_fog_type);
	mem_sz+=(map.group.ngroup*sizeof(group_run_type))+sizeof(int);
	mem_sz+=(map.movement.nmovement*sizeof(movement_run_type))+sizeof(int);

		// timers and script data

	mem_sz+=(timers_count_list()*(sizeof(timer_type)+sizeof(int)))+sizeof(int);
	mem_sz+=(script_global_count_list()*(sizeof(global_type)+sizeof(int)))+sizeof(int);

		// create the memory

	game_file_data=(unsigned char*)malloc(mem_sz);
	if (game_file_data==NULL) return(FALSE);

	game_file_pos=0;

	return(TRUE);
}

void game_file_end(void)
{
	free(game_file_data);
}

bool game_file_add_chunk(void *data,int count,int sz)
{
	memmove((game_file_data+game_file_pos),&count,sizeof(int));
	game_file_pos+=sizeof(int);

	memmove((game_file_data+game_file_pos),&sz,sizeof(int));
	game_file_pos+=sizeof(int);
	
	memmove((game_file_data+game_file_pos),data,sz);
	game_file_pos+=sz;
	
	return(TRUE);
}

char* game_file_get_chunk(void *data)
{
	int			count,sz;
	
	memmove(&count,(game_file_data+game_file_pos),sizeof(int));
	game_file_pos+=sizeof(int);

	memmove(&sz,(game_file_data+game_file_pos),sizeof(int));
	game_file_pos+=sizeof(int);
	
	memmove(data,(game_file_data+game_file_pos),sz);
	game_file_pos+=sz;
	
	return(data);
}

unsigned char* game_file_replace_chunk(void)
{
	int				count,sz;
	unsigned char	*data;
	
	memmove(&count,(game_file_data+game_file_pos),sizeof(int));
	game_file_pos+=sizeof(int);

	memmove(&sz,(game_file_data+game_file_pos),sizeof(int));
	game_file_pos+=sizeof(int);
	
	data=malloc(sz);
	if (data==NULL) return(NULL);
	
	memmove(data,(game_file_data+game_file_pos),sz);
	game_file_pos+=sz;
	
	return(data);
}

/* =======================================================

      Compression and Expansion Utilities
      
======================================================= */

bool game_file_compress_save(char *path,char *err_str)
{
	unsigned long   compress_sz;
	ptr				compress_data;
	FILE			*file;
	
		// compress it

	compress_data=zip_compress(game_file_data,game_file_pos,&compress_sz,err_str);
	if (compress_data==NULL) return(FALSE);
	
		// save file
		
	file=fopen(path,"wb");
	if (file==NULL) {
		strcpy(err_str,"Could not create file");
		free(compress_data);
		return(FALSE);
	}
		
	fwrite(&game_file_pos,1,sizeof(unsigned long),file);
	fwrite(&compress_sz,1,sizeof(unsigned long),file);
	fwrite(compress_data,1,compress_sz,file);
	fclose(file);
	
#ifndef D3_OS_WINDOWS
	truncate(path,((sizeof(unsigned long)*2)+compress_sz));
#endif

	free(compress_data);
	
	return(TRUE);
}

bool game_file_expand_load(char *path,char *err_str)
{
	unsigned long   file_sz,game_file_sz,compress_sz;
	ptr				compress_data;
	FILE			*file;
	struct stat		sb;
	
		// get file size

	if (stat(path,&sb)!=0) {
		strcpy(err_str,"File does not exist");
		return(FALSE);
	}
	file_sz=sb.st_size;
	
	compress_data=malloc(file_sz);
	if (compress_data==NULL) {
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}
	
		// get compressed data
		
	file=fopen(path,"rb");
	if (file==NULL) {
		strcpy(err_str,"Could not open file");
		free(compress_data);
		return(FALSE);
	}
	
	fread(&game_file_sz,1,sizeof(unsigned long),file);
	fread(&compress_sz,1,sizeof(unsigned long),file);
	fread(compress_data,1,compress_sz,file);
	fclose(file);
	
		// decompress file

	game_file_data=zip_expand(compress_data,compress_sz,&game_file_sz,err_str);
	if (game_file_data==NULL) {
		free(compress_data);
		return(FALSE);
	}
	
	free(compress_data);
	
	return(TRUE);
}

/* =======================================================

      Get Save File Path
      
======================================================= */

void game_file_create_name(int tick,char *file_name)
{
	int					sec;
	char				elapse_str[32],time_str[32];
	struct tm			*tm;
	time_t				curtime;

	curtime=time(NULL);
	tm=localtime(&curtime);
	sprintf(time_str,"%.4d%.2d%.2d%.2d%.2d",(tm->tm_year+1900),(tm->tm_mon+1),tm->tm_mday,tm->tm_hour,tm->tm_min);
		
	sec=(game_time_get()-map.start_game_tick)/1000;
	if (sec>(99*60)) sec=99*60;
	sprintf(elapse_str,"%.2d%.2d",(sec/60),(sec%60));
		
	sprintf(file_name,"%s_%s_%s",time_str,elapse_str,map.info.name);
}

/* =======================================================

      Save Game
      
======================================================= */

bool game_file_save(bool no_progress,bool suspend_save,char *err_str)
{
	int					n,k,t,count,tick;
	char				path[1024],file_name[256];
	bool				ok;
	file_save_header	head;
	obj_type			*obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	proj_type			*proj;
	effect_type			*effect;
	timer_type			*timer;
	global_type			*global;

	if (!no_progress) progress_initialize(NULL);

		// get saved data file names
		
	tick=game_time_get();

	game_file_create_name(tick,file_name);

	if (!suspend_save) {
		fprintf(stdout,"Saving \"%s\"\n",file_name);
	}
	else {
		fprintf(stdout,"Saving suspended game\n");
	}
	
		// save screen
		
	if (!suspend_save) {
		file_paths_app_data(&file_path_setup,path,"Saved Games",file_name,"png");
		view_capture_draw(path);
	}

		// start chunks

	if (!game_file_start()) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

		// header

	head.tick=tick;
	head.skill=server.skill;
	head.option_flags=server.option_flags;
	head.simple_save_idx=server.simple_save_idx;
	head.checkpoint_spot_idx=server.checkpoint_spot_idx;
	head.player_obj_idx=server.player_obj_idx;
	
	strcpy(head.version,dim3_version);
	strcpy(head.map_name,map.info.name);
		
	game_file_add_chunk(&head,1,sizeof(file_save_header));
	
		// view & server state
		
	if (!no_progress) progress_update();
		
	game_file_add_chunk(&view.time,1,sizeof(view_time_type));
	game_file_add_chunk(&camera,1,sizeof(camera_type));

	if (!no_progress) progress_update();
	
	game_file_add_chunk(&server.time,1,sizeof(server_time_type));
	game_file_add_chunk(&js.timer_tick,1,sizeof(int));
	
		// objects, weapons, and projectile setups

	count=object_count_list();
	game_file_add_chunk(&count,1,sizeof(int));

	for (n=0;n!=max_obj_list;n++) {
		if (!no_progress) progress_update();

		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		game_file_add_chunk(&n,1,sizeof(int));
		game_file_add_chunk(obj,1,sizeof(obj_type));

		count=object_count_weapons(obj);
		game_file_add_chunk(&count,1,sizeof(int));

		for (k=0;k!=max_weap_list;k++) {
			weap=obj->weap_list.weaps[k];
			if (weap==NULL) continue;

			game_file_add_chunk(&k,1,sizeof(int));
			game_file_add_chunk(weap,1,sizeof(weapon_type));

			count=weapon_count_projectile_setups(weap);
			game_file_add_chunk(&count,1,sizeof(int));

			for (t=0;t!=max_proj_setup_list;t++) {
				proj_setup=weap->proj_setup_list.proj_setups[t];
				if (proj_setup==NULL) continue;

				game_file_add_chunk(&t,1,sizeof(int));
				game_file_add_chunk(proj_setup,1,sizeof(proj_setup_type));
			}
		}
	}

		// projectiles and effects

	count=projectile_count_list();
	game_file_add_chunk(&count,1,sizeof(int));

	for (n=0;n!=max_proj_list;n++) {
		if (!no_progress) progress_update();

		proj=server.proj_list.projs[n];
		if (!proj->on) continue;

		game_file_add_chunk(&n,1,sizeof(int));
		game_file_add_chunk(proj,1,sizeof(proj_type));
	}

	count=effect_count_list();
	game_file_add_chunk(&count,1,sizeof(int));

	for (n=0;n!=max_effect_list;n++) {
		if (!no_progress) progress_update();

		effect=server.effect_list.effects[n];
		if (effect==NULL) continue;
		if (!effect->on) continue;

		game_file_add_chunk(&n,1,sizeof(int));
		game_file_add_chunk(effect,1,sizeof(effect_type));
	}

		// spots (mostly for checkpoint data)

	game_file_add_chunk(map.spots,map.nspot,sizeof(spot_type));
	
		// HUD

	if (!no_progress) progress_update();
	
	game_file_add_chunk(iface.bitmap_list.bitmaps,iface.bitmap_list.nbitmap,sizeof(iface_bitmap_type));
	game_file_add_chunk(iface.text_list.texts,iface.text_list.ntext,sizeof(iface_text_type));
	game_file_add_chunk(iface.bar_list.bars,iface.bar_list.nbar,sizeof(iface_bar_type));
	game_file_add_chunk(&iface.radar,1,sizeof(iface_radar_type));
	
		// map changes
		
	if (!no_progress) progress_update();

	game_file_add_chunk(&map.ambient,1,sizeof(map_ambient_type));					
	game_file_add_chunk(&map.fog,1,sizeof(map_fog_type));

	if (!no_progress) progress_update();

	game_file_add_chunk(&map.group.ngroup,1,sizeof(int));

	for (n=0;n!=map.group.ngroup;n++) {
		game_file_add_chunk(&map.group.groups[n].run,1,sizeof(group_run_type));
	}

	if (!no_progress) progress_update();

	game_file_add_chunk(&map.movement.nmovement,1,sizeof(int));

	for (n=0;n!=map.movement.nmovement;n++) {
		game_file_add_chunk(&map.movement.movements[n].run,1,sizeof(movement_run_type));
	}
	
		// script states
		
	if (!no_progress) progress_update();

	if (!script_state_save((server.checkpoint_spot_idx!=-1),err_str)) {
		free(game_file_data);
		if (!no_progress) progress_shutdown();
		return(FALSE);
	}

		// timers and script data

	if (!no_progress) progress_update();

	count=timers_count_list();
	game_file_add_chunk(&count,1,sizeof(int));

	for (n=0;n!=max_timer_list;n++) {
		timer=js.timer_list.timers[n];
		if (timer==NULL) continue;

		game_file_add_chunk(&n,1,sizeof(int));
		game_file_add_chunk(timer,1,sizeof(timer_type));
	}

	if (!no_progress) progress_update();

	count=script_global_count_list();
	game_file_add_chunk(&count,1,sizeof(int));

	for (n=0;n!=max_global_list;n++) {
		global=js.global_list.globals[n];
		if (global==NULL) continue;

		game_file_add_chunk(&n,1,sizeof(int));
		game_file_add_chunk(global,1,sizeof(global_type));
	}

		// compress and save
		
	if (!no_progress) progress_update();

	if (!suspend_save) {
		file_paths_app_data(&file_path_setup,path,"Saved Games",file_name,"sav");
	}
	else {
		file_paths_app_data(&file_path_setup,path,"Saved Games","suspend","ssv");
	}

	ok=game_file_compress_save(path,err_str);
	
	if (!no_progress) progress_update();
	
	game_file_end();
	
		// remember last map
		
	if (!suspend_save) strcpy(game_file_last_save_name,strrchr(path,'/'));
	
		// finished
		
	if (!no_progress) progress_shutdown();

		// misc initialization

	view_clear_fps();
	rain_reset();
  
    return(ok);
}

/* =======================================================

      Load Game
      
======================================================= */

bool game_file_load(char *file_name,bool resume_load,char *err_str)
{
	int					n,k,t,count,weap_count,proj_setup_count,idx;
	bool				ok,map_change;
	char				*c,path[1024],fname[256];
	file_save_header	head;
	obj_type			*obj,*player_obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	spot_type			*spot;

		// load and expand
		
	if (!resume_load) {
		strcpy(fname,file_name);
		c=strrchr(fname,'.');
		if (c!=NULL) *c=0x0;			// remove any extensions
	
		file_paths_app_data(&file_path_setup,path,"Saved Games",fname,"sav");
	}
	else {
		file_paths_app_data(&file_path_setup,path,"Saved Games","suspend","ssv");
	}

	if (!game_file_expand_load(path,err_str)) return(FALSE);
	
	game_file_pos=0;

		// get header

	game_file_get_chunk(&head);

		// check version
		
	if (strcmp(head.version,dim3_version)!=0) {
		sprintf(err_str,"This saved game file is from a different version");
		free(game_file_data);
		return(FALSE);
	}

		// if game isn't running, then start
		// game and set map change,
		// otherwise just determine if map
		// has changed
		
	if (!server.game_open) {

		scripts_lock_events();
		ok=game_start(TRUE,head.skill,head.option_flags,head.simple_save_idx,err_str);
		scripts_unlock_events();
		
		if (!ok) {
			free(game_file_data);
			return(FALSE);
		}
		
		server.map_open=FALSE;
		map_change=TRUE;
	}
	else {
		map_change=((!server.map_open) || (strcmp(head.map_name,map.info.name)!=0));
	}

		// additional header stuff
		
	strcpy(map.info.name,head.map_name);
	map.info.player_start_name[0]=0x0;

	server.skill=head.skill;
	server.simple_save_idx=head.simple_save_idx;
	server.player_obj_idx=head.player_obj_idx;
		
		// reload map

	if (map_change) {

		if (server.map_open) map_end();

		scripts_lock_events();
		ok=map_start(TRUE,TRUE,err_str);
		scripts_unlock_events();
		
		if (!ok) {
			free(game_file_data);
			return(FALSE);
		}
	}

		// restore printout

	if (!resume_load) {
		fprintf(stdout,"Restoring \"%s\"\n",fname);
	}
	else {
		fprintf(stdout,"Restoring suspended game\n");
	}

		// start progress

	if (!resume_load) progress_initialize(NULL);

		// view & server state
		
	if (!resume_load) progress_update();
					
	game_file_get_chunk(&view.time);
	game_file_get_chunk(&camera);
	
	if (!resume_load) progress_update();

	game_file_get_chunk(&server.time);
	game_file_get_chunk(&js.timer_tick);

		// free all the model meshes
		// and rebuild

	view_dim3rtl_map_model_mesh_stop();

		// objects, weapons, and projectile setups
	
	if (!resume_load) progress_update();

	object_dispose_all();
	object_free_list();
	object_initialize_list();

	game_file_get_chunk(&count);

	for (n=0;n!=count;n++) {
		if (!resume_load) progress_update();

		game_file_get_chunk(&idx);

		server.obj_list.objs[idx]=(obj_type*)malloc(sizeof(obj_type));
		if (server.obj_list.objs[idx]==NULL) {
			free(game_file_data);
			if (!resume_load) progress_shutdown();
			return(FALSE);
		}

		obj=server.obj_list.objs[idx];

		game_file_get_chunk(obj);
		view_dim3rtl_model_clear_draw(&obj->draw);
		model_rebuild_draw_memory(&obj->draw);

			// rebuild object script

		scripts_lock_events();
		ok=object_start_script(obj,TRUE,err_str);
		scripts_unlock_events();

		if (!ok) {
			free(game_file_data);
			if (!resume_load) progress_shutdown();
			return(FALSE);
		}

			// object weapons

		game_file_get_chunk(&weap_count);

		for (k=0;k!=weap_count;k++) {
			
			game_file_get_chunk(&idx);
			obj->weap_list.weaps[idx]=(weapon_type*)malloc(sizeof(weapon_type));
			if (obj->weap_list.weaps[idx]==NULL) {
				free(game_file_data);
				if (!resume_load) progress_shutdown();
				return(FALSE);
			}

			weap=obj->weap_list.weaps[idx];
			
			game_file_get_chunk(weap);
			view_dim3rtl_model_clear_draw(&weap->draw);
			model_rebuild_draw_memory(&weap->draw);

				// rebuild weapon script

			scripts_lock_events();
			ok=weapon_start_script(obj,weap,TRUE,err_str);
			scripts_unlock_events();
			
			if (!ok) {
				free(game_file_data);
				if (!resume_load) progress_shutdown();
				return(FALSE);
			}

				// object weapon projectile setups

			game_file_get_chunk(&proj_setup_count);

			for (t=0;t!=proj_setup_count;t++) {
				
				game_file_get_chunk(&idx);
				weap->proj_setup_list.proj_setups[idx]=(proj_setup_type*)malloc(sizeof(proj_setup_type));
				if (weap->proj_setup_list.proj_setups[idx]==NULL) {
					free(game_file_data);
					if (!resume_load) progress_shutdown();
					return(FALSE);
				}

				proj_setup=weap->proj_setup_list.proj_setups[idx];

				game_file_get_chunk(proj_setup);
				view_dim3rtl_model_clear_draw(&proj_setup->draw);
				model_rebuild_draw_memory(&proj_setup->draw);

					// rebuild projectile setup script

				scripts_lock_events();
				ok=proj_setup_start_script(obj,weap,proj_setup,TRUE,err_str);
				scripts_unlock_events();

				if (!ok) {
					free(game_file_data);
					if (!resume_load) progress_shutdown();
					return(FALSE);
				}
			}
		}
	}

		// projectiles, effects and decals
	
	projectile_free_list();
	projectile_initialize_list();

	game_file_get_chunk(&count);

	for (n=0;n!=count;n++) {
		game_file_get_chunk(&idx);
		server.proj_list.projs[idx]=(proj_type*)malloc(sizeof(proj_type));
		if (server.proj_list.projs[idx]==NULL) {
			free(game_file_data);
			if (!resume_load) progress_shutdown();
			return(FALSE);
		}

		if (!resume_load) progress_update();

		game_file_get_chunk(server.proj_list.projs[idx]);
		view_dim3rtl_model_clear_draw(&server.proj_list.projs[idx]->draw);
		model_rebuild_draw_memory(&server.proj_list.projs[idx]->draw);
	}

	effect_free_list();
	effect_initialize_list();

	game_file_get_chunk(&count);

	for (n=0;n!=count;n++) {
		game_file_get_chunk(&idx);
		server.effect_list.effects[idx]=(effect_type*)malloc(sizeof(effect_type));
		if (server.effect_list.effects[idx]==NULL) {
			free(game_file_data);
			if (!resume_load) progress_shutdown();
			return(FALSE);
		}

		if (!resume_load) progress_update();

		game_file_get_chunk(server.effect_list.effects[idx]);
		server.effect_list.effects[idx]->rtl_mesh_id=-1;
		view_dim3rtl_effect_mesh_setup(server.effect_list.effects[idx]);
	}

		// reload all the model
		// meshes into the rtl scene

	view_dim3rtl_map_model_mesh_start();

		// spots (mostly for checkpoint data)

	game_file_get_chunk(map.spots);

		// HUD

	if (!resume_load) progress_update();

	game_file_get_chunk(iface.bitmap_list.bitmaps);
	game_file_get_chunk(iface.text_list.texts);
	game_file_get_chunk(iface.bar_list.bars);
	game_file_get_chunk(&iface.radar);
	
		// map changes
		
	if (!resume_load) progress_update();

	game_file_get_chunk(&map.ambient);					
	game_file_get_chunk(&map.fog);

	if (!resume_load) progress_update();

	game_file_get_chunk(&count);

	for (n=0;n!=count;n++) {
		game_file_get_chunk(&map.group.groups[n].run);
	}

	if (!resume_load) progress_update();

	game_file_get_chunk(&count);

	for (n=0;n!=count;n++) {
		game_file_get_chunk(&map.movement.movements[n].run);
	}

		// if this is a checkpoint
		// save, we need to reset player and motion

	if (head.checkpoint_spot_idx!=-1) {
		spot=&map.spots[head.checkpoint_spot_idx];
		player_obj=server.obj_list.objs[server.player_obj_idx];

		memmove(&player_obj->pnt,&spot->pnt,sizeof(d3pnt));
		memmove(&player_obj->ang,&spot->ang,sizeof(d3ang));
		object_stop(player_obj);

		player_obj->last_spawn_spot_idx=head.checkpoint_spot_idx;
	}

		// script objects
		
	if (!resume_load) progress_update();

	if (!script_state_load((head.checkpoint_spot_idx!=-1),err_str)) {
		free(game_file_data);
		if (!resume_load) progress_shutdown();
		return(FALSE);
	}

		// timers and script data

	if (!resume_load) progress_update();

	timers_free_list();
	timers_initialize_list();

	game_file_get_chunk(&count);

	for (n=0;n!=count;n++) {
		game_file_get_chunk(&idx);
		js.timer_list.timers[idx]=(timer_type*)malloc(sizeof(timer_type));
		if (js.timer_list.timers[idx]==NULL) {
			free(game_file_data);
			if (!resume_load) progress_shutdown();
			return(FALSE);
		}

		game_file_get_chunk(js.timer_list.timers[idx]);
	}

	if (!resume_load) progress_update();

	script_global_free_list();
	script_global_initialize_list();

	game_file_get_chunk(&count);

	for (n=0;n!=count;n++) {
		game_file_get_chunk(&idx);
		js.global_list.globals[idx]=(global_type*)malloc(sizeof(global_type));
		if (js.global_list.globals[idx]==NULL) {
			free(game_file_data);
			if (!resume_load) progress_shutdown();
			return(FALSE);
		}

		game_file_get_chunk(js.global_list.globals[idx]);
	}

		// reset models and cached images

		// this resets the image cache (as indexes could
		// have changed) by calling load() again, since images
		// are shared, this will just return the index of the
		// already loaded image, or in case the save files are different,
		// a new image

	if (!resume_load) progress_update();

	view_images_cached_load();
	
		// fix the script state
		// and reset indexes on timers
		
	if (!resume_load) progress_update();

	timers_fix_script_indexes();

		// finish
		
	if (!resume_load) progress_update();

	free(game_file_data);
	if (!resume_load) progress_shutdown();

		// fix some necessary functions

	map.rain.reset=TRUE;
	view_fade_cancel();

        // this is now the current saved
		// game

	if (!resume_load) strcpy(game_file_last_save_name,file_name);
		
		// fix all the timing
		// and state information
		 
	input_clear();

	game_time_reset(head.tick);
	view_game_reset_timing();

    return(TRUE);
}

bool game_file_reload_ok(void)
{
	char			*c,name[256];
	
		// is there a last save?
		
	if (game_file_last_save_name[0]==0x0) return(FALSE);
	
		// find the map name for this save

	strcpy(name,(game_file_last_save_name+18));
	
	c=strchr(name,'.');			// remove the .sav
	if (c!=NULL) *c=0x0;

	return(strcmp(name,map.info.name)==0);
}

bool game_file_reload(char *err_str)
{
	return(game_file_load(game_file_last_save_name,FALSE,err_str));
}

/* =======================================================

      Suspend/Resume State Save
	  These are mostly used for phones/pads where
	  the application goes into the background and
	  needs to stop chewing up memory
      
======================================================= */

void game_file_suspend(void)
{
	char			err_str[256];

		// if there is a game running, shut it down and save it

	if (server.game_open) {
		
		if (server.map_open) {
			if (game_file_save(TRUE,TRUE,err_str)) {
				game_file_has_suspended_save=TRUE;
			}
		
			map_end();
		}
		
		game_end();
	}
}

void game_file_resume(void)
{
	char			err_str[256];

		// if there was a game, resume it

	if (game_file_has_suspended_save) {
		game_file_load(NULL,TRUE,err_str);
		game_file_has_suspended_save=FALSE;
	}
}

/* =======================================================

      Checkpoints
      
======================================================= */

void game_checkpoint_clear(void)
{
	if (server.checkpoint_spot_idx!=-1) {
		server.checkpoint_spot_idx=-1;
		hud_checkpoint_show(FALSE);
	}
}

void game_checkpoint_set(int checkpoint_spot_idx)
{
	server.checkpoint_spot_idx=checkpoint_spot_idx;
	hud_checkpoint_show(TRUE);
}

void game_checkpoint_run(void)
{
	char			err_str[256];

	if (server.checkpoint_spot_idx==-1) return;
	
	if (!game_file_save(TRUE,FALSE,err_str)) {
		console_add_error(err_str);
	}

	server.checkpoint_spot_idx=-1;
	hud_checkpoint_show(FALSE);
}

