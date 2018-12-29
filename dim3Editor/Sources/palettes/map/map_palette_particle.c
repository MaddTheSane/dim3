/****************************** File *********************************

Module: dim3 Editor
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

#define kParticleOn								0
#define kParticlePropertyName					1
#define kParticlePropertySpawnTick				2
#define kParticlePropertySlopTick				3
#define kParticlePropertySingleSpawn			4

#define kLightPropertyOn						10
#define kLightPropertyLightMap					11
#define kLightPropertyType						12
#define kLightPropertyDirection					13
#define kLightPropertyIntensity					14
#define kLightPropertyExponent					15
#define kLightPropertyColor						16
#define kLightPropertyHalo						17

extern map_type					map;
extern iface_type				iface;

extern list_palette_type		map_palette;

extern char						light_map_type_list[][name_str_len],
								light_map_direction_list[][name_str_len];

int								pal_particle_index;

/* =======================================================

      Property Palette Fill Particle
      
======================================================= */

void map_palette_fill_particle(int particle_idx)
{
	map_particle_type		*particle;

	particle=&map.particles[particle_idx];

	list_palette_set_title(&map_palette,"Particle",particle->name,NULL,NULL,NULL,NULL);

		// particle
		
	list_palette_add_header(&map_palette,0,"Particle Settings");
	list_palette_add_checkbox(&map_palette,kParticleOn,"On",&particle->on,FALSE);

	list_palette_add_header(&map_palette,0,"Particle Type");
	list_palette_add_picker_list_string(&map_palette,kParticlePropertyName,"Name",(char*)iface.particle_list.particles,iface.particle_list.nparticle,sizeof(iface_particle_type),(int)offsetof(iface_particle_type,name),TRUE,TRUE,particle->name,FALSE);
	list_palette_add_int(&map_palette,kParticlePropertySpawnTick,"Spawn Tick",&particle->spawn_tick,FALSE);
	list_palette_add_int(&map_palette,kParticlePropertySlopTick,"Slop Tick",&particle->slop_tick,FALSE);
	list_palette_add_checkbox(&map_palette,kParticlePropertySingleSpawn,"Single Spawn",&particle->single_spawn,FALSE);

		// particle lighting
		
	list_palette_add_header(&map_palette,0,"Particle Light Settings");
	list_palette_add_checkbox(&map_palette,kLightPropertyOn,"On",&particle->light_setting.on,FALSE);
	list_palette_add_checkbox(&map_palette,kLightPropertyLightMap,"Used in Light Map",&particle->light_setting.light_map,FALSE);

	list_palette_add_header(&map_palette,0,"Particle Light Display");
	list_palette_add_picker_list_int(&map_palette,kLightPropertyType,"Type",(char*)light_map_type_list,-1,name_str_len,0,FALSE,FALSE,&particle->light_setting.type,FALSE);
	list_palette_add_picker_list_int(&map_palette,kLightPropertyDirection,"Direction",(char*)light_map_direction_list,-1,name_str_len,0,FALSE,FALSE,&particle->light_setting.direction,FALSE);
	list_palette_add_int(&map_palette,kLightPropertyIntensity,"Intensity",&particle->light_setting.intensity,FALSE);
	list_palette_add_float(&map_palette,kLightPropertyExponent,"Exponent",&particle->light_setting.exponent,FALSE);
	list_palette_add_pick_color(&map_palette,kLightPropertyColor,"Color",&particle->light_setting.col,FALSE);
	list_palette_add_picker_list_string(&map_palette,kLightPropertyHalo,"Halo",(char*)iface.halo_list.halos,iface.halo_list.nhalo,sizeof(iface_halo_type),(int)offsetof(iface_halo_type,name),TRUE,TRUE,particle->light_setting.halo_name,FALSE);
	
	pal_particle_index=particle_idx;
	
	list_palette_add_header(&map_palette,0,"Paticle Info");
	list_palette_add_int(&map_palette,-1,"Index",&pal_particle_index,TRUE);
	list_palette_add_point(&map_palette,-1,"Position",&particle->pnt,TRUE);
}

/* =======================================================

      Property Palette Click Particle
      
======================================================= */

void map_palette_click_particle(int particle_idx,bool double_click)
{
}

