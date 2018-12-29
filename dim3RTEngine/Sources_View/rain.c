/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Rain

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

#include "interface.h"

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

int							rain_last_tick,rain_slant_add,rain_slant_next_add,
							rain_slant_next_start_tick,rain_slant_next_end_tick,
							rain_last_camera_y;
float						rain_slant_ang_y,rain_slant_next_ang_y;
bool						rain_vbo_created;

/* =======================================================

      Rain Initialize and Release
      
======================================================= */

void rain_draw_init(void)
{
	rain_vbo_created=FALSE;
}

void rain_draw_release(void)
{
//	if (rain_vbo_created) view_dispose_rain_vertex_object();
}

/* =======================================================

      Setup Rain
      
======================================================= */

void rain_setup_single(rain_draw_type *rain_draw,d3pnt *pnt)
{
	rain_draw->x_off=random_int_spread(map.rain.radius);
	rain_draw->z_off=random_int_spread(map.rain.radius);

	rain_draw->x=pnt->x+rain_draw->x_off;
	rain_draw->z=pnt->z+rain_draw->z_off;

	rain_draw->y=pnt->y+random_int_spread(map.rain.height);
	rain_draw->by=pnt->y+map.rain.height;
}

void rain_setup_single_reset(rain_draw_type *rain_draw,d3pnt *pnt)
{
	rain_draw->x=pnt->x+rain_draw->x_off;
	rain_draw->z=pnt->z+rain_draw->z_off;
	rain_draw->y=(pnt->y-map.rain.height)+((rain_draw->y-rain_draw->by)%(map.rain.height<<1));

	rain_draw->by=pnt->y+map.rain.height;
}

void rain_setup_next_slant(void)
{
	if (map.rain.slant_time_msec==0) return;
	
	rain_slant_next_add=random_int(map.rain.slant_add);
	rain_slant_next_ang_y=random_float(360);

	rain_slant_next_start_tick=game_time_get()+random_int(map.rain.slant_time_msec);
	rain_slant_next_end_tick=rain_slant_next_start_tick+map.rain.slant_change_msec;
}

void rain_setup(d3pnt *pnt)
{
	int				n;
	rain_draw_type	*rain_draw;

	rain_draw=view.rain_draws;

	for (n=0;n!=max_rain_density;n++) {
		rain_setup_single(rain_draw,pnt);
		rain_draw++;
	}
	
	rain_last_tick=game_time_get();
	
	rain_slant_add=random_int(map.rain.slant_add);
	rain_slant_ang_y=random_float(360);
	
	rain_setup_next_slant();

	rain_last_camera_y=view.render->camera.pnt.y;
}

void rain_reset(void)
{
	if (map.rain.on) map.rain.reset=TRUE;
}

/* =======================================================

      Draw Rain
      
======================================================= */

void rain_draw(void)
{
	/* supergumba -- will need to be redone

	int				n,tick,xadd,yadd,zadd,ypush,density,mem_sz,
					slant_add,slant_mult,slant_div;
	float			slant_ang_y;
	float			*vp;
	unsigned char	*vertex_ptr,*cp;
	unsigned char	start_r,start_g,start_b,end_r,end_g,end_b,uc_alpha;
	rain_draw_type	*rain_draw;

		// is rain on and not under liquid?

	if (!map.rain.on) return;
	if (view.render->camera.under_liquid_idx!=-1) return;
	
		// if this is first time, setup VBO
		
	if (!rain_vbo_created) {
		rain_vbo_created=TRUE;
		
		mem_sz=((max_rain_density*2)*3)*sizeof(float);
		mem_sz+=(((max_rain_density*2)*4)*sizeof(unsigned char));
		view_create_rain_vertex_object(mem_sz);
	}
	
		// reset on?
		
	if (map.rain.reset) {
		map.rain.reset=FALSE;
		rain_setup(&view.render->camera.pnt);
	}

		// rain y
		// rain will be pushed by the Y because it looks
		// wrong if you are falling too fast

	ypush=view.render->camera.pnt.y-rain_last_camera_y;
	rain_last_camera_y=view.render->camera.pnt.y;
	
		// rain slant

	tick=game_time_get();
		
	slant_add=rain_slant_add;
	slant_ang_y=rain_slant_ang_y;
		
	if (map.rain.slant_time_msec!=0) {
		
			// time to change slant?
			
		if (tick>rain_slant_next_end_tick) {
			rain_slant_add=slant_add=rain_slant_next_add;
			rain_slant_ang_y=slant_ang_y=rain_slant_next_ang_y;
			rain_setup_next_slant();
		}
		else {
		
				// slant in the middle of changing
				
			if (tick>rain_slant_next_start_tick) {
				slant_mult=tick-rain_slant_next_start_tick;
				slant_div=(rain_slant_next_end_tick-rain_slant_next_start_tick);
				
				slant_add=rain_slant_add+(((rain_slant_next_add-rain_slant_add)*slant_mult)/slant_div);
				slant_ang_y=rain_slant_ang_y+((rain_slant_next_ang_y-rain_slant_ang_y)*((float)slant_mult/(float)slant_div));
			}
			
		}
	}

	angle_get_movement(slant_ang_y,slant_add,&xadd,&zadd);

		// rain change

	xadd=(tick-rain_last_tick)*xadd;
	yadd=((tick-rain_last_tick)*map.rain.speed)+ypush;
	zadd=(tick-rain_last_tick)*zadd;
	
	rain_last_tick=tick;

		// rain density

	density=map.rain.density;
	if (density>max_rain_density) density=max_rain_density;

		// construct VBO
		
	view_bind_rain_vertex_object();
	vertex_ptr=view_map_rain_vertex_object();
	if (vertex_ptr==NULL) {
		view_unbind_rain_vertex_object();
		return;
	}

	vp=(float*)vertex_ptr;
	cp=vertex_ptr+(((density*2)*3)*sizeof(float));
	
		// uc rain colors
		
	start_r=(unsigned char)(map.rain.start_color.r*255.0f);
	start_g=(unsigned char)(map.rain.start_color.g*255.0f);
	start_b=(unsigned char)(map.rain.start_color.b*255.0f);
	
	end_r=(unsigned char)(map.rain.end_color.r*255.0f);
	end_g=(unsigned char)(map.rain.end_color.g*255.0f);
	end_b=(unsigned char)(map.rain.end_color.b*255.0f);
	
	uc_alpha=(unsigned char)(map.rain.alpha*255.0f);

		// create vertexes

	rain_draw=view.rain_draws;

	for (n=0;n!=density;n++) {

			// move rain

		rain_draw->x+=xadd;
		rain_draw->y+=yadd;
		rain_draw->z+=zadd;
		
		if (rain_draw->y>rain_draw->by) rain_setup_single_reset(rain_draw,&view.render->camera.pnt);

			// draw rain

		*vp++=(float)rain_draw->x;
		*vp++=(float)rain_draw->y;
		*vp++=(float)rain_draw->z;

		*cp++=start_r;
		*cp++=start_g;
		*cp++=start_b;
		*cp++=uc_alpha;

		*vp++=(float)(rain_draw->x+xadd);
		*vp++=(float)(rain_draw->y+map.rain.line_length);
		*vp++=(float)(rain_draw->z+zadd);

		*cp++=end_r;
		*cp++=end_g;
		*cp++=end_b;
		*cp++=uc_alpha;

		rain_draw++;
	}

  	view_unmap_rain_vertex_object();

		// setup view

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

		// draw the rain

	glLineWidth((float)map.rain.line_width);
	
	gl_shader_draw_execute_simple_gradient_start(3,0,(((density*2)*3)*sizeof(float)));
	glDrawArrays(GL_LINES,0,(density*2));
	gl_shader_draw_execute_simple_gradient_end();

	glLineWidth(1.0f);
	
	glDepthMask(GL_TRUE);

		// unbind the vbo

	view_unbind_rain_vertex_object();
	*/
}

