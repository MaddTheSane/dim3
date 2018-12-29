//
// dim3RTL
// by Brian Barnes (bbarnes@klinksoftware.com)
// free software offered as is for any use
//

#include "ray_os_defs.h"
#include "ray.h"
#include "ray_interface.h"

ray_global_type					ray_global;

/* =======================================================

      Precalculate Mesh Bounds
      
======================================================= */

void ray_precalc_mesh_bounds(ray_mesh_type *mesh)
{
	int					n;
	ray_point_type		*pnt;
	ray_bound_type		*bnd;
	
	bnd=&mesh->bound;
	
	pnt=mesh->vertex_block.vertexes;

	bnd->min.x=bnd->max.x=pnt->x;
	bnd->min.y=bnd->max.y=pnt->y;
	bnd->min.z=bnd->max.z=pnt->z;
	
	pnt++;
	
	for (n=1;n<mesh->vertex_block.count;n++) {
		if (pnt->x<bnd->min.x) bnd->min.x=pnt->x;
		if (pnt->x>bnd->max.x) bnd->max.x=pnt->x;

		if (pnt->y<bnd->min.y) bnd->min.y=pnt->y;
		if (pnt->y>bnd->max.y) bnd->max.y=pnt->y;

		if (pnt->z<bnd->min.z) bnd->min.z=pnt->z;
		if (pnt->z>bnd->max.z) bnd->max.z=pnt->z;
		
		pnt++;
	}

	bnd->mid.x=(bnd->min.x+bnd->max.x)*0.5f;
	bnd->mid.y=(bnd->min.y+bnd->max.y)*0.5f;
	bnd->mid.z=(bnd->min.z+bnd->max.z)*0.5f;
}

/* =======================================================

      Precalculate Polygon Bounds and Normals
      
======================================================= */

void ray_precalc_polygon_bounds(ray_mesh_type *mesh,ray_poly_type *poly)
{
	int					n;
	ray_point_type		*pnt;
	ray_bound_type		*bnd;
	
	bnd=&poly->bound;
	
	pnt=&mesh->vertex_block.vertexes[poly->idxs[0].vertex];

	bnd->min.x=bnd->max.x=pnt->x;
	bnd->min.y=bnd->max.y=pnt->y;
	bnd->min.z=bnd->max.z=pnt->z;
	
	pnt++;
	
	for (n=1;n<poly->nvertex;n++) {
		pnt=&mesh->vertex_block.vertexes[poly->idxs[n].vertex];
		
		if (pnt->x<bnd->min.x) bnd->min.x=pnt->x;
		if (pnt->x>bnd->max.x) bnd->max.x=pnt->x;

		if (pnt->y<bnd->min.y) bnd->min.y=pnt->y;
		if (pnt->y>bnd->max.y) bnd->max.y=pnt->y;

		if (pnt->z<bnd->min.z) bnd->min.z=pnt->z;
		if (pnt->z>bnd->max.z) bnd->max.z=pnt->z;
		
		pnt++;
	}

		// calc the mid

	bnd->mid.x=(bnd->min.x+bnd->max.x)*0.5f;
	bnd->mid.y=(bnd->min.y+bnd->max.y)*0.5f;
	bnd->mid.z=(bnd->min.z+bnd->max.z)*0.5f;
}

void ray_precalc_polygon_normal(ray_mesh_type *mesh,ray_poly_type *poly)
{
	int					n;
	float				f;
	ray_vector_type		*normal;
	
	poly->surface_normal.x=0.0f;
	poly->surface_normal.y=0.0f;
	poly->surface_normal.z=0.0f;
	
	if (mesh->normal_block.count==0) return;
	
	for (n=0;n!=poly->nvertex;n++) {
		normal=&mesh->normal_block.normals[poly->idxs[n].normal];
		poly->surface_normal.x+=normal->x;
		poly->surface_normal.y+=normal->y;
		poly->surface_normal.z+=normal->z;
	}
	
	f=(float)poly->nvertex;
	poly->surface_normal.x/=f;
	poly->surface_normal.y/=f;
	poly->surface_normal.z/=f;
	
	ray_vector_normalize(&poly->surface_normal);
}

void ray_precalc_polygon_plane(ray_mesh_type *mesh,ray_poly_type *poly)
{
	ray_point_type			*pnt;
	
		// plane equation is just the normal
		// plus a distance calc
		
	poly->plane.a=poly->surface_normal.x;
	poly->plane.b=poly->surface_normal.y;
	poly->plane.c=poly->surface_normal.z;
	
	pnt=&mesh->vertex_block.vertexes[poly->idxs[0].vertex];
	poly->plane.d=-((poly->plane.a*pnt->x)+(poly->plane.b*pnt->y)+(poly->plane.c*pnt->z));
}

/* =======================================================

      Precalculate Light Bounds
      
======================================================= */

void ray_precalc_light_bounds(ray_light_type *light)
{
	light->bound.min.x=light->pnt.x-light->intensity;
	light->bound.min.y=light->pnt.y-light->intensity;
	light->bound.min.z=light->pnt.z-light->intensity;
	
	light->bound.max.x=light->pnt.x+light->intensity;
	light->bound.max.y=light->pnt.y+light->intensity;
	light->bound.max.z=light->pnt.z+light->intensity;
	
	light->bound.mid.x=(light->bound.min.x+light->bound.max.x)*0.5f;
	light->bound.mid.y=(light->bound.min.y+light->bound.max.y)*0.5f;
	light->bound.mid.z=(light->bound.min.z+light->bound.max.z)*0.5f;
}

/* =======================================================

      Precalculate Triangle Vectors
      
======================================================= */

void ray_precalc_triangle_vectors(ray_mesh_type *mesh,ray_trig_type *trig)
{
	ray_point_type		*p0,*p1,*p2;
	
		// get the points
		
	p0=&mesh->vertex_block.vertexes[trig->idxs[0].vertex];
	p1=&mesh->vertex_block.vertexes[trig->idxs[1].vertex];
	p2=&mesh->vertex_block.vertexes[trig->idxs[2].vertex];

		// precalc the vectors sharing
		// triangle around vertex 0
		
	ray_vector_create_from_points(&trig->v1,p1,p0);
	ray_vector_create_from_points(&trig->v2,p2,p0);
}

/* =======================================================

      Frustum Culling
      
======================================================= */

void ray_precalc_build_frustum_plane_single(ray_plane_type *plane,ray_point_type *p0,ray_point_type *p1,ray_point_type *p2)
{
	ray_vector_type			v,u,normal;
	
		// get the normal
		
	ray_vector_create_from_points(&v,p1,p0);
	ray_vector_create_from_points(&u,p2,p0);
	
	ray_vector_cross_product(&normal,&v,&u);
	ray_vector_normalize(&normal);

		// get the a,b,c

	plane->a=normal.x;
	plane->b=normal.y;
	plane->c=normal.z;

		// calculate d

	plane->d=-((plane->a*p0->x)+(plane->b*p0->y)+(plane->c*p0->z));
}

void ray_precalc_build_frustum_planes(ray_scene_type *scene,ray_scene_slice_type *slice,ray_plane_type *planes)
{
	float						lx,rx,ty,by,wid,high;
	ray_point_type				*eye_point;
	ray_point_type				view_plane_point,
								top_lft_near_pnt,top_rgt_near_pnt,
								bot_lft_near_pnt,bot_rgt_near_pnt,
								top_lft_far_pnt,top_rgt_far_pnt,
								bot_lft_far_pnt,bot_rgt_far_pnt;
	ray_vector_type				center_vct,
								top_lft_vct,top_rgt_vct,
								bot_lft_vct,bot_rgt_vct;
	
		// projection plane size

	lx=(float)slice->pixel_start.x;
	rx=(float)slice->pixel_end.x;
	ty=(float)slice->pixel_start.y;
	by=(float)slice->pixel_end.y;

	wid=(float)(scene->buffer.wid>>1);
	high=(float)(scene->buffer.high>>1);
		
		// the the four vectors that
		// pass through the four corners of
		// the near and far plane
		
	eye_point=&scene->eye.pnt;

	view_plane_point.z=eye_point->z+scene->eye.plane_dist;

	view_plane_point.x=eye_point->x;
	view_plane_point.y=eye_point->y;
	ray_vector_create_from_points(&center_vct,&view_plane_point,eye_point);
	rtlMatrixVectorMultiply(&scene->eye.matrix,&center_vct);
	ray_vector_normalize(&center_vct);

	view_plane_point.x=(eye_point->x-wid)+lx;
	view_plane_point.y=(eye_point->y-high)+ty;
	ray_vector_create_from_points(&top_lft_vct,&view_plane_point,eye_point);
	rtlMatrixVectorMultiply(&scene->eye.matrix,&top_lft_vct);
	ray_vector_normalize(&top_lft_vct);

	view_plane_point.x=(eye_point->x-wid)+lx;
	view_plane_point.y=(eye_point->y-high)+by;
	ray_vector_create_from_points(&bot_lft_vct,&view_plane_point,eye_point);
	rtlMatrixVectorMultiply(&scene->eye.matrix,&bot_lft_vct);
	ray_vector_normalize(&bot_lft_vct);

	view_plane_point.x=(eye_point->x-wid)+rx;
	view_plane_point.y=(eye_point->y-high)+ty;
	ray_vector_create_from_points(&top_rgt_vct,&view_plane_point,eye_point);
	rtlMatrixVectorMultiply(&scene->eye.matrix,&top_rgt_vct);
	ray_vector_normalize(&top_rgt_vct);

	view_plane_point.x=(eye_point->x-wid)+rx;
	view_plane_point.y=(eye_point->y-high)+by;
	ray_vector_create_from_points(&bot_rgt_vct,&view_plane_point,eye_point);
	rtlMatrixVectorMultiply(&scene->eye.matrix,&bot_rgt_vct);
	ray_vector_normalize(&bot_rgt_vct);
	
		// get the 8 plane points

	top_lft_near_pnt.x=eye_point->x+(top_lft_vct.x*scene->eye.plane_dist);
	top_lft_near_pnt.y=eye_point->y+(top_lft_vct.y*scene->eye.plane_dist);
	top_lft_near_pnt.z=eye_point->z+(top_lft_vct.z*scene->eye.plane_dist);
		
	top_lft_far_pnt.x=eye_point->x+(top_lft_vct.x*scene->eye.max_dist);
	top_lft_far_pnt.y=eye_point->y+(top_lft_vct.y*scene->eye.max_dist);
	top_lft_far_pnt.z=eye_point->z+(top_lft_vct.z*scene->eye.max_dist);
	
	top_rgt_near_pnt.x=eye_point->x+(top_rgt_vct.x*scene->eye.plane_dist);
	top_rgt_near_pnt.y=eye_point->y+(top_rgt_vct.y*scene->eye.plane_dist);
	top_rgt_near_pnt.z=eye_point->z+(top_rgt_vct.z*scene->eye.plane_dist);
		
	top_rgt_far_pnt.x=eye_point->x+(top_rgt_vct.x*scene->eye.max_dist);
	top_rgt_far_pnt.y=eye_point->y+(top_rgt_vct.y*scene->eye.max_dist);
	top_rgt_far_pnt.z=eye_point->z+(top_rgt_vct.z*scene->eye.max_dist);
	
	bot_lft_near_pnt.x=eye_point->x+(bot_lft_vct.x*scene->eye.plane_dist);
	bot_lft_near_pnt.y=eye_point->y+(bot_lft_vct.y*scene->eye.plane_dist);
	bot_lft_near_pnt.z=eye_point->z+(bot_lft_vct.z*scene->eye.plane_dist);
		
	bot_lft_far_pnt.x=eye_point->x+(bot_lft_vct.x*scene->eye.max_dist);
	bot_lft_far_pnt.y=eye_point->y+(bot_lft_vct.y*scene->eye.max_dist);
	bot_lft_far_pnt.z=eye_point->z+(bot_lft_vct.z*scene->eye.max_dist);
	
	bot_rgt_near_pnt.x=eye_point->x+(bot_rgt_vct.x*scene->eye.plane_dist);
	bot_rgt_near_pnt.y=eye_point->y+(bot_rgt_vct.y*scene->eye.plane_dist);
	bot_rgt_near_pnt.z=eye_point->z+(bot_rgt_vct.z*scene->eye.plane_dist);
		
	bot_rgt_far_pnt.x=eye_point->x+(bot_rgt_vct.x*scene->eye.max_dist);
	bot_rgt_far_pnt.y=eye_point->y+(bot_rgt_vct.y*scene->eye.max_dist);
	bot_rgt_far_pnt.z=eye_point->z+(bot_rgt_vct.z*scene->eye.max_dist);
	
		// get the 6 frustum planes
		// left, right, top, bottom, near, and far
		
	ray_precalc_build_frustum_plane_single(&planes[0],&bot_lft_near_pnt,&top_lft_near_pnt,&bot_lft_far_pnt);
	ray_precalc_build_frustum_plane_single(&planes[1],&top_rgt_near_pnt,&bot_rgt_near_pnt,&top_rgt_far_pnt);
	ray_precalc_build_frustum_plane_single(&planes[2],&top_lft_near_pnt,&top_rgt_near_pnt,&top_lft_far_pnt);
	ray_precalc_build_frustum_plane_single(&planes[3],&bot_rgt_near_pnt,&bot_lft_near_pnt,&bot_rgt_far_pnt);
	ray_precalc_build_frustum_plane_single(&planes[4],&top_lft_near_pnt,&bot_lft_near_pnt,&top_rgt_near_pnt);
	ray_precalc_build_frustum_plane_single(&planes[5],&bot_lft_far_pnt,&top_lft_far_pnt,&bot_rgt_far_pnt);
}

bool ray_precalc_frustum_plane_bound_cull(ray_plane_type *planes,ray_bound_type *bnd)
{
	int				n;

	for (n=0;n!=6;n++) {
		if (((planes[n].a*bnd->min.x)+(planes[n].b*bnd->min.y)+(planes[n].c*bnd->min.z)+planes[n].d)>0.0f) continue;
		if (((planes[n].a*bnd->max.x)+(planes[n].b*bnd->min.y)+(planes[n].c*bnd->min.z)+planes[n].d)>0.0f) continue;
		if (((planes[n].a*bnd->min.x)+(planes[n].b*bnd->max.y)+(planes[n].c*bnd->min.z)+planes[n].d)>0.0f) continue;
		if (((planes[n].a*bnd->max.x)+(planes[n].b*bnd->max.y)+(planes[n].c*bnd->min.z)+planes[n].d)>0.0f) continue;
		if (((planes[n].a*bnd->min.x)+(planes[n].b*bnd->min.y)+(planes[n].c*bnd->max.z)+planes[n].d)>0.0f) continue;
		if (((planes[n].a*bnd->max.x)+(planes[n].b*bnd->min.y)+(planes[n].c*bnd->max.z)+planes[n].d)>0.0f) continue;
		if (((planes[n].a*bnd->min.x)+(planes[n].b*bnd->max.y)+(planes[n].c*bnd->max.z)+planes[n].d)>0.0f) continue;
		if (((planes[n].a*bnd->max.x)+(planes[n].b*bnd->max.y)+(planes[n].c*bnd->max.z)+planes[n].d)>0.0f) continue;
		return(FALSE);
	}

	return(TRUE);
}

/* =======================================================

      Normal Culling
      
======================================================= */

bool ray_precalc_eye_normal_cull(ray_scene_type *scene,ray_scene_slice_type *slice,ray_mesh_type *mesh,ray_poly_type *poly)
{
	float					wid,high;
	ray_point_type			pnt;
	ray_vector_type			eye_vct;
	
		// skip meshes without normals
		
	if (mesh->normal_block.count==0) return(TRUE);
	
		// get midpoint of thread box
		
	wid=(float)(scene->buffer.wid>>1);
	high=(float)(scene->buffer.high>>1);

	pnt.x=(scene->eye.pnt.x-wid)+((slice->pixel_start.x+slice->pixel_start.x)>>1);
	pnt.y=(scene->eye.pnt.y-high)+((slice->pixel_start.y+slice->pixel_start.y)>>1);
	pnt.z=scene->eye.pnt.z+scene->eye.plane_dist;
	
	eye_vct.x=poly->bound.mid.x-pnt.x;
	eye_vct.y=poly->bound.mid.y-pnt.y;
	eye_vct.z=poly->bound.mid.z-pnt.z;

		// run the dot product against
		// the poly normal
		
	return(((poly->surface_normal.x*eye_vct.x)+(poly->surface_normal.y*eye_vct.y)+(poly->surface_normal.z*eye_vct.z))<=0.0f);
}

bool ray_precalc_light_normal_cull(ray_scene_type *scene,ray_light_type *light,ray_mesh_type *mesh,ray_poly_type *poly)
{
	ray_vector_type			light_vct;
	
		// skip meshes without normals
		
	if (mesh->normal_block.count==0) return(TRUE);
	
		// get the vector
		
	light_vct.x=poly->bound.mid.x-light->pnt.x;
	light_vct.y=poly->bound.mid.y-light->pnt.y;
	light_vct.z=poly->bound.mid.z-light->pnt.z;

		// run the dot product against
		// the poly normal
		
	return(((poly->surface_normal.x*light_vct.x)+(poly->surface_normal.y*light_vct.y)+(poly->surface_normal.z*light_vct.z))<=0.0f);
}

/* =======================================================

      Precalc For Scene Rendering

	  Mesh in scene list
	  Includes a mesh/light cross collision list
	  Mipmap poly cache
      
======================================================= */

void ray_precalc_render_scene_setup(ray_scene_type *scene)
{
	int							n,k,t,mesh_idx,mesh_2_idx,light_idx,
								list_poly_count_idx,list_idx,list_2_idx,
								poly_count;
	float						d;
	double						dx,dy,dz;
	ray_mesh_type				*mesh,*mesh2;
	ray_poly_type				*poly;
	ray_light_type				*light;
	ray_mesh_poly_pack_list		*pack_list;

		// no slices used yet

	scene->render.next_slice_idx=0;

		// create a list of meshes within
		// the eye max_dist, centered around
		// the eye point.  These are the drawing
		// view meshes

		// this uses the regular packed lists
		// but it'll only be the meshes, this list
		// is (currently) assumed to be all polys
		
	scene->render.view_mesh_pack_list.idx=0;

	for (n=0;n!=scene->mesh_list.count;n++) {

		mesh=scene->mesh_list.meshes[n];
		if (mesh->hidden) continue;

		dx=(mesh->bound.mid.x-scene->eye.pnt.x);
		dy=(mesh->bound.mid.y-scene->eye.pnt.y);
		dz=(mesh->bound.mid.z-scene->eye.pnt.z);

		d=(float)sqrt((dx*dx)+(dy*dy)+(dz*dz));
		if (d>scene->eye.max_dist) continue;

			// add mesh
		
		if (scene->render.view_mesh_pack_list.idx<(ray_max_mesh_pack_list-2)) {
			scene->render.view_mesh_pack_list.list[scene->render.view_mesh_pack_list.idx++]=n;
		}

			// setup polys in the scene

		poly=mesh->poly_block.polys;
		
		for (k=0;k!=mesh->poly_block.count;k++) {
			poly->mm_level=-1;
			poly++;
		}
	}

		// init some caches
	
	for (n=0;n!=scene->mesh_list.count;n++) {
		scene->mesh_list.meshes[n]->light_collide.count=0;
		for (k=0;k!=ray_max_light_per_mesh;k++) {
			scene->mesh_list.meshes[n]->light_collide.lights[k].mesh_poly_pack_block_list.idx=0;
		}
	}

	for (n=0;n!=scene->light_list.count;n++) {
		scene->light_list.lights[n]->mesh_poly_pack_collide_list.idx=0;
	}

		// here we build two lists, one for each
		// mesh which shows what lights fall on them,
		// the other for all the meshes and polys
		// that can be seen from each light, we will
		// later use this list to create a cross list
		// of what meshes block other meshes from
		// tracing to a light

		// this list is pared down by light radius
		// and normal culling

	list_idx=0;

	while (list_idx<scene->render.view_mesh_pack_list.idx) {

		mesh_idx=scene->render.view_mesh_pack_list.list[list_idx++];

		mesh=scene->mesh_list.meshes[mesh_idx];

		for (k=0;k!=scene->light_list.count;k++) {
			light=scene->light_list.lights[k];
			if (light->hidden) continue;

			if (!ray_bound_bound_collision(&mesh->bound,&light->bound)) continue;

				// add this light to the mesh list so the
				// mesh knows only the lights that hit it

			if (mesh->light_collide.count<ray_max_light_per_mesh) {
				mesh->light_collide.lights[mesh->light_collide.count].idx=k;
				mesh->light_collide.count++;
			}

				// skip adding any meshes to the light
				// collision list that don't block light

			if ((mesh->flags&RL_MESH_FLAG_NON_LIGHT_TRACE_BLOCKING)!=0x0) continue;

				// determine the polys the light
				// can hit by bounding and normal culling

			list_poly_count_idx=-1;

			poly=mesh->poly_block.polys;

			for (t=0;t!=mesh->poly_block.count;t++) {
				if (ray_bound_bound_collision(&poly->bound,&light->bound)) {
					if (ray_precalc_light_normal_cull(scene,light,mesh,poly)) {

						if (light->mesh_poly_pack_collide_list.idx<(ray_max_mesh_poly_pack_list-4)) {

							if (list_poly_count_idx==-1) {
								light->mesh_poly_pack_collide_list.list[light->mesh_poly_pack_collide_list.idx++]=mesh_idx;
								list_poly_count_idx=light->mesh_poly_pack_collide_list.idx;
								light->mesh_poly_pack_collide_list.list[light->mesh_poly_pack_collide_list.idx++]=0;
							}

							light->mesh_poly_pack_collide_list.list[light->mesh_poly_pack_collide_list.idx++]=t;
							light->mesh_poly_pack_collide_list.list[list_poly_count_idx]++;
						}
					}
				}

				poly++;
			}
		}
	}

		// now build a list of which meshes are possible
		// light blockers for every mesh-light combo

		// we use the lists created above to speed up
		// this process

	list_idx=0;

	while (list_idx<scene->render.view_mesh_pack_list.idx) {

		mesh_idx=scene->render.view_mesh_pack_list.list[list_idx++];
		mesh=scene->mesh_list.meshes[mesh_idx];

		for (k=0;k!=mesh->light_collide.count;k++) {

			light_idx=mesh->light_collide.lights[k].idx;
			light=scene->light_list.lights[light_idx];

				// now run through the meshes and find
				// every mesh that is between this light
				// and this mesh, these are the possible blockers

			list_2_idx=0;

			while (list_2_idx<light->mesh_poly_pack_collide_list.idx) {

				mesh_2_idx=light->mesh_poly_pack_collide_list.list[list_2_idx++];
				poly_count=light->mesh_poly_pack_collide_list.list[list_2_idx++];

				mesh2=scene->mesh_list.meshes[mesh_2_idx];

					// skip any meshes that aren't between
					// light and mesh

				if (mesh_2_idx!=mesh_idx) {
					if (mesh->bound.min.x>light->pnt.x) {
						if (mesh2->bound.max.x<light->pnt.x) {
							list_2_idx+=poly_count;
							continue;
						}
					}
					if (mesh->bound.max.x<light->pnt.x) {
						if (mesh2->bound.min.x>light->pnt.x) {
							list_2_idx+=poly_count;
							continue;
						}
					}
					if (mesh->bound.min.y>light->pnt.y) {
						if (mesh2->bound.max.y<light->pnt.y) {
							list_2_idx+=poly_count;
							continue;
						}
					}
					if (mesh->bound.max.y<light->pnt.y) {
						if (mesh2->bound.min.y>light->pnt.y) {
							list_2_idx+=poly_count;
							continue;
						}
					}
					if (mesh->bound.min.z>light->pnt.z) {
						if (mesh2->bound.max.z<light->pnt.z) {
							list_2_idx+=poly_count;
							continue;
						}
					}
					if (mesh->bound.max.z<light->pnt.z) {
						if (mesh2->bound.min.z>light->pnt.z) {
							list_2_idx+=poly_count;
							continue;
						}
					}
				}

					// add this mesh and all it's polys
					// to the mesh-light blocking list

				pack_list=&mesh->light_collide.lights[k].mesh_poly_pack_block_list;

				pack_list->list[pack_list->idx++]=mesh_2_idx;
				pack_list->list[pack_list->idx++]=poly_count;
						
				for (t=0;t!=poly_count;t++) {
					pack_list->list[pack_list->idx++]=light->mesh_poly_pack_collide_list.list[list_2_idx++];
				}
			}
		}
	}
}

/* =======================================================

      Precalc For Thread Mesh Rendering

	  This includes a smaller mesh list for the
	  initial meshes available for collisions in
	  the thread's pixel block
	  
	  If the ray bounces, we retreat to the
	  meshes in the larger scene list
      
======================================================= */

void ray_precalc_render_scene_slice_setup(ray_scene_type *scene,ray_scene_slice_type *slice)
{
	int							n,k,mesh_idx,
								list_idx,list_poly_count_idx;
	ray_mesh_type				*mesh;
	ray_poly_type				*poly;
	ray_plane_type				planes[6];

		// build the frustum planes
		// for this chunk of rendering

	ray_precalc_build_frustum_planes(scene,slice,planes);

		// build the mesh list for this slice
		// from the view mesh list
		// we use this one for first hits, but then
		// retreat to the main list for bounces

	slice->mesh_pack_list.idx=0;
	
	list_idx=0;

	while (list_idx<scene->render.view_mesh_pack_list.idx) {

		mesh_idx=scene->render.view_mesh_pack_list.list[list_idx++];
		mesh=scene->mesh_list.meshes[mesh_idx];
		
			// special knock-out flags
			
		if ((mesh->hidden) || ((mesh->flags&RL_MESH_FLAG_NON_RAY_TRACE_BLOCKING)!=0)) continue;

			// bound collisions

		if (!ray_precalc_frustum_plane_bound_cull(planes,&mesh->bound)) continue;

			// start running through the polygons
			// we do this before adding the mesh as
			// we want to see if we hit any polygons at all

		list_poly_count_idx=-1;

			// set up the polys that can be seen from the
			// frustum for this slice and aren't
			// culled by the poly normal
			// these are the candidates for ray trace collisions

		poly=mesh->poly_block.polys;

		for (k=0;k!=mesh->poly_block.count;k++) {
		
			if (ray_precalc_frustum_plane_bound_cull(planes,&poly->bound)) {
				if (ray_precalc_eye_normal_cull(scene,slice,mesh,poly)) {

					if (slice->mesh_pack_list.idx<(ray_max_mesh_poly_pack_list-4)) {

						if (list_poly_count_idx==-1) {
							slice->mesh_pack_list.list[slice->mesh_pack_list.idx++]=mesh_idx;
							list_poly_count_idx=slice->mesh_pack_list.idx;
							slice->mesh_pack_list.list[slice->mesh_pack_list.idx++]=0;
						}

						slice->mesh_pack_list.list[slice->mesh_pack_list.idx++]=k;
						slice->mesh_pack_list.list[list_poly_count_idx]++;
					}
				}
			}
			
			poly++;
		}
	}
	
		// reset the likely blocking poly list

	for (n=0;n!=ray_max_scene_light;n++) {
		slice->likely_block[n].mesh_idx=-1;
	}
}

