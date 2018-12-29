/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: View Movement Routines

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

      Mouse Movement Utility
      
======================================================= */

void view_get_direction(d3pnt *pnt,d3ang *ang)
{
	float			fx,fy,fz;
	matrix_type		mat;
	
	fx=(float)pnt->x;
	fy=(float)pnt->y;
	fz=(float)pnt->z;

	if (ang->x!=0) {
		matrix_rotate_x(&mat,ang->x);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	}
	
	if (ang->y!=0) {
		matrix_rotate_y(&mat,angle_add(ang->y,180.0f));
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	}
	
	if (ang->z!=0) {
		matrix_rotate_z(&mat,ang->z);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	}
	
	pnt->x=(int)fx;
	pnt->y=(int)fy;
	pnt->z=(int)fz;
}

int view_get_movement_scale(editor_view_type *view,d3pnt *pnt)
{

	int					mesh_idx,dist,scale;
	d3pnt				mpnt;
	
	scale=move_mouse_min_scale;
	
		// find closest mesh
		
	mesh_idx=map_mesh_find_closest(&map,pnt);
	if (mesh_idx!=-1) {
		map_mesh_calculate_center(&map,mesh_idx,&mpnt);
		
		dist=distance_get(view->pnt.x,view->pnt.y,view->pnt.z,mpnt.x,mpnt.y,mpnt.z);
		
		scale=dist/move_mouse_distance_ratio;
		if (scale<move_mouse_min_scale) scale=move_mouse_min_scale;
	}
	
	return(scale);
}

/* =======================================================

      Mouse Movement
      
======================================================= */

void view_mouse_get_scroll_horizontal_axis(editor_view_type *view,d3pnt *pnt,int dist)
{
	float			fx,fy,fz;
	matrix_type		mat;

	if (dist==0) return;

	fx=(float)dist;
	fy=0.0f;
	fz=0.0f;

	matrix_rotate_y(&mat,view->ang.y);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);

	pnt->x+=(int)fx;
	pnt->z+=(int)fz;
}

void view_mouse_get_scroll_vertical_axis(editor_view_type *view,d3pnt *pnt,int dist)
{
	float			fx,fy,fz;
	matrix_type		mat;

	if (dist==0) return;

		// always move on axises, find
		// closest to view angle

		// top
		
	if ((view->ang.x<=315.0f) && (view->ang.x>180.0f)) {
		fx=0.0f;
		fy=0.0f;
		fz=(float)dist;

		matrix_rotate_y(&mat,view->ang.y);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);

		pnt->x-=(int)fx;
		pnt->z+=(int)fz;

		return;
	}
	
		// bottom

	if ((view->ang.x>=45.0f) && (view->ang.x<180.0f)) {
		fx=0.0f;
		fy=0.0f;
		fz=-(float)dist;

		matrix_rotate_y(&mat,view->ang.y);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);

		pnt->x-=(int)fx;
		pnt->z+=(int)fz;

		return;
	}
	
		// forward, backward, left, right

	pnt->y+=dist;
}

void view_mouse_get_forward_axis(editor_view_type *view,d3pnt *pnt,int dist)
{
	float			fx,fy,fz,ang_x;
	matrix_type		mat;

	if (dist==0) return;

	fx=0.0f;
	fy=0.0f;
	fz=(float)dist;

	matrix_rotate_y(&mat,view->ang.y);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	ang_x=view_get_lookat_x_angle(view);
	if (fabsf(view->ang.x)<45.0f) ang_x=-ang_x;

	matrix_rotate_x(&mat,ang_x);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);

	pnt->x+=(int)fx;
	pnt->y+=(int)fy;
	pnt->z+=(int)fz;
}

void view_mouse_scroll_movement(editor_view_type *view,d3pnt *pnt)
{
	int						x,y,scale;
	d3pnt					old_pnt,move_pnt;

    os_set_hand_cursor();

	scale=view_get_movement_scale(view,pnt);
	memmove(&old_pnt,pnt,sizeof(d3pnt));

	while (!os_track_mouse_location(pnt,NULL)) {
		
		if ((pnt->x==old_pnt.x) && (pnt->y==old_pnt.y)) continue;

		x=old_pnt.x-pnt->x;
		y=old_pnt.y-pnt->y;

		if (pref.flip_horz_movement) x=-x;
		if (pref.flip_vert_movement) y=-y;
		
		memmove(&old_pnt,pnt,sizeof(d3pnt));
		
		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		view_mouse_get_scroll_horizontal_axis(view,&move_pnt,(x*scale));
		view_mouse_get_scroll_vertical_axis(view,&move_pnt,(y*scale));

		view_move_position(&move_pnt);
		
        main_wind_draw();
	}
}

void view_mouse_forward_movement(editor_view_type *view,d3pnt *pnt)
{
	int				x,y,scale;
	d3pnt			old_pnt,move_pnt;
	d3ang			turn_ang;
    
    os_set_drag_cursor();

	scale=view_get_movement_scale(view,pnt);
	memmove(&old_pnt,pnt,sizeof(d3pnt));
	
	while (!os_track_mouse_location(pnt,NULL)) {
		
		if ((pnt->x==old_pnt.x) && (pnt->y==old_pnt.y)) continue;
		
			// z movement
			
		x=old_pnt.x-pnt->x;
		y=old_pnt.y-pnt->y;
		
		memmove(&old_pnt,pnt,sizeof(d3pnt));
		
		move_pnt.x=move_pnt.y=move_pnt.z=0;

		view_mouse_get_forward_axis(view,&move_pnt,(y*scale));
		view_move_position(&move_pnt);
		
			// y turn
			
		if (x!=0) {
			turn_ang.x=turn_ang.z=0.0f;
			turn_ang.y=((float)x)/move_mouse_turn_reduce_scale;
			if (pref.flip_forward_movement) turn_ang.y=-turn_ang.y;
			view_turn_angle(&turn_ang);
		}

        main_wind_draw();
	}
}

/* =======================================================

      Key Movement
      
======================================================= */

void view_key_forward_movement(editor_view_type *view,int dir)
{
	d3pnt			move_pnt;
	d3ang			ang;
	
	move_pnt.x=0;
	move_pnt.y=0;
	move_pnt.z=dir*2000;

	ang.x=view->ang.x;
	ang.y=angle_add(view->ang.y,180.0f);
	ang.z=view->ang.z;

	view_get_direction(&move_pnt,&ang);
	view_move_position(&move_pnt);

	main_wind_draw();
}

void view_key_side_movement(editor_view_type *view,int dir)
{
	d3pnt			move_pnt;
	d3ang			ang;
	
	move_pnt.x=dir*2000;
	move_pnt.y=0;
	move_pnt.z=0;

	ang.x=view->ang.x;
	ang.y=angle_add(view->ang.y,180.0f);
	ang.z=view->ang.z;

	view_get_direction(&move_pnt,&ang);
	view_move_position(&move_pnt);

	main_wind_draw();
}

void view_key_vert_movement(editor_view_type *view,int dir)
{
	d3pnt			move_pnt;
	d3ang			ang;
	
	move_pnt.x=0;
	move_pnt.y=dir*2000;
	move_pnt.z=0;

	ang.x=view->ang.x;
	ang.y=angle_add(view->ang.y,180.0f);
	ang.z=view->ang.z;

	view_get_direction(&move_pnt,&view->ang);
	view_move_position(&move_pnt);

	main_wind_draw();
}

/* =======================================================

      Turn Mouse Movement
      
======================================================= */

void view_mouse_turn_free(d3pnt *pnt)
{
	int				x,y;
	bool			redraw;
	d3pnt			old_pnt;
	d3ang			turn_ang;
	
    os_set_drag_cursor();

	memmove(&old_pnt,pnt,sizeof(d3pnt));
	
	while (!os_track_mouse_location(pnt,NULL)) {
		
		x=old_pnt.x-pnt->x;
		y=old_pnt.y-pnt->y;

		if (pref.flip_horz_turn) x=-x;
		if (pref.flip_vert_turn) y=-y;
		
		redraw=FALSE;
	
			// y turning
			
		if (x!=0) {
			old_pnt.x=pnt->x;
			
			turn_ang.x=turn_ang.z=0.0f;
			turn_ang.y=((float)x)/move_mouse_turn_reduce_scale;
			view_turn_angle(&turn_ang);

			redraw=TRUE;
		}
		
			// x turning
			
		if (y!=0) {
			old_pnt.y=pnt->y;
			
			turn_ang.y=turn_ang.z=0.0f;
			turn_ang.x=-((float)y)/move_mouse_turn_reduce_scale;
			view_turn_angle(&turn_ang);

			redraw=TRUE;
		}
		
		if (redraw) main_wind_draw();
	}
}

void view_mouse_turn_center(d3pnt *pnt,d3pnt *cnt)
{
	int				x,y;
	float			ang_y,ang_x;
	d3pnt			old_pnt,org_pnt,
					view_pnt,org_view_pnt;
	d3ang			view_ang,org_view_ang;
    
    os_set_drag_cursor();

	memmove(&old_pnt,pnt,sizeof(d3pnt));
	memmove(&org_pnt,pnt,sizeof(d3pnt));
	
	view_get_position(&org_view_pnt);
	view_get_angle(&org_view_ang);
	
	while (!os_track_mouse_location(pnt,NULL)) {
		
		x=old_pnt.x-pnt->x;
		y=old_pnt.y-pnt->y;
		if ((x==0) && (y==0)) continue;
		
		old_pnt.x=pnt->x;
		old_pnt.y=pnt->y;
	
		ang_y=-(((float)(org_pnt.x-pnt->x))/move_mouse_turn_reduce_scale);
		ang_x=-(((float)(org_pnt.y-pnt->y))/move_mouse_turn_reduce_scale);
		
		ang_x=0.0f;		// supergumba -- just rotate around Y now
		
		if (pref.flip_horz_turn) ang_x=-ang_x;
		if (pref.flip_vert_turn) ang_y=-ang_y;

			// run the angle first to get any constraints

		memmove(&view_ang,&org_view_ang,sizeof(d3ang));
		view_ang.x=angle_add(view_ang.x,ang_x);
		view_ang.y=angle_add(view_ang.y,ang_y);
		view_set_angle(&view_ang);

			// rebuild the angle change
			// as setting the angle can constraint it

		view_get_angle(&view_ang);
		ang_x=view_ang.x-org_view_ang.x;
		ang_y=view_ang.y-org_view_ang.y;

			// now move the point

		memmove(&view_pnt,&org_view_pnt,sizeof(d3pnt));
		rotate_point(&view_pnt.x,&view_pnt.y,&view_pnt.z,cnt->x,cnt->y,cnt->z,0.0f,ang_y,0.0f);
		rotate_point(&view_pnt.x,&view_pnt.y,&view_pnt.z,cnt->x,cnt->y,cnt->z,ang_x,0.0f,0.0f);
		view_set_position(&view_pnt);
		
		main_wind_draw();
	}
}

void view_mouse_turn(editor_view_type *view,d3pnt *pnt)
{
	d3pnt			cnt;
	
		// is the view locked?
		
	if (view->no_rot) return;

		// free look

	if ((select_count()!=1) || (state.map.free_look)) {
		view_mouse_turn_free(pnt);
		return;
	}

		// look at

	select_get_center(&cnt);
	view_mouse_turn_center(pnt,&cnt);
}