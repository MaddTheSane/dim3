/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Main Model

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

extern app_type					app;
extern server_type				server;
extern setup_type				setup;
extern iface_type				iface;
extern file_path_setup_type		file_path_setup;

/* =======================================================

      Model Lists
      
======================================================= */

void model_initialize_list(void)
{
	int				n;

	for (n=0;n!=max_model_list;n++) {
		server.model_list.models[n]=NULL;
	}
}

void model_free_list(void)
{
	int				n;
	
	for (n=0;n!=max_model_list;n++) {
		if (server.model_list.models[n]==NULL) free(server.model_list.models[n]);
	}
}

/* =======================================================

      Find Models
      
======================================================= */

model_type* model_find(char *name)
{
	int				n;
	model_type		*mdl;
	
	for (n=0;n!=max_model_list;n++) {
		mdl=server.model_list.models[n];
		if (mdl==NULL) continue;

		if (strcasecmp(mdl->name,name)==0) return(mdl);
	}
	
	return(NULL);
}

int model_find_index(char *name)
{
	int				n;
	model_type		*mdl;

	for (n=0;n!=max_model_list;n++) {
		mdl=server.model_list.models[n];
		if (mdl==NULL) continue;

		if (strcasecmp(mdl->name,name)==0) return(n);
	}
	
	return(-1);
}

/* =======================================================

      Load and Dispose Models
      
======================================================= */

int model_load(char *name)
{
	int				n,idx;
	model_type		*mdl;

		// find empty spot

	idx=-1;

	for (n=0;n!=max_model_list;n++) {
		mdl=server.model_list.models[n];
		if (mdl==NULL) {
			idx=n;
			break;
		}
	}

	if (idx==-1) return(-1);

		// memory for model

	mdl=(model_type*)malloc(sizeof(model_type));
	if (mdl==NULL) return(-1);

		// load model

	if (!model_open(mdl,name,FALSE)) {
		free(mdl);
		return(-1);
	}

		// setup some animation indexes to avoid name lookups

	model_animation_effect_setup(mdl);

		// put in model list

	server.model_list.models[idx]=mdl;
	
	return(idx);
}

void model_dispose(int idx)
{
	model_type			*mdl;

		// dispose model
		
	mdl=server.model_list.models[idx];
	model_close(mdl);

		// free the data and fix
		// the list

	free(mdl);
	server.model_list.models[idx]=NULL;
}

/* =======================================================

      Open and Close Model Draws
      
======================================================= */

bool model_draw_initialize(model_draw *draw,char *item_type,char *item_name,char *err_str)
{
	int					n;
	bool				ok;
	model_type			*mdl;
	
	draw->model_idx=-1;
	draw->center.x=draw->center.y=draw->center.z=0;
	draw->size.x=draw->size.y=draw->size.z=0;

	draw->script_animation_idx=0;
	draw->script_halo_idx=0;
	draw->script_light_idx=0;
	
	ok=TRUE;
	mdl=NULL;

	if (draw->name[0]!=0x0) {
		draw->model_idx=model_find_index(draw->name);
		if (draw->model_idx!=-1) {
			mdl=server.model_list.models[draw->model_idx];
			model_get_size(mdl,&draw->size.x,&draw->size.y,&draw->size.z);
			memmove(&draw->center,&mdl->center,sizeof(d3pnt));
		}
		else {
			draw->on=FALSE;
			sprintf(err_str,"Unable to load model named %s for %s: %s",draw->name,item_type,item_name);
			ok=FALSE;
		}
	}
	else {
		draw->on=FALSE;
	}
	
		// setup flags telling which textures
		// are used by which mesh
	
		// initialize draw memory
		
	if (mdl!=NULL) model_draw_setup_initialize(mdl,&draw->setup,TRUE);

		// stop all animations and fades

	for (n=0;n!=max_model_blend_animation;n++) {
		draw->script_animation_idx=n;
		model_stop_animation(draw);
	}

	draw->script_animation_idx=0;

	model_fade_clear(draw);

	return(ok);
}

void model_draw_free(model_draw *draw)
{
	if (draw->model_idx!=-1) model_draw_setup_shutdown(server.model_list.models[draw->model_idx],&draw->setup);
}

/* =======================================================

      Reset Model UIDs after Load
      
======================================================= */

void model_reset_single(model_draw *draw)
{
//	int			x;

//	x=draw->model_idx;
//	draw->model_idx=model_find_index(draw->name);


		// if model is loaded, update
		// reference count

//	if (draw->model_idx!=-1) {
	//	server.model_list.models[draw->model_idx]->load.reference_count++;
//	}

		// try to load it

//	else {
	//	draw->model_idx=model_load(draw->name,FALSE);
	//	if (draw->model_idx==-1) draw->on=FALSE;
//	}
//	fprintf(stdout,"%s: original=%d, new=%d\n",draw->name,x,draw->model_idx);

		// and create the draw setup memory

	if (draw->model_idx!=-1) {
		model_draw_setup_initialize(server.model_list.models[draw->model_idx],&draw->setup,TRUE);
	}
}

void model_rebuild_draw_memory(model_draw *draw)
{
	if (draw->model_idx!=-1) {
		model_draw_setup_initialize(server.model_list.models[draw->model_idx],&draw->setup,TRUE);
	}
}

void models_reset(void)
{
	int					n,k,i;
	obj_type			*obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	proj_type			*proj;

		// fix model indexes
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
		
		model_reset_single(&obj->draw);
	
		for (k=0;k!=max_weap_list;k++) {
			weap=obj->weap_list.weaps[k];
			if (weap==NULL) continue;

			model_reset_single(&weap->draw);
			if (weap->dual.on) model_reset_single(&weap->draw_dual);
		
			for (i=0;i!=max_proj_setup_list;i++) {
				proj_setup=weap->proj_setup_list.proj_setups[i];
				if (proj_setup==NULL) continue;
				
				model_reset_single(&proj_setup->draw);
			}
		}
	}

    for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (!proj->on) continue;
		
		model_reset_single(&proj->draw);
	}
}

/* =======================================================

      Model PreLoading
      
======================================================= */

void model_preload_start(void)
{
	int							n;
	file_path_directory_type	*fpd;
	
	fpd=file_paths_read_directory_data_dir(&file_path_setup,"Models","Model.xml");

	for (n=0;n!=fpd->nfile;n++) {
		if (!fpd->files[n].is_dir) model_load(fpd->files[n].file_name);
	}
}

void model_preload_free(void)
{
	int				n;
	model_type		*mdl;

	for (n=0;n!=max_model_list;n++) {
		mdl=server.model_list.models[n];
		if (mdl!=NULL) model_dispose(n);
	}
}


