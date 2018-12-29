/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Piece Click Routines

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern map_type					map;
extern app_state_type			state;
extern app_pref_type			pref;

extern list_palette_type		map_palette;

/* =======================================================

      Grid and Snap
      
======================================================= */

int view_get_grid(void)
{
	switch (state.map.grid_mode) {
		case grid_mode_small:
			return(500);
		case grid_mode_large:
			return(2000);
	}

	return(1);
}

void view_click_grid(d3pnt *pt)
{
	int			sz;
	
	sz=view_get_grid();
	
	pt->x/=sz;
	pt->y/=sz;
	pt->z/=sz;
	
	pt->x*=sz;
	pt->y*=sz;
	pt->z*=sz;
}

void view_force_grid(int mesh_idx,bool pos_only)
{
	int					k,grid_sz;
	d3pnt				min,max,mov_pnt;
	map_mesh_type		*mesh;

		// get box

	mesh=&map.mesh.meshes[mesh_idx];
	map_prepare_mesh_box(mesh);

		// push to grid start

	grid_sz=view_get_grid();
	if (grid_sz<1) return;

	k=mesh->box.min.x/grid_sz;
	min.x=k*grid_sz;

	k=mesh->box.min.y/grid_sz;
	min.y=k*grid_sz;

	k=mesh->box.min.z/grid_sz;
	min.z=k*grid_sz;

		// if pos only, only move
		// position

	if (pos_only) {
		mov_pnt.x=min.x-mesh->box.min.x;
		mov_pnt.y=min.y-mesh->box.min.y;
		mov_pnt.z=min.z-mesh->box.min.z;

		map_mesh_move(&map,mesh_idx,&mov_pnt);
		view_vbo_mesh_rebuild(mesh_idx);
		return;
	}

		// if not pos only, then
		// make sure bound box is grid sized

	k=(mesh->box.max.x-mesh->box.min.x)/grid_sz;
	max.x=min.x+(k*grid_sz);

	k=(mesh->box.max.y-mesh->box.min.y)/grid_sz;
	max.y=min.y+(k*grid_sz);

	k=(mesh->box.max.z-mesh->box.min.z)/grid_sz;
	max.z=min.z+(k*grid_sz);

		// reset mesh

	map_mesh_resize(&map,mesh_idx,&min,&max);
	view_vbo_mesh_rebuild(mesh_idx);

		// handle any cascades

	view_click_fix_cascade_size(mesh_idx);
}

/* =======================================================

      Snap Vertexes and Polys
      
======================================================= */

bool view_click_snap(int mesh_idx,int vertex_idx,int liquid_idx,d3pnt *pt)
{
	int				n,t,y,snap_sz;
	d3pnt			*dpt;
	map_mesh_type	*mesh;
	map_liquid_type	*liq;
	
	if (state.map.vertex_mode!=vertex_mode_snap) return(FALSE);
	
		// snap size
		
	snap_sz=(pref.snap_size*view_snap_clip_size_factor);
	
		// any mesh vertexes to snap to?
		
	mesh=map.mesh.meshes;
		
	for (n=0;n!=map.mesh.nmesh;n++) {
		
		dpt=mesh->vertexes;
		
		for (t=0;t!=mesh->nvertex;t++) {
			
				// not to itself
				
			if ((n==mesh_idx) && (t==vertex_idx)) {
				dpt++;
				continue;
			}
		
				// check distance
				
			if (abs(dpt->x-pt->x)>snap_sz) {
				dpt++;
				continue;
			}
			
			if (abs(dpt->z-pt->z)>snap_sz) {
				dpt++;
				continue;
			}
			
			if (abs(dpt->y-pt->y)>snap_sz) {
				dpt++;
				continue;
			}
		
			if (distance_get(dpt->x,dpt->y,dpt->z,pt->x,pt->y,pt->z)<snap_sz) {
				memmove(pt,dpt,sizeof(d3pnt));
				return(TRUE);
			}
			
			dpt++;
		}
	
		mesh++;
	}

		// any liquid vertexes to snap to?

	liq=map.liquid.liquids;

	for (n=0;n!=map.liquid.nliquid;n++) {

		if (n==liquid_idx) {
			liq++;
			continue;
		}

		if (distance_get(liq->lft,liq->y,liq->top,pt->x,pt->y,pt->z)<snap_sz) {
			pt->x=liq->lft;
			pt->y=liq->y;
			pt->z=liq->top;
			return(TRUE);
		}

		if (distance_get(liq->rgt,liq->y,liq->top,pt->x,pt->y,pt->z)<snap_sz) {
			pt->x=liq->rgt;
			pt->y=liq->y;
			pt->z=liq->top;
			return(TRUE);
		}

		if (distance_get(liq->lft,liq->y,liq->bot,pt->x,pt->y,pt->z)<snap_sz) {
			pt->x=liq->lft;
			pt->y=liq->y;
			pt->z=liq->bot;
			return(TRUE);
		}

		if (distance_get(liq->rgt,liq->y,liq->bot,pt->x,pt->y,pt->z)<snap_sz) {
			pt->x=liq->rgt;
			pt->y=liq->y;
			pt->z=liq->bot;
			return(TRUE);
		}

		y=liq->y+liq->depth;

		if (distance_get(liq->lft,y,liq->top,pt->x,pt->y,pt->z)<snap_sz) {
			pt->x=liq->lft;
			pt->y=y;
			pt->z=liq->top;
			return(TRUE);
		}

		if (distance_get(liq->rgt,y,liq->top,pt->x,pt->y,pt->z)<snap_sz) {
			pt->x=liq->rgt;
			pt->y=y;
			pt->z=liq->top;
			return(TRUE);
		}

		if (distance_get(liq->lft,y,liq->bot,pt->x,pt->y,pt->z)<snap_sz) {
			pt->x=liq->lft;
			pt->y=y;
			pt->z=liq->bot;
			return(TRUE);
		}

		if (distance_get(liq->rgt,y,liq->bot,pt->x,pt->y,pt->z)<snap_sz) {
			pt->x=liq->rgt;
			pt->y=y;
			pt->z=liq->bot;
			return(TRUE);
		}
		
		liq++;
	}
	
	return(FALSE);
}

bool view_click_snap_poly(int mesh_idx,int poly_idx,d3pnt *pt)
{
	int					n,d,cur_dist;
	d3pnt				hpt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	if (state.map.vertex_mode!=vertex_mode_snap) return(FALSE);
	
	cur_dist=-1;
	
	mesh=&map.mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
	for (n=0;n!=poly->ptsz;n++) {
		memmove(&hpt,pt,sizeof(d3pnt));
		
		if (view_click_snap(mesh_idx,poly->v[n],-1,&hpt)) {
			d=distance_get(pt->x,pt->y,pt->z,hpt.x,hpt.y,hpt.z);
			if (d>(pref.snap_size*view_snap_clip_size_factor)) continue;
			
			if ((d<cur_dist) || (cur_dist<0)) {
				cur_dist=d;
				memmove(pt,&hpt,sizeof(d3pnt));
			}
		}
	}
	
	return(cur_dist!=-1);
}

/* =======================================================

      Snap Meshes
      
======================================================= */

bool view_click_snap_mesh_vertexes(int mesh_idx,d3pnt *pnt)
{
	int					n,d,cur_dist;
	d3pnt				hpnt;
	map_mesh_type		*mesh;

	cur_dist=-1;
	
	mesh=&map.mesh.meshes[mesh_idx];
	
	for (n=0;n!=mesh->nvertex;n++) {
		memmove(&hpnt,pnt,sizeof(d3pnt));
		
		if (view_click_snap(mesh_idx,n,-1,&hpnt)) {
			d=distance_get(pnt->x,pnt->y,pnt->z,hpnt.x,hpnt.y,hpnt.z);
			if (d>(pref.snap_size*view_snap_clip_size_factor)) continue;
			
			if ((d<cur_dist) || (cur_dist<0)) {
				cur_dist=d;
				memmove(pnt,&hpnt,sizeof(d3pnt));
			}
		}
	}
	
	return(cur_dist!=-1);
}

int view_click_get_snap_count(d3pnt *old_dpnt,d3pnt *mpnt)
{
	int				n,k,t,i,nsel,type,count,
					mesh_idx,poly_idx;
	bool			hit;
	d3pnt			*old_dpnt_ptr;
	d3pnt			spnt;
	map_mesh_type	*mesh,*mesh2;

	count=0;
	old_dpnt_ptr=old_dpnt;

	nsel=select_count();
	
	for (k=0;k!=nsel;k++) {
		select_get(k,&type,&mesh_idx,&poly_idx);
		if (type!=item_map_mesh) continue;
		
		mesh=&map.mesh.meshes[mesh_idx];
	
		for (n=0;n!=mesh->nvertex;n++) {
			spnt.x=old_dpnt_ptr->x+mpnt->x;
			spnt.y=old_dpnt_ptr->y+mpnt->y;
			spnt.z=old_dpnt_ptr->z+mpnt->z;

				// count any collision
				// skip any meshes within selection

			hit=FALSE;

			for (i=0;i!=map.mesh.nmesh;i++) {
				if (i==n) continue;
				if (!select_check(item_map_mesh,i,-1)) continue;

				mesh2=&map.mesh.meshes[i];

				for (t=0;t!=mesh2->nvertex;t++) {
					if (spnt.x!=mesh2->vertexes[t].x) continue;
					if (spnt.y!=mesh2->vertexes[t].y) continue;
					if (spnt.z!=mesh2->vertexes[t].z) continue;

					hit=TRUE;
					break;
				}

				if (hit) break;
			}

			if (hit) count++;
		}
	}

	return(count);
}

void view_click_snap_mesh(d3pnt *old_dpnt,d3pnt *mpnt)
{
	int				n,k,nsel,type,
					snap_count,snap_min_count,
					mesh_idx,poly_idx;
	d3pnt			*old_dpnt_ptr;
	d3pnt			spnt,mv_pnt,cur_mv_pnt;
	map_mesh_type	*mesh;

	if (state.map.vertex_mode!=vertex_mode_snap) return;

		// check all vertexes in all meshes
		// in the selection for snaps.  Anything that
		// can snap is compared by the total
		// number of vetexes snapped, the one with the
		// least is the snap target

	snap_min_count=9999;
	
	old_dpnt_ptr=old_dpnt;

	nsel=select_count();
	
	for (k=0;k!=nsel;k++) {
		select_get(k,&type,&mesh_idx,&poly_idx);
		if (type!=item_map_mesh) continue;
		
		mesh=&map.mesh.meshes[mesh_idx];
	
		for (n=0;n!=mesh->nvertex;n++) {
			spnt.x=old_dpnt_ptr->x+mpnt->x;
			spnt.y=old_dpnt_ptr->y+mpnt->y;
			spnt.z=old_dpnt_ptr->z+mpnt->z;

				// can this vertex snap?
			
			if (view_click_snap_mesh_vertexes(mesh_idx,&spnt)) {

					// get the new snap point

				mv_pnt.x=spnt.x-old_dpnt_ptr->x;
				mv_pnt.y=spnt.y-old_dpnt_ptr->y;
				mv_pnt.z=spnt.z-old_dpnt_ptr->z;

					// get the snap count

				snap_count=view_click_get_snap_count(old_dpnt,mpnt);

				if (snap_count<snap_min_count) {
					snap_min_count=snap_count;
					memmove(&cur_mv_pnt,&mv_pnt,sizeof(d3pnt));
				}
			}
			
			old_dpnt_ptr++;
		}
	}

		// if we had a good snap,
		// update the move

	if (snap_min_count!=9999) memmove(mpnt,&cur_mv_pnt,sizeof(d3pnt));
}

/* =======================================================

      Rot Handle Rotation Run
      
======================================================= */

void view_click_rot_handle_rotate_run_axis(float *value,float org_value,float ang_add)
{
	int				k;
	
	*value=angle_add(org_value,ang_add);
	
	if (os_key_shift_down()) {
		k=(int)*value;
		k/=45;
		*value=(float)(k*45);
	}
}

void view_click_rot_handle_rotate_run(int type,int idx,d3ang *ang,d3ang *org_ang,d3pnt *mesh_center_pnt,float ang_add,int which_axis)
{
	d3pnt			mov_pnt;
	d3ang			rot_ang;

		// run the rotate

	switch (which_axis) {
		case 0:
			view_click_rot_handle_rotate_run_axis(&ang->x,org_ang->x,ang_add);
			break;
		case 1:
			view_click_rot_handle_rotate_run_axis(&ang->y,org_ang->y,ang_add);
			break;
		case 2:
			view_click_rot_handle_rotate_run_axis(&ang->z,org_ang->z,ang_add);
			break;
	}

		// if it's a mesh, then
		// we need to move the mesh

	if (type==item_map_mesh) {
		mov_pnt.x=mov_pnt.y=mov_pnt.z=0;
		rot_ang.x=org_ang->x-ang->x;
		rot_ang.y=org_ang->y-ang->y;
		rot_ang.z=org_ang->z-ang->z;
		map_mesh_move_rotate_copy(&map,idx,mesh_center_pnt,&mov_pnt,&rot_ang);
		view_vbo_mesh_rebuild(idx);
	}
}

/* =======================================================

      Rot Handle Move Run
      
======================================================= */

void view_click_rot_handle_move_run_axis(int *value,int org_value,int mv_add)
{
	*value=org_value+mv_add;
}

void view_click_rot_handle_move_run(int type,int idx,d3pnt *pnt,d3pnt *org_pnt,int mv_add,int scale,int which_axis)
{
	d3pnt			mesh_pnt,mov_pnt;
	d3ang			rot_ang;

		// run the move

	mv_add=-(mv_add*scale);
	
	switch (which_axis) {
		case 0:
			view_click_rot_handle_move_run_axis(&pnt->x,org_pnt->x,mv_add);
			break;
		case 1:
			view_click_rot_handle_move_run_axis(&pnt->y,org_pnt->y,mv_add);
			break;
		case 2:
			view_click_rot_handle_move_run_axis(&pnt->z,org_pnt->z,mv_add);
			break;
	}

	view_click_grid(pnt);

		// if it's a mesh, then
		// we need to move the mesh

	if (type==item_map_mesh) {
		mov_pnt.x=pnt->x-org_pnt->x;
		mov_pnt.y=pnt->y-org_pnt->y;
		mov_pnt.z=pnt->z-org_pnt->z;
		rot_ang.x=rot_ang.y=rot_ang.z=0.0f;
		map_mesh_calculate_center(&map,idx,&mesh_pnt);
		map_mesh_move_rotate_copy(&map,idx,&mesh_pnt,&mov_pnt,&rot_ang);
		view_vbo_mesh_rebuild(idx);
	}
}

/* =======================================================

      Rot Handle Clicking Main
      
======================================================= */

bool view_click_rot_handles(editor_view_type *view,d3pnt *click_pt)
{
	int			n,k,idx,scale,mv,
				type,main_idx,sub_idx,
				which_axis,sel_count,item_count;
	int			*type_list,*idx_list;
	bool		first_drag;
	d3pnt		pt,center_pnt,mesh_pnt,
				*hand_pnts,*pnt,org_pnt;
	d3ang		mesh_ang,*ang,org_ang;
	d3rect		box;

		// need to create the items outside
		// of the pick call so rendering doesn't get
		// reset on us
		
		// first count the items

	item_count=0;
	sel_count=select_count();

	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&main_idx,&sub_idx);
		if ((type==item_map_mesh) || (type==item_map_node) || (type==item_map_spot) || (type==item_map_scenery)) item_count++;
	}

	if (item_count==0) return(FALSE);
	
		// build the item list
		
	hand_pnts=(d3pnt*)malloc(sizeof(d3pnt)*(item_count*3));
	type_list=(int*)malloc(sizeof(int)*item_count);
	idx_list=(int*)malloc(sizeof(int)*item_count);
	
		// get the handles
		
	item_count=0;
	
	for (n=0;n!=sel_count;n++) {
	
		select_get(n,&type,&main_idx,&sub_idx);
		
		switch (type) {

			case item_map_mesh:
				map_mesh_calculate_center(&map,main_idx,&mesh_pnt);
				if (view_handle_create_rot_handle(view,&mesh_pnt,NULL,&center_pnt,&hand_pnts[item_count*3])) {
					type_list[item_count]=type;
					idx_list[item_count]=main_idx;
					item_count++;
				}
				break;
		
			case item_map_node:
				if (view_handle_create_rot_handle(view,&map.nodes[main_idx].pnt,&map.nodes[main_idx].ang,&center_pnt,&hand_pnts[item_count*3])) {
					type_list[item_count]=type;
					idx_list[item_count]=main_idx;
					item_count++;
				}
				break;
				
			case item_map_spot:
				if (view_handle_create_rot_handle(view,&map.spots[main_idx].pnt,&map.spots[main_idx].ang,&center_pnt,&hand_pnts[item_count*3])) {
					type_list[item_count]=type;
					idx_list[item_count]=main_idx;
					item_count++;
				}
				break;
				
			case item_map_scenery:
				if (view_handle_create_rot_handle(view,&map.sceneries[main_idx].pnt,&map.sceneries[main_idx].ang,&center_pnt,&hand_pnts[item_count*3])) {
					type_list[item_count]=type;
					idx_list[item_count]=main_idx;
					item_count++;
				}
				break;

		}
	}
	
	if (item_count==0) {
		free(hand_pnts);
		free(type_list);
		free(idx_list);
		return(FALSE);
	}

		// run pick list for handles
	
	view_pick_list_start(view,TRUE,(item_count*3));

	for (n=0;n!=item_count;n++) {
		idx=n*3;
		for (k=0;k!=3;k++) {
			view_pick_list_add_2D_handle(&hand_pnts[idx],type_list[n],idx_list[n],k);
			idx++;
		}
	}
	
	free(hand_pnts);
	free(type_list);
	free(idx_list);
	
	view_pick_list_end(view,click_pt,&type,&main_idx,&which_axis);
	if (type==-1) return(FALSE);

		// setup the drag

	switch (type) {

		case item_map_mesh:
			map_mesh_move_rotate_copy_reset(&map,main_idx);		// reset move-rotate copy so we start at current position/rotation
			map_mesh_calculate_center(&map,main_idx,&mesh_pnt);
			pnt=&mesh_pnt;
			mesh_ang.x=mesh_ang.y=mesh_ang.z=0.0f;
			ang=&mesh_ang;
			break;
	
		case item_map_node:
			pnt=&map.nodes[main_idx].pnt;
			ang=&map.nodes[main_idx].ang;
			break;
			
		case item_map_spot:
			pnt=&map.spots[main_idx].pnt;
			ang=&map.spots[main_idx].ang;
			break;
			
		case item_map_scenery:
			pnt=&map.sceneries[main_idx].pnt;
			ang=&map.sceneries[main_idx].ang;
			break;
	}

	memmove(&org_pnt,pnt,sizeof(d3pnt));
	memmove(&org_ang,ang,sizeof(d3ang));
	
		// get the scale
		
	scale=distance_get(view->pnt.x,view->pnt.y,view->pnt.z,pnt->x,pnt->y,pnt->z)/move_mouse_drag_distance_ratio;
	
		// handle drag
	
    if (!os_button_down()) return(FALSE);
	
	map_undo_push();
	
	first_drag=TRUE;
	
	view_get_pixel_box(view,&box);
	
	while (!os_track_mouse_location(&pt,&box)) {
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}
		
			// get biggest move
			
		mv=click_pt->x-pt.x;
		if (abs(mv)<abs(click_pt->y-pt.y)) mv=click_pt->y-pt.y;

			// handle movement
			
		if (state.map.handle_mode==handle_mode_rotate) {
			view_click_rot_handle_rotate_run(type,main_idx,ang,&org_ang,&mesh_pnt,(float)mv,which_axis);
		}
		else {
			view_click_rot_handle_move_run(type,main_idx,pnt,&org_pnt,mv,scale,which_axis);
		}
		
        main_wind_draw();
	}
	
	os_set_arrow_cursor();
	
	return(!first_drag);
}

/* =======================================================

      Run Pick List for Map Items
      
======================================================= */

void view_click_piece_map_pick_start(editor_view_type *view)
{
	int					n,k,t,count;
	float				vertexes[8*3];
	float				*pv;
	d3pnt				*pt;
	d3pnt				v_pnts[8],min,max;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	map_liquid_type		*liq;
	spot_type			*spot;
	map_scenery_type	*scenery;
	map_light_type		*map_light;
	map_sound_type		*map_sound;
	map_particle_type	*map_particle;
	node_type			*node;

		// get picker count
		// we create a list that marks the color
		// for each item we are going to pick
		// from

	count=0;

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
		count+=mesh->npoly;
		mesh++;
	}
	
	if (state.map.show_liquid) count+=map.liquid.nliquid;
	
	if (state.map.show_object) {
		count+=map.nspot;
		count+=map.nscenery;
	}
	
	if (state.map.show_lightsoundparticle) {
		count+=map.nlight;
		count+=map.nsound;
		count+=map.nparticle;
	}
	
	if (state.map.show_node) count+=map.nnode;

		// start the pick list

	view_pick_list_start(view,FALSE,count);

		// run through all the items
		// in the map and draw them to the
		// back buffer with unique colors

		// meshes
		
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
	
			// clip meshes to view
			
		map_mesh_calculate_extent(&map,n,&min,&max);
		if (!view_cull_boundbox_in_frustum(&min,&max)) {
			mesh++;
			continue;
		}
		
			// run through polys

		poly=mesh->polys;
	
		for (k=0;k!=mesh->npoly;k++) {
					
				// clipping
				
			if (view_clip_poly(view,poly)) {
				poly++;
				continue;
			}
			
				// clip poly to view
				
			map_mesh_poly_calculate_extent(&map,n,k,&min,&max);
			if (!view_cull_boundbox_in_frustum(&min,&max)) {
				poly++;
				continue;
			}
			
				// draw color poly

			view_pick_list_add(item_map_mesh,n,k);

			pv=vertexes;
			
			for (t=0;t!=poly->ptsz;t++) {
				pt=&mesh->vertexes[poly->v[t]];
				*pv++=(float)pt->x;
				*pv++=(float)pt->y;
				*pv++=(float)pt->z;
			}
			
			glVertexPointer(3,GL_FLOAT,0,vertexes);
			glDrawArrays(GL_POLYGON,0,poly->ptsz);

			poly++;
		}

		mesh++;
	}

		// liquids
		
	if (state.map.show_liquid) {

		liq=map.liquid.liquids;
		
		for (n=0;n!=map.liquid.nliquid;n++) {

					// clipping
				
			if (view_clip_liquid(view,liq)) {
				liq++;
				continue;
			}

				// add to pick list

			view_pick_list_add(item_map_liquid,n,-1);

			vertexes[0]=vertexes[9]=(float)liq->lft;
			vertexes[3]=vertexes[6]=(float)liq->rgt;
			vertexes[1]=vertexes[4]=vertexes[7]=vertexes[10]=(float)liq->y;
			vertexes[2]=vertexes[5]=(float)liq->top;
			vertexes[8]=vertexes[11]=(float)liq->bot;

			glVertexPointer(3,GL_FLOAT,0,vertexes);
			glDrawArrays(GL_QUADS,0,4);

			vertexes[0]=vertexes[9]=(float)liq->lft;
			vertexes[3]=vertexes[6]=(float)liq->rgt;
			vertexes[1]=vertexes[4]=vertexes[7]=vertexes[10]=(float)(liq->y+liq->depth);
			vertexes[2]=vertexes[5]=(float)liq->top;
			vertexes[8]=vertexes[11]=(float)liq->bot;

			glVertexPointer(3,GL_FLOAT,0,vertexes);
			glDrawArrays(GL_QUADS,0,4);

			liq++;
		}
	}

		// objects and scenery

	if (state.map.show_object) {

		spot=map.spots;

		for (n=0;n!=map.nspot;n++) {
			map_view_model_cube_vertexes(spot->display_model,&spot->pnt,&spot->ang,1.0f,v_pnts);
			view_pick_list_add_cube(v_pnts,item_map_spot,n,-1);
			spot++;
		}
			
		scenery=map.sceneries;

		for (n=0;n!=map.nscenery;n++) {
			map_view_model_cube_vertexes(scenery->model_name,&scenery->pnt,&scenery->ang,scenery->resize,v_pnts);
			view_pick_list_add_cube(v_pnts,item_map_scenery,n,-1);
			scenery++;
		}

	}

		// lights, sounds, and particles

	if (state.map.show_lightsoundparticle) {
	
		map_light=map.lights;

		for (n=0;n!=map.nlight;n++) {
			view_get_sprite_vertexes(&map_light->pnt,NULL,v_pnts);
			view_pick_list_add_cube(v_pnts,item_map_light,n,-1);
			map_light++;
		}
		
		map_sound=map.sounds;

		for (n=0;n!=map.nsound;n++) {
			view_get_sprite_vertexes(&map_sound->pnt,NULL,v_pnts);
			view_pick_list_add_cube(v_pnts,item_map_sound,n,-1);
			map_sound++;
		}
		
		map_particle=map.particles;

		for (n=0;n!=map.nparticle;n++) {
			view_get_sprite_vertexes(&map_particle->pnt,NULL,v_pnts);
			view_pick_list_add_cube(v_pnts,item_map_particle,n,-1);
			map_particle++;
		}
	}

		// nodes

	if (state.map.show_node) {
		node=map.nodes;

		for (n=0;n!=map.nnode;n++) {
			view_get_sprite_vertexes(&node->pnt,NULL,v_pnts);
			view_pick_list_add_cube(v_pnts,item_map_node,n,-1);
			node++;
		}
	}
}
	
/* =======================================================

      View Map Clicking
      
======================================================= */

void view_click_piece(editor_view_type *view,d3pnt *pt,bool double_click)
{
	int				type,main_idx,sub_idx,org_node_idx;
	d3rect			box;
	
       // 3D view
        
	view_set_viewport(view,TRUE,TRUE);
	view_set_3D_projection(view,map.editor_setup.view_near_dist,map.editor_setup.view_far_dist,view_near_offset);
	
	view_cull_setup_frustum_clipping_planes();

		// convert point to view

	view_get_pixel_box(view,&box);

	pt->x-=box.lx;
	pt->y-=box.ty;

		// rotation handles

	if (view_click_rot_handles(view,pt)) return;
	
		// liquid vertex drags
		
	if (view_click_drag_liquid_vertex(view,pt)) return;
	
		// mesh vertex drags
		
	switch (state.map.select_mode) {
	
		case select_mode_vertex:
			if (view_click_drag_vertex(view,pt)) return;
			break;
	
		case select_mode_mesh:
			if (view_click_drag_mesh_handle(view,pt)) return;
			break;
			
	}

		// pick clicked map item (mesh, liquid, nodes, etc)
		
	view_click_piece_map_pick_start(view);
	view_pick_list_end(view,pt,&type,&main_idx,&sub_idx);
	
		// special normal flip if in normal hide 
		// mode and polygon selection
		
	if ((type==item_map_mesh) && (view->cull) && (state.map.show_normals) && (state.map.select_mode==select_mode_polygon)) {
		piece_mesh_poly_invert_normals(&map.mesh.meshes[main_idx].polys[sub_idx]);
	}
	
		// if a node, run special node
		// click types
		
	if (type==item_map_node) {
	
		switch (state.map.node_mode) {
		
			case node_mode_duplicate:
				org_node_idx=main_idx;
				main_idx=node_duplicate_and_drag(main_idx);
				if (main_idx==-1) return;
				break;
		
			case node_mode_link:
			case node_mode_remove_link:
				if (node_link_click(main_idx)) {
					main_wind_draw();
					return;
				}
				break;
		}
		
	}
	
		// drag selects or toggle selection
		
	if (type==-1) {
		if (!view_click_box_select(view,pt)) {
			if (!state.map.select_add) select_clear();
		}
	}
	else {
		if ((!state.map.select_add) && (!os_key_shift_down())) {
			if (!select_check(type,main_idx,sub_idx)) {			// keep selection if selecting an already selected piece
				select_clear();	
				select_add(type,main_idx,sub_idx);
			}
		}
		else {
			select_flip(type,main_idx,sub_idx);
		}
	}
	
		// redraw and reset palettes and menus

	main_wind_menu_update();
	
	map_texture_palette_reset();
	map_palette_reset();

	if (type!=-1) list_palette_set_level(&map_palette,1);

	main_wind_draw();

		// if no item or item was toggled off,
		// nothing to drag

	if (type==-1) return;
	if (!select_check(type,main_idx,sub_idx)) return;

		// item (spots, lights, sounds, etc) drags

	if (view_click_drag_item(view,pt)) {
		
			// special check for node piece,
			// if we are duplicate dragging, reselect
			// old node so it's easy to click again
			
		if ((type==item_map_node) && (state.map.node_mode==node_mode_duplicate)) {
			select_clear();
			select_add(item_map_node,org_node_idx,-1);
			main_wind_draw();
		}
		
		return;
	}
	
		// liquid drags
		
	if (view_click_drag_liquid(view,pt)) return;
	
		// mesh or poly drags

	switch (state.map.select_mode) {
	
		case select_mode_mesh:
			if (!os_key_control_down()) {
				view_click_drag_mesh(view,pt);
			}
			else {
				view_click_drag_texture_uv(view,pt,TRUE);
			}
			break;

		case select_mode_polygon:
			if (!os_key_control_down()) {
				view_click_drag_mesh_poly(view,pt);
			}
			else {
				view_click_drag_texture_uv(view,pt,FALSE);
			}
			break;
			
	}
}



