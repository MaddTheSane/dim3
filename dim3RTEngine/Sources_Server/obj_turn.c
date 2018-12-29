/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Turning

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
#include "objects.h"

// supergumba -- these should eventually be settable
#define auto_aim_max_ray_trace_distance			150000
#define auto_aim_max_catch_angle				8.0f
#define auto_aim_turn_speed_factor				0.25f

extern iface_type			iface;
extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern camera_type			camera;

/* =======================================================

      Find Turn Speeds for Object
      
======================================================= */

float object_get_turn_speed(obj_type *obj)
{
		// ground speeds
		
    if (obj->air_mode==am_ground) {
		if (obj->duck.mode==dm_duck) return(obj->turn.crawl_speed);
		if (obj->forward_move.running) return(obj->turn.run_speed);
		return(obj->turn.walk_speed);
    }
	
		// air speeds
		
	return(obj->turn.air_speed);
}

float object_get_turn_motion(obj_type *obj)
{
		// ground speeds
		
    if (obj->air_mode==am_ground) {
		if (obj->duck.mode==dm_duck) return(obj->turn.crawl_motion_speed);
		if (obj->forward_move.running) return(obj->turn.run_motion_speed);
		return(obj->turn.walk_motion_speed);
    }
	
		// air speed
	
	return(obj->turn.air_motion_speed);
}

/* =======================================================

      Object Turning
      
======================================================= */

void object_turn(obj_type *obj)
{
	float		ang,rang,turn_add,subway,addway;
	
	turn_add=obj->turn.ang_add.y;
	if (turn_add==0) return;
	
	ang=obj->ang.y;
	if (obj->turn.ang_to.y==ang) return;
	
	rang=obj->turn.ang_to.y;
	
	if (ang>rang) {
		subway=ang-rang;
		addway=360-(ang-rang);
	}
	else {
		subway=360-(rang-ang);
		addway=rang-ang;
	}
	
	if (turn_add<0) {
		if (fabsf(turn_add)>=subway) {
			obj->ang.y=rang;
			return;
		}
	}
	else {
		if (turn_add>=addway) {
			obj->ang.y=rang;
			return;
		}
	}
	
		// turn object

	obj->ang.y=angle_add(ang,turn_add);

		// turn any standing objects

	object_rotate_with_standing_object(obj,turn_add);
}

void object_turn_stop(obj_type *obj)
{
	obj->turn.ang_to.y=(float)obj->pnt.y;
	obj->turn.ang_add.y=0.0f;

	object_auto_walk_stop(obj);
}

void object_fix_motion(obj_type *obj)
{
	float			turn_max;
    d3ang			*ang;
	
		// thrust movement ignores all turning motion
		
	if (obj->input.mode==im_thrust) {
		obj->motion.ang.y=obj->ang.y;
		return;
	}

		// fix motion to facing angle
	
    ang=&obj->ang;
	if (ang->y==obj->motion.ang.y) return;

		// if no motion whatsoever, auto reset motion angle

	if ((obj->forward_move.speed==0.0f) && (obj->side_move.speed==0.0f)) {
		obj->motion.ang.y=ang->y;
		return;
	}

		// no motion changes when flying and no thrust

	if ((obj->fly) && (!obj->forward_move.moving) && (!obj->side_move.moving)) return;

		// fix motion angle to facing angle
		
	turn_max=object_get_turn_motion(obj)*map.physics.resistance;
    obj->motion.ang.y=angle_turn_toward(obj->motion.ang.y,ang->y,turn_max);
}

/* =======================================================

      Object Facing
      
======================================================= */

void object_face(obj_type *obj)
{
	int				dist,py,ty;
	float			to_ang;
	obj_type		*track_obj;

		// no tracking, turn off facing

	if (obj->face.obj_idx==-1) {
		if (obj->fly) if (obj->face.ang.x!=0.0f) obj->face.ang.x=angle_turn_toward(obj->face.ang.x,0.0f,obj->turn.walk_speed);
		if (obj->face.ang.y!=0.0f) obj->face.ang.y=angle_turn_toward(obj->face.ang.y,0.0f,obj->turn.walk_speed);
		return;
	}

		// track object

	track_obj=server.obj_list.objs[obj->face.obj_idx];
	
	if (obj->fly) {
		py=obj->pnt.y-(obj->size.y>>1);
		ty=track_obj->pnt.y-(track_obj->size.y>>1);

		dist=distance_2D_get(obj->pnt.x,obj->pnt.z,track_obj->pnt.x,track_obj->pnt.z);
		to_ang=angle_find(py,0,ty,dist)-180.0f;

		to_ang=angle_add(to_ang,-obj->ang.x);
		if (obj->face.ang.x!=to_ang) obj->face.ang.x=angle_turn_toward(obj->face.ang.x,to_ang,obj->turn.walk_speed);
	}

	to_ang=angle_find(obj->pnt.x,obj->pnt.z,track_obj->pnt.x,track_obj->pnt.z);
	to_ang=angle_add(to_ang,-obj->ang.y);
	if (obj->face.ang.y!=to_ang) obj->face.ang.y=angle_turn_toward(obj->face.ang.y,to_ang,obj->turn.walk_speed);
}

void object_face_object_start(obj_type *obj,obj_type *track_obj)
{
	obj->face.obj_idx=track_obj->idx;
}

void object_face_stop(obj_type *obj)
{
	obj->face.obj_idx=-1;
}

/* =======================================================

      Player Turning
      
======================================================= */

void object_player_turn(obj_type *obj)
{
	float			turn_add,to_ang,turn_restrict;
	obj_type		*track_obj;
	weapon_type		*weap;
	
		// can it turn?
		
	if ((obj->turn.only_when_moving) && (!obj->forward_move.moving)) {
		obj->turn.ang_add.y=0;
		return;
	}
	
		// get turning, if facing
		// is on, force to always face object

	obj->turn.fix_ang_add.y=0;

	if (obj->face.obj_idx==-1) {
		turn_add=obj->turn.ang_add.y;
	}
	else {
		track_obj=server.obj_list.objs[obj->face.obj_idx];
		to_ang=angle_find(obj->pnt.x,obj->pnt.z,track_obj->pnt.x,track_obj->pnt.z);
		turn_add=angle_add(to_ang,-obj->ang.y);
	}

    if (turn_add==0) return;
	
		// zoom factors

	if (map.camera.camera_mode==cv_fpp) {
		weap=weapon_find_current(obj);
		if (weap!=NULL) {
			if ((weap->zoom.on) && (weap->zoom.mode!=zoom_mode_off)) {
				if (obj->duck.mode!=dm_duck) {
					turn_add*=weap->zoom.turn_factor;
				}
				else {
					turn_add*=weap->zoom.crawl_turn_factor;
				}
			}
		}
	}
	
		// restriction

	if (obj->turn.restrict_player_turning) {
		turn_restrict=object_get_turn_speed(obj);
		
		if (turn_add<-turn_restrict) turn_add=-turn_restrict;
		if (turn_add>turn_restrict) turn_add=turn_restrict;
	}

		// turning
		
    obj->ang.y=angle_add(obj->ang.y,turn_add);
	obj->turn.fix_ang_add.y=turn_add;

		// turn any standing objects

	object_rotate_with_standing_object(obj,turn_add);
}

void object_player_turn_direct(obj_type *obj,float ang)
{
		// turned around?
		
	if (obj->ang.y!=ang) obj->forward_move.speed=0;

		// face correct direction
		
	obj->forward_move.moving=TRUE;
	obj->forward_move.reverse=FALSE;
	obj->turn.ang_add.y=0.0f;
	obj->ang.y=obj->motion.ang.y=ang;
}

void object_player_turn_auto_aim(obj_type *obj)
{
	d3pnt					pnt;
	d3ang					ang;
	weapon_type				*weap;
	ray_trace_contact_type	contact;

	if ((!iface.setup.allow_auto_aim) || (!setup.auto_aim)) return;

		// get weapon

	weap=weapon_find_current(obj);
	if (weap==NULL) return;

		// angle to crosshair

	pnt.x=obj->pnt.x;
	pnt.y=(obj->pnt.y+obj->duck.y_move)+obj->size.eye_offset;
	pnt.z=obj->pnt.z;

	ang.x=-obj->view_ang.x;
	ang.y=obj->ang.y;
	ang.z=0.0f;

		// check if any objects
		// are in current crosshair
		// if so, just return

	contact.obj.on=TRUE;
	contact.obj.ignore_idx=obj->idx;
	contact.proj.on=FALSE;
	contact.origin=poly_ray_trace_origin_object;

	ray_trace_map_by_angle(&pnt,&ang,auto_aim_max_ray_trace_distance,&contact);
	if (contact.obj.idx!=-1) return;

		// turn towards near objects
		// only if we are turning in that direction

		// check slightly to left

	ang.y=angle_add(obj->ang.y,-auto_aim_max_catch_angle);

	ray_trace_map_by_angle(&pnt,&ang,auto_aim_max_ray_trace_distance,&contact);
	if (contact.obj.idx!=-1) {
		if (obj->turn.ang_add.y<=0.0f) {
			obj->ang.y=angle_add(obj->ang.y,-(obj->turn.walk_speed*auto_aim_turn_speed_factor));
			return;
		}
	}

		// check slightly to right

	ang.y=angle_add(obj->ang.y,auto_aim_max_catch_angle);

	ray_trace_map_by_angle(&pnt,&ang,auto_aim_max_ray_trace_distance,&contact);
	if (contact.obj.idx!=-1) {
		if (obj->turn.ang_add.y>=0.0f) {
			obj->ang.y=angle_add(obj->ang.y,(obj->turn.walk_speed*auto_aim_turn_speed_factor));
			return;
		}
	}
}

/* =======================================================

      Player Looking
      
======================================================= */

float object_player_look_constrain(obj_type *obj,weapon_type *weap,float ang_x)
{
	float			up_angle,down_angle;

	up_angle=obj->look.up_angle;
	down_angle=obj->look.down_angle;

	if (weap!=NULL) {
		if ((weap->zoom.on) && (weap->zoom.mode!=zoom_mode_off)) {
			up_angle*=weap->zoom.look_factor;
			down_angle*=weap->zoom.look_factor;
		}
	}
		
	if (ang_x<-up_angle) ang_x=-up_angle;
	if (ang_x>down_angle) ang_x=down_angle;

	return(ang_x);
}

void object_player_look(obj_type *obj)
{
	float			look_add;
	weapon_type		*weap;
	
	look_add=obj->look.ang_add;
    if (look_add==0.0f) return;
	
		// inverted looking
		
	if (setup.invert_look) look_add=-look_add;
	
		// zoom factors

	weap=weapon_find_current(obj);
	if (weap!=NULL) {
		if ((weap->zoom.on) && (weap->zoom.mode!=zoom_mode_off)) {
			look_add*=weap->zoom.look_factor;
		}
	}
	
		// change look

	obj->view_ang.x=object_player_look_constrain(obj,weap,(obj->view_ang.x+look_add));
}

void object_player_look_stop(obj_type *obj)
{
	obj->look.ang_add=0.0f;
}

void object_player_look_fix_zoom(obj_type *obj,weapon_type *weap)
{
	obj->view_ang.x=object_player_look_constrain(obj,weap,obj->view_ang.x);
}

void object_fly_reset_angle(obj_type *obj)
{
	if ((obj->type==object_type_player) && (obj->input.mode==im_fly)) obj->ang.x=-obj->view_ang.x;
}

