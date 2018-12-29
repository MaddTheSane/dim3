/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View dim3RTL Effects

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
#include "ray_interface.h"

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

extern int					view_rtl_draw_scene_id;

/* =======================================================

      dim3RTL Effect Meshes
      
======================================================= */

void view_dim3rtl_effect_mesh_setup(effect_type *effect)
{
	int						quad_count;
	iface_particle_type		*particle;

		// only create meshes for
		// particles or rings

	effect->rtl_mesh_id=-1;
	if ((effect->effecttype!=ef_particle) && (effect->effecttype!=ef_ring)) return;

		// create mesh
		
	effect->rtl_mesh_id=rtlSceneMeshAdd(view_rtl_draw_scene_id,(RL_MESH_FLAG_NON_LIGHT_TRACE_BLOCKING|RL_MESH_FLAG_HIGHLIGHT));
	if (effect->rtl_mesh_id==-1) return;

		// create vertexes and uvs

	switch (effect->effecttype) {

		case ef_particle:
			particle=&iface.particle_list.particles[effect->data.particle.particle_idx];
			quad_count=particle->count*(particle->trail_count+1);
			rtlSceneMeshSetVertex(view_rtl_draw_scene_id,effect->rtl_mesh_id,RL_MESH_FORMAT_VERTEX_3_FLOAT,(quad_count*4),NULL);
			rtlSceneMeshSetUV(view_rtl_draw_scene_id,effect->rtl_mesh_id,RL_MESH_FORMAT_UV_2_FLOAT,(quad_count*4),NULL);
			break;

		case ef_ring:
			rtlSceneMeshSetVertex(view_rtl_draw_scene_id,effect->rtl_mesh_id,RL_MESH_FORMAT_VERTEX_3_FLOAT,(36*2),NULL);
			rtlSceneMeshSetUV(view_rtl_draw_scene_id,effect->rtl_mesh_id,RL_MESH_FORMAT_UV_2_FLOAT,(36*2),NULL);
			break;
	}

}

void view_dim3rtl_effect_mesh_close(effect_type *effect)
{
	if (effect->rtl_mesh_id!=-1) {
		rtlSceneMeshDelete(view_rtl_draw_scene_id,effect->rtl_mesh_id);
		effect->rtl_mesh_id=-1;
	}
}

/* =======================================================

      Remove and Reload Meshes for Load/Save
      
======================================================= */

void view_dim3rtl_effect_mesh_reload_all(void)
{
	int					n;
	effect_type			*effect;

	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (effect==NULL) continue;

		if (effect->on) view_dim3rtl_effect_mesh_setup(effect);

		effect->rtl_mesh_id=-1;
	}
}

void view_dim3rtl_effect_mesh_close_all(void)
{
	int					n;
	effect_type			*effect;

	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (effect==NULL) continue;

		if (effect->on) view_dim3rtl_effect_mesh_close(effect);
	}
}

/* =======================================================

      Update dim3RTL Particle Meshes
      
======================================================= */

void view_dim3rtl_effect_mesh_particle_quad(float *vp,float *uv,d3pnt *pnt,d3ang *rot_ang,float pixel_size,matrix_type *pixel_x_mat,matrix_type *pixel_y_mat,float gravity,float f_count,int particle_count,iface_particle_piece_type *pps,float gx,float gy,float g_size)
{
	int					n,k;
	float				fx,fy,fz,px[4],py[4],pz[4];
	matrix_type			rot_mat;
	
		// if move angle, setup matrixes
		
	if (rot_ang!=NULL) matrix_rotate_xzy(&rot_mat,rot_ang->x,rot_ang->y,rot_ang->z);
	
		// get particle box
		
	px[0]=px[3]=-pixel_size;
	px[1]=px[2]=pixel_size;
	py[0]=py[1]=-pixel_size;
	py[2]=py[3]=pixel_size;
	pz[0]=pz[1]=pz[2]=pz[3]=0.0f;
	
	for (k=0;k!=4;k++) {
		matrix_vertex_multiply_ignore_transform(pixel_x_mat,&px[k],&py[k],&pz[k]);
		matrix_vertex_multiply_ignore_transform(pixel_y_mat,&px[k],&py[k],&pz[k]);
	}
	
		// fill particle arrays
	
	for (n=0;n!=particle_count;n++) {
	
			// particle quad
			
		if (rot_ang!=NULL) {			// rotated particles
			fx=pps->vct.x;
			fy=pps->vct.y;
			fz=pps->vct.z;
			
			matrix_vertex_multiply_ignore_transform(&rot_mat,&fx,&fy,&fz);
		
			fx=(float)(pnt->x+pps->pt.x)+(fx*f_count);
			fy=(float)(pnt->y+pps->pt.y)+((fy*f_count)+gravity);
			fz=(float)(pnt->z+pps->pt.z)+(fz*f_count);
		}
		else {							// normal particles
			fx=(float)(pnt->x+pps->pt.x)+((pps->vct.x)*f_count);
			fy=(float)(pnt->y+pps->pt.y)+(((pps->vct.y)*f_count)+gravity);
			fz=(float)(pnt->z+pps->pt.z)+((pps->vct.z)*f_count);
		}
		
		pps++;

			// the quad

		*vp++=(px[0]+fx);
		*vp++=(py[0]+fy);
		*vp++=(pz[0]+fz);
		
		*uv++=gx;
		*uv++=gy;

		*vp++=(px[1]+fx);
		*vp++=(py[1]+fy);
		*vp++=(pz[1]+fz);
		
		*uv++=gx+g_size;
		*uv++=gy;

		*vp++=(px[2]+fx);
		*vp++=(py[2]+fy);
		*vp++=(pz[2]+fz);
		
		*uv++=gx+g_size;
		*uv++=gy+g_size;

		*vp++=(px[3]+fx);
		*vp++=(py[3]+fy);
		*vp++=(pz[3]+fz);
		
		*uv++=gx;
		*uv++=gy+g_size;

			// change particle image
		
		if (g_size==1.0f) continue;
		
		if (gx<1.0f) {
			gx+=g_size;
			continue;
		}

		gx=0.0f;
		gy+=g_size;
		if (gy>=1.0f) gy=0.0f;
	}
}

void view_dim3rtl_effect_mesh_particle_update(effect_type *effect,int count,int image_offset)
{
	int						n,k,idx,particle_count,quad_count,
							ntrail,pixel_dif,material_id;
	short					*polys,*vk;
	float					gravity,gx,gy,g_size,pixel_sz,f,trail_step,
							alpha_dif,color_dif,f_count,f_tick;
	float					*vp,*uv;
	d3pnt					pnt;
	d3ang					*rot_ang,rang;
	rtlColor				col;
	iface_particle_type		*particle;
	particle_effect_data	*eff_particle;
	matrix_type				pixel_x_mat,pixel_y_mat;

		// have a mesh?

	if (effect->rtl_mesh_id==-1) return;

		// get particle
	
	eff_particle=&effect->data.particle;
	particle=&iface.particle_list.particles[eff_particle->particle_idx];

	f_count=(float)count;

		// position
	
	particle_draw_position(effect,count,&pnt);
	
		// particle move rotation
		// we can have rotations from being attached to a bone of a model
		// or the timed rotations from the particle setup
		// we only want to set these up if the rotation is necessary as it
		// saves time over thousands of particles
		
	rot_ang=NULL;

	if (eff_particle->rot.on) {
		rang.x=eff_particle->rot.ang.x;
		rang.y=eff_particle->rot.ang.y;
		rang.z=eff_particle->rot.ang.z;
		rot_ang=&rang;
	}

	if ((particle->rot.x!=0.0f) || (particle->rot.y!=0.0f) || (particle->rot.z!=0.0f)) {

		if (rot_ang==NULL) {
			rang.x=rang.y=rang.z=0.0f;
			rot_ang=&rang;
		}

		f=f_count*particle->rot.x;
		f+=(f*particle->rot_accel.x);
		rot_ang->x=angle_add(rot_ang->x,f);

		f=f_count*particle->rot.y;
		f+=(f*particle->rot_accel.y);
		rot_ang->y=angle_add(rot_ang->y,f);

		f=f_count*particle->rot.z;
		f+=(f*particle->rot_accel.z);
		rot_ang->z=angle_add(rot_ang->z,f);
	}

		// particle billboard rotation
		
	matrix_rotate_x(&pixel_x_mat,view.render->camera.ang.x);
	matrix_rotate_y(&pixel_y_mat,view.render->camera.ang.y);
	
		// reverse
		
	if (particle->reverse) count=(effect->life_tick-count);

		// setup size

	f_tick=(float)effect->life_tick;
		
	pixel_dif=particle->end_pixel_size-particle->start_pixel_size;
    pixel_sz=(float)(particle->start_pixel_size+((pixel_dif*count)/effect->life_tick));
	
		// setup images
		
	effect_image_animate_get_uv(count,image_offset,&particle->animate,&gx,&gy,&g_size);

		// reduce x/z/y movement and add in offset
		
	count=count/10;

		// construct meshes
		// each particle is a quad

	quad_count=particle->count*(particle->trail_count+1);

	rtlSceneMeshMapVertexPointer(view_rtl_draw_scene_id,effect->rtl_mesh_id,(void**)&vp);
	rtlSceneMeshMapUVPointer(view_rtl_draw_scene_id,effect->rtl_mesh_id,(void**)&uv);

		// setup the vertexes

	particle_count=particle->count;
	ntrail=particle->trail_count+1;
	
	trail_step=particle->trail_step;
	f_count=(float)count;
		
	for (n=0;n!=ntrail;n++) {

			// get gravity

		gravity=particle_get_gravity(particle,count);

			// create mesh quads
			
		view_dim3rtl_effect_mesh_particle_quad(vp,uv,&pnt,rot_ang,pixel_sz,&pixel_x_mat,&pixel_y_mat,gravity,f_count,particle_count,particle->pieces[eff_particle->variation_idx],gx,gy,g_size);

		vp+=((particle->count*3)*4);
		uv+=((particle->count*2)*4);

			// reduce pixel sizes and counts for trails
			
		pixel_sz=pixel_sz*particle->reduce_pixel_fact;
		f_count-=trail_step;
	}

	rtlSceneMeshUnMapVertexPointer(view_rtl_draw_scene_id,effect->rtl_mesh_id);
	rtlSceneMeshUnMapUVPointer(view_rtl_draw_scene_id,effect->rtl_mesh_id);

		// build the polygons

	material_id=particle->rtl_material_id;

	polys=(short*)malloc(sizeof(short)*(quad_count*10));
	vk=polys;

	idx=0;

	for (n=0;n!=quad_count;n++) {
		*vk++=4;
		*vk++=material_id;
		for (k=0;k!=4;k++) {
			*vk++=idx;
			*vk++=idx;
			idx++;
		}
	}

	rtlSceneMeshSetPoly(view_rtl_draw_scene_id,effect->rtl_mesh_id,RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV,quad_count,polys);
	free(polys);
	
		// setup color and alpha
		
	color_dif=particle->end_color.r-particle->start_color.r;
    col.r=particle->start_color.r+((color_dif*f_count)/f_tick);
	color_dif=particle->end_color.g-particle->start_color.g;
    col.g=particle->start_color.g+((color_dif*f_count)/f_tick);
	color_dif=particle->end_color.b-particle->start_color.b;
    col.b=particle->start_color.b+((color_dif*f_count)/f_tick);

	alpha_dif=particle->end_alpha-particle->start_alpha;
    col.a=particle->start_alpha+((alpha_dif*f_count)/f_tick);

	rtlSceneMeshSetPolyColorAll(view_rtl_draw_scene_id,effect->rtl_mesh_id,&col);
}

/* =======================================================

      Update dim3RTL Ring Meshes
      
======================================================= */

void view_dim3rtl_effect_mesh_ring_update(effect_type *effect,int count,int image_offset)
{
	int						n,k,life_tick,idx,
							material_id;
	short					*polys,*vk;
	float					mx,my,mz,fx,fy,fz,
							outer_sz,inner_sz,rd,
							color_dif,alpha,gx,gy,g_size,
							f_count,f_tick;
	float					*vp,*uv;
	d3pnt					pnt;
	rtlColor				col;
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

		// create the ring vertexes

	rtlSceneMeshMapVertexPointer(view_rtl_draw_scene_id,effect->rtl_mesh_id,(void**)&vp);
	rtlSceneMeshMapUVPointer(view_rtl_draw_scene_id,effect->rtl_mesh_id,(void**)&uv);

	for (n=0;n!=360;n+=10) {
		rd=((float)n)*ANG_to_RAD;

			// outer

		fx=cosf(rd)*outer_sz;
		fy=-(sinf(rd)*outer_sz);
		fz=0.0f;

		matrix_vertex_multiply(&mat_x,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_z,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_y,&fx,&fy,&fz);

		*vp++=mx+fx;
		*vp++=my+fy;
		*vp++=mz+fz;

		*uv++=gx+(g_size*((fx+outer_sz)/(outer_sz*2.0f)));
		*uv++=gy+(g_size*((fy+outer_sz)/(outer_sz*2.0f)));

			// inner

		fx=cosf(rd)*inner_sz;
		fy=-(sinf(rd)*inner_sz);
		fz=0.0f;

		matrix_vertex_multiply(&mat_x,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_z,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_y,&fx,&fy,&fz);

		*vp++=mx+fx;
		*vp++=my+fy;
		*vp++=mz+fz;

		*uv++=gx+(g_size*((fx+outer_sz)/(outer_sz*2.0f)));
		*uv++=gy+(g_size*((fy+outer_sz)/(outer_sz*2.0f)));
	}

	rtlSceneMeshUnMapVertexPointer(view_rtl_draw_scene_id,effect->rtl_mesh_id);
	rtlSceneMeshUnMapUVPointer(view_rtl_draw_scene_id,effect->rtl_mesh_id);

		// create the polys
		// last one needs to wrap around to beginning

	material_id=ring->rtl_material_id;

	polys=(short*)malloc(sizeof(short)*(36*10));
	vk=polys;

	idx=0;

	for (n=0;n!=36;n++) {
		*vk++=4;
		*vk++=material_id;

		*vk++=idx;
		*vk++=idx;

		*vk++=idx+1;
		*vk++=idx+1;

		if (n!=35) {
			k=idx+2;
		}
		else {
			k=0;
		}

		*vk++=k+1;
		*vk++=k+1;

		*vk++=k;
		*vk++=k;
		
		idx+=2;
	}

	rtlSceneMeshSetPoly(view_rtl_draw_scene_id,effect->rtl_mesh_id,RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV,36,polys);
	free(polys);

		// set color and alpha

	color_dif=ring->end_color.r-ring->start_color.r;
    col.r=ring->start_color.r+((color_dif*f_count)/f_tick);
	color_dif=ring->end_color.g-ring->start_color.g;
    col.g=ring->start_color.g+((color_dif*f_count)/f_tick);
	color_dif=ring->end_color.b-ring->start_color.b;
    col.b=ring->start_color.b+((color_dif*f_count)/f_tick);

	alpha=ring->end_alpha-ring->start_alpha;
	col.a=((alpha*f_count)/f_tick)+ring->start_alpha;

	rtlSceneMeshSetPolyColorAll(view_rtl_draw_scene_id,effect->rtl_mesh_id,&col);
}

/* =======================================================

      Update dim3RTL Effect Meshes
      
======================================================= */

void view_dim3rtl_effect_mesh_update(void)
{
	int					n,count;
	effect_type			*effect;

	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (effect==NULL) continue;

		if (!effect->on) continue;
		if (effect->rtl_mesh_id==-1) continue;

		count=game_time_get()-effect->start_tick;

		switch (effect->effecttype) {

			case ef_particle:
				view_dim3rtl_effect_mesh_particle_update(effect,count,n);
				break;

			case ef_ring:
				view_dim3rtl_effect_mesh_ring_update(effect,count,n);
				break;

		}
	}
}
