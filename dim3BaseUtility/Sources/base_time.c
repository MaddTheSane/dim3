/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
	    Keith Bauer for Mac/Linux Timing Code
 Usage: Timing

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
	#include "dim3baseutility.h"
#endif

#if defined(D3_OS_MAC) || defined(D3_OS_IPHONE)
	#include <mach/mach.h>
	#include <mach/clock.h>

	int				start_time;
	clock_serv_t	sys_clock;
#endif

#ifdef D3_OS_WINDOWS
	double			start_time,freq_time;
#endif

#ifdef D3_OS_LINUX
    double           start_time;
#endif

/* =======================================================

      Mac and Linux Low-Level Time Routines
      
======================================================= */

#if defined(D3_OS_MAC) || defined(D3_OS_IPHONE)

int time_now(void)
{
	double				dsec,dns;
	mach_timespec_t		now;
	
	clock_get_time(sys_clock,&now);

	dsec=((double)now.tv_sec)*1000.0;
	dns=((double)now.tv_nsec)*(1.0/1000000.0);
	
	return((int)(dsec+dns));
}

void time_start(void)
{
	host_get_clock_service(mach_host_self(),SYSTEM_CLOCK,&sys_clock);
	
	start_time=time_now();
}

int time_get(void)
{
	return(time_now()-start_time);
}

#endif

/* =======================================================

      Windows Low-Level Time Routines
      
======================================================= */

#ifdef D3_OS_WINDOWS

void time_start(void)
{
	LARGE_INTEGER		time;

	QueryPerformanceFrequency(&time);
	freq_time=(double)time.QuadPart;

	QueryPerformanceCounter(&time);
	start_time=(double)time.QuadPart;
}

int time_get(void)
{
	double				d;
	LARGE_INTEGER		time;

	QueryPerformanceCounter(&time);
	d=(double)time.QuadPart;

	return((int)(((d-start_time)/freq_time)*1000.0));
}

#endif

/* =======================================================

      Linux Low-Level Time Routines
      
======================================================= */

#ifdef D3_OS_LINUX

double time_now()
{
    struct timeval		now;
	
    gettimeofday(&now,NULL);
    return((double)now.tv_sec+(double)now.tv_usec*0.000001);
}

void time_start(void)
{
	start_time=time_now();
}

int time_get(void)
{
	return((int)((time_now()-start_time)*1000.0));
}

#endif
