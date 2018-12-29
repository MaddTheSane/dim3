/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Draw Model Axis and Boxes

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
	#include "dim3editor.h"
#endif

#include "glue.h"

extern model_type			model;
extern model_draw_setup		draw_setup;
extern app_state_type		state;

/* =======================================================

      Draw Model Axis
      
======================================================= */

void model_draw_axis(void)
{
	float			vertexes[2*3];

		// x axis

	vertexes[0]=-20000.0f;
	vertexes[1]=(float)model.center.y;
	vertexes[2]=(float)model.center.z;
	vertexes[3]=20000.0f;
	vertexes[4]=(float)model.center.y;
	vertexes[5]=(float)model.center.z;
	
	glVertexPointer(3,GL_FLOAT,0,vertexes);

	glColor4f(1.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINES,0,2);
	
		// y axis
		
    vertexes[0]=(float)model.center.x;
	vertexes[1]=-20000.0f;
	vertexes[2]=(float)model.center.z;
	vertexes[3]=(float)model.center.x;
	vertexes[4]=20000.0f;
	vertexes[5]=(float)model.center.z;

	glVertexPointer(3,GL_FLOAT,0,vertexes);

	glColor4f(0.0f,1.0f,0.0f,1.0f);
	glDrawArrays(GL_LINES,0,2);

		// z axis
		
    vertexes[0]=(float)model.center.x;
	vertexes[1]=(float)model.center.y;
	vertexes[2]=-20000.0f;
	vertexes[3]=(float)model.center.x;
	vertexes[4]=(float)model.center.y;
	vertexes[5]=(float)20000.0f;

	glVertexPointer(3,GL_FLOAT,0,vertexes);

	glColor4f(0.0f,0.0f,1.0f,1.0f);
	glDrawArrays(GL_LINES,0,2);
}

/* =======================================================

      Draw View Box
      
======================================================= */

void model_draw_box_view(void)
{
	int				n,xsz,zsz,ysz,offx,offz,offy;
	float			vertexes[8*3];
	float			*pv;
	d3pnt			pnt[8];

		// get box

    xsz=model.view_box.size.x/2;
    offx=model.view_box.offset.x;
    zsz=model.view_box.size.z/2;
    offz=model.view_box.offset.z;
    ysz=model.view_box.size.y;
    offy=model.view_box.offset.y;
	
	pnt[0].x=pnt[1].x=pnt[4].x=pnt[5].x=offx-xsz;
	pnt[2].x=pnt[3].x=pnt[6].x=pnt[7].x=offx+xsz;
	pnt[0].y=pnt[1].y=pnt[2].y=pnt[3].y=offy-ysz;
	pnt[4].y=pnt[5].y=pnt[6].y=pnt[7].y=offy;
	pnt[0].z=pnt[3].z=pnt[4].z=pnt[7].z=offz-zsz;
	pnt[1].z=pnt[2].z=pnt[5].z=pnt[6].z=offz+zsz;
	
	for (n=0;n!=8;n++) {
		model_get_point_position(&draw_setup,&pnt[n]);
	}
    
		// draw box

 	glColor4f(0.0f,1.0f,0.0f,0.5f);
	glLineWidth(4.0f);

	pv=vertexes;
    
	for (n=0;n!=4;n++) {
		*pv++=(float)pnt[n].x;
		*pv++=(float)pnt[n].y;
		*pv++=(float)pnt[n].z;
	}

	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINE_LOOP,0,4);
    
	pv=vertexes;
    
	for (n=4;n!=8;n++) {
		*pv++=(float)pnt[n].x;
		*pv++=(float)pnt[n].y;
		*pv++=(float)pnt[n].z;
	}

	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINE_LOOP,0,4);
    
	pv=vertexes;

	for (n=0;n!=4;n++) {
		*pv++=(float)pnt[n].x;
		*pv++=(float)pnt[n].y;
		*pv++=(float)pnt[n].z;
		*pv++=(float)pnt[n+4].x;
		*pv++=(float)pnt[n+4].y;
		*pv++=(float)pnt[n+4].z;
	}

	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINES,0,8);
		
    glLineWidth(1.0f);

		// draw floor

	glColor4f(0.5f,0.5f,0.5f,0.5f);
		
	pv=vertexes;

	for (n=4;n!=8;n++) {
		*pv++=(float)pnt[n].x;
		*pv++=(float)pnt[n].y;
		*pv++=(float)pnt[n].z;
	}

	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_QUADS,0,4);
}

/* =======================================================

      Draw Model Box
      
======================================================= */

void model_draw_box_hit_boxes(void)
{
	int				n,k;
	float			vertexes[8*3];
	float			*pv;
	d3pnt			pnt,min,max,box_pnt[8];

	pnt.x=pnt.y=pnt.z=0;
	
	for (n=0;n<model.nhit_box;n++) {

			// get box

		model_hit_box_get_box(&model,n,&pnt,&draw_setup,&min,&max);

		box_pnt[0].x=box_pnt[1].x=box_pnt[4].x=box_pnt[5].x=min.x;
		box_pnt[2].x=box_pnt[3].x=box_pnt[6].x=box_pnt[7].x=max.x;
		box_pnt[0].y=box_pnt[1].y=box_pnt[2].y=box_pnt[3].y=min.y;
		box_pnt[4].y=box_pnt[5].y=box_pnt[6].y=box_pnt[7].y=max.y;
		box_pnt[0].z=box_pnt[3].z=box_pnt[4].z=box_pnt[7].z=min.z;
		box_pnt[1].z=box_pnt[2].z=box_pnt[5].z=box_pnt[6].z=max.z;
		
		for (k=0;k!=8;k++) {
			model_get_point_position(&draw_setup,&box_pnt[k]);
		}
	    
			// draw box

		glColor4f(1.0f,1.0f,0.0f,0.5f);
		glLineWidth(4.0f);
	    
		pv=vertexes;

		for (k=0;k!=4;k++) {
			*pv++=(float)box_pnt[k].x;
			*pv++=(float)box_pnt[k].y;
			*pv++=(float)box_pnt[k].z;
		}
		
		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINE_LOOP,0,4);
	    
		pv=vertexes;

		for (k=4;k!=8;k++) {
			*pv++=(float)box_pnt[k].x;
			*pv++=(float)box_pnt[k].y;
			*pv++=(float)box_pnt[k].z;
		}

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINE_LOOP,0,4);
	    
		pv=vertexes;

		for (k=0;k!=4;k++) {
			*pv++=(float)box_pnt[k].x;
			*pv++=(float)box_pnt[k].y;
			*pv++=(float)box_pnt[k].z;
			*pv++=(float)box_pnt[k+4].x;
			*pv++=(float)box_pnt[k+4].y;
			*pv++=(float)box_pnt[k+4].z;
		}
		
		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINES,0,8);
		
		glLineWidth(1.0f);
	
			// draw handles

		glDisable(GL_DEPTH_TEST);
		
		glColor4f(0.0f,0.0f,0.0f,1.0f);
		glPointSize(10.0f);
		
		pv=vertexes;

		for (k=0;k!=8;k++) {
			*pv++=(float)box_pnt[k].x;
			*pv++=(float)box_pnt[k].y;
			*pv++=(float)box_pnt[k].z;
		}
		
		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_POINTS,0,8);

		glPointSize(1.0f);
		
		glEnable(GL_DEPTH_TEST);
	}
}

