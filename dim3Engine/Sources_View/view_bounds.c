/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Checking For Item In View Bounds

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

float						gl_proj_matrix[16],gl_model_view_matrix[16],
							view_cull_frustum[6][4];

/* =======================================================

      Frustum/Bound Box Clipping
      
======================================================= */

void view_cull_setup_frustum_clipping_planes(void)
{
	int			n;
	float		f,clip[16];

		// combine the matrixes

	clip[0]=(gl_model_view_matrix[0]*gl_proj_matrix[0])+(gl_model_view_matrix[1]*gl_proj_matrix[4])+(gl_model_view_matrix[2]*gl_proj_matrix[8])+(gl_model_view_matrix[3]*gl_proj_matrix[12]);
	clip[1]=(gl_model_view_matrix[0]*gl_proj_matrix[1])+(gl_model_view_matrix[1]*gl_proj_matrix[5])+(gl_model_view_matrix[2]*gl_proj_matrix[9])+(gl_model_view_matrix[3]*gl_proj_matrix[13]);
	clip[2]=(gl_model_view_matrix[0]*gl_proj_matrix[2])+(gl_model_view_matrix[1]*gl_proj_matrix[6])+(gl_model_view_matrix[2]*gl_proj_matrix[10])+(gl_model_view_matrix[3]*gl_proj_matrix[14]);
	clip[3]=(gl_model_view_matrix[0]*gl_proj_matrix[3])+(gl_model_view_matrix[1]*gl_proj_matrix[7])+(gl_model_view_matrix[2]*gl_proj_matrix[11])+(gl_model_view_matrix[3]*gl_proj_matrix[15]);

	clip[4]=(gl_model_view_matrix[4]*gl_proj_matrix[0])+(gl_model_view_matrix[5]*gl_proj_matrix[4])+(gl_model_view_matrix[6]*gl_proj_matrix[8])+(gl_model_view_matrix[7]*gl_proj_matrix[12]);
	clip[5]=(gl_model_view_matrix[4]*gl_proj_matrix[1])+(gl_model_view_matrix[5]*gl_proj_matrix[5])+(gl_model_view_matrix[6]*gl_proj_matrix[9])+(gl_model_view_matrix[7]*gl_proj_matrix[13]);
	clip[6]=(gl_model_view_matrix[4]*gl_proj_matrix[2])+(gl_model_view_matrix[5]*gl_proj_matrix[6])+(gl_model_view_matrix[6]*gl_proj_matrix[10])+(gl_model_view_matrix[7]*gl_proj_matrix[14]);
	clip[7]=(gl_model_view_matrix[4]*gl_proj_matrix[3])+(gl_model_view_matrix[5]*gl_proj_matrix[7])+(gl_model_view_matrix[6]*gl_proj_matrix[11])+(gl_model_view_matrix[7]*gl_proj_matrix[15]);

	clip[8]=(gl_model_view_matrix[8]*gl_proj_matrix[0])+(gl_model_view_matrix[9]*gl_proj_matrix[4])+(gl_model_view_matrix[10]*gl_proj_matrix[8])+(gl_model_view_matrix[11]*gl_proj_matrix[12]);
	clip[9]=(gl_model_view_matrix[8]*gl_proj_matrix[1])+(gl_model_view_matrix[9]*gl_proj_matrix[5])+(gl_model_view_matrix[10]*gl_proj_matrix[9])+(gl_model_view_matrix[11]*gl_proj_matrix[13]);
	clip[10]=(gl_model_view_matrix[8]*gl_proj_matrix[2])+(gl_model_view_matrix[9]*gl_proj_matrix[6])+(gl_model_view_matrix[10]*gl_proj_matrix[10])+(gl_model_view_matrix[11]*gl_proj_matrix[14]);
	clip[11]=(gl_model_view_matrix[8]*gl_proj_matrix[3])+(gl_model_view_matrix[9]*gl_proj_matrix[7])+(gl_model_view_matrix[10]*gl_proj_matrix[11])+(gl_model_view_matrix[11]*gl_proj_matrix[15]);

	clip[12]=(gl_model_view_matrix[12]*gl_proj_matrix[0])+(gl_model_view_matrix[13]*gl_proj_matrix[4])+(gl_model_view_matrix[14]*gl_proj_matrix[8])+(gl_model_view_matrix[15]*gl_proj_matrix[12]);
	clip[13]=(gl_model_view_matrix[12]*gl_proj_matrix[1])+(gl_model_view_matrix[13]*gl_proj_matrix[5])+(gl_model_view_matrix[14]*gl_proj_matrix[9])+(gl_model_view_matrix[15]*gl_proj_matrix[13]);
	clip[14]=(gl_model_view_matrix[12]*gl_proj_matrix[2])+(gl_model_view_matrix[13]*gl_proj_matrix[6])+(gl_model_view_matrix[14]*gl_proj_matrix[10])+(gl_model_view_matrix[15]*gl_proj_matrix[14]);
	clip[15]=(gl_model_view_matrix[12]*gl_proj_matrix[3])+(gl_model_view_matrix[13]*gl_proj_matrix[7])+(gl_model_view_matrix[14]*gl_proj_matrix[11])+(gl_model_view_matrix[15]*gl_proj_matrix[15]);

		// left plane
		
	view_cull_frustum[0][0]=clip[3]+clip[0];
	view_cull_frustum[0][1]=clip[7]+clip[4];
	view_cull_frustum[0][2]=clip[11]+clip[8];
	view_cull_frustum[0][3]=clip[15]+clip[12];

		// right plane
		
	view_cull_frustum[1][0]=clip[3]-clip[0];
	view_cull_frustum[1][1]=clip[7]-clip[4];
	view_cull_frustum[1][2]=clip[11]-clip[8];
	view_cull_frustum[1][3]=clip[15]-clip[12];

		// top plane
		
	view_cull_frustum[2][0]=clip[3]-clip[1];
	view_cull_frustum[2][1]=clip[7]-clip[5];
	view_cull_frustum[2][2]=clip[11]-clip[9];
	view_cull_frustum[2][3]=clip[15]-clip[13];

		// bottom plane
		
	view_cull_frustum[3][0]=clip[3]+clip[1];
	view_cull_frustum[3][1]=clip[7]+clip[5];
	view_cull_frustum[3][2]=clip[11]+clip[9];
	view_cull_frustum[3][3]=clip[15]+clip[13];

		// near plane
		
	view_cull_frustum[4][0]=clip[3]+clip[2];
	view_cull_frustum[4][1]=clip[7]+clip[6];
	view_cull_frustum[4][2]=clip[11]+clip[10];
	view_cull_frustum[4][3]=clip[15]+clip[14];

		// far plane
		
	view_cull_frustum[5][0]=clip[3]-clip[2];
	view_cull_frustum[5][1]=clip[7]-clip[6];
	view_cull_frustum[5][2]=clip[11]-clip[10];
	view_cull_frustum[5][3]=clip[15]-clip[14];
	
		// normalize the ABCD plane
		
	for (n=0;n!=6;n++) {
		f=sqrtf((view_cull_frustum[n][0]*view_cull_frustum[n][0])+(view_cull_frustum[n][1]*view_cull_frustum[n][1])+(view_cull_frustum[n][2]*view_cull_frustum[n][2]));
		view_cull_frustum[n][0]/=f;
		view_cull_frustum[n][1]/=f;
		view_cull_frustum[n][2]/=f;
		view_cull_frustum[n][3]/=f;
	}
}

bool view_cull_boundbox_in_frustum(d3pnt *min,d3pnt *max)
{
	int				n;
	d3fpnt			f_min,f_max;
	
	f_min.x=(float)min->x;
	f_min.y=(float)min->y;
	f_min.z=(float)min->z;

	f_max.x=(float)max->x;
	f_max.y=(float)max->y;
	f_max.z=(float)max->z;

	for (n=0;n!=6;n++) {
		if (((view_cull_frustum[n][0]*f_min.x)+(view_cull_frustum[n][1]*f_min.y)+(view_cull_frustum[n][2]*f_min.z)+view_cull_frustum[n][3])>0.0f) continue;
		if (((view_cull_frustum[n][0]*f_max.x)+(view_cull_frustum[n][1]*f_min.y)+(view_cull_frustum[n][2]*f_min.z)+view_cull_frustum[n][3])>0.0f) continue;
		if (((view_cull_frustum[n][0]*f_min.x)+(view_cull_frustum[n][1]*f_max.y)+(view_cull_frustum[n][2]*f_min.z)+view_cull_frustum[n][3])>0.0f) continue;
		if (((view_cull_frustum[n][0]*f_max.x)+(view_cull_frustum[n][1]*f_max.y)+(view_cull_frustum[n][2]*f_min.z)+view_cull_frustum[n][3])>0.0f) continue;
		if (((view_cull_frustum[n][0]*f_min.x)+(view_cull_frustum[n][1]*f_min.y)+(view_cull_frustum[n][2]*f_max.z)+view_cull_frustum[n][3])>0.0f) continue;
		if (((view_cull_frustum[n][0]*f_max.x)+(view_cull_frustum[n][1]*f_min.y)+(view_cull_frustum[n][2]*f_max.z)+view_cull_frustum[n][3])>0.0f) continue;
		if (((view_cull_frustum[n][0]*f_min.x)+(view_cull_frustum[n][1]*f_max.y)+(view_cull_frustum[n][2]*f_max.z)+view_cull_frustum[n][3])>0.0f) continue;
		if (((view_cull_frustum[n][0]*f_max.x)+(view_cull_frustum[n][1]*f_max.y)+(view_cull_frustum[n][2]*f_max.z)+view_cull_frustum[n][3])>0.0f) continue;

		return(FALSE);
	}

	return(TRUE);
}

/* =======================================================

      Distances to View Center
      
======================================================= */

int view_cull_distance_to_view_center(int x,int y,int z)
{
	float			fx,fy,fz;
	
	fx=(float)(x-view.render->camera.pnt.x);
	fy=(float)(y-view.render->camera.pnt.y);
	fz=(float)(z-view.render->camera.pnt.z);
	return((int)sqrtf((fx*fx)+(fy*fy)+(fz*fz)));
}

/* =======================================================

      Pieces in View
      
======================================================= */

bool view_cull_mesh(map_mesh_type *mesh)
{
	int				obscure_dist;

		// check obscure distance

	if (!fog_solid_on()) {
		obscure_dist=map.camera.plane.far_z-map.camera.plane.near_z;
	}
	else {
		obscure_dist=(map.fog.outer_radius>>1)*3;
	}

	if (map_mesh_calculate_distance(mesh,&view.render->camera.pnt)>obscure_dist) return(FALSE);

		// clip against view frustum
	
	return(view_cull_boundbox_in_frustum(&mesh->box.min,&mesh->box.max));
}

bool view_cull_liquid(map_liquid_type *liq)
{
	int				obscure_dist;
	d3pnt			min,max;

		// check obscure distance

	if (!fog_solid_on()) {
		obscure_dist=map.camera.plane.far_z-map.camera.plane.near_z;
	}
	else {
		obscure_dist=(map.fog.outer_radius>>1)*3;
	}
		
	if (map_liquid_calculate_distance(liq,&view.render->camera.pnt)>obscure_dist) return(FALSE);

		// clip against view frustum
		
	min.x=liq->lft;
	min.y=liq->y;
	min.z=liq->top;
	max.x=liq->rgt;
	max.y=liq->y;
	max.z=liq->bot;
	
	return(view_cull_boundbox_in_frustum(&min,&max));
}

bool view_cull_model(model_draw *draw)
{
	int				obscure_dist;
	d3pnt			min,max;

		// no model

	if ((draw->model_idx==-1) || (!draw->on)) return(FALSE);

		// check obscure distance

	if (!fog_solid_on()) {
		obscure_dist=map.camera.plane.far_z-map.camera.plane.near_z;
	}
	else {
		obscure_dist=(map.fog.outer_radius>>1)*3;
	}
	
	if (map.optimize.obscure_dist.model!=0) {
		if (map.optimize.obscure_dist.model<obscure_dist) obscure_dist=map.optimize.obscure_dist.model;
	}
		
	draw->draw_dist=view_cull_distance_to_view_center(draw->pnt.x,draw->pnt.y,draw->pnt.z);
	if (draw->draw_dist>=((float)obscure_dist)) return(FALSE);

		// check bounding box
		
	model_get_view_complex_bounding_volume(server.model_list.models[draw->model_idx],&draw->pnt,&draw->setup.ang,draw->resize,&min,&max);
	return(view_cull_boundbox_in_frustum(&min,&max));
}

bool view_cull_model_shadow(model_draw *draw)
{
	int				obscure_dist,dist;
	d3pnt			min,max;

		// no model to draw

	if ((draw->model_idx==-1) || (!draw->on)) return(FALSE);

		// check obscure distance

	if (!fog_solid_on()) {
		obscure_dist=map.camera.plane.far_z-map.camera.plane.near_z;
	}
	else {
		obscure_dist=(map.fog.outer_radius>>1)*3;
	}
	
		// shadows can cull both by the shadow and model distance
		
	if (map.optimize.obscure_dist.shadow!=0) {
		if (map.optimize.obscure_dist.shadow<obscure_dist) obscure_dist=map.optimize.obscure_dist.shadow;
	}
	if (map.optimize.obscure_dist.model!=0) {
		if (map.optimize.obscure_dist.model<obscure_dist) obscure_dist=map.optimize.obscure_dist.model;
	}
		
	dist=view_cull_distance_to_view_center(draw->pnt.x,draw->pnt.y,draw->pnt.z);
	if (dist>=obscure_dist) return(FALSE);
	
		// get shadow volume

	model_get_view_complex_bounding_volume(server.model_list.models[draw->model_idx],&draw->pnt,&draw->setup.ang,draw->resize,&min,&max);
	shadow_get_volume(&draw->pnt,draw->size.y,&draw->shadow.light_pnt,draw->shadow.light_intensity,&min,&max);
	return(view_cull_boundbox_in_frustum(&min,&max));
}

bool view_cull_effect(effect_type *effect,d3pnt *center_pnt)
{
	int					obscure_dist;
	d3pnt				min,max;

		// check obscure distance

	if (!fog_solid_on()) {
		obscure_dist=map.camera.plane.far_z-map.camera.plane.near_z;
	}
	else {
		obscure_dist=(map.fog.outer_radius>>1)*3;
	}

	if (map.optimize.obscure_dist.effect!=0) {
		if (map.optimize.obscure_dist.effect<obscure_dist) obscure_dist=map.optimize.obscure_dist.effect;
	}
		
	if (view_cull_distance_to_view_center(effect->pnt.x,effect->pnt.y,effect->pnt.z)>obscure_dist) return(FALSE);
	
		// get box

	effect_draw_get_bound_box(effect,&min,&max);
	
		// remember center position for
		// distance calcs
		
	center_pnt->x=(min.x+max.x)>>1;
	center_pnt->y=(min.y+max.y)>>1;
	center_pnt->z=(min.z+max.z)>>1;
	
		// check bounds
		
	return(view_cull_boundbox_in_frustum(&min,&max));
}

bool view_cull_halo(d3pnt *pnt)
{
	int				obscure_dist;

		// check obscure distance

	if (!fog_solid_on()) {
		obscure_dist=map.camera.plane.far_z-map.camera.plane.near_z;
	}
	else {
		obscure_dist=(map.fog.outer_radius>>1)*3;
	}

	if (map.optimize.obscure_dist.effect!=0) {
		if (map.optimize.obscure_dist.effect<obscure_dist) obscure_dist=map.optimize.obscure_dist.effect;
	}
		
	return(view_cull_distance_to_view_center(pnt->x,pnt->y,pnt->z)<obscure_dist);
}

/* =======================================================

      Mesh-Poly Culling
      
======================================================= */

static inline bool view_cull_poly(map_mesh_type *mesh,map_mesh_poly_type *poly)
{
		// view culling

	if (!view_cull_boundbox_in_frustum(&poly->box.min,&poly->box.max)) return(TRUE);

		// check for never cull 
		// by normal flags

	if (mesh->flag.never_cull) return(FALSE);
	if (poly->flag.never_cull) return(FALSE);
	if (map.optimize.never_cull) return(FALSE);

		// skip polys with away facing normals
		// do dot product between normal and vector
		// from poly mid-eye point

	return(((poly->tangent_space.normal.x*(float)(poly->box.mid.x-view.render->camera.pnt.x))+(poly->tangent_space.normal.y*(float)(poly->box.mid.y-view.render->camera.pnt.y))+(poly->tangent_space.normal.z*(float)(poly->box.mid.z-view.render->camera.pnt.z)))>map.optimize.cull_angle);
}

void view_cull_draw_list_mesh_poly(void)
{
	int					n,k,mesh_idx;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	
	
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;
	
		mesh_idx=view.render->draw_list.items[n].idx;
		mesh=&map.mesh.meshes[mesh_idx];
		
		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			poly->draw.culled[view.render->cull_idx]=view_cull_poly(mesh,poly);
			poly++;
		}
	}
}

