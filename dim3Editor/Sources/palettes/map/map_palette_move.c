/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Movement Move

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

#define kMovePropertyUserID							0
#define kMovePropertyMillisecond					1
#define kMovePropertyMove							2
#define kMovePropertyRot							3
#define kMovePropertySoundName						4
#define kMovePropertySoundPitch						5

extern map_type					map;
extern file_path_setup_type		file_path_setup;
extern iface_type				iface;

extern list_palette_type		map_palette;

/* =======================================================

      Property Palette Fill Movement Move
      
======================================================= */

void map_palette_fill_movement_move(int movement_idx,int move_idx)
{
	char				str[32];
	movement_type		*movement;
	movement_move_type	*move;

	if (move_idx==-1) return;

	movement=&map.movement.movements[movement_idx];
	move=&movement->moves[move_idx];

	sprintf(str,"(%d,%d,%d)",move->mov.x,move->mov.y,move->mov.z);
	list_palette_set_title(&map_palette,"Movement",movement->name,"Move",str,NULL,NULL);

	list_palette_add_header(&map_palette,0,"Movement Move Options");
	list_palette_add_int(&map_palette,kMovePropertyUserID,"Event User Id",&move->user_id,FALSE);
	list_palette_add_int(&map_palette,kMovePropertyMillisecond,"Millisecond Time",&move->msec,FALSE);

	list_palette_add_header(&map_palette,0,"Movement Move Position");
	list_palette_add_point(&map_palette,kMovePropertyMove,"Move",&move->mov,FALSE);
	list_palette_add_angle(&map_palette,kMovePropertyRot,"Rot",&move->rot,FALSE);
	
	list_palette_add_header(&map_palette,0,"Movement Move Sound");
	list_palette_add_picker_list_string(&map_palette,kMovePropertySoundName,"Sound",(char*)iface.sound_list.sounds,iface.sound_list.nsound,sizeof(iface_sound_type),(int)offsetof(iface_sound_type,name),TRUE,TRUE,move->sound_name,FALSE);
	list_palette_add_float(&map_palette,kMovePropertySoundPitch,"Pitch",&move->sound_pitch,FALSE);
}

/* =======================================================

      Property Palette Click Movement Move
      
======================================================= */

void map_palette_click_movement_move(int movement_idx,int move_idx,bool double_click)
{
}
