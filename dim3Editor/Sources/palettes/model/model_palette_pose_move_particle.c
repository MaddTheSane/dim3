/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette Animate Pose Move Particle

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

#define kAnimationPoseMovePropertyParticleName				0
#define kAnimationPoseMovePropertyParticleBone				1
#define kAnimationPoseMovePropertyParticleMotionFactor		2
#define kAnimationPoseMovePropertyParticleMotion			3
#define kAnimationPoseMovePropertyParticleRotate			4
#define kAnimationPoseMovePropertyParticleStick				5
#define kAnimationPoseMovePropertyParticleSlop				6

extern model_type				model;
extern file_path_setup_type		file_path_setup;
extern iface_type				iface;

extern list_palette_type		model_palette;

/* =======================================================

      Property Palette Fill Animate Pose Move Particle
      
======================================================= */

void model_palette_fill_animate_pose_move_particle(int animate_idx,int pose_move_idx,int particle_idx)
{
	model_animate_type		*animate;
	model_pose_move_type	*pose_move;
	model_particle_type		*particle;

	if (particle_idx==-1) return;

	animate=&model.animates[animate_idx];
	pose_move=&animate->pose_moves[pose_move_idx];
	particle=&pose_move->particle.particles[particle_idx];

	list_palette_set_title(&model_palette,"Animation",animate->name,"Pose Move",model.poses[pose_move->pose_idx].name,"Particle",particle->name);
	list_palette_add_picker_list_string(&model_palette,kAnimationPoseMovePropertyParticleName,"Name",(char*)iface.particle_list.particles,iface.particle_list.nparticle,sizeof(iface_particle_type),(int)offsetof(iface_particle_type,name),TRUE,TRUE,particle->name,FALSE);
	list_palette_add_picker_list_int(&model_palette,kAnimationPoseMovePropertyParticleBone,"Bone",(char*)model.bones,model.nbone,sizeof(model_bone_type),(int)offsetof(model_bone_type,name),TRUE,TRUE,&particle->bone_idx,FALSE);
	list_palette_add_float(&model_palette,kAnimationPoseMovePropertyParticleMotionFactor,"Motion Factor",&particle->motion_factor,FALSE);
	list_palette_add_checkbox(&model_palette,kAnimationPoseMovePropertyParticleMotion,"Follow Model Motion",&particle->motion,FALSE);
	list_palette_add_checkbox(&model_palette,kAnimationPoseMovePropertyParticleRotate,"Follow Model Rotation",&particle->rotate,FALSE);
	list_palette_add_checkbox(&model_palette,kAnimationPoseMovePropertyParticleStick,"Follow Model Bone",&particle->stick,FALSE);
	list_palette_add_point(&model_palette,kAnimationPoseMovePropertyParticleSlop,"Position Slop",&particle->slop,FALSE);
}

/* =======================================================

      Property Palette Click Animate Pose Move Particle
      
======================================================= */

void model_palette_click_animate_pose_move_particle(int animate_idx,int pose_move_idx,int particle_idx,bool double_click)
{
}

