/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog Play Blended Animation

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern model_type			model;
extern app_state_type		state;

// controls

#define diag_prop_blend_animation_1			5000
#define diag_prop_blend_animation_2			5001
#define diag_prop_blend_animation_3			5002
#define diag_prop_blend_animation_4			5003
#define diag_prop_blend_animation_cancel	5004
#define diag_prop_blend_animation_ok		5005

os_dialog_ctrl_type		diag_property_blend_animation_ctrls[]={
							{os_dialog_ctrl_type_text_right,0,"Animation 1:",10,10,75,20},
							{os_dialog_ctrl_type_combo,diag_prop_blend_animation_1,"",90,10,310,20},
							{os_dialog_ctrl_type_text_right,0,"Animation 2:",10,40,75,20},
							{os_dialog_ctrl_type_combo,diag_prop_blend_animation_2,"",90,40,310,20},
							{os_dialog_ctrl_type_text_right,0,"Animation 3:",10,70,75,20},
							{os_dialog_ctrl_type_combo,diag_prop_blend_animation_3,"",90,70,310,20},
							{os_dialog_ctrl_type_text_right,0,"Animation 4:",10,100,75,20},
							{os_dialog_ctrl_type_combo,diag_prop_blend_animation_4,"",90,100,310,20},
							{os_dialog_ctrl_type_button,diag_prop_blend_animation_cancel,"Cancel",250,130,80,25},
							{os_dialog_ctrl_type_default_button,diag_prop_blend_animation_ok,"Play",340,130,80,25},
							{-1,-1,"",0,0,0,0}
						};

/* =======================================================

      Set and Get Animation Combo
      
======================================================= */

void dialog_blend_animate_set_combo(int id,int sel_idx,bool use_none)
{
	int				n;
	
	os_dialog_combo_clear(id);

	if (use_none) os_dialog_combo_add(id,"None");
	
	for (n=0;n!=model.nanimate;n++) {
		os_dialog_combo_add(id,model.animates[n].name);
	}

	if (!use_none) {
		if (sel_idx<0) sel_idx=0;
		os_dialog_combo_set_value(id,sel_idx);
	}
	else {
		if (sel_idx==-1) {
			os_dialog_combo_set_value(id,0);
		}
		else {
			os_dialog_combo_set_value(id,(sel_idx+1));
		}
	}
}

int dialog_blend_animate_get_combo(int id,bool use_none)
{
	int				sel_idx;
	
	sel_idx=os_dialog_combo_get_value(id);
	if (!use_none) return(sel_idx);

	if (sel_idx==0) return(-1);
	
	return(sel_idx-1);
}

/* =======================================================

      Run Blended Animation
      
======================================================= */

void dialog_property_blend_animation_proc(int msg_type,int id)
{
	switch (msg_type) {

		case os_dialog_msg_type_init:
			dialog_blend_animate_set_combo(diag_prop_blend_animation_1,state.model.blend[0].animate_idx,FALSE);
			dialog_blend_animate_set_combo(diag_prop_blend_animation_2,state.model.blend[1].animate_idx,TRUE);
			dialog_blend_animate_set_combo(diag_prop_blend_animation_3,state.model.blend[2].animate_idx,TRUE);
			dialog_blend_animate_set_combo(diag_prop_blend_animation_4,state.model.blend[3].animate_idx,TRUE);
			break;

		case os_dialog_msg_type_command:

			if (id==diag_prop_blend_animation_cancel) {
				os_dialog_close(FALSE);
				return;
			}

			if (id==diag_prop_blend_animation_ok) {
				state.model.blend[0].animate_idx=dialog_blend_animate_get_combo(diag_prop_blend_animation_1,FALSE);
				state.model.blend[1].animate_idx=dialog_blend_animate_get_combo(diag_prop_blend_animation_2,TRUE);
				state.model.blend[2].animate_idx=dialog_blend_animate_get_combo(diag_prop_blend_animation_3,TRUE);
				state.model.blend[3].animate_idx=dialog_blend_animate_get_combo(diag_prop_blend_animation_4,TRUE);
				os_dialog_close(TRUE);
				return;
			}

			break;
	}
}

bool dialog_play_blend_animation_run(void)
{
	return(os_dialog_run("Play Blended Animations",425,160,diag_property_blend_animation_ctrls,dialog_property_blend_animation_proc));
}
