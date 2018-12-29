/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Projectile Hit Scans

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
#include "scripts.h"
#include "objects.h"

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern network_setup_type	net_setup;

/* =======================================================

      HitScan Projectile																																			
      
======================================================= */

void projectile_hitscan(obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,d3pnt *pt,d3ang *ang)
{
	d3pnt					spt;
	proj_type				*proj,*hit_proj;
	ray_trace_contact_type	contact;

		// setup projectile
		
	proj=projectile_create(obj,weap,proj_setup,TRUE);
    if (proj==NULL) return;

	projectile_set_position(proj,pt,ang);
	projectile_set_origin(proj);
	memmove(&proj->size,&proj_setup->size,sizeof(obj_size));
	
		// call spawn
		
	scripts_post_event_console(proj->script_idx,proj->idx,sd_event_spawn,sd_event_spawn_init,0);
		
		// if this object is the player or multiplayer\map bot,
		// then spawn projectile in remotes
		
	if (net_setup.mode!=net_mode_none) {
		if (object_networkable(obj)) net_client_send_hitscan_add(obj,weap->name,proj_setup->name,pt,ang);
	}
	
		// hitscan
		
	spt.x=proj->pnt.x;
	spt.y=proj->pnt.y;
	spt.z=proj->pnt.z;
	
	contact.obj.on=TRUE;
	contact.proj.on=proj_setup->collision;
	contact.obj.ignore_idx=obj->idx;
	contact.proj.ignore_idx=proj->idx;

	contact.origin=poly_ray_trace_origin_projectile;
	
	ray_trace_map_by_angle(&spt,ang,proj_setup->hitscan.max_dist,&contact);

		// reset position and contacts
		
	proj->pnt.x=contact.hpt.x;
	proj->pnt.y=contact.hpt.y;
	proj->pnt.z=contact.hpt.z;
	
	memmove(&proj->contact.hit_poly,&contact.poly,sizeof(poly_pointer_type));
	proj->contact.liquid_idx=-1;

	proj->contact.obj_idx=contact.obj.idx;
	proj->contact.proj_idx=contact.proj.idx;
	proj->contact.melee=FALSE;
	
		// projectile collisions force other projectile into hit mode
			
	if (contact.proj.idx!=-1) {
		hit_proj=server.proj_list.projs[contact.proj.idx];
		hit_proj->contact.proj_idx=proj->idx;
		projectile_hit(hit_proj);
	}

		// hit projectile
			
	projectile_hit(proj);
}
