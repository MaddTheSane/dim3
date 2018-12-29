/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Point Routines

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

/* =======================================================

      Transform a Point Inside Model
	        
======================================================= */

void model_get_point_position(model_draw_setup *draw_setup,d3pnt *pnt)
{
	float				cx,cy,cz,fx,fy,fz;
	matrix_type			rot_x_mat,rot_z_mat,rot_y_mat,sway_mat;
	
	fx=(float)pnt->x;
	fy=(float)pnt->y;
	fz=(float)pnt->z;
	
		// sway
		
	matrix_rotate_zyx(&sway_mat,draw_setup->sway.x,draw_setup->sway.y,draw_setup->sway.z);
	matrix_vertex_multiply(&sway_mat,&fx,&fy,&fz);
		
		// rotate
		
	cx=(float)draw_setup->center.x;
	cy=(float)draw_setup->center.y;
	cz=(float)draw_setup->center.z;

	matrix_rotate_x(&rot_x_mat,draw_setup->ang.x);
	matrix_rotate_y(&rot_y_mat,draw_setup->ang.y);
	matrix_rotate_z(&rot_z_mat,draw_setup->ang.z);
		
	fx-=cx;
	fy-=cy;
	fz-=cz;
		
	matrix_vertex_multiply_ignore_transform(&rot_x_mat,&fx,&fy,&fz);
	matrix_vertex_multiply_ignore_transform(&rot_y_mat,&fx,&fy,&fz);
	matrix_vertex_multiply_ignore_transform(&rot_z_mat,&fx,&fy,&fz);

	pnt->x=(int)((fx+cx)+draw_setup->move.x);
	pnt->y=(int)((fy+cy)+draw_setup->move.y);
	pnt->z=(int)((fz+cz)+draw_setup->move.z);
}

/* =======================================================

      Find Position of Single Bone
      
======================================================= */

void model_get_draw_bone_position(model_draw_setup *draw_setup,int bone_idx,d3pnt *pnt)
{
	model_draw_bone_type		*draw_bone;
	
	draw_bone=&draw_setup->bones[bone_idx];
	pnt->x=(int)draw_bone->fpnt.x;
	pnt->y=(int)draw_bone->fpnt.y;
	pnt->z=(int)draw_bone->fpnt.z;
	
	model_get_point_position(draw_setup,pnt);	
}

void model_calc_draw_bone_position(model_type *model,model_draw_setup *draw_setup,int pose_idx,int bone_idx,d3pnt *pnt)
{
	draw_setup->poses[0].idx_1=pose_idx;
	draw_setup->poses[0].idx_2=-1;
    draw_setup->poses[0].factor=0.0f;
	draw_setup->poses[0].acceleration=0.0f;
	
	model_create_draw_bones(model,draw_setup);
	
	model_get_draw_bone_position(draw_setup,bone_idx,pnt);
}

/* =======================================================

      Light, Halo, and Name Positions
      
======================================================= */

bool model_get_generic_bone_position(model_draw_setup *draw_setup,int bone_idx,d3pnt *pnt)
{
	d3pnt		bone_pnt;
	
		// get bone position
	
	if (bone_idx==-1) return(FALSE);
		
	model_get_draw_bone_position(draw_setup,bone_idx,&bone_pnt);
	
		// add in model position
		
	pnt->x+=bone_pnt.x;
	pnt->y+=bone_pnt.y;
	pnt->z+=bone_pnt.z;
	
	return(TRUE);
}

bool model_get_light_position(model_type *model,model_draw_setup *draw_setup,int idx,d3pnt *pnt)
{
	return(model_get_generic_bone_position(draw_setup,model->bone_connect.light_bone_idx[idx],pnt));
}

bool model_get_halo_position(model_type *model,model_draw_setup *draw_setup,int idx,d3pnt *pnt)
{
	return(model_get_generic_bone_position(draw_setup,model->bone_connect.halo_bone_idx[idx],pnt));
}

