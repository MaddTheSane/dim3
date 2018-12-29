/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Cinema

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

/* =======================================================

      Add and Delete Map Cinema
      
======================================================= */

int map_cinema_add(map_type *map)
{
	int					cinema_idx;
	map_cinema_type		*nptr,*cinema;
	
	cinema_idx=map->cinema.ncinema;

		// create new memory

	if (map->cinema.ncinema==0) {
		map->cinema.cinemas=(map_cinema_type*)malloc(sizeof(map_cinema_type));
		if (map->cinema.cinemas==NULL) return(-1);
	}
	else {
		nptr=(map_cinema_type*)malloc((map->cinema.ncinema+1)*sizeof(map_cinema_type));
		if (nptr==NULL) return(-1);

		memmove(nptr,map->cinema.cinemas,(map->cinema.ncinema*sizeof(map_cinema_type)));
		free(map->cinema.cinemas);

		map->cinema.cinemas=nptr;
	}
	
	cinema=&map->cinema.cinemas[cinema_idx];
	
	strcpy(cinema->name,"New Cinema");
	cinema->len_msec=0;
	cinema->freeze_input=TRUE;
	cinema->show_hud=FALSE;
	cinema->no_cancel=FALSE;
	
	cinema->naction=0;
	cinema->actions=NULL;
	
	map->cinema.ncinema++;

	return(cinema_idx);
}

bool map_cinema_delete(map_type *map,int cinema_idx)
{
	int					sz;
	map_cinema_type		*nptr;

		// free actions

	if (map->cinema.cinemas[cinema_idx].actions!=NULL) free(map->cinema.cinemas[cinema_idx].actions);

		// free cinema
	
	if (map->cinema.ncinema<=1) {
		map->cinema.ncinema=0;
		free(map->cinema.cinemas);
		map->cinema.cinemas=NULL;
		return(TRUE);
	}

	nptr=(map_cinema_type*)malloc((map->cinema.ncinema-1)*sizeof(map_cinema_type));
	if (nptr==NULL) return(FALSE);

	if (cinema_idx>0) {
		sz=cinema_idx*sizeof(map_cinema_type);
		memmove(nptr,map->cinema.cinemas,sz);
	}

	sz=((map->cinema.ncinema-cinema_idx)-1)*sizeof(map_cinema_type);
	if (sz>0) memmove(&nptr[cinema_idx],&map->cinema.cinemas[cinema_idx+1],sz);

	free(map->cinema.cinemas);

	map->cinema.cinemas=nptr;
	map->cinema.ncinema--;

	return(TRUE);
}

/* =======================================================

      Add and Delete Cinema Action
      
======================================================= */

int map_cinema_add_action(map_type *map,int cinema_idx)
{
	int						action_idx;
	map_cinema_type			*cinema;
	map_cinema_action_type	*nptr,*action;
	
	cinema=&map->cinema.cinemas[cinema_idx];

	action_idx=cinema->naction;

		// create new memory

	if (cinema->naction==0) {
		cinema->actions=(map_cinema_action_type*)malloc(sizeof(map_cinema_action_type));
		if (cinema->actions==NULL) return(-1);
	}
	else {
		nptr=(map_cinema_action_type*)malloc((cinema->naction+1)*sizeof(map_cinema_action_type));
		if (nptr==NULL) return(-1);

		memmove(nptr,cinema->actions,(cinema->naction*sizeof(map_cinema_action_type)));
		free(cinema->actions);

		cinema->actions=nptr;
	}
	
	action=&cinema->actions[action_idx];
	
	action->action=cinema_action_none;
	action->actor_type=cinema_actor_camera;
	action->actor_name[0]=0x0;
	action->start_msec=action->end_msec=0;
	action->animation_name[0]=0x0;
	action->next_animation_name[0]=0x0;
	action->node_name[0]=0x0;
	action->text_str[0]=0x0;
	action->move_reverse=FALSE;
	
	cinema->naction++;

	return(action_idx);
}

bool map_cinema_delete_action(map_type *map,int cinema_idx,int action_idx)
{
	int						sz;
	map_cinema_type			*cinema;
	map_cinema_action_type	*nptr;
	
	cinema=&map->cinema.cinemas[cinema_idx];

	if (cinema->naction<=1) {
		cinema->naction=0;
		free(cinema->actions);
		return(TRUE);
	}

	nptr=(map_cinema_action_type*)malloc((cinema->naction-1)*sizeof(map_cinema_action_type));
	if (nptr==NULL) return(FALSE);

	if (action_idx>0) {
		sz=action_idx*sizeof(map_cinema_action_type);
		memmove(nptr,cinema->actions,sz);
	}

	sz=((cinema->naction-action_idx)-1)*sizeof(map_cinema_action_type);
	if (sz>0) memmove(&nptr[action_idx],&cinema->actions[action_idx+1],sz);

	free(cinema->actions);

	cinema->actions=nptr;
	cinema->naction--;

	return(TRUE);
}
