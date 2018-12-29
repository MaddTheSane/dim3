/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Movement

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

#define kMovementPropertyName				0
#define kMovementPropertyAutoOpenDistance	1
#define kMovementPropertyGroup				2
#define kMovementPropertyReverseGroup		3
#define kMovementPropertyAutoStart			4
#define kMovementPropertyAutoOpen			5
#define kMovementPropertyAutoOpenStand		6
#define kMovementPropertyAutoOpenNeverClose	7
#define kMovementPropertyLoop				8

#define kMovementPropertyMoveAdd			10

#define kMovementPropertyMove				1000
#define kMovementPropertyMoveDelete			2000
#define kMovementPropertyMoveUp				3000
#define kMovementPropertyMoveDown			4000

extern map_type					map;
extern app_state_type			state;

extern list_palette_type		map_palette;

/* =======================================================

      Property Palette Fill Movement
      
======================================================= */

void map_palette_fill_movement(int movement_idx)
{
	int					n;
	char				str[256];
	movement_type		*movement;
	movement_move_type	*move;
	
	movement=&map.movement.movements[movement_idx];

	list_palette_set_title(&map_palette,"Movement",movement->name,NULL,NULL,NULL,NULL);

	list_palette_add_header(&map_palette,0,"Movement Options");
	list_palette_add_string(&map_palette,kMovementPropertyName,"Name",movement->name,name_str_len,FALSE);
	list_palette_add_int(&map_palette,kMovementPropertyAutoOpenDistance,"Auto-Open Distance",&movement->auto_open_distance,FALSE);

	list_palette_add_header(&map_palette,0,"Movement Groups");
	list_palette_add_picker_list_int(&map_palette,kMovementPropertyGroup,"Group",(char*)map.group.groups,map.group.ngroup,sizeof(group_type),(int)offsetof(group_type,name),TRUE,TRUE,&movement->group_idx,FALSE);
	list_palette_add_picker_list_int(&map_palette,kMovementPropertyReverseGroup,"Reverse Group",(char*)map.group.groups,map.group.ngroup,sizeof(group_type),(int)offsetof(group_type,name),TRUE,TRUE,&movement->reverse_group_idx,FALSE);

	list_palette_add_header(&map_palette,0,"Movement Settings");
	list_palette_add_checkbox(&map_palette,kMovementPropertyAutoStart,"Auto-Start",&movement->auto_start,FALSE);
	list_palette_add_checkbox(&map_palette,kMovementPropertyAutoOpen,"Auto-Open on Distance",&movement->auto_open,FALSE);
	list_palette_add_checkbox(&map_palette,kMovementPropertyAutoOpenStand,"Auto-Open on Stand",&movement->auto_open_stand,FALSE);
	list_palette_add_checkbox(&map_palette,kMovementPropertyAutoOpenNeverClose,"Never Close After Auto-Open",&movement->auto_open_never_close,FALSE);
	list_palette_add_checkbox(&map_palette,kMovementPropertyLoop,"Looping",&movement->loop,FALSE);

	list_palette_add_header_button(&map_palette,kMovementPropertyMoveAdd,"Movement Moves",list_button_plus);

	move=movement->moves;

	for (n=0;n!=movement->nmove;n++) {
		sprintf(str,"(%d,%d,%d)(%d,%d,%d)@%d",move->mov.x,move->mov.y,move->mov.z,(int)move->rot.x,(int)move->rot.y,(int)move->rot.z,move->msec);
		list_palette_add_string_selectable_moveable_button(&map_palette,(kMovementPropertyMove+n),list_button_minus,(kMovementPropertyMoveDelete+n),(kMovementPropertyMoveUp+n),(kMovementPropertyMoveDown+n),str,(state.map.cur_movement_move_idx==n),FALSE);
		move++;
	}
}

/* =======================================================

      Property Palette Click Movement
      
======================================================= */

void map_palette_click_movement(bool double_click)
{
	int					id;
	movement_type		*movement;

	id=map_palette.item_pane.click.id;
	movement=&map.movement.movements[state.map.cur_movement_idx];

		// click move

	if ((id>=kMovementPropertyMove) && (id<kMovementPropertyMoveDelete)) {
		state.map.cur_movement_move_idx=id-kMovementPropertyMove;
		if (double_click) list_palette_set_level(&map_palette,2);
		return;
	}

		// delete move

	if ((id>=kMovementPropertyMoveDelete) && (id<kMovementPropertyMoveUp)) {
		state.map.cur_movement_move_idx=-1;
		map_movement_move_delete(&map,state.map.cur_movement_idx,(id-kMovementPropertyMoveDelete));
		return;
	}
	
		// move pose up
		
	if ((id>=kMovementPropertyMoveUp) && (id<kMovementPropertyMoveDown)) {
		state.map.cur_movement_move_idx=map_shift_movement_move(state.map.cur_movement_idx,(id-kMovementPropertyMoveUp),-1);
		return;
	}
	
		// move pose down
		
	if (id>=kMovementPropertyMoveDown) {
		state.map.cur_movement_move_idx=map_shift_movement_move(state.map.cur_movement_idx,(id-kMovementPropertyMoveDown),1);
		return;
	}

		// add move

	if (id==kMovementPropertyMoveAdd) {
		state.map.cur_movement_move_idx=map_movement_move_add(&map,state.map.cur_movement_idx);
		list_palette_set_level(&map_palette,2);
		return;
	}
}

