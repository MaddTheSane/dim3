/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Vertex Lists

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
extern view_type			view;
extern setup_type			setup;

/* =======================================================

      Initialize VBOs For Meshes
      
======================================================= */

bool view_map_vbo_initialize_mesh(map_mesh_type *mesh)
{
	int					n,k;
	unsigned short		idx;
	unsigned short		*index_ptr;
	float				*pf;
	unsigned char		*vertex_ptr,*vp;
	d3pnt				*pnt;
	map_mesh_poly_type	*poly;
	
		// setup vertex pointer

	view_bind_mesh_liquid_vertex_object(&mesh->vbo);
	vertex_ptr=view_map_mesh_liquid_vertex_object(&mesh->vbo);
	if (vertex_ptr==NULL) {
		view_unbind_mesh_liquid_vertex_object();
		return(FALSE);
	}

		// vertexes, uvs, tangents, normals
		
	vp=vertex_ptr;

	poly=mesh->polys;
	
	for (n=0;n!=mesh->npoly;n++) {
	
		for (k=0;k!=poly->ptsz;k++) {
		
			pnt=&mesh->vertexes[poly->v[k]];
			
			pf=(float*)vp;

			*pf++=(float)pnt->x;
			*pf++=(float)pnt->y;
			*pf++=(float)pnt->z;
			
			*pf++=poly->main_uv.uvs[k].x+poly->draw.shift_offset.x;
			*pf++=poly->main_uv.uvs[k].y+poly->draw.shift_offset.y;
			
			*pf++=poly->lmap_uv.uvs[k].x;
			*pf++=poly->lmap_uv.uvs[k].y;

			*pf++=poly->tangent_space.tangent.x;
			*pf++=poly->tangent_space.tangent.y;
			*pf++=poly->tangent_space.tangent.z;
			*pf++=poly->tangent_space.normal.x;
			*pf++=poly->tangent_space.normal.y;
			*pf++=poly->tangent_space.normal.z;
			
			vp+=mesh->vbo.vertex_stride;
		}

		poly++;
	}

	view_unmap_mesh_liquid_vertex_object();
	view_unbind_mesh_liquid_vertex_object();

		// create the indexes

	view_bind_mesh_liquid_index_object(&mesh->vbo);
	
	index_ptr=view_map_mesh_liquid_index_object();
	if (index_ptr==NULL) {
		view_unbind_mesh_liquid_index_object();
		return(FALSE);
	}

	idx=0;

	poly=mesh->polys;
		
	for (n=0;n!=mesh->npoly;n++) {
		
		poly->vbo.index_offset=idx*sizeof(unsigned short);
		
		for (k=0;k!=poly->ptsz;k++) {
			*index_ptr++=(unsigned short)idx;
			idx++;
		}

		poly++;
	}

	view_unmap_mesh_liquid_index_object();
	view_unbind_mesh_liquid_index_object();

		// reset some flags that
		// optimize when the VBO is rebuilt

	mesh->draw.moved=FALSE;
	mesh->draw.cur_ambient_only=FALSE;

	return(TRUE);
}

/* =======================================================

      Initialize VBOs For Liquids
      
======================================================= */

bool view_map_vbo_initialize_liquid(map_liquid_type *liq)
{
	int				n,idx;
	unsigned short	*index_ptr;

		// only setup indexes for liquids

	view_bind_mesh_liquid_index_object(&liq->vbo);
	
	index_ptr=view_map_mesh_liquid_index_object();
	if (index_ptr==NULL) {
		view_unbind_mesh_liquid_index_object();
		return(FALSE);
	}

	idx=0;

	for (n=0;n!=liq->vbo.index_count;n++) {
		*index_ptr++=(unsigned short)idx;
		idx++;
	}

	view_unmap_mesh_liquid_index_object();
	view_unbind_mesh_liquid_index_object();

	return(TRUE);
}

/* =======================================================

      Initialize VBOs For Map
      
======================================================= */

bool view_map_vbo_initialize(void)
{
	int					n,k,vertex_cnt,index_cnt,stride,
						liq_div_count;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	map_liquid_type		*liq;

		// meshes

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {

			// get index count

		index_cnt=0;
		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			index_cnt+=poly->ptsz;
			poly++;
		}

			// get vertex count

		vertex_cnt=index_cnt;
		
		stride=(3+2+2+3+3)*sizeof(float);			// 3 vertex, 2 uv, 2 light map uv, 3 tangent, 3 normal

			// create the VBO
			
		view_create_mesh_liquid_vertex_object(&mesh->vbo,vertex_cnt,stride,index_cnt);

		if (!view_map_vbo_initialize_mesh(mesh)) return(FALSE);

		mesh++;
	}

		// liquids

	liq=map.liquid.liquids;

	for (n=0;n!=map.liquid.nliquid;n++) {

			// liquids can be broken up for waves,
			// with 2 vertexes per wave and 4 indexes
			// to draw the triangles

		liq_div_count=liquid_wave_get_divisions(liq);

		index_cnt=(liq_div_count+1)*2;				// strip drawing, so only one per vertex
		vertex_cnt=(liq_div_count+1)*2;

		stride=(3+2+2+3+3)*sizeof(float);				// 3 vertex, 2 uv, 2 light map uv, 3 tangent, 3 normal

			// create the liquid

		view_create_mesh_liquid_vertex_object(&liq->vbo,vertex_cnt,stride,index_cnt);

			// initialize the liquid

		if (!view_map_vbo_initialize_liquid(liq)) return(FALSE);
		liq++;
	}

	return(TRUE);
}

void view_map_vbo_release(void)
{
	int					n;
	map_mesh_type		*mesh;
	map_liquid_type		*liq;

		// release meshes
		// color caches get released when mesh is destroyed

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
		view_dispose_mesh_liquid_vertex_object(&mesh->vbo);
		mesh++;
	}

		// release liquids

	liq=map.liquid.liquids;

	for (n=0;n!=map.liquid.nliquid;n++) {
		view_dispose_mesh_liquid_vertex_object(&liq->vbo);
		liq++;
	}

}

/* =======================================================

      Rebuild VBOs For Map Changes
      
======================================================= */

void view_map_vbo_rebuild_mesh(map_mesh_type *mesh)
{
	int					n,k;
	float				x_shift_offset,y_shift_offset;
	float				*pf;
	unsigned char		*vertex_ptr,*vp;
	d3pnt				*pnt;
	map_mesh_poly_type	*poly;

		// we don't immediately bind and map
		// until we actually require a change
		
	vertex_ptr=NULL;
	
	if ((mesh->flag.moveable) && (mesh->draw.moved)) {
	
		if (vertex_ptr==NULL) {
			view_bind_mesh_liquid_vertex_object(&mesh->vbo);
			vertex_ptr=view_map_mesh_liquid_vertex_object(&mesh->vbo);
			if (vertex_ptr==NULL) {
				view_unbind_mesh_liquid_vertex_object();
				return;
			}
		}

		vp=vertex_ptr;

		poly=mesh->polys;
		
		for (n=0;n!=mesh->npoly;n++) {

				// polygon vertexes

			for (k=0;k!=poly->ptsz;k++) {
			
				pnt=&mesh->vertexes[poly->v[k]];
				
				pf=(float*)vp;

				*pf++=(float)pnt->x;
				*pf++=(float)pnt->y;
				*pf++=(float)pnt->z;
				
				pf+=4;
				
				*pf++=poly->tangent_space.tangent.x;
				*pf++=poly->tangent_space.tangent.y;
				*pf++=poly->tangent_space.tangent.z;
				*pf++=poly->tangent_space.normal.x;
				*pf++=poly->tangent_space.normal.y;
				*pf++=poly->tangent_space.normal.z;
				
				vp+=mesh->vbo.vertex_stride;
			}

			poly++;
		}
		
			// only rebuild when moving
			
		mesh->draw.moved=FALSE;
	}

		// recalculate the uvs if this
		// mesh has shiftable uvs

	if (mesh->precalc_flag.shiftable) {
	
		if (vertex_ptr==NULL) {
			view_bind_mesh_liquid_vertex_object(&mesh->vbo);
			vertex_ptr=view_map_mesh_liquid_vertex_object(&mesh->vbo);
			if (vertex_ptr==NULL) {
				view_unbind_mesh_liquid_vertex_object();
				return;
			}
		}

			// only shift main UVs (not light mapped ones)

		vp=vertex_ptr;
		
		poly=mesh->polys;

		for (n=0;n!=mesh->npoly;n++) {

			x_shift_offset=poly->draw.shift_offset.x;
			y_shift_offset=poly->draw.shift_offset.y;

			for (k=0;k!=poly->ptsz;k++) {
				pf=(float*)(vp+(3*sizeof(float)));
				
				*pf++=poly->main_uv.uvs[k].x+x_shift_offset;
				*pf=poly->main_uv.uvs[k].y+y_shift_offset;
				
				vp+=mesh->vbo.vertex_stride;
			}

			poly++;
		}
	}
	
		// if we made a change, then it got
		// mapped and we need to unmap it
		
	if (vertex_ptr!=NULL) {
		view_unmap_mesh_liquid_vertex_object();
		view_unbind_mesh_liquid_vertex_object();
	}
}

void view_map_vbo_rebuild(void)
{
	int				n;
	map_mesh_type	*mesh;
	
		// run through all meshes in
		// view list
		
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];
		view_map_vbo_rebuild_mesh(mesh);
	}
}

