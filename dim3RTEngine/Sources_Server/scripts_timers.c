/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Timers

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

extern server_type			server;
extern js_type				js;

/* =======================================================

      Initialize Timers
      
======================================================= */

void timers_initialize_list(void)
{
	int				n;

	for (n=0;n!=max_timer_list;n++) {
		js.timer_list.timers[n]=NULL;
	}
}

void timers_free_list(void)
{
	int				n;

	for (n=0;n!=max_timer_list;n++) {
		if (js.timer_list.timers[n]!=NULL) free(js.timer_list.timers[n]);
	}
}

int timers_count_list(void)
{
	int				n,count;

	count=0;

	for (n=0;n!=max_timer_list;n++) {
		if (js.timer_list.timers[n]!=NULL) count++;
	}

	return(count);
}

/* =======================================================

      Find Timer
      
======================================================= */

int timers_find(int script_idx,int mode)
{
	int				n;
	timer_type		*timer;
	
	for (n=0;n!=max_timer_list;n++) {
		timer=js.timer_list.timers[n];
		if (timer==NULL) continue;
		
		if ((timer->mode==mode) && (timer->script_idx==script_idx)) return(n);
	}
	
	return(-1);
}

/* =======================================================

      Add a Timer
      
======================================================= */

bool timers_add(int script_idx,int freq,int user_id,char *chain_func_name,int mode,char *err_str)
{
	int				n,idx;
	script_type		*script;
	timer_type		*timer;

		// check for bad frequency
		// no greater than 10 minutes of less than 1 tick

	if ((freq<=0) || (freq>=(10*(10*60)))) {
		sprintf(err_str,"Timer: Illegal frequency: %d\n",freq);
		return(FALSE);
	}

		// if already timer for this script, mark it as disposed
		
	idx=timers_find(script_idx,mode);
	if (idx!=-1) js.timer_list.timers[idx]->mode=timer_mode_dispose;

		// find free timer

	idx=-1;

	for (n=0;n!=max_timer_list;n++) {
		timer=js.timer_list.timers[n];
		if (timer==NULL) {
			idx=n;
			break;
		}
	}

	if (idx==-1) {
		strcpy(err_str,"Timer: Too many timers launched (timers, waits, chains)");
		return(FALSE);
	}

		// create it

	js.timer_list.timers[idx]=(timer_type*)malloc(sizeof(timer_type));
	if (js.timer_list.timers[idx]==NULL) {
		strcpy(err_str,"Timer: Out of memory");
		return(FALSE);
	}

		// setup timer
	
	timer=js.timer_list.timers[idx];
	
	timer->script_idx=script_idx;
	timer->mode=mode;
	
    timer->freq=freq;
    if (timer->freq<1) timer->freq=1;

    timer->count=timer->freq;
    timer->user_id=user_id;
	if (chain_func_name!=NULL) strcpy(timer->chain_func_name,chain_func_name);

		// remember the attachments
		// we use this to re-hook up scripts after a load

	script=js.script_list.scripts[script_idx];
	memmove(&timer->attach,&script->attach,sizeof(script_attach_type));

		// remember the proj_setup/proj swap
		// because chains will need to rebuild it

	timer->override_proj_idx=-1;
	if (script->attach.thing_type==thing_type_projectile) timer->override_proj_idx=script->attach.proj_idx;

	return(TRUE);
}

/* =======================================================

      Remove a Timers
      
======================================================= */

void timers_remove(int idx)
{
	if (js.timer_list.timers[idx]!=NULL) {
		free(js.timer_list.timers[idx]);
		js.timer_list.timers[idx]=NULL;
	}
}

/* =======================================================

      Timer Clear
      
======================================================= */

void timers_clear(int script_idx,int mode)
{
	int				idx;
	
	idx=timers_find(script_idx,mode);
	if (idx!=-1) js.timer_list.timers[idx]->mode=timer_mode_dispose;
}

/* =======================================================

      Dispose of Script Timers
      
======================================================= */

void timers_script_dispose(int script_idx)
{
	int				n;
	timer_type		*timer;
	
	for (n=0;n!=max_timer_list;n++) {
		timer=js.timer_list.timers[n];
		if (timer==NULL) continue;

		if (timer->script_idx==script_idx) timers_remove(n);
	}
}

/* =======================================================

      Fix Timers After Game Restore
      
======================================================= */

void timers_fix_script_indexes(void)
{
	int					n;
	obj_type			*obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	timer_type			*timer;
	
	for (n=0;n!=max_timer_list;n++) {
		timer=js.timer_list.timers[n];
		if (timer==NULL) continue;

		switch (timer->attach.thing_type) {

			case thing_type_game:
				timer->script_idx=js.game_script_idx;
				break;

			case thing_type_course:
				timer->script_idx=js.course_script_idx;
				break;

			case thing_type_object:
				obj=server.obj_list.objs[timer->attach.obj_idx];
				timer->script_idx=obj->script_idx;
				break;
				
			case thing_type_weapon:
				obj=server.obj_list.objs[timer->attach.obj_idx];
				weap=obj->weap_list.weaps[timer->attach.weap_idx];
				timer->script_idx=weap->script_idx;
				break;
				
			case thing_type_projectile:
				obj=server.obj_list.objs[timer->attach.obj_idx];
				weap=obj->weap_list.weaps[timer->attach.weap_idx];
				proj_setup=weap->proj_setup_list.proj_setups[timer->attach.proj_setup_idx];
				timer->script_idx=proj_setup->script_idx;
				break;

		}
	}
}

/* =======================================================

      Run Timer
      
======================================================= */

void timers_run(void)
{
	int				n,ntimer,tick;
	timer_type*		timers[max_timer_list];
	timer_type		*timer;

		// time to run timers?

	tick=game_time_get();
	if (tick<js.timer_tick) return;
	
		// only run current timers
		// don't run new timers created inside these timer calls
		// as that might lead to an infinite loop

		// also, we can only mark timers as disposed and clean up
		// at the end so we don't dangle any pointers

	ntimer=0;

	for (n=0;n!=max_timer_list;n++) {
		timer=js.timer_list.timers[n];
		if (timer==NULL) continue;

		timers[ntimer]=js.timer_list.timers[n];
		ntimer++;
	}
	
		// no timers, so skip ahead
		
	if (ntimer==0) {
		tick=((tick-js.timer_tick)/100)+1;
		js.timer_tick+=(tick*100);
		return;
	}
	
		// run timers

		// we might need to call multiple times
		// for guarenteed 1/10th second calling

	while (tick>=js.timer_tick) {
		js.timer_tick+=100;

		for (n=0;n!=ntimer;n++) {
			timer=timers[n];
			
				// skip timers in dispose
				
			if (timer->mode==timer_mode_dispose) continue;
		
				// time to fire?
				
			timer->count--;
			if (timer->count>0) continue;
			
				// fire timer

			switch (timer->mode) {
			
				case timer_mode_single:
					scripts_post_event_console(timer->script_idx,timer->override_proj_idx,sd_event_wait,0,timer->user_id);
					timer->mode=timer_mode_dispose;		// auto-dispose waits
					break;
					
				case timer_mode_chain:
					scripts_chain_console(timer->script_idx,timer->override_proj_idx,timer->chain_func_name);
					timer->mode=timer_mode_dispose;		// auto-dispose chains
					break;

				case timer_mode_repeat:
					scripts_post_event_console(timer->script_idx,timer->override_proj_idx,sd_event_timer,0,timer->user_id);
					break;
			}
		
				// repeat timer
				
			timer->count=timer->freq;
		}
	}

		// now we can remove any timers that
		// were marked as disposed
		
	for (n=0;n!=max_timer_list;n++) {
		timer=js.timer_list.timers[n];
		if (timer==NULL) continue;
	
		if (timer->mode==timer_mode_dispose) timers_remove(n);
	}
}
	
