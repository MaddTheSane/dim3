/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Piece Routines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern map_type				map;
extern app_state_type		state;
extern app_pref_type		pref;

/* =======================================================

      Mesh and Polygon Duplication
      
======================================================= */

bool map_piece_duplicate_mesh(int mesh_idx,d3pnt *mov_pnt)
{
	int			index;
	
	index=map_mesh_duplicate(&map,mesh_idx);
	if (index==-1) {
		os_dialog_alert("Can Not Create Mesh","Not enough memory.");
		return(FALSE);
	}
	
	map_mesh_move(&map,index,mov_pnt);
	view_force_grid(index,TRUE);

	view_vbo_mesh_initialize(index);
	
	select_duplicate_add(item_map_mesh,index,0);
	
	return(TRUE);
}

bool map_piece_duplicate_mesh_poly(int mesh_idx,int poly_idx,d3pnt *mov_pnt)
{
	int			index;
	
	index=map_mesh_add_duplicate_internal_poly(&map,mesh_idx,poly_idx,mov_pnt);
	if (index==-1) {
		os_dialog_alert("Can Not Create Polygon","Not enough memory.");
		return(FALSE);
	}
	
	view_vbo_mesh_rebuild(mesh_idx);
	
	select_duplicate_add(item_map_mesh,mesh_idx,index);
	
	return(TRUE);
}

/* =======================================================

      Duplicate Piece
      
======================================================= */

void piece_duplicate(void)
{
	int				n,i,nsel_count,type,main_idx,sub_idx,
					index;
	d3pnt			mpt,mov_pt;

	map_undo_push();
	
		// duplicate selection
		
	select_duplicate_clear();
	
		// get duplicate offset
		
	mov_pt.x=view_get_grid();
	mov_pt.y=0;
	mov_pt.z=view_get_grid();
	
		// duplicate pieces
		
	nsel_count=select_count();
	
	for (n=0;n!=nsel_count;n++) {
		select_get(n,&type,&main_idx,&sub_idx);
		
		switch (type) {
			
			case item_map_mesh:
				if (state.map.select_mode!=select_mode_polygon) {
					if (!map_piece_duplicate_mesh(main_idx,&mov_pt)) return;
				}
				else {
					if (!map_piece_duplicate_mesh_poly(main_idx,sub_idx,&mov_pt)) return;
				}
				break;
			
			case item_map_liquid:
				index=map_liquid_duplicate(&map,main_idx);
				if (index==-1) {
					os_dialog_alert("Can Not Create Liquid","Not enough memory.");
					return;
				}
				
				map_liquid_calculate_center(&map,index,&mpt);
				map_liquid_move(&map,index,&mov_pt);
				
				select_duplicate_add(item_map_liquid,index,-1);
				break;
				
			case item_map_spot:
				if (map.nspot==max_spot) {
					os_dialog_alert("Can Not Create Spot","You've reached the maximum number of spots for a map.");
					return;
				}
				
				map.spots[map.nspot]=map.spots[main_idx];
				map.spots[map.nspot].pnt.x+=mov_pt.x;
				map.spots[map.nspot].pnt.y+=mov_pt.y;
				map.spots[map.nspot].pnt.z+=mov_pt.z;
				select_duplicate_add(item_map_spot,map.nspot,-1);
				map.nspot++;
				break;
				
			case item_map_scenery:
				if (map.nscenery==max_map_scenery) {
					os_dialog_alert("Can Not Create Scenery","You've reached the maximum number of scenery for a map.");
					return;
				}
				
				map.sceneries[map.nscenery]=map.sceneries[main_idx];
				map.sceneries[map.nscenery].pnt.x+=mov_pt.x;
				map.sceneries[map.nscenery].pnt.y+=mov_pt.y;
				map.sceneries[map.nscenery].pnt.z+=mov_pt.z;
				select_duplicate_add(item_map_scenery,map.nscenery,-1);
				map.nscenery++;
				break;
				
			case item_map_node:
				if (map.nnode==max_node) {
					os_dialog_alert("Can Not Create Node","You've reached the maximum number of nodes for a map.");
					return;
				}

				map.nodes[map.nnode]=map.nodes[main_idx];
				map.nodes[map.nnode].pnt.x+=mov_pt.x;
				map.nodes[map.nnode].pnt.y+=mov_pt.y;
				map.nodes[map.nnode].pnt.z+=mov_pt.z;
				for (i=0;i!=max_node_link;i++) {
					map.nodes[map.nnode].link[i]=-1;
				}
				select_duplicate_add(item_map_node,map.nnode,-1);
				map.nnode++;
				break;
				
			case item_map_light:
				if (map.nlight==max_map_light) {
					os_dialog_alert("Can Not Create Light","You've reached the maximum number of lights for a map.");
					return;
				}

				map.lights[map.nlight]=map.lights[main_idx];
				map.lights[map.nlight].pnt.x+=mov_pt.x;
				map.lights[map.nlight].pnt.y+=mov_pt.y;
				map.lights[map.nlight].pnt.z+=mov_pt.z;
				select_duplicate_add(item_map_light,map.nlight,-1);
				map.nlight++;
				break;
				
			case item_map_sound:
				if (map.nsound==max_map_sound) {
					os_dialog_alert("Can Not Create Sound","You've reached the maximum number of sounds for a map.");
					return;
				}

				map.sounds[map.nsound]=map.sounds[main_idx];
				map.sounds[map.nsound].pnt.x+=mov_pt.x;
				map.sounds[map.nsound].pnt.y+=mov_pt.y;
				map.sounds[map.nsound].pnt.z+=mov_pt.z;
				select_duplicate_add(item_map_sound,map.nsound,-1);
				map.nsound++;
				break;
				
			case item_map_particle:
				if (map.nparticle==max_map_particle) {
					os_dialog_alert("Can Not Create Particle","You've reached the maximum number of particles for a map.");
					return;
				}

				map.particles[map.nparticle]=map.particles[main_idx];
				map.particles[map.nparticle].pnt.x+=mov_pt.x;
				map.particles[map.nparticle].pnt.y+=mov_pt.y;
				map.particles[map.nparticle].pnt.z+=mov_pt.z;
				select_duplicate_add(item_map_particle,map.nparticle,-1);
				map.nparticle++;
				break;
				
		}
    }
	
		// switch selections
		
	select_duplicate_copy();
	
	map_palette_reset();
	
	main_wind_draw();
}

/* =======================================================

      Delete Piece
      
======================================================= */

void piece_delete(void)
{
	int				n,i,k,nsel_count,
					type,main_idx,sub_idx;
	unsigned char	*mesh_mask;
	
	map_undo_push();
	
		// sort segment so higher indexes are deleted first
		
	select_sort();
	
		// selections have multiple polygons in them
		// so keep a mesh list
	
	mesh_mask=(unsigned char*)malloc(map.mesh.nmesh);
	if (mesh_mask==NULL) return;
	
	bzero(mesh_mask,map.mesh.nmesh);
	
		// delete selection
	
	nsel_count=select_count();
	
	for (n=0;n!=nsel_count;n++) {
		select_get(n,&type,&main_idx,&sub_idx);
	
		switch (type) {
			
			case item_map_mesh:
				if (state.map.select_mode==select_mode_polygon) {
					map_mesh_delete_poly(&map,main_idx,sub_idx);
					select_delete_move_index(item_map_mesh,main_idx,sub_idx);
					break;
				}
				if (state.map.select_mode==select_mode_mesh) {
					if (mesh_mask[main_idx]==0x0) map_mesh_delete(&map,main_idx);
					mesh_mask[main_idx]=0x1;
					break;
				}
				break;
				
			case item_map_liquid:
				map_liquid_delete(&map,main_idx);
				break;
				
			case item_map_spot:
				for (i=main_idx;i<map.nspot;i++) {
					map.spots[i]=map.spots[i+1];
				}
				map.nspot--;
				break;
				
			case item_map_scenery:
				for (i=main_idx;i<map.nscenery;i++) {
					map.sceneries[i]=map.sceneries[i+1];
				}
				map.nscenery--;
				break;
				
			case item_map_node:
				for (i=0;i!=map.nnode;i++) {			// clear all linkage
					if (i==main_idx) continue;

					for (k=0;k!=max_node_link;k++) {
						if (map.nodes[i].link[k]==main_idx) map.nodes[i].link[k]=-1;
						if (map.nodes[i].link[k]>main_idx) map.nodes[i].link[k]--;
					}
				}

				for (i=main_idx;i<map.nnode;i++) {
					map.nodes[i]=map.nodes[i+1];
				}
				
				map.nnode--;
				break;
				
			case item_map_light:
				for (i=main_idx;i<map.nlight;i++) {
					map.lights[i]=map.lights[i+1];
				}
				map.nlight--;
				break;
				
			case item_map_sound:
				for (i=main_idx;i<map.nsound;i++) {
					map.sounds[i]=map.sounds[i+1];
				}
				map.nsound--;
				break;
				
			case item_map_particle:
				for (i=main_idx;i<map.nparticle;i++) {
					map.particles[i]=map.particles[i+1];
				}
				map.nparticle--;
				break;
				
		}
	}
	
	free(mesh_mask);
	
	select_clear();
	
	map_palette_reset();
    
	main_wind_draw();
}

/* =======================================================

      Select More Piece
      
======================================================= */

void piece_select_more_check_edge(d3pnt *k_pt1,d3pnt *k_pt2)
{
	int					n,k,t,t2;
	d3pnt				*pt1,*pt2;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
	
		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
		
			for (t=0;t!=poly->ptsz;t++) {
				t2=t+1;
				if (t2==poly->ptsz) t2=0;
				
				pt1=&mesh->vertexes[poly->v[t]];
				pt2=&mesh->vertexes[poly->v[t2]];
				
				if (((pt1->x==k_pt1->x) && (pt1->y==k_pt1->y) && (pt1->z==k_pt1->z) && (pt2->x==k_pt2->x) && (pt2->y==k_pt2->y) && (pt2->z==k_pt2->z)) || ((pt1->x==k_pt2->x) && (pt1->y==k_pt2->y) && (pt1->z==k_pt2->z) && (pt2->x==k_pt1->x) && (pt2->y==k_pt1->y) && (pt2->z==k_pt1->z))) {
					select_add(item_map_mesh,n,k);
					break;
				}
			}
			
			poly++;
		}
		
		mesh++;
	}
}

void piece_select_more(void)
{
	int					n,t,t2,sel_count,type,mesh_idx,poly_idx;
	d3pnt				*pt1,*pt2;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// select more polygons
		
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
	
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=item_map_mesh) continue;
		
		mesh=&map.mesh.meshes[mesh_idx];
		poly=&mesh->polys[poly_idx];
		
		for (t=0;t!=poly->ptsz;t++) {
			t2=t+1;
			if (t2==poly->ptsz) t2=0;
			
			pt1=&mesh->vertexes[poly->v[t]];
			pt2=&mesh->vertexes[poly->v[t2]];
			
			piece_select_more_check_edge(pt1,pt2);
		}
	}
	
	main_wind_draw();
}

/* =======================================================

      Piece Tesselate
      
======================================================= */

void piece_tesselate(bool mesh)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	
	map_undo_push();
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==item_map_mesh) {
			if (mesh) {
				map_mesh_tesselate(&map,mesh_idx);
			}
			else {
				map_mesh_poly_tesselate(&map,mesh_idx,poly_idx);
			}
			view_vbo_mesh_rebuild(mesh_idx);
		}
	}

	select_clear();
	
	main_wind_draw();
}

/* =======================================================

      Piece Distort
      
======================================================= */

void piece_distort(void)
{
	int				n,k,t,sel_count,type,
					idx,sub_idx,mesh_idx;
	bool			distort_ok;
	d3pnt			mov_pnt;
	d3pnt			*pnt,*chk_pnt;
	map_mesh_type	*mesh,*chk_mesh;

		// get mesh
	
	mesh_idx=-1;
		
	sel_count=select_count();

	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&idx,&sub_idx);
		if (type==item_map_mesh) {
			mesh_idx=idx;
			break;
		}
	}

	if (mesh_idx==-1) return;
	
		// run dialog
		
    if (!dialog_distort_run(&mov_pnt)) return;

		// distort
	
	map_undo_push();

	mesh=&map.mesh.meshes[mesh_idx];

	pnt=mesh->vertexes;

	for (n=0;n!=mesh->nvertex;n++) {
	
			// don't distort if this point
			// is shared by another mesh
			
		distort_ok=TRUE;
			
		for (k=0;k!=map.mesh.nmesh;k++) {
			if (k==mesh_idx) continue;
			
			chk_mesh=&map.mesh.meshes[k];
			chk_pnt=chk_mesh->vertexes;
			
			for (t=0;t!=chk_mesh->nvertex;t++) {
				if ((chk_pnt->x==pnt->x) && (chk_pnt->y==pnt->y) && (chk_pnt->z==pnt->z)) {
					distort_ok=FALSE;
					break;
				}
				chk_pnt++;
			}
			
			if (!distort_ok) break;
		}
		
			// randomly distort the vertex
			
		if (distort_ok) {
			if (mov_pnt.x>0) pnt->x+=(mov_pnt.x-((random()%mov_pnt.x)*2));
			if (mov_pnt.y>0) pnt->y+=(mov_pnt.y-((random()%mov_pnt.y)*2));
			if (mov_pnt.z>0) pnt->z+=(mov_pnt.z-((random()%mov_pnt.z)*2));
		}
		
		pnt++;
	}

	view_vbo_mesh_rebuild(mesh_idx);
	main_wind_draw();
}

/* =======================================================

      Piece Resize and Reposition
      
======================================================= */

void piece_resize(void)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	d3pnt			min,max,mpt;
	d3fpnt			scale;

		// setup scale
	
	mesh_idx=-1;
		
	sel_count=select_count();

	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==item_map_mesh) break;
	}

	if (mesh_idx==-1) return;

	map_mesh_calculate_extent(&map,mesh_idx,&min,&max);
	scale.x=(float)abs(max.x-min.x);
	scale.y=(float)abs(max.y-min.y);
	scale.z=(float)abs(max.z-min.z);
	
		// run dialog
		
    if (!dialog_scale_run(&scale)) return;
	
	if ((scale.x<=0.0f) || (scale.y<=0.0f) || (scale.z<=0.0f)) return;
	
		// resize meshes
		
	map_undo_push();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		
		if (type==item_map_mesh) {

				// resize piece

			map_mesh_calculate_extent(&map,mesh_idx,&min,&max);
			map_mesh_calculate_center(&map,mesh_idx,&mpt);
						
			min.x=(int)((float)(min.x-mpt.x)*scale.x)+mpt.x;
			min.y=(int)((float)(min.y-mpt.y)*scale.y)+mpt.y;
			min.z=(int)((float)(min.z-mpt.z)*scale.z)+mpt.z;
			
			max.x=(int)((float)(max.x-mpt.x)*scale.x)+mpt.x;
			max.y=(int)((float)(max.y-mpt.y)*scale.y)+mpt.y;
			max.z=(int)((float)(max.z-mpt.z)*scale.z)+mpt.z;

			map_mesh_resize(&map,mesh_idx,&min,&max);
			view_vbo_mesh_rebuild(mesh_idx);

				// handle any cascades

			view_click_fix_cascade_size(mesh_idx);
		}
	}
	
	main_wind_draw();
}

void piece_force_grid(void)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	
	map_undo_push();
		
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==item_map_mesh) {
			view_force_grid(mesh_idx,FALSE);
			view_vbo_mesh_rebuild(mesh_idx);
		}
	}
	
	main_wind_draw();
}

/* =======================================================

      Piece Transforms
      
======================================================= */

void piece_flip(bool flip_x,bool flip_y,bool flip_z)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	
	map_undo_push();
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==item_map_mesh) {
			map_mesh_flip(&map,mesh_idx,flip_x,flip_y,flip_z);
			view_vbo_mesh_rebuild(mesh_idx);
		}
	}
	
	main_wind_draw();
}

void piece_rotate(d3ang *ang)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	d3pnt			center_pnt;
	
	map_undo_push();
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=item_map_mesh) continue;
		
		map_mesh_calculate_center(&map,mesh_idx,&center_pnt);
		map_mesh_rotate(&map,mesh_idx,&center_pnt,ang);
		view_vbo_mesh_rebuild(mesh_idx);
	}
	
	main_wind_draw();
}

void piece_free_rotate(void)
{
	d3ang			ang;
	
	if (dialog_free_rotate_run(&ang)) piece_rotate(&ang);
}

void piece_move(int move_x,int move_y,int move_z)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	d3pnt			mov_pnt;
	
	map_undo_push();
	
	sel_count=select_count();
	
	mov_pnt.x=move_x;
	mov_pnt.y=move_y;
	mov_pnt.z=move_z;
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==item_map_mesh) {
			if (!map.mesh.meshes[mesh_idx].flag.lock_move) {
				map_mesh_move(&map,mesh_idx,&mov_pnt);
				view_force_grid(mesh_idx,TRUE);
				view_vbo_mesh_rebuild(mesh_idx);
			}
		}
	}
	
	main_wind_draw();
}

/* =======================================================

      Select All Polys in Mesh
      
======================================================= */

void piece_mesh_select_all_poly(void)
{
	int				n,k,sel_count,type,mesh_idx,poly_idx;
	map_mesh_type	*mesh;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=item_map_mesh) continue;
		
		mesh=&map.mesh.meshes[mesh_idx];
		
		for (k=0;k!=mesh->npoly;k++) {
			if (!select_check(item_map_mesh,mesh_idx,k)) select_add(item_map_mesh,mesh_idx,k);
		}
	}
}

/* =======================================================

      Reset/Rotate Mesh UVs
      
======================================================= */

void piece_reset_uvs(bool poly_only)
{
	int						n,sel_count,
							type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=item_map_mesh) continue;
		if (map.mesh.meshes[mesh_idx].flag.lock_uv) continue;
		
		if (poly_only) {
			map_mesh_reset_poly_uv(&map,mesh_idx,poly_idx);
		}
		else {
			map_mesh_reset_uv(&map,mesh_idx);
		}

		view_vbo_mesh_rebuild(mesh_idx);
	}
}

void piece_whole_uvs(bool poly_only)
{
	int						n,sel_count,
							type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=item_map_mesh) continue;
		
		if (poly_only) {
			map_mesh_whole_poly_uv(&map,mesh_idx,poly_idx);
		}
		else {
			map_mesh_whole_uv(&map,mesh_idx);
		}
		view_vbo_mesh_rebuild(mesh_idx);
	}
}

void piece_single_uvs(bool poly_only)
{
	int						n,sel_count,
							type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=item_map_mesh) continue;
		
		if (poly_only) {
			map_mesh_poly_single_uv(&map,mesh_idx,poly_idx);
		}
		else {
			map_mesh_single_uv(&map,mesh_idx);
		}
		view_vbo_mesh_rebuild(mesh_idx);
	}
}

void piece_rotate_uvs(void)
{
	int						n,sel_count,
							type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=item_map_mesh) continue;

		map_mesh_rotate_poly_uv(&map,mesh_idx,poly_idx,90);
		view_vbo_mesh_rebuild(mesh_idx);
	}
}

void piece_flip_uvs(bool flip_u,bool flip_v)
{
	int						n,sel_count,
							type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=item_map_mesh) continue;

		map_mesh_flip_poly_uv(&map,mesh_idx,poly_idx,flip_u,flip_v);
		view_vbo_mesh_rebuild(mesh_idx);
	}
}

/* =======================================================

      Piece Normals
      
======================================================= */

void piece_mesh_recalc_normals(bool poly_only)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=item_map_mesh) continue;
		
		if (poly_only) {
			map_recalc_normals_mesh_poly(&map,&map.mesh.meshes[mesh_idx],poly_idx,normal_mode_none,FALSE);
		}
		else {
			map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_none,FALSE);
		}
	}
	
	main_wind_draw();
}

void piece_mesh_poly_invert_normals(map_mesh_poly_type *poly)
{
	poly->tangent_space.normal.x=-poly->tangent_space.normal.x;
	poly->tangent_space.normal.y=-poly->tangent_space.normal.y;
	poly->tangent_space.normal.z=-poly->tangent_space.normal.z;
}
	
void piece_mesh_invert_normals(bool poly_only)
{
	int				n,k,sel_count,type,mesh_idx,poly_idx;
	map_mesh_type	*mesh;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=item_map_mesh) continue;
		
			// invert the poly
			
		mesh=&map.mesh.meshes[mesh_idx];
		
		if (poly_only) {
			piece_mesh_poly_invert_normals(&mesh->polys[poly_idx]);
		}
		else {
			for (k=0;k!=mesh->npoly;k++) {
				piece_mesh_poly_invert_normals(&mesh->polys[k]);
			}
		}
	}
	
	main_wind_draw();
}

void piece_mesh_set_normals_in_out(bool out)
{
	int					n,sel_count,normal_mode,
						type,mesh_idx,poly_idx;
	map_mesh_type		*mesh;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=item_map_mesh) continue;
		
			// recalc the normals
			
		mesh=&map.mesh.meshes[mesh_idx];

		normal_mode=out?normal_mode_out:normal_mode_in;
		map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode,FALSE);
	}
}

/* =======================================================

      Piece Holes
      
======================================================= */

void piece_poly_hole(void)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==item_map_mesh) {
			map_mesh_poly_punch_hole(&map,mesh_idx,poly_idx,NULL,FALSE,normal_mode_in);
			view_vbo_mesh_rebuild(mesh_idx);
		}
	}
	
	select_clear();
	
	main_wind_draw();
}

/* =======================================================

      Piece Liquid Reset
      
======================================================= */

void piece_liquid_reset_size(map_liquid_type *liq)
{
	int				len,cnt;
	
	if (liq->wave.dir_north_south) {
		len=liq->bot-liq->top;
	}
	else {
		len=liq->rgt-liq->lft;
	}
	
	cnt=len/liq->wave.length;
	if ((len%liq->wave.length)!=0) cnt++;
	
	cnt=cnt/4;
	if ((cnt%4)!=0) cnt++;
	cnt=cnt*4;
	
	len=cnt*liq->wave.length;
	
	if (liq->wave.dir_north_south) {
		liq->bot=liq->top+len;
	}
	else {
		liq->rgt=liq->lft+len;
	}
}

/* =======================================================

      Piece Keys
      
======================================================= */

void piece_key(char ch)
{
	int					n,sel_count,type,main_idx,sub_idx,mv;
	d3pnt				move_pnt;
	editor_view_type	*view;
	
		// special check for delete key
		
	if ((ch==D3_KEY_BACKSPACE) || (ch==D3_KEY_DELETE)) {
		piece_delete();
		map_palette_reset();
		main_wind_draw();
		main_wind_menu_update();
		return;
	}
	
		// nudge works with grid
	
	mv=view_get_grid();
	if (os_key_shift_down()) mv*=move_key_shift_multiply;
	
		// nudge keys movement

	view=view_get_current_view();
	move_pnt.x=move_pnt.y=move_pnt.z=0;
		
	switch (ch) {
	
		case D3_KEY_LEFT:
			view_mouse_get_scroll_horizontal_axis(view,&move_pnt,-mv);
			break;
			
		case D3_KEY_RIGHT:
			view_mouse_get_scroll_horizontal_axis(view,&move_pnt,+mv);
			break;
			
		case D3_KEY_UP:
			view_mouse_get_scroll_vertical_axis(view,&move_pnt,-mv);
			break;
			
		case D3_KEY_DOWN:
			view_mouse_get_scroll_vertical_axis(view,&move_pnt,mv);
			break;
			
	}
	
	if ((move_pnt.x==0) && (move_pnt.y==0) && (move_pnt.z==0)) return;
	
		// move selection
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&main_idx,&sub_idx);

		switch (type) {
		
			case item_map_mesh:
				if (map.mesh.meshes[main_idx].flag.lock_move) break;
				map_mesh_move(&map,main_idx,&move_pnt);
				if ((state.map.auto_texture) && (!map.mesh.meshes[main_idx].flag.lock_uv)) map_mesh_reset_uv(&map,main_idx);
				view_vbo_mesh_rebuild(main_idx);
				break;
				
			case item_map_liquid:
				map_liquid_move(&map,main_idx,&move_pnt);
				if ((state.map.auto_texture) && (!map.liquid.liquids[main_idx].flag.lock_uv)) map_liquid_reset_uv(&map,main_idx);
				break;
				
			case item_map_node:
				map.nodes[main_idx].pnt.x+=move_pnt.x;
				map.nodes[main_idx].pnt.y+=move_pnt.y;
				map.nodes[main_idx].pnt.z+=move_pnt.z;
				break;
				
			case item_map_spot:
				map.spots[main_idx].pnt.x+=move_pnt.x;
				map.spots[main_idx].pnt.y+=move_pnt.y;
				map.spots[main_idx].pnt.z+=move_pnt.z;
				break;
				
			case item_map_scenery:
				map.sceneries[main_idx].pnt.x+=move_pnt.x;
				map.sceneries[main_idx].pnt.y+=move_pnt.y;
				map.sceneries[main_idx].pnt.z+=move_pnt.z;
				break;
				
			case item_map_light:
				map.lights[main_idx].pnt.x+=move_pnt.x;
				map.lights[main_idx].pnt.y+=move_pnt.y;
				map.lights[main_idx].pnt.z+=move_pnt.z;
				break;
				
			case item_map_sound:
				map.sounds[main_idx].pnt.x+=move_pnt.x;
				map.sounds[main_idx].pnt.y+=move_pnt.y;
				map.sounds[main_idx].pnt.z+=move_pnt.z;
				break;
				
			case item_map_particle:
				map.particles[main_idx].pnt.x+=move_pnt.x;
				map.particles[main_idx].pnt.y+=move_pnt.y;
				map.particles[main_idx].pnt.z+=move_pnt.z;
				break;
				
		}
	}
	
	main_wind_draw();
}

