/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Segment Liquid Tide and Breakup Routines

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

#include "interface.h"

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern setup_type			setup;

extern bitmap_type			lmap_white_bitmap;

float						liquid_normal_cycle_xz[4]={0.0f,0.5f,0.0f,-0.5f},
							liquid_normal_cycle_y[4]={-1.0f,-0.5f,-1.0f,-0.5f};
							
/* =======================================================

      Liquid Tides
      
======================================================= */

float liquid_tide_get_high(map_liquid_type *liq)
{
	float			f_time,sn;

		// get rate between 0..1

	f_time=(float)(game_time_get()%liq->tide.rate);
	f_time=f_time/((float)liq->tide.rate);
	
		// waves are sin waves

	sn=sinf((TRIG_PI*2.0f)*f_time);
	return(((float)liq->tide.high)*sn);
}

float liquid_tide_get_uv_factor(map_liquid_type *liq)
{
	float			f_time,sn,shift;

		// get rate between 0..1

	f_time=(float)(game_time_get()%liq->tide.rate);
	f_time=f_time/((float)liq->tide.rate);
	
		// waves are sin waves

	sn=sinf((TRIG_PI*2.0f)*f_time);
	
	shift=liq->tide.uv_shift*0.5f;
	return(shift+(shift*sn));
}

/* =======================================================

      Liquid Waves
      
======================================================= */

int liquid_wave_get_divisions(map_liquid_type *liq)
{
	int			len,count;

	if (!liq->wave.on) return(1);

	if (liq->wave.dir_north_south) {
		len=liq->bot-liq->top;
	}
	else {
		len=liq->rgt-liq->lft;
	}

	count=len/liq->wave.length;
	if ((len%liq->wave.length)!=0) count++;

	return(count);
}

void liquid_wave_get_high(map_liquid_type *liq,float *wave_y)
{
	float			f,fy,f_time,cs;
	
		// start with the tide height

	fy=((float)liq->y)-liquid_tide_get_high(liq);

		// if no waves, then flat Ys

	if (!liq->wave.on) {
		wave_y[0]=wave_y[1]=wave_y[2]=wave_y[3]=fy;
		return;
	}

		// get rate between 0..1

	f_time=(float)(game_time_get()%liq->wave.period_msec);
	f_time=f_time/((float)liq->wave.period_msec);
	
		// waves are sin waves

	cs=cosf((TRIG_PI*2.0f)*f_time);
	f=((float)liq->wave.high)*cs;
	
	wave_y[0]=fy-f;
	wave_y[1]=wave_y[3]=fy-(f*0.5f);
	wave_y[2]=fy-(f*0.25f);
}

void liquid_wave_get_normal_x(map_liquid_type *liq,int div,float *wave_y,int lft_add,d3vct *normal)
{
	int				lft,lft_prev,lft_next,
					y,y_prev,y_next;
	d3vct			p10,p20,n1,n2;

	lft=liq->lft+(lft_add*div);
	y=(int)wave_y[div&0x3];

	lft_prev=lft-lft_add;
	y_prev=(int)wave_y[(div-1)&0x3];

	lft_next=lft+lft_add;
	y_next=(int)wave_y[(div+1)&0x3];

		// get previous and next
		// polygon normals

	vector_create(&p10,lft_prev,y_prev,liq->top,lft_prev,y_prev,liq->bot);
	vector_create(&p20,lft,y,liq->bot,lft_prev,y_prev,liq->bot);
	vector_cross_product(&n1,&p10,&p20);
	vector_normalize(&n1);

	vector_create(&p10,lft,y,liq->top,lft,y,liq->bot);
	vector_create(&p20,lft_next,y_next,liq->bot,lft,y,liq->bot);
	vector_cross_product(&n2,&p10,&p20);
	vector_normalize(&n2);

		// average for normal

	normal->x=(n1.x+n2.x)*0.5f;
	normal->y=(n1.y+n2.y)*0.5f;
	normal->z=(n1.z+n2.z)*0.5f;
	vector_normalize(normal);
}

void liquid_wave_get_normal_z(map_liquid_type *liq,int div,float *wave_y,int top_add,d3vct *normal)
{
	int				top,top_prev,top_next,
					y,y_prev,y_next;
	d3vct			p10,p20,n1,n2;

	top=liq->top+(top_add*div);
	y=(int)wave_y[div&0x3];

	top_prev=top-top_add;
	y_prev=(int)wave_y[(div-1)&0x3];

	top_next=top+top_add;
	y_next=(int)wave_y[(div+1)&0x3];

		// get previous and next
		// polygon normals

	vector_create(&p10,liq->rgt,y_prev,top_prev,liq->lft,y_prev,top_prev);
	vector_create(&p20,liq->lft,y,top,liq->lft,y_prev,top_prev);
	vector_cross_product(&n1,&p10,&p20);
	vector_normalize(&n1);

	vector_create(&p10,liq->rgt,y,top,liq->lft,y,top);
	vector_create(&p20,liq->lft,y_next,top_next,liq->lft,y,top);
	vector_cross_product(&n2,&p10,&p20);
	vector_normalize(&n2);

		// average for normal

	normal->x=(n1.x+n2.x)*0.5f;
	normal->y=(n1.y+n2.y)*0.5f;
	normal->z=(n1.z+n2.z)*0.5f;
	vector_normalize(normal);
}

