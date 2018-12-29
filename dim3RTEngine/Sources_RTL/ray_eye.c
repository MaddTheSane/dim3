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

      Setup Eye

	  Note:
	   Rotate matrix is in column major order
	   If the scene is currently rendering, this API
	   will stall until it's finished
	   
 	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
     
======================================================= */

int rtlSceneEyePositionSet(int sceneId,ray_point_type *pnt,float fovy,ray_matrix_type *rotMatrix,float eyeMaxDist)
{
	int				idx;
	float			x,y,ang;
	ray_scene_type	*scene;
	
		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);
	
	scene=ray_global.scene_list.scenes[idx];

		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// set eye position
		// and rotate matrix

	memmove(&scene->eye.pnt,pnt,sizeof(ray_point_type));
	memmove(&scene->eye.matrix,rotMatrix,sizeof(ray_matrix_type));

		// calcualte the plane distance
		// from the fov (tan(fov_y)=y[high/2]/x[distance]

	ang=fovy*0.5f;
	y=(float)(scene->buffer.high/2);
	x=y/tanf(ang*ray_ANG_to_RAD);
	scene->eye.plane_dist=x;

		// set the max view distance

	scene->eye.max_dist=eyeMaxDist;
	
	return(RL_ERROR_OK);
}

/* =======================================================

      Translate 3D point to 2D for current eye
	   
 	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_POINT_BEHIND_PLANE
     
======================================================= */

int rtlSceneEyeTranslatePoint(int sceneId,ray_point_type *pnt3d,ray_2d_point_type *pnt2d)
{
	int				idx;
	float			z;
	ray_scene_type	*scene;
	ray_vector_type	eye_vector;
	ray_matrix_type	mat;
	
		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);
	
	scene=ray_global.scene_list.scenes[idx];

		// get ray from eye to 3D point
		// and then rotate it by the transpose
		// matrix to get it's position if z was
		// always positive

	memmove(&mat,&scene->eye.matrix,sizeof(ray_matrix_type));
	rlMatrixTranpose(&mat);

	ray_vector_create_from_points(&eye_vector,pnt3d,&scene->eye.pnt);
	rtlMatrixVectorMultiply(&mat,&eye_vector);

		// behind z?

	if (eye_vector.z<=0.0f) return(RL_ERROR_POINT_BEHIND_PLANE);

		// solve it for hitting the z plane
		// if z>=1.0f, then it's still behind the plane

	z=scene->eye.plane_dist/eye_vector.z;
	if (z>=1.0f) return(RL_ERROR_POINT_BEHIND_PLANE);

		// create the plane points
		// and move them to within the target

	pnt2d->x=((int)(eye_vector.x*z))+(scene->buffer.wid>>1);
	pnt2d->y=((int)(eye_vector.y*z))+(scene->buffer.high>>1);

	return(RL_ERROR_OK);
}


