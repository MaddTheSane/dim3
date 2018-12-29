/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Effect Drawing

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

float					effect_lightning_line_zag[16]={1.0f,0.5f,0.8f,-0.2f,0.3f,-1.0f,-0.7f,0.1f,-0.4f,0.6f,-0.4f,0.5f,-0.6f,0.2f,-0.8f,-0.4f};

extern map_type			map;
extern server_type		server;
extern view_type		view;
extern iface_type		iface;

/* =======================================================

      Effect Bound Box
      
======================================================= */

void effect_draw_get_bound_box(effect_type *effect,d3pnt *min,d3pnt *max)
{
	int					count,size,gravity_y;
	d3pnt				pnt;
	iface_particle_type	*particle;

	switch (effect->effecttype) {

		case ef_particle:
			count=game_time_get()-effect->start_tick;
			particle_draw_position(effect,count,&pnt);

			size=effect->size;
			min->x=pnt.x-(size>>1);
			max->x=min->x+size;
			min->z=pnt.z-(size>>1);
			max->z=min->z+size;
			min->y=pnt.y-size;
			max->y=pnt.y;

			particle=&iface.particle_list.particles[effect->data.particle.particle_idx];
			gravity_y=(int)particle_get_gravity(particle,count);

			if (gravity_y<0) {
				min->y+=gravity_y;
			}
			else {
				max->y+=gravity_y;
			}
			
			break;

		case ef_ring:
			count=game_time_get()-effect->start_tick;
			ring_draw_position(effect,count,&pnt);

			size=effect->size;
			min->x=pnt.x-(size>>1);
			max->x=min->x+size;
			min->z=pnt.z-(size>>1);
			max->z=min->z+size;
			min->y=pnt.y-size;
			max->y=pnt.y;
			break;

		case ef_lightning:
			if (effect->data.lightning.start_pnt.x<effect->data.lightning.end_pnt.x) {
				min->x=effect->data.lightning.start_pnt.x;
				max->x=effect->data.lightning.end_pnt.x;
			}
			else {
				min->x=effect->data.lightning.end_pnt.x;
				max->x=effect->data.lightning.start_pnt.x;
			}
			if (effect->data.lightning.start_pnt.y<effect->data.lightning.end_pnt.y) {
				min->y=effect->data.lightning.start_pnt.y;
				max->y=effect->data.lightning.end_pnt.y;
			}
			else {
				min->y=effect->data.lightning.end_pnt.y;
				max->y=effect->data.lightning.start_pnt.y;
			}
			if (effect->data.lightning.start_pnt.z<effect->data.lightning.end_pnt.z) {
				min->z=effect->data.lightning.start_pnt.z;
				max->z=effect->data.lightning.end_pnt.z;
			}
			else {
				min->z=effect->data.lightning.end_pnt.z;
				max->z=effect->data.lightning.start_pnt.z;
			}
			break;

		case ef_ray:
			if (effect->data.ray.start_pnt.x<effect->data.ray.end_pnt.x) {
				min->x=effect->data.ray.start_pnt.x;
				max->x=effect->data.ray.end_pnt.x;
			}
			else {
				min->x=effect->data.ray.end_pnt.x;
				max->x=effect->data.ray.start_pnt.x;
			}
			if (effect->data.ray.start_pnt.y<effect->data.ray.end_pnt.y) {
				min->y=effect->data.ray.start_pnt.y;
				max->y=effect->data.ray.end_pnt.y;
			}
			else {
				min->y=effect->data.ray.end_pnt.y;
				max->y=effect->data.ray.start_pnt.y;
			}
			if (effect->data.ray.start_pnt.z<effect->data.ray.end_pnt.z) {
				min->z=effect->data.ray.start_pnt.z;
				max->z=effect->data.ray.end_pnt.z;
			}
			else {
				min->z=effect->data.ray.end_pnt.z;
				max->z=effect->data.ray.start_pnt.z;
			}
			break;

		default:
			size=effect->size;
			min->x=effect->pnt.x-(size>>1);
			max->x=min->x+size;
			min->z=effect->pnt.z-(size>>1);
			max->z=min->z+size;
			min->y=effect->pnt.y-size;
			max->y=effect->pnt.y;
			break;
	}
}

/* =======================================================

      Draw Lightning
      
======================================================= */

void effect_draw_lightning(effect_type *effect)
{
	int						n,k,nline,wid,x,z,y,
                            sx,sz,sy,ex,ez,ey,xadd,zadd,yadd;
	float					varient,fx,fy,fz,f_xadd,f_yadd,f_zadd;
	float					*vertex_ptr;
	lightning_effect_data	*lightning;
	
	lightning=&effect->data.lightning;
	
        // get line start and end
        
	sx=lightning->start_pnt.x;
	sy=lightning->start_pnt.y;
	sz=lightning->start_pnt.z;

	ex=lightning->end_pnt.x;
	ey=lightning->end_pnt.y;
	ez=lightning->end_pnt.z;
	
        // find number of lines

    x=abs(ex-sx);
    z=abs(ez-sz);
    y=abs(ey-sy);
	nline=distance_get(x,z,y,0,0,0)/200;
    
    if (nline<effect_lightning_min_lines) nline=effect_lightning_min_lines;
	if (nline>=effect_lightning_max_lines) nline=(effect_lightning_max_lines-1);
	
        // get line steps
        
	xadd=(ex-sx)/nline;
	zadd=(ez-sz)/nline;
	yadd=(ey-sy)/nline;
	
        // line varients
        
	k=((game_time_get()>>5)+lightning->line_offset)&0xF;
	varient=lightning->varient;

		// construct VBO
		// effect vbos are dynamic, so it'll auto construct
		// the first time called

	view_create_effect_vertex_object(effect,(((nline*3)*2)*sizeof(float)),-1);

	view_bind_effect_vertex_object(effect);
	vertex_ptr=(float*)view_map_effect_vertex_object();
	if (vertex_ptr==NULL) {
		view_unbind_effect_vertex_object();
		return;
	}

		// setup vertexes

	fx=(float)sx;
	fy=(float)sy;
	fz=(float)sz;

	f_xadd=(float)xadd;
	f_yadd=(float)yadd;
	f_zadd=(float)zadd;

	for (n=0;n!=nline;n++) {
		
		*vertex_ptr++=fx;
		*vertex_ptr++=fy;
		*vertex_ptr++=fz;
	
		if (n==(nline-1)) {
			*vertex_ptr++=(float)ex;
			*vertex_ptr++=(float)ey;
			*vertex_ptr++=(float)ez;
		}
		else {
			fx=(fx+f_xadd)+(int)(effect_lightning_line_zag[k]*varient);
			fy=(fy+f_yadd)+(int)(effect_lightning_line_zag[(k+4)&0xF]*varient);
			fz=(fz+f_zadd)+(int)(effect_lightning_line_zag[(k+8)&0xF]*varient);

			*vertex_ptr++=fx;
			*vertex_ptr++=fy;
			*vertex_ptr++=fz;
		}

		k=(k+1)&0xF;
	}

		// unmap vertex object

	view_unmap_effect_vertex_object();
	    
        // draw lines
		
	wid=lightning->wid;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	gl_shader_draw_execute_simple_color_start(3,0,&lightning->col,0.1f);
	
	glLineWidth((float)(wid*5));
	glDrawArrays(GL_LINES,0,(nline*2));

	gl_shader_draw_execute_simple_color_set_color(&lightning->col,0.05f);
	glLineWidth((float)(wid*3));
	glDrawArrays(GL_LINES,0,(nline*2));

	gl_shader_draw_execute_simple_color_set_color(&lightning->col,0.6f);
	glLineWidth((float)wid);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_LINES,0,(nline*2));

	gl_shader_draw_execute_simple_color_end();

	glLineWidth(1.0f);
	
	glDepthMask(GL_TRUE);

		// unbind vertex object
		
	view_unbind_effect_vertex_object();
}

/* =======================================================

      Draw Ray
      
======================================================= */

void effect_draw_ray(effect_type *effect,int count)
{
	int						wid,sx,sz,sy,ex,ez,ey,life_tick;
	float					*vp;
	ray_effect_data			*ray;
	
	ray=&effect->data.ray;
	
        // find line
        
	sx=ray->start_pnt.x;
	sy=ray->start_pnt.y;
	sz=ray->start_pnt.z;

	ex=ray->end_pnt.x;
	ey=ray->end_pnt.y;
	ez=ray->end_pnt.z;
	
		// build line for count
	
	life_tick=effect->life_tick>>1;

	if (count<life_tick) {
		ex=sx+(((ex-sx)*count)/life_tick);
		ey=sy+(((ey-sy)*count)/life_tick);
		ez=sz+(((ez-sz)*count)/life_tick);
	}
	else {
		count=effect->life_tick-count;
		sx=ex+(((sx-ex)*count)/life_tick);
		sy=ey+(((sy-ey)*count)/life_tick);
		sz=ez+(((sz-ez)*count)/life_tick);
	}

		// setup vertexes

	view_bind_utility_vertex_object();
	vp=(float*)view_map_utility_vertex_object();

	*vp++=(float)sx;
	*vp++=(float)sy;
	*vp++=(float)sz;

	*vp++=(float)ex;
	*vp++=(float)ey;
	*vp++=(float)ez;

	view_unmap_utility_vertex_object();

        // draw lines
		
	wid=ray->wid;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	gl_shader_draw_execute_simple_color_start(3,0,&ray->col,1.0f);
	
	while (wid>0) {
	
		glLineWidth((float)wid);
		glDrawArrays(GL_LINES,0,2);
		
		wid-=2;
	}

	gl_shader_draw_execute_simple_color_end();
	
	glDepthMask(GL_TRUE);

	view_unbind_utility_vertex_object();
}

/* =======================================================

      Get UV for animated Images
      
======================================================= */

void effect_image_animate_get_uv(int tick,int image_offset,iface_image_animation_type *animate,float *gx,float *gy,float *g_size)
{
	int				k,nimage,image_per_row;
	float			gsz;
	
	nimage=animate->image_count;
	
	*gx=*gy=0.0f;
	*g_size=1.0f;
	
	if (nimage>1) {
		if (animate->msec==0) {
			k=0;
		}
		else {
			k=(tick/animate->msec)+image_offset;
		}

		if ((!animate->loop) && (k>=nimage)) {
			k=nimage-1;
		}
		else {
			if (!animate->loop_back) {
				k=k%nimage;
			}
			else {
				k=k%((nimage<<1)-2);
				if (k>=nimage) k=(nimage+2)-k;
			}
		}

		image_per_row=animate->image_per_row;
		gsz=1.0f/(float)image_per_row;
		
		*gx=gsz*(float)(k%image_per_row);
		*gy=gsz*(float)(k/image_per_row);
		
		*g_size=gsz;
	}
}

/* =======================================================

      Effects Draw Mainline
      
======================================================= */

void effect_draw(void)
{
	int				n,idx,tick,count;
	effect_type		*effect;
		
		// draw effects
		// draw backwards to sort back to front
		
	tick=game_time_get();

	for (n=(view.render->draw_list.count-1);n>=0;n--) {
		if (view.render->draw_list.items[n].type!=view_render_type_effect) continue;

		idx=view.render->draw_list.items[n].idx;
		effect=server.effect_list.effects[idx];
		
		count=tick-effect->start_tick;

		switch (effect->effecttype) {

			case ef_particle:
				particle_draw(effect,count,idx);
				break;
				
			case ef_ring:
				ring_draw(effect,count,idx);
				break;
				
			case ef_lightning:
				effect_draw_lightning(effect);
				break;
				
			case ef_ray:
				effect_draw_ray(effect,count);
				break;
				
		}
	}
}


