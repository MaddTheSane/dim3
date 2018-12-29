/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Particles

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

#define kParticlePropertyAdd					0
#define kParticlePropertyGroupAdd				1

#define kParticleProperyName					1000
#define kParticleProperyDelete					2000

extern iface_type				iface;
extern app_state_type			state;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Particles
      
======================================================= */

void project_palette_fill_particles(void)
{
	int						n;

	list_palette_set_title(&project_palette,"Particles",NULL,NULL,NULL,NULL,NULL);

		// regular particles

	list_palette_add_header_button(&project_palette,kParticlePropertyAdd,"Particles",list_button_plus);

	list_palette_sort_mark_start(&project_palette,&project_palette.item_pane);
	
	for (n=0;n!=iface.particle_list.nparticle;n++) {
		if (!iface.particle_list.particles[n].group.on) list_palette_add_string_selectable_button(&project_palette,(kParticleProperyName+n),list_button_minus,(kParticleProperyDelete+n),iface.particle_list.particles[n].name,(state.proj.cur_particle_idx==n),FALSE);
	}

	list_palette_sort(&project_palette,&project_palette.item_pane);

		// group particles

	list_palette_add_header_button(&project_palette,kParticlePropertyGroupAdd,"Particle Groups",list_button_plus);

	list_palette_sort_mark_start(&project_palette,&project_palette.item_pane);
	
	for (n=0;n!=iface.particle_list.nparticle;n++) {
		if (iface.particle_list.particles[n].group.on) list_palette_add_string_selectable_button(&project_palette,(kParticleProperyName+n),list_button_minus,(kParticleProperyDelete+n),iface.particle_list.particles[n].name,(state.proj.cur_particle_idx==n),FALSE);
	}

	list_palette_sort(&project_palette,&project_palette.item_pane);
}

/* =======================================================

      Property Palette Click Particles
      
======================================================= */

void project_palette_click_particles(bool double_click)
{
	int					id,idx,sz;

	id=project_palette.item_pane.click.id;

		// particle edit
		
	if ((id>=kParticleProperyName) && (id<(kParticleProperyName+max_iface_particle))) {
		state.proj.cur_particle_idx=id-kParticleProperyName;
		state.proj.cur_group_particle_idx=-1;
		if (double_click) list_palette_set_level(&project_palette,2);
		return;
	}
	
		// particle delete
		
	if ((id>=kParticleProperyDelete) && (id<(kParticleProperyDelete+max_iface_particle))) {
		state.proj.cur_particle_idx=-1;
		state.proj.cur_group_particle_idx=-1;

		idx=id-kParticleProperyDelete;

		sz=(iface.particle_list.nparticle-idx)-1;
		if (sz>0) memmove(&iface.particle_list.particles[idx],&iface.particle_list.particles[idx+1],(sz*sizeof(iface_particle_type)));

		iface.particle_list.nparticle--;

		return;
	}
	
		// particle add

	if ((id==kParticlePropertyAdd) || (id==kParticlePropertyGroupAdd)) {
		state.proj.cur_particle_idx=-1;
		state.proj.cur_group_particle_idx=-1;

		if (iface.particle_list.nparticle>=max_iface_particle) {
			os_dialog_alert("Add Particle","Reached the maximum number of particles");
			return;
		}

		idx=iface.particle_list.nparticle;
		iface.particle_list.nparticle++;

		iface.particle_list.particles[idx].group.on=(id==kParticlePropertyGroupAdd);
		iface.particle_list.particles[idx].group.count=0;

		iface.particle_list.particles[idx].name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)iface.particle_list.particles[idx].name,name_str_len,0,0);
	
		iface.particle_list.particles[idx].count=20;
		iface.particle_list.particles[idx].trail_count=0;
		iface.particle_list.particles[idx].life_msec=1000;
		iface.particle_list.particles[idx].start_pixel_size=100;
		iface.particle_list.particles[idx].end_pixel_size=100;

		iface.particle_list.particles[idx].start_gravity=0.0f;
		iface.particle_list.particles[idx].gravity_add=0.0f;
		iface.particle_list.particles[idx].start_alpha=1.0f;
		iface.particle_list.particles[idx].end_alpha=1.0f;
		iface.particle_list.particles[idx].reduce_pixel_fact=1.0f;
		iface.particle_list.particles[idx].trail_step=0.0f;
		iface.particle_list.particles[idx].ambient_factor=1.0f;

		iface.particle_list.particles[idx].bitmap_name[0]=0x0;
		iface.particle_list.particles[idx].chain_name[0]=0x0;

		iface.particle_list.particles[idx].reverse=FALSE;
		iface.particle_list.particles[idx].blend_add=TRUE;
		iface.particle_list.particles[idx].globe=FALSE;

		iface.particle_list.particles[idx].start_color.r=1.0f;
		iface.particle_list.particles[idx].start_color.g=1.0f;
		iface.particle_list.particles[idx].start_color.b=1.0f;
		iface.particle_list.particles[idx].end_color.r=1.0f;
		iface.particle_list.particles[idx].end_color.g=1.0f;
		iface.particle_list.particles[idx].end_color.b=1.0f;

		iface.particle_list.particles[idx].pt.x=0;
		iface.particle_list.particles[idx].pt.y=0;
		iface.particle_list.particles[idx].pt.z=0;

		iface.particle_list.particles[idx].vct.x=100.0f;
		iface.particle_list.particles[idx].vct.y=100.0f;
		iface.particle_list.particles[idx].vct.z=100.0f;

		iface.particle_list.particles[idx].rot.x=0.0f;
		iface.particle_list.particles[idx].rot.y=0.0f;
		iface.particle_list.particles[idx].rot.z=0.0f;

		iface.particle_list.particles[idx].rot_accel.x=0.0f;
		iface.particle_list.particles[idx].rot_accel.y=0.0f;
		iface.particle_list.particles[idx].rot_accel.z=0.0f;

		iface.particle_list.particles[idx].animate.image_count=1;
		iface.particle_list.particles[idx].animate.msec=0;
		iface.particle_list.particles[idx].animate.loop=FALSE;
		iface.particle_list.particles[idx].animate.loop_back=FALSE;

		state.proj.cur_particle_idx=idx;

		return;
	}

}

