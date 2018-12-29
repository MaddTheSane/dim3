/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Playing

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

model_type						model;
model_draw_setup				draw_setup;
app_state_type					state;

/* =======================================================

      Mode Play Drawing
      
======================================================= */

void model_play_draw_calc_animation(int cur_tick,int animate_idx,int blend_idx,bool non_blend_setup)
{
	int						first_pose,last_pose,tick,nxt_tick,
							pose_move_1_idx,pose_move_2_idx,msec;
	float					pose_factor;
	model_animate_type		*animate;
	model_pose_move_type	*pose_move_1,*pose_move_2;

		// calc animation
		
	animate=&model.animates[animate_idx];
	
	first_pose=0;
	if (animate->loop_start!=-1) first_pose=animate->loop_start;

	last_pose=animate->npose_move-1;
	if (animate->loop_end!=-1) last_pose=animate->loop_end;
	
	tick=state.model.blend[blend_idx].tick;
	pose_move_1_idx=state.model.blend[blend_idx].pose_move_idx;
	
	msec=animate->pose_moves[pose_move_1_idx].msec;
	nxt_tick=tick+msec;
	
	while (nxt_tick<cur_tick) {				// skip ahead till we find correct pose
		pose_move_1_idx++;
		if (pose_move_1_idx>last_pose) pose_move_1_idx=first_pose;
		
		msec=animate->pose_moves[pose_move_1_idx].msec;

		tick=nxt_tick;
		nxt_tick=tick+msec;
	}
	
	state.model.blend[blend_idx].tick=tick;
	state.model.blend[blend_idx].pose_move_idx=pose_move_1_idx;
	
	pose_move_2_idx=pose_move_1_idx+1;
	if (pose_move_2_idx>last_pose) pose_move_2_idx=first_pose;
	
		// get the pose moves
		
	pose_move_1=&animate->pose_moves[pose_move_1_idx];
	pose_move_2=&animate->pose_moves[pose_move_2_idx];
	
		// setup the poses

	draw_setup.enhance_factor=animate->enhance_factor;
	
	draw_setup.poses[blend_idx].idx_1=pose_move_1->pose_idx;
	draw_setup.poses[blend_idx].idx_2=pose_move_2->pose_idx;
	
	pose_factor=(float)(nxt_tick-cur_tick)/(float)msec;
	
    draw_setup.poses[blend_idx].factor=pose_factor;
	draw_setup.poses[blend_idx].acceleration=pose_move_1->acceleration;
	
		// non-blended setup
		
	if (non_blend_setup) {
		draw_setup.ang.x=0;
		draw_setup.ang.y=0;
		draw_setup.ang.z=0;
		
		draw_setup.sway.x=pose_move_2->sway.x+((pose_move_1->sway.x-pose_move_2->sway.x)*pose_factor);
		draw_setup.sway.y=pose_move_2->sway.y+((pose_move_1->sway.y-pose_move_2->sway.y)*pose_factor);
		draw_setup.sway.z=pose_move_2->sway.z+((pose_move_1->sway.z-pose_move_2->sway.z)*pose_factor);
		draw_setup.move.x=pose_move_2->mov.x+((pose_move_1->mov.x-pose_move_2->mov.x)*pose_factor);
		draw_setup.move.y=pose_move_2->mov.y+((pose_move_1->mov.y-pose_move_2->mov.y)*pose_factor);
		draw_setup.move.z=pose_move_2->mov.z+((pose_move_1->mov.z-pose_move_2->mov.z)*pose_factor);
	}

		// if this is the first pose, reset
		// the selected poses

	if (blend_idx==0) {
		state.model.cur_animate_pose_move_idx=pose_move_1_idx;
		state.model.cur_pose_idx=animate->pose_moves[pose_move_1_idx].pose_idx;
	}
}

void model_view_draw_play(void)
{
	int					n,cur_tick,
						old_pose_move_idx;
	
		// if no current animation, just do no pose for animated textures
		
	if ((state.model.cur_animate_idx==-1) && (state.model.play_mode!=model_play_mode_blend)) {
		main_wind_draw();
		return;
	}
	
		// clear the draw setup
		
	model_draw_setup_clear(&model,&draw_setup);
	
		// get the current animation tick
		// and divide by 4 if we are in slow play
		
	cur_tick=time_get();
	if (state.model.play_mode==model_play_mode_slow) cur_tick=cur_tick>>2;
	
		// calculate the animation

	old_pose_move_idx=state.model.blend[0].pose_move_idx;

	if (state.model.play_mode!=model_play_mode_blend) {
		model_play_draw_calc_animation(cur_tick,state.model.cur_animate_idx,0,TRUE);
	}
	else {
		for (n=0;n!=max_model_blend_animation;n++) {
			if (state.model.blend[n].animate_idx!=-1) {
				model_play_draw_calc_animation(cur_tick,state.model.blend[n].animate_idx,n,(n==0));
			}
		}
	}

		// if prev/next animation type
		// stop if pose has changed

	if ((state.model.play_mode==model_play_mode_prev) || (state.model.play_mode==model_play_mode_next)) {
		if (old_pose_move_idx!=state.model.blend[0].pose_move_idx) {
			model_play(model_play_mode_stop);
		}
	}

		// global draw setup
	
	model_draw_wind(state.model.cur_mesh_idx);
}

/* =======================================================

      Main Model View Draw
      
======================================================= */

void model_view_draw(void)
{
	if (state.model.play_mode==model_play_mode_stop) {
		model_draw_wind_pose(state.model.cur_mesh_idx,state.model.cur_pose_idx);
	}
	else {
		model_view_draw_play();
	}
}

/* =======================================================

      Model Playing
      
======================================================= */

void model_play(int play_mode)
{
	int					n,tick,pose_move_idx;
	
		// good animation?
		
	if (play_mode!=model_play_mode_stop) {
		if (state.model.cur_animate_idx==-1) return;
		if (model.animates[state.model.cur_animate_idx].npose_move==0) return;
	}
	
		// always turn off animation until setup is complete
		// as animation is on a timer
		
	state.model.play_mode=model_play_mode_stop;

		// if it's next or previous, we
		// need to start on current selection

	pose_move_idx=0;

	switch (play_mode) {

		case model_play_mode_prev:
			pose_move_idx=state.model.cur_animate_pose_move_idx-1;
			if (pose_move_idx<0) pose_move_idx=model.animates[state.model.cur_animate_idx].npose_move-1;
			break;

		case model_play_mode_next:
			pose_move_idx=state.model.cur_animate_pose_move_idx;
			break;
	}
	
		// setup animation
		
	time_start();
	tick=time_get();

	for (n=0;n!=max_model_blend_animation;n++) {
		state.model.blend[n].pose_move_idx=pose_move_idx;
		state.model.blend[n].tick=tick;
	}
	
		// turn on/off animation
		
	state.model.play_mode=play_mode;
	
	if (state.model.model_open) main_wind_draw();
}

