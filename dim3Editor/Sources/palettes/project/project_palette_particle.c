/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Particle

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

#define kParticleSettingsName					0
#define kParticleSettingsBitmapName				1
#define kParticleSettingsChainName				2
#define kParticleSettingsCount					3
#define kParticleSettingsLife					4

#define kParticleOptionReverse					5
#define kParticleOptionBlendAdd					6
#define kParticleOptionGlobe					7

#define kParticleMotionOffset					9
#define kParticleMotionMove						10
#define kParticleMotionRot						11
#define kParticleMotionRotAccel					12

#define kParticleParticleStartSize				13
#define kParticleParticleEndSize				14
#define kParticleParticleStartGravity			15
#define kParticleParticleGravityAdd				16
#define kParticleParticleStartAlpha				17
#define kParticleParticleEndAlpha				18
#define kParticleParticleAmbientFactor			19
#define kParticleParticleStartColor				20
#define kParticleParticleEndColor				21

#define kParticleSettingsTrailCount				22
#define kParticleSettingsTrailStep				23
#define kParticleSettingsTrailReduceFactor		24

#define kParticleImageCount						25
#define kParticleImageMillisecond				26
#define kParticleImageLoop						27
#define kParticleImageLoopBack					28

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Particle
      
======================================================= */

void project_palette_fill_particle(int particle_idx)
{
	iface_particle_type			*particle;

	particle=&iface.particle_list.particles[particle_idx];

	list_palette_set_title(&project_palette,"Particles",NULL,"Particle",particle->name,NULL,NULL);

		// settings

	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_string(&project_palette,kParticleSettingsName,"Name",particle->name,name_str_len,FALSE);
	list_palette_add_picker_file(&project_palette,kParticleSettingsBitmapName,list_button_none,0,"Bitmap","Bitmaps/Particles","png","",particle->bitmap_name,FALSE);
	list_palette_add_picker_list_string(&project_palette,kParticleSettingsChainName,"Chain Particle",(char*)iface.particle_list.particles,iface.particle_list.nparticle,sizeof(iface_particle_type),(int)offsetof(iface_particle_type,name),FALSE,TRUE,particle->chain_name,FALSE);
	list_palette_add_int(&project_palette,kParticleSettingsCount,"Count",&particle->count,FALSE);
	list_palette_add_int(&project_palette,kParticleSettingsLife,"Life Milliseconds",&particle->life_msec,FALSE);

		// particles

	list_palette_add_header(&project_palette,0,"Particles");
	list_palette_add_int(&project_palette,kParticleParticleStartSize,"Start Size",&particle->start_pixel_size,FALSE);
	list_palette_add_int(&project_palette,kParticleParticleEndSize,"End Size",&particle->end_pixel_size,FALSE);
	list_palette_add_float(&project_palette,kParticleParticleStartGravity,"Start Gravity",&particle->start_gravity,FALSE);
	list_palette_add_float(&project_palette,kParticleParticleGravityAdd,"Gravity Add",&particle->gravity_add,FALSE);
	list_palette_add_float(&project_palette,kParticleParticleStartAlpha,"Start Alpha",&particle->start_alpha,FALSE);
	list_palette_add_float(&project_palette,kParticleParticleEndAlpha,"End Alpha",&particle->end_alpha,FALSE);
	list_palette_add_float(&project_palette,kParticleParticleAmbientFactor,"Ambient Factor",&particle->ambient_factor,FALSE);
	list_palette_add_pick_color(&project_palette,kParticleParticleStartColor,"Start Color",&particle->start_color,FALSE);
	list_palette_add_pick_color(&project_palette,kParticleParticleEndColor,"End Color",&particle->end_color,FALSE);

		// motion

	list_palette_add_header(&project_palette,0,"Motion");
	list_palette_add_point(&project_palette,kParticleMotionOffset,"Spawn Offset",&particle->pt,FALSE);
	list_palette_add_vector(&project_palette,kParticleMotionMove,"Move",&particle->vct,FALSE);
	list_palette_add_angle(&project_palette,kParticleMotionRot,"Rotation",&particle->rot,FALSE);
	list_palette_add_vector(&project_palette,kParticleMotionRotAccel,"Rotation Accel",&particle->rot_accel,FALSE);

		// trail

	list_palette_add_header(&project_palette,0,"Trails");
	list_palette_add_int(&project_palette,kParticleSettingsTrailCount,"Count",&particle->trail_count,FALSE);
	list_palette_add_float(&project_palette,kParticleSettingsTrailStep,"Step",&particle->trail_step,FALSE);
	list_palette_add_float(&project_palette,kParticleSettingsTrailReduceFactor,"Reduction Factor",&particle->reduce_pixel_fact,FALSE);

		// options

	list_palette_add_header(&project_palette,0,"Options");
	list_palette_add_checkbox(&project_palette,kParticleOptionReverse,"Reverse Motion",&particle->reverse,FALSE);
	list_palette_add_checkbox(&project_palette,kParticleOptionBlendAdd,"Additive Blend",&particle->blend_add,FALSE);
	list_palette_add_checkbox(&project_palette,kParticleOptionGlobe,"Globe",&particle->globe,FALSE);

		// animation

	list_palette_add_header(&project_palette,0,"Animation");
	list_palette_add_int(&project_palette,kParticleImageCount,"Count",&particle->animate.image_count,FALSE);
	list_palette_add_int(&project_palette,kParticleImageMillisecond,"Display Milliseconds",&particle->animate.msec,FALSE);
	list_palette_add_checkbox(&project_palette,kParticleImageLoop,"Loop",&particle->animate.loop,FALSE);
	list_palette_add_checkbox(&project_palette,kParticleImageLoopBack,"Loop Back",&particle->animate.loop_back,FALSE);
}

/* =======================================================

      Property Palette Click Particle
      
======================================================= */

void project_palette_click_particle(int particle_idx,bool double_click)
{
}

