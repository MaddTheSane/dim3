/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: OpenGL Frame Routines

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

extern app_type				app;
extern map_type				map;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern render_info_type		render_info;
extern camera_type			camera;

extern SDL_Window			*sdl_wind;

int							gl_viewport[4],gl_sav_viewport[4];
float						gl_proj_matrix[16],gl_model_view_matrix[16],
							gl_normal_matrix[9];

/* =======================================================

      View Port
      
======================================================= */

void gl_set_viewport(int x,int y,int wid,int high)
{
	glViewport(x,y,wid,high);
	
	gl_viewport[0]=x;
	gl_viewport[1]=y;
	gl_viewport[2]=wid;
	gl_viewport[3]=high;
}

void gl_save_viewport(void)
{
	memmove(gl_sav_viewport,gl_viewport,(sizeof(int)*4));
}

void gl_restore_viewport(void)
{
	gl_set_viewport(gl_sav_viewport[0],gl_sav_viewport[1],gl_sav_viewport[2],gl_sav_viewport[3]);
}

/* =======================================================

      Construct Normal Matrix
      
======================================================= */

void gl_create_normal_matrix(matrix_type *model_mat)
{
	matrix_type			mat;
	
	memmove(&mat,model_mat,sizeof(matrix_type));

	matrix_inverse(&mat);
	matrix_transpose(&mat);
	
	matrix_to_opengl_uniform_3x3(&mat,gl_normal_matrix);
}

/* =======================================================

      3D Projections
      
======================================================= */

void gl_3D_view(void)
{
	float			fov,ratio;
	matrix_type		mat;
	
		// get fov and screen ratio

	fov=view.render->camera.fov*iface.devices[view.device_type].render.fov_adjust;
		
#ifndef D3_ROTATE_VIEW
	ratio=(((float)view.screen.x_sz)/((float)view.screen.y_sz))*map.camera.plane.aspect_ratio;
#else
	ratio=(((float)view.screen.y_sz)/((float)view.screen.x_sz))*map.camera.plane.aspect_ratio;
#endif

		// create the projection matrix
		// as a column ordered opengl matrix
	
	matrix_perspective(&mat,fov,ratio,(float)map.camera.plane.near_z,(float)map.camera.plane.far_z);

#ifdef D3_ROTATE_VIEW
	matrix_rotate(&mat,-90.0f,0.0f,0.0f,1.0f);
#endif

	if (view.render->camera.flip) {
		matrix_scale(&mat,1.0f,1.0f,-1.0f);
	}
	else {
		matrix_scale(&mat,-1.0f,-1.0f,-1.0f);
	}
	
	matrix_translate(&mat,0.0f,0.0f,(float)(map.camera.plane.near_z_offset+view.render->camera.z_adjust));
	
	matrix_to_opengl_uniform_4x4(&mat,gl_proj_matrix);

		// create the model view matrix
		// as a column ordered opengl matrix

	matrix_identity(&mat);

	matrix_lookat(&mat,(float)view.render->camera.pnt.x,(float)view.render->camera.pnt.y,(float)(view.render->camera.pnt.z+map.camera.plane.near_z),(float)view.render->camera.pnt.x,(float)view.render->camera.pnt.y,(float)view.render->camera.pnt.z,0.0f,1.0f,0.0f);

	matrix_to_opengl_uniform_4x4(&mat,gl_model_view_matrix);
	
		// create normal matrix
		
	gl_create_normal_matrix(&mat);
	
		// force shaders to reset matrixes
		
	gl_shader_force_matrix_resets();
}

void gl_3D_rotate(d3pnt *pnt,d3ang *ang)
{
	float		fx,fz,fy,ang_x;
	matrix_type	mat,look_mat;

		// create the model view matrix
		// as a column ordered opengl matrix

	matrix_identity(&mat);
	
		// need to cap look up/down at 90
		
	ang_x=ang->x;
	if (ang_x>=90.0f) ang_x=89.9f;
	if (ang_x<=-90.0f) ang_x=-89.9f;

		// create the look at vector
		
	matrix_rotate_zyx(&look_mat,ang_x,ang->y,ang->z);
	fx=fy=0.0f;
	fz=-400.0f;			// the default near z
	matrix_vertex_multiply(&look_mat,&fx,&fy,&fz);

	if (pnt==NULL) {
		matrix_lookat(&mat,fx,fy,fz,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
	}
	else {
		matrix_lookat(&mat,(((float)pnt->x)+fx),(((float)pnt->y)+fy),(((float)pnt->z)+fz),(float)pnt->x,(float)pnt->y,(float)pnt->z,0.0f,1.0f,0.0f);
	}

	matrix_to_opengl_uniform_4x4(&mat,gl_model_view_matrix);
	
		// create normal matrix
		
	gl_create_normal_matrix(&mat);
	
		// force shaders to reset matrixes
		
	gl_shader_force_matrix_resets();
}

/* =======================================================

      2D Projections
      
======================================================= */

void gl_2D_view_screen(void)
{
	matrix_type			mat;
	
		// create the projection matrix
		// as a column ordered opengl matrix
				
#ifndef D3_ROTATE_VIEW
	matrix_ortho(&mat,0.0f,(float)view.screen.x_sz,(float)view.screen.y_sz,0.0f,-1.0f,1.0f);
#else
	matrix_ortho(&mat,0.0f,(float)view.screen.y_sz,(float)view.screen.x_sz,0.0f,-1.0f,1.0f);
	matrix_translate(&mat,(float)view.screen.y_sz,0.0f,0.0f);
	matrix_rotate(&mat,90.0f,0.0f,0.0f,1.0f);
#endif

	matrix_to_opengl_uniform_4x4(&mat,gl_proj_matrix);

		// create the model view matrix
		// as a column ordered opengl matrix

	matrix_identity(&mat);

	matrix_to_opengl_uniform_4x4(&mat,gl_model_view_matrix);
	
		// create normal matrix
		
	gl_create_normal_matrix(&mat);

		// force shaders to reset matrixes
		
	gl_shader_force_matrix_resets();
}

void gl_2D_view_interface(void)
{
	matrix_type		mat;
	
		// create the projection matrix
		// as a column ordered opengl matrix
				
#ifndef D3_ROTATE_VIEW
	matrix_ortho(&mat,0.0f,(float)iface.scale_x,(float)iface.scale_y,0.0f,-1.0f,1.0f);
#else
	matrix_ortho(&mat,0.0f,(float)iface.scale_y,(float)iface.scale_x,0.0f,-1.0f,1.0f);
	matrix_translate(&mat,(float)iface.scale_y,0.0f,0.0f);
	matrix_rotate(&mat,90.0f,0.0f,0.0f,1.0f);
#endif

	matrix_to_opengl_uniform_4x4(&mat,gl_proj_matrix);

		// create the model view matrix
		// as a column ordered opengl matrix

	matrix_identity(&mat);

	matrix_to_opengl_uniform_4x4(&mat,gl_model_view_matrix);
	
		// create normal matrix
		
	gl_create_normal_matrix(&mat);

		// force shaders to reset matrixes
		
	gl_shader_force_matrix_resets();
}

/* =======================================================

      Pseudo 3D Model Projections (for models in interface)
      
======================================================= */

void gl_3D_view_interface_model()
{
	float				x,y;
	matrix_type			mat;

		// create the projection matrix
		// as a column ordered opengl matrix
		
	x=(float)(iface.scale_x>>1);
	y=(float)(iface.scale_y>>1);

#ifndef D3_ROTATE_VIEW
	matrix_frustum(&mat,-x,x,-y,y,1000.0f,21000.0f);
#else
	matrix_frustum(&mat,-y,y,-x,x,1000.0f,21000.0f);
#endif

	matrix_scale(&mat,1.0f,-1.0f,-1.0f);
	matrix_translate(&mat,0.0f,0.0f,5000.0f);

#ifdef D3_ROTATE_VIEW
	matrix_rotate(&mat,90.0f,0.0f,0.0f,1.0f);
#endif
	
	matrix_to_opengl_uniform_4x4(&mat,gl_proj_matrix);

		// create the model view matrix
		// as a column ordered opengl matrix

	matrix_identity(&mat);

	matrix_to_opengl_uniform_4x4(&mat,gl_model_view_matrix);
	
		// create normal matrix
		
	gl_create_normal_matrix(&mat);

		// force shaders to reset matrixes
		
	gl_shader_force_matrix_resets();
}

/* =======================================================

      Start and End a Drawing Session
      
======================================================= */

void gl_frame_clear(bool in_view)
{
		// if obscuring fog on, then background = fog color

	if ((!fog_solid_on()) || (!in_view)) {
		glClearColor(0.0f,0.0f,0.0f,0.0f);
	}
	else {
		glClearColor(map.fog.col.r,map.fog.col.g,map.fog.col.b,0.0f);
	}

		// clear the frame

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
}

void gl_frame_swap(void)
{
#ifdef D3_OS_IPHONE
	const GLenum		discards[]={GL_DEPTH_ATTACHMENT,GL_STENCIL_ATTACHMENT};
#endif
	d3col				col;
	
		// is this app deactivated?

	if (app.state==as_inactive) {
		gl_2D_view_screen();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glDisable(GL_DEPTH_TEST);
		
		col.r=col.g=col.b=0.0f;
		view_primitive_2D_tint_screen(&col,0.5f);
	}
	
		// discard the depth and stencil

#ifdef D3_OS_IPHONE
	glDiscardFramebufferEXT(GL_FRAMEBUFFER,2,discards);
#endif

		// swap buffer

	SDL_GL_SwapWindow(sdl_wind);
}

/* =======================================================

      Scissoring
      
======================================================= */

void gl_2D_scissor_start(int lx,int rx,int ty,int by)
{
	lx=(view.screen.x_sz*(lx))/iface.scale_x;
	rx=(view.screen.x_sz*(rx))/iface.scale_x;
	ty=(view.screen.y_sz*(ty))/iface.scale_y;
	by=(view.screen.y_sz*(by))/iface.scale_y;

	glEnable(GL_SCISSOR_TEST);
	
#ifndef D3_ROTATE_VIEW
	glScissor(lx,(view.screen.y_sz-by),(rx-lx),(by-ty));
#else
	glScissor((view.screen.y_sz-by),lx,(by-ty),(rx-lx));
#endif
}

void gl_2D_scissor_end(void)
{
	glDisable(GL_SCISSOR_TEST);
}

/* =======================================================

      Projection Points
      
======================================================= */

void gl_project_point_patch(d3pnt *pnt,d3fpnt *win_pnt)
{
    float		in[4],out[4];

		// duplicate the gl_Position calculation

		// multiply by modelview matrix
		// last part of vector is always 1.0 here

	out[0]=(pnt->x*gl_model_view_matrix[0])+(pnt->y*gl_model_view_matrix[4])+(pnt->z*gl_model_view_matrix[8])+gl_model_view_matrix[12];
	out[1]=(pnt->x*gl_model_view_matrix[1])+(pnt->y*gl_model_view_matrix[5])+(pnt->z*gl_model_view_matrix[9])+gl_model_view_matrix[13];
	out[2]=(pnt->x*gl_model_view_matrix[2])+(pnt->y*gl_model_view_matrix[6])+(pnt->z*gl_model_view_matrix[10])+gl_model_view_matrix[14];
	out[3]=(pnt->x*gl_model_view_matrix[3])+(pnt->y*gl_model_view_matrix[7])+(pnt->z*gl_model_view_matrix[11])+gl_model_view_matrix[15];

		// now multiply by the projection matrix

	in[0]=(out[0]*gl_proj_matrix[0])+(out[1]*gl_proj_matrix[4])+(out[2]*gl_proj_matrix[8])+(out[3]*gl_proj_matrix[12]);
	in[1]=(out[0]*gl_proj_matrix[1])+(out[1]*gl_proj_matrix[5])+(out[2]*gl_proj_matrix[9])+(out[3]*gl_proj_matrix[13]);
	in[2]=(out[0]*gl_proj_matrix[2])+(out[1]*gl_proj_matrix[6])+(out[2]*gl_proj_matrix[10])+(out[3]*gl_proj_matrix[14]);
	in[3]=(out[0]*gl_proj_matrix[3])+(out[1]*gl_proj_matrix[7])+(out[2]*gl_proj_matrix[11])+(out[3]*gl_proj_matrix[15]);

	if (in[3]==0.0f) return;

	in[0]=((in[0]/in[3])*0.5f)+0.5f;
    in[1]=((in[1]/in[3])*0.5f)+0.5f;
    in[2]=((in[2]/in[3])*0.5f)+0.5f;

    win_pnt->x=(in[0]*gl_viewport[2])+gl_viewport[0];
    win_pnt->y=(in[1]*gl_viewport[3])+gl_viewport[1];
	win_pnt->z=in[2];
}

void gl_project_point_patch_f(d3fpnt *f_pnt,d3fpnt *win_pnt)
{
    float		in[4],out[4];

		// duplicate the gl_Position calculation

		// multiply by modelview matrix
		// last part of vector is always 1.0 here

	out[0]=(f_pnt->x*gl_model_view_matrix[0])+(f_pnt->y*gl_model_view_matrix[4])+(f_pnt->z*gl_model_view_matrix[8])+gl_model_view_matrix[12];
	out[1]=(f_pnt->x*gl_model_view_matrix[1])+(f_pnt->y*gl_model_view_matrix[5])+(f_pnt->z*gl_model_view_matrix[9])+gl_model_view_matrix[13];
	out[2]=(f_pnt->x*gl_model_view_matrix[2])+(f_pnt->y*gl_model_view_matrix[6])+(f_pnt->z*gl_model_view_matrix[10])+gl_model_view_matrix[14];
	out[3]=(f_pnt->x*gl_model_view_matrix[3])+(f_pnt->y*gl_model_view_matrix[7])+(f_pnt->z*gl_model_view_matrix[11])+gl_model_view_matrix[15];

		// now multiply by the projection matrix

	in[0]=(out[0]*gl_proj_matrix[0])+(out[1]*gl_proj_matrix[4])+(out[2]*gl_proj_matrix[8])+(out[3]*gl_proj_matrix[12]);
	in[1]=(out[0]*gl_proj_matrix[1])+(out[1]*gl_proj_matrix[5])+(out[2]*gl_proj_matrix[9])+(out[3]*gl_proj_matrix[13]);
	in[2]=(out[0]*gl_proj_matrix[2])+(out[1]*gl_proj_matrix[6])+(out[2]*gl_proj_matrix[10])+(out[3]*gl_proj_matrix[14]);
	in[3]=(out[0]*gl_proj_matrix[3])+(out[1]*gl_proj_matrix[7])+(out[2]*gl_proj_matrix[11])+(out[3]*gl_proj_matrix[15]);

	if (in[3]==0.0f) return;

	in[0]=((in[0]/in[3])*0.5f)+0.5f;
    in[1]=((in[1]/in[3])*0.5f)+0.5f;
    in[2]=((in[2]/in[3])*0.5f)+0.5f;

    win_pnt->x=(in[0]*gl_viewport[2])+gl_viewport[0];
    win_pnt->y=(in[1]*gl_viewport[3])+gl_viewport[1];
	win_pnt->z=in[2];
}

bool gl_project_in_view_z(d3pnt *pnt)
{
	return(((((float)pnt->x)*gl_model_view_matrix[2])+(((float)pnt->y)*gl_model_view_matrix[6])+(((float)pnt->z)*gl_model_view_matrix[10])+gl_model_view_matrix[14])>0.0);
}

bool gl_project_in_view_z_f(d3fpnt *f_pnt)
{
	return(((f_pnt->x*gl_model_view_matrix[2])+(f_pnt->y*gl_model_view_matrix[6])+(f_pnt->z*gl_model_view_matrix[10])+gl_model_view_matrix[14])>0.0);
}

void gl_project_point(d3pnt *pnt)
{
	d3fpnt			win_pnt;

#ifndef D3_ROTATE_VIEW
	gl_project_point_patch(pnt,&win_pnt);

	pnt->x=(int)win_pnt.x;
	pnt->y=(int)win_pnt.y;
	pnt->z=(int)win_pnt.z;
#else
	gl_project_point_patch(pnt,&win_pnt);

	pnt->x=(int)(gl_viewport[3]-win_pnt.y);
	pnt->y=(int)win_pnt.x;
	pnt->z=(int)win_pnt.z;
#endif
}

void gl_project_point_f(d3pnt *pnt,d3fpnt *f_pnt)
{
#ifndef D3_ROTATE_VIEW
	gl_project_point_patch(pnt,f_pnt);
#else
	d3fpnt			win_pnt;

	gl_project_point_patch(pnt,&win_pnt);

	f_pnt->x=gl_viewport[3]-win_pnt.y;
	f_pnt->y=win_pnt.x;
	f_pnt->z=win_pnt.z;
#endif
}

float gl_project_get_depth(d3pnt *pnt)
{
	d3fpnt			win_pnt;

	gl_project_point_patch(pnt,&win_pnt);	
	return(win_pnt.z);
}

float gl_project_get_depth_f(d3fpnt *f_pnt)
{
	d3fpnt			win_pnt;

	gl_project_point_patch_f(f_pnt,&win_pnt);	
	return(win_pnt.z);
}

void gl_project_to_eye_coordinates(d3vct *vct)
{
	float		fx,fy,fz;
	
	fx=vct->x;
	fy=vct->y;
	fz=vct->z;
	
	vct->x=(float)((fx*gl_model_view_matrix[0])+(fy*gl_model_view_matrix[4])+(fz*gl_model_view_matrix[8])+gl_model_view_matrix[12]);
	vct->y=(float)((fx*gl_model_view_matrix[1])+(fy*gl_model_view_matrix[5])+(fz*gl_model_view_matrix[9])+gl_model_view_matrix[13]);
	vct->z=(float)((fx*gl_model_view_matrix[2])+(fy*gl_model_view_matrix[6])+(fz*gl_model_view_matrix[10])+gl_model_view_matrix[14]);
}
