/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Network Score Limit

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
	#include "dim3engine.h"
#endif

#define SCORE_LIMIT_EXTRA_SECOND_FAIL	5

#include "interface.h"
#include "network.h"
#include "scripts.h"
#include "objects.h"

extern server_type			server;
extern map_type				map;
extern iface_type			iface;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

int							score_limit_start_tick;

extern void remote_host_exit(void);
extern void network_score_draw(void);

/* =======================================================

      Open/Close Score Limits
      
======================================================= */

void score_limit_open(void)
{
	score_limit_start_tick=game_time_get_raw();

	game_time_pause_start();
}

void score_limit_close(void)
{
	game_time_pause_end();
}

/* =======================================================

      Check for Score Limits
      
======================================================= */

void score_limit_start(void)
{
	obj_type		*player_obj;
	
		// goto score limit state
		
	server.next_state=gs_score_limit;
	
		// push score limit to all remotes

	player_obj=server.obj_list.objs[server.player_obj_idx];
	net_host_player_send_message_to_clients_all(NULL,net_action_request_game_score_limit,NULL,0);
}

void score_limit_check_scores(void)
{
	int				n,limit,red_score,blue_score;
	obj_type		*obj;

		// only check score limits
		// during network games on the host

	if (net_setup.mode!=net_mode_host) return;

		// is limit already on?

	if (server.state==gs_score_limit) return;

		// get score limit
		
	limit=net_setup.score_limit;
	if (limit==0) return;

		// team checks?
		
	if (iface.multiplayer.game_list.games[net_setup.game_idx].use_teams) {
		
		red_score=blue_score=0;

		for (n=0;n!=max_obj_list;n++) {
			obj=server.obj_list.objs[n];
			if (obj==NULL) continue;
		
			if ((obj->type!=object_type_player) && (obj->type!=object_type_remote_player) && (obj->type!=object_type_bot_multiplayer)) continue;
			
			if (obj->team_idx==net_team_red) {
				red_score+=obj->score.score;
				if (red_score>=limit) {
					score_limit_start();
					return;
				}
			}
			else {
				blue_score+=obj->score.score;
				if (blue_score>=limit) {
					score_limit_start();
					return;
				}
			}
		}
		
		return;
	}
	
		// regular player checks

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if ((obj->type==object_type_player) || (obj->type==object_type_remote_player) || (obj->type==object_type_bot_multiplayer)) {
			if (obj->score.score>=limit) {
				score_limit_start();
				return;
			}
		}
	}
}

/* =======================================================

      Resume Timing
      
======================================================= */

int score_limit_get_resume_time(void)
{
	int			secs;
	
	secs=net_setup.game_reset_secs-((game_time_get_raw()-score_limit_start_tick)/1000);
	if (secs<0) secs=0;
	
	return(secs);
}

int score_limit_get_resume_tick(void)
{
	return(score_limit_start_tick+(net_setup.game_reset_secs*1000));
}

/* =======================================================

      Run Score Limit
      
======================================================= */

void score_limit_run(void)
{
	char			err_str[256];

	gl_frame_clear(FALSE);
	gl_shader_frame_start();
	
		// draw
		
	view_primitive_2D_color_quad(&iface.color.background,1.0f,0,iface.scale_x,0,iface.scale_y);
	network_score_draw();
	gl_frame_swap();
	
		// hosts can cancel at anytime
		// or after timeout
	
	if (net_setup.mode==net_mode_host) {
		if ((game_time_get_raw()>score_limit_get_resume_tick()) || (input_action_get_state_single(nc_menu))) {

			server.next_state=gs_running;

			if (!game_host_reset(err_str)) {
				error_setup(err_str,"Hosting Game Canceled");
				server.next_state=gs_error;
				return;
			}
		}
		return;
	}
	
		// clients either wait for host
		// message or time out and fail
		
	if (game_time_get_raw()>(score_limit_start_tick+((net_setup.game_reset_secs+SCORE_LIMIT_EXTRA_SECOND_FAIL)*1000))) {
		error_setup("Host failed to restart game","Join Canceled");
		server.next_state=gs_error;
	}
	
}

