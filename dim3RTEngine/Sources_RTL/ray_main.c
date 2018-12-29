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

      dim3RTL Initialize

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rtlInitialize(void)
{
	int					n,thread_count;
#ifdef __APPLE__
	int					names[2];
	size_t				len;
	uint32_t			count;
#endif
#ifdef WIN32
	SYSTEM_INFO			info;
#endif

		// initialize the scene list

	ray_global.scene_list.count=0;
	ray_global.scene_list.next_id=1;

		// initialize the material list
		// need to NULL out all materials
		// material list doesn't move when adding/deleting
		// to preserve indexes, instead depends on NULL
		// for free

	ray_global.material_list.next_id=1;

	for (n=0;n!=ray_max_material;n++) {
		ray_global.material_list.materials[n]=NULL;
	}

		// determine the number of
		// cores avaiable

#ifdef __APPLE__
	names[0]=CTL_HW;
	names[1]=HW_AVAILCPU;
	len=4;
	
    sysctl(names,2,&count,&len,NULL,0);

    if (count<1) {
        names[1]=HW_NCPU;
        sysctl(names,2,&count,&len,NULL,0);
        if (count<1) count=1;
    }
	
	thread_count=count;
#endif

#ifdef __linux__
	thread_count=sysconf(_SC_NPROCESSORS_ONLN);
#endif

#ifdef WIN32
	GetSystemInfo(&info);
	thread_count=info.dwNumberOfProcessors;
#endif

		// currently we create double
		// the number of threads on the system

	ray_global.settings.thread_count=thread_count*2;
	if (ray_global.settings.thread_count>ray_render_max_thread_count) ray_global.settings.thread_count=ray_render_max_thread_count;

		// the number of slices is currently
		// set at ray_render_max_slice_count

	ray_global.settings.slice_count=ray_render_max_slice_count;

	return(RL_ERROR_OK);
}

/* =======================================================

      dim3RTL Shutdown

	  Returns:
	   RL_ERROR_OK
      
======================================================= */

int rtlShutdown(void)
{
	int				id,err;

		// free scenes

	while (ray_global.scene_list.count!=0) {

		id=ray_global.scene_list.scenes[0]->id;

			// need to wait out any rendering

		while (rtlSceneRenderState(id)==RL_SCENE_STATE_RENDERING) {
			usleep(100);
		}

			// delete scene

		err=rtlSceneDelete(id);
		if (err!=RL_ERROR_OK) return(err);
	}

		// free materials

	err=rtlMaterialDeleteAll();
	
	return(err);
}

