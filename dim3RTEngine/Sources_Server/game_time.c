/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Game Timing

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
	#include "dim3engine.h"
#endif

int							raw_tick,raw_last_tick,
							game_tick,game_last_tick;

extern server_type			server;

/* =======================================================

      Start Game Time
      
======================================================= */

void game_time_initialize(void)
{
	time_start();

	raw_tick=game_tick=0;
	raw_last_tick=game_last_tick=time_get();
	
	server.time.paused=FALSE;
}

/* =======================================================

      Calculate and Set Game Tick
      
======================================================= */

void game_time_calculate(void)
{
	int			current_time;

	current_time=time_get();

		// raw ticks can't be paused
		// used for view drawing and input which
		// can work through a pause

	raw_tick+=(current_time-raw_last_tick);
	raw_last_tick=current_time;
	
		// server ticks can be paused, used
		// for physics, scripts, etc

	if (!server.time.paused) {
		game_tick+=(current_time-game_last_tick);
		game_last_tick=current_time;
	}
}

void game_time_reset(int tick)
{
	game_tick=tick;
	game_last_tick=time_get();
}

/* =======================================================

      Get Game Tick
      
======================================================= */

int game_time_get(void)
{
	return(game_tick);
}

int game_time_get_raw(void)
{
	return(raw_tick);
}

float game_time_fequency_second_get(int start_tick)
{
	int				k;
	
	k=(raw_tick-start_tick)%2000;
	if (k>=1000) k=2000-k;
	
	return(((float)k)/1000.0f);
}

/* =======================================================

      Start and Stop Pauses
      
======================================================= */

void game_time_pause_start(void)
{
	server.time.paused=TRUE;
}

void game_time_pause_end(void)
{
	if (!server.time.paused) return;
	
	game_last_tick=time_get();
	server.time.paused=FALSE;
}

