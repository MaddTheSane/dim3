/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Projectile Hits and Collisions

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
#include "scripts.h"
#include "objects.h"

extern map_type				map;
extern server_type			server;
extern setup_type			setup;

/* =======================================================

      Projectiles Hit
      
======================================================= */

void projectile_hit(proj_type *proj)
{
	int					idx;
    obj_type			*obj,*hurt_obj;
    weapon_type			*weap;
	proj_setup_type		*proj_setup;

		// call projectile hit

	scripts_post_event_console(proj->script_idx,proj->idx,sd_event_hit,0,0);
	
	if (!proj->script_dispose) return;
  
        // object damage

	obj=server.obj_list.objs[proj->obj_idx]; 
	weap=obj->weap_list.weaps[proj->weap_idx];
	proj_setup=weap->proj_setup_list.proj_setups[proj->proj_setup_idx];
	
	idx=proj->contact.obj_idx;
    if (idx!=-1) {
	
			// damage object
			
        hurt_obj=server.obj_list.objs[idx];
		if (proj_setup->damage>=0) {
			object_damage(hurt_obj,obj,weap,proj,NULL,proj_setup->damage);
		}
		else {
			object_heal(obj,-proj_setup->damage);
		}
		
			// push object
			
		if (proj_setup->push.on) object_push(hurt_obj,&proj->pnt,-1,proj_setup->push.force,TRUE);
    }
		
		// dispose of projectile
		
	projectile_dispose(proj);
}

/* =======================================================

      Projectile Auto Hits
      
======================================================= */

bool projectile_hit_auto(proj_type *proj)
{
	if (proj->action.hit_tick==0) return(FALSE);
	if ((proj->start_tick+proj->action.hit_tick)>game_time_get()) return(FALSE);
	
	projectile_hit(proj);
	return(TRUE);
}


