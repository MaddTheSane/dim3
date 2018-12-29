/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View dim3RTL Patches

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
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "ray_interface.h"

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern render_info_type		render_info;
extern file_path_setup_type	file_path_setup;

int								view_rtl_draw_scene_id,
								view_rtl_x,view_rtl_y,
								view_rtl_wid,view_rtl_high;
bool							view_rtl_has_render;
GLuint							view_rtl_draw_gl_id;

int								view_rtl_screen_sizes[][2]={{320,200},{400,250},{480,300},{640,400},{720,450},{960,600},{0,0}};

extern bool view_dim3rtl_create_opengl_texture(GLuint *p_gl_id,int wid,int high,char *err_str);
extern void view_dim3rtl_transfer_texture_to_view(int scene_id,int x,int y,int wid,int high,GLuint gl_id);
extern int view_dim3rtl_create_material_from_path(char *path,int alpha_type);
extern void view_dim3rtl_map_mesh_update(void);
extern void view_dim3rtl_map_liquid_mesh_update(void);
extern void view_dim3rtl_map_model_update(void);
extern void view_dim3rtl_projectile_model_update(void);
extern void view_dim3rtl_effect_mesh_update(void);

/* =======================================================

      View dim3RTL Init/Shutdown
      
======================================================= */

bool view_dim3rtl_initialize(char *err_str)
{
	rtl2DPoint			s_pnt;

	if (rtlInitialize()!=RL_ERROR_OK) {
		strcpy(err_str,"Unable to initialize dim3RTL");
		return(FALSE);
	}
	
		// we need a texture to transfer
		// the scene to opengl rastering

	if (!view_dim3rtl_create_opengl_texture(&view_rtl_draw_gl_id,setup.screen_rtl_wid,setup.screen_rtl_high,err_str)) {
		rtlShutdown();
		return(FALSE);
	}
	
		// initialy no view rendering yet
		
	view_rtl_has_render=FALSE;

		// create the drawing scene

	s_pnt.x=setup.screen_rtl_wid;
	s_pnt.y=setup.screen_rtl_high;

	view_rtl_draw_scene_id=rtlSceneAdd(&s_pnt,RL_SCENE_TARGET_OPENGL_TEXTURE,RL_SCENE_FORMAT_32_RGBA,(void*)view_rtl_draw_gl_id,0);
	if (view_rtl_draw_scene_id<0) {
		strcpy(err_str,"Unable to create dim3RTL scene");
		glDeleteTextures(1,&view_rtl_draw_gl_id);
		rtlShutdown();
		return(FALSE);
	}

		// get the view drawing offsets
		// and size

	if (setup.full_screen) {
		view_rtl_x=0;
		view_rtl_y=0;
		view_rtl_wid=render_info.desktop.wid;
		view_rtl_high=render_info.desktop.high;
	}
	else {
		view_rtl_x=0;
		view_rtl_y=0;
		view_rtl_wid=dim3_window_wid;
		view_rtl_high=dim3_window_high;
	}

	return(TRUE);
}

void view_dim3rtl_shutdown(void)
{
	glDeleteTextures(1,&view_rtl_draw_gl_id);
	
	rtlSceneDelete(view_rtl_draw_scene_id);

	rtlShutdown();
}

/* =======================================================

      dim3RTL Image Cache
      
======================================================= */

void view_dim3rtl_image_cache_load(void)
{
	int							n;
	char						path[1024];
	iface_particle_type			*particle;
	iface_ring_type				*ring;
 	iface_mark_type				*mark;

		// hud bitmaps, halos, crosshairs,
		// and labels are 2D effects done in OpenGL

		// particles

	particle=iface.particle_list.particles;

	for (n=0;n!=iface.particle_list.nparticle;n++) {
		file_paths_data(&file_path_setup,path,"Bitmaps/Particles",particle->bitmap_name,"png");
		particle->rtl_material_id=view_dim3rtl_create_material_from_path(path,RL_MATERIAL_ALPHA_ADDITIVE);
		particle++;
	}
	
		// rings

	ring=iface.ring_list.rings;

	for (n=0;n!=iface.ring_list.nring;n++) {
		file_paths_data(&file_path_setup,path,"Bitmaps/Rings",ring->bitmap_name,"png");
		ring->rtl_material_id=view_dim3rtl_create_material_from_path(path,RL_MATERIAL_ALPHA_ADDITIVE);
		ring++;
	}

		// marks
		
	mark=iface.mark_list.marks;

	for (n=0;n!=iface.mark_list.nmark;n++) {
		file_paths_data(&file_path_setup,path,"Bitmaps/Marks",mark->bitmap_name,"png");
		mark->rtl_material_id=view_dim3rtl_create_material_from_path(path,RL_MATERIAL_ALPHA_PASS_THROUGH);
		mark++;
	}
}

void view_dim3rtl_image_cache_free(void)
{
	int							n;
	iface_particle_type			*particle;
	iface_ring_type				*ring;
 	iface_mark_type				*mark;

		// particles

	particle=iface.particle_list.particles;

	for (n=0;n!=iface.particle_list.nparticle;n++) {
		rtlMaterialDelete(particle->rtl_material_id);
		particle++;
	}
	
		// rings

	ring=iface.ring_list.rings;

	for (n=0;n!=iface.ring_list.nring;n++) {
		rtlMaterialDelete(ring->rtl_material_id);
		ring++;
	}

		// marks
		
	mark=iface.mark_list.marks;

	for (n=0;n!=iface.mark_list.nmark;n++) {
		rtlMaterialDelete(mark->rtl_material_id);
		mark++;
	}
}

/* =======================================================

      dim3RTL Translate Points
      
======================================================= */

void view_dim3rtl_project_point(d3pnt *pnt)
{
	ray_point_type			rtl_3d_pnt;
	ray_2d_point_type		rtl_2d_pnt;

		// translate 3D to 2D
		
	rtl_3d_pnt.x=(float)pnt->x;
	rtl_3d_pnt.y=(float)pnt->y;
	rtl_3d_pnt.z=(float)pnt->z;

	rtlSceneEyeTranslatePoint(view_rtl_draw_scene_id,&rtl_3d_pnt,&rtl_2d_pnt);

	pnt->x=(int)rtl_2d_pnt.x;
	pnt->y=(int)rtl_2d_pnt.y;
	
		// since the screen could be blown up
		// we need to translate
		
	pnt->x=(pnt->x*view_rtl_wid)/setup.screen_rtl_wid;
	pnt->y=(pnt->y*view_rtl_high)/setup.screen_rtl_high;
}

bool view_dim3rtl_project_point_behind_z(d3pnt *pnt)
{
	ray_point_type			rtl_3d_pnt;
	ray_2d_point_type		rtl_2d_pnt;

		// translate 3D to 2D
		
	rtl_3d_pnt.x=(float)pnt->x;
	rtl_3d_pnt.y=(float)pnt->y;
	rtl_3d_pnt.z=(float)pnt->z;

	return(rtlSceneEyeTranslatePoint(view_rtl_draw_scene_id,&rtl_3d_pnt,&rtl_2d_pnt)==RL_ERROR_POINT_BEHIND_PLANE);
}

/* =======================================================

      dim3RTL Screenshot
      
======================================================= */

bool view_dim3rtl_screenshot(bool thumbnail,char *path)
{
	int					x,y,y_add,
						ss_wid,ss_high,sav_high,dsz,err;
	float				x_skip,y_skip;
	unsigned char		*pixel_buffer,*data,*sptr,*dptr,*s2ptr;
	bool				ok;
	
		// force update
		
	rtlSceneRenderFinish(view_rtl_draw_scene_id);

		// get the scene buffer

	err=rtlSceneGetBuffer(view_rtl_draw_scene_id,(void**)&pixel_buffer);
	if (err!=RL_ERROR_OK) return(FALSE);
	
		// is this is a thumbnail,
		// then reduce the picture (but keep
		// the dimensions)
		
	x_skip=y_skip=1;
	y_add=0;
	ss_wid=setup.screen_rtl_wid;
	ss_high=sav_high=setup.screen_rtl_high;

	dsz=(setup.screen_rtl_wid*3)*setup.screen_rtl_high;
	
	if (thumbnail) {
		x_skip=((float)setup.screen_rtl_wid)/128.0f;
		ss_wid=128;

		ss_high=(setup.screen_rtl_high*128)/setup.screen_rtl_wid;
		y_skip=((float)setup.screen_rtl_high)/((float)ss_high);
		y_add=(128-ss_high)/2;
		sav_high=128;

		dsz=(128*3)*128;
	}
	
		// transfer the data
		
	data=(unsigned char*)malloc(dsz);
	if (data==NULL) {
		free(pixel_buffer);
		return(FALSE);
	}
	
	bzero(data,dsz);
	
	dptr=data+(y_add*(ss_wid*3));

	for (y=0;y!=ss_high;y++) {
		sptr=pixel_buffer+((setup.screen_rtl_wid*4)*(int)(((float)y)*y_skip));
		
		for (x=0;x!=ss_wid;x++) {
			s2ptr=sptr+(((int)(((float)x)*x_skip))*4);
			*dptr++=*s2ptr;
			*dptr++=*(s2ptr+1);
			*dptr++=*(s2ptr+2);
		}
	}

		// save screenshot

	ok=bitmap_write_png_data(data,ss_wid,sav_high,FALSE,path);
		
	free(data);
	
	return(ok);
}

/* =======================================================

      dim3RTL Rendering
      
======================================================= */

void view_dim3rtl_render_scene(void)
{
	float			ang_y;
	rtlPoint		pnt;
	rtlVector		scale;
	rtlMatrix		mat,x_mat,scale_mat;
	
		// build the eye point

	pnt.x=(float)view.render->camera.pnt.x;
	pnt.y=(float)view.render->camera.pnt.y;
	pnt.z=(float)view.render->camera.pnt.z;

		// build the rotation matrix
		// dim3 always had a backwards look, so
		// we need to fix that with the matrix
		// normally it wouldn't be this complex
		
	ang_y=angle_add(view.render->camera.ang.y,180.0f);
	rtlMatrixRotateY(&mat,ang_y);

	rtlMatrixRotateX(&x_mat,-view.render->camera.ang.x);
	rtlMatrixMultiply(&mat,&x_mat);

	scale.x=-1.0f;
	scale.y=1.0f;
	scale.z=1.0f;
	rtlMatrixScale(&scale_mat,&scale);
	rtlMatrixMultiply(&mat,&scale_mat);

		// set the eye position

	rtlSceneEyePositionSet(view_rtl_draw_scene_id,&pnt,60.0f,&mat,300000.0f);

		// update the scene
		
	view_dim3rtl_map_mesh_update();
	view_dim3rtl_map_liquid_mesh_update();
	view_dim3rtl_map_model_update();
	view_dim3rtl_projectile_model_update();
	view_dim3rtl_effect_mesh_update();

		// render

	rtlSceneRender(view_rtl_draw_scene_id);
}

void view_dim3rtl_render(void)
{
		// if we are in menu, don't
		// do anything (use the last render)

	if (view.menu.active) {
		view_dim3rtl_transfer_texture_to_view(view_rtl_draw_scene_id,view_rtl_x,view_rtl_y,view_rtl_wid,view_rtl_high,view_rtl_draw_gl_id);
		return;
	}

		// if we started a render last
		// time, then make sure it's finished
		// so it gets transfered to texture
		
	if (view_rtl_has_render) {
		rtlSceneRenderFinish(view_rtl_draw_scene_id);
		view_dim3rtl_transfer_texture_to_view(view_rtl_draw_scene_id,view_rtl_x,view_rtl_y,view_rtl_wid,view_rtl_high,view_rtl_draw_gl_id);
		view_rtl_has_render=FALSE;
	}
	
		// render the scene

	view_dim3rtl_render_scene();
	view_rtl_has_render=TRUE;

		// these routines interfere with
		// the first texture unit, so we clear dim3's
		// opengl cache

	gl_texture_clear(0);
}

/* =======================================================

      Quick and Dirty Debug
      
======================================================= */

void view_dim3rtl_debug(char *title)
{
	fprintf(stdout,"%s=================\n",title);

	fprintf(stdout," scene count=%d\n",rtlSceneCount());
	fprintf(stdout," material count=%d\n",rtlMaterialCount());
	fprintf(stdout," mesh count=%d\n",rtlSceneMeshCount(view_rtl_draw_scene_id));
	fprintf(stdout," light count=%d\n",rtlSceneLightCount(view_rtl_draw_scene_id));
}
