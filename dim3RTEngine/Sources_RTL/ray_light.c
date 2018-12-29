//
// dim3RTL
// by Brian Barnes (bbarnes@klinksoftware.com)
// free software offered as is for any use
//

#include "ray_os_defs.h"
#include "ray.h"
#include "ray_interface.h"

ray_global_type					ray_global;

/* =======================================================

      Find Light Index for ID
      
======================================================= */

int ray_scene_light_get_index(ray_scene_type *scene,int lightId)
{
	int					n;

	for (n=0;n!=scene->light_list.count;n++) {
		if (scene->light_list.lights[n]->id==lightId) return(n);
	}

	return(-1);
}

/* =======================================================

      Sets the Scene Ambient Color

	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
      
======================================================= */

int rtlSceneAmbient(int sceneId,ray_color_type *col)
{
	int					idx;
	ray_scene_type		*scene;
	
		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// set the ambient

	memmove(&scene->ambient_col,col,sizeof(ray_color_type));

	return(RL_ERROR_OK);
}

/* =======================================================

      Adds a New Light to a Scene
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   If >=0, then a light ID
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rtlSceneLightAdd(int sceneId)
{
	int					idx;
	ray_light_type		*light;
	ray_scene_type		*scene;
	
		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// create new light

	light=(ray_light_type*)malloc(sizeof(ray_light_type));
	if (light==NULL) return(RL_ERROR_OUT_OF_MEMORY);

		// add light

	light->pnt.x=0.0f;
	light->pnt.y=0.0f;
	light->pnt.z=0.0f;
	
	light->intensity=1.0f;
	light->exponent=1.0f;
	
	light->col.r=1.0f;
	light->col.g=1.0f;
	light->col.b=1.0f;

	light->direction.on=FALSE;
	light->direction.cos_sweep=0.0f;
	light->direction.vct.x=light->direction.vct.y=light->direction.vct.z=0.0f;
	
	light->hidden=FALSE;

		// set id

	light->id=scene->light_list.next_id;
	scene->light_list.next_id++;

		// add light to list

	scene->light_list.lights[scene->light_list.count]=light;
	scene->light_list.count++;
	
	return(light->id);
}

/* =======================================================

      Get the count of Lights in a Scene

	  Returns:
	   if >=0 then Count of Lights in Scene
	   RL_ERROR_UNKNOWN_SCENE_ID
      
======================================================= */

int rtlSceneLightCount(int sceneId)
{
	int					idx;
	ray_scene_type		*scene;

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

	return(scene->light_list.count);
}

/* =======================================================

      Deletes a Light from a Scene

	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_LIGHT_ID
      
======================================================= */

int rtlSceneLightDelete(int sceneId,int lightId)
{
	int					n,idx,count;
	ray_light_type		*light;
	ray_scene_type		*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// get the light

	idx=ray_scene_light_get_index(scene,lightId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_LIGHT_ID);

	light=scene->light_list.lights[idx];

		// remove light

	free(light);

	count=scene->light_list.count-2;

	for (n=idx;n<=count;n++) {
		scene->light_list.lights[n]=scene->light_list.lights[n+1];
	}

	scene->light_list.lights[scene->light_list.count-1]=NULL;

	scene->light_list.count--;

	return(RL_ERROR_OK);
}

/* =======================================================

      Deletes all Lights from a Scene

	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
      
======================================================= */

int rtlSceneLightDeleteAll(int sceneId)
{
	int				idx,err;
	ray_scene_type	*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// clear the lights

	while (scene->light_list.count!=0) {
		err=rtlSceneLightDelete(sceneId,scene->light_list.lights[0]->id);
		if (err!=RL_ERROR_OK) return(err);
	}
	
	return(RL_ERROR_OK);
}

/* =======================================================

      Shows or Hides Light in Scene
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_LIGHT_ID
      
======================================================= */

int rtlSceneLightSetHidden(int sceneId,int lightId,bool hidden)
{
	int					idx;
	ray_light_type		*light;
	ray_scene_type		*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// get the light

	idx=ray_scene_light_get_index(scene,lightId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_LIGHT_ID);

	light=scene->light_list.lights[idx];

		// show or hide
		
	light->hidden=hidden;

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes Position of a Light Already in a Scene
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_LIGHT_ID
      
======================================================= */

int rtlSceneLightSetPosition(int sceneId,int lightId,ray_point_type *pnt)
{
	int					idx;
	ray_light_type		*light;
	ray_scene_type		*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// get the light

	idx=ray_scene_light_get_index(scene,lightId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_LIGHT_ID);

	light=scene->light_list.lights[idx];

		// reset light

	light->pnt.x=pnt->x;
	light->pnt.y=pnt->y;
	light->pnt.z=pnt->z;
	
		// setup bounds
		
	ray_precalc_light_bounds(light);

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes Color of a Light Already in a Scene
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_LIGHT_ID
      
======================================================= */

int rtlSceneLightSetColor(int sceneId,int lightId,ray_color_type *col)
{
	int					idx;
	ray_light_type		*light;
	ray_scene_type		*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// get the light

	idx=ray_scene_light_get_index(scene,lightId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_LIGHT_ID);

	light=scene->light_list.lights[idx];

		// reset light

	light->col.r=col->r;
	light->col.g=col->g;
	light->col.b=col->b;

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes Intensity of a Light Already in a Scene
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_LIGHT_ID
      
======================================================= */

int rtlSceneLightSetIntensity(int sceneId,int lightId,float intensity,float exponent)
{
	int					idx;
	ray_light_type		*light;
	ray_scene_type		*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// get the light

	idx=ray_scene_light_get_index(scene,lightId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_LIGHT_ID);

	light=scene->light_list.lights[idx];

		// reset light

	light->intensity=intensity;
	light->exponent=exponent;
	
		// setup bounds
		
	ray_precalc_light_bounds(light);

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes Direction of a Light Already in a Scene
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_LIGHT_ID
      
======================================================= */

int rtlSceneLightSetDirection(int sceneId,int lightId,rtlVector *vector,float angle,bool active)
{
	int					idx;
	ray_light_type		*light;
	ray_scene_type		*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// get the light

	idx=ray_scene_light_get_index(scene,lightId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_LIGHT_ID);

	light=scene->light_list.lights[idx];

		// reset direction

	light->direction.on=active;

	light->direction.cos_sweep=angle/90.0f;

	memmove(&light->direction.vct,vector,sizeof(ray_vector_type));
	ray_vector_normalize(&light->direction.vct);

	return(RL_ERROR_OK);
}
