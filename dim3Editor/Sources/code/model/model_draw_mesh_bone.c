/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Draw Model Mesh and Bones

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

extern model_type				model;
extern model_draw_setup			draw_setup;
extern app_state_type			state;
extern app_pref_type			pref;

/* =======================================================

      Draw Mesh
      
======================================================= */

void model_draw_mesh(int mesh_idx)
{
	int					n,k,npoly;
 	float				vertexes[8*3];
	float				*pa,*pv;
	model_mesh_type		*mesh;
	model_poly_type		*poly;

		// draw the mesh
		
	mesh=&model.meshes[mesh_idx];
	
	glColor4f(pref.col.mesh_line.r,pref.col.mesh_line.g,pref.col.mesh_line.b,1.0f);
    
	npoly=mesh->npoly;
	poly=mesh->polys;
    
    for (n=0;n!=npoly;n++) {
	
		if (model_vertex_mask_check_hide_poly(mesh_idx,poly)) {
			poly++;
			continue;
		}

		pv=vertexes;

		for (k=0;k!=poly->ptsz;k++) {
			pa=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array+(poly->v[k]*3);
			*pv++=*pa++;
			*pv++=*pa++;
			*pv++=*pa;
		}

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINE_LOOP,0,poly->ptsz);
		
		poly++;
    }
}

/* =======================================================

      Draw Bones Drag Handles
      
======================================================= */

float model_draw_bones_drag_handle_offset(void)
{
	float				f;
	
	f=(float)(model.view_box.size.x+model.view_box.size.z+model.view_box.size.y)/3.0f;
	f*=0.15f;
	
	if (f<20.0f) return(20.0f);
	if (f>250.0f) return(250.0f);
	
	return(f);
}

void model_draw_bones_drag_handle_calc(d3fpnt *bone_pnt,d3vct *vct,d3ang *ang,d3fpnt *hand_pnt)
{
	matrix_type		mat;
	
		// handle coordinates
	
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
	
	hand_pnt->x=bone_pnt->x-vct->x;
	hand_pnt->y=bone_pnt->y-vct->y;
	hand_pnt->z=bone_pnt->z-vct->z;
}

void model_draw_bones_drag_handle(d3fpnt *pnt,d3vct *vct,d3ang *ang,d3col *col)
{
	float			vertexes[2*3];
	d3fpnt			hand_pnt;
	
		// handle coordinates
		
	model_draw_bones_drag_handle_calc(pnt,vct,ang,&hand_pnt);
		
		// line
		
	glLineWidth(2);

	vertexes[0]=(float)pnt->x;
	vertexes[1]=(float)pnt->y;
	vertexes[2]=(float)pnt->z;
	vertexes[3]=(float)hand_pnt.x;
	vertexes[4]=(float)hand_pnt.y;
	vertexes[5]=(float)hand_pnt.z;
	
	glVertexPointer(3,GL_FLOAT,0,vertexes);
	
	glColor4f(col->r,col->g,col->b,1.0f);
	glDrawArrays(GL_LINES,0,2);
	
	glLineWidth(1);
	
	glPointSize(10);

	vertexes[0]=(float)hand_pnt.x;
	vertexes[1]=(float)hand_pnt.y;
	vertexes[2]=(float)hand_pnt.z;
	
	glVertexPointer(3,GL_FLOAT,0,vertexes);
	
	glColor4f(col->r,col->g,col->b,1.0f);
	glDrawArrays(GL_POINTS,0,1);
	
	glPointSize(1);
}

/* =======================================================

      Find Rotation for Handles
      
======================================================= */

void model_draw_bones_get_handle_rot(int bone_idx,d3ang *rot)
{
	int						parent_idx;
	model_draw_bone_type	*draw_bone;
	
	draw_bone=&draw_setup.bones[bone_idx];
	
		// non-comulative rotations
		
	rot->x=draw_bone->rot.x;
	rot->y=draw_bone->rot.y;
	rot->z=draw_bone->rot.z;

	if (!model.comulative_rotation) return;
	
		// cumulative rotations
		
	parent_idx=bone_idx;
	
	while (TRUE) {
		parent_idx=draw_bone->parent_idx;
		if (parent_idx==-1) break;
		
		draw_bone=&draw_setup.bones[parent_idx];

		rot->x+=draw_bone->rot.x;
		rot->y+=draw_bone->rot.y;
		rot->z+=draw_bone->rot.z;
	}
}

/* =======================================================

      Draw Bones
      
======================================================= */

void model_draw_bones(int sel_bone_idx)
{
	int						n,nbone,parent_idx;
	float					x_move,z_move,y_move,
							bone_drag_handle_offset,
							vertexes[2*3];
	d3fpnt					bone_pnt;
	d3vct					vct;
	d3ang					ang,rot;
	d3col					col;
	model_bone_type			*bone;
	model_draw_bone_type	*draw_bone,*parent_bone;

	glDisable(GL_DEPTH_TEST);

		// draw the bones

	nbone=model.nbone;
    
    x_move=(float)draw_setup.move.x;
    z_move=(float)draw_setup.move.z;
    y_move=(float)draw_setup.move.y;
	
        // connections
        
	glColor4f(0.2f,0.2f,0.2f,1.0f);
	glLineWidth(2);
	
	draw_bone=draw_setup.bones;
	
	for (n=0;n!=nbone;n++) {
        parent_idx=draw_bone->parent_idx;
        if (parent_idx!=-1) {
            parent_bone=&draw_setup.bones[parent_idx];
			
			vertexes[0]=draw_bone->fpnt.x+x_move;
			vertexes[1]=draw_bone->fpnt.y+y_move;
			vertexes[2]=draw_bone->fpnt.z+z_move;
			
			vertexes[3]=parent_bone->fpnt.x+x_move;
			vertexes[4]=parent_bone->fpnt.y+y_move;
			vertexes[5]=parent_bone->fpnt.z+z_move;
			
			glVertexPointer(3,GL_FLOAT,0,vertexes);
			glDrawArrays(GL_LINES,0,2);
        }
		
		draw_bone++;
	}
	
	glLineWidth(1);
        
        // bones
		
	bone=model.bones;
	draw_bone=draw_setup.bones;
	
	bone_drag_handle_offset=model_draw_bones_drag_handle_offset();
	
	for (n=0;n!=nbone;n++) {

			// bone points

		vertexes[0]=bone_pnt.x=draw_bone->fpnt.x+x_move;
		vertexes[1]=bone_pnt.y=draw_bone->fpnt.y+y_move;
		vertexes[2]=bone_pnt.z=draw_bone->fpnt.z+z_move;

		glVertexPointer(3,GL_FLOAT,0,vertexes);
	
			// selected or normal
	
		if (sel_bone_idx==n) {

			glPointSize(10.0f);
			
				// draw selected bone
				
			glColor4f(1.0f,0.0f,1.0f,1.0f);
			glDrawArrays(GL_POINTS,0,1);
		
				// draw drag handles
				
			if (state.model.cur_pose_idx!=-1) {
				model_draw_bones_get_handle_rot(n,&rot);
					
				vct.x=bone_drag_handle_offset;
				vct.y=0.0f;
				vct.z=0.0f;
				ang.x=bone->natural_rot.x;
				ang.y=rot.y+bone->natural_rot.y;
				ang.z=rot.z+bone->natural_rot.z;
				col.r=1.0f;
				col.g=0.0f;
				col.b=0.0f;
				model_draw_bones_drag_handle(&bone_pnt,&vct,&ang,&col);
				
				vct.x=0.0f;
				vct.y=bone_drag_handle_offset;
				vct.z=0.0f;
				ang.x=rot.x+bone->natural_rot.x;
				ang.y=bone->natural_rot.y;
				ang.z=rot.z+bone->natural_rot.z;
				col.r=0.0f;
				col.g=1.0f;
				col.b=0.0f;
				model_draw_bones_drag_handle(&bone_pnt,&vct,&ang,&col);
				
				vct.x=0.0f;
				vct.y=0.0f;
				vct.z=bone_drag_handle_offset;
				ang.x=rot.x+bone->natural_rot.x;
				ang.y=rot.y+bone->natural_rot.y;
				ang.z=bone->natural_rot.z;
				col.r=0.0f;
				col.g=0.0f;
				col.b=1.0f;
				model_draw_bones_drag_handle(&bone_pnt,&vct,&ang,&col);
			}
		}
		else {
		
				// draw regular bone
				
			glPointSize(8.0f);

			glColor4f(1.0f,0.0f,0.0f,1.0f);
			glDrawArrays(GL_POINTS,0,1);
		}
		
		bone++;
		draw_bone++;
	}
	
	glPointSize(1);
}

void model_draw_bone_names(int sel_bone_idx)
{
	int						n,nbone;
	float					x_move,z_move,y_move;
	d3fpnt					pnt;
	d3pnt					pnt2d;
	d3col					col;
	d3rect					mbox;
	model_bone_type			*bone;
	model_draw_bone_type	*draw_bone;
	
	glDisable(GL_DEPTH_TEST);
	
		// get translation matrix
				
	model_draw_setup_project_point();
	model_draw_gl_setup_2D();
	
	model_wind_get_box(&mbox);
	
		// draw the bone names
		
	col.r=col.b=1.0f;
	col.g=0.0f;
		
	nbone=model.nbone;
    
    x_move=(float)draw_setup.move.x;
    z_move=(float)draw_setup.move.z;
    y_move=(float)draw_setup.move.y;
		
	bone=model.bones;
	draw_bone=draw_setup.bones;
	
	for (n=0;n!=nbone;n++) {
	
			// bone position
			
		pnt.x=draw_bone->fpnt.x+x_move;
		pnt.y=draw_bone->fpnt.y+y_move;
		pnt.z=draw_bone->fpnt.z+z_move;
		
		model_draw_project_point(&pnt,&pnt2d);
		
		pnt2d.x+=mbox.lx;
		
			// text
			
		if (n!=sel_bone_idx) {
			text_draw_center(pnt2d.x,(pnt2d.y+50),12.0f,NULL,bone->name);
		}
		else {
			text_draw_center(pnt2d.x,(pnt2d.y+50),12.0f,&col,bone->name);
		}
		
		bone++;
		draw_bone++;
	}
	
	glEnable(GL_DEPTH_TEST);
}

