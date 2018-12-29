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

extern app_type			app;
extern server_type		server;
extern setup_type		setup;
extern iface_type		iface;

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

      Open Models
      
======================================================= */

int model_load(char *name,bool force_opengl_textures)
{
	int				n,idx;
	bool			load_textures;
	model_type		*mdl;
	
		// has model been already loaded?
		// if so, return model and increment reference count
	
	idx=model_find_index(name);
	if (idx!=-1) {
		server.model_list.models[idx]->load.reference_count++;
		return(idx);
	}

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

	load_textures=((!app.dedicated_host) || (force_opengl_textures));
		
	if (!model_open(mdl,name,load_textures)) {
		free(mdl);
		return(-1);
	}

	if (!app.dedicated_host) gl_shader_attach_model(mdl);

		// setup some animation indexes to avoid name lookups

	model_animation_effect_setup(mdl);

		// start reference count at 1

	mdl->load.reference_count=1;
	mdl->load.preloaded=FALSE;

		// put in model list

	server.model_list.models[idx]=mdl;
	
	return(idx);
}

bool model_draw_load(model_draw *draw,char *item_type,char *item_name,bool force_opengl_textures,char *err_str)
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
		draw->model_idx=model_load(draw->name,force_opengl_textures);
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
	model_rag_doll_clear(draw);

		// create the VBO

	if (!app.dedicated_host) {
		if (draw->model_idx!=-1) view_create_model_vertex_object(draw);
	}

	return(ok);
}

/* =======================================================

      Dispose Models
      
======================================================= */

void model_dispose(int idx)
{
	model_type			*mdl;

		// find model
		
	mdl=server.model_list.models[idx];

		// decrement reference count

	mdl->load.reference_count--;
	if (mdl->load.reference_count>0) return;

		// dispose model

	model_close(mdl);

		// free the data and fix
		// the list

	free(mdl);
	server.model_list.models[idx]=NULL;
}

void model_draw_dispose(model_draw *draw)
{
		// find model
		
	if (draw->model_idx==-1) return;

		// dispose VBO

	if (!app.dedicated_host) view_dispose_model_vertex_object(draw);
	
		// clear draw memory
		
	model_draw_setup_shutdown(server.model_list.models[draw->model_idx],&draw->setup);

		// dispose model

	model_dispose(draw->model_idx);
}

/* =======================================================

      Reset Model UIDs after Load
      
======================================================= */

void model_reset_single(model_draw *draw)
{
	draw->model_idx=model_find_index(draw->name);

		// if model is loaded, update
		// reference count

	if (draw->model_idx!=-1) {
		server.model_list.models[draw->model_idx]->load.reference_count++;
	}

		// try to load it

	else {
		draw->model_idx=model_load(draw->name,FALSE);
		if (draw->model_idx==-1) draw->on=FALSE;
	}

		// and create the draw setup memory

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
	model_type			*mdl;

		// will need to reset all model
		// reference counts, we don't know
		// where they are left off after load
		
		// special check for preloaded models
		// so we don't end up losing their
		// one preload count

	for (n=0;n!=max_model_list;n++) {
		mdl=server.model_list.models[n];
		if (mdl!=NULL) {
			mdl->load.reference_count=0;
			if (mdl->load.preloaded) mdl->load.reference_count++;
		}
	}

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

		// now remove all models with zero
		// reference count, our loaded file
		// isn't using them

	for (n=0;n!=max_model_list;n++) {
		mdl=server.model_list.models[n];
		if (mdl!=NULL) {
			if (mdl->load.reference_count<=0) model_dispose(n);
		}
	}
}

/* =======================================================

      Model PreLoading
      
======================================================= */

void model_preload_start(void)
{
	int			n,idx;
	
	for (n=0;n!=max_preload_model;n++) {
		if (iface.preload_model.names[n][0]==0x0) continue;
		
		idx=model_load(iface.preload_model.names[n],FALSE);
		if (idx!=-1) server.model_list.models[idx]->load.preloaded=TRUE;
	}
}

void model_preload_free(void)
{
	int			n,idx;
	
	for (n=0;n!=max_preload_model;n++) {
		if (iface.preload_model.names[n][0]==0x0) continue;
		
		idx=model_find_index(iface.preload_model.names[n]);
		if (idx!=-1) model_dispose(idx);
	}
}


