//
// dim3RTL
// by Brian Barnes (bbarnes@klinksoftware.com)
// free software offered as is for any use
//

#include "ray_os_defs.h"
#include "ray.h"
#include "ray_interface.h"

extern ray_global_type				ray_global;

/* =======================================================

      Find Material Index for ID
      
======================================================= */

int ray_scene_get_index(int sceneId)
{
	int						n;

	for (n=0;n!=ray_global.scene_list.count;n++) {
		if (ray_global.scene_list.scenes[n]->id==sceneId) return(n);
	}

	return(-1);
}

/* =======================================================

      Scene 3D to 2D
      
======================================================= */

void ray_scene_3D_to_2D_point(ray_scene_type *scene,ray_point_type *pnt_3d,ray_2d_point_type *pnt_2d)
{
	float				d,t;
	ray_vector_type		vct;

		// find the vector from the eye to the point
		// and turn it to face the eye plane

	ray_vector_create_from_points(&vct,pnt_3d,&scene->eye.pnt);
	rtlMatrixVectorMultiply(&scene->eye.matrix,&vct);

		// solve for z=plane distance

	d=ray_distance_between_points(&scene->eye.pnt,pnt_3d);
	t=scene->eye.plane_dist/d;

		// find the intersection with the
		// eye plane and then center into
		// the view

	pnt_2d->x=((int)(scene->eye.pnt.x*t))+(scene->buffer.wid>>1);
	pnt_2d->y=((int)(scene->eye.pnt.y*t))+(scene->buffer.high>>1);
}

/* =======================================================

      Create a Scene

 	  Returns:
	   If >=0, then a scene ID
	   RL_ERROR_UNKNOWN_TARGET
	   RL_ERROR_UNKNOWN_FORMAT
	   RL_ERROR_OUT_OF_MEMORY
	   RL_ERROR_THREADING_ERROR
     
======================================================= */

int rtlSceneAdd(ray_2d_point_type *size,int target,int format,void *attachment,unsigned long flags)
{
	int							n,k,split,x_add,y_add;
	ray_scene_type				*scene;
	ray_scene_slice_type		*slice;
	ray_scene_thread_type		*thread;

		// right now only one target/format

	if ((target!=RL_SCENE_TARGET_MEMORY) && (target!=RL_SCENE_TARGET_OPENGL_TEXTURE)) return(RL_ERROR_UNKNOWN_TARGET);
	if (format!=RL_SCENE_FORMAT_32_RGBA) return(RL_ERROR_UNKNOWN_FORMAT);

		// new scene

	scene=(ray_scene_type*)malloc(sizeof(ray_scene_type));
	if (scene==NULL) return(RL_ERROR_OUT_OF_MEMORY);

		// format

	scene->target=target;
	
	switch (target) {
		case RL_SCENE_TARGET_OPENGL_TEXTURE:
			scene->attachment.gl_id=(unsigned long)attachment;
			break;
	}

		// the buffer

	scene->buffer.target=target;
	scene->buffer.format=format;
	scene->buffer.wid=size->x;
	scene->buffer.high=size->y;

	scene->buffer.data=(unsigned long*)malloc((scene->buffer.wid<<2)*scene->buffer.high);
	if (scene->buffer.data==NULL) {
		free(scene);
		return(RL_ERROR_OUT_OF_MEMORY);
	}

		// rendering slices

	scene->render.slices=(ray_scene_slice_type*)malloc(sizeof(ray_scene_slice_type)*ray_render_max_slice_count);
	if (scene->render.slices==NULL) {
		free(scene->buffer.data);
		free(scene);
		return(RL_ERROR_OUT_OF_MEMORY);
	}

		// rendering threads

	scene->render.threads=(ray_scene_thread_type*)malloc(sizeof(ray_scene_thread_type)*ray_render_max_thread_count);
	if (scene->render.threads==NULL) {
		free(scene->render.slices);
		free(scene->buffer.data);
		free(scene);
		return(RL_ERROR_OUT_OF_MEMORY);
	}

		// clear thread states

	ray_render_clear_threads(scene);
	
		// precalc slice settings, drawing rects,
		// and rendering memory

	split=(int)sqrtf(ray_global.settings.slice_count);

	x_add=scene->buffer.wid/split;
	y_add=scene->buffer.high/split;

	slice=scene->render.slices;

	for (n=0;n!=ray_global.settings.slice_count;n++) {

			// slices need to remember their index as
			// polygons have per-slice flags in them

		slice->idx=n;

			// build the pixel rect

		k=n%split;
		slice->pixel_start.x=x_add*k;
		slice->pixel_end.x=slice->pixel_start.x+x_add;
		if (k==(split-1)) slice->pixel_end.x=scene->buffer.wid;
		
		k=n/split;
		slice->pixel_start.y=y_add*k;
		slice->pixel_end.y=slice->pixel_start.y+y_add;
		if (k==(split-1)) slice->pixel_end.y=scene->buffer.high;

		slice++;
	}

		// precalc scene threads
		// current just need a pointer to parent scene
		// we need to do this here as scene could move
		// in memory

	thread=scene->render.threads;

	for (n=0;n!=ray_global.settings.thread_count;n++) {
		thread->parent_scene=scene;
		thread++;
	}

		// make sure thread done list is set
		// high so we don't stall if a scene is never
		// drawn

	ray_scene_clear_threads(scene);
	scene->render.thread_done_count=ray_global.settings.thread_count;

		// setup

	scene->ambient_col.r=scene->ambient_col.g=scene->ambient_col.b=0.0f;

		// any lists

	scene->light_list.count=0;
	scene->light_list.next_id=1;

	scene->mesh_list.count=0;
	scene->mesh_list.next_id=1;
	
		// default eye position
		
	scene->eye.pnt.x=0.0f;
	scene->eye.pnt.y=0.0f;
	scene->eye.pnt.z=0.0f;
	rtlMatrixIdentity(&scene->eye.matrix);
	scene->eye.plane_dist=0.0f;
	scene->eye.max_dist=0.0f;
	
		// create the scene lock mutex

	if (!ray_scene_create_mutexes(scene)) {
		free(scene->render.threads);
		free(scene->render.slices);
		free(scene->buffer.data);
		free(scene);
		return(RL_ERROR_THREADING_ERROR);
	}

		// set the id
		
	scene->id=ray_global.scene_list.next_id;
	ray_global.scene_list.next_id++;
	
		// add to list

	ray_global.scene_list.scenes[ray_global.scene_list.count]=scene;
	ray_global.scene_list.count++;

		// build the threads

	if (!ray_scene_create_threads(scene)) {
		rtlSceneDelete(scene->id);
		return(RL_ERROR_THREADING_ERROR);
	}
	
	return(scene->id);
}

/* =======================================================

      Get the count of Scenes

	  Returns:
	   Scene Count
      
======================================================= */

int rtlSceneCount(void)
{
	return(ray_global.scene_list.count);
}

/* =======================================================

      Deletes a Scene
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

 	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
     
======================================================= */

int rtlSceneDelete(int sceneId)
{
	int							n,idx,count;
	ray_scene_type				*scene;
	
		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// exit the threads
		// and release mutexes

	ray_scene_release_threads(scene);
	ray_scene_release_mutexes(scene);

		// free memory

	free(scene->render.slices);
	free(scene->render.threads);
	free(scene->buffer.data);

		// clear lights and meshes

	rtlSceneLightDeleteAll(sceneId);
	rtlSceneMeshDeleteAll(sceneId);

		// delete scene

	free(scene);

	count=ray_global.scene_list.count-2;

	for (n=idx;n<=count;n++) {
		ray_global.scene_list.scenes[n]=ray_global.scene_list.scenes[n+1];
	}

	ray_global.scene_list.count--;

	return(RL_ERROR_OK);
}

/* =======================================================

      Clears a Scene
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

 	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
     
======================================================= */

int rtlSceneClearBuffer(int sceneId,ray_color_type *col)
{
	int					n,idx,sz;
	unsigned long		*data,ul;
	ray_scene_type		*scene;
	
		// get scene
	
	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);
	
		// buffer clear color

	ul=ray_create_ulong_color_from_float(col);
	
		// clear the buffer
		
	sz=scene->buffer.wid*scene->buffer.high;
	data=scene->buffer.data;
	
	for (n=0;n!=sz;n++) {
		*data++=ul;
	}
	
	return(RL_ERROR_OK);
}

/* =======================================================

      Gets the Buffer Attached to a Scene if Possible
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

 	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
     
======================================================= */

int rtlSceneGetBuffer(int sceneId,void **buffer)
{
	int					idx;
	ray_scene_type		*scene;
	
	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);
	
	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);
	
		// get buffer

	*buffer=scene->buffer.data;
	return(RL_ERROR_OK);
}

