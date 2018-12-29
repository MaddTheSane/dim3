/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Hand

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

extern map_type				map;
extern camera_type			camera;
extern server_type			server;
extern view_type			view;

/* =======================================================

      Draw Weapon in Hand
      
======================================================= */

void draw_weapon_hand_setup(obj_type *obj,weapon_type *weap)
{
	int				tick;
    model_draw		*draw;
	model_type		*mdl;
	
		// weapons hidden or zoom on?
		
	if (obj->hide_all_weapons) return;
	if ((weap->zoom.on) && (weap->zoom.mode!=zoom_mode_off) && (obj->zoom_draw.on) && (!weap->zoom.show_weapon)) return;

		// weapon model draw
		
	draw=&weap->draw;
	if ((draw->model_idx==-1) || (!draw->on)) return;
	
	mdl=server.model_list.models[draw->model_idx];

		// regular weapon model

	tick=game_time_get();
	
	model_draw_setup_clear(mdl,&draw->setup);
	model_draw_setup_weapon(obj,weap,FALSE,FALSE);

	model_calc_animation(draw,tick);
	model_calc_draw_bones(draw);
	
	render_model_setup(draw,tick);
	render_model_build_vertex_lists(draw,TRUE);

		// dual wielded weapons

	if ((weap->dual.on) && (weap->dual.active)) {
	
		draw=&weap->draw_dual;
		if ((draw->model_idx!=-1) && (draw->on)) {
			mdl=server.model_list.models[draw->model_idx];

			model_draw_setup_clear(mdl,&draw->setup);
			model_draw_setup_weapon(obj,weap,FALSE,TRUE);

			model_calc_animation(draw,tick);
			model_calc_draw_bones(draw);
			
			render_model_setup(draw,tick);
			render_model_build_vertex_lists(draw,TRUE);
		}
	}
}

void draw_weapon_hand(obj_type *obj,weapon_type *weap)
{
	float			old_fov;
    model_draw		*draw;
	model_type		*mdl;
	
		// weapons hidden or zoom on?
		
	if (obj->hide_all_weapons) return;
	if ((weap->zoom.on) && (weap->zoom.mode!=zoom_mode_off) && (obj->zoom_draw.on) && (!weap->zoom.show_weapon)) return;

		// weapon model draw
		
	draw=&weap->draw;
	if ((draw->model_idx==-1) || (!draw->on)) return;
	
	mdl=server.model_list.models[draw->model_idx];
	
		// handle any FOV overrides
		
	if (weap->hand.fov_override!=0.0f) {
		old_fov=view.render->camera.fov;
		view.render->camera.fov=weap->hand.fov_override;
	}

		// setup drawing
	
	glClear(GL_DEPTH_BUFFER_BIT);

		// regular weapon model

	render_model_opaque(draw);
	render_model_transparent(draw);

		// dual wielded weapons

	if ((weap->dual.on) && (weap->dual.active)) {
	
		draw=&weap->draw_dual;
		if ((draw->model_idx!=-1) && (draw->on)) {
			mdl=server.model_list.models[draw->model_idx];

			render_model_opaque(draw);
			render_model_transparent(draw);
		}
	}
	
		// restore FOV
		
	if (weap->hand.fov_override!=0.0f) {
		view.render->camera.fov=old_fov;
	}
}

