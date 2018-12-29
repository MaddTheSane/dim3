/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Ring Drawing Routines

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
#include "objects.h"

extern map_type				map;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;

/* =======================================================

      Draw Ring Position
      
======================================================= */

void ring_draw_position(effect_type *effect,int count,d3pnt *pnt)
{
	float				m_tick;
	iface_ring_type		*ring;
	ring_effect_data	*eff_ring;
	
	eff_ring=&effect->data.ring;
	ring=&iface.ring_list.rings[eff_ring->ring_idx];

	m_tick=((float)count)/10.0f;
	
	pnt->x=effect->pnt.x+(int)(ring->vct.x*m_tick);
	pnt->y=effect->pnt.y+(int)(ring->vct.y*m_tick);
	pnt->z=effect->pnt.z+(int)(ring->vct.z*m_tick);
}

/* =======================================================

      Draw Rings
      
======================================================= */

void ring_draw(effect_type *effect,int count,int image_offset)
{
	int						n,nvertex,nindex,life_tick;
	unsigned short			v_idx;
	float					mx,my,mz,fx,fy,fz,
							outer_sz,inner_sz,rd,
							color_dif,alpha,gx,gy,g_size,
							f_count,f_tick;
	float					*pf,*vertex_ptr;
	unsigned short			*index_ptr;
	d3pnt					pnt;
	d3col					col;
	iface_ring_type			*ring;
	ring_effect_data		*eff_ring;
	matrix_type				mat_x,mat_y,mat_z;
	
	eff_ring=&effect->data.ring;
	ring=&iface.ring_list.rings[eff_ring->ring_idx];
	
		// get size
		
	life_tick=effect->life_tick;

	f_tick=(float)life_tick;
	f_count=(float)count;
	
	outer_sz=(float)(ring->end_outer_size-ring->start_outer_size);
	outer_sz=((outer_sz*f_count)/f_tick)+(float)ring->start_outer_size;

	inner_sz=(float)(ring->end_inner_size-ring->start_inner_size);
	inner_sz=((inner_sz*f_count)/f_tick)+(float)ring->start_inner_size;

		// color and alpha

	color_dif=ring->end_color.r-ring->start_color.r;
    col.r=ring->start_color.r+((color_dif*f_count)/f_tick);
	color_dif=ring->end_color.g-ring->start_color.g;
    col.g=ring->start_color.g+((color_dif*f_count)/f_tick);
	color_dif=ring->end_color.b-ring->start_color.b;
    col.b=ring->start_color.b+((color_dif*f_count)/f_tick);

	alpha=ring->end_alpha-ring->start_alpha;
	alpha=((alpha*f_count)/f_tick)+ring->start_alpha;
	
		// setup images
		
	effect_image_animate_get_uv(count,image_offset,&ring->animate,&gx,&gy,&g_size);
	
		// position and ring rotation

	ring_draw_position(effect,count,&pnt);

	mx=(float)pnt.x;
	my=(float)pnt.y;
	mz=(float)pnt.z;

	fx=f_count*ring->rot.x;
	fx+=(fx*ring->rot_accel.x);
	fx=angle_add(eff_ring->ang.x,fx);

	fy=f_count*ring->rot.y;
	fy+=(fy*ring->rot_accel.y);
	fy=angle_add(eff_ring->ang.y,fy);

	fz=f_count*ring->rot.z;
	fz+=(fz*ring->rot_accel.z);
	fz=angle_add(eff_ring->ang.z,fz);

	matrix_rotate_x(&mat_x,-fx);
	matrix_rotate_z(&mat_z,fz);
	matrix_rotate_y(&mat_y,fy);

		// construct VBO
		// effect vbos are dynamic, so it'll auto construct
		// the first time called

	nvertex=36*2;
	nindex=36*6;

	view_create_effect_vertex_object(effect,((nvertex*(3+2))*sizeof(float)),(nindex*sizeof(float)));

	view_bind_effect_vertex_object(effect);
	vertex_ptr=(float*)view_map_effect_vertex_object();
	if (vertex_ptr==NULL) {
		view_unbind_effect_vertex_object();
		return;
	}

		// create the ring vertexes

	pf=vertex_ptr;

	for (n=0;n!=360;n+=10) {
		rd=((float)n)*ANG_to_RAD;

			// outer

		fx=cosf(rd)*outer_sz;
		fy=-(sinf(rd)*outer_sz);
		fz=0.0f;

		matrix_vertex_multiply(&mat_x,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_z,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_y,&fx,&fy,&fz);

		*pf++=mx+fx;
		*pf++=my+fy;
		*pf++=mz+fz;

		*pf++=gx+(g_size*((fx+outer_sz)/(outer_sz*2.0f)));
		*pf++=gy+(g_size*((fy+outer_sz)/(outer_sz*2.0f)));

			// inner

		fx=cosf(rd)*inner_sz;
		fy=-(sinf(rd)*inner_sz);
		fz=0.0f;

		matrix_vertex_multiply(&mat_x,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_z,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_y,&fx,&fy,&fz);

		*pf++=mx+fx;
		*pf++=my+fy;
		*pf++=mz+fz;

		*pf++=gx+(g_size*((fx+outer_sz)/(outer_sz*2.0f)));
		*pf++=gy+(g_size*((fy+outer_sz)/(outer_sz*2.0f)));
	}

		// unmap vertex object

	view_unmap_effect_vertex_object();

		// create the indexes
		// last one needs to wrap around to beginning

	view_bind_effect_index_object(effect);
	index_ptr=(unsigned short*)view_map_effect_index_object();
	if (index_ptr==NULL) {
		view_unbind_effect_vertex_object();
		return;
	}

	v_idx=0;

	for (n=0;n!=35;n++) {
		*index_ptr++=v_idx;
		*index_ptr++=v_idx+2;
		*index_ptr++=v_idx+1;

		*index_ptr++=v_idx+1;
		*index_ptr++=v_idx+2;
		*index_ptr++=v_idx+3;

		v_idx+=2;
	}

	*index_ptr++=v_idx;
	*index_ptr++=0;
	*index_ptr++=v_idx+1;

	*index_ptr++=v_idx+1;
	*index_ptr++=0;
	*index_ptr++=1;

	view_unmap_effect_index_object();

		// draw ring
		
	gl_shader_draw_execute_simple_bitmap_set_texture(view_images_get_gl_id(ring->image_idx));
	gl_shader_draw_execute_simple_bitmap_start(3,0,(3*sizeof(float)),((3+2)*sizeof(float)),&col,alpha);
	
	glEnable(GL_BLEND);
	
	if (ring->blend_add) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glDrawElements(GL_TRIANGLES,nindex,GL_UNSIGNED_SHORT,(GLvoid*)0);
	
	glDepthMask(GL_TRUE);

	gl_shader_draw_execute_simple_bitmap_end();

		// unbind vertex/index object
		
	view_unbind_effect_index_object();
	view_unbind_effect_vertex_object();
}

