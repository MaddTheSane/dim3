/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Sky Boxes

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

extern map_type			map;
extern setup_type		setup;
extern view_type		view;

/* =======================================================

      Draw Background
      
======================================================= */

void sky_draw_background_single(map_background_layer_type *layer)
{
	float				f_screen_x_sz,f_screen_y_sz,ratio,
						f_ty,f_by,gx_lft,gx_rgt,gy_top,gy_bot;
	float				*vp;
	d3col				col;
	texture_type		*texture;
	bitmap_type			*bitmap;

	if ((layer->fill<0) || (layer->fill>=max_map_texture)) return;
	
	f_screen_x_sz=(float)view.screen.x_sz;
	f_screen_y_sz=(float)view.screen.y_sz;
	
		// The Y actually uses the
		// X screen size to keep the backgrounds
		// square, we fix later
		
	f_ty=f_screen_x_sz*layer->clip.x;
	f_by=f_screen_x_sz*layer->clip.y;
	
		// get UV
		
	gx_lft=((float)(view.render->camera.pnt.x+view.render->camera.pnt.z))*(layer->scroll_factor.x*0.001f);
	gx_rgt=gx_lft+layer->size.x;
	
	gy_top=(((float)view.render->camera.pnt.y)*(layer->scroll_factor.y*0.001f))+layer->clip.x;
	gy_bot=gy_top+(layer->size.y*(layer->clip.y-layer->clip.x));
	
		// alter the Ys so we aren't drawing
		// below the screen
		
	if (f_by>f_screen_y_sz) {
		ratio=f_screen_y_sz/f_screen_x_sz;
		f_by*=ratio;
		gy_bot*=ratio;
	}
	
		// setup background
		
	texture=&map.textures[layer->fill];
	bitmap=&texture->frames[texture->animate.current_frame].bitmap;
	
	view_bind_utility_vertex_object();
	vp=(float*)view_map_utility_vertex_object();

	*vp++=0.0f;
	*vp++=f_ty;
	*vp++=gx_lft;
	*vp++=gy_top;

	*vp++=0.0f;
	*vp++=f_by;
	*vp++=gx_lft;
	*vp++=gy_bot;

	*vp++=(float)view.screen.x_sz;
	*vp++=f_ty;
	*vp++=gx_rgt;
	*vp++=gy_top;

	*vp++=(float)view.screen.x_sz;
	*vp++=f_by;
	*vp++=gx_rgt;
	*vp++=gy_bot;

	view_unmap_utility_vertex_object();

	col.r=col.g=col.b=1.0f;

		// draw the quad

	gl_shader_draw_execute_simple_bitmap_set_texture(bitmap->gl_id);
	gl_shader_draw_execute_simple_bitmap_start(2,0,(2*sizeof(float)),((2+2)*sizeof(float)),&col,1.0f);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	gl_shader_draw_execute_simple_bitmap_end();

	view_unbind_utility_vertex_object();
}

void sky_draw_background(void)
{
		// draw the layers
		
	glDisable(GL_DEPTH_TEST);
	
		// first background draws with
		// no alpha

	sky_draw_background_single(&map.background.back);
	
		// other background draw with alpha
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	sky_draw_background_single(&map.background.middle);
	sky_draw_background_single(&map.background.front);

	glDisable(GL_BLEND);
}

/* =======================================================

      Draw Dome Panoramic Sky
      
======================================================= */

void sky_draw_dome_panoramic_setup(void)
{
    int					i,n,sz,radius;
	float				gx1,gx2,tgy,bgy,f_ty,f_by,
						top_reduce,bot_reduce,f_radius,
						rxz,rxz2,r_add;
	float				*vertex_ptr,*uv_ptr;

		// dome setup

	int					y_fct[5]={2,3,5,6,9};
	float				gy_fct[6]={1.00f,0.80f,0.60f,0.40f,0.20f,0.01f},
						reduce_fct[5]={6.8f,6.4f,6.0f,5.5f,5.0f};
	
		// dome setup
	
	radius=map.sky.radius;
	f_radius=(float)radius;

	f_ty=(float)map.sky.dome_y;
	top_reduce=f_radius;

	r_add=ANG_to_RAD*(360.0f/20.0f);
	
		// construct VBO

	sz=((5*20)*6)+(20*3);
	view_create_sky_vertex_object(((sz*(3+2))*sizeof(float)));

	view_bind_sky_vertex_object();

	vertex_ptr=(float*)view_map_sky_vertex_object();
	if (vertex_ptr==NULL) {
		view_unbind_sky_vertex_object();
		return;
	}

	uv_ptr=vertex_ptr+(sz*3);

		// create the dome vertexes

	for (i=0;i!=5;i++) {				// the y
		
		f_by=f_ty;
		f_ty-=(float)(radius/y_fct[i]);

		rxz=0.0f;

		bot_reduce=top_reduce;
		top_reduce-=(f_radius/reduce_fct[i]);
			
		bgy=gy_fct[i];
		tgy=gy_fct[i+1];

		for (n=0;n!=20;n++) {			// the x and z
		
			if (n!=19) {
				rxz2=rxz+r_add;
			}
			else {
				rxz2=0.0f;
			}
			
			gx1=((float)n)/20.0f;
			gx2=((float)(n+1))/20.0f;
		
				// trig 1

			*vertex_ptr++=-(sinf(rxz)*bot_reduce);
			*vertex_ptr++=f_by;
			*vertex_ptr++=cosf(rxz)*bot_reduce;

			*uv_ptr++=gx1;
			*uv_ptr++=bgy;

			*vertex_ptr++=-(sinf(rxz2)*bot_reduce);
			*vertex_ptr++=f_by;
			*vertex_ptr++=cosf(rxz2)*bot_reduce;

			*uv_ptr++=gx2;
			*uv_ptr++=bgy;

			*vertex_ptr++=-(sinf(rxz)*top_reduce);
			*vertex_ptr++=f_ty;
			*vertex_ptr++=cosf(rxz)*top_reduce;

			*uv_ptr++=gx1;
			*uv_ptr++=tgy;

				// trig 2

			*vertex_ptr++=-(sinf(rxz2)*bot_reduce);
			*vertex_ptr++=f_by;
			*vertex_ptr++=cosf(rxz2)*bot_reduce;

			*uv_ptr++=gx2;
			*uv_ptr++=bgy;
			
			*vertex_ptr++=-(sinf(rxz2)*top_reduce);
			*vertex_ptr++=f_ty;
			*vertex_ptr++=cosf(rxz2)*top_reduce;

			*uv_ptr++=gx2;
			*uv_ptr++=tgy;

			*vertex_ptr++=-(sinf(rxz)*top_reduce);
			*vertex_ptr++=f_ty;
			*vertex_ptr++=cosf(rxz)*top_reduce;

			*uv_ptr++=gx1;
			*uv_ptr++=tgy;

			rxz+=r_add;
		}
	}

		// create the cap vertexes
		// cap gets color from pixel at 0,0

	rxz=0.0f;

	for (n=0;n!=20;n++) {			// the x and z
	
		if (n!=19) {
			rxz2=rxz+r_add;
		}
		else {
			rxz2=0.0f;
		}

		*vertex_ptr++=-(sinf(rxz)*bot_reduce);
		*vertex_ptr++=f_by;
		*vertex_ptr++=cosf(rxz)*bot_reduce;

		*uv_ptr++=gx1;
		*uv_ptr++=bgy;

		*vertex_ptr++=-(sinf(rxz2)*bot_reduce);
		*vertex_ptr++=f_by;
		*vertex_ptr++=cosf(rxz2)*bot_reduce;

		*uv_ptr++=gx2;
		*uv_ptr++=bgy;

		*vertex_ptr++=0.0f;
		*vertex_ptr++=f_ty;
		*vertex_ptr++=0.0f;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		rxz+=r_add;
	}

		// unmap and unbind vbo

  	view_unmap_sky_vertex_object();
	view_unbind_sky_vertex_object();
}

void sky_draw_dome_panoramic(void)
{
    int					pan_count;
	d3col				col;
	texture_type		*texture;
	
		// construct vbo

	pan_count=((5*20)*6)+(20*3);

	view_bind_sky_vertex_object();

		// draw textured dome
		
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	texture=&map.textures[map.sky.fill];
	col.r=col.g=col.b=1.0f;

	gl_shader_draw_execute_simple_bitmap_set_texture(texture->frames[texture->animate.current_frame].bitmap.gl_id);
	gl_shader_draw_execute_simple_bitmap_start(3,0,((pan_count*3)*sizeof(float)),0,&col,1.0f);
	glDrawArrays(GL_TRIANGLES,0,pan_count);
	gl_shader_draw_execute_simple_bitmap_end();

		// unbind the vbo

	view_unbind_sky_vertex_object();
}

/* =======================================================

      Draw Dome Hemisphere Sky
      
======================================================= */

void sky_draw_dome_hemisphere_setup(void)
{
    int					i,n,radius,sz;
	float				f_ty,f_by,f_ty2,f_by2,gx1,gx2,tgy,bgy,
						top_reduce,bot_reduce,f_radius,
						rxz,rxz2,r_add;
	float				*vertex_ptr,*uv_ptr;

		// dome setup

	int					y_fct[6]={2,3,4,6,8,11};
	float				gy_fct[7]={1.00f,0.70f,0.40f,0.20f,0.10f,0.01f,0.01f},
						reduce_fct[7]={6.6f,6.4f,5.9f,5.2f,4.3f,3.2f};

		// dome setup
	
	radius=map.sky.radius;
	f_radius=(float)radius;

	f_ty=f_ty2=(float)map.sky.dome_y;
	top_reduce=f_radius;

	r_add=ANG_to_RAD*(360.0f/20.0f);

		// construct VBO
		
	sz=((4*20)*6)+(20*3);
	if (map.sky.dome_mirror) sz*=2;

	view_create_sky_vertex_object(((sz*(3+2))*sizeof(float)));

	view_bind_sky_vertex_object();

	vertex_ptr=(float*)view_map_sky_vertex_object();
	if (vertex_ptr==NULL) {
		view_unbind_sky_vertex_object();
		return;
	}

	uv_ptr=vertex_ptr+(sz*3);

		// create the dome vertexes

	for (i=0;i!=5;i++) {				// the y
		
		f_by=f_ty;
		f_ty-=(float)(radius/y_fct[i]);

		f_by2=f_ty2;
		f_ty2+=(float)(radius/y_fct[i]);

		rxz=0.0;

		bot_reduce=top_reduce;
		top_reduce-=(f_radius/reduce_fct[i]);

		bgy=gy_fct[i];
		tgy=gy_fct[i+1];
			
		for (n=0;n!=20;n++) {			// the x and z
		
			if (n!=19) {
				rxz2=rxz+r_add;
			}
			else {
				rxz2=0.0f;
			}

			gx1=((float)n)/20.0f;
			gx2=((float)(n+1))/20.0f;

				// regular dome

			if (i!=4) {

					// trig 1

				*vertex_ptr++=-(sinf(rxz)*bot_reduce);
				*vertex_ptr++=f_by;
				*vertex_ptr++=cosf(rxz)*bot_reduce;

				*uv_ptr++=gx1;
				*uv_ptr++=bgy;

				*vertex_ptr++=-(sinf(rxz2)*bot_reduce);
				*vertex_ptr++=f_by;
				*vertex_ptr++=cosf(rxz2)*bot_reduce;

				*uv_ptr++=gx2;
				*uv_ptr++=bgy;

				*vertex_ptr++=-(sinf(rxz)*top_reduce);
				*vertex_ptr++=f_ty;
				*vertex_ptr++=cosf(rxz)*top_reduce;

				*uv_ptr++=gx1;
				*uv_ptr++=tgy;

					// trig 2

				*vertex_ptr++=-(sinf(rxz2)*bot_reduce);
				*vertex_ptr++=f_by;
				*vertex_ptr++=cosf(rxz2)*bot_reduce;

				*uv_ptr++=gx2;
				*uv_ptr++=bgy;

				*vertex_ptr++=-(sinf(rxz2)*top_reduce);
				*vertex_ptr++=f_ty;
				*vertex_ptr++=cosf(rxz2)*top_reduce;

				*uv_ptr++=gx2;
				*uv_ptr++=tgy;

				*vertex_ptr++=-(sinf(rxz)*top_reduce);
				*vertex_ptr++=f_ty;
				*vertex_ptr++=cosf(rxz)*top_reduce;

				*uv_ptr++=gx1;
				*uv_ptr++=tgy;
			}
			else {

					// cap trig

				*vertex_ptr++=-(sinf(rxz)*bot_reduce);
				*vertex_ptr++=f_by;
				*vertex_ptr++=cosf(rxz)*bot_reduce;

				*uv_ptr++=gx1;
				*uv_ptr++=bgy;

				*vertex_ptr++=-(sinf(rxz2)*bot_reduce);
				*vertex_ptr++=f_by;
				*vertex_ptr++=cosf(rxz2)*bot_reduce;

				*uv_ptr++=gx2;
				*uv_ptr++=bgy;

				*vertex_ptr++=0.0f;
				*vertex_ptr++=f_ty;
				*vertex_ptr++=0.0f;

				*uv_ptr++=(gx1+gx2)*0.5f;
				*uv_ptr++=0.01f;
			}

				// flipped dome

			if (map.sky.dome_mirror) {

				if (i!=4) {

						// trig 1

					*vertex_ptr++=-(sinf(rxz)*bot_reduce);
					*vertex_ptr++=f_by2;
					*vertex_ptr++=cosf(rxz)*bot_reduce;
				
					*uv_ptr++=gx1;
					*uv_ptr++=bgy;

					*vertex_ptr++=-(sinf(rxz2)*bot_reduce);
					*vertex_ptr++=f_by2;
					*vertex_ptr++=cosf(rxz2)*bot_reduce;
					
					*uv_ptr++=gx2;
					*uv_ptr++=bgy;

					*vertex_ptr++=-(sinf(rxz)*top_reduce);
					*vertex_ptr++=f_ty2;
					*vertex_ptr++=cosf(rxz)*top_reduce;
					
					*uv_ptr++=gx1;
					*uv_ptr++=tgy;

						// trig 2

					*vertex_ptr++=-(sinf(rxz2)*bot_reduce);
					*vertex_ptr++=f_by2;
					*vertex_ptr++=cosf(rxz2)*bot_reduce;
					
					*uv_ptr++=gx2;
					*uv_ptr++=bgy;

					*vertex_ptr++=-(sinf(rxz2)*top_reduce);
					*vertex_ptr++=f_ty2;
					*vertex_ptr++=cosf(rxz2)*top_reduce;
					
					*uv_ptr++=gx2;
					*uv_ptr++=tgy;

					*vertex_ptr++=-(sinf(rxz)*top_reduce);
					*vertex_ptr++=f_ty2;
					*vertex_ptr++=cosf(rxz)*top_reduce;
					
					*uv_ptr++=gx1;
					*uv_ptr++=tgy;
				}
				else {

						// cap trig

					*vertex_ptr++=-(sinf(rxz)*bot_reduce);
					*vertex_ptr++=f_by2;
					*vertex_ptr++=cosf(rxz)*bot_reduce;
				
					*uv_ptr++=gx1;
					*uv_ptr++=bgy;

					*vertex_ptr++=-(sinf(rxz2)*bot_reduce);
					*vertex_ptr++=f_by2;
					*vertex_ptr++=cosf(rxz2)*bot_reduce;
					
					*uv_ptr++=gx2;
					*uv_ptr++=bgy;

					*vertex_ptr++=0.0f;
					*vertex_ptr++=f_ty2;
					*vertex_ptr++=0.0f;

					*uv_ptr++=(gx1+gx2)*0.5f;
					*uv_ptr++=0.01f;
				}
			}
			
			rxz+=r_add;
		}
	}

		// unmap and unbind vbo

  	view_unmap_sky_vertex_object();
	view_unbind_sky_vertex_object();
}

void sky_draw_dome_hemisphere(void)
{
    int					dome_cnt;
	d3col				col;
	texture_type		*texture;
	
		// setup texture
		
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	texture=&map.textures[map.sky.fill];
	col.r=col.g=col.b=1.0f;

		// bind the vbo

	view_bind_sky_vertex_object();

		// quad counts

	dome_cnt=((4*20)*6)+(20*3);
	if (map.sky.dome_mirror) dome_cnt*=2;

		// draw the dome
	
	gl_shader_draw_execute_simple_bitmap_set_texture(texture->frames[texture->animate.current_frame].bitmap.gl_id);
	gl_shader_draw_execute_simple_bitmap_start(3,0,((dome_cnt*3)*sizeof(float)),0,&col,1.0f);
	glDrawArrays(GL_TRIANGLES,0,dome_cnt);
	gl_shader_draw_execute_simple_bitmap_end();

		// unbind the vbo

	view_unbind_sky_vertex_object();
}

/* =======================================================

      Draw Cube Sky
      
======================================================= */

void sky_draw_cube_setup(void)
{
	int					sz;
    float				f_radius,txt_fact;
	float				*vertex_ptr,*uv_ptr;

		// construct VBO

	sz=6*4;
	view_create_sky_vertex_object(((sz*(3+2+4))*sizeof(float)));

	view_bind_sky_vertex_object();

	vertex_ptr=(float*)view_map_sky_vertex_object();
	if (vertex_ptr==NULL) {
		view_unbind_sky_vertex_object();
		return;
	}

	uv_ptr=vertex_ptr+(sz*3);

	txt_fact=map.sky.txt_fact;

		// setup cube quads
		
	f_radius=(float)map.sky.radius;
		
		// top
		
	if (map.sky.fill!=-1) {
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=txt_fact;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=txt_fact;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=0.0f;
	}
	
		// bottom
		
	if (map.sky.bottom_fill!=-1) {
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=txt_fact;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=txt_fact;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=0.0f;
	}
	
		// north
		
	if (map.sky.north_fill!=-1) {
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.00f;
		*uv_ptr++=0.00f;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.00f;
		*uv_ptr++=txt_fact;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=0.00f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=txt_fact;
	}
	
		// east

	if (map.sky.east_fill!=-1) {
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=txt_fact;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=0.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=txt_fact;
	}
	
		// south

	if (map.sky.south_fill!=-1) {
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=txt_fact;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=0.0f;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=txt_fact;
	}
	
		// west

	if (map.sky.west_fill!=-1) {
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=txt_fact;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=0.0f;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=txt_fact;
	}
 
		// unmap and unbind vbo

	view_unmap_sky_vertex_object();
	view_unbind_sky_vertex_object();
}

void sky_draw_cube(void)
{
    int					offset;
	d3col				col;
	texture_type		*texture;
					
		// setup texture
		
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	
	col.r=col.g=col.b=1.0f;
	
		// bind VBO

	view_bind_sky_vertex_object();

		// draw cube sides

	gl_shader_draw_execute_simple_bitmap_start(3,0,(((6*4)*3)*sizeof(float)),0,&col,1.0f);

	offset=0;

	if (map.sky.fill!=-1) {
		texture=&map.textures[map.sky.fill];
		gl_shader_draw_execute_simple_bitmap_set_texture(texture->frames[texture->animate.current_frame].bitmap.gl_id);
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		
		offset+=4;
	}

		// bottom
		
	if (map.sky.bottom_fill!=-1) {
		texture=&map.textures[map.sky.bottom_fill];
		gl_shader_draw_execute_simple_bitmap_set_texture(texture->frames[texture->animate.current_frame].bitmap.gl_id);
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glDrawArrays(GL_TRIANGLE_STRIP,offset,4);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		
		offset+=4;
	}
	
		// north
		
	if (map.sky.north_fill!=-1) {
		texture=&map.textures[map.sky.north_fill];
		gl_shader_draw_execute_simple_bitmap_set_texture(texture->frames[texture->animate.current_frame].bitmap.gl_id);
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glDrawArrays(GL_TRIANGLE_STRIP,offset,4);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		
		offset+=4;
	}

		// east

	if (map.sky.east_fill!=-1) {
		texture=&map.textures[map.sky.east_fill];
		gl_shader_draw_execute_simple_bitmap_set_texture(texture->frames[texture->animate.current_frame].bitmap.gl_id);
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glDrawArrays(GL_TRIANGLE_STRIP,offset,4);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		
		offset+=4;
	}
	
		// south

	if (map.sky.south_fill!=-1) {
		texture=&map.textures[map.sky.south_fill];
		gl_shader_draw_execute_simple_bitmap_set_texture(texture->frames[texture->animate.current_frame].bitmap.gl_id);
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glDrawArrays(GL_TRIANGLE_STRIP,offset,4);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		
		offset+=4;
	}
	
		// west

	if (map.sky.west_fill!=-1) {
		texture=&map.textures[map.sky.west_fill];
		gl_shader_draw_execute_simple_bitmap_set_texture(texture->frames[texture->animate.current_frame].bitmap.gl_id);
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glDrawArrays(GL_TRIANGLE_STRIP,offset,4);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	}

		// unbind the vbo

	gl_shader_draw_execute_simple_bitmap_end();

	view_unbind_sky_vertex_object();
}

/* =======================================================

      Initialize Sky Drawing
      
======================================================= */

void sky_draw_init(void)
{
		// is there a sky?
	
	if (!map.sky.on) return;

		// init vertex list

	switch (map.sky.type) {
	
		case st_dome_panoramic:
			sky_draw_dome_panoramic_setup();
			break;

		case st_dome_hemisphere:
			sky_draw_dome_hemisphere_setup();
			break;
			
		case st_cube:
			sky_draw_cube_setup();
			break;
	
	}
}

void sky_draw_release(void)
{
	if (map.sky.on) view_dispose_sky_vertex_object();
}

/* =======================================================

      Draw Sky Mainline
      
======================================================= */

void sky_draw(void)
{
	switch (map.sky.type) {
	
		case st_dome_panoramic:
			sky_draw_dome_panoramic();
			break;

		case st_dome_hemisphere:
			sky_draw_dome_hemisphere();
			break;
			
		case st_cube:
			sky_draw_cube();
			break;
	
	}
}


