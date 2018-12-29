/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Selection Drawing Routines

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

/* =======================================================

      Draw Selection for Mesh
      
======================================================= */

void map_view_draw_select_mesh_get_grow_handles(int mesh_idx,d3pnt *pts)
{
	d3pnt			min,max;
	
	map_mesh_calculate_extent(&map,mesh_idx,&min,&max);
	
		// resize

	pts[0].x=pts[3].x=pts[4].x=pts[7].x=min.x;
	pts[0].z=pts[1].z=pts[4].z=pts[5].z=min.z;
	pts[1].x=pts[2].x=pts[5].x=pts[6].x=max.x;
	pts[2].z=pts[3].z=pts[6].z=pts[7].z=max.z;
	pts[0].y=pts[1].y=pts[2].y=pts[3].y=min.y;
	pts[4].y=pts[5].y=pts[6].y=pts[7].y=max.y;

		// skew x

	pts[8].x=pts[9].x=min.x;
	pts[10].x=pts[11].x=max.x;
	pts[8].y=pts[10].y=min.y;
	pts[9].y=pts[11].y=max.y;
	pts[8].z=pts[9].z=pts[10].z=pts[11].z=(min.z+max.z)>>1;

		// skew y

	pts[12].y=pts[13].y=min.y;
	pts[14].y=pts[15].y=max.y;
	pts[12].z=pts[14].z=min.z;
	pts[13].z=pts[15].z=max.z;
	pts[12].x=pts[13].x=pts[14].x=pts[15].x=(min.x+max.x)>>1;

		// skew z

	pts[16].z=pts[17].z=min.z;
	pts[18].z=pts[19].z=max.z;
	pts[16].x=pts[18].x=min.x;
	pts[17].x=pts[19].x=max.x;
	pts[16].y=pts[17].y=pts[18].y=pts[19].y=(min.y+max.y)>>1;
}

void map_view_draw_select_mesh(int mesh_idx)
{
	int						n,k;
	float					vertexes[12*3];
	float					*pv;
	d3pnt					pts[20];
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	
	mesh=&map.mesh.meshes[mesh_idx];
	
		// draw selected mesh
		
	glEnable(GL_DEPTH_TEST);

	glColor4f(pref.col.mesh_sel.r,pref.col.mesh_sel.g,pref.col.mesh_sel.b,1.0f);

	glBindBuffer(GL_ARRAY_BUFFER,mesh->vbo.vertex);
	glVertexPointer(3,GL_FLOAT,((3+2+2)*sizeof(float)),(GLvoid*)0);
		
	poly=mesh->polys;
	
	for (k=0;k!=mesh->npoly;k++) {
		glDrawArrays(GL_LINE_LOOP,poly->draw.vertex_offset,poly->ptsz);
		poly++;
	}

	glBindBuffer(GL_ARRAY_BUFFER,0);
	
		// is mesh only, draw resize handles
		
	if (state.map.select_mode==select_mode_mesh) {
		
		map_view_draw_select_mesh_get_grow_handles(mesh_idx,pts);

			// draw the resize handles

		glEnable(GL_DEPTH_TEST);

		glPointSize(view_handle_size);
		
		pv=vertexes;

		for (n=0;n!=8;n++) {
			if (state.map.drag_handle_idx==n) {
				glColor4f(pref.col.poly_sel.r,pref.col.poly_sel.g,pref.col.poly_sel.b,1.0f);
			}
			else {
				glColor4f(pref.col.mesh_sel.r,pref.col.mesh_sel.g,pref.col.mesh_sel.b,1.0f);
			}
			
			*pv++=(float)pts[n].x;
			*pv++=(float)pts[n].y;
			*pv++=(float)pts[n].z;
		}

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_POINTS,0,8);

			// draw the skew handles

		glPointSize(view_skew_handle_size);

		pv=vertexes;
		
		for (n=8;n!=20;n++) {
			if (state.map.drag_handle_idx==n) {
				glColor4f(pref.col.poly_sel.r,pref.col.poly_sel.g,pref.col.poly_sel.b,1.0f);
			}
			else {
				glColor4f(pref.col.mesh_sel.r,pref.col.mesh_sel.g,pref.col.mesh_sel.b,1.0f);
			}

			*pv++=(float)pts[n].x;
			*pv++=(float)pts[n].y;
			*pv++=(float)pts[n].z;
		}

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_POINTS,0,12);

		glPointSize(1.0f);
	}
}

void map_view_draw_select_mesh_vertex(int mesh_idx)
{
	int						n;
	float					vertexes[8];
	d3pnt					*pt;
	map_mesh_type			*mesh;
	
	glEnable(GL_DEPTH_TEST);

	glPointSize(view_handle_size);
	
	mesh=&map.mesh.meshes[mesh_idx];
	pt=mesh->vertexes;
	
	for (n=0;n!=mesh->nvertex;n++) {
	
		if (state.map.drag_handle_idx==n) {
			glColor4f(pref.col.poly_sel.r,pref.col.poly_sel.g,pref.col.poly_sel.b,1.0f);
		}
		else {
			glColor4f(pref.col.mesh_sel.r,pref.col.mesh_sel.g,pref.col.mesh_sel.b,1.0f);
		}
		
		vertexes[0]=(float)pt->x;
		vertexes[1]=(float)pt->y;
		vertexes[2]=(float)pt->z;
		
		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_POINTS,0,1);
		
		pt++;
	}
}

void map_view_draw_select_mesh_poly(int mesh_idx,int poly_idx)
{
	int						n;
	float					vertexes[8*3];
	float					*pv;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	
	glDisable(GL_DEPTH_TEST);
	
	glLineWidth(2.0f);

	mesh=&map.mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
	pv=vertexes;
	
	for (n=0;n!=poly->ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		*pv++=(float)pt->x;
		*pv++=(float)pt->y;
		*pv++=(float)pt->z;
	}
	
	glVertexPointer(3,GL_FLOAT,0,vertexes);
	
	glColor4f(pref.col.poly_sel.r,pref.col.poly_sel.g,pref.col.poly_sel.b,1.0f);
	glDrawArrays(GL_LINE_LOOP,0,poly->ptsz);
	
	glLineWidth(1.0f);
}

/* =======================================================

      Draw Selection for Liquid
      
======================================================= */

void map_view_draw_select_liquid_get_grow_handles(int liquid_idx,d3pnt *pts)
{
	map_liquid_type			*liq;
	
	liq=&map.liquid.liquids[liquid_idx];
	
	pts[0].x=pts[3].x=pts[4].x=pts[7].x=liq->lft;
	pts[1].x=pts[2].x=pts[5].x=pts[6].x=liq->rgt;
	pts[0].z=pts[1].z=pts[4].z=pts[5].z=liq->top;
	pts[2].z=pts[3].z=pts[6].z=pts[7].z=liq->bot;
	pts[0].y=pts[1].y=pts[2].y=pts[3].y=liq->y;
	pts[4].y=pts[5].y=pts[6].y=pts[7].y=liq->y+liq->depth;
}

void map_view_draw_select_liquid(int liquid_idx)
{
	int						n;
	float					vertexes[8*3];
	float					*pv;
	d3pnt					pts[8];
	map_liquid_type			*liq;
	
	liq=&map.liquid.liquids[liquid_idx];
	
	glEnable(GL_DEPTH_TEST);

	glColor4f(pref.col.mesh_sel.r,pref.col.mesh_sel.g,pref.col.mesh_sel.b,1.0f);
	
		// outline
		
	vertexes[0]=vertexes[9]=(float)liq->lft;
	vertexes[3]=vertexes[6]=(float)liq->rgt;
	vertexes[1]=vertexes[4]=vertexes[7]=vertexes[10]=(float)liq->y;
	vertexes[2]=vertexes[5]=(float)liq->top;
	vertexes[8]=vertexes[11]=(float)liq->bot;
	
	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINE_LOOP,0,4);

	vertexes[0]=vertexes[9]=(float)liq->lft;
	vertexes[3]=vertexes[6]=(float)liq->rgt;
	vertexes[1]=vertexes[4]=vertexes[7]=vertexes[10]=(float)(liq->y+liq->depth);
	vertexes[2]=vertexes[5]=(float)liq->top;
	vertexes[8]=vertexes[11]=(float)liq->bot;
	
	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINE_LOOP,0,4);
	
		// handles

	glPointSize(view_handle_size);
		
	map_view_draw_select_liquid_get_grow_handles(liquid_idx,pts);
		
	pv=vertexes;

	for (n=0;n!=8;n++) {
		if (state.map.drag_handle_idx==n) {
			glColor4f(pref.col.poly_sel.r,pref.col.poly_sel.g,pref.col.poly_sel.b,1.0f);
		}
		else {
			glColor4f(pref.col.mesh_sel.r,pref.col.mesh_sel.g,pref.col.mesh_sel.b,1.0f);
		}
		
		*pv++=(float)pts[n].x;
		*pv++=(float)pts[n].y;
		*pv++=(float)pts[n].z;
	}
	
	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_POINTS,0,8);
}

/* =======================================================

      Draw Selection for Cube Items
      
======================================================= */

void map_view_draw_select_cube(d3pnt *v_pnts)
{
	int				n,k,line_indexes[8]={0,4,1,5,2,6,3,7};
	float			vertexes[8*3];
	float			*pv;
	
    glLineWidth(3.0f);
	glColor4f(1.0f,0.0f,0.0f,1.0f);
	
	pv=vertexes;
	
	for (n=0;n!=4;n++) {
		*pv++=(float)v_pnts[n].x;
		*pv++=(float)v_pnts[n].y;
		*pv++=(float)v_pnts[n].z;
	}
	
	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINE_LOOP,0,4);

	pv=vertexes;
	
	for (n=4;n!=8;n++) {
		*pv++=(float)v_pnts[n].x;
		*pv++=(float)v_pnts[n].y;
		*pv++=(float)v_pnts[n].z;
	}
	
	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINE_LOOP,0,4);
	
	for (n=0;n!=8;n+=2) {
		k=line_indexes[n];
		vertexes[0]=(float)v_pnts[k].x;
		vertexes[1]=(float)v_pnts[k].y;
		vertexes[2]=(float)v_pnts[k].z;
		
		k=line_indexes[n+1];
		vertexes[3]=(float)v_pnts[k].x;
		vertexes[4]=(float)v_pnts[k].y;
		vertexes[5]=(float)v_pnts[k].z;
		
		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINES,0,2);
	}

    glLineWidth(1.0f);
}

/* =======================================================

      Draw Rotation and Move Handles
      
======================================================= */

void map_view_draw_select_2D_rot_handle(d3pnt *pnt,d3pnt *hand_pnt,d3col *col)
{
	float			vertexes[2*2];
	
    glLineWidth(view_handle_line_width);
	
	glColor4f(col->r,col->g,col->b,1.0f);
	
	vertexes[0]=(float)pnt->x;
	vertexes[1]=(float)pnt->y;
	vertexes[2]=(float)hand_pnt->x;
	vertexes[3]=(float)hand_pnt->y;
	
	glVertexPointer(2,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINES,0,2);
	
    glLineWidth(1.0f);
	
	glPointSize(view_handle_size);
	glDrawArrays(GL_POINTS,1,1);
	glPointSize(1.0f);
}

void map_view_draw_select_rot_handles(editor_view_type *view,d3pnt *pnt,d3ang *ang)
{
	d3pnt			center_pnt,hand_pnt[3];
	d3col			col;

		// create rot handles
		
	if (!view_handle_create_rot_handle(view,pnt,ang,&center_pnt,hand_pnt)) return;

		// draw points

	view_set_2D_projection(view);

	glDisable(GL_DEPTH_TEST);
	
	col.r=1.0f;
	col.g=0.0f;
	col.b=0.0f;
	
	map_view_draw_select_2D_rot_handle(&center_pnt,&hand_pnt[0],&col);
	
	col.r=0.0f;
	col.g=1.0f;
	col.b=0.0f;
	
	map_view_draw_select_2D_rot_handle(&center_pnt,&hand_pnt[1],&col);
	
	col.r=0.0f;
	col.g=0.0f;
	col.b=1.0f;
	
	map_view_draw_select_2D_rot_handle(&center_pnt,&hand_pnt[2],&col);
	
		// put back projection
		
	view_set_3D_projection(view,(map.editor_setup.view_near_dist+10),(map.editor_setup.view_far_dist-10),view_near_offset);
}

/* =======================================================

      Draw Selection Box
      
======================================================= */

void map_view_draw_select_box(editor_view_type *view)
{
	int				lx,rx,ty,by;
	float			vertexes[4*2];
	d3rect			box;

		// selection in this view?

	if ((!state.map.select_box_on) || (view!=view_get_current_view())) return;

		// need the box to position

	view_get_pixel_box(view,&box);

	lx=state.map.select_box_start_pnt.x+box.lx;
	rx=state.map.select_box_end_pnt.x+box.lx;
	ty=state.map.select_box_start_pnt.y+box.ty;
	by=state.map.select_box_end_pnt.y+box.ty;

		// draw the selection

	view_set_2D_projection(view);
	
	vertexes[0]=vertexes[6]=(float)lx;
	vertexes[2]=vertexes[4]=(float)rx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;
	
	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(0.5f,0.5f,0.5f,0.7f);
	glDrawArrays(GL_QUADS,0,4);

	glColor4f(1.0f,1.0f,1.0f,0.7f);
	glDrawArrays(GL_LINE_LOOP,0,4);
}

/* =======================================================

      Draw Selections for Map
      
======================================================= */

void map_view_draw_select(editor_view_type *view)
{
	int						n,sel_count,
							type,main_idx,sub_idx;
	unsigned char			draw_mesh_once[max_mesh];
	d3pnt					pnt,v_pnts[8];

		// push view forward so selections
		// display properly

	view_set_3D_projection(view,(map.editor_setup.view_near_dist+10),(map.editor_setup.view_far_dist-10),view_near_offset);
	
		// if no selection, only the box
		// select can draw

	sel_count=select_count();
	if (sel_count==0) {
		map_view_draw_select_box(view);
		return;
	}

		// only draw mesh selections once
		// as there could be multiple polygon selections
		// for a single mesh
		
	memset(draw_mesh_once,0x0,max_mesh);
	
		// draw the selection of meshes, liquids,
		// and other pieces, drawing the selected
		// polygons later
		
	for (n=(sel_count-1);n>=0;n--) {
	
		select_get(n,&type,&main_idx,&sub_idx);
		
			// draw selection
			
		switch (type) {
		
			case item_map_mesh:
				if (draw_mesh_once[main_idx]==0x0) {
					draw_mesh_once[main_idx]=0x1;
					map_view_draw_select_mesh(main_idx);
					map_mesh_calculate_center(&map,main_idx,&pnt);
					map_view_draw_select_rot_handles(view,&pnt,NULL);
				}
				break;
				
			case item_map_liquid:
				map_view_draw_select_liquid(main_idx);
				break;
				
			case item_map_node:
				view_get_sprite_vertexes(&map.nodes[main_idx].pnt,NULL,v_pnts);
				map_view_draw_select_cube(v_pnts);
				map_view_draw_select_rot_handles(view,&map.nodes[main_idx].pnt,&map.nodes[main_idx].ang);
				break;
				
			case item_map_spot:
				map_view_model_cube_vertexes(map.spots[main_idx].display_model,&map.spots[main_idx].pnt,&map.spots[main_idx].ang,1.0f,v_pnts);
				map_view_draw_select_cube(v_pnts);
				map_view_draw_select_rot_handles(view,&map.spots[main_idx].pnt,&map.spots[main_idx].ang);
				break;
				
			case item_map_scenery:
				map_view_model_cube_vertexes(map.sceneries[main_idx].model_name,&map.sceneries[main_idx].pnt,&map.sceneries[main_idx].ang,map.sceneries[main_idx].resize,v_pnts);
				map_view_draw_select_cube(v_pnts);
				map_view_draw_select_rot_handles(view,&map.sceneries[main_idx].pnt,&map.sceneries[main_idx].ang);
				break;
				
			case item_map_light:
				view_get_sprite_vertexes(&map.lights[main_idx].pnt,NULL,v_pnts);
				map_view_draw_select_cube(v_pnts);
				break;
				
			case item_map_sound:
				view_get_sprite_vertexes(&map.sounds[main_idx].pnt,NULL,v_pnts);
				map_view_draw_select_cube(v_pnts);
				break;
				
			case item_map_particle:
				view_get_sprite_vertexes(&map.particles[main_idx].pnt,NULL,v_pnts);
				map_view_draw_select_cube(v_pnts);
				break;

		}
	}
	
		// draw selected polygons
		// draw afterwards so mesh highlighting doesn't
		// effect selection
		
	if (state.map.select_mode==select_mode_polygon) {
	
		for (n=(sel_count-1);n>=0;n--) {
		
			select_get(n,&type,&main_idx,&sub_idx);
			if (type!=item_map_mesh) continue;
			
			map_view_draw_select_mesh_poly(main_idx,sub_idx);
		}
		
	}
	
		// draw selected mesh vertexes
		
	if (state.map.select_mode==select_mode_vertex) {

		memset(draw_mesh_once,0x0,max_mesh);
			
		for (n=(sel_count-1);n>=0;n--) {
		
			select_get(n,&type,&main_idx,&sub_idx);
			if (type!=item_map_mesh) continue;
			
			if (draw_mesh_once[main_idx]==0x0) {
				draw_mesh_once[main_idx]=0x1;
				map_view_draw_select_mesh_vertex(main_idx);
			}
		}
	}

		// draw box selection

	map_view_draw_select_box(view);
}
			
