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
#include "ray_interface.h"

extern map_type			map;
extern setup_type		setup;
extern view_type		view;

extern int				view_rtl_draw_scene_id;

int						rtl_sky_mesh_id,sky_vertex_count;

/* =======================================================

      Sky Cube Mesh
      
======================================================= */

void sky_init(void)
{
	int				sky_vertex_count;

		// is there a sky?
		// only supporting cube type skies
	
	rtl_sky_mesh_id=-1;

	if (!map.sky.on) return;
	if (map.sky.type!=st_cube) return;

		// get vertex counts

	sky_vertex_count=0;

	if (map.sky.fill!=-1) sky_vertex_count+=4;
	if (map.sky.bottom_fill!=-1) sky_vertex_count+=4;
	if (map.sky.north_fill!=-1) sky_vertex_count+=4;
	if (map.sky.south_fill!=-1) sky_vertex_count+=4;
	if (map.sky.east_fill!=-1) sky_vertex_count+=4;
	if (map.sky.west_fill!=-1) sky_vertex_count+=4;

	if (sky_vertex_count==0) return;

		// create mesh
		// sky moves with player, so we just
		// setup mesh here and update on the fly
		
	rtl_sky_mesh_id=rtlSceneMeshAdd(view_rtl_draw_scene_id,RL_MESH_FLAG_HIGHLIGHT);
	if (rtl_sky_mesh_id==-1) return;

	rtlSceneMeshSetVertex(view_rtl_draw_scene_id,rtl_sky_mesh_id,RL_MESH_FORMAT_VERTEX_3_FLOAT,sky_vertex_count,NULL);
	rtlSceneMeshSetUV(view_rtl_draw_scene_id,rtl_sky_mesh_id,RL_MESH_FORMAT_UV_2_FLOAT,sky_vertex_count,NULL);
}

void sky_release(void)
{
	if (rtl_sky_mesh_id!=-1) rtlSceneMeshDelete(view_rtl_draw_scene_id,rtl_sky_mesh_id);
}

void sky_update(void)
{
	int				k,idx,poly_count;
	short			polys[6*10];
	short			*vk;
    float			f_radius,txt_fact;
	float			*vp,*uv;
	d3fpnt			camera_pos;
	
	if (rtl_sky_mesh_id==-1) return;

		// setup cube quads
		
	f_radius=(float)map.sky.radius;
	camera_pos.x=(float)view.render->camera.pnt.x;
	camera_pos.y=(float)view.render->camera.pnt.y;
	camera_pos.z=(float)view.render->camera.pnt.z;

	txt_fact=map.sky.txt_fact;

		// run polys

	idx=0;
	poly_count=0;

	vk=polys;

		// top
		
	rtlSceneMeshMapVertexPointer(view_rtl_draw_scene_id,rtl_sky_mesh_id,(void**)&vp);
	rtlSceneMeshMapUVPointer(view_rtl_draw_scene_id,rtl_sky_mesh_id,(void**)&uv);

	if (map.sky.fill!=-1) {

			// vertexes and uv

		*vp++=camera_pos.x-f_radius;
		*vp++=camera_pos.y-f_radius;
		*vp++=camera_pos.z+f_radius;

		*uv++=0.0f;
		*uv++=0.0f;

		*vp++=camera_pos.x+f_radius;
		*vp++=camera_pos.y-f_radius;
		*vp++=camera_pos.z+f_radius;

		*uv++=txt_fact;
		*uv++=0.0f;

		*vp++=camera_pos.x+f_radius;
		*vp++=camera_pos.y-f_radius;
		*vp++=camera_pos.z-f_radius;

		*uv++=txt_fact;
		*uv++=txt_fact;

		*vp++=camera_pos.x-f_radius;
		*vp++=camera_pos.y-f_radius;
		*vp++=camera_pos.z-f_radius;

		*uv++=0.0f;
		*uv++=txt_fact;

			// polygon

		*vk++=4;
		*vk++=(short)map.textures[map.sky.fill].frames[0].bitmap.rl_material_id;
		for (k=0;k!=4;k++) {
			*vk++=(short)idx;
			*vk++=(short)idx;
			idx++;
		}

		poly_count++;
	}
	
		// bottom
		
	if (map.sky.bottom_fill!=-1) {

			// vertexes and uv

		*vp++=camera_pos.x-f_radius;
		*vp++=camera_pos.y+f_radius;
		*vp++=camera_pos.z+f_radius;

		*uv++=0.0f;
		*uv++=0.0f;

		*vp++=camera_pos.x+f_radius;
		*vp++=camera_pos.y+f_radius;
		*vp++=camera_pos.z+f_radius;

		*uv++=txt_fact;
		*uv++=0.0f;

		*vp++=camera_pos.x+f_radius;
		*vp++=camera_pos.y+f_radius;
		*vp++=camera_pos.z-f_radius;

		*uv++=txt_fact;
		*uv++=txt_fact;

		*vp++=camera_pos.x-f_radius;
		*vp++=camera_pos.y+f_radius;
		*vp++=camera_pos.z-f_radius;

		*uv++=0.0f;
		*uv++=txt_fact;

			// polygon

		*vk++=4;
		*vk++=(short)map.textures[map.sky.bottom_fill].frames[0].bitmap.rl_material_id;
		for (k=0;k!=4;k++) {
			*vk++=(short)idx;
			*vk++=(short)idx;
			idx++;
		}

		poly_count++;
	}
	
		// north
		
	if (map.sky.north_fill!=-1) {

			// vertexes and uv

		*vp++=camera_pos.x-f_radius;
		*vp++=camera_pos.y-f_radius;
		*vp++=camera_pos.z+f_radius;

		*uv++=0.00f;
		*uv++=0.00f;

		*vp++=camera_pos.x+f_radius;
		*vp++=camera_pos.y-f_radius;
		*vp++=camera_pos.z+f_radius;

		*uv++=txt_fact;
		*uv++=0.00f;

		*vp++=camera_pos.x+f_radius;
		*vp++=camera_pos.y+f_radius;
		*vp++=camera_pos.z+f_radius;

		*uv++=txt_fact;
		*uv++=txt_fact;

		*vp++=camera_pos.x-f_radius;
		*vp++=camera_pos.y+f_radius;
		*vp++=camera_pos.z+f_radius;

		*uv++=0.00f;
		*uv++=txt_fact;

			// polygon

		*vk++=4;
		*vk++=(short)map.textures[map.sky.north_fill].frames[0].bitmap.rl_material_id;
		for (k=0;k!=4;k++) {
			*vk++=(short)idx;
			*vk++=(short)idx;
			idx++;
		}

		poly_count++;
	}
	
		// east

	if (map.sky.east_fill!=-1) {

			// vertexes and uv

		*vp++=camera_pos.x+f_radius;
		*vp++=camera_pos.y-f_radius;
		*vp++=camera_pos.z+f_radius;

		*uv++=0.0f;
		*uv++=0.0f;

		*vp++=camera_pos.x+f_radius;
		*vp++=camera_pos.y-f_radius;
		*vp++=camera_pos.z-f_radius;

		*uv++=txt_fact;
		*uv++=0.0f;

		*vp++=camera_pos.x+f_radius;
		*vp++=camera_pos.y+f_radius;
		*vp++=camera_pos.z-f_radius;

		*uv++=txt_fact;
		*uv++=txt_fact;

		*vp++=camera_pos.x+f_radius;
		*vp++=camera_pos.y+f_radius;
		*vp++=camera_pos.z+f_radius;

		*uv++=0.0f;
		*uv++=txt_fact;

			// polygon

		*vk++=4;
		*vk++=(short)map.textures[map.sky.east_fill].frames[0].bitmap.rl_material_id;
		for (k=0;k!=4;k++) {
			*vk++=(short)idx;
			*vk++=(short)idx;
			idx++;
		}

		poly_count++;
	}
	
		// south

	if (map.sky.south_fill!=-1) {

			// vertexes and uv

		*vp++=camera_pos.x+f_radius;
		*vp++=camera_pos.y-f_radius;
		*vp++=camera_pos.z-f_radius;

		*uv++=0.0f;
		*uv++=0.0f;

		*vp++=camera_pos.x-f_radius;
		*vp++=camera_pos.y-f_radius;
		*vp++=camera_pos.z-f_radius;

		*uv++=txt_fact;
		*uv++=0.0f;

		*vp++=camera_pos.x-f_radius;
		*vp++=camera_pos.y+f_radius;
		*vp++=camera_pos.z-f_radius;

		*uv++=txt_fact;
		*uv++=txt_fact;

		*vp++=camera_pos.x+f_radius;
		*vp++=camera_pos.y+f_radius;
		*vp++=camera_pos.z-f_radius;

		*uv++=0.0f;
		*uv++=txt_fact;

			// polygon

		*vk++=4;
		*vk++=(short)map.textures[map.sky.south_fill].frames[0].bitmap.rl_material_id;
		for (k=0;k!=4;k++) {
			*vk++=(short)idx;
			*vk++=(short)idx;
			idx++;
		}

		poly_count++;
	}
	
		// west

	if (map.sky.west_fill!=-1) {

			// vertexes and uv

		*vp++=camera_pos.x-f_radius;
		*vp++=camera_pos.y-f_radius;
		*vp++=camera_pos.z-f_radius;

		*uv++=0.0f;
		*uv++=0.0f;

		*vp++=camera_pos.x-f_radius;
		*vp++=camera_pos.y-f_radius;
		*vp++=camera_pos.z+f_radius;

		*uv++=txt_fact;
		*uv++=0.0f;

		*vp++=camera_pos.x-f_radius;
		*vp++=camera_pos.y+f_radius;
		*vp++=camera_pos.z+f_radius;

		*uv++=txt_fact;
		*uv++=txt_fact;

		*vp++=camera_pos.x-f_radius;
		*vp++=camera_pos.y+f_radius;
		*vp++=camera_pos.z-f_radius;

		*uv++=0.0f;
		*uv++=txt_fact;

			// polygon

		*vk++=4;
		*vk++=(short)map.textures[map.sky.west_fill].frames[0].bitmap.rl_material_id;
		for (k=0;k!=4;k++) {
			*vk++=(short)idx;
			*vk++=(short)idx;
			idx++;
		}

		poly_count++;
	}

	rtlSceneMeshUnMapVertexPointer(view_rtl_draw_scene_id,rtl_sky_mesh_id);
	rtlSceneMeshUnMapUVPointer(view_rtl_draw_scene_id,rtl_sky_mesh_id);

		// build the polygons

	rtlSceneMeshSetPoly(view_rtl_draw_scene_id,rtl_sky_mesh_id,RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV,poly_count,polys);
}
