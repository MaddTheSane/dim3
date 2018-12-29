/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Touch Input Routines

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

extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;

bool						touch_gui_click;
d3pnt						touch_gui_pnt;
touch_state_type			touch_states[max_touch_state];

/* =======================================================

      Touch Initialize
      
======================================================= */

#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)

void input_touch_initialize(void)
{
	int							n;
	iface_virtual_button_type	*button;
	iface_virtual_stick_type	*stick;
	
	button=iface.virtual_control.buttons;
	
	for (n=0;n!=max_virtual_button;n++) {
		button->down=FALSE;
		button->show=TRUE;
		button++;
	}

	stick=&iface.virtual_control.sticks[n];

	for (n=0;n!=max_virtual_stick;n++) {
		stick->show=TRUE;
		stick->touch_x=0.0f;
		stick->touch_y=0.0f;

		stick++;
	}
}

void input_touch_shutdown(void)
{
}

/* =======================================================

      Check If Touch Input OK
      
======================================================= */

bool input_touch_check_ok(void)
{
	return(TRUE);
}

/* =======================================================

      Touch Clear
      
======================================================= */

void input_touch_clear(void)
{
	int							n;
	iface_virtual_button_type	*button;
	
		// clear all buttons
		
	button=iface.virtual_control.buttons;
	
	for (n=0;n!=max_virtual_button;n++) {
		button->down=FALSE;
		button++;
	}

		// clear all states
		
	for (n=0;n!=max_touch_state;n++) {
		touch_states[n].on=FALSE;
	}
	
	touch_gui_pnt.x=touch_gui_pnt.y=-1;
	touch_gui_click=FALSE;
}

/* =======================================================

      Virtual Buttons
      
======================================================= */

int input_touch_to_virtual_button_start(d3pnt *pt)
{
	int							n;
	iface_virtual_button_type	*button;
	
	for (n=0;n!=max_virtual_button;n++) {
		button=&iface.virtual_control.buttons[n];
		if ((!button->on) || (!button->show)) continue;

		if ((pt->x<button->pnt.x) || (pt->x>(button->pnt.x+button->size.x)) || (pt->y<button->pnt.y) || (pt->y>(button->pnt.y+button->size.y))) continue;
		
			// regular buttons go down
			
		if (!button->sticky) {
			button->down=TRUE;
			input_action_set_touch_trigger_state(button->control_idx,TRUE,FALSE);
			return(n);
		}
		
			// sticky buttons toggle
			
		if (!button->down) {
			button->down=TRUE;
			input_action_set_touch_trigger_state(button->control_idx,TRUE,FALSE);
			return(n);
		}
		
		button->down=FALSE;
		input_action_set_touch_trigger_state(button->control_idx,FALSE,FALSE);
		return(n);
	}
	
	return(-1);
}

void input_touch_to_virtual_button_release(int button_idx)
{
	iface_virtual_button_type	*button;
	
	button=&iface.virtual_control.buttons[button_idx];
	if (button->sticky) return;			// sticky buttons never go up
	
	button->down=FALSE;
	input_action_set_touch_trigger_state(button->control_idx,FALSE,FALSE);
}

/* =======================================================

      Virtual Sticks
      
======================================================= */

void input_touch_to_virtual_stick_move(touch_state_type *state)
{
	int							mx,my,sx,sy;
	iface_virtual_stick_type	*stick;

		// did this touch start in stick?
		
	if (state->stick_idx==-1) return;
	
	stick=&iface.virtual_control.sticks[state->stick_idx];
	
		// move within stick
		
	sx=stick->size.x>>1;
	mx=stick->pnt.x+sx;
	
	sy=stick->size.y>>1;
	my=stick->pnt.y+sy;
	
	stick->touch_x=(float)(state->pt.x-mx)/(float)sx;
	if (stick->touch_x<-1.0f) stick->touch_x=-1.0f;
	if (stick->touch_x>1.0f) stick->touch_x=1.0f;
	

	stick->touch_y=(float)(state->pt.y-my)/(float)sy;
	if (stick->touch_y<-1.0f) stick->touch_y=-1.0f;
	if (stick->touch_y>1.0f) stick->touch_y=1.0f;
	
	if (stick->flip_x) stick->touch_x=-stick->touch_x;
	if (stick->flip_y) stick->touch_y=-stick->touch_y;
}

int input_touch_to_virtual_stick_start(touch_state_type *state)
{
	int							n;
	iface_virtual_stick_type	*stick;
	
	for (n=0;n!=max_virtual_stick;n++) {
		stick=&iface.virtual_control.sticks[n];
		if ((!stick->on) || (!stick->show)) continue;

			// in this stick?
			
		if ((state->pt.x<stick->pnt.x) || (state->pt.x>(stick->pnt.x+stick->size.x)) || (state->pt.y<stick->pnt.y) || (state->pt.y>(stick->pnt.y+stick->size.y))) continue;
		
			// move the stick
			
		input_touch_to_virtual_stick_move(state);
		
		return(n);
	}
	
	return(-1);
}

void input_touch_to_virtual_stick_release(int stick_idx,int tick)
{
	iface_virtual_stick_type	*stick;

		// release the stick

	stick=&iface.virtual_control.sticks[stick_idx];
	stick->touch_x=0.0f;
	stick->touch_y=0.0f;

		// see if this counts as a click

	if (stick->click_control_idx==-1) return;
	if (tick>(game_time_get_raw()+touch_stick_up_down_click_msec)) return;

	input_action_set_touch_trigger_state(stick->click_control_idx,TRUE,TRUE);		// set auto up so it gets read once and then set to up
}

/* =======================================================

      Handle Touch States
      
======================================================= */

bool input_touch_get_state(int idx)
{
	return(touch_states[idx].on);
}

void input_touch_get_point(int idx,int *x,int *y)
{
	*x=touch_states[idx].pt.x;
	*y=touch_states[idx].pt.y;
}

void input_touch_state_add_up(int id)
{
	int					n;
	touch_state_type	*state;
	
	state=touch_states;
	
	for (n=0;n!=max_touch_state;n++) {
		if (state->on) {
			if (state->id==id) {
				state->on=FALSE;
				if (state->button_idx!=-1) input_touch_to_virtual_button_release(state->button_idx);
				if (state->stick_idx!=-1) input_touch_to_virtual_stick_release(state->stick_idx,state->tick);
				return;
			}
		}
		
		state++;
	}
}

void input_touch_state_add_down(int id,d3pnt *pt)
{
	int					n,idx;
	touch_state_type	*state;
	
	state=touch_states;
	
	idx=-1;
	
		// if already in, then update
		// the point
		
	for (n=0;n!=max_touch_state;n++) {
	
		if (state->on) {
			if (state->id==id) {
				state->pt.x=pt->x;
				state->pt.y=pt->y;
				input_touch_to_virtual_stick_move(state);
				return;
			}
		}
		else {
			if (idx==-1) idx=n;
		}
		
		state++;
	}
	
		// otherwise add it
		// if there is room
		
	if (idx==-1) return;
	
	state=&touch_states[idx];
	
	state->on=TRUE;
	state->id=id;
	state->stick_idx=-1;
	state->pt.x=pt->x;
	state->pt.y=pt->y;
	state->tick=game_time_get_raw();
	
		// in a button or a stick?

	state->stick_idx=-1;
	state->button_idx=input_touch_to_virtual_button_start(&state->pt);
	if (state->button_idx==-1) state->stick_idx=input_touch_to_virtual_stick_start(state);
}

/* =======================================================

      Touch Event
      
======================================================= */

void input_touch_event_up(int finger_id)
{
	input_touch_state_add_up(finger_id);

	touch_gui_click=FALSE;
}

void input_touch_scrub_point(d3pnt *pt,int touch_id,float x,float y)
{
		// convert to screen
		
#ifndef D3_ROTATE_VIEW
	pt->x=(int)(x*((float)view.screen.x_sz));
	pt->y=(int)(y*((float)view.screen.y_sz));
#else
	pt->x=(int)(y*((float)view.screen.x_sz));
	pt->y=view.screen.y_sz-(int)(x*((float)view.screen.y_sz));
#endif

		// convert to view scale
		
	pt->x=(pt->x*iface.scale_x)/view.screen.x_sz;
	pt->y=(pt->y*iface.scale_y)/view.screen.y_sz;
}

void input_touch_event_down(int touch_id,int finger_id,float x,float y)
{
	d3pnt				pt;
	
	input_touch_scrub_point(&pt,touch_id,x,y);
	input_touch_state_add_down(finger_id,&pt);

	touch_gui_pnt.x=pt.x;
	touch_gui_pnt.y=pt.y;
	
	touch_gui_click=TRUE;
}

void input_touch_event_move(int touch_id,int finger_id,float x,float y)
{
	d3pnt				pt;
	
	input_touch_scrub_point(&pt,touch_id,x,y);
	input_touch_state_add_down(finger_id,&pt);
}

/* =======================================================

      Touch Stick
      
======================================================= */

float input_touch_get_axis(int axis)
{
	iface_virtual_stick_type	*stick;
	
		// get right stick
		
	if (axis<2) {
		stick=&iface.virtual_control.sticks[0];
	}
	else {
		stick=&iface.virtual_control.sticks[1];
	}
	
		// if stick not on, always centered
		
	if (!stick->on) return(0.0f);
	
		// get right axis
		
	if ((axis==0) || (axis==2)) return(stick->touch_x);
	return(stick->touch_y);
}

bool input_touch_get_axis_as_button_min(int axis)
{
	return(input_touch_get_axis(axis)<-0.1f);
}

bool input_touch_get_axis_as_button_max(int axis)
{
	return(input_touch_get_axis(axis)>0.1f);
}

/* =======================================================

      Touch GUI
      
======================================================= */

void input_touch_gui_get_position(int *x,int *y)
{
	*x=touch_gui_pnt.x;
	*y=touch_gui_pnt.y;
}

void input_touch_gui_get_hilite_position(int *x,int *y)
{
	if (touch_gui_click) {
		input_touch_gui_get_position(x,y);
	}
	else {
		*x=*y=-1;
	}
}

bool input_touch_gui_is_click_down(void)
{
	return(touch_gui_click);
}

/* =======================================================

      Touch Stubs
      
======================================================= */

#else

void input_touch_initialize(void) {}
void input_touch_shutdown(void) {}
bool input_touch_check_ok(void) { return(FALSE); }
void input_touch_clear(void) {}
void input_touch_event_up(int finger_id) {}
void input_touch_event_down(int touch_id,int finger_id,float x,float y) {}
void input_touch_event_move(int touch_id,int finger_id,float x,float y) {}
float input_touch_get_axis(int axis) { return(0.0f); }
bool input_touch_get_axis_as_button_min(int axis) { return(FALSE); }
bool input_touch_get_axis_as_button_max(int axis) { return(FALSE); }

#endif
