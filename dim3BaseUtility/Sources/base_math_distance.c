/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Distance Functions

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
	#include "dim3baseutility.h"
#endif

/* =======================================================

      Get distance
      
======================================================= */

int distance_get(int x,int y,int z,int tox,int toy,int toz)
{
	float		fx,fz,fy;
	
	fx=(float)(x-tox);
	fy=(float)(y-toy);
	fz=(float)(z-toz);
	
	return((int)sqrtf((fx*fx)+(fy*fy)+(fz*fz)));
}

bool distance_check(int x,int y,int z,int tox,int toy,int toz,int max_distance)
{
	float		f,fx,fz,fy,md;
	
	fx=(float)(x-tox);
	fy=(float)(y-toy);
	fz=(float)(z-toz);
	f=(fx*fx)+(fy*fy)+(fz*fz);
	
	md=(float)max_distance;
	if (f>(md*md)) return(FALSE);
	
	return(sqrtf(f)<=md);
}

int distance_2D_get(int x,int z,int tox,int toz)
{
	float		fx,fz;
	
	fx=(float)(x-tox);
	fz=(float)(z-toz);
	
	return((int)sqrtf((fx*fx)+(fz*fz)));
}
