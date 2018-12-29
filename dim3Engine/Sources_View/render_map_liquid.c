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

/* =======================================================

      Check if Liquid is Transparent
      
======================================================= */

bool liquid_is_transparent(map_liquid_type *liq)
{
	texture_type				*texture;

	texture=&map.textures[liq->txt_idx];
	return(!texture->frames[texture->animate.current_frame&max_texture_frame_mask].bitmap.opaque);
}

/* =======================================================

      Setup Liquid Vertexes
      
======================================================= */

bool liquid_render_liquid_create_vertex(map_liquid_type *liq,float uv_shift,bool is_overlay)
{
	int				k,div,div_count,lft,rgt,top,bot,
					top_add,lft_add;
	float			fy,wave_y[4],f_tick,f_stamp_size,
					gx,gy,gx2,gy2,gx_add,gy_add,
					lmap_gx,lmap_gy,lmap_gx2,lmap_gy2,lmap_gx_add,lmap_gy_add;
	float			*cf;
	unsigned char	*vertex_ptr,*vp;
	d3vct			normal;
	
		// setup vbo
		
	view_bind_mesh_liquid_vertex_object(&liq->vbo);

	vertex_ptr=view_map_mesh_liquid_vertex_object(&liq->vbo);
	if (vertex_ptr==NULL) {
		view_unbind_mesh_liquid_vertex_object();
		return(FALSE);
	}

		// uv shift tick
		
	f_tick=((float)game_time_get())*0.001f;

		// liquid texture uvs
		
	if (is_overlay) {
		gx_add=f_tick*liq->overlay.shift.x;
		k=(int)gx_add;
		gx_add=gx_add-(float)k;
		
		gy_add=f_tick*liq->overlay.shift.y;
		k=(int)gy_add;
		gy_add=gy_add-(float)k;

		f_stamp_size=1.0f/((float)liq->overlay.stamp_size);
		
		gx=((float)liq->lft)*f_stamp_size;
		gx2=((float)liq->rgt)*f_stamp_size;
		
		k=(int)gx;
		gx-=((float)k);
		gx2-=((float)k);
		
		gy=((float)liq->top)*f_stamp_size;
		gy2=((float)liq->bot)*f_stamp_size;
		
		k=(int)gx;
		gy-=((float)k);
		gy2-=((float)k);

		gx+=gx_add;
		gy+=gy_add;
		
		gx2+=gx_add;
		gy2+=gy_add;
	}
	else {
		gx_add=f_tick*liq->shift.x;
		k=(int)gx_add;
		gx_add=gx_add-(float)k;
		
		gy_add=f_tick*liq->shift.y;
		k=(int)gy_add;
		gy_add=gy_add-(float)k;

		gx=liq->main_uv.offset.x+gx_add;
		gy=liq->main_uv.offset.y+gy_add;

		gx2=gx+liq->main_uv.size.x;
		gy2=gy+liq->main_uv.size.y;
	}
	
		// uv factors

	gx+=uv_shift;
	gy+=uv_shift;
	gx2-=uv_shift;
	gy2-=uv_shift;

		// build the tide and wave Y

	liquid_wave_get_high(liq,wave_y);

		// get div count and setup
		// the division calculations

	lft=liq->lft;
	rgt=liq->rgt;
	top=liq->top;
	bot=liq->bot;

	lmap_gx=liq->lmap_uv.offset.x;
	lmap_gx2=liq->lmap_uv.offset.x+liq->lmap_uv.size.x;

	lmap_gy=liq->lmap_uv.offset.y;
	lmap_gy2=liq->lmap_uv.offset.y+liq->lmap_uv.size.y;

	div_count=liquid_wave_get_divisions(liq);
	
	lft_add=rgt-lft;
	gx_add=gx2-gx;
	lmap_gx_add=lmap_gx2-lmap_gx;

	top_add=bot-top;
	gy_add=gy2-gy;
	lmap_gy_add=lmap_gy2-lmap_gy;

	if (liq->wave.on) {
		if (liq->wave.dir_north_south) {
			top_add=liq->wave.length;
			gy_add=(gy2-gy)/((float)div_count);
			lmap_gy_add=(lmap_gy2-lmap_gy)/((float)div_count);
		}
		else {
			lft_add=liq->wave.length;
			gx_add=(gx2-gx)/((float)div_count);
			lmap_gx_add=(lmap_gx2-lmap_gx)/((float)div_count);
		}
	}
	
		// draw the divisions

	vp=vertex_ptr;

	for (div=0;div<=div_count;div++) {

		fy=wave_y[div&0x3];

			// north-south (z) waves
		
		if (liq->wave.dir_north_south) {

				// normal

			liquid_wave_get_normal_z(liq,div,wave_y,top_add,&normal);
			
				// left-top
			
			cf=(float*)vp;

			*cf++=(float)lft;
			*cf++=fy;
			*cf++=(float)top;

			*cf++=gx;
			*cf++=gy;

			*cf++=lmap_gx;
			*cf++=lmap_gy;
			
			*cf++=1.0f;
			*cf++=0.0f;
			*cf++=0.0f;
		
			*cf++=normal.x;
			*cf++=normal.y;
			*cf++=normal.z;
			
			vp+=liq->vbo.vertex_stride;

				// right-top

			cf=(float*)vp;

			*cf++=(float)rgt;
			*cf++=fy;
			*cf++=(float)top;

			*cf++=gx2;
			*cf++=gy;

			*cf++=lmap_gx2;
			*cf++=lmap_gy;
				
			*cf++=1.0f;
			*cf++=0.0f;
			*cf++=0.0f;
		
			*cf++=normal.x;
			*cf++=normal.y;
			*cf++=normal.z;
			
			vp+=liq->vbo.vertex_stride;

				// div change

			top+=top_add;
			if (top>liq->bot) top=liq->bot;

			gy+=gy_add;
			if (gy>gy2) gy=gy2;

			lmap_gy+=lmap_gy_add;
			if (lmap_gy>lmap_gy2) lmap_gy=lmap_gy2;
		}

			// east-west (x) waves

		else {

				// normal

			liquid_wave_get_normal_x(liq,div,wave_y,lft_add,&normal);
			
				// left-top

			cf=(float*)vp;

			*cf++=(float)lft;
			*cf++=fy;
			*cf++=(float)top;

			*cf++=gx;
			*cf++=gy;

			*cf++=lmap_gx;
			*cf++=lmap_gy;
			
			*cf++=1.0f;
			*cf++=0.0f;
			*cf++=0.0f;
		
			*cf++=normal.x;
			*cf++=normal.y;
			*cf++=normal.z;
			
			vp+=liq->vbo.vertex_stride;

				// left-bottom

			cf=(float*)vp;

			*cf++=(float)lft;
			*cf++=fy;
			*cf++=(float)bot;

			*cf++=gx;
			*cf++=gy2;

			*cf++=lmap_gx;
			*cf++=lmap_gy2;
			
			*cf++=1.0f;
			*cf++=0.0f;
			*cf++=0.0f;
		
			*cf++=normal.x;
			*cf++=normal.y;
			*cf++=normal.z;
		
			vp+=liq->vbo.vertex_stride;

				// div change

			lft+=lft_add;
			if (lft>liq->rgt) lft=liq->rgt;

			gx+=gx_add;
			if (gx>gx2) gx=gx2;

			lmap_gx+=lmap_gx_add;
			if (lmap_gx>lmap_gx2) lmap_gx=lmap_gx2;
		}
	}

	view_unmap_mesh_liquid_vertex_object();
	view_unbind_mesh_liquid_vertex_object();

		// indexes are pre-created on startup

	return(TRUE);
}

/* =======================================================

      Liquid Rendering
      
======================================================= */

void liquid_render_liquid_layer(map_liquid_type *liq,int txt_idx,int lmap_txt_idx,bool back_rendering)
{
	int							frame;
	float						alpha;
	GLuint						gl_id;
	texture_type				*texture;
	view_glsl_light_list_type	light_list;

		// setup texture

	texture=&map.textures[txt_idx];
	frame=texture->animate.current_frame&max_texture_frame_mask;

	if (texture->additive) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
	
		// the liquid VBO
	
	view_bind_mesh_liquid_vertex_object(&liq->vbo);
	view_bind_mesh_liquid_index_object(&liq->vbo);

	gl_shader_draw_execute_reset_cached_offsets();

		// shader lights and tangents

	gl_lights_build_liquid_glsl_light_list(liq,&light_list);

	gl_shader_draw_execute_liquid_start(texture,txt_idx,frame,lmap_txt_idx,1.0f,0,(3*sizeof(float)),(5*sizeof(float)),(7*sizeof(float)),(10*sizeof(float)),liq->vbo.vertex_stride,&light_list);
			
		// fix texture if any back rendering
		
	gl_id=texture->frames[frame].bitmap.gl_id;
	alpha=1.0f;

	if (back_rendering) {
		if (gl_back_render_get_texture(liq->camera,&gl_id,&alpha)) {
			gl_shader_texture_override(gl_id,alpha);
		}
	}
	
		// draw liquid

	glDrawElements(GL_TRIANGLE_STRIP,liq->vbo.index_count,GL_UNSIGNED_SHORT,(GLvoid*)0);
	
	gl_shader_draw_execute_liquid_end(texture,&light_list);

	view_unbind_mesh_liquid_vertex_object();
	view_unbind_mesh_liquid_index_object();
}

void liquid_render_liquid(map_liquid_type *liq)
{
	int					lmap_txt_idx;
	float				uv_shift;

		// uv factor

	uv_shift=liquid_tide_get_uv_factor(liq);

		// get light map

	if (!setup.debug_on) {
		lmap_txt_idx=liq->lmap_txt_idx;
	}
	else {
		lmap_txt_idx=lmap_white_bitmap.gl_id;
	}

		// draw the reflection liquid
		// or just the regular texture

	if (!liquid_render_liquid_create_vertex(liq,uv_shift,FALSE)) return;

	liquid_render_liquid_layer(liq,liq->txt_idx,lmap_txt_idx,TRUE);
	
		// count the liquid polys
		
	view.count.liquid_poly+=((liq->vbo.vertex_count+2)>>2);

		// draw the overlay?

	if ((!liq->overlay.on) || (liq->overlay.txt_idx==-1)) return;

		// draw the overlay

	view.count.liquid_poly+=((liq->vbo.vertex_count+2)>>2);

	if (!liquid_render_liquid_create_vertex(liq,(uv_shift*0.5f),TRUE)) return;

	liquid_render_liquid_layer(liq,liq->overlay.txt_idx,lmap_txt_idx,FALSE);
}

/* =======================================================

      Liquid Rendering
      
======================================================= */

void render_map_liquid_opaque(void)
{
	int					n;
	map_liquid_type		*liq;
	
		// common setup

	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);

		// draw opaque liquids
		
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type==view_render_type_liquid) {
			liq=&map.liquid.liquids[view.render->draw_list.items[n].idx];
			if (!liquid_is_transparent(liq)) {
				liquid_render_liquid(liq);
			}
		}
	}
}

void render_map_liquid_transparent(void)
{
	int					n;
	map_liquid_type		*liq;
	
		// common setup

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
						
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

		// draw transparent liquids
		
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type==view_render_type_liquid) {
			liq=&map.liquid.liquids[view.render->draw_list.items[n].idx];
			if (liquid_is_transparent(liq)) {
				liquid_render_liquid(liq);
			}
		}
	}
	
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

