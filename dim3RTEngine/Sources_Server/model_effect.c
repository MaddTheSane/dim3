/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Model Effects (Sounds, Particles)

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
#include "objects.h"

extern app_type					app;
extern server_type				server;
extern view_type				view;
extern map_type					map;
extern network_setup_type		net_setup;

/* =======================================================

      Model Effect Setup
      
======================================================= */

void model_animation_effect_setup(model_type *mdl)
{
	int						n,k,t,nanimate,npose_move;
	model_animate_type		*animate;
	model_pose_move_type	*pose_move;

		// get sound, particle, and ring indexes
		// so they don't have to be looked up every time
		// they are launched

	nanimate=mdl->nanimate;
	animate=mdl->animates;

	for (n=0;n!=nanimate;n++) {

		npose_move=animate->npose_move;
		pose_move=animate->pose_moves;

		for (k=0;k!=npose_move;k++) {

			pose_move->sound.buffer_idx=-1;
			if (pose_move->sound.name[0]!=0x0) pose_move->sound.buffer_idx=al_find_buffer(pose_move->sound.name);

			for (t=0;t!=pose_move->particle.count;t++) {
				pose_move->particle.particles[t].particle_idx=-1;
				if (pose_move->particle.particles[t].name[0]!=0x0) pose_move->particle.particles[t].particle_idx=particle_find_index(pose_move->particle.particles[t].name);
			}
			
			for (t=0;t!=pose_move->ring.count;t++) {
				pose_move->ring.rings[t].ring_idx=-1;
				if (pose_move->ring.rings[t].name[0]!=0x0) pose_move->ring.rings[t].ring_idx=ring_find_index(pose_move->ring.rings[t].name);
			}

			pose_move++;
		}

		animate++;
	}
}

/* =======================================================

      Model Effect Bone Position
      
======================================================= */

void model_animation_effect_launch_bone_position(model_draw *draw,int animate_idx,int pose_idx,int bone_idx,d3pnt *pnt)
{
	model_type				*mdl;

		// default to center of model

	pnt->x=draw->pnt.x;
	pnt->y=draw->pnt.y;
	pnt->z=draw->pnt.z;

		// bone position

	if ((bone_idx!=-1) && (draw->model_idx!=-1)) {
		mdl=server.model_list.models[draw->model_idx];

		model_calc_draw_bone_position(mdl,&draw->setup,pose_idx,bone_idx,pnt);
		pnt->x+=draw->pnt.x;
		pnt->y+=draw->pnt.y;
		pnt->z+=draw->pnt.z;
	}
}

/* =======================================================

      Model Effect Particles
      
======================================================= */

void model_animation_effect_launch_particle(model_particle_type *particle,d3pnt *pnt,d3ang *ang,model_draw_connect *connect,int bone_idx)
{
	d3pnt			spnt;
	particle_rotate	rot;
	particle_motion	motion;

	memmove(&spnt,pnt,sizeof(d3pnt));
	
		// make particle slop
		
	if (particle->slop.x!=0) spnt.x+=random_int(particle->slop.x*2)-particle->slop.x;
	if (particle->slop.y!=0) spnt.y+=random_int(particle->slop.y*2)-particle->slop.y;
	if (particle->slop.z!=0) spnt.z+=random_int(particle->slop.z*2)-particle->slop.z;
			
		// setup particle

	rot.on=FALSE;

	motion.bone_idx=-1;
	motion.vct.x=motion.vct.y=motion.vct.z=0.0f;

	if (particle->rotate) {
		rot.on=TRUE;
		memmove(&rot.ang,ang,sizeof(d3ang));
	}

	if (particle->motion) {
		memmove(&motion.vct,&connect->motion_vct,sizeof(d3vct));
		motion.vct.x*=particle->motion_factor;
		motion.vct.y*=particle->motion_factor;
		motion.vct.z*=particle->motion_factor;
	}

	if (particle->stick) {
		motion.obj_idx=connect->obj_idx;
		motion.weap_idx=connect->weap_idx;
		motion.proj_idx=connect->proj_idx;
		motion.weap_in_dual=connect->weap_in_dual;
		motion.bone_idx=bone_idx;
	}

		// spawn particle

	particle_spawn(particle->particle_idx,connect->obj_idx,&spnt,&rot,&motion);
}

/* =======================================================

      Model Effect Rings
      
======================================================= */

void model_animation_effect_launch_ring(model_ring_type *ring,d3pnt *pnt,d3ang *ang,model_draw_connect *connect)
{
	d3pnt			spnt;

		// make ring slop

	memmove(&spnt,pnt,sizeof(d3pnt));
		
	if (ring->slop.x!=0) spnt.x+=random_int(ring->slop.x*2)-ring->slop.x;
	if (ring->slop.y!=0) spnt.y+=random_int(ring->slop.y*2)-ring->slop.y;
	if (ring->slop.z!=0) spnt.z+=random_int(ring->slop.z*2)-ring->slop.z;
	
		// spawn ring
		
	if (ring->angle) {
		ring_spawn(ring->ring_idx,connect->obj_idx,&spnt,ang);
	}
	else {
		ring_spawn(ring->ring_idx,connect->obj_idx,&spnt,NULL);
	}
}

/* =======================================================

      Model Effect Launches
      
======================================================= */

void model_animation_effect_launch(model_draw *draw,int animate_idx,int pose_idx)
{
	int						t;
	bool					global;
	d3pnt					pnt;
	model_type				*mdl;
	model_draw_setup		*setup;
	model_animate_type		*animate;
	model_pose_move_type	*pose_move;
	
	if (draw->model_idx==-1) return;

	setup=&draw->setup;
	mdl=server.model_list.models[draw->model_idx];
	
	animate=&mdl->animates[animate_idx];
	if ((pose_idx<0) || (pose_idx>=animate->npose_move)) return;
	
	pose_move=&animate->pose_moves[pose_idx];

		// sounds
		
	if (pose_move->sound.name[0]!=0x0) {
		model_animation_effect_launch_bone_position(draw,animate_idx,pose_move->pose_idx,pose_move->sound.bone_idx,&pnt);

		global=pose_move->sound.no_position;		// only use global sounds if launched by player
		if (!draw->player) global=FALSE;

		if ((!app.dedicated_host) && (pose_move->sound.buffer_idx!=-1)) {
			al_play_source(pose_move->sound.buffer_idx,&pnt,pose_move->sound.pitch,FALSE,FALSE,global,draw->player);
			object_watch_sound_alert(&pnt,draw->connect.obj_idx,pose_move->sound.name);	// sound watches
		}
		
		if ((net_setup.mode!=net_mode_none) && (draw->connect.net_sound)) {
			if (draw->connect.obj_idx!=-1) net_client_send_sound(server.obj_list.objs[draw->connect.obj_idx],&pnt,pose_move->sound.pitch,pose_move->sound.name);
		}
	}

		// mesh fades

	if (pose_move->mesh_fade.mesh_idx!=-1) {
		model_mesh_fade_start(draw,pose_move->mesh_fade.mesh_idx,pose_move->mesh_fade.fade_in_msec,pose_move->mesh_fade.fade_life_msec,pose_move->mesh_fade.fade_out_msec);
	}
	
		// flash
		
	if (pose_move->flash.intensity!=0) {
		model_animation_effect_launch_bone_position(draw,animate_idx,pose_move->pose_idx,pose_move->flash.bone_idx,&pnt);
		effect_spawn_flash(&pnt,&pose_move->flash.col,pose_move->flash.intensity,pose_move->flash.exponent,pose_move->flash.flash_msec,pose_move->flash.fade_msec);
	}
	
		// shake
		
	if (pose_move->shake.distance!=0) {
		effect_spawn_shake(&pnt,pose_move->shake.distance,pose_move->shake.size,pose_move->shake.life_msec);
	}
	
		// particles
		
	for (t=0;t!=pose_move->particle.count;t++) {
		if (pose_move->particle.particles[t].particle_idx!=-1) {
			model_animation_effect_launch_bone_position(draw,animate_idx,pose_move->pose_idx,pose_move->particle.particles[t].bone_idx,&pnt);
			model_animation_effect_launch_particle(&pose_move->particle.particles[t],&pnt,&setup->ang,&draw->connect,pose_move->particle.particles[t].bone_idx);
		}
	}

		// rings
		
	for (t=0;t!=pose_move->ring.count;t++) {
		if (pose_move->ring.rings[t].ring_idx!=-1) {
			model_animation_effect_launch_bone_position(draw,animate_idx,pose_move->pose_idx,pose_move->ring.rings[t].bone_idx,&pnt);
			model_animation_effect_launch_ring(&pose_move->ring.rings[t],&pnt,&setup->ang,&draw->connect);
		}
	}
}

