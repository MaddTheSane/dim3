/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Player Routines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern camera_type			camera;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

int							mouse_last_read_tick;
bool						weapon_change_key_down,weapon_target_key_down,weapon_zoom_key_down,
							enter_exit_key_down,network_score_key_down,toggle_run_state,respawn_key_down,
							fire_key_down[4],command_key_down[20],player_key_down[20];

/* =======================================================

      Clear Input
      
======================================================= */

void player_clear_input(void)
{
	int			i;
	
	mouse_last_read_tick=-1;

	weapon_change_key_down=FALSE;
	weapon_target_key_down=FALSE;
	weapon_zoom_key_down=FALSE;
	enter_exit_key_down=FALSE;
	network_score_key_down=FALSE;
	toggle_run_state=FALSE;
	respawn_key_down=FALSE;
	
	for (i=0;i!=4;i++) {
		fire_key_down[i]=FALSE;
	}

    for (i=0;i!=20;i++) {
        command_key_down[i]=FALSE;
        player_key_down[i]=FALSE;
    }
	
	input_clear();

	view.chat.type_on=FALSE;
}

/* =======================================================

      Player Command Keys
      
======================================================= */

void player_command_input(obj_type *obj)
{
	int			n,k;

        // game keys
        
	for (n=nc_game_start;n<=nc_game_end;n++) {
    
        k=n-nc_game_start;
        
        if (input_action_get_state(n)) {
            if (!command_key_down[k]) {
                command_key_down[k]=TRUE;
				scripts_post_event_console(js.game_script_idx,-1,sd_event_message,sd_event_message_from_key_down,k);
            }
        }
        else {
            if (command_key_down[k]) {
                command_key_down[k]=FALSE;
				scripts_post_event_console(js.game_script_idx,-1,sd_event_message,sd_event_message_from_key_up,k);
            }
        }
    }
    
        // player keys
        
	for (n=nc_player_start;n<=nc_player_end;n++) {
    
        k=n-nc_player_start;
        
        if (input_action_get_state(n)) {
            if (!player_key_down[k]) {
                player_key_down[k]=TRUE;
				scripts_post_event_console(obj->script_idx,-1,sd_event_message,sd_event_message_from_key_down,k);
            }
        }
        else {
            if (player_key_down[k]) {
                player_key_down[k]=FALSE;
				scripts_post_event_console(obj->script_idx,-1,sd_event_message,sd_event_message_from_key_up,k);
            }
        }
    }
}

/* =======================================================

      Y Movements 
      
======================================================= */

void player_movement_fpp_y_input(obj_type *obj)
{
    if (input_action_get_state(nc_down)) {
        obj->vert_move.moving=TRUE;
        if (obj->vert_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=FALSE;
		return;
    }

	if (input_action_get_state(nc_up)) {
		obj->vert_move.moving=TRUE;
        if (!obj->vert_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=TRUE;
		return;
    }
}

void player_movement_fly_swim_y_input(obj_type *obj)
{
	if (!obj->forward_move.moving) return;

    if (obj->view_ang.x<-obj->vert_move.slop) {
        obj->vert_move.moving=TRUE;
        if (obj->vert_move.reverse==obj->forward_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=!obj->forward_move.reverse;
		return;
    }
    
	if (obj->view_ang.x>obj->vert_move.slop) {
		obj->vert_move.moving=TRUE;
        if (!obj->vert_move.reverse==obj->forward_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=obj->forward_move.reverse;
		return;
    }
}

void player_movement_ladder_y_input(obj_type *obj)
{
	if ((!obj->climb.on) || (!obj->forward_move.moving)) return;
			
    if (obj->view_ang.x<-obj->vert_move.slop) {
        obj->vert_move.moving=TRUE;
        if (obj->vert_move.reverse==obj->forward_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=!obj->forward_move.reverse;
		return;
    }
    
	if (obj->view_ang.x>obj->vert_move.slop) {
		obj->vert_move.moving=TRUE;
        if (!obj->vert_move.reverse==obj->forward_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=obj->forward_move.reverse;
		return;
    }
}

/* =======================================================

      Player Weapon Input
      
======================================================= */

void player_weapon_target_input(obj_type *obj,weapon_type *weap)
{
	bool				target_next,target_previous;
 	
		// targetting on?
		
	if (!weap->target.on) return;
	
		// any zoom keys down?
		
	target_next=input_action_get_state(nc_weapon_target_next);
	target_previous=input_action_get_state(nc_weapon_target_previous);
	
	if ((!target_next) && (!target_previous)) {
		weapon_target_key_down=FALSE;
		return;
	}
	
		// key held down, ignore
		
	if (weapon_target_key_down) return;
	
		// change targets

	if (target_next) {
		weapon_target_next_object(obj,weap);
		weapon_target_key_down=TRUE;
		return;
	}

	if (target_previous) {
		weapon_target_previous_object(obj,weap);
		weapon_target_key_down=TRUE;
		return;
	}
}

void player_weapon_zoom_input(obj_type *obj,weapon_type *weap)
{
	bool				zoom_key,zoom_increase_key,zoom_decrease_key;
 	
		// zooms allowed?
		
	if (!weap->zoom.on) return;
	
		// any zoom keys down?
		
	zoom_key=input_action_get_state(nc_weapon_zoom);
	zoom_increase_key=input_action_get_state(nc_weapon_zoom_increase);
	zoom_decrease_key=input_action_get_state(nc_weapon_zoom_decrease);
	
	if ((!zoom_key) && (!zoom_increase_key) && (!zoom_decrease_key)) {
		weapon_zoom_key_down=FALSE;
		return;
	}
	
		// key held down, ignore
		
	if (weapon_zoom_key_down) return;
	
		// check zoom activate/deactive key
		
	if (zoom_key) {
		weapon_zoom_key(obj,weap);
		weapon_zoom_key_down=TRUE;
		return;
	}
		
		// if zoom active, check increase/decrease keys
		
	if (weap->zoom.mode!=zoom_mode_on) return;
	
	if ((zoom_increase_key) && (weap->zoom.current_step<(weap->zoom.step_count-1))) {
		weap->zoom.current_step++;
		weapon_zoom_key_down=TRUE;
		return;
	}

	if ((zoom_decrease_key) && (weap->zoom.current_step>0)) {
		weap->zoom.current_step--;
		weapon_zoom_key_down=TRUE;
		return;
	}
}

void player_weapon_change_input(obj_type *obj,weapon_type *weap)
{
	int				n,k;
	bool			down_key,weapon_key[(nc_weapon_end-nc_weapon_start)+1],
					next_weapon_key,previous_weapon_key;
	
		// special check if weapon change key is same as the
		// zoom or targetting keys.  If either are on, don't
		// change the weapon
		
	next_weapon_key=input_action_get_state(nc_next_weapon);
	previous_weapon_key=input_action_get_state(nc_previous_weapon);
		
	if ((weap->zoom.on) && (weap->zoom.mode!=zoom_mode_off)) {
		if (input_check_action_same_attachment(nc_next_weapon,nc_weapon_zoom_increase)) next_weapon_key=FALSE;
		if (input_check_action_same_attachment(nc_next_weapon,nc_weapon_zoom_decrease)) next_weapon_key=FALSE;
		if (input_check_action_same_attachment(nc_previous_weapon,nc_weapon_zoom_increase)) previous_weapon_key=FALSE;
		if (input_check_action_same_attachment(nc_previous_weapon,nc_weapon_zoom_decrease)) previous_weapon_key=FALSE;
	}
	if (weap->target.on) {
		if (input_check_action_same_attachment(nc_next_weapon,nc_weapon_target_next)) next_weapon_key=FALSE;
		if (input_check_action_same_attachment(nc_next_weapon,nc_weapon_target_previous)) next_weapon_key=FALSE;
		if (input_check_action_same_attachment(nc_previous_weapon,nc_weapon_target_next)) previous_weapon_key=FALSE;
		if (input_check_action_same_attachment(nc_previous_weapon,nc_weapon_target_previous)) previous_weapon_key=FALSE;
	}
	
		// any weapon change keys down?
		
	down_key=next_weapon_key||previous_weapon_key;

	for (n=nc_weapon_start;n<=nc_weapon_end;n++) {
		k=n-nc_weapon_start;
		weapon_key[k]=input_action_get_state(n);
		down_key=down_key||weapon_key[k];
	}
	
	if (!down_key) {
		weapon_change_key_down=FALSE;
		return;
	}
	
		// key held down, ignore
		
	if (weapon_change_key_down) return;
	
		// change weapons

	if (weap->zoom.mode!=zoom_mode_off) weapon_zoom_off(obj,weap);
	if (weap->target.on) weapon_target_end(obj,weap);

	if (next_weapon_key) {
		weapon_switch(obj,1);
		weapon_change_key_down=TRUE;
		return;
	}

	if (previous_weapon_key) {
		weapon_switch(obj,-1);
		weapon_change_key_down=TRUE;
		return;
	}
	
	for (n=nc_weapon_start;n<=nc_weapon_end;n++) {
		k=n-nc_weapon_start;
		if (weapon_key[k]) {
			weapon_pick(obj,k);
			weapon_change_key_down=TRUE;
			return;
		}
	}
}

void player_weapon_fire_input(obj_type *obj,weapon_type *weap)
{
	int					n,method;
	
		// no firing if no weapon is being held
		
	if (obj->held_weapon.mode!=wm_held) return;
	
		// check keys
		
	for (n=nc_fire_start;n<=nc_fire_end;n++) {
	
		method=n-nc_fire_start;
		
		if (input_action_get_state(n)) {
			if (!fire_key_down[method]) {
				fire_key_down[method]=TRUE;
				weapon_player_fire_down(obj,weap,method);
			}
			else {
				weapon_player_fire_repeat(obj,weap);
			}
		}
		else {
			if (fire_key_down[method]) {
				fire_key_down[method]=FALSE;
				weapon_player_fire_up(obj,weap,method);
			}
		}
	}
}

void player_weapon_input(obj_type *obj)
{
	weapon_type			*weap;
	
		// get held weapon
		
	weap=weapon_find_current(obj);
	if (weap==NULL) return;

		// weapon keys
		
	if ((obj->held_weapon.mode==wm_held) && (weap!=NULL)) {
		player_weapon_target_input(obj,weap);
		player_weapon_zoom_input(obj,weap);
		player_weapon_fire_input(obj,weap);
	}

	player_weapon_change_input(obj,weap);
}

/* =======================================================

      Player Enter-Exit
      
======================================================= */

void player_enter_exit_input(obj_type *obj)
{
	if (!input_action_get_state(nc_enter_exit)) {
		enter_exit_key_down=FALSE;
		return;
	}
	
	if (enter_exit_key_down) return;			// only check on key downs
	
	enter_exit_key_down=TRUE;
	
		// exit a vehicle?
		
	if (obj->vehicle.attach_obj_idx!=-1) {
		object_exit_vehicle(obj,FALSE,NULL);
		return;
	}
		
		// enter a vehicle?
	
	object_enter_vehicle(obj,-1,NULL);
}

/* =======================================================

      Player Messages
      
======================================================= */

bool player_message_input(obj_type *obj)
{
	int				len;
	char			ch,nstr[max_view_chat_str_len];
	char			*c,*msg;
	d3col			col;
	
	if (net_setup.mode==net_mode_none) return(TRUE);
	
		// scores
		
	if (input_action_get_state(nc_score)) {
		if (!network_score_key_down) {
			view.score.on=!view.score.on;
			network_score_key_down=TRUE;
		}
	}
	else {
		network_score_key_down=FALSE;
	}
		
		// chatting

	if (view.chat.type_on) {
	
			// get last raw key as character
			
		ch=input_get_text_input_key();
		if (ch==0x0) return(FALSE);
		
			// get current message len
			
		msg=view.chat.type_str;
		len=strlen(msg);
		
			// escape cancels
			
		if (ch==0x1B) {
			view.chat.type_on=FALSE;
			return(FALSE);
		}
		
			// return ends entry
			
		if (ch==0xD) {
			view.chat.type_on=FALSE;
			if (len>1) {
				msg[len-1]=0x0;
				net_client_send_chat(obj,msg);
				object_get_tint(obj,&col);
				chat_add_message(obj,msg,&col);
			}
			return(FALSE);
		}
		
			// delete
			
		if (ch==0x8) {
			len-=2;
			if (len<0) len=0;

			msg[len++]='_';
			msg[len]=0x0;

			return(FALSE);
		}
		
			// eliminate certain characters
			
		if (((ch<'!') || (ch>'z')) && (ch!=0x20)) return(FALSE);
		
			// force start a new line if too many characters
			
		if (len>=(max_view_chat_str_len-1)) {
			nstr[0]=0x0;
			
			c=strrchr(msg,' ');
			if (c!=NULL) {
				if ((int)(c-msg)>5) {
					strcpy(nstr,(c+1));
					*c=0x0;
				}
			}
			
			msg[len-1]=0x0;
			net_client_send_chat(obj,msg);
			object_get_tint(obj,&col);
			chat_add_message(obj,msg,&col);
			
			strcpy(msg,nstr);
			len=strlen(msg);
		}
		
			// add new character
			
		if (len>0) len--;
		
		msg[len++]=ch;
		msg[len++]='_';
		msg[len]=0x0;
	
		return(FALSE);
	}
	
	if (input_action_get_state(nc_message)) {
		view.chat.type_on=TRUE;
		
		input_clear_text_input();
		
		view.chat.type_str[0]='_';
		view.chat.type_str[1]=0x0;
		
		return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      Input Utilities
      
======================================================= */

float player_mouse_smooth(float mouse_ang,float turn_ang)
{
	float			dif;
	
	if (!setup.mouse_smooth) return(mouse_ang);
	
		// if there isn't any current
		// movement, then always auto-start
		// to deal with lag
		
	if (turn_ang==0.0f) return(mouse_ang);
	
		// get the difference, if
		// 1.0 or less, just do it
		
	dif=turn_ang-mouse_ang;
	if (fabsf(dif)<=1.0f) return(mouse_ang);
	
		// else go halfway there
		
	return(mouse_ang+(dif*0.5f));
}

void player_get_6_way_input(obj_type *obj,float *mouse_x,float *mouse_y,bool *go_forward,bool *go_backward,bool *go_side_left,bool *go_side_right)
{
		// get input

	if (!obj->turn.ignore_mouse) {
		input_get_mouse_movement(mouse_x,mouse_y);
	}
	else {
		*mouse_x=*mouse_y=0.0f;
	}

		// keyboard
		
	*go_forward=input_action_get_state(nc_move_forward);
	*go_backward=input_action_get_state(nc_move_backward);
	*go_side_left=input_action_get_state(nc_sidestep_left);
	*go_side_right=input_action_get_state(nc_sidestep_right);
	
		// touch
		
		// if one virtual joystick, treat as forward/backwards + turn
		// if two, treat as dual stick controls
		
	if (input_touch_check_ok()) {
	
		if ((iface.virtual_control.sticks[0].on) && (!iface.virtual_control.sticks[1].on)) {
			(*mouse_x)+=input_touch_get_axis(0);

			(*go_forward)|=input_touch_get_axis_as_button_min(1);
			(*go_backward)|=input_touch_get_axis_as_button_max(1);
		}
		else {
			if ((iface.virtual_control.sticks[0].on) && (iface.virtual_control.sticks[1].on)) {
				(*mouse_x)+=input_touch_get_axis(2);
				(*mouse_y)+=input_touch_get_axis(3);

				(*go_forward)|=input_touch_get_axis_as_button_min(1);
				(*go_backward)|=input_touch_get_axis_as_button_max(1);
				(*go_side_left)|=input_touch_get_axis_as_button_min(0);
				(*go_side_right)|=input_touch_get_axis_as_button_max(0);
			}
		}
	}
		
		// joystick

	if (input_check_joystick_ok()) {
		(*mouse_x)-=input_joystick_get_axis(2);
		(*mouse_y)+=input_joystick_get_axis(3);

		(*go_forward)|=input_joystick_get_axis_as_button_min(1);
		(*go_backward)|=input_joystick_get_axis_as_button_max(1);
		(*go_side_left)|=input_joystick_get_axis_as_button_min(0);
		(*go_side_right)|=input_joystick_get_axis_as_button_max(0);
	}

	if (input_action_get_state(nc_turn_left)) (*mouse_x)-=obj->turn.key_speed;
	if (input_action_get_state(nc_turn_right)) (*mouse_x)+=obj->turn.key_speed;

	if (*go_forward) *go_backward=FALSE;
	if (*go_side_left) *go_side_right=FALSE;

		// quick reverse locks

	if (!obj->quick_reverse) {
		if (*go_forward) {
			if ((obj->forward_move.reverse) && (obj->forward_move.speed!=0)) *go_forward=FALSE;
		}

		if (*go_backward) {
			if ((!obj->forward_move.reverse) && (obj->forward_move.speed!=0)) *go_backward=FALSE;
		}
	}
}

void player_get_4_way_input(bool *go_left,bool *go_right,bool *go_up,bool *go_down)
{
		// keyboard
		
	*go_left=input_action_get_state(nc_turn_left);
	*go_right=input_action_get_state(nc_turn_right);
	*go_up=input_action_get_state(nc_up);
	*go_down=input_action_get_state(nc_down);
	
		// touch
		
	if (input_touch_check_ok()) {
		(*go_left)|=input_touch_get_axis_as_button_min(0);
		(*go_right)|=input_touch_get_axis_as_button_max(0);
		(*go_up)|=input_touch_get_axis_as_button_min(1);
		(*go_down)|=input_touch_get_axis_as_button_max(1);
	}

		// joystick
		
	if (input_check_joystick_ok()) {
		(*go_left)|=input_joystick_get_axis_as_button_min(0);
		(*go_right)|=input_joystick_get_axis_as_button_max(0);
		(*go_up)|=input_joystick_get_axis_as_button_min(1);
		(*go_down)|=input_joystick_get_axis_as_button_max(1);
	}

	if (*go_right) *go_left=FALSE;
	if (*go_down) *go_up=FALSE;
}

/* =======================================================

      Input Run/Walk State
      
======================================================= */

void player_set_run_walk_state(obj_type *obj)
{
	if (obj->single_speed) {
		obj->forward_move.running=FALSE;
		return;
	}

	if (!setup.toggle_run) {
		toggle_run_state=input_action_get_state(nc_run);
	}
	else {
		if (input_action_get_state_single(nc_run)) toggle_run_state=!toggle_run_state;
	}
	
	if (setup.always_run) {
		obj->forward_move.running=!toggle_run_state;
	}
	else {
		obj->forward_move.running=toggle_run_state;
	}
}

/* =======================================================

      FPP Input Type
      
======================================================= */

void player_fpp_input(obj_type *obj)
{
	float			mouse_x,mouse_y;
	bool			go_forward,go_backward,go_side_left,go_side_right;

		// start with no movement
		
    obj->forward_move.moving=FALSE;
	obj->side_move.moving=FALSE;
	obj->vert_move.moving=FALSE;

		// get input

	player_get_6_way_input(obj,&mouse_x,&mouse_y,&go_forward,&go_backward,&go_side_left,&go_side_right);

		// turning and looking
		
	obj->turn.ang_add.y=player_mouse_smooth(mouse_x,obj->turn.ang_add.y);
	obj->look.ang_add=player_mouse_smooth(mouse_y,obj->look.ang_add);
	
		// don't move if ducked and crawling disabled
		
	if ((!obj->crawl) && (obj->duck.mode!=dm_stand)) return;

		// forward/backwards movement

	if (go_forward) {
		obj->forward_move.moving=TRUE;
		if (obj->forward_move.reverse) obj->forward_move.speed=0;
		obj->forward_move.reverse=FALSE;
	}

	if (go_backward) {
		obj->forward_move.moving=TRUE;
		if (!obj->forward_move.reverse) obj->forward_move.speed=0;
		obj->forward_move.reverse=TRUE;
	}

		// run and walk state

	player_set_run_walk_state(obj);

		// side movement
	
	if (go_side_left) {
		obj->side_move.moving=TRUE;
		if (obj->side_move.reverse) obj->side_move.speed=0;
		obj->side_move.reverse=FALSE;
	}

	if (go_side_right) {
		obj->side_move.moving=TRUE;
		if (!obj->side_move.reverse) obj->side_move.speed=0;
		obj->side_move.reverse=TRUE;
	}

		// y movement

	if ((obj->fly) || (obj->liquid.mode==lm_under) || (obj->liquid.mode==lm_float)) {
		player_movement_fly_swim_y_input(obj);
	}
	else {
		player_movement_fpp_y_input(obj);
		player_movement_ladder_y_input(obj);
	}
}

/* =======================================================

      Side Scroll Input Type
      
======================================================= */

void player_side_scroll_input(obj_type *obj)
{
	bool			go_left,go_right,go_up,go_down;

		// get input

	player_get_4_way_input(&go_left,&go_right,&go_up,&go_down);

		// left-right movement
		
	if (go_left) object_player_turn_direct(obj,270.0f);
	if (go_right) object_player_turn_direct(obj,90.0f);
	
		// up-down movement
	
    if (go_down) {
        obj->vert_move.moving=TRUE;
        if (obj->vert_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=FALSE;
    }

	if (go_up) {
        obj->vert_move.moving=TRUE;
        if (!obj->vert_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=TRUE;
    }
	
		// if no movement, then stop
		
	if ((!go_left) && (!go_right)) obj->forward_move.moving=FALSE;
	if ((!go_up) && (!go_down)) obj->vert_move.moving=FALSE;
}

/* =======================================================

      Top-Down Input Type
      
======================================================= */

void player_top_down_input(obj_type *obj)
{
	bool			go_left,go_right,go_up,go_down;

		// get input

	player_get_4_way_input(&go_left,&go_right,&go_up,&go_down);

		// if facing is on, we need
		// to switch keys to be forward/backward
		// and side step

	if (obj->face.obj_idx!=-1) {

		if (go_left) {
			obj->side_move.moving=TRUE;
			obj->side_move.reverse=FALSE;
		}
		else {
			if (go_right) {
				obj->side_move.moving=TRUE;
				obj->side_move.reverse=TRUE;
			}
			else {
				obj->side_move.moving=FALSE;
			}
		}

		if (go_up) {
			obj->forward_move.moving=TRUE;
			obj->forward_move.reverse=FALSE;
		}
		else {
			if (go_down) {
				obj->forward_move.moving=TRUE;
				obj->forward_move.reverse=TRUE;
			}
			else {
				obj->forward_move.moving=FALSE;
			}
		}

		return;
	}
	
		// all the movements
		
	if (go_left) {
		if (go_up) {
			object_player_turn_direct(obj,angle_add(obj->turn.top_down_ang_offset,315.0f));
			return;
		}
		if (go_down) {
			object_player_turn_direct(obj,angle_add(obj->turn.top_down_ang_offset,225.0f));
			return;
		}
		object_player_turn_direct(obj,angle_add(obj->turn.top_down_ang_offset,270.0f));
		return;
	}
	
	if (go_right) {
		if (go_up) {
			object_player_turn_direct(obj,angle_add(obj->turn.top_down_ang_offset,45.0f));
			return;
		}
		if (go_down) {
			object_player_turn_direct(obj,angle_add(obj->turn.top_down_ang_offset,135.0f));
			return;
		}
		object_player_turn_direct(obj,angle_add(obj->turn.top_down_ang_offset,90.0f));
		return;
	}
	
	if (go_up) {
		object_player_turn_direct(obj,angle_add(obj->turn.top_down_ang_offset,0.0f));
		return;
	}
	
	if (go_down) {
		object_player_turn_direct(obj,angle_add(obj->turn.top_down_ang_offset,180.0f));
		return;
	}

		// no directions, stop movements

	obj->forward_move.moving=FALSE;
	obj->side_move.moving=FALSE;
}

/* =======================================================

      Fly Input Type
      
======================================================= */

void player_fly_input(obj_type *obj)
{
	float			mouse_x,mouse_y;
	bool			go_forward,go_backward,go_side_left,go_side_right;

		// start with no movement
		
    obj->forward_move.moving=FALSE;
	obj->side_move.moving=FALSE;
	obj->vert_move.moving=FALSE;

		// get input

	player_get_6_way_input(obj,&mouse_x,&mouse_y,&go_forward,&go_backward,&go_side_left,&go_side_right);

		// turning and looking

	obj->turn.ang_add.y=player_mouse_smooth(mouse_x,obj->turn.ang_add.y);
	obj->look.ang_add=player_mouse_smooth(mouse_y,obj->look.ang_add);

		// forward/backwards movement

	if (go_forward) {
		obj->forward_move.moving=TRUE;
		if (obj->forward_move.reverse) obj->forward_move.speed=0;
		obj->forward_move.reverse=FALSE;
	}

	if (go_backward) {
		obj->forward_move.moving=TRUE;
		if (!obj->forward_move.reverse) obj->forward_move.speed=0;
		obj->forward_move.reverse=TRUE;
	}

		// flying has a single speed

	obj->forward_move.running=FALSE;

		// y movement

	player_movement_fly_swim_y_input(obj);
}

/* =======================================================

      Thrust Input Type
      
======================================================= */

void player_thrust_input(obj_type *obj)
{
	float			mouse_x,mouse_y;
	bool			go_forward,go_backward,go_side_left,go_side_right;

		// start with no movement
		
    obj->forward_move.moving=FALSE;
	obj->side_move.moving=FALSE;
	obj->vert_move.moving=FALSE;

		// get input

	player_get_6_way_input(obj,&mouse_x,&mouse_y,&go_forward,&go_backward,&go_side_left,&go_side_right);

		// turning and looking

	obj->turn.ang_add.y=player_mouse_smooth(mouse_x,obj->turn.ang_add.y);
	obj->look.ang_add=player_mouse_smooth(mouse_y,obj->look.ang_add);

		// forward/backwards movement

	if (go_forward) {
		obj->forward_move.moving=TRUE;
		if (obj->forward_move.reverse) obj->forward_move.speed=0;
		obj->forward_move.reverse=FALSE;
	}

	if (go_backward) {
		obj->forward_move.moving=TRUE;
		if (!obj->forward_move.reverse) obj->forward_move.speed=0;
		obj->forward_move.reverse=TRUE;
	}

		// thrusting has a single speed

	obj->forward_move.running=FALSE;
}

/* =======================================================

      Player Death Input
      
======================================================= */

void player_death_input(obj_type *obj)
{
		// can only respawn by key press
		// if in single player game
		
	if (net_setup.mode!=net_mode_none) return;
	if (obj->input.respawn_freeze) return;

		// restart key
		
	if (input_action_get_state_range(nc_respawn_start,nc_respawn_end)) {
		if (!respawn_key_down) {
			server.map_change.on=TRUE;
			server.map_change.skip_media=TRUE;
			server.map_change.player_restart=TRUE;
		}
		return;
	}

	respawn_key_down=FALSE;
}

/* =======================================================

      Get Player Input
      
======================================================= */

void player_get_input(void)
{
	obj_type		*obj;
	
		// get player object
	
	obj=server.obj_list.objs[server.player_obj_idx];

		// no input if menu or console typing is on

	if ((view.menu.active) || (view.menu.fade_in) || (view.menu.fade_out) || ((view.console.on) && (view.console.focus))) {
	    obj->forward_move.moving=FALSE;
		obj->side_move.moving=FALSE;
		obj->vert_move.moving=FALSE;
		obj->turn.ang_add.y=0.0f;
		obj->look.ang_add=0.0f;
		return;
	}
	
		// respawn
		
	if (obj->status.health.value<=0) {
		player_death_input(obj);
		return;
	}
	
	respawn_key_down=TRUE;

		// message inputs
		
	if (!player_message_input(obj)) return;
		
		// commands (game and player)
		
	player_command_input(obj);
	
        // check for non-input states
        
	if ((obj->hidden) || (obj->input.freeze)) return;
	
		// proper movement routines
		
	switch (obj->input.mode) {
	
		case im_fpp:
			player_fpp_input(obj);
			break;
			
		case im_side_scroll:
			player_side_scroll_input(obj);
			break;
			
		case im_top_down:
			player_top_down_input(obj);
			break;

		case im_fly:
			player_fly_input(obj);
			break;

		case im_thrust:
			player_thrust_input(obj);
			break;
	
	}

        // jump and duck
		
	if (input_action_get_state(nc_jump)) object_start_jump(obj);
    
	if (input_action_get_state(nc_duck)) {
        object_start_duck(obj);
    }
    else {
        object_start_stand(obj);
    }
	
        // weapons
    
	player_weapon_input(obj);
	
		// enter and exit
		
	player_enter_exit_input(obj);

		// mouse wheels don't have a state
		// so we need to reset state

	input_mouse_wheel_reset();
}

