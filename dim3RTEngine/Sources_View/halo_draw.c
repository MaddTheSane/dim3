/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Halo Drawing

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
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;

/* =======================================================

      Add Halos
      
======================================================= */

void halo_draw_clear(void)
{
	view.render->halo_draw.count=0;
}

void halo_draw_add(d3pnt *pnt,int obj_idx,int halo_idx)
{
	halo_draw_type		*halo_draw;
	
	if (halo_idx==-1) return;
	if (view.render->halo_draw.count>=max_light_spot) return;
	
	halo_draw=&view.render->halo_draw.halos[view.render->halo_draw.count];
	view.render->halo_draw.count++;
	
	halo_draw->idx=halo_idx;

	halo_draw->obj_idx=obj_idx;
	
	halo_draw->pnt.x=pnt->x;
	halo_draw->pnt.y=pnt->y;
	halo_draw->pnt.z=pnt->z;
}

/* =======================================================

      Setup Halos
      
======================================================= */

bool halo_draw_setup_cull(iface_halo_type *halo,int obj_idx,d3pnt *pnt,int *p_pixel_sz,float *p_alpha)
{
	int						pixel_sz,dist,d;
	float					alpha;
	bool					hit;
	d3pnt					ept;
	ray_trace_contact_type	contact;
	
		// is ray greater than max distance?

	dist=distance_get(pnt->x,pnt->y,pnt->z,view.render->camera.pnt.x,view.render->camera.pnt.y,view.render->camera.pnt.z);
	if (dist>halo->max_dist) return(FALSE);

		// ray trace for visibily

	contact.proj.on=FALSE;
	contact.origin=poly_ray_trace_origin_halo;
	contact.obj.on=!halo->no_clip_object;

	if ((halo->no_clip_self) && (obj_idx!=-1)) {
		contact.obj.ignore_idx=obj_idx;
	}
	else {
		contact.obj.ignore_idx=-1;
	}

	ept.x=view.render->camera.pnt.x;
	ept.y=view.render->camera.pnt.y;
	ept.z=view.render->camera.pnt.z;

	hit=ray_trace_map_by_point(pnt,&ept,&contact);

		// check hit and ignore hitting the projecting
		// player

	if (hit) {
		if ((contact.poly.mesh_idx!=-1) || (map.camera.camera_mode!=cv_fpp)) return(FALSE);
		if (contact.obj.idx!=server.player_obj_idx) return(FALSE);
	}

		// get size

	if (dist>=halo->max_dist) {
		pixel_sz=halo->max_size;
		alpha=halo->max_alpha;
	}
	else {
		if (dist<=halo->min_dist) {
			pixel_sz=halo->min_size;
			alpha=halo->min_alpha;
		}
		else {
			dist-=halo->min_dist;
			
			d=halo->max_dist-halo->min_dist;
			pixel_sz=halo->max_size-halo->min_size;
			alpha=halo->max_alpha-halo->min_alpha;
			
			pixel_sz=((pixel_sz*dist)/d)+halo->min_size;
			alpha=((alpha*(float)dist)/(float)d)+halo->min_alpha;
		}
	}

	*p_pixel_sz=pixel_sz;
	*p_alpha=alpha;

	return(TRUE);
}

void halo_draw_setup(void)
{
	int						n;
	halo_draw_type			*halo_draw;
	
	view.render->halo_draw.in_view_count=0;
	
		// clear all halo spots
	
	for (n=0;n!=view.render->halo_draw.count;n++) {

		halo_draw=&view.render->halo_draw.halos[n];
		halo_draw->in_view=FALSE;

			// is it behind the z?

		if (view_dim3rtl_project_point_behind_z(&halo_draw->pnt)) continue;
				
			// cull halo

		if (!halo_draw_setup_cull(&iface.halo_list.halos[halo_draw->idx],halo_draw->obj_idx,&halo_draw->pnt,&halo_draw->pixel_sz,&halo_draw->alpha)) continue;

			// project the halo
			// and check if on screen

		halo_draw->proj_pnt.x=halo_draw->pnt.x;
		halo_draw->proj_pnt.y=halo_draw->pnt.y;
		halo_draw->proj_pnt.z=halo_draw->pnt.z;
			
		view_dim3rtl_project_point(&halo_draw->proj_pnt);
		
		if (((halo_draw->proj_pnt.x+halo_draw->pixel_sz)<0) || ((halo_draw->proj_pnt.y+halo_draw->pixel_sz)<0) || ((halo_draw->proj_pnt.x-halo_draw->pixel_sz)>=view.screen.x_sz) || ((halo_draw->proj_pnt.y-halo_draw->pixel_sz)>=view.screen.y_sz)) continue;

		halo_draw->in_view=TRUE;
		view.render->halo_draw.in_view_count++;
	}
}

/* =======================================================

      Render Halos
      
======================================================= */

void halo_draw_render(void)
{
	int						n,x,y,psz;
	float					*vp;
	d3col					col;
	halo_draw_type			*halo_draw;
	
		// any halos to draw?
		
	if (view.render->halo_draw.in_view_count==0) return;

		// setup texture drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
		// get color

	col.r=col.g=col.b=1.0f;

		// draw halos
		
	for (n=0;n!=view.render->halo_draw.count;n++) {
		halo_draw=&view.render->halo_draw.halos[n];
		if (!halo_draw->in_view) continue;
	
			// halo size
			
		x=halo_draw->proj_pnt.x;
		y=halo_draw->proj_pnt.y;
		psz=halo_draw->pixel_sz>>1;

			// setup vertex

		view_bind_utility_vertex_object();
		vp=(float*)view_map_utility_vertex_object();

		*vp++=(float)(x-psz);
		*vp++=(float)(y-psz);
		*vp++=0.0f;
		*vp++=0.0f;

		*vp++=(float)(x-psz);
		*vp++=(float)(y+psz);
		*vp++=0.0f;
		*vp++=1.0f;

		*vp++=(float)(x+psz);
		*vp++=(float)(y-psz);
		*vp++=1.0f;
		*vp++=0.0f;

		*vp++=(float)(x+psz);
		*vp++=(float)(y+psz);
		*vp++=1.0f;
		*vp++=1.0f;

		view_unmap_utility_vertex_object();

			// draw halo

		gl_shader_draw_execute_simple_bitmap_set_texture(view_images_get_gl_id(iface.halo_list.halos[halo_draw->idx].image_idx));
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

		gl_shader_draw_execute_simple_bitmap_start(2,0,(2*sizeof(float)),((2+2)*sizeof(float)),&col,halo_draw->alpha);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		
		gl_shader_draw_execute_simple_bitmap_end();

		view_unbind_utility_vertex_object();
	}
}

