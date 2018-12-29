/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Liquids

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

      Add Liquid to Map
      
======================================================= */

int map_liquid_add(map_type *map)
{
	int					liquid_idx;
	map_liquid_type		*nptr,*liq;
	
	liquid_idx=map->liquid.nliquid;

		// create new memory

	if (map->liquid.nliquid==0) {
		map->liquid.liquids=(map_liquid_type*)malloc(sizeof(map_liquid_type));
		if (map->liquid.liquids==NULL) return(-1);
	}
	else {
		nptr=(map_liquid_type*)malloc((map->liquid.nliquid+1)*sizeof(map_liquid_type));
		if (nptr==NULL) return(-1);

		memmove(nptr,map->liquid.liquids,(map->liquid.nliquid*sizeof(map_liquid_type)));
		free(map->liquid.liquids);

		map->liquid.liquids=nptr;
	}
	
	liq=&map->liquid.liquids[liquid_idx];
	
	liq->flag.lock_uv=FALSE;
	liq->flag.never_obscure=FALSE;
	liq->flag.never_cull=FALSE;
	liq->flag.no_draw=FALSE;

	liq->wave.on=FALSE;
	liq->wave.dir_north_south=FALSE;
	liq->wave.length=2000;
	liq->wave.high=200;
	liq->wave.period_msec=3000;

	liq->overlay.on=FALSE;

	liq->copy.copied=FALSE;
	
	liq->camera[0]=0x0;
	liq->ambient.sound_name[0]=0x0;
	
	map->liquid.nliquid++;

	return(liquid_idx);
}

bool map_liquid_delete(map_type *map,int liquid_idx)
{
	int					sz;
	map_liquid_type		*nptr;
	
	if (map->liquid.nliquid<=1) {
		map->liquid.nliquid=0;
		free(map->liquid.liquids);
		return(TRUE);
	}

	nptr=(map_liquid_type*)malloc((map->liquid.nliquid-1)*sizeof(map_liquid_type));
	if (nptr==NULL) return(FALSE);

	if (liquid_idx>0) {
		sz=(liquid_idx+1)*sizeof(map_liquid_type);
		memmove(nptr,map->liquid.liquids,sz);
	}

	sz=(map->liquid.nliquid-liquid_idx)*sizeof(map_liquid_type);
	if (sz>0) memmove(&nptr[liquid_idx],&map->liquid.liquids[liquid_idx+1],sz);

	free(map->liquid.liquids);

	map->liquid.liquids=nptr;
	map->liquid.nliquid--;

	return(TRUE);
}

/* =======================================================

      Duplicate Liquid
      
======================================================= */

void map_liquid_create_copy_data(map_type *map,int liquid_idx)
{
	map_liquid_type		*liq;
	
	liq=&map->liquid.liquids[liquid_idx];
	if (liq->copy.copied) return;

	liq->copy.copied=TRUE;
	liq->copy.y=liq->y;
	liq->copy.lft=liq->lft;
	liq->copy.rgt=liq->rgt;
	liq->copy.top=liq->top;
	liq->copy.bot=liq->bot;
}

/* =======================================================

      Duplicate Liquid
      
======================================================= */

int map_liquid_duplicate(map_type *map,int liquid_idx)
{
	int					new_liquid_idx;
	map_liquid_type		*liq,*liq2;
	
	new_liquid_idx=map_liquid_add(map);
	if (new_liquid_idx==-1) return(-1);
	
	liq=&map->liquid.liquids[liquid_idx];
	liq2=&map->liquid.liquids[new_liquid_idx];
	memmove(liq2,liq,sizeof(map_liquid_type));

	return(new_liquid_idx);
}

/* =======================================================

      Liquid Info
      
======================================================= */

void map_liquid_calculate_center(map_type *map,int liquid_idx,d3pnt *pt)
{
	map_liquid_type			*liq;
	
	liq=&map->liquid.liquids[liquid_idx];

	pt->x=(liq->lft+liq->rgt)>>1;
	pt->y=liq->y;
	pt->z=(liq->top+liq->bot)>>1;
}

/* =======================================================

      Liquid Distance
      
======================================================= */

int map_liquid_calculate_distance(map_liquid_type *liq,d3pnt *pnt)
{
	float			fx,fy,fz;
	
		// find collision points
		
	if ((pnt->x>=liq->lft) && (pnt->x<=liq->rgt)) {
		fx=0.0f;
	}
	else {
		if (pnt->x<liq->lft) {
			fx=(float)(liq->lft-pnt->x);
		}
		else {
			fx=(float)(pnt->x-liq->rgt);
		}
	}
	
	fy=(float)(liq->y-pnt->y);
	
	if ((pnt->z>=liq->top) && (pnt->z<=liq->bot)) {
		fz=0.0f;
	}
	else {
		if (pnt->z<liq->top) {
			fz=(float)(liq->top-pnt->z);
		}
		else {
			fz=(float)(pnt->z-liq->bot);
		}
	}
	
	return((int)sqrtf((fx*fx)+(fy*fy)+(fz*fz)));
}

