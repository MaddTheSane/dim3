/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: FPP Camera

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
#include "objects.h"

extern server_type			server;
extern map_type				map;
extern camera_type			camera;
extern setup_type			setup;

/* =======================================================

      FPP Camera Position
      
======================================================= */

void camera_fpp_calc_position(void)
{
	obj_type		*obj;

	obj=server.obj_list.objs[camera.obj_idx];
	
    camera.cur_pos.pnt.x=obj->pnt.x+map.camera.pnt_offset.x;
    camera.cur_pos.pnt.y=((obj->pnt.y+obj->duck.y_move+obj->liquid.bob_y_move)+obj->size.eye_offset)+map.camera.pnt_offset.y;
    camera.cur_pos.pnt.z=obj->pnt.z+map.camera.pnt_offset.z;
	
	if (obj->bump.on) camera.cur_pos.pnt.y+=obj->bump.smooth_offset;

	camera.cur_pos.ang.x=map.camera.ang_offset.x-obj->view_ang.x;
	camera.cur_pos.ang.y=angle_add(obj->ang.y,map.camera.ang_offset.y);
    camera.cur_pos.ang.z=map.camera.ang_offset.z+obj->view_ang.z;
}

