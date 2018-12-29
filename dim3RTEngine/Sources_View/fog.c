/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Fog

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
extern server_type			server;
extern view_type			view;
extern setup_type			setup;

bool						fog_vbo_created;

/* =======================================================

      Fog Initialize and Release
      
======================================================= */

void fog_draw_init(void)
{
	fog_vbo_created=FALSE;
}

void fog_draw_release(void)
{
	if (fog_vbo_created) view_dispose_fog_vertex_object();
}

/* =======================================================

      Textured Fog
      
======================================================= */

void fog_draw_textured(void)
{
	int					n,k,tick,count,outer_radius,inner_radius,
						radius_add,radius,frame,mem_sz;
	unsigned long		gl_id;
	float				r_ang,r_shift,r_ang_2,r_add,fx,fz,fx_1,fx_2,fz_1,fz_2,f_ty,f_by,
						f_radius,gx,gx2;
	float				*vp;
	d3col				col;
	texture_type		*texture;
	
		// textured fog on?
		
	if ((!map.fog.on) || (map.fog.use_solid_color)) return;
	
		// if this is first time, setup VBO
		
	if (!fog_vbo_created) {
		fog_vbo_created=TRUE;
		
		mem_sz=(((16*6)*map.fog.count)*(3+2))*sizeof(float);
		view_create_fog_vertex_object(mem_sz);
	}

		// drawing layers
		
	count=map.fog.count;
	outer_radius=map.fog.outer_radius;
	inner_radius=map.fog.inner_radius;

	radius_add=(inner_radius-outer_radius)/count;

		// construct VBO
		
	view_bind_fog_vertex_object();

	vp=(float*)view_map_fog_vertex_object();
	if (vp==NULL) {
		view_unbind_fog_vertex_object();
		return;
	}

		// get drawing setup

	tick=game_time_get();

	r_add=ANG_to_RAD*(360.0f/16.0f);
	r_shift=(ANG_to_RAD*360.0f)/((float)count);

	radius=outer_radius;
	
	fx=(float)view.render->camera.pnt.x;
	fz=(float)view.render->camera.pnt.z;

	f_ty=(float)(view.render->camera.pnt.y-map.fog.high);
	f_by=(float)(view.render->camera.pnt.y+map.fog.drop);

		// create the fog triangle vertexes
		
	for (n=0;n!=count;n++) {
	
		gx=0.0f;

		r_ang=r_shift*((float)n);
		f_radius=(float)radius;

		for (k=0;k!=16;k++) {

			r_ang_2=r_ang+r_add;

			fx_1=(-sinf(r_ang)*f_radius)+fx;
			fx_2=(-sinf(r_ang_2)*f_radius)+fx;

			fz_1=-(cosf(r_ang)*f_radius)+fz;
			fz_2=-(cosf(r_ang_2)*f_radius)+fz;
			
			gx2=gx+0.5f;

				// triangle 1
				
			*vp++=fx_1;
			*vp++=f_ty;
			*vp++=fz_1;

			*vp++=gx;
			*vp++=0.0f;
			
			*vp++=fx_2;
			*vp++=f_ty;
			*vp++=fz_2;

			*vp++=gx2;
			*vp++=0.0f;
			
			*vp++=fx_1;
			*vp++=f_by;
			*vp++=fz_1;

			*vp++=gx;
			*vp++=1.0f;
			
				// triangle 2
			
			*vp++=fx_2;
			*vp++=f_ty;
			*vp++=fz_2;

			*vp++=gx2;
			*vp++=0.0f;

			*vp++=fx_2;
			*vp++=f_by;
			*vp++=fz_2;

			*vp++=gx2;
			*vp++=1.0f;
			
			*vp++=fx_1;
			*vp++=f_by;
			*vp++=fz_1;

			*vp++=gx;
			*vp++=1.0f;

			r_ang=r_ang_2;
			
			gx+=0.5f;
			if (gx==1.0f) gx=0.0f;
		}

		radius+=radius_add;
	}

  	view_unmap_fog_vertex_object();

		// setup drawing
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	
	col.r=col.g=col.b=1.0f;

	texture=&map.textures[map.fog.texture_idx];
	frame=texture->animate.current_frame&max_texture_frame_mask;
	gl_id=texture->frames[frame].bitmap.gl_id;

		// draw the fog

	gl_shader_draw_execute_simple_bitmap_set_texture(gl_id);
	gl_shader_draw_execute_simple_bitmap_start(3,0,(3*sizeof(float)),((3+2)*sizeof(float)),&col,map.fog.alpha);
	glDrawArrays(GL_TRIANGLES,0,((16*6)*count));
	gl_shader_draw_execute_simple_bitmap_end();
	
	glDepthMask(GL_TRUE);

		// unbind the vbo

	view_unbind_fog_vertex_object();
}

/* =======================================================

      Solid Fog
      
======================================================= */

bool fog_solid_on(void)
{
	return((map.fog.on) && (map.fog.use_solid_color));
}
