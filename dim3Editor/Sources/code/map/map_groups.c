/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Group Utility Routines

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

extern map_type					map;

/* =======================================================

      Group Counts
      
======================================================= */

int group_count(int group_idx)
{
	int					n,cnt;
	map_mesh_type		*mesh;
	map_liquid_type		*liq;
	
	cnt=0;
	
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		if (mesh->group_idx==group_idx) cnt++;
		mesh++;
	}
	
	liq=map.liquid.liquids;
	
	for (n=0;n!=map.liquid.nliquid;n++) {
		if (liq->group_idx==group_idx) cnt++;
		liq++;
	}
	
	return(cnt);
}

/* =======================================================

      Group Sets
      
======================================================= */

void group_set(int group_idx)
{
	int				n,sel_count,type,main_idx,sub_idx;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&main_idx,&sub_idx);
		
		switch (type) {
		
			case item_map_mesh:
				map.mesh.meshes[main_idx].group_idx=group_idx;
				break;
				
			case item_map_liquid:
				map.liquid.liquids[main_idx].group_idx=group_idx;
				break;
				
		}
	}
}

void group_clear(int group_idx,bool delete_shift)
{
	int					n;
	map_mesh_type		*mesh;
	map_liquid_type		*liq;
	
		// clear group from meshes and liquids
		
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		if (mesh->group_idx==group_idx) mesh->group_idx=-1;
		if (delete_shift) {
			if (mesh->group_idx>group_idx) mesh->group_idx--;
		}
		mesh++;
	}
	
	liq=map.liquid.liquids;
	
	for (n=0;n!=map.liquid.nliquid;n++) {
		if (liq->group_idx==group_idx) liq->group_idx=-1;
		if (delete_shift) {
			if (liq->group_idx>group_idx) liq->group_idx--;
		}
		liq++;
	}
}

/* =======================================================

      Shift Movement Moves
      
======================================================= */

int map_shift_movement_move(int movement_idx,int move_idx,int dir)
{
	int						idx;
	movement_type			*movement;
	movement_move_type		temp_move;
	
	movement=&map.movement.movements[movement_idx];

		// can't move if at top or bottom
		
	if ((dir==-1) && (move_idx==0)) return(move_idx);
	if ((dir==1) && (move_idx==(movement->nmove-1))) return(move_idx);
	
		// shift move
		
	idx=move_idx+dir;
	memmove(&temp_move,&movement->moves[move_idx],sizeof(movement_move_type));
	memmove(&movement->moves[move_idx],&movement->moves[idx],sizeof(movement_move_type));
	memmove(&movement->moves[idx],&temp_move,sizeof(movement_move_type));
	
	return(idx);
}
