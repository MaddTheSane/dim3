/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Grows

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

#include "objects.h"

extern server_type				server;
extern map_type					map;

/* =======================================================

      Clear Object Grows
      
======================================================= */

void object_grow_clear(obj_type *obj)
{
	obj->grow.on=FALSE;
}

/* =======================================================

      Direct Object Grows
      
======================================================= */

void object_grow_direct(obj_type *obj,float end_resize)
{
	obj_grow		*grow;

	grow=&obj->grow;

	grow->on=FALSE;

	obj->draw.resize=end_resize;
	
	obj->size.x=(int)((float)obj->size.x*end_resize);
	obj->size.y=(int)((float)obj->size.y*end_resize);
	obj->size.z=(int)((float)obj->size.z*end_resize);

	obj->size.eye_offset=(int)((float)obj->size.eye_offset*end_resize);
}

/* =======================================================

      Start Object Grows
      
======================================================= */

void object_grow_start(obj_type *obj,int grow_msec,float end_resize,d3pnt *end_size,d3pnt *end_offset)
{
	float			f_msec;
	obj_grow		*grow;
	
	grow=&obj->grow;

	grow->count=grow_msec/10;
	f_msec=(float)grow->count;

		// model changes

	grow->resize_end=end_resize;
	grow->resize_add=(end_resize-obj->draw.resize)/f_msec;

		// collision changes

	grow->change_size=FALSE;
	
	if (end_size!=NULL) {
		grow->change_size=TRUE;
		
		grow->size_end.x=end_size->x;
		grow->size_end.y=end_size->y;
		grow->size_end.z=end_size->z;
		
		grow->size_add.x=(float)(grow->size_end.x-obj->size.x)/f_msec;
		grow->size_accum.x=0.0f;

		grow->size_add.y=(float)(grow->size_end.y-obj->size.y)/f_msec;
		grow->size_accum.y=0.0f;

		grow->size_add.z=(float)(grow->size_end.z-obj->size.z)/f_msec;
		grow->size_accum.z=0.0f;
	
			// eye offset (part of Y size change)

		grow->eye_offset_end=(int)((float)obj->size.eye_offset*end_resize);
		grow->eye_offset_add=(float)(grow->eye_offset_end-obj->size.eye_offset)/f_msec;
		grow->eye_offset_accum=0.0f;
	}
	
		// offset changes

	grow->change_offset=FALSE;
	
	if (end_offset!=NULL) {
		grow->change_offset=TRUE;
		
		memmove(&grow->offset_end,end_offset,sizeof(d3pnt));
		grow->offset_add.x=(float)(end_offset->x-obj->draw.offset.x)/f_msec;
		grow->offset_add.y=(float)(end_offset->y-obj->draw.offset.y)/f_msec;
		grow->offset_add.z=(float)(end_offset->z-obj->draw.offset.z)/f_msec;

		grow->offset_accum.x=0.0f;
		grow->offset_accum.y=0.0f;
		grow->offset_accum.z=0.0f;
	}

	grow->on=TRUE;
}

/* =======================================================

      Run Object Grows
      
======================================================= */

void object_grow_run(obj_type *obj)
{
	int					n,xmove,ymove,zmove,xoff,yoff,zoff,eye,yadd;
	float				ypush;
	obj_grow			*grow;
	obj_type			*test_obj;

	grow=&obj->grow;
	if (!grow->on) return;

		// finished?
		// make sure final numbers are hit

	grow->count--;
	if (grow->count==0) {
	
		grow->on=FALSE;
		
		obj->draw.resize=grow->resize_end;
		
		if (grow->change_size) {
			obj->size.x=grow->size_end.x;
			obj->size.y=grow->size_end.y;
			obj->size.z=grow->size_end.z;
			
			obj->size.eye_offset=grow->eye_offset_end;
		}
		
		if (grow->change_offset) {
			obj->draw.offset.x=grow->offset_end.x;
			obj->draw.offset.y=grow->offset_end.y;
			obj->draw.offset.z=grow->offset_end.z;
		}

		return;
	}

		// calculate model resize

	obj->draw.resize+=grow->resize_add;

		// calculate collision size
		
	yadd=0;
		
	if (grow->change_size) {
	
		grow->size_accum.x+=grow->size_add.x;
		xmove=(int)grow->size_accum.x;
		grow->size_accum.x-=(float)xmove;

		obj->size.x+=xmove;

		grow->size_accum.y+=grow->size_add.y;
		ymove=(int)grow->size_accum.y;
		grow->size_accum.y-=(float)ymove;

		obj->size.y+=ymove;
		yadd+=ymove;

		grow->size_accum.z+=grow->size_add.z;
		zmove=(int)grow->size_accum.z;
		grow->size_accum.z-=(float)zmove;

		obj->size.z+=zmove;
		
			// calculate eye offset

		grow->eye_offset_accum+=grow->eye_offset_add;
		eye=(int)grow->eye_offset_accum;
		grow->eye_offset_accum-=(float)eye;

		obj->size.eye_offset+=eye;
	}
	
		// calculate model offset
	
	if (grow->change_offset) {
		grow->offset_accum.x+=grow->offset_add.x;
		xoff=(int)grow->offset_accum.x;
		grow->offset_accum.x-=(float)xoff;

		obj->draw.offset.x+=xoff;

		grow->offset_accum.y+=grow->offset_add.y;
		yoff=(int)grow->offset_accum.y;
		grow->offset_accum.y-=(float)yoff;

		obj->draw.offset.y+=yoff;
		yadd+=yoff;

		grow->offset_accum.z+=grow->offset_add.z;
		zoff=(int)grow->offset_accum.z;
		grow->offset_accum.z-=(float)zoff;

		obj->draw.offset.z+=zoff;
	}

		// move any objects standing on this one
		// if object is growing upwards

	if (yadd>0) {
	
		ypush=-(((float)yadd)/10.0f);

		for (n=0;n!=max_obj_list;n++) {
			test_obj=server.obj_list.objs[n];
			if (test_obj==NULL) continue;

			if (test_obj->contact.stand_obj_idx==obj->idx) {
				object_move_y(test_obj,-yadd);
				if (test_obj->force.vct.y>ypush) test_obj->force.vct.y+=ypush;
			}
		}
	}
}
