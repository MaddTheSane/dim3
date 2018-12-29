/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: View Handles

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

/* =======================================================

      Create Rotation Handles
      
======================================================= */

void view_handle_create_single_rot_handle(d3pnt *pnt,d3vct *vct,d3ang *ang,d3pnt *hand_pnt)
{
	matrix_type		mat;

		// rotations
	
	if (ang!=NULL) {
		if (ang->x!=0) {
			matrix_rotate_x(&mat,ang->x);
			matrix_vertex_multiply(&mat,&vct->x,&vct->y,&vct->z);
		}
		
		if (ang->y!=0) {
			matrix_rotate_y(&mat,ang->y);
			matrix_vertex_multiply(&mat,&vct->x,&vct->y,&vct->z);
		}
		
		if (ang->z!=0) {
			matrix_rotate_z(&mat,ang->z);
			matrix_vertex_multiply(&mat,&vct->x,&vct->y,&vct->z);
		}
	}

		// make point

	hand_pnt->x=pnt->x+(int)vct->x;
	hand_pnt->y=pnt->y+(int)vct->y;
	hand_pnt->z=pnt->z+(int)vct->z;
}

bool view_handle_create_rot_handle(editor_view_type *view,d3pnt *pnt,d3ang *ang,d3pnt *center_pnt,d3pnt *hand_pnt)
{
	float			len;
	d3vct			vct;

	memmove(center_pnt,pnt,sizeof(d3pnt));
	
		// create the handle points

	len=view_handle_length_min+(((float)distance_get(view->pnt.x,view->pnt.y,view->pnt.z,center_pnt->x,center_pnt->y,center_pnt->z)*view_handle_length_factor));

	vct.x=len;
	vct.y=0.0f;
	vct.z=0.0f;

	view_handle_create_single_rot_handle(center_pnt,&vct,ang,&hand_pnt[0]);

	vct.x=0.0f;
	vct.y=-len;
	vct.z=0.0f;

	view_handle_create_single_rot_handle(center_pnt,&vct,ang,&hand_pnt[1]);

	vct.x=0.0f;
	vct.y=0.0f;
	vct.z=len;

	view_handle_create_single_rot_handle(center_pnt,&vct,ang,&hand_pnt[2]);

		// project the points
		// no points if z is behind the camera

	view_set_viewport(view,FALSE,FALSE);
	view_set_3D_projection(view,map.editor_setup.view_near_dist,map.editor_setup.view_far_dist,view_near_offset);

	if (!map_view_project_point_in_z(center_pnt)) return(FALSE);

	map_view_project_point(view,center_pnt);

	map_view_project_point(view,&hand_pnt[0]);
	map_view_project_point(view,&hand_pnt[1]);
	map_view_project_point(view,&hand_pnt[2]);
	
	return(TRUE);
}
