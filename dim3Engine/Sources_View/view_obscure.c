/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Ray Trace Obscuring

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

// supergumba -- move to defs

#define view_obscure_split_div			20000
#define view_obscure_max_split			8
#define view_obscure_skip_range			50000

typedef struct		{
						int						poly_idx,trig_count,dist;
						d3fpnt					pnts[8];
						bool					skip_ray_collision;
					} view_obscure_poly_type;

typedef struct		{
						int						mesh_idx,npoly;
						bool					skip_obscured,skip_ray_collision;
						view_obscure_poly_type	*polys;
					} view_obscure_mesh_type;

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;

int							view_obscure_max_mesh_count,view_obscure_check_mesh_count;
unsigned char				*view_obscure_hits;
d3vct						*view_obscure_vcts;
view_obscure_mesh_type		*view_obscure_meshes;

/* =======================================================

      Initialize Obscuring
      
======================================================= */

bool view_obscure_initialize(void)
{
	int					n,max_poly,ray_count;
	map_mesh_type		*mesh;

		// is view obscuring on?

	view_obscure_max_mesh_count=0;
	view_obscure_check_mesh_count=0;

	view_obscure_meshes=NULL;
	view_obscure_vcts=NULL;
	view_obscure_hits=NULL;

	if (!map.optimize.ray_trace_obscure) return(TRUE);

		// count obscuring polygons

	max_poly=0;

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {

		if (mesh->precalc_flag.has_obscure_poly) {
			view_obscure_max_mesh_count++;
			if (mesh->poly_list.obscure_count>max_poly) max_poly=mesh->poly_list.obscure_count;
		}

		mesh++;
	}

	if ((view_obscure_max_mesh_count==0) || (max_poly==0)) return(TRUE);

		// memory for meshes

	view_obscure_meshes=(view_obscure_mesh_type*)malloc(view_obscure_max_mesh_count*sizeof(view_obscure_mesh_type));
	if (view_obscure_meshes==NULL) return(FALSE);

		// memory for polys

	for (n=0;n!=view_obscure_max_mesh_count;n++) {
		view_obscure_meshes[n].polys=NULL;
	}

	for (n=0;n!=view_obscure_max_mesh_count;n++) {
		view_obscure_meshes[n].polys=(view_obscure_poly_type*)malloc(max_poly*sizeof(view_obscure_poly_type));
		if (view_obscure_meshes[n].polys==NULL) {
			view_obscure_release();
			return(FALSE);
		}
	}

		// memory for rays and hits
		// rays have grid divisions on each side
		// of a cube plus one extra for the end of
		// the division, and one more for the direct
		// to mid point check

	ray_count=(((view_obscure_max_split+1)*(view_obscure_max_split+1))*6)+1;

	view_obscure_vcts=(d3vct*)malloc(ray_count*sizeof(d3vct));
	if (view_obscure_vcts==NULL) {
		view_obscure_release();
		return(FALSE);
	}

	view_obscure_hits=(unsigned char*)malloc(ray_count*sizeof(unsigned char));
	if (view_obscure_hits==NULL) {
		view_obscure_release();
		return(FALSE);
	}

	return(TRUE);
}

void view_obscure_release(void)
{
	int				n;

	if (view_obscure_meshes!=NULL) {
		for (n=0;n!=view_obscure_max_mesh_count;n++) {
			if (view_obscure_meshes[n].polys!=NULL) free(view_obscure_meshes[n].polys);
		}

		free(view_obscure_meshes);
		view_obscure_meshes=NULL;
	}

	if (view_obscure_vcts!=NULL) free(view_obscure_vcts);
	view_obscure_vcts=NULL;

	if (view_obscure_hits!=NULL) free(view_obscure_hits);
	view_obscure_hits=NULL;
}

/* =======================================================

      Build Obscuring Polygon List
      
======================================================= */

bool view_obscure_create_obscuring_poly_list(void)
{
	int						n,k,t,mesh_idx;
	short					*poly_idx;
	d3pnt					*pnt;
	view_obscure_mesh_type	*obs_mesh;
	view_obscure_poly_type	*obs_poly;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	view_obscure_check_mesh_count=0;

		// build the obscure mesh/polygons
		// to ray trace against

	obs_mesh=view_obscure_meshes;

	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

		mesh_idx=view.render->draw_list.items[n].idx;
		mesh=&map.mesh.meshes[mesh_idx];

			// had any obscure polys?

		if (!mesh->precalc_flag.has_obscure_poly) continue;

			// add the mesh

		obs_mesh->mesh_idx=mesh_idx;
		obs_mesh->skip_obscured=FALSE;
		obs_mesh->skip_ray_collision=FALSE;

		obs_mesh->npoly=0;

		obs_poly=obs_mesh->polys;
		
			// when building the poly list, descontruct
			// the polys into triangles and pre-convert
			// to float to speed this up

		poly_idx=mesh->poly_list.obscure_idxs;

		for (k=0;k!=mesh->poly_list.obscure_count;k++) {
				
			poly=&mesh->polys[poly_idx[k]];

			obs_poly->poly_idx=poly_idx[k];

			obs_poly->trig_count=poly->ptsz-2;

			for (t=0;t!=poly->ptsz;t++) {
				pnt=&mesh->vertexes[poly->v[t]];
				obs_poly->pnts[t].x=(float)pnt->x;
				obs_poly->pnts[t].y=(float)pnt->y;
				obs_poly->pnts[t].z=(float)pnt->z;
			}
		
			obs_poly->dist=view_cull_distance_to_view_center(poly->box.mid.x,poly->box.mid.y,poly->box.mid.z);
			obs_poly->skip_ray_collision=FALSE;

			obs_mesh->npoly++;

			obs_poly++;
		}

			// next mesh if we actually
			// added polys

		if (obs_mesh->npoly!=0) {
			obs_mesh++;
			view_obscure_check_mesh_count++;
		}
	}

	return(view_obscure_check_mesh_count!=0);
}

void view_obscure_remove_mesh_from_obscuring_poly_list(int mesh_idx)
{
	int						n;
	view_obscure_mesh_type	*obs_mesh;

	obs_mesh=view_obscure_meshes;

	for (n=0;n!=view_obscure_check_mesh_count;n++) {
		if (obs_mesh->mesh_idx==mesh_idx) {
			obs_mesh->skip_obscured=TRUE;
			return;
		}

		obs_mesh++;
	}
}

/* =======================================================

      Run Obscuring
      
======================================================= */

bool view_obscure_check_box(d3pnt *camera_pnt,int skip_mesh_idx,d3pnt *min,d3pnt *max,int dist)
{
	int						n,k,j,t,x,y,z,kx,ky,kz,ray_cnt,hit_cnt;
	unsigned char			*hit;
	bool					ray_hit;
	d3pnt					mid,div,div_add,ray_min,ray_max;
	d3fpnt					camera_pnt_f;
	d3vct					*vct;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	view_obscure_mesh_type	*obs_mesh;
	view_obscure_poly_type	*obs_poly;

		// if camera is inside this box, do
		// not obscure

	if ((min->x>=camera_pnt->x) && (max->x<=camera_pnt->x) && (min->y>=camera_pnt->y) && (max->y<=camera_pnt->y) && (min->z>=camera_pnt->z) && (max->z<=camera_pnt->z)) return(TRUE);

		// find the divisions
		// we do ray tracing against a grid
		// created from the mesh box

		// we always have at least 0..2 divisions
		// so we hit corners and middle

	k=max->x-min->x;
	div.x=k/view_obscure_split_div;
	if (div.x<2) div.x=2;
	if (div.x>view_obscure_max_split) div.x=view_obscure_max_split;
	div_add.x=k/div.x;

	k=max->y-min->y;
	if (k==0) {
		div.y=1;
	}
	else {
		div.y=k/view_obscure_split_div;
		if (div.y<2) div.y=2;
		if (div.y>view_obscure_max_split) div.y=view_obscure_max_split;
	}
	div_add.y=k/div.y;
	
	k=max->z-min->z;
	div.z=k/view_obscure_split_div;
	if (div.z<2) div.z=2;
	if (div.z>view_obscure_max_split) div.z=view_obscure_max_split;
	div_add.z=k/div.z;

		// get the mid point

	mid.x=(min->x+max->x)>>1;
	mid.y=(min->y+max->y)>>1;
	mid.z=(min->z+max->z)>>1;

		// build rays
		// we check against grids on a cube
		// we auto-eliminate sides of the cube
		// that aren't seen from the camera point

	ray_cnt=0;

	vct=view_obscure_vcts;

		// top

	if (camera_pnt->y<min->y) {
		for (x=0;x<=div.x;x++) {
			kx=min->x+(x*div_add.x);

			for (z=0;z<=div.z;z++) {
				kz=min->z+(z*div_add.z);

				vct->x=(float)(kx-camera_pnt->x);
				vct->y=(float)(min->y-camera_pnt->y);
				vct->z=(float)(kz-camera_pnt->z);

				vct++;
				ray_cnt++;
			}
		}
	}

		// bottom

	if (camera_pnt->y>max->y) {
		for (x=0;x<=div.x;x++) {
			kx=min->x+(x*div_add.x);

			for (z=0;z<=div.z;z++) {
				kz=min->z+(z*div_add.z);

				vct->x=(float)(kx-camera_pnt->x);
				vct->y=(float)(max->y-camera_pnt->y);
				vct->z=(float)(kz-camera_pnt->z);

				vct++;
				ray_cnt++;
			}
		}
	}

		// left

	if (camera_pnt->x<min->x) {
		for (y=0;y<=div.y;y++) {
			ky=min->y+(y*div_add.y);

			for (z=0;z<=div.z;z++) {
				kz=min->z+(z*div_add.z);

				vct->x=(float)(min->x-camera_pnt->x);
				vct->y=(float)(ky-camera_pnt->y);
				vct->z=(float)(kz-camera_pnt->z);

				vct++;
				ray_cnt++;
			}
		}
	}

		// right

	if (camera_pnt->x>max->x) {
		for (y=0;y<=div.y;y++) {
			ky=min->y+(y*div_add.y);

			for (z=0;z<=div.z;z++) {
				kz=min->z+(z*div_add.z);

				vct->x=(float)(max->x-camera_pnt->x);
				vct->y=(float)(ky-camera_pnt->y);
				vct->z=(float)(kz-camera_pnt->z);

				vct++;
				ray_cnt++;
			}
		}
	}

		// front

	if (camera_pnt->z<min->z) {
		for (x=0;x<=div.x;x++) {
			kx=min->x+(x*div_add.x);

			for (y=0;y<=div.y;y++) {
				ky=min->y+(y*div_add.y);

				vct->x=(float)(kx-camera_pnt->x);
				vct->y=(float)(ky-camera_pnt->y);
				vct->z=(float)(min->z-camera_pnt->z);

				vct++;
				ray_cnt++;
			}
		}
	}

		// back

	if (camera_pnt->z>max->z) {
		for (x=0;x<=div.x;x++) {
			kx=min->x+(x*div_add.x);

			for (y=0;y<=div.y;y++) {
				ky=min->y+(y*div_add.y);

				vct->x=(float)(kx-camera_pnt->x);
				vct->y=(float)(ky-camera_pnt->y);
				vct->z=(float)(max->z-camera_pnt->z);

				vct++;
				ray_cnt++;
			}
		}
	}

		// the midpoint ray

	vct->x=(float)(mid.x-camera_pnt->x);
	vct->y=(float)(mid.y-camera_pnt->y);
	vct->z=(float)(mid.z-camera_pnt->z);

	ray_cnt++;

		// mark no hits

	hit=view_obscure_hits;

	for (n=0;n!=ray_cnt;n++) {
		*hit++=0x0;
	}

		// get the rays min/max

	ray_min.x=ray_max.x=camera_pnt->x;
	ray_min.y=ray_max.y=camera_pnt->y;
	ray_min.z=ray_max.z=camera_pnt->z;

	if (min->x<ray_min.x) ray_min.x=min->x;
	if (max->x>ray_max.x) ray_max.x=max->x;
	if (min->y<ray_min.y) ray_min.y=min->y;
	if (max->y>ray_max.y) ray_max.y=max->y;
	if (min->z<ray_min.z) ray_min.z=min->z;
	if (max->z>ray_max.z) ray_max.z=max->z;

		// float camera

	camera_pnt_f.x=(float)camera_pnt->x;
	camera_pnt_f.y=(float)camera_pnt->y;
	camera_pnt_f.z=(float)camera_pnt->z;

		// check rays against meshes

	obs_mesh=view_obscure_meshes;

	for (n=0;n!=view_obscure_check_mesh_count;n++) {

			// skipped mesh?

		if (obs_mesh->skip_obscured) {
			obs_mesh++;
			continue;
		}

			// if we are comparing meshes, don't
			// compare against itself

		if (skip_mesh_idx==obs_mesh->mesh_idx) {
			obs_mesh++;
			continue;
		}

			// Check all the rays
			// against the mesh, and skip mesh if
			// there isn't a collision

		mesh=&map.mesh.meshes[obs_mesh->mesh_idx];

		if ((ray_max.x<mesh->box.min.x) || (ray_min.x>mesh->box.max.x) || (ray_max.y<mesh->box.min.y) || (ray_min.y>mesh->box.max.y) || (ray_max.z<mesh->box.min.z) || (ray_min.z>mesh->box.max.z)) {
			obs_mesh++;
			continue;
		}

			// run the polys

		obs_poly=obs_mesh->polys;

		for (k=0;k!=obs_mesh->npoly;k++) {

				// distance poly elimination
				
			if (obs_poly->dist>dist) {
				obs_poly++;
				continue;
			}

				// min-max poly elimination

			poly=&mesh->polys[obs_poly->poly_idx];
			
			if ((ray_max.x<poly->box.min.x) || (ray_min.x>poly->box.max.x) || (ray_max.y<poly->box.min.y) || (ray_min.y>poly->box.max.y) || (ray_max.z<poly->box.min.z) || (ray_min.z>poly->box.max.z)) {
				obs_poly++;
				continue;
			}
			
				// check the box
				// skipping checking rays that
				// all already obscured

			hit_cnt=0;

			for (j=0;j!=ray_cnt;j++) {

					// did we already hit this ray?

				if (*(view_obscure_hits+j)==0x1) {
					hit_cnt++;
					continue;
				}
			
					// run the blocking ray trace

				ray_hit=FALSE;

				for (t=0;t!=obs_poly->trig_count;t++) {
					if (ray_trace_triangle_blocking_f(&camera_pnt_f,&view_obscure_vcts[j],&obs_poly->pnts[0],&obs_poly->pnts[t+1],&obs_poly->pnts[t+2])) {
						ray_hit=TRUE;
						break;
					}
				}

					// was a hit, mark it

				if (ray_hit) {
					*(view_obscure_hits+j)=0x1;
					hit_cnt++;
				}
			}

				// are we obscured?

			if (hit_cnt==ray_cnt) return(FALSE);

			obs_poly++;
		}

		obs_mesh++;
	}

	return(TRUE);
}

void view_obscure_run(void)
{
	int						n,dist,remove_count,org_count,
							mesh_idx,liq_idx;
	d3pnt					min,max,camera_pnt;
	map_mesh_type			*mesh;
	map_liquid_type			*liq;
	obj_type				*obj;
	proj_type				*proj;
	model_type				*mdl;
	effect_type				*effect;
	
		// view obscure on?

	if (view_obscure_meshes==NULL) return;

		// build the obscure polygons
		// to ray trace against

	if (!view_obscure_create_obscuring_poly_list()) return;

		// build the camera pnt

	camera_pnt.x=view.render->camera.pnt.x;
	camera_pnt.y=view.render->camera.pnt.y;
	camera_pnt.z=view.render->camera.pnt.z+map.camera.plane.near_z;

		// run through and obscure all
		// meshes, liquids, models, and effects

	remove_count=0;
	org_count=view.render->draw_list.count;

	for (n=0;n!=view.render->draw_list.count;n++) {

			// if too close, don't obscure

		dist=view.render->draw_list.items[n].dist;
		if (dist<view_obscure_skip_range) continue;

			// check for removal

		switch (view.render->draw_list.items[n].type) {

				// mesh
				// if we remove a mesh, remove any of it's
				// polys from the removal list as they aren't
				// blocking anymore

			case view_render_type_mesh:
				mesh_idx=view.render->draw_list.items[n].idx;
				mesh=&map.mesh.meshes[mesh_idx];
				if (mesh->flag.never_obscure) break;

				if (!view_obscure_check_box(&camera_pnt,mesh_idx,&mesh->box.min,&mesh->box.max,dist)) {
					view.render->draw_list.items[n].type=view_render_type_none;
					view_obscure_remove_mesh_from_obscuring_poly_list(mesh_idx);
					remove_count++;
					view.count.mesh--;
				}
				break;
				
				// liquids
				
			case view_render_type_liquid:
				liq_idx=view.render->draw_list.items[n].idx;
				liq=&map.liquid.liquids[liq_idx];
				if (liq->flag.never_obscure) break;
				
				min.x=liq->lft;
				max.x=liq->rgt;
				min.y=max.y=liq->y;
				min.z=liq->top;
				max.z=liq->bot;

				if (!view_obscure_check_box(&camera_pnt,-1,&min,&max,dist)) {
					view.render->draw_list.items[n].type=view_render_type_none;
					remove_count++;
					view.count.liquid--;
				}
				break;

				// model

			case view_render_type_object:
				obj=server.obj_list.objs[view.render->draw_list.items[n].idx];
				if ((obj->draw.model_idx==-1) || (!obj->draw.on)) break;
	
				mdl=server.model_list.models[obj->draw.model_idx];
				model_get_view_complex_bounding_volume(mdl,&obj->draw.pnt,&obj->ang,obj->draw.resize,&min,&max);

				if (!view_obscure_check_box(&camera_pnt,-1,&min,&max,dist)) {
					view.render->draw_list.items[n].type=view_render_type_none;
					remove_count++;
					view.count.model--;
				}
				break;

			case view_render_type_projectile:
				proj=server.proj_list.projs[view.render->draw_list.items[n].idx];
				if ((proj->draw.model_idx==-1) || (!proj->draw.on)) break;
	
				mdl=server.model_list.models[proj->draw.model_idx];
				model_get_view_complex_bounding_volume(mdl,&proj->draw.pnt,&proj->ang,proj->draw.resize,&min,&max);

				if (!view_obscure_check_box(&camera_pnt,-1,&min,&max,dist)) {
					view.render->draw_list.items[n].type=view_render_type_none;
					remove_count++;
					view.count.model--;
				}
				break;

				// effects

			case view_render_type_effect:
				effect=server.effect_list.effects[view.render->draw_list.items[n].idx];
				effect_draw_get_bound_box(effect,&min,&max);

				if (!view_obscure_check_box(&camera_pnt,-1,&min,&max,dist)) {
					view.render->draw_list.items[n].type=view_render_type_none;
					remove_count++;
					view.count.effect--;
				}
				break;

		}
	}

	view.count.obscure_percent=(100*remove_count)/org_count;
}

