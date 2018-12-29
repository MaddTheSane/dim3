/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Dragging Routines

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

extern map_type					map;
extern app_state_type			state;

/* =======================================================

      Fix Cascading Meshes
      
======================================================= */

void view_click_fix_cascade_size(int mesh_idx)
{
	int						n;
	d3pnt					dpnt,min,max;
	map_mesh_type			*mesh,*ccd_mesh;

		// cascades only work if original mesh
		// has cascade set, and to all meshes with
		// cascading and the same import strings

	mesh=&map.mesh.meshes[mesh_idx];
	if (!mesh->flag.cascade_size) return;

		// get difference

	map_mesh_calculate_extent(&map,mesh_idx,&min,&max);

	dpnt.x=max.x-min.x;
	dpnt.y=max.y-min.y;
	dpnt.z=max.z-min.z;

		// fix any connected meshes

	for (n=0;n!=map.mesh.nmesh;n++) {
		if (n==mesh_idx) continue;

			// good target?

		ccd_mesh=&map.mesh.meshes[n];
		if (!ccd_mesh->flag.cascade_size) continue;
		if (strcmp(ccd_mesh->import.obj_name,mesh->import.obj_name)!=0) continue;
		if (strcmp(ccd_mesh->import.group_name,mesh->import.group_name)!=0) continue;

			// fix!

		map_mesh_calculate_extent(&map,n,&min,&max);

		max.x=min.x+dpnt.x;
		max.y=min.y+dpnt.y;
		max.z=min.z+dpnt.z;

		map_mesh_resize(&map,n,&min,&max);
		view_vbo_mesh_rebuild(n);
	}
}

/* =======================================================

      Drag Mesh Handles
      
======================================================= */

void view_click_drag_mesh_handle_resize(int mesh_idx,d3pnt *old_dpt,int handle_idx,d3pnt *mpt,d3pnt *org_min,d3pnt *org_max)
{
	d3pnt			min,max;
	map_mesh_type	*mesh;

		// start with original size

	memmove(&min,org_min,sizeof(d3pnt));
	memmove(&max,org_max,sizeof(d3pnt));
	
		// use correct handle to alter the mesh
		
	if ((handle_idx==0) || (handle_idx==3) || (handle_idx==4) || (handle_idx==7)) {
		min.x+=mpt->x>>1;
	}
	else {
		max.x+=mpt->x>>1;
	}
	
	if (handle_idx<4) {
		min.y+=mpt->y>>1;
	}
	else {
		max.y+=mpt->y>>1;
	}
	
	if ((handle_idx==0) || (handle_idx==1) || (handle_idx==4) || (handle_idx==5)) {
		min.z+=mpt->z>>1;
	}
	else {
		max.z+=mpt->z>>1;
	}
	
		// always resize from original vertexes
		
	mesh=&map.mesh.meshes[mesh_idx];

	memmove(mesh->vertexes,old_dpt,(mesh->nvertex*sizeof(d3pnt)));
	map_mesh_resize(&map,mesh_idx,&min,&max);
	view_vbo_mesh_rebuild(mesh_idx);
}

void view_click_drag_mesh_handle_skew(int mesh_idx,d3pnt *old_dpt,int handle_idx,d3pnt *mpt)
{
	int				axis,dir,size;
	d3pnt			move_pnt;
	map_mesh_type	*mesh;

		// always skew from original points

	mesh=&map.mesh.meshes[mesh_idx];
	memmove(mesh->vertexes,old_dpt,(mesh->nvertex*sizeof(d3pnt)));

		// get proper skew

	axis=0;
	dir=0;
	size=mpt->x+mpt->y;
	
	move_pnt.x=move_pnt.y=move_pnt.z=0;

	switch (handle_idx) {

			// x skewing

		case 8:
		case 9:
			axis=1;
			dir=0;
			size=-size;
			move_pnt.y=size;
			break;

		case 10:
		case 11:
			axis=1;
			dir=0;
			break;

			// y skewing

		case 12:
		case 13:
			axis=2;
			dir=1;
			size=-size;
			move_pnt.z=-size;
			break;

		case 14:
		case 15:
			axis=2;
			dir=1;
			break;

			// z skewing

		case 16:
		case 17:
			axis=0;
			dir=2;
			size=-size;
			move_pnt.x=-size;
			break;

		case 18:
		case 19:
			axis=0;
			dir=2;
			break;
	}

		// run the skew

	if (size==0) return;
	
	map_mesh_skew(&map,mesh_idx,axis,dir,size);
	map_mesh_move(&map,mesh_idx,&move_pnt);

	view_vbo_mesh_rebuild(mesh_idx);
}

bool view_click_drag_mesh_handle(editor_view_type *view,d3pnt *pt)
{
	int						n,x,y,mx,my,mz,scale,sub_idx,
							type,mesh_idx,poly_idx,handle_idx;
	bool					first_drag;
	d3pnt					pts[20],old_pt,*old_dpt,mpt,move_pnt,
							org_min,org_max;
	d3rect					box;
	map_mesh_type			*mesh;
	
		// any selection?
		
	if (select_count()==0) return(FALSE);
	
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=item_map_mesh) return(FALSE);
	
	mesh=&map.mesh.meshes[mesh_idx];
	
	if (mesh->flag.lock_move) return(FALSE);
	
		// are we clicking in the grow handles?
		
	map_view_draw_select_mesh_get_grow_handles(mesh_idx,pts);
		
	view_pick_list_start(view,FALSE,20);
	
	for (n=0;n!=20;n++) {
		view_pick_list_add_handle(&pts[n],item_map_liquid,n,0);
	}

	handle_idx=-1;

	view_pick_list_end(view,pt,&type,&handle_idx,&sub_idx);
	
	if (handle_idx==-1) return(FALSE);
	
		// are we dragging?
		
    if (!os_button_down()) return(FALSE);
	
	map_undo_push();
	
		// hilite the drag handle
		
	state.map.drag_handle_idx=handle_idx;
	main_wind_draw();
	
		// backup all the vertexes
		
	old_dpt=malloc(mesh->nvertex*sizeof(d3pnt));
	if (old_dpt==NULL) return(FALSE);
	
	memmove(old_dpt,mesh->vertexes,(mesh->nvertex*sizeof(d3pnt)));
	
		// get the scale
		
	map_mesh_calculate_center(&map,mesh_idx,&mpt);
	scale=distance_get(view->pnt.x,view->pnt.y,view->pnt.z,mpt.x,mpt.y,mpt.z)/move_mouse_drag_distance_ratio;
		
		// start dragging

	first_drag=TRUE;
	
	map_mesh_calculate_extent(&map,mesh_idx,&org_min,&org_max);
	
	view_get_pixel_box(view,&box);
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	mx=my=mz=0;
	
	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}
		
			// get change

		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		if (handle_idx<8) {
			view_mouse_get_scroll_horizontal_axis(view,&move_pnt,-(x*scale));
			view_mouse_get_scroll_vertical_axis(view,&move_pnt,-(y*scale));
		}
		else {
			move_pnt.x=-(x*scale);
			move_pnt.y=-(y*scale);
		}

		mx+=move_pnt.x;
		my+=move_pnt.y;
		mz+=move_pnt.z;

		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		view_click_grid(&mpt);

			// resize mesh

		if (handle_idx<8) {
			view_click_drag_mesh_handle_resize(mesh_idx,old_dpt,handle_idx,&mpt,&org_min,&org_max);
		}
		else {
			view_click_drag_mesh_handle_skew(mesh_idx,old_dpt,handle_idx,&mpt);
		}
		
		if ((state.map.auto_texture) && (!mesh->flag.lock_uv)) map_mesh_reset_uv(&map,mesh_idx);

        main_wind_draw();
	}
	
	free(old_dpt);

		// run any cascades

	view_click_fix_cascade_size(mesh_idx);

		// done dragging

	os_set_arrow_cursor();
	
		// turn off hilite
		
	state.map.drag_handle_idx=-1;
	
	main_wind_draw();
	
	return(TRUE);
}

/* =======================================================

      Drag Mesh
      
======================================================= */

bool view_click_drag_mesh(editor_view_type *view,d3pnt *pt)
{
	int						n,k,x,y,mx,my,mz,scale,nsel,nvertex,
							type,mesh_idx,poly_idx;
	bool					first_drag;
	d3pnt					old_pt,*dpt,*old_dpt,*old_dpt_ptr,move_pnt,mpt;
	d3rect					box;
	map_mesh_type			*mesh;
	
		// any selection?
		
	nsel=select_count();
	if (nsel==0) return(FALSE);
	
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=item_map_mesh) return(FALSE);
	
	mesh=&map.mesh.meshes[mesh_idx];
	
	if (mesh->flag.lock_move) return(FALSE);
		
		// start drag
		
    if (!os_button_down()) return(FALSE);
	
	map_undo_push();
	
		// save old vertexes
		
	nvertex=0;
	
	for (k=0;k!=nsel;k++) {
		select_get(k,&type,&mesh_idx,&poly_idx);
		if (type!=item_map_mesh) continue;
		
		nvertex+=map.mesh.meshes[mesh_idx].nvertex;
	}
	
	old_dpt=malloc(nvertex*sizeof(d3pnt));
	if (old_dpt==NULL) return(FALSE);
	
	old_dpt_ptr=old_dpt;
	
	for (k=0;k!=nsel;k++) {
		select_get(k,&type,&mesh_idx,&poly_idx);
		if (type!=item_map_mesh) continue;
		
		mesh=&map.mesh.meshes[mesh_idx];

		memmove(old_dpt_ptr,mesh->vertexes,(mesh->nvertex*sizeof(d3pnt)));
		old_dpt_ptr+=mesh->nvertex;
	}
	
		// get the scale
		
	map_mesh_calculate_center(&map,mesh_idx,&mpt);
	scale=distance_get(view->pnt.x,view->pnt.y,view->pnt.z,mpt.x,mpt.y,mpt.z)/move_mouse_drag_distance_ratio;
	
		// drag meshes
		
	mx=my=mz=0;
			
	first_drag=TRUE;
	
	view_get_pixel_box(view,&box);
	
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}
		
			// get movement

		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		view_mouse_get_scroll_horizontal_axis(view,&move_pnt,-(x*scale));
		view_mouse_get_scroll_vertical_axis(view,&move_pnt,-(y*scale));
		
		mx+=move_pnt.x;
		my+=move_pnt.y;
		mz+=move_pnt.z;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
			// if there is a grid, use first vertex
			// for grid
			
		mpt.x+=old_dpt[0].x;
		mpt.y+=old_dpt[0].y;
		mpt.z+=old_dpt[0].z;

		view_click_grid(&mpt);
		
		mpt.x-=old_dpt[0].x;
		mpt.y-=old_dpt[0].y;
		mpt.z-=old_dpt[0].z;
		
			// snaps

		view_click_snap_mesh(old_dpt,&mpt);

			// move vertexes
		
		old_dpt_ptr=old_dpt;
		
		for (k=0;k!=nsel;k++) {
			select_get(k,&type,&mesh_idx,&poly_idx);
			if (type!=item_map_mesh) continue;
			
			mesh=&map.mesh.meshes[mesh_idx];
			dpt=mesh->vertexes;
		
			for (n=0;n!=mesh->nvertex;n++) {
				dpt->x=old_dpt_ptr->x+mpt.x;
				dpt->y=old_dpt_ptr->y+mpt.y;
				dpt->z=old_dpt_ptr->z+mpt.z;
				
				dpt++;
				old_dpt_ptr++;
			}
			
			if ((state.map.auto_texture) && (!mesh->flag.lock_uv)) map_mesh_reset_uv(&map,mesh_idx);

			view_vbo_mesh_rebuild(mesh_idx);
		}
		
        main_wind_draw();
	}
	
	free(old_dpt);
	
	os_set_arrow_cursor();
	
	main_wind_draw();
	
	return(!first_drag);
}

/* =======================================================

      Drag Mesh Poly
      
======================================================= */

bool view_click_drag_mesh_poly(editor_view_type *view,d3pnt *pt)
{
	int						n,x,y,mx,my,mz,scale,
							type,mesh_idx,poly_idx;
	bool					first_drag;
	d3pnt					old_pt,*dpt,*old_dpt,move_pnt,mpt,spt;
	d3rect					box;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	
		// any selection?
		
	if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=item_map_mesh) return(FALSE);

		// are we clicking in mesh
		
	mesh=&map.mesh.meshes[mesh_idx];
	mesh_poly=&mesh->polys[poly_idx];
	
	if (mesh->flag.lock_move) return(FALSE);
	
		// setup drag
		
    if (!os_button_down()) return(FALSE);
	
	map_undo_push();
			
	first_drag=TRUE;
	
	old_dpt=malloc(mesh_poly->ptsz*sizeof(d3pnt));
	if (old_dpt==NULL) return(FALSE);
	
	for (n=0;n!=mesh_poly->ptsz;n++) {
		memmove(&old_dpt[n],&mesh->vertexes[mesh_poly->v[n]],sizeof(d3pnt));
	}
	
		// get the scale
		
	map_mesh_poly_calculate_center(&map,mesh_idx,poly_idx,&mpt);
	scale=distance_get(view->pnt.x,view->pnt.y,view->pnt.z,mpt.x,mpt.y,mpt.z)/move_mouse_drag_distance_ratio;
	
		// run the drag
		
	mx=my=mz=0;
	
	view_get_pixel_box(view,&box);

	memmove(&old_pt,pt,sizeof(d3pnt));
	
	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}
		
			// get vertex movement

		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		view_mouse_get_scroll_horizontal_axis(view,&move_pnt,-(x*scale));
		view_mouse_get_scroll_vertical_axis(view,&move_pnt,-(y*scale));
		
		mx+=move_pnt.x;
		my+=move_pnt.y;
		mz+=move_pnt.z;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
			// if there is a grid, use first vertex
			// for grid
			
		mpt.x+=old_dpt[0].x;
		mpt.y+=old_dpt[0].y;
		mpt.z+=old_dpt[0].z;

		view_click_grid(&mpt);
		
		mpt.x-=old_dpt[0].x;
		mpt.y-=old_dpt[0].y;
		mpt.z-=old_dpt[0].z;
		
			// check all vertexes for snaps
			// and stop on first hit

		for (n=0;n!=mesh_poly->ptsz;n++) {
			spt.x=old_dpt[n].x+mpt.x;
			spt.y=old_dpt[n].y+mpt.y;
			spt.z=old_dpt[n].z+mpt.z;
				
			if (view_click_snap_poly(mesh_idx,poly_idx,&spt)) {
				mpt.x=spt.x-old_dpt[n].x;
				mpt.y=spt.y-old_dpt[n].y;
				mpt.z=spt.z-old_dpt[n].z;
				break;
			}
		}

			// move vertexes
			
		for (n=0;n!=mesh_poly->ptsz;n++) {
			dpt=&mesh->vertexes[mesh_poly->v[n]];
			dpt->x=old_dpt[n].x+mpt.x;
			dpt->y=old_dpt[n].y+mpt.y;
			dpt->z=old_dpt[n].z+mpt.z;
		}

		if ((state.map.auto_texture) && (!mesh->flag.lock_uv)) map_mesh_reset_poly_uv(&map,mesh_idx,poly_idx);

		view_vbo_mesh_rebuild(mesh_idx);

        main_wind_draw();
	}
	
	free(old_dpt);

	os_set_arrow_cursor();
	
	main_wind_draw();
	
	return(!first_drag);
}

/* =======================================================

      Drag Mesh Vertex
      
======================================================= */

bool view_click_drag_vertex(editor_view_type *view,d3pnt *pt)
{
	int						n,x,y,mx,my,mz,scale,sub_idx,
							type,mesh_idx,poly_idx,vertex_idx;
	d3pnt					old_pt,*dpt,old_dpt,move_pnt,mpt;
	d3rect					box;
	bool					first_drag;
	map_mesh_type			*mesh;
	
    if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=item_map_mesh) return(FALSE);
	
		// check for clicking points

	mesh=&map.mesh.meshes[mesh_idx];
	
	if (mesh->flag.lock_move) return(FALSE);
		
	view_pick_list_start(view,FALSE,mesh->nvertex);
	
	dpt=mesh->vertexes;
	
	for (n=0;n!=mesh->nvertex;n++) {
		view_pick_list_add_handle(dpt,item_map_mesh,n,0);
		dpt++;
	}

	vertex_idx=-1;

	view_pick_list_end(view,pt,&type,&vertex_idx,&sub_idx);

    if (vertex_idx==-1) return(FALSE);
	
		// drag mesh
	
    if (!os_button_down()) return(FALSE);
	
	map_undo_push();
	
		// turn on hilite
		
	state.map.drag_handle_idx=vertex_idx;
	main_wind_draw();
	
		// get the scale
		
	scale=distance_get(view->pnt.x,view->pnt.y,view->pnt.z,mesh->vertexes[vertex_idx].x,mesh->vertexes[vertex_idx].y,mesh->vertexes[vertex_idx].z)/move_mouse_drag_distance_ratio;

		// drag
		
	first_drag=TRUE;
	
	dpt=&mesh->vertexes[vertex_idx];
	memmove(&old_dpt,dpt,sizeof(d3pnt));
	
	view_get_pixel_box(view,&box);

	memmove(&old_pt,pt,sizeof(d3pnt));
	
	mx=my=mz=0;
	
	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}
		
			// get vertex movement

		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		view_mouse_get_scroll_horizontal_axis(view,&move_pnt,-(x*scale));
		view_mouse_get_scroll_vertical_axis(view,&move_pnt,-(y*scale));
			
		mx+=move_pnt.x;
		my+=move_pnt.y;
		mz+=move_pnt.z;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
			// move vertex
			
		dpt->x=old_dpt.x+mpt.x;
		dpt->y=old_dpt.y+mpt.y;
		dpt->z=old_dpt.z+mpt.z;
			
			// grid and snap
			
		view_click_grid(dpt);
		view_click_snap(mesh_idx,vertex_idx,-1,dpt);
	
		if ((state.map.auto_texture) && (!mesh->flag.lock_uv)) map_mesh_reset_uv(&map,mesh_idx);

		view_vbo_mesh_rebuild(mesh_idx);

        main_wind_draw();
	}
	
	os_set_arrow_cursor();
	
		// turn off hilite
		
	state.map.drag_handle_idx=-1;
	
	main_wind_draw();
	
	return(TRUE);
}

/* =======================================================

      Drag Texture UV
      
======================================================= */

bool view_click_drag_texture_uv(editor_view_type *view_setup,d3pnt *pt,bool entire_mesh)
{
	int						n,k,x,y,
							type,mesh_idx,poly_idx;
	bool					first_drag;
	float					gx_add,gy_add;
	d3pnt					old_pt;
	d3rect					box;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	
		// any selection?
		
	if (select_count()==0) return(FALSE);
	
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=item_map_mesh) return(FALSE);
	
	mesh=&map.mesh.meshes[mesh_idx];
	
		// drag
		
    if (!os_button_down()) return(FALSE);
			
	first_drag=TRUE;
	
	view_get_pixel_box(view_setup,&box);
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}
		
			// get UV change

		gx_add=((float)x)/view_texture_shift_factor;
		gy_add=((float)y)/view_texture_shift_factor;
		
			// alter UVs
			
		poly=mesh->polys;

		for (n=0;n!=mesh->npoly;n++) {
		
			if ((entire_mesh) || (n==poly_idx)) {
		
				for (k=0;k!=poly->ptsz;k++) {
					poly->main_uv.uvs[k].x+=gx_add;
					poly->main_uv.uvs[k].y+=gy_add;
				}
				
			}
			
			poly++;
		}

        main_wind_draw();
	}
	
	os_set_arrow_cursor();
	
	return(!first_drag);
}

/* =======================================================

      Drag Liquid Vertex
      
======================================================= */

bool view_click_drag_liquid_vertex(editor_view_type *view,d3pnt *pt)
{
	int						n,x,y,mx,my,mz,scale,chk_x,chk_z,old_depth,
							type,liquid_idx,sub_idx,handle_idx;
	d3pnt					pts[8],old_pt,old_dpt,move_pnt,mpt,dpt;
	d3rect					box;
	bool					first_drag;
	map_liquid_type			*liq;
	
    if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&liquid_idx,&sub_idx);
	if (type!=item_map_liquid) return(FALSE);
	
		// check for clicking handle points

	liq=&map.liquid.liquids[liquid_idx];
	
	map_view_draw_select_liquid_get_grow_handles(liquid_idx,pts);
		
	view_pick_list_start(view,FALSE,8);
	
	for (n=0;n!=8;n++) {
		view_pick_list_add_handle(&pts[n],item_map_liquid,n,0);
	}

	handle_idx=-1;

	view_pick_list_end(view,pt,&type,&handle_idx,&sub_idx);

    if (handle_idx==-1) return(FALSE);
	
		// drag liquid vertex
	
    if (!os_button_down()) return(FALSE);

		// hilite the drag handle
		
	state.map.drag_handle_idx=handle_idx;
	main_wind_draw();

		// get the scale
		
	map_liquid_calculate_center(&map,liquid_idx,&mpt);
	scale=distance_get(view->pnt.x,view->pnt.y,view->pnt.z,mpt.x,mpt.y,mpt.z)/move_mouse_drag_distance_ratio;
		
		// drag
	
	map_undo_push();

	first_drag=TRUE;
	
	old_dpt.x=pts[handle_idx].x;
	old_dpt.y=pts[handle_idx].y;
	old_dpt.z=pts[handle_idx].z;
	
	old_depth=liq->depth;
	
	view_get_pixel_box(view,&box);
	
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	mx=my=mz=0;
	
	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}
		
			// get vertex move (never on Y)

		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		view_mouse_get_scroll_horizontal_axis(view,&move_pnt,-(x*scale));
		view_mouse_get_scroll_vertical_axis(view,&move_pnt,-(y*scale));
			
		mx+=move_pnt.x;
		my+=move_pnt.y;
		mz+=move_pnt.z;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;

			// get vertex
			
		switch (handle_idx) {
			case 0:
			case 4:
				dpt.x=old_dpt.x+mpt.x;
				dpt.y=old_dpt.y+mpt.y;
				dpt.z=old_dpt.z+mpt.z;
				break;
			case 1:
			case 5:
				dpt.x=old_dpt.x+mpt.x;
				dpt.y=old_dpt.y+mpt.y;
				dpt.z=old_dpt.z+mpt.z;
				break;
			case 2:
			case 6:
				dpt.x=old_dpt.x+mpt.x;
				dpt.y=old_dpt.y+mpt.y;
				dpt.z=old_dpt.z+mpt.z;
				break;
			case 3:
			case 7:
				dpt.x=old_dpt.x+mpt.x;
				dpt.y=old_dpt.y+mpt.y;
				dpt.z=old_dpt.z+mpt.z;
				break;
		}

			// grid and snap
		
		view_click_grid(&dpt);
		view_click_snap(-1,-1,liquid_idx,&dpt);
		
			// move vertex
			
		switch (handle_idx) {
			case 0:
				liq->lft=dpt.x;
				liq->y=dpt.y;
				liq->depth=old_depth+(old_dpt.y-dpt.y);
				liq->top=dpt.z;
				break;
			case 1:
				liq->rgt=dpt.x;
				liq->y=dpt.y;
				liq->depth=old_depth+(old_dpt.y-dpt.y);
				liq->top=dpt.z;
				break;
			case 2:
				liq->rgt=dpt.x;
				liq->y=dpt.y;
				liq->depth=old_depth+(old_dpt.y-dpt.y);
				liq->bot=dpt.z;
				break;
			case 3:
				liq->lft=dpt.x;
				liq->y=dpt.y;
				liq->depth=old_depth+(old_dpt.y-dpt.y);
				liq->bot=dpt.z;
				break;
			case 4:
				liq->lft=dpt.x;
				liq->depth=dpt.y-liq->y;
				liq->top=dpt.z;
				break;
			case 5:
				liq->rgt=dpt.x;
				liq->depth=dpt.y-liq->y;
				liq->top=dpt.z;
				break;
			case 6:
				liq->rgt=dpt.x;
				liq->depth=dpt.y-liq->y;
				liq->bot=dpt.z;
				break;
			case 7:
				liq->lft=dpt.x;
				liq->depth=dpt.y-liq->y;
				liq->bot=dpt.z;
				break;
		}
		
		if (liq->depth<100) liq->depth=100;
		
		if ((state.map.auto_texture) && (!liq->flag.lock_uv)) map_liquid_reset_uv(&map,liquid_idx);

        main_wind_draw();
	}

		// fix any left/right swaps
		
	if (liq->lft>liq->rgt) {
		chk_x=liq->lft;
		liq->lft=liq->rgt;
		liq->rgt=chk_x;
	}
	
	if (liq->top>liq->bot) {
		chk_z=liq->top;
		liq->top=liq->bot;
		liq->bot=chk_z;
	}

		// remove hilite

	state.map.drag_handle_idx=-1;
	
	main_wind_draw();
	
	os_set_arrow_cursor();
	
	return(!first_drag);
}

/* =======================================================

      Drag Liquid
      
======================================================= */

bool view_click_drag_liquid(editor_view_type *view,d3pnt *pt)
{
	int						x,y,mx,my,mz,scale,
							old_lft,old_rgt,old_top,old_bot,old_y,
							type,main_idx,sub_idx;
	d3pnt					old_pt,move_pnt,mpt;
	d3rect					box;
	bool					first_drag;
	map_liquid_type			*liq;
	
    if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&main_idx,&sub_idx);
	if (type!=item_map_liquid) return(FALSE);
	
	liq=&map.liquid.liquids[main_idx];
		
		// drag item
	
    if (!os_button_down()) return(FALSE);
	
	map_undo_push();
	
		// get the scale
		
	map_liquid_calculate_center(&map,main_idx,&mpt);
	scale=distance_get(view->pnt.x,view->pnt.y,view->pnt.z,mpt.x,mpt.y,mpt.z)/move_mouse_drag_distance_ratio;

		// run the drag

	first_drag=TRUE;
	
	old_lft=liq->lft;
	old_rgt=liq->rgt;
	old_top=liq->top;
	old_bot=liq->bot;
	old_y=liq->y;
	
	view_get_pixel_box(view,&box);
	
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	mx=my=mz=0;
	
	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}
		
			// get movement

		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		view_mouse_get_scroll_horizontal_axis(view,&move_pnt,-(x*scale));
		view_mouse_get_scroll_vertical_axis(view,&move_pnt,-(y*scale));
			
		mx+=move_pnt.x;
		my+=move_pnt.y;
		mz+=move_pnt.z;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		view_click_grid(&mpt);
		
			// move liquid
			
		liq->lft=old_lft+mpt.x;
		liq->rgt=old_rgt+mpt.x;
		liq->top=old_top+mpt.z;
		liq->bot=old_bot+mpt.z;
		liq->y=old_y+mpt.y;

		if ((state.map.auto_texture) && (!liq->flag.lock_uv)) map_liquid_reset_uv(&map,main_idx);

        main_wind_draw();
	}
	
	os_set_arrow_cursor();
	
	main_wind_draw();
	
	return(!first_drag);
}

/* =======================================================

      Drag Item
      
======================================================= */

bool view_click_drag_item(editor_view_type *view,d3pnt *pt)
{
	int						x,y,mx,my,mz,scale,
							type,main_idx,sub_idx;
	d3pnt					*pnt,old_pt,old_pnt,move_pnt,mpt;
	d3rect					box;
	bool					first_drag;
	
    if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&main_idx,&sub_idx);
	
		// get item pos
		
	pnt=NULL;
	
	switch (type) {
		case item_map_node:
			pnt=&map.nodes[main_idx].pnt;
			break;
		case item_map_spot:
			pnt=&map.spots[main_idx].pnt;
			break;
		case item_map_scenery:
			pnt=&map.sceneries[main_idx].pnt;
			break;
		case item_map_light:
			pnt=&map.lights[main_idx].pnt;
			break;
		case item_map_sound:
			pnt=&map.sounds[main_idx].pnt;
			break;
		case item_map_particle:
			pnt=&map.particles[main_idx].pnt;
			break;
	}
	
	if (pnt==NULL) return(FALSE);
		
		// setup drag
	
    if (!os_button_down()) return(FALSE);
	
	map_undo_push();
		
		// get the scale
		
	scale=distance_get(view->pnt.x,view->pnt.y,view->pnt.z,pnt->x,pnt->y,pnt->z)/move_mouse_drag_distance_ratio;

		// run the drag

	first_drag=TRUE;
	
	view_get_pixel_box(view,&box);
	
	memmove(&old_pt,pt,sizeof(d3pnt));
	memmove(&old_pnt,pnt,sizeof(d3pnt));
	
	mx=my=mz=0;
	
	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}
		
			// get movement

		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		view_mouse_get_scroll_horizontal_axis(view,&move_pnt,-(x*scale));
		view_mouse_get_scroll_vertical_axis(view,&move_pnt,-(y*scale));
			
		mx+=move_pnt.x;
		my+=move_pnt.y;
		mz+=move_pnt.z;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		view_click_grid(&mpt);
		
		pnt->x=old_pnt.x+mpt.x;
		pnt->y=old_pnt.y+mpt.y;
		pnt->z=old_pnt.z+mpt.z;

        main_wind_draw();
	}
	
	os_set_arrow_cursor();
	
	main_wind_draw();
	
	return(!first_drag);
}

/* =======================================================

      Box Select
      
======================================================= */

bool view_click_box_select(editor_view_type *view,d3pnt *pt)
{
	int					n,x,y,item_count;
	bool				shift,moved;
	d3pnt				old_pt;
	d3rect				box;
	view_picker_type	*pick_list;
		
	view_get_pixel_box(view,&box);
	
		// draw map and get pixels
		
	view_click_piece_map_pick_start(view);
	if (!view_pick_list_multiple_setup(view)) {	
		view_pick_list_multiple_end();
		return(FALSE);
	}

		// setup the start point

	state.map.select_box_on=TRUE;
	memmove(&state.map.select_box_start_pnt,pt,sizeof(d3pnt));

		// if shift, then backup current selection

	shift=os_key_shift_down();
	if (shift) select_duplicate_backup();

		// memory for picks

	pick_list=(view_picker_type*)malloc(sizeof(view_picker_type)*select_max_item);
	if (pick_list==NULL) return(FALSE);
	
		// run the select

	moved=FALSE;
	memmove(&old_pt,pt,sizeof(d3pnt));

	while (!os_track_mouse_location(pt,&box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		moved=TRUE;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// setup the end point

		memmove(&state.map.select_box_end_pnt,pt,sizeof(d3pnt));

			// select any items
			
		item_count=view_pick_list_multiple_pick(view,&state.map.select_box_start_pnt,&state.map.select_box_end_pnt,pick_list);
		
		if (!shift) {
			select_clear();
		}
		else {
			select_duplicate_copy();
		}
		
		for (n=0;n!=item_count;n++) {
			if (state.map.select_mode==select_mode_mesh) {
				select_add(pick_list[n].type,pick_list[n].main_idx,-1);
			}
			else {
				select_add(pick_list[n].type,pick_list[n].main_idx,pick_list[n].sub_idx);
			}
		}

			// redraw the selection

        main_wind_draw();
	}

		// end picking

	view_pick_list_multiple_end();

	free(pick_list);

		// clear the select box

	state.map.select_box_on=FALSE;
	
	main_wind_draw();
	
	return(moved);
}