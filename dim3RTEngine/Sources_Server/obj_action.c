/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Actions

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
extern camera_type			camera;
extern network_setup_type	net_setup;

/* =======================================================

      Animations
      
======================================================= */

void object_post_move_animation_event(obj_type *obj,int sub_event)
{
	obj->last_move_animation_event=sub_event;
	scripts_post_event_console(obj->script_idx,-1,sd_event_animation_object,sub_event,0);
}

bool object_post_move_animation_event_change(obj_type *obj,int sub_event)
{
	if (obj->last_move_animation_event==sub_event) return(FALSE);
	
	obj->last_move_animation_event=sub_event;
	scripts_post_event_console(obj->script_idx,-1,sd_event_animation_object,sub_event,0);
	
	return(TRUE);
}

void object_post_turn_animation_event(obj_type *obj,int sub_event)
{
	obj->last_turn_animation_event=sub_event;
	scripts_post_event_console(obj->script_idx,-1,sd_event_animation_object,sub_event,0);
}

bool object_post_turn_animation_event_change(obj_type *obj,int sub_event)
{
	if (obj->last_turn_animation_event==sub_event) return(FALSE);
	
	obj->last_turn_animation_event=sub_event;
	scripts_post_event_console(obj->script_idx,-1,sd_event_animation_object,sub_event,0);
	
	return(TRUE);
}

void object_event_animations(obj_type *obj)
{
	int				last_event;
	bool			moving,forward_moving,side_moving,
					in_air,standing,ducked,ducking,running,forward_reverse,side_reverse;
	
		// current states
		
	forward_moving=obj->forward_move.moving;
	side_moving=obj->side_move.moving;
	moving=forward_moving||side_moving;
	
	in_air=obj->air_mode!=am_ground;
	standing=obj->duck.mode==dm_stand;
	ducked=obj->duck.mode==dm_duck;
	ducking=(obj->duck.mode==dm_stand_up) || (obj->duck.mode==dm_duck_down);
	running=obj->forward_move.running;
	
	forward_reverse=obj->forward_move.reverse;
	side_reverse=obj->side_move.reverse;
	
	last_event=obj->last_move_animation_event;

		// turning movement

	if (obj->turn.ang_add.y==0.0f) {
		object_post_turn_animation_event_change(obj,sd_event_animation_object_turn_stop);
	}
	else {
		if (obj->turn.ang_add.y<0.0f) {
			object_post_turn_animation_event_change(obj,sd_event_animation_object_turn_left);
		}
		else {
			object_post_turn_animation_event_change(obj,sd_event_animation_object_turn_right);
		}
	}
		
		// not moving, only stop events
		
	if (!moving) {

		if ((!in_air) && (!ducking)) {
			object_post_move_animation_event_change(obj,sd_event_animation_object_stop);
			return;
		}

		return;
	}

		// walking

	if ((forward_moving) && (!ducked) && (!running)) {

			// walking forward

		if (!forward_reverse) {

				// walking forward to side

			if (side_moving) {

					// walking forward to left side

				if (!side_reverse) {
					object_post_move_animation_event_change(obj,sd_event_animation_object_walk_left_forward);
					return;
				}

					// walking forward to right side

				else {
					object_post_move_animation_event_change(obj,sd_event_animation_object_walk_right_forward);
					return;
				}
			}

				// walking straight forward

			else {
				object_post_move_animation_event_change(obj,sd_event_animation_object_walk);
				return;
			}
		}

			// walking backwards

		else {

				// walking backwards to side

			if (side_moving) {

					// walking backwards to left side

				if (!side_reverse) {
					object_post_move_animation_event_change(obj,sd_event_animation_object_walk_left_back);
					return;
				}

					// walking backwards to right side

				else {
					object_post_move_animation_event_change(obj,sd_event_animation_object_walk_right_back);
					return;
				}
			}

				// walking straight backwards

			else {
				object_post_move_animation_event_change(obj,sd_event_animation_object_walk_back);
				return;
			}
		}

		return;
	}

		// running

	if ((forward_moving) && (!ducked) && (running)) {

			// running forward

		if (!forward_reverse) {

				// running forward to side

			if (side_moving) {

					// running forward to left side

				if (!side_reverse) {
					object_post_move_animation_event_change(obj,sd_event_animation_object_run_left_forward);
					return;
				}

					// running forward to right side

				else {
					object_post_move_animation_event_change(obj,sd_event_animation_object_run_right_forward);
					return;
				}
			}

				// running straight forward

			else {
				object_post_move_animation_event_change(obj,sd_event_animation_object_run);
				return;
			}
		}

			// running backwards

		else {

				// running backwards to side

			if (side_moving) {

					// running backwards to left side

				if (!side_reverse) {
					object_post_move_animation_event_change(obj,sd_event_animation_object_run_left_back);
					return;
				}

					// running backwards to right side

				else {
					object_post_move_animation_event_change(obj,sd_event_animation_object_run_right_back);
					return;
				}
			}

				// running straight backwards

			else {
				object_post_move_animation_event_change(obj,sd_event_animation_object_run_back);
				return;
			}
		}

		return;
	}

		// crawling

	if ((forward_moving) && (ducked)) {

			// crawling forward

		if (!forward_reverse) {

				// crawling forward to side

			if (side_moving) {

					// crawling forward to left side

				if (!side_reverse) {
					object_post_move_animation_event_change(obj,sd_event_animation_object_crawl_left_forward);
					return;
				}

					// crawling forward to right side

				else {
					object_post_move_animation_event_change(obj,sd_event_animation_object_crawl_right_forward);
					return;
				}
			}

				// crawling straight forward

			else {
				object_post_move_animation_event_change(obj,sd_event_animation_object_crawl);
				return;
			}
		}

			// crawling backwards

		else {

				// crawling backwards to side

			if (side_moving) {

					// crawling backwards to left side

				if (!side_reverse) {
					object_post_move_animation_event_change(obj,sd_event_animation_object_crawl_left_back);
					return;
				}

					// crawling backwards to right side

				else {
					object_post_move_animation_event_change(obj,sd_event_animation_object_crawl_right_back);
					return;
				}
			}

				// crawling straight backwards

			else {
				object_post_move_animation_event_change(obj,sd_event_animation_object_crawl_back);
				return;
			}
		}

		return;
	}

		// left and right only movement

	if ((!forward_moving) && (side_moving)) {

			// crawling left/right

		if (ducked) {

				// crawl left side

			if (!side_reverse) {
				object_post_move_animation_event_change(obj,sd_event_animation_object_crawl_left);
				return;
			}

				// crawl right side

			else {
				object_post_move_animation_event_change(obj,sd_event_animation_object_crawl_right);
				return;
			}
		}

		else {

				// walking left/right

			if (!running) {

					// walk left side

				if (!side_reverse) {
					object_post_move_animation_event_change(obj,sd_event_animation_object_walk_left);
					return;
				}

					// walk right side

				else {
					object_post_move_animation_event_change(obj,sd_event_animation_object_walk_right);
					return;
				}
			}

				// running left/right

			else {

					// running left side

				if (!side_reverse) {
					object_post_move_animation_event_change(obj,sd_event_animation_object_run_left);
					return;
				}

					// running right side

				else {
					object_post_move_animation_event_change(obj,sd_event_animation_object_run_right);
					return;
				}
			}
		}

		return;
	}
}

/* =======================================================

      Jumping
      
======================================================= */

void object_start_jump(obj_type *obj)
{
	poly_pointer_type	*poly_ptr;
	map_mesh_poly_type	*mesh_poly;
	
		// can't jump if flying or not on ground
		
	if ((obj->fly) || (!obj->jump.on)) return;
	if ((obj->liquid.mode==lm_under) || (obj->liquid.mode==lm_float) || (obj->air_mode!=am_ground)) return;
	
		// if current standing slope is too high to climb, you can't jump
		
	if (obj->slope_gravity) {

		poly_ptr=&obj->contact.stand_poly;

		if (poly_ptr->mesh_idx!=-1) {
			mesh_poly=&map.mesh.meshes[poly_ptr->mesh_idx].polys[poly_ptr->poly_idx];
			if (!mesh_poly->box.flat) {
				if (mesh_poly->slope.y>(map.physics.slope_max_ang*slope_angle_to_slope)) return;
			}
		}

	}
	
		// start jump
    
	scripts_post_event_console(obj->script_idx,-1,sd_event_jump,0,0);
    
	obj->force.vct.y=-(float)obj->jump.y_add;
    obj->force.gravity=gravity_start_power;
    
	scripts_post_event_console(obj->script_idx,-1,sd_event_animation_object,sd_event_animation_object_jump,0);
}

void object_liquid_jump(obj_type *obj)
{
		// only jump out of water if
		// touching a polygon

	if (obj->contact.hit_poly.mesh_idx==-1) return;
	
		// jump out of water

	scripts_post_event_console(obj->script_idx,-1,sd_event_jump,0,0);
    
	obj->force.vct.y=-(float)obj->jump.y_add;
    obj->force.gravity=gravity_start_power;
    
	scripts_post_event_console(obj->script_idx,-1,sd_event_animation_object,sd_event_animation_object_jump,0);
}

/* =======================================================

      Ducking
      
======================================================= */

void object_start_stand(obj_type *obj)
{
	int			old_mode;
	bool		stand_ok;
	
	if ((obj->fly) || (!obj->duck.on)) return;
    if ((obj->duck.mode==dm_stand) || (obj->duck.mode==dm_stand_up)) return;
	
		// check if object can stand up

	old_mode=obj->duck.mode;
	obj->duck.mode=dm_stand;

	stand_ok=map_stand_check_object(obj);
	
	obj->duck.mode=old_mode;
	
	if (!stand_ok) return;

		// stand up
    
    obj->duck.mode=dm_stand_up;
	scripts_post_event_console(obj->script_idx,-1,sd_event_animation_object,sd_event_animation_object_stand_up,0);
}

void object_start_duck(obj_type *obj)
{
	if ((obj->fly) || (!obj->duck.on)) return;
	if ((obj->liquid.mode==lm_under) || (obj->liquid.mode==lm_float) || (obj->air_mode!=am_ground)) return;
    if ((obj->duck.mode==dm_duck) || (obj->duck.mode==dm_duck_down)) return;

    obj->duck.mode=dm_duck_down;
	scripts_post_event_console(obj->script_idx,-1,sd_event_animation_object,sd_event_animation_object_duck_down,0);
}

void object_ducking(obj_type *obj)
{
    int			duck_mode,y_change;
	
    duck_mode=obj->duck.mode;
    if ((duck_mode==dm_stand) || (duck_mode==dm_duck)) return;
    
    if (duck_mode==dm_stand_up) {
        obj->duck.y_move-=obj->duck.y_add;
        if (obj->duck.y_move>0) return;
        
        obj->duck.y_move=0;
        obj->duck.mode=dm_stand;
        object_post_move_animation_event(obj,sd_event_animation_object_standing);
        return;
    }
    
    if (duck_mode==dm_duck_down) {
        obj->duck.y_move+=obj->duck.y_add;
        y_change=obj->duck.y_change;
        if (obj->duck.y_move<y_change) return;
        
        obj->duck.y_move=y_change;
        obj->duck.mode=dm_duck;
        object_post_move_animation_event(obj,sd_event_animation_object_ducking);
        return;
    }
}

/* =======================================================

      Attach Sounds To Objects
      
======================================================= */

void object_set_ambient(obj_type *obj,char *name,float pitch)
{
	strcpy(obj->ambient.name,name);
	obj->ambient.pitch=pitch;
	obj->ambient.on=TRUE;

	obj->ambient.buffer_idx=al_find_buffer(name);
}

void object_change_ambient_pitch(obj_type *obj,float pitch)
{
	obj->ambient.pitch=pitch;
}

void object_clear_ambient(obj_type *obj)
{
	obj->ambient.on=FALSE;
}

/* =======================================================

      Enter/Exit Vehicles
      
======================================================= */

bool object_enter_vehicle(obj_type *obj,int vehicle_idx,char *err_str)
{
	int				n,x,z,y,sz;
	d3pnt			motion;
	obj_type		*vehicle_obj;
	obj_vehicle		*vehicle;

		// can this object use vehicles?

	if (!obj->vehicle.use_vehicles) {
		if (err_str!=NULL) strcpy(err_str,"Object is flagged to not be able to enter vehicles");
		return(FALSE);
	}

		// if vehicle_idx is -1, then
		// find a vehicle within right distance
		// if we do this, we mark it so exiting
		// causes the original obj to be reset
		// to an exit position

	obj->vehicle.offset_exit=FALSE;
	
	if (vehicle_idx==-1) {

		obj->vehicle.offset_exit=TRUE;
	
		sz=(obj->size.z*3)>>1;
		angle_get_movement(obj->motion.ang.y,obj->size.z,&motion.x,&motion.z);
		
		motion.y=0;
		
		for (n=0;n!=max_obj_list;n++) {
			vehicle_obj=server.obj_list.objs[n];
			if (vehicle_obj==NULL) continue;
	    
			if ((vehicle_obj->hidden) || (!vehicle_obj->contact.object_on) || (!vehicle_obj->vehicle.on) || (vehicle_obj->idx==obj->idx)) continue;

				// check bounds
				
			if (collide_object_to_object(obj,&motion,vehicle_obj,FALSE)) {
				vehicle_idx=n;
				break;
			}
		}
	}

	if (vehicle_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"No object to enter");
		return(FALSE);
	}

	vehicle_obj=server.obj_list.objs[vehicle_idx];
	if (!vehicle_obj->vehicle.on) {
		if (err_str!=NULL) strcpy(err_str,"Object is not a vehicle");
		return(FALSE);
	}
	
		// can't enter a vehicle already entered
		
	vehicle=&vehicle_obj->vehicle;

	if (vehicle->attach_obj_idx!=-1) {
		if (err_str!=NULL) strcpy(err_str,"Vehicle has already been entered");
		return(FALSE);
	}

		// get enter offset

	if (obj->vehicle.offset_exit) {
		x=vehicle_obj->pnt.x-obj->pnt.x;
		z=vehicle_obj->pnt.z-obj->pnt.z;
		y=vehicle_obj->pnt.y-obj->pnt.y;
			
		rotate_2D_point_center(&x,&z,-vehicle_obj->ang.y);
			
		vehicle->attach_offset.x=x;
		vehicle->attach_offset.z=z;
		vehicle->attach_offset.y=y;
	}

		// send events

	obj->vehicle.in_enter=TRUE;
	obj->vehicle.in_exit=FALSE;
		
	scripts_post_event_console(obj->script_idx,-1,sd_event_vehicle,sd_event_vehicle_enter,0);
	scripts_post_event_console(vehicle_obj->script_idx,-1,sd_event_vehicle,sd_event_vehicle_enter,0);

	obj->vehicle.in_enter=FALSE;

		// special check to see if exit
		// was called from events, if so back out

	if (obj->vehicle.in_exit) {
		obj->vehicle.in_exit=FALSE;
		return(TRUE);
	}

		// hide the object
		
	obj->hidden=TRUE;
	object_stop(obj);
	
		// attach object

	vehicle->attach_obj_idx=obj->idx;

		// if this object was the camera object, then reconnect

	if (camera.obj_idx==obj->idx) camera_connect(vehicle_obj);
	
		// if this object was the player object, move to vehicle

	if (server.player_obj_idx==obj->idx) {
		vehicle_obj->type=object_type_player;
		obj->type=object_type_object;
		server.player_obj_idx=vehicle_obj->idx;
	}

	return(TRUE);
}

bool object_exit_vehicle(obj_type *vehicle_obj,bool ignore_errors,char *err_str)
{
	int						x,z,y,radius;
	bool					empty;
	d3pnt					spt,ept;
	obj_type				*orig_obj;
	obj_vehicle				*vehicle;
	ray_trace_contact_type	contact;

		// if we are inside a vehicle enter
		// event, then we need to just trigger
		// an exit and leave so the state doesn't
		// get messed up

	if (vehicle_obj->vehicle.in_enter) {
		vehicle_obj->vehicle.in_exit=TRUE;
		return(TRUE);
	}

		// don't exit moving vehicles

	if (!ignore_errors) {
		if ((vehicle_obj->forward_move.moving) || (vehicle_obj->side_move.moving) || (vehicle_obj->vert_move.moving)) {
			if (err_str!=NULL) strcpy(err_str,"Can not exit moving vehicle");
			return(FALSE);
		}
	}

		// get original object
	
	orig_obj=server.obj_list.objs[vehicle_obj->vehicle.attach_obj_idx];
	
		// find exit point
		// if we entered directly, we just leave the object
		// where it was
		
	vehicle=&vehicle_obj->vehicle;
	
	if (orig_obj->vehicle.offset_exit) {
		x=vehicle->attach_offset.x;
		z=vehicle->attach_offset.z;
		y=vehicle->attach_offset.y;
		
		rotate_2D_point_center(&x,&z,vehicle_obj->ang.y);
		
		orig_obj->pnt.x=vehicle_obj->pnt.x-x;
		orig_obj->pnt.z=vehicle_obj->pnt.z-z;
		orig_obj->pnt.y=vehicle_obj->pnt.y-y;

		orig_obj->ang.y=angle_find(orig_obj->pnt.x,orig_obj->pnt.z,vehicle_obj->pnt.x,vehicle_obj->pnt.z);
	
			// is there enough empty space to exit?
			// go from center of vehicle to obj + radius

		spt.x=vehicle_obj->pnt.x;
		spt.y=vehicle_obj->pnt.y;
		spt.z=vehicle_obj->pnt.z;

		ept.x=orig_obj->pnt.x;
		ept.y=orig_obj->pnt.y;
		ept.z=orig_obj->pnt.z;

		radius=object_get_radius(orig_obj);
		ray_push_to_end(&ept,&spt,-radius);
		
		contact.obj.on=TRUE;
		contact.proj.on=FALSE;
		contact.obj.ignore_idx=vehicle_obj->idx;

		contact.origin=poly_ray_trace_origin_object;
		
		empty=!ray_trace_map_by_point(&spt,&ept,&contact);
		
		if ((!empty) && (!ignore_errors)) {
			if (err_str!=NULL) strcpy(err_str,"No space in map to exit");
			return(FALSE);
		}
	}

		// send events
		
	scripts_post_event_console(vehicle_obj->script_idx,-1,sd_event_vehicle,sd_event_vehicle_exit,0);
	scripts_post_event_console(orig_obj->script_idx,-1,sd_event_vehicle,sd_event_vehicle_exit,0);
	
		// exit
	
	vehicle->attach_obj_idx=-1;
	object_stop(vehicle_obj);

		// if this vehicle was the camera object, then reconnect

	if (camera.obj_idx==vehicle_obj->idx) camera_connect(orig_obj);

		// if this vehicle was the player object, move to object

	if (server.player_obj_idx==vehicle_obj->idx) {
		vehicle_obj->type=object_type_object;
		orig_obj->type=object_type_player;
		server.player_obj_idx=orig_obj->idx;
	}

		// unhide the original object
		
	orig_obj->hidden=FALSE;

	return(TRUE);
}
