/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Movements

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

/* =======================================================

      Map Movements
      
======================================================= */

int map_movement_add(map_type *map)
{
	int					movement_idx;
	movement_type		*nptr,*movement;
	
	movement_idx=map->movement.nmovement;

		// create new memory

	if (map->movement.nmovement==0) {
		map->movement.movements=(movement_type*)malloc(sizeof(movement_type));
		if (map->movement.movements==NULL) return(-1);
	}
	else {
		nptr=(movement_type*)malloc((map->movement.nmovement+1)*sizeof(movement_type));
		if (nptr==NULL) return(-1);

		memmove(nptr,map->movement.movements,(map->movement.nmovement*sizeof(movement_type)));
		free(map->movement.movements);

		map->movement.movements=nptr;
	}
	
	movement=&map->movement.movements[movement_idx];
	
	strcpy(movement->name,"New Movement");
	movement->group_idx=-1;
	movement->reverse_group_idx=-1;
	movement->auto_start=FALSE;
	movement->auto_open=FALSE;
	movement->auto_open_stand=FALSE;
	movement->auto_open_never_close=FALSE;
	movement->auto_open_distance=20000;
	movement->loop=FALSE;
	
	movement->nmove=0;
	movement->moves=NULL;
	
	map->movement.nmovement++;

	return(movement_idx);
}

bool map_movement_delete(map_type *map,int movement_idx)
{
	int					sz;
	movement_type		*nptr;

		// free moves

	if (map->movement.movements[movement_idx].moves!=NULL) free(map->movement.movements[movement_idx].moves);
	
		// free movement

	if (map->movement.nmovement<=1) {
		map->movement.nmovement=0;
		free(map->movement.movements);
		map->movement.movements=NULL;
		return(TRUE);
	}

	nptr=(movement_type*)malloc((map->movement.nmovement-1)*sizeof(movement_type));
	if (nptr==NULL) return(FALSE);

	if (movement_idx>0) {
		sz=movement_idx*sizeof(movement_type);
		memmove(nptr,map->movement.movements,sz);
	}

	sz=((map->movement.nmovement-movement_idx)-1)*sizeof(movement_type);
	if (sz>0) memmove(&nptr[movement_idx],&map->movement.movements[movement_idx+1],sz);

	free(map->movement.movements);

	map->movement.movements=nptr;
	map->movement.nmovement--;

	return(TRUE);
}

/* =======================================================

      Map Movements Moves
      
======================================================= */

int map_movement_move_add(map_type *map,int movement_idx)
{
	int						move_idx;
	movement_type			*movement;
	movement_move_type		*nptr,*move;
	
	movement=&map->movement.movements[movement_idx];

	move_idx=movement->nmove;

		// create new memory

	if (movement->nmove==0) {
		movement->moves=(movement_move_type*)malloc(sizeof(movement_move_type));
		if (movement->moves==NULL) return(-1);
	}
	else {
		nptr=(movement_move_type*)malloc((movement->nmove+1)*sizeof(movement_move_type));
		if (nptr==NULL) return(-1);

		memmove(nptr,movement->moves,(movement->nmove*sizeof(movement_move_type)));
		free(movement->moves);

		movement->moves=nptr;
	}
	
	move=&movement->moves[move_idx];
	
	move->msec=1000;
	move->mov.x=move->mov.y=move->mov.z=0;
	move->rot.x=move->rot.y=move->rot.z=0.0f;
	move->user_id=0;
	move->sound_name[0]=0x0;
	move->sound_pitch=1.0f;
	
	movement->nmove++;

	return(move_idx);
}

bool map_movement_move_delete(map_type *map,int movement_idx,int move_idx)
{
	int						sz;
	movement_type			*movement;
	movement_move_type		*nptr;
	
	movement=&map->movement.movements[movement_idx];

	if (movement->nmove<=1) {
		movement->nmove=0;
		free(movement->moves);
		return(TRUE);
	}

	nptr=(movement_move_type*)malloc((movement->nmove-1)*sizeof(movement_move_type));
	if (nptr==NULL) return(FALSE);

	if (move_idx>0) {
		sz=move_idx*sizeof(movement_move_type);
		memmove(nptr,movement->moves,sz);
	}

	sz=((movement->nmove-move_idx)-1)*sizeof(movement_move_type);
	if (sz>0) memmove(&nptr[move_idx],&movement->moves[move_idx+1],sz);

	free(movement->moves);

	movement->moves=nptr;
	movement->nmove--;

	return(TRUE);
}

/* =======================================================

      Find Map Movements
      
======================================================= */

int map_movement_find(map_type *map,char *name)
{
	int				n,nmovement;
	movement_type	*movement;
	
	nmovement=map->movement.nmovement;
	movement=map->movement.movements;
	
	for (n=0;n!=nmovement;n++) {
		if (strcmp(movement->name,name)==0) return(n);
		movement++;
	}
	
	return(-1);
}

