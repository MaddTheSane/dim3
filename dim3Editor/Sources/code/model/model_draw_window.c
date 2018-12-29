/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Window Drawing

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

extern list_palette_type		file_palette,model_palette;

double							model_mod_matrix[16],model_proj_matrix[16],model_vport[4];
d3rect							model_wbox;

/* =======================================================

      Model Window Box
      
======================================================= */

void model_wind_get_box(d3rect *box)
{
	os_get_window_box(box);
	
	box->lx+=list_palette_width(&file_palette);
	box->rx-=list_palette_width(&model_palette);
	box->ty+=tool_button_size;
	box->by-=model_texture_palette_pixel_size();
}

/* =======================================================

      Setup GL Draw
      
======================================================= */

void model_draw_gl_setup(int z_offset)
{
	int				yoff,sz;
	float			ratio;
	d3rect			wbox,mbox;
	
		// viewport setup
		
	os_get_window_box(&wbox);
	model_wind_get_box(&mbox);

	glEnable(GL_SCISSOR_TEST);
	glScissor(mbox.lx,(wbox.by-mbox.by),(mbox.rx-mbox.lx),(mbox.by-mbox.ty));

	glViewport(mbox.lx,(wbox.by-mbox.by),(mbox.rx-mbox.lx),(mbox.by-mbox.ty));

		// projection
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	ratio=(float)(mbox.rx-mbox.lx)/(float)(mbox.by-mbox.ty);
	gluPerspective(45.0,ratio,(float)(100+z_offset),(float)(25000-z_offset));
	glScalef(-1.0f,-1.0f,-1.0f);

	yoff=model.view_box.size.y/2;

	sz=500+((4000-state.model.magnify_z)*5);
	
	glTranslatef(-((GLfloat)state.model.shift.x),-((GLfloat)(state.model.shift.y-yoff)),(GLfloat)sz);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glRotatef(-state.model.ang.x,1.0f,0.0f,0.0f);
	glRotatef(angle_add(state.model.ang.y,180.0f),0.0f,1.0f,0.0f);

		// drawing setup
		
	glColor4f(1.0f,1.0f,1.0f,0.0f);
	
	glDisable(GL_SMOOTH);
	glDisable(GL_DITHER);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_DEPTH_TEST);
}

void model_draw_gl_setup_2D(void)
{
	d3rect				wbox;

	os_get_window_box(&wbox);

	glEnable(GL_SCISSOR_TEST);
	glScissor(wbox.lx,0,(wbox.rx-wbox.lx),(wbox.by-wbox.ty));

	glViewport(wbox.lx,0,(wbox.rx-wbox.lx),(wbox.by-wbox.ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)wbox.lx,(GLdouble)wbox.rx,(GLdouble)wbox.by,(GLdouble)wbox.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/* =======================================================

      Projection Utilities
      
======================================================= */

void model_draw_setup_project_point(void)
{
	os_get_window_box(&model_wbox);
	
	glGetDoublev(GL_MODELVIEW_MATRIX,model_mod_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX,model_proj_matrix);
	glGetIntegerv(GL_VIEWPORT,(GLint*)model_vport);
}

float model_draw_project_point(d3fpnt *pnt,d3pnt *pnt2d)
{
	double			dx,dy,dz;
	d3rect			mbox;

	model_wind_get_box(&mbox);
	
	gluProject(pnt->x,pnt->y,pnt->z,model_mod_matrix,model_proj_matrix,(GLint*)model_vport,&dx,&dy,&dz);
	pnt2d->x=((int)dx)-mbox.lx;
	pnt2d->y=(model_wbox.by-((int)dy))-mbox.ty;
	return((float)dz);
}

bool model_draw_project_point_in_z(d3fpnt *pnt)
{
	return(((((double)pnt->x)*model_mod_matrix[2])+(((double)pnt->y)*model_mod_matrix[6])+(((double)pnt->z)*model_mod_matrix[10])+model_mod_matrix[14])>0.0);
}

float model_draw_project_get_depth(d3fpnt *pnt)
{
	d3pnt			win_pnt;

	return(model_draw_project_point(pnt,&win_pnt));	
}

/* =======================================================

      Draw Model Bone and Vertex Setup
      
======================================================= */

void model_draw_setup_bones_vertexes(int mesh_idx)
{
	int				n;

		// calc the distance
		// normally this is only done once, but many
		// things can change bone positions in animator

	model_calculate_parents(&model);

		// create the drawing bones
	
	model_create_draw_bones(&model,&draw_setup);
	
		// calculate vertexes for all shown
		// meshes

	for (n=0;n!=model.nmesh;n++) {
		if ((n==mesh_idx) || (state.model.show_mesh[n])) {
			model_create_draw_vertexes(&model,n,&draw_setup);
			model_create_draw_normals(&model,n,&draw_setup);
		}
	}
}

/* =======================================================

      Draw Model Info
      
======================================================= */

void model_draw_info(void)
{
	int				x,y,pose_idx;
	char			str[256];
	d3rect			mbox;

	model_wind_get_box(&mbox);

		// model info

	x=mbox.lx+5;
	y=mbox.ty+20;

	sprintf(str,"Mesh: %s",model.meshes[state.model.cur_mesh_idx].name);
	text_draw(x,y,15,NULL,str);
	y+=15;

	if (state.model.cur_animate_idx!=-1) {
		sprintf(str,"Animation: %s",model.animates[state.model.cur_animate_idx].name);
	}
	else {
		strcpy(str,"Animation: (none)");
	}

	text_draw(x,y,15,NULL,str);
	y+=15;

	pose_idx=state.model.cur_pose_idx;
	if ((state.model.play_mode!=model_play_mode_stop) && (state.model.cur_animate_idx!=-1)) pose_idx=model.animates[state.model.cur_animate_idx].pose_moves[state.model.blend[0].pose_move_idx].pose_idx;
	
	if (pose_idx!=-1) {
		sprintf(str,"Pose: %s",model.poses[pose_idx].name);
	}
	else {
		strcpy(str,"Pose: (Neutral)");
	}

	text_draw(x,y,15,NULL,str);

		// selection info

	x=mbox.rx-5;
	y=mbox.by-2;

	str[0]=0x0;

	switch (state.model.select_mode) {

		case select_mode_vertex:
			sprintf(str,"Selected Vertexes: %d",model_vertex_mask_count_sel(state.model.cur_mesh_idx));
			break;

		case select_mode_polygon:
			sprintf(str,"Selected Polys: %d",model_poly_mask_count_sel(state.model.cur_mesh_idx));
			break;

	}

	if (str[0]!=0x0) text_draw_right(x,y,15,NULL,str);
}

void model_draw_mesh_list(void)
{
	int				n,x,y,wid;
	float			vertexes[8];
	d3col			col;
	d3rect			mbox;

	model_wind_get_box(&mbox);

	x=mbox.lx+5;
	y=mbox.by-((15*(model.nmesh-1))+2);

	col.r=col.g=0.0f;

	for (n=0;n!=model.nmesh;n++) {

			// hilite showing meshes

		if ((n==state.model.cur_mesh_idx) || (state.model.show_mesh[n])) {
			wid=text_width(15,model.meshes[n].name);

			if (n==state.model.cur_mesh_idx) {
				glColor4f(1.0f,1.0f,0.0f,1.0f);
			}
			else {
				glColor4f(0.5f,0.5f,1.0f,1.0f);
			}

			vertexes[0]=vertexes[6]=(float)(x-2);
			vertexes[2]=vertexes[4]=(float)((x+wid)+4);
			vertexes[1]=vertexes[3]=(float)(y-16);
			vertexes[5]=vertexes[7]=(float)(y-1);

			glVertexPointer(2,GL_FLOAT,0,vertexes);
			glDrawArrays(GL_QUADS,0,4);
		}

			// mesh name

		text_draw(x,y,15,NULL,model.meshes[n].name);
		y+=15;
	}
}

/* =======================================================

      Draw Model Window
      
======================================================= */

void model_draw_wind(int mesh_idx)
{
	int					n;
	float				vertexes[8];
	d3rect				mbox;

	if (!state.model.model_open) return;

		// setup transformation to fit model in middle of screen
		
	model_draw_gl_setup(0);
	
		// draw memory
		
	model_draw_setup_initialize(&model,&draw_setup,TRUE);
	
		// draw the center
		
	model_draw_axis();

		// create the drawing bones, vertex arrays and normal arrays
		
	model_draw_setup_bones_vertexes(mesh_idx);
	
		// draw the mesh(es) in the current view
	
	if (state.model.texture) {
		for (n=0;n!=model.nmesh;n++) {
			if ((n==mesh_idx) || (state.model.show_mesh[n])) model_draw(n);
		}
	}
	
	if (state.model.mesh) {
		model_draw_gl_setup(1);
		for (n=0;n!=model.nmesh;n++) {
			if ((n==mesh_idx) || (state.model.show_mesh[n])) model_draw_mesh(n);
		}
		model_draw_gl_setup(0);
	}
	
	if (state.model.bone) {
		model_draw_bones(state.model.cur_bone_idx);
		if (state.model.bone_names) {
			model_draw_bone_names(state.model.cur_bone_idx);
			model_draw_gl_setup(0);			// names goes into 2D
		}
	}

		// normals
		
	if (state.model.normal) {
		model_draw_gl_setup(2);
		model_draw_normals(mesh_idx);
		model_draw_gl_setup(0);
	}

		// selection

	if ((state.model.texture) || (state.model.mesh)) {
		model_draw_gl_setup(2);

		switch(state.model.select_mode) {
			case select_mode_polygon:
				model_draw_selected_poly(mesh_idx);
				break;
			case select_mode_vertex:
				model_draw_selected_vertexes(mesh_idx);
				break;
		}

		model_draw_gl_setup(0);
	}
	
		// boxes
		
	if (state.model.hit_box) model_draw_box_hit_boxes();
	if (state.model.view_box) model_draw_box_view();
	
		// free memory
		
	model_draw_setup_shutdown(&model,&draw_setup);
	
		// 2D drawing

	model_wind_get_box(&mbox);
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)mbox.lx,(GLdouble)mbox.rx,(GLdouble)mbox.by,(GLdouble)mbox.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
		// draw the drag selection
		
	if (state.model.drag_sel_on) {
		vertexes[0]=vertexes[6]=(float)(state.model.drag_sel_box.lx+mbox.lx);
		vertexes[2]=vertexes[4]=(float)(state.model.drag_sel_box.rx+mbox.lx);
		vertexes[1]=vertexes[3]=(float)(state.model.drag_sel_box.ty+mbox.ty);
		vertexes[5]=vertexes[7]=(float)(state.model.drag_sel_box.by+mbox.ty);

		glVertexPointer(2,GL_FLOAT,0,vertexes);

		glColor4f(0.8f,0.8f,0.8f,0.4f);
		glDrawArrays(GL_QUADS,0,4);
	}
	
		// info

	model_draw_info();
	model_draw_mesh_list();
}

void model_draw_setup_pose(int pose_idx)
{
	model_draw_setup_clear(&model,&draw_setup);

    draw_setup.poses[0].idx_1=pose_idx;
	draw_setup.poses[0].idx_2=-1;
    draw_setup.poses[0].factor=0;
	draw_setup.poses[0].acceleration=0;

	draw_setup.ang.x=0;
	draw_setup.ang.y=0;
	draw_setup.ang.z=0;
	
    draw_setup.sway.x=draw_setup.sway.z=draw_setup.sway.y=0;
    draw_setup.move.x=draw_setup.move.z=draw_setup.move.y=0;
}

void model_draw_wind_pose(int mesh_idx,int pose_idx)
{
	model_draw_setup_pose(pose_idx);
	model_draw_wind(mesh_idx);
}
