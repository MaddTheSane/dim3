/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Spot Routines

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

      Find spots
      
======================================================= */

int map_count_spot(map_type *map,char *name,int type)
{
	int				n,nspot,count;
	spot_type		*spot;
	
	count=0;
	nspot=map->nspot;
	
	for (n=0;n!=nspot;n++) {
		spot=&map->spots[n];
		
		if (name!=NULL) if (strcmp(spot->name,name)!=0) continue;
		if (type!=-1) if (spot->type!=type) continue;
		
		count++;
	}
	
	return(count);
}

int map_find_spot(map_type *map,char *name,int type)
{
	int				n,nspot;
	spot_type		*spot;
	
	nspot=map->nspot;
	
	for (n=0;n!=nspot;n++) {
		spot=&map->spots[n];
		
		if (name!=NULL) if (strcmp(spot->name,name)!=0) continue;
		if (type!=-1) if (spot->type!=type) continue;
		
		return(n);
	}
	
	return(-1);
}

/* =======================================================

      Random Spot Find
      
======================================================= */

void map_find_random_spot_clear(map_type *map,char *name,int type)
{
	int				n,nspot;
	spot_type		*spot;
	
	nspot=map->nspot;
	
	for (n=0;n!=nspot;n++) {
		spot=&map->spots[n];
		
		if (name!=NULL) if (strcmp(spot->name,name)!=0) continue;
		if (type!=-1) if (spot->type!=type) continue;

		spot->lookup.random_hit=FALSE;
	}
}

int map_find_random_spot(map_type *map,char *name,int type)
{
	int				n,nspot,count,spot_count,free_spot_count;
	spot_type		*spot;
	
		// count the spots
		
	spot_count=0;
	free_spot_count=0;
	
	nspot=map->nspot;

	for (n=0;n!=nspot;n++) {
		spot=&map->spots[n];
		
		if (name!=NULL) if (strcmp(spot->name,name)!=0) continue;
		if (type!=-1) if (spot->type!=type) continue;
		
		spot_count++;
		if (!spot->lookup.random_hit) free_spot_count++;
	}
	
	if (spot_count==0) return(-1);
	
		// do we need to clear the round robin flags?
		
	if (free_spot_count==0) {
		map_find_random_spot_clear(map,name,type);
		free_spot_count=spot_count;
	}
	
		// find random number in count
		
	count=random_int(free_spot_count);
	
	for (n=0;n!=nspot;n++) {
		spot=&map->spots[n];
		if (spot->lookup.random_hit) continue;
		
		if (name!=NULL) if (strcmp(spot->name,name)!=0) continue;
		if (type!=-1) if (spot->type!=type) continue;
		
		if (count==0) {
			spot->lookup.random_hit=TRUE;
			return(n);
		}
		
		count--;
	}
	
	return(-1);
}

/* =======================================================

      Find Lights
      
======================================================= */

int map_find_light(map_type *map,char *name)
{
	int					n,nlight;
	map_light_type		*light;
	
	nlight=map->nlight;
	light=map->lights;
	
	for (n=0;n!=nlight;n++) {
		if (strcmp(light->name,name)==0) return(n);
		light++;
	}
	
	return(-1);
}
