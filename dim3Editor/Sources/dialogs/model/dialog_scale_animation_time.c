/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog Scale Animation

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
	#include "dim3Editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern model_type		model;

int						dialog_animate_idx;

// controls

#define diag_prop_animation_scale_time_msec		5000
#define diag_prop_animation_scale_time_cancel	5004
#define diag_prop_animation_scale_time_ok		5005

os_dialog_ctrl_type		diag_property_animation_scale_time_ctrls[]={
							{os_dialog_ctrl_type_text_right,0,"Milliseconds:",10,10,75,20},
							{os_dialog_ctrl_type_text_left,0,"Select a number of milliseconds (thousands of a second) for which you want the entire select animation to take place.",10,40,400,40},
							{os_dialog_ctrl_type_text_edit,diag_prop_animation_scale_time_msec,"",90,10,100,20},
							{os_dialog_ctrl_type_button,diag_prop_animation_scale_time_cancel,"Cancel",250,80,80,25},
							{os_dialog_ctrl_type_default_button,diag_prop_animation_scale_time_ok,"Scale",340,80,80,25},
							{-1,-1,"",0,0,0,0}
						};

/* =======================================================

      Animate Time Reset Utility
	        
======================================================= */

int dialog_animation_scale_time_get_total_time(void)
{
	int						n,tot_time;
	model_animate_type		*animate;
	
	animate=&model.animates[dialog_animate_idx];
	
	tot_time=0;
	
	for (n=0;n!=animate->npose_move;n++) {
		tot_time+=animate->pose_moves[n].msec;
	}
	
	return(tot_time);
}

void dialog_animate_scale_change(int tot_time)
{
	int						n,org_tot_time;
	model_animate_type		*animate;

	org_tot_time=dialog_animation_scale_time_get_total_time();
	if (org_tot_time==tot_time) return;
	
	animate=&model.animates[dialog_animate_idx];
	
	for (n=0;n!=animate->npose_move;n++) {
		animate->pose_moves[n].msec=(animate->pose_moves[n].msec*tot_time)/org_tot_time;
	}
}

/* =======================================================

      Run Reset Animation Time
      
======================================================= */

void dialog_property_animation_scale_time_proc(int msg_type,int id)
{
	int				tot_time;

	switch (msg_type) {

		case os_dialog_msg_type_init:
			os_dialog_set_int(diag_prop_animation_scale_time_msec,dialog_animation_scale_time_get_total_time());
			os_dialog_set_focus(diag_prop_animation_scale_time_msec,TRUE);
			break;

		case os_dialog_msg_type_command:

			if (id==diag_prop_animation_scale_time_cancel) {
				os_dialog_close(FALSE);
				return;
			}

			if (id==diag_prop_animation_scale_time_ok) {
				tot_time=os_dialog_get_int(diag_prop_animation_scale_time_msec);
				dialog_animate_scale_change(tot_time);
				os_dialog_close(TRUE);
				return;
			}

			break;
	}
}

bool dialog_animation_scale_time_run(int animate_idx)
{
	dialog_animate_idx=animate_idx;

	return(os_dialog_run("Scale Animation Time",425,110,diag_property_animation_scale_time_ctrls,dialog_property_animation_scale_time_proc));
}
