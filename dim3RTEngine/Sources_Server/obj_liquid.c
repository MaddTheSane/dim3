/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object In Liquids

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

/* =======================================================

      Liquid Contacts
      
======================================================= */

void object_liquid_contact(obj_type *obj)
{
	int					n,nliquid,sz,
						y,eye_y,f_ty,f_by,f_high,
						lft,rgt,top,bot;
	map_liquid_type		*liq;
	
	obj->liquid.mode=lm_out;
	obj->contact.liquid_idx=-1;

	sz=obj->size.x>>1;
	lft=obj->pnt.x-sz;
	rgt=obj->pnt.x+sz;

	sz=obj->size.z>>1;
	top=obj->pnt.z-sz;
	bot=obj->pnt.z+sz;

	y=obj->pnt.y;

	nliquid=map.liquid.nliquid;
	
	for (n=0;n!=nliquid;n++) {
		liq=&map.liquid.liquids[n];
		
		if (rgt<liq->lft) continue;
		if (lft>liq->rgt) continue;
		if (bot<liq->top) continue;
		if (top>liq->bot) continue;
		
		if ((y>=liq->y) && (y<(liq->y+liq->depth))) {

				// eye offset and floating values

			eye_y=(y+obj->duck.y_move)+obj->size.eye_offset;

			f_high=((int)(((float)obj->size.y)*liquid_float_slop))>>1;
			f_ty=eye_y-f_high;
			f_by=eye_y+f_high;

				// find liquid mode

			if ((liq->y>=f_ty) && (liq->y<=f_by)) {
				if (obj->floating) {
					obj->liquid.mode=lm_float;
				}
				else {
					obj->liquid.mode=lm_in;
				}
			}
			else {
 				if (eye_y>liq->y) {
					obj->liquid.mode=lm_under;
				}
				else {
					obj->liquid.mode=lm_in;
				}
			}

			obj->contact.liquid_idx=n;
        }
	}
}

/* =======================================================

      Get Alter Speed for Liquids
      
======================================================= */

float object_liquid_alter_speed(obj_type *obj)
{
	if ((obj->liquid.mode!=lm_under) && (obj->liquid.mode!=lm_float)) return(1.0f);
	if (obj->contact.liquid_idx==-1) return(1.0f);

	return(map.liquid.liquids[obj->contact.liquid_idx].speed_alter);
}

/* =======================================================

      Objects in Liquid
      
======================================================= */

void object_liquid(obj_type *obj)
{
	int					tick,harm,old_liquid_mode;
	map_liquid_type		*liq;
	
    old_liquid_mode=obj->liquid.mode;
	
	object_liquid_contact(obj);
    
        // leaving liquid
     
    if (obj->contact.liquid_idx==-1) {
        if (old_liquid_mode!=lm_out) scripts_post_event_console(obj->script_idx,-1,sd_event_liquid,sd_event_liquid_out,0);
        return;
    }
	
		// we can't go right from out
		// to floating or under without a in message
		
	if (old_liquid_mode==lm_out) {
		if ((obj->liquid.mode==lm_float) || (obj->liquid.mode==lm_under)) obj->liquid.mode=lm_in;
	}

		// setup bobbing

	tick=game_time_get();

    liq=&map.liquid.liquids[obj->contact.liquid_idx];

	if (obj->liquid.mode!=lm_float) {
		obj->liquid.bob_y_move=0;
	}
	else {
		obj->liquid.bob_y_move=-(int)(liquid_tide_get_high(liq)*0.75f);
	}

        // entering or leaving liquids
        
	switch (obj->liquid.mode) {
	
		case lm_in:
			if (old_liquid_mode==lm_out) {
				obj->status.liquid_harm_count=0;
				scripts_post_event_console(obj->script_idx,-1,sd_event_liquid,sd_event_liquid_in,0);
				break;
			}
			if (old_liquid_mode==lm_under) {
				scripts_post_event_console(obj->script_idx,-1,sd_event_liquid,sd_event_liquid_surface,0);
				object_liquid_jump(obj);				// jump out of liquid when surfacing
				break;
			}
			break;

		case lm_float:
			if (old_liquid_mode!=lm_float) {
				obj->force.vct.y=0;
				obj->force.gravity=gravity_start_power;		// reduce all forces when moving from liquid to floating
			}

			if (old_liquid_mode==lm_under) {
				scripts_post_event_console(obj->script_idx,-1,sd_event_liquid,sd_event_liquid_surface,0);
				object_liquid_jump(obj);				// jump out of liquid when surfacing
			}
			break;
			
		case lm_under:
			if (old_liquid_mode==lm_out) {
				obj->status.liquid_harm_count=0;
			}
			if (old_liquid_mode!=lm_under) {
				obj->status.liquid_under_tick=tick;
				obj->status.liquid_drown_count=0;
				scripts_post_event_console(obj->script_idx,-1,sd_event_liquid,sd_event_liquid_submerge,0);
			}
			break;
			
	}
	
		// no damage objects or non-player/bot objects
		// aren't effected by liquids
				
	if ((!obj->damage.on) || (obj->damage.invincible)) return;
	if ((obj->type!=object_type_player) && (obj->type!=object_type_bot_multiplayer)) return;
	
		// downing

	if (obj->liquid.mode==lm_under) {

		if ((tick-obj->status.liquid_under_tick)>liq->harm.drown_tick) {

			if (obj->status.liquid_drown_count==0) {
				obj->status.liquid_drown_count=100;

				harm=liq->harm.drown_harm;
				if (harm>0) object_damage(obj,NULL,NULL,NULL,NULL,harm);
			}
			else {
				obj->status.liquid_drown_count--;
			}

		}
	}

		// hurting or healing liquids

	if (obj->liquid.mode!=lm_out) {

		if (obj->status.liquid_harm_count==0) {
			obj->status.liquid_harm_count=100;

			harm=liq->harm.in_harm;
			if (harm!=0) {
				if (harm>0) {
					object_damage(obj,NULL,NULL,NULL,NULL,harm);
				}
				else {
					object_heal(obj,-harm);
				}
			}
		}
		else {
			obj->status.liquid_harm_count--;
		}
	}
}

