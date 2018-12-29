/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Model Fades

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

extern server_type				server;
extern map_type					map;

/* =======================================================

      Model Fades
      
======================================================= */

void model_fade_clear(model_draw *draw)
{
	draw->fade.on=FALSE;
}

void model_fade_start(model_draw *draw,int fade_msec,float end_alpha)
{
	model_draw_fade		*fade;

	fade=&draw->fade;

	fade->start_alpha=draw->alpha;
	fade->end_alpha=end_alpha;
	fade->tick=fade_msec;
	fade->start_tick=game_time_get();

	fade->on=TRUE;
}

void model_fade_run(model_draw *draw)
{
	int					tick;
	float				fct;
	model_draw_fade		*fade;

	fade=&draw->fade;
	if (!fade->on) return;

	tick=game_time_get();

		// time up?

	tick-=fade->start_tick;
	if (tick>fade->tick) {
		draw->alpha=fade->end_alpha;
		fade->on=FALSE;
		return;
	}

		// calculate fade

	fct=((float)tick)/(float)fade->tick;
	draw->alpha=fade->start_alpha+((fade->end_alpha-fade->start_alpha)*fct);
}

/* =======================================================

      Model Mesh Fades
      
======================================================= */

void model_mesh_fade_start(model_draw *draw,int mesh_idx,int fade_in_msec,int fade_life_msec,int fade_out_msec)
{
	model_draw_mesh_fade	*mesh_fade;

	if (draw->model_idx==-1) return;
	
	if (fade_in_msec<1) fade_in_msec=1;
	if (fade_life_msec<1) fade_life_msec=1;
	if (fade_out_msec<1) fade_out_msec=1;		// avoid /0 errors

	mesh_fade=&draw->meshes[mesh_idx].fade;

	mesh_fade->on=TRUE;
	mesh_fade->start_tick=game_time_get();
	mesh_fade->fade_in_msec=fade_in_msec;
	mesh_fade->fade_life_msec=fade_life_msec;
	mesh_fade->fade_out_msec=fade_out_msec;
	
	mesh_fade->alpha=0.0f;
}

void model_mesh_fade_run(model_draw *draw)
{
	int						n,tick,nmesh,curtick;
	model_type				*mdl;
	model_draw_mesh_fade	*mesh_fade;

	if (draw->model_idx==-1) return;

	mdl=server.model_list.models[draw->model_idx];

	tick=game_time_get();

	nmesh=mdl->nmesh;

	for (n=0;n!=nmesh;n++) {
		mesh_fade=&draw->meshes[n].fade;
		if (!mesh_fade->on) continue;

			// fade over?

		curtick=tick-mesh_fade->start_tick;

		if (curtick>(mesh_fade->fade_in_msec+mesh_fade->fade_life_msec+mesh_fade->fade_out_msec)) {
			mesh_fade->on=FALSE;
		}

			// calculate fade

		else {
			if (curtick<mesh_fade->fade_in_msec) {
				mesh_fade->alpha=(float)curtick/(float)mesh_fade->fade_in_msec;
			}
			else {
				if (curtick<(mesh_fade->fade_in_msec+mesh_fade->fade_life_msec)) {
					mesh_fade->alpha=1.0f;
				}
				else {
					mesh_fade->alpha=1.0f-((float)(curtick-(mesh_fade->fade_in_msec+mesh_fade->fade_life_msec))/(float)mesh_fade->fade_out_msec);
				}
			}
		}
	}
}
