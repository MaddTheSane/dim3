/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Model Hit Boxes

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

/* =======================================================

      Add and Delete Hit Boxes
      
======================================================= */

int model_hit_box_add(model_type *model)
{
	int					hit_box_idx;
	model_hit_box_type	*hit_box,*ptr;

		// only allow a maximum number of hit boxes

	if (model->nhit_box>=max_model_hit_box) return(-1);

		// create memory for new hit box

	ptr=(model_hit_box_type*)malloc(sizeof(model_hit_box_type)*(model->nhit_box+1));
	if (ptr==NULL) return(-1);

	if (model->hit_boxes!=NULL) {
		memmove(ptr,model->hit_boxes,(sizeof(model_hit_box_type)*model->nhit_box));
		free(model->hit_boxes);
	}

	model->hit_boxes=ptr;

	hit_box_idx=model->nhit_box;
	model->nhit_box++;

		// initialize the hit box
	
	hit_box=&model->hit_boxes[hit_box_idx];
	bzero(hit_box,sizeof(model_hit_box_type));
	
	strcpy(hit_box->name,"New Hit Box");
	hit_box->box.offset.x=0;
	hit_box->box.offset.z=0;
	hit_box->box.offset.y=0;
	hit_box->box.size.x=200;
	hit_box->box.size.z=200;
	hit_box->box.size.y=200;

	hit_box->bone_idx=-1;
	
	return(hit_box_idx);
}

void model_hit_box_delete(model_type *model,int hit_box_idx)
{
	model_hit_box_type	*ptr;

		// is the list completely empty?

	if (model->nhit_box==1) {
		free(model->hit_boxes);
		model->hit_boxes=NULL;
		model->nhit_box=0;
		return;
	}

		// if for some reason we can't create new
		// memory, just shuffle the list and wait
		// until next time

	ptr=(model_hit_box_type*)malloc(sizeof(model_hit_box_type)*(model->nhit_box-1));

	if (ptr==NULL) {
		if (hit_box_idx<(model->nhit_box-1)) {
			memmove(&model->hit_boxes[hit_box_idx],&model->hit_boxes[hit_box_idx+1],(sizeof(model_hit_box_type)*((model->nhit_box-hit_box_idx)-1)));
		}
	}
	else {

		if (hit_box_idx>0) {
			memmove(ptr,model->hit_boxes,(sizeof(model_hit_box_type)*hit_box_idx));
		}
		if (hit_box_idx<(model->nhit_box-1)) {
			memmove(&ptr[hit_box_idx],&model->hit_boxes[hit_box_idx+1],(sizeof(model_hit_box_type)*((model->nhit_box-hit_box_idx)-1)));
		}

		free(model->hit_boxes);
		model->hit_boxes=ptr;
	}
	
	model->nhit_box--;
}

/* =======================================================

      Add and Delete Hit Boxes
      
======================================================= */

void model_hit_box_get_center(model_type *model,int hit_box_idx,d3pnt *pnt,model_draw_setup *draw_setup,d3pnt *center_pnt)
{
	model_bone_type			*bone;
	model_draw_bone_type	*draw_bone;
	model_hit_box_type		*hit_box;

		// get center point

	hit_box=&model->hit_boxes[hit_box_idx];

	center_pnt->x=pnt->x+hit_box->box.offset.x;
	center_pnt->y=pnt->y+hit_box->box.offset.y;
	center_pnt->z=pnt->z+hit_box->box.offset.z;

		// any bone moves

	if (hit_box->bone_idx!=-1) {

			// get movement

		bone=&model->bones[hit_box->bone_idx];
		draw_bone=&draw_setup->bones[hit_box->bone_idx];

		center_pnt->x+=(((int)draw_bone->fpnt.x)-bone->pnt.x);
		center_pnt->y+=(((int)draw_bone->fpnt.y)-bone->pnt.y);
		center_pnt->z+=(((int)draw_bone->fpnt.z)-bone->pnt.z);
	}
}

void model_hit_box_get_box(model_type *model,int hit_box_idx,d3pnt *pnt,model_draw_setup *draw_setup,d3pnt *min,d3pnt *max)
{
	int						sz;
	d3pnt					pt;
	model_hit_box_type		*hit_box;

		// get center point

	model_hit_box_get_center(model,hit_box_idx,pnt,draw_setup,&pt);

		// create box

	hit_box=&model->hit_boxes[hit_box_idx];

	sz=hit_box->box.size.x>>1;
	min->x=pt.x-sz;
	max->x=pt.x+sz;

	min->y=pt.y-hit_box->box.size.y;
	max->y=pt.y;

	sz=hit_box->box.size.z>>1;
	min->z=pt.z-sz;
	max->z=pt.z+sz;
}
