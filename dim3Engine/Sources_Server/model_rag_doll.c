/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Model Rag Dolls

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

extern server_type				server;
extern map_type					map;

/* =======================================================

      Model Rag Doll Clear Bones
      
======================================================= */

void model_rag_doll_clear_bones(model_draw *draw)
{
	int							n,nbone;
	model_type					*model;
	model_draw_rag_doll_bone	*rag_bone;

	if (draw->model_idx==-1) return;
	model=server.model_list.models[draw->model_idx];

	nbone=model->nbone;
	rag_bone=draw->rag_doll.bones;
	
	for (n=0;n!=nbone;n++) {
		rag_bone->rot.x=0.0f;
		rag_bone->rot.y=0.0f;
		rag_bone->rot.z=0.0f;
		rag_bone++;
	}
}

/* =======================================================

      Model Rag Doll Constrain Bones
      
======================================================= */

void model_rag_doll_constrain_bone(model_draw_rag_doll_bone *rag_bone)
{
	if (rag_bone->rot.x<-45.0f) rag_bone->rot.x=-45.0f;		// supergumba -- temporary
	if (rag_bone->rot.x>45.0f) rag_bone->rot.x=45.0f;

	if (rag_bone->rot.y<-45.0f) rag_bone->rot.y=-45.0f;
	if (rag_bone->rot.y>45.0f) rag_bone->rot.y=45.0f;

	if (rag_bone->rot.z<-45.0f) rag_bone->rot.z=-45.0f;
	if (rag_bone->rot.z>45.0f) rag_bone->rot.z=45.0f;
}

/* =======================================================

      Model Rag Doll Setup Random Bones Rotations
      
======================================================= */

void model_rag_doll_setup_random_bones(model_draw *draw)
{
	int							n,nbone;
	model_type					*model;
	model_draw_rag_doll_bone	*rag_bone;

		// get the model

	if (draw->model_idx==-1) return;
	model=server.model_list.models[draw->model_idx];

	nbone=model->nbone;

		// push the bones

	rag_bone=draw->rag_doll.bones;

	for (n=0;n!=nbone;n++) {

		switch (n%3) {
			case 0:
				rag_bone->rot_add.x=0.5f;
				break;
			case 1:
				rag_bone->rot_add.y=0.5f;
				break;
			case 2:
				rag_bone->rot_add.z=0.5f;
				break;
		}

		rag_bone++;
	}
}

/* =======================================================

      Model Rag Doll Setup Force Bones Rotations
      
======================================================= */

float model_rag_doll_2D_bone_direction(int pivot_x,int pivot_y,int bone_x,int bone_y,int force_x,int force_y)
{
	float		f,f_pivot_x,f_pivot_y,f_bone_x,f_bone_y,
				f_force_x,f_force_y;

		// translate the points and
		// work up the vectors that tell
		// us which direction the bone
		// should turn

	f_pivot_x=(float)pivot_x;
	f_pivot_y=(float)pivot_y;
	f_bone_x=(float)bone_x;
	f_bone_y=(float)bone_y;
	f_force_x=(float)force_x;
	f_force_y=(float)force_y;

	f=(((((f_bone_y-f_pivot_y)/(f_bone_x-f_pivot_x))*(f_force_x-f_pivot_x))+f_pivot_y)-f_force_y);
	return((f<0.0f)?-1.0f:1.0f);
}

void model_rag_doll_setup_push_bones(model_draw *draw)
{
	int							n,nbone;
	float						f,fd;
	d3pnt						bone_pnt[max_model_bone];
	d3ang						rot;
	model_type					*model;
	model_draw_bone_type		*draw_bone;
	model_draw_rag_doll			*rag_doll;
	model_draw_rag_doll_bone	*rag_bone;

	rag_doll=&draw->rag_doll;

		// get the model

	if (draw->model_idx==-1) return;
	model=server.model_list.models[draw->model_idx];

		// precalc all the bone positions
		// and clear all the rotations

	nbone=model->nbone;

	rag_bone=rag_doll->bones;

	for (n=0;n!=nbone;n++) {
		model_get_draw_bone_position(&draw->setup,n,&bone_pnt[n]);
		bone_pnt[n].x+=draw->pnt.x;
		bone_pnt[n].y+=draw->pnt.y;
		bone_pnt[n].z+=draw->pnt.z;

		rag_bone->rot_add.x=0.0f;
		rag_bone->rot_add.y=0.0f;
		rag_bone->rot_add.z=0.0f;
		rag_bone++;
	}

		// push the bones

	for (n=0;n!=nbone;n++) {

			// can't rotate if no parents

		draw_bone=&draw->setup.bones[n];
		if (draw_bone->parent_idx==-1) continue;

			// get distance from hit
			// supergumba -- hard code distance!

		fd=(float)distance_get(bone_pnt[n].x,bone_pnt[n].y,bone_pnt[n].z,rag_doll->force_pnt.x,rag_doll->force_pnt.y,rag_doll->force_pnt.z);
		f=((fd*0.5f)/50000.0f);
		
			// for force hit

		rot.x=f*model_rag_doll_2D_bone_direction(bone_pnt[draw_bone->parent_idx].y,bone_pnt[draw_bone->parent_idx].z,bone_pnt[n].y,bone_pnt[n].z,rag_doll->force_pnt.y,rag_doll->force_pnt.z);
		rot.y=f*model_rag_doll_2D_bone_direction(bone_pnt[draw_bone->parent_idx].x,bone_pnt[draw_bone->parent_idx].z,bone_pnt[n].x,bone_pnt[n].z,rag_doll->force_pnt.x,rag_doll->force_pnt.z);
		rot.z=f*model_rag_doll_2D_bone_direction(bone_pnt[draw_bone->parent_idx].x,bone_pnt[draw_bone->parent_idx].y,bone_pnt[n].x,bone_pnt[n].y,rag_doll->force_pnt.x,rag_doll->force_pnt.y);
		
			// for gravity
			// supergumba -- also hard coded!

		rot.x+=0.2f*model_rag_doll_2D_bone_direction(bone_pnt[draw_bone->parent_idx].y,bone_pnt[draw_bone->parent_idx].z,bone_pnt[n].y,bone_pnt[n].z,draw->pnt.y,draw->pnt.z);
		rot.y+=0.2f*model_rag_doll_2D_bone_direction(bone_pnt[draw_bone->parent_idx].x,bone_pnt[draw_bone->parent_idx].z,bone_pnt[n].x,bone_pnt[n].z,draw->pnt.x,draw->pnt.z);
		rot.z+=0.2f*model_rag_doll_2D_bone_direction(bone_pnt[draw_bone->parent_idx].x,bone_pnt[draw_bone->parent_idx].y,bone_pnt[n].x,bone_pnt[n].y,draw->pnt.x,draw->pnt.y);

			// get parent rag bone
			// for setting rotation

		rag_bone=&rag_doll->bones[draw_bone->parent_idx];

		rag_bone->rot_add.x=rot.x;
		rag_bone->rot_add.y=rot.y;
		rag_bone->rot_add.z=rot.z;
	}
}

/* =======================================================

      Model Rag Doll Run Bones
      
======================================================= */

void model_rag_doll_run_bones(model_draw *draw)
{
	int							n,nbone;
	model_type					*model;
	model_draw_rag_doll_bone	*rag_bone;

		// get the model

	if (draw->model_idx==-1) return;
	model=server.model_list.models[draw->model_idx];

	nbone=model->nbone;

		// position the bones

	rag_bone=draw->rag_doll.bones;

	for (n=0;n!=nbone;n++) {
		rag_bone->rot.x+=rag_bone->rot_add.x;
		rag_bone->rot.y+=rag_bone->rot_add.y;
		rag_bone->rot.z+=rag_bone->rot_add.z;

		model_rag_doll_constrain_bone(rag_bone);

		rag_bone++;
	}
}

void model_rag_doll_reset_bones(model_draw *draw)
{
	int							n,nbone;
	bool						finished;
	model_type					*model;
	model_draw_rag_doll_bone	*rag_bone;

		// get the model

	if (draw->model_idx==-1) return;
	model=server.model_list.models[draw->model_idx];

	nbone=model->nbone;

		// reset by 10% each time

	finished=TRUE;
	rag_bone=draw->rag_doll.bones;

	for (n=0;n!=nbone;n++) {
		rag_bone->rot.x*=0.9f;
		if (rag_bone->rot.x<=1.0f) rag_bone->rot.x=0.0f;
		rag_bone->rot.y*=0.9f;
		if (rag_bone->rot.y<=1.0f) rag_bone->rot.y=0.0f;
		rag_bone->rot.z*=0.9f;
		if (rag_bone->rot.z<=1.0f) rag_bone->rot.z=0.0f;

		if ((rag_bone->rot.x!=0.0f) || (rag_bone->rot.y!=0.0f) || (rag_bone->rot.z!=0.0f)) finished=FALSE;

		rag_bone++;
	}

	if (finished) draw->rag_doll.mode=rag_doll_mode_off;
}

/* =======================================================

      Model Rag Doll Start/Stop
      
======================================================= */

void model_rag_doll_clear(model_draw *draw)
{
	model_rag_doll_clear_bones(draw);
	draw->rag_doll.mode=rag_doll_mode_off;
}

void model_rag_doll_start(model_draw *draw,d3pnt *force_pnt,int force,int force_msec,bool need_reset,bool rag_random)
{
	model_draw_rag_doll		*rag_doll;

		// stop all current animation

	model_stop_animation(draw);

		// start the rag doll

	rag_doll=&draw->rag_doll;

	rag_doll->tick=force_msec;
	rag_doll->start_tick=game_time_get();

	rag_doll->force=force;
	memmove(&rag_doll->force_pnt,force_pnt,sizeof(d3pnt));

	rag_doll->need_reset=need_reset;

		// setup rotations

	if (rag_random) {
		model_rag_doll_setup_random_bones(draw);
		rag_doll->mode=rag_doll_mode_random;
	}
	else {
		model_rag_doll_setup_push_bones(draw);
		rag_doll->mode=rag_doll_mode_force;
	}
}

void model_rag_doll_stop(model_draw *draw)
{
	if (draw->rag_doll.need_reset) {
		draw->rag_doll.mode=rag_doll_mode_reset;
		return;
	}

	draw->rag_doll.mode=rag_doll_mode_off;
}

/* =======================================================

      Model Rag Doll Run
      
======================================================= */

void model_rag_doll_run(model_draw *draw)
{
	int						tick;
	model_draw_rag_doll		*rag_doll;

	rag_doll=&draw->rag_doll;
	if (rag_doll->mode==rag_doll_mode_off) return;

		// time up?

	if (rag_doll->mode!=rag_doll_mode_reset) {
		tick=game_time_get()-rag_doll->start_tick;
		if (tick>rag_doll->tick) {
			if (!rag_doll->need_reset) {
				rag_doll->mode=rag_doll_mode_off;
				return;
			}

			rag_doll->mode=rag_doll_mode_reset;
		}
	}

		// run rag dolls

	switch (rag_doll->mode) {
		case rag_doll_mode_force:
		case rag_doll_mode_random:
			model_rag_doll_run_bones(draw);
			break;
		case rag_doll_mode_reset:
			model_rag_doll_reset_bones(draw);
			break;
	}
}

