/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Auto Generation Utility

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

(c) 2000-2014 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3autogenerate.h"
#endif

int					ag_seed;

/* =======================================================

      Random Utilities
      
======================================================= */

void ag_random_seed(void)
{
#ifdef D3_OS_WINDOWS
	ag_seed=GetTickCount();
#else
	ag_seed=TickCount();
#endif

	srandom(ag_seed);
}

int ag_random_get_seed(void)
{
	return(ag_seed);
}

void ag_random_previous_seed(void)
{
	ag_seed--;
	srandom(ag_seed);
}

void ag_random_next_seed(void)
{
	ag_seed++;
	srandom(ag_seed);
}

int ag_random_int(int max)
{
	float			f;

	f=(float)random()/(float)RAND_MAX;
	return((int)(f*(float)max));
}

bool ag_random_bool(void)
{
	return(ag_random_int(100)>=50);
}
