/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Picking

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

typedef struct		{
						int						col;
						short					idx;
					} model_picker_type;

extern model_type			model;

int							pick_count,pick_col[3];
model_picker_type			*picks;

// supergumba -- it'll be complex, but in the future commonize this with
//               editors version as the base

/* =======================================================

      View Picking
      
======================================================= */

bool model_pick_list_start(int count)
{
		// start picker list

	picks=(model_picker_type*)malloc(sizeof(model_picker_type)*count);
	if (picks==NULL) return(FALSE);

		// no picks set yet

	pick_count=0;

		// start the pick color

	pick_col[0]=0x1;
	pick_col[1]=0x0;
	pick_col[2]=0x0;

		// start the back buffer drawing
		// disable smoothing so colors stay true

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glDisable(GL_SMOOTH);
	glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	return(TRUE);
}

void model_pick_list_end(d3pnt *pnt,int *idx)
{
	int					n,col;
	unsigned char		pixel[3];
	d3pnt				click_pnt;
	d3rect				wbox,mbox;
	model_picker_type	*pick;

		// flush the drawing

	glFinish();

		// need to make click point global
		// and swap Y for OpenGL read

	os_get_window_box(&wbox);
	model_wind_get_box(&mbox);

	click_pnt.x=pnt->x+mbox.lx;
	click_pnt.y=wbox.by-(pnt->y+mbox.ty);

		// find the color at the clicked point

	glReadPixels(click_pnt.x,click_pnt.y,1,1,GL_RGB,GL_UNSIGNED_BYTE,(void*)pixel);

	col=(pixel[2]<<16)|(pixel[1]<<8)|(pixel[0]);

		// find which pick it represents
		
	*idx=-1;
		
	pick=picks;

	for (n=0;n!=pick_count;n++) {

		if (pick->col==col) {
			*idx=pick->idx;
			break;
		}

		pick++;
	}

	free(picks);

		// restore smoothing

	glEnable(GL_SMOOTH);
}

void model_pick_list_add_poly(int idx,int ptsz,d3pnt *v_pnts)
{
	int					n;
	float				vertexes[8*3];
	float				*pv;
	model_picker_type	*pick;

		// set color

	glColor3ub((unsigned char)pick_col[0],(unsigned char)pick_col[1],(unsigned char)pick_col[2]);

		// remember color in pick list

	pick=&picks[pick_count];
	pick->idx=(short)idx;
	
	pick->col=(pick_col[2]<<16)|(pick_col[1]<<8)|(pick_col[0]);

		// next color

	pick_col[0]++;
	if (pick_col[0]>0xFF) {
		pick_col[0]=0x0;
		pick_col[1]++;
		if (pick_col[1]>0xFF) {
			pick_col[1]=0x0;
			pick_col[2]++;
		}
	}

		// next picker

	pick_count++;

		// draw the trig

	pv=vertexes;

	for (n=0;n!=ptsz;n++) {
		*pv++=(float)v_pnts[n].x;
		*pv++=(float)v_pnts[n].y;
		*pv++=(float)v_pnts[n].z;
	}

	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_POLYGON,0,ptsz);
}

