/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Item Routines

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

extern server_type			server;
extern network_setup_type	net_setup;

/* =======================================================

      Pick Up Item
      
======================================================= */

bool item_pickup(obj_type *obj,obj_type *item_obj)
{
		// setup pickup for items
		
	item_obj->pickup.item_idx=-1;
	item_obj->pickup.obj_idx=obj->idx;
	item_obj->pickup.canceled=FALSE;

		// setup pickup for objects

	obj->pickup.item_idx=item_obj->idx;
	obj->pickup.obj_idx=-1;

		// send pickup event to item
		// and exit if cancelled

	scripts_post_event_console(item_obj->script_idx,-1,sd_event_pickup,0,0);
	if (item_obj->pickup.canceled) return(FALSE);

		// send pickup event to object
		
	scripts_post_event_console(obj->script_idx,-1,sd_event_pickup,0,0);
			
		// successfully picked up
				
	obj->item_count=50;
	
	return(TRUE);
}

/* =======================================================

      Check for Object Picking Up Item
      
======================================================= */

void item_pickup_check(obj_type *obj)
{
	int					n;
	obj_type			*item_obj;

		// pickups happen on host, so clients
		// ignore this

	if (net_setup.mode==net_mode_client) return;
	
		// can pick up if you are hidden, no contact,
		// set to ignore pickup, or are a pickup item
		// yourself
		
	if (obj->hidden) return;
	if (!obj->contact.object_on) return;
	if (obj->pickup.on) return;
	if (obj->pickup.ignore) return;

		// dead players can't pickup items

	if ((obj->type==object_type_player) && (obj->status.health.value==0)) return;

		// check for collisions with pickup items
    
	for (n=0;n!=max_obj_list;n++) {
		item_obj=server.obj_list.objs[n];
		if (item_obj==NULL) continue;
    
		if ((item_obj->hidden) || (!item_obj->pickup.on) || (!item_obj->contact.object_on) || (item_obj->idx==obj->idx)) continue;

            // check bounds
			
		if (collide_object_to_object(obj,NULL,item_obj,FALSE)) {
		
				// run the pickup
				
			if (!item_pickup(obj,item_obj)) continue;
	
				// if this is host, then we need
				// to update the remotes stats

			if (net_setup.mode==net_mode_host) {
				if (obj->type==object_type_remote_player) {
					net_host_player_send_pickup(obj,item_obj);
				}
			}
		}
	}
}

/* =======================================================

      Item Actions
      
======================================================= */

bool item_add_weapon(obj_type *obj,weapon_type *weap)
{
		// already own weapon

	if (!weap->hidden) {
	
			// can we pick up a second one?

		if (!weap->dual.on) return(FALSE);
		if (weap->dual.active) return(FALSE);

			// pick it up
		
		weap->dual.active=TRUE;

		return(TRUE);
	}

		// add the weapon

	weap->hidden=FALSE;
	weapon_reset_ammo(weap);
	
	return(TRUE);
}

bool item_swap_weapon(obj_type *obj,weapon_type *weap)
{
		// add the weapon

	if (!item_add_weapon(obj,weap)) return(FALSE);

		// set new weapon

	weapon_goto(obj,weap);
	
	return(TRUE);
}

bool item_add_ammo(obj_type *obj,weapon_type *weap,int add_count)
{
	if (weap->hidden) return(FALSE);
	if (weap->ammo.count==weap->ammo.max_count) return(FALSE);
	
    weap->ammo.count+=add_count;
    if (weap->ammo.count>weap->ammo.max_count) weap->ammo.count=weap->ammo.max_count;

	scripts_post_event_console(weap->script_idx,-1,sd_event_animation_weapon,sd_event_animation_weapon_add_ammo,0);

	return(TRUE);
}


bool item_add_clip(obj_type *obj,weapon_type *weap,int add_count)
{
	if (weap->hidden) return(FALSE);
	if (!weap->ammo.use_clips) return(FALSE);
	if (weap->ammo.clip_count==weap->ammo.max_clip_count) return(FALSE);
    
    weap->ammo.clip_count+=add_count;
	if (weap->ammo.clip_count>weap->ammo.max_clip_count) weap->ammo.clip_count=weap->ammo.max_clip_count;

	scripts_post_event_console(weap->script_idx,-1,sd_event_animation_weapon,sd_event_animation_weapon_add_clip,0);

	return(TRUE);
}

bool item_add_alt_ammo(obj_type *obj,weapon_type *weap,int add_count)
{
	if (weap->hidden) return(FALSE);
    if (weap->alt_ammo.count==weap->alt_ammo.max_count) return(FALSE);

    weap->alt_ammo.count+=add_count;
    if (weap->alt_ammo.count>weap->alt_ammo.max_count) weap->alt_ammo.count=weap->alt_ammo.max_count;

	scripts_post_event_console(weap->script_idx,-1,sd_event_animation_weapon,sd_event_animation_weapon_add_alt_ammo,0);

	return(TRUE);
}

bool item_add_alt_clip(obj_type *obj,weapon_type *weap,int add_count)
{
	if (weap->hidden) return(FALSE);
	if (!weap->alt_ammo.use_clips) return(FALSE);
	if (weap->alt_ammo.clip_count==weap->alt_ammo.max_clip_count) return(FALSE);
   
    weap->alt_ammo.clip_count+=add_count;
    if (weap->alt_ammo.clip_count>weap->alt_ammo.max_clip_count) weap->alt_ammo.clip_count=weap->alt_ammo.max_clip_count;

	scripts_post_event_console(weap->script_idx,-1,sd_event_animation_weapon,sd_event_animation_weapon_add_alt_clip,0);

	return(TRUE);
}

bool item_add_health(obj_type *obj,int add_count)
{
	return(object_heal(obj,add_count));
}

bool item_add_armor(obj_type *obj,int add_count)
{
	return(object_heal_armor(obj,add_count));
}

void item_add_custom(obj_type *obj,int custom_id)
{
	scripts_post_event_console(obj->script_idx,-1,sd_event_add_custom,0,custom_id);
}
