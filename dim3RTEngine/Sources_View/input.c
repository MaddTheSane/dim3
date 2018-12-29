/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Action Routines

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

#ifdef D3_OS_MAC
	#include <IOKit/IOKitLib.h>
	#include <IOKit/hidsystem/IOHIDLib.h>
	#include <IOKit/hidsystem/IOHIDParameter.h>
	#include <IOKit/hidsystem/event_status_driver.h>
#endif

#include "interface.h"

int							key_define_code[input_max_keyboard]=key_codes,
							mouse_button_define_code[input_max_mouse_button]=mouse_button_codes;
char						key_define_str[input_max_keyboard][32]=key_names,
							mouse_button_define_str[input_max_mouse_button][32]=mouse_button_names,
							joystick_button_define_str[input_max_joystick_button][32]=joystick_button_names;
bool						input_app_active_flag,input_key_set_skip_flag[input_max_keyboard];

#ifdef D3_OS_MAC
	double					os_x_mouse_speed,os_x_trackpad_speed;
#endif

d3pnt						input_gui_pnt;
input_action_type			input_actions[256];

SDL_Joystick				*input_joy;

extern char					setup_control_names[][32];

extern app_type				app;
extern setup_type			setup;

/* =======================================================

      Input Start and Stop
      
======================================================= */

void input_initialize(bool in_window)
{
/*
#ifdef D3_OS_MAC
	NXEventHandle		nxe;

		// turn off mouse acceleration
		// we aren't using this right now
	
	nxe=NXOpenEventStatus();
	IOHIDGetAccelerationWithKey(nxe,CFSTR(kIOHIDMouseAccelerationType),&os_x_mouse_speed);
	IOHIDSetAccelerationWithKey(nxe,CFSTR(kIOHIDMouseAccelerationType),0.0);
	IOHIDGetAccelerationWithKey(nxe,CFSTR(kIOHIDTrackpadAccelerationType),&os_x_trackpad_speed);
	IOHIDSetAccelerationWithKey(nxe,CFSTR(kIOHIDTrackpadAccelerationType),0.0);
	NXCloseEventStatus(nxe);
#endif
*/

		// app starts activated
		
	input_app_active_flag=TRUE;
	
		// setup event ignores
		// we don't gather joystick axis
		// but it's a point on a scale, not continious
		// movement

	SDL_EventState(SDL_JOYAXISMOTION,SDL_IGNORE);
	SDL_EventState(SDL_JOYBALLMOTION,SDL_IGNORE);
	SDL_EventState(SDL_JOYHATMOTION,SDL_IGNORE);
	SDL_EventState(SDL_JOYBUTTONDOWN,SDL_IGNORE);
	SDL_EventState(SDL_JOYBUTTONUP,SDL_IGNORE);
	SDL_EventState(SDL_SYSWMEVENT,SDL_IGNORE);
	SDL_EventState(SDL_USEREVENT,SDL_IGNORE);

		// initialize mouse, joysticks, and touch

	input_mouse_initialize();
	input_joystick_initialize();
	input_touch_initialize();
}

void input_shutdown(void)
{
/*
#ifdef D3_OS_MAC
	NXEventHandle		nxe;
	
	nxe=NXOpenEventStatus();
	IOHIDSetAccelerationWithKey(nxe,CFSTR(kIOHIDMouseAccelerationType),os_x_mouse_speed);
	IOHIDSetAccelerationWithKey(nxe,CFSTR(kIOHIDTrackpadAccelerationType),os_x_trackpad_speed);
	NXCloseEventStatus(nxe);
#endif
*/

	input_touch_shutdown();
	input_joystick_shutdown();
	input_mouse_shutdown();
}

/* =======================================================

      Input Setup
      
======================================================= */

void input_clear(void)
{
		// pump out rest of events
		
	input_event_pump();

		// clear keyboard, mouse, joystick, and touch
		
	input_keyboard_clear();
	input_mouse_clear();
	input_joystick_clear();
	input_touch_clear();
}

/* =======================================================

      Clear Actions
      
======================================================= */

void input_action_clear(void)
{
    int					n;
	input_action_type	*action;
	
	action=input_actions;
    
	for (n=0;n!=256;n++) {
		action->nitem=0;
		action->still_down=FALSE;
		action->touch.trigger=FALSE;
		action->touch.auto_up=FALSE;
		action++;
	}
}

/* =======================================================

      Setup Utilities
      
======================================================= */

int setup_find_control_for_name(char *name)
{
	int			n;
	
	for (n=0;n!=ncontrol;n++) {
		if (strcasecmp(setup_control_names[n],name)==0) return(n);
	}
	
	return(-1);
}

int setup_find_action_in_setup(int action_idx)
{
	int			n;
	
	for (n=0;n!=setup.action_list.naction;n++) {
		if (strcasecmp(setup_control_names[action_idx],setup.action_list.actions[n].name)==0) return(n);
	}
	
	return(-1);
}

void setup_to_input(void)
{
	int					n,k,control_idx;
	setup_action_type	*action;
	
	input_action_clear();
	
		// actions
		
	action=setup.action_list.actions;
	
	for (n=0;n!=setup.action_list.naction;n++) {
		control_idx=setup_find_control_for_name(action->name);
		if (control_idx!=-1) {
			for (k=0;k!=max_setup_action_attach;k++) {
				if (action->attach[k][0]!=0x0) input_action_attach(action->attach[k],control_idx);
			}
		}
		action++;
	}
}

/* =======================================================

      Wait For Key/Button And Return
      
======================================================= */

void input_set_key_start(void)
{
	int				n;

		// get a list of keys to ignore if they
		// are started out down (works around problems
		// with capsLock, numLock, scrollLock, etc

	for (n=0;n!=input_max_keyboard;n++) {
		input_key_set_skip_flag[n]=input_get_keyboard_key(key_define_code[n]);
	}

}

bool input_set_key_wait(char *name)
{
	int				n;
	
	name[0]=0x0;
	
		// pump input
		
	input_event_pump();

		// any keys pressed?
		
	for (n=0;n!=input_max_keyboard;n++) {
		if (!input_key_set_skip_flag[n]) {
			if (input_get_keyboard_key(key_define_code[n])) {
				strcpy(name,key_define_str[n]);
				return(TRUE);
			}
		}
	}
	
		// any mouse buttons
		
	for (n=0;n!=input_max_mouse_button;n++) {
		if (input_get_mouse_button(mouse_button_define_code[n])) {
			strcpy(name,mouse_button_define_str[n]);
			return(TRUE);
		}
	}
	
		// any joystick buttons
		
	if (input_check_joystick_ok()) {

		for (n=0;n!=input_max_joystick_button;n++) {
			if (input_joystick_get_button(n)) {
				strcpy(name,joystick_button_define_str[n]);
				return(TRUE);
			}
		}

	}
	
	return(FALSE);
}

/* =======================================================

      App Active State
      
======================================================= */

bool input_app_active(void)
{
	return(input_app_active_flag);
}

bool input_event_window(int event)
{
	switch (event) {
	
#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)

			// going inactive on a pad/phone
	
		case SDL_WINDOWEVENT_MAXIMIZED:
			input_app_active_flag=TRUE;
			return(TRUE);
			
		case SDL_WINDOWEVENT_MINIMIZED:
			input_app_active_flag=FALSE;
			return(TRUE);

#else
			// going inactive on a pad/phone
	
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			input_app_active_flag=TRUE;
			return(TRUE);
			
		case SDL_WINDOWEVENT_FOCUS_LOST:
			input_app_active_flag=FALSE;
			return(TRUE);

#endif

	}
	
	return(FALSE);
}

/* =======================================================

      Event Pumping
      
======================================================= */

bool input_event_pump(void)
{
	bool				active_change;
	SDL_Event			event;
	
	active_change=FALSE;
	
	while (SDL_PollEvent(&event)!=0) {
	
		switch (event.type) {
		
				// window events
				
			case SDL_WINDOWEVENT:
				active_change|=input_event_window(event.window.event);
				break;

				// touch events

		#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)
			case SDL_FINGERDOWN:
				input_touch_event_down(event.tfinger.touchId,event.tfinger.fingerId,event.tfinger.x,event.tfinger.y);
				break;
					
			case SDL_FINGERUP:
				input_touch_event_up(event.tfinger.fingerId);
				break;
				
			case SDL_FINGERMOTION:
				input_touch_event_move(event.tfinger.touchId,event.tfinger.fingerId,event.tfinger.x,event.tfinger.y);
				break;

				// keyboard events
				
		#else
		
			case SDL_KEYDOWN:
				input_event_key(event.key.keysym.scancode,TRUE);
				break;

			case SDL_KEYUP:
				input_event_key(event.key.keysym.scancode,FALSE);
				break;
				
				// mouse events
		
			case SDL_MOUSEBUTTONDOWN:
				input_event_mouse_button(event.button.button,TRUE);
				break;
				
			case SDL_MOUSEBUTTONUP:
				input_event_mouse_button(event.button.button,FALSE);
				break;

			case SDL_MOUSEMOTION:
				input_event_mouse_motion(event.motion.xrel,event.motion.yrel);
				break;
				
			case SDL_MOUSEWHEEL:
				input_event_mouse_wheel(event.wheel.y);
				break;

		#endif
			
				// joystick events
				
			case SDL_JOYBUTTONDOWN:
				input_event_joystick_button(event.button.button,TRUE);
				break;
			
			case SDL_JOYBUTTONUP:
				input_event_joystick_button(event.button.button,FALSE);
				break;

				// quit event
				
			case SDL_QUIT:
				app.loop_quit=TRUE;
				break;
				
		}
	}
	
	return(active_change);
}

/* =======================================================

      Attach Actions
      
======================================================= */

void input_action_attach(char *attach_name,int action_index)
{
    int						i,index,type,nitem;
	input_action_type		*action;
	input_action_item_type  *item;
    
		// is it a key?
		
	index=-1;
	type=0;
	
    for (i=0;i!=input_max_keyboard;i++) {
        if (strcasecmp(attach_name,key_define_str[i])==0) {
            index=key_define_code[i];
			type=input_type_key;
            break;
        }
    }
	
		// is it a mouse button?
	
	if (index==-1) {
	
		for (i=0;i!=input_max_mouse_button;i++) {
			if (strcasecmp(attach_name,mouse_button_define_str[i])==0) {
				index=mouse_button_define_code[i];
				type=input_type_mouse_button;
				break;
			}
		}
	
	}

		// is it a joystick button?
	
	if ((index==-1) && (input_check_joystick_ok())) {
	
		for (i=0;i!=input_max_joystick_button;i++) {
			if (strcasecmp(attach_name,joystick_button_define_str[i])==0) {
				index=i;
				type=input_type_joystick_button;
				break;
			}
		}
	
	}
	
		// any attachment found?
		
	if (index==-1) return;
	
		// attach to action
		
	action=&input_actions[action_index];
	
	nitem=action->nitem;
	if (nitem==input_max_action_item) return;
	
	item=&action->items[nitem];
	action->nitem++;
	
	item->type=type;
	item->index=index;
}

/* =======================================================

      See If States Are Attached to Same Key/Button
      
======================================================= */

bool input_check_action_same_attachment(int action_1_index,int action_2_index)
{
	int						n1,n2,nitem_1,nitem_2;
	input_action_type		*action_1,*action_2;
	input_action_item_type	*item_1,*item_2;
	
	action_1=&input_actions[action_1_index];
	action_2=&input_actions[action_2_index];
	
	nitem_1=action_1->nitem;
	item_1=action_1->items;
	
	for (n1=0;n1<nitem_1;n1++) {
	
		nitem_2=action_2->nitem;
		item_2=action_2->items;
		
		for (n2=0;n2<nitem_2;n2++) {
			if ((item_1->type==item_2->type) && (item_1->index==item_2->index)) return(TRUE);
			item_2++;
		}
		
		item_1++;
	}
	
	return(FALSE);
}

/* =======================================================

      Action Touch Changes
      
======================================================= */

void input_action_set_touch_trigger_state(int action_idx,bool down,bool auto_up)
{
	input_actions[action_idx].touch.trigger=down;
	input_actions[action_idx].touch.auto_up=auto_up;
}

/* =======================================================

      Action States
      
======================================================= */

bool input_action_get_state(int action_index)
{
	int						n,nitem;
	input_action_type		*action;
	input_action_item_type  *item;

	action=&input_actions[action_index];

		// touches send commands directly
		// to this structure, virtual stick
		// clicks are always auto up

	if (action->touch.trigger) {
		if (action->touch.auto_up) action->touch.trigger=FALSE;
		return(TRUE);
	}

		// check other inputs
	
	nitem=action->nitem;
	item=action->items;
	
	for (n=0;n<nitem;n++) {
		
		switch (item->type) {
		
				// keys
				
			case input_type_key:
				if (input_get_keyboard_key(item->index)) return(TRUE);
				break;
				
				// mouse buttons
				
			case input_type_mouse_button:
				if (input_get_mouse_button(item->index)) return(TRUE);
				break;

				// joystick buttons
				
			case input_type_joystick_button:
				if (input_joystick_get_button(item->index)) return(TRUE);
				break;
				
		}
		
		item++;
	}
	
	return(FALSE);
}

bool input_action_get_state_range(int start_action_index,int end_action_index)
{
	int						n;
	
	for (n=start_action_index;n<=end_action_index;n++) {
		if (input_action_get_state(n)) return(TRUE);
	}
	
	return(FALSE);
}

bool input_action_get_state_single(int action_index)
{
	input_action_type		*action;

	action=&input_actions[action_index];

	if (!input_action_get_state(action_index)) {
		action->still_down=FALSE;
		return(FALSE);
	}
	
	if (action->still_down) return(FALSE);			// only return TRUE on first trigger
	
	action->still_down=TRUE;
	return(TRUE);
}

/* =======================================================

      GUI Input

	  Either Mouse or Touch
      
======================================================= */

#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)

void input_gui_set_position(int x,int y)
{
}

void input_gui_get_position(int *x,int *y)
{
	input_touch_gui_get_position(x,y);
}

void input_gui_get_hilite_position(int *x,int *y)
{
	input_touch_gui_get_hilite_position(x,y);
}

bool input_gui_is_click_down(void)
{
	return(input_touch_gui_is_click_down());
}

#else

void input_gui_set_position(int x,int y)
{
	input_mouse_gui_set_position(x,y);
}

void input_gui_get_position(int *x,int *y)
{
	input_mouse_gui_get_position(x,y);
}

void input_gui_get_hilite_position(int *x,int *y)
{
	input_mouse_gui_get_hilite_position(x,y);
}

bool input_gui_is_click_down(void)
{
	return(input_mouse_gui_is_click_down());
}

#endif

void input_gui_wait_click_up(void)
{
	while (input_gui_is_click_down()) {
		usleep(1000);
	}
}

