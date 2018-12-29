/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: View Picking

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

extern map_type				map;

int							pick_count,pick_col[3];
unsigned char				*pick_pixels;
view_picker_type			*picks;

/* =======================================================

      View Picking Start
      
======================================================= */

bool view_pick_list_start(editor_view_type *view,bool in_2D,int count)
{
		// start picker list

	picks=(view_picker_type*)malloc(sizeof(view_picker_type)*count);
	if (picks==NULL) return(FALSE);

		// no multiple pixel data

	pick_pixels=NULL;

		// no picks set yet

	pick_count=0;

		// start the pick color

	pick_col[0]=0x1;
	pick_col[1]=0x0;
	pick_col[2]=0x0;

		// start the back buffer drawing
		// disable smoothing so colors stay true

	view_set_viewport(view,FALSE,FALSE);
	
	if (!in_2D) {
		view_set_3D_projection(view,map.editor_setup.view_near_dist,map.editor_setup.view_far_dist,view_near_offset);
	}
	else {
		view_set_2D_projection(view);
	}
	
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glDisable(GL_SMOOTH);
	glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	return(TRUE);
}

/* =======================================================

      View Picking Single End
      
======================================================= */

void view_pick_list_end(editor_view_type *view,d3pnt *pnt,int *type,int *main_idx,int *sub_idx)
{
	int					n,col;
	unsigned char		pixel[3];
	d3pnt				click_pnt;
	d3rect				box,wbox;
	view_picker_type	*pick;

		// flush the drawing

	glFinish();

		// need to make click point global
		// and swap Y for OpenGL read

	view_get_pixel_box(view,&box);
	os_get_window_box(&wbox);

	click_pnt.x=pnt->x+box.lx;
	click_pnt.y=wbox.by-(pnt->y+box.ty);

		// find the color at the clicked point

	glReadPixels(click_pnt.x,click_pnt.y,1,1,GL_RGB,GL_UNSIGNED_BYTE,(void*)pixel);

	col=(pixel[2]<<16)|(pixel[1]<<8)|(pixel[0]);

		// find which pick it represents
		
	*type=-1;

	pick=picks;

	for (n=0;n!=pick_count;n++) {

		if (pick->col==col) {
			*type=pick->type;
			*main_idx=pick->main_idx;
			*sub_idx=pick->sub_idx;
			break;
		}

		pick++;
	}

	free(picks);

		// restore smoothing

	glEnable(GL_SMOOTH);
}

/* =======================================================

      View Picking Multiple End
      
======================================================= */

bool view_pick_list_multiple_setup(editor_view_type *view)
{
	int					wid,high;
	d3rect				box,wbox;

		// flush the drawing

	glFinish();

		// grab the pixels

	view_get_pixel_box(view,&box);
	os_get_window_box(&wbox);

	wid=box.rx-box.lx;
	high=box.by-box.ty;
	
	pick_pixels=(unsigned char*)malloc((wid*3)*high);
	if (pick_pixels==NULL) return(FALSE);

	glPixelStoref(GL_PACK_ALIGNMENT,1);
	glReadPixels(box.lx,(wbox.by-box.by),wid,high,GL_RGB,GL_UNSIGNED_BYTE,(void*)pick_pixels);

		// restore smoothing

	glEnable(GL_SMOOTH);
	
	return(TRUE);
}

int view_pick_list_multiple_pick(editor_view_type *view,d3pnt *start_pnt,d3pnt *end_pnt,view_picker_type *pick_list)
{
	int					n,x,y,lx,rx,ty,by,wid,high,col,
						item_count,p_type,p_main_idx,p_sub_idx;
	unsigned char		*pixel;
	d3rect				box;
	view_picker_type	*pick;

		// get the starting position and size

	view_get_pixel_box(view,&box);
	
	wid=box.rx-box.lx;
	high=box.by-box.ty;

	if (start_pnt->x<end_pnt->x) {
		lx=start_pnt->x;
		rx=end_pnt->x;
	}
	else {
		lx=end_pnt->x;
		rx=start_pnt->x;
	}

	if (start_pnt->y<end_pnt->y) {
		ty=start_pnt->y;
		by=end_pnt->y;
	}
	else {
		ty=end_pnt->y;
		by=start_pnt->y;
	}

	if (lx<0) lx=0;
	if (rx>wid) rx=wid;
	if (ty<0) ty=0;
	if (by>high) by=high;

		// clear the pick flags

	pick=picks;

	for (n=0;n!=pick_count;n++) {
		pick->hit=FALSE;
		pick++;
	}

		// run through the pixels

	item_count=0;
	
	for (y=ty;y<by;y++) {
	
		pixel=pick_pixels+(((high-y)*(wid*3))+(lx*3));
		
		for (x=lx;x<rx;x++) {
		
				// get the color
				
			col=((*(pixel+2))<<16)|((*(pixel+1))<<8)|(*pixel);
			pixel+=3;

				// find color in pick list
				// we use the hit to early exit if we've already
				// selected this item

			p_type=-1;

			pick=picks;

			for (n=0;n!=pick_count;n++) {

				if (pick->hit) {
					pick++;
					continue;
				}

				if (pick->col==col) {
					pick->hit=TRUE;
					
					p_type=pick->type;
					p_main_idx=pick->main_idx;
					p_sub_idx=pick->sub_idx;
					break;
				}

				pick++;
			}

				// no hit or a skip?

			if (p_type==-1) continue;

				// add to list

			if (item_count==select_max_item) break;

			pick_list[item_count].type=p_type;
			pick_list[item_count].main_idx=p_main_idx;
			pick_list[item_count].sub_idx=p_sub_idx;

			item_count++;
		}
	}
	
	return(item_count);
}
	
void view_pick_list_multiple_end(void)
{
		// free memory

	if (pick_pixels!=NULL) free(pick_pixels);
	free(picks);
}

/* =======================================================

      View Picking Adding
      
======================================================= */

void view_pick_list_add(int type,int main_idx,int sub_idx)
{
	view_picker_type	*pick;

		// set color

	glColor3ub((unsigned char)pick_col[0],(unsigned char)pick_col[1],(unsigned char)pick_col[2]);

		// remember color in pick list

	pick=&picks[pick_count];

	pick->type=type;
	pick->main_idx=(short)main_idx;
	pick->sub_idx=(short)sub_idx;

	pick->hit=FALSE;
	
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
}

void view_pick_list_add_cube(d3pnt *v_pnts,int type,int main_idx,int sub_idx)
{
	int				n,idx,quad_list[24]={0,1,2,3,4,5,6,7,3,2,6,7,0,1,5,4,0,3,7,4,1,2,6,5};
	float			vertexes[24*3];
	float			*pv;

		// set the color

	view_pick_list_add(type,main_idx,sub_idx);

		// draw the cube

	pv=vertexes;
	
	for (n=0;n!=24;n++) {
		idx=quad_list[n];
		*pv++=(float)v_pnts[idx].x;
		*pv++=(float)v_pnts[idx].y;
		*pv++=(float)v_pnts[idx].z;
	}

	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_QUADS,0,24);
}

void view_pick_list_add_handle(d3pnt *pnt,int type,int main_idx,int sub_idx)
{
	float				vertexes[3];

		// set the color

	view_pick_list_add(type,main_idx,sub_idx);

		// draw the point

	vertexes[0]=(float)pnt->x;
	vertexes[1]=(float)pnt->y;
	vertexes[2]=(float)pnt->z;

	glPointSize(view_handle_size);

	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_POINTS,0,1);
	
	glPointSize(1.0f);
}

void view_pick_list_add_2D_handle(d3pnt *pnt,int type,int main_idx,int sub_idx)
{
	float				vertexes[2];

		// set the color

	view_pick_list_add(type,main_idx,sub_idx);

		// draw the point

	vertexes[0]=(float)pnt->x;
	vertexes[1]=(float)pnt->y;

	glPointSize(view_handle_size);
	
	glVertexPointer(2,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_POINTS,0,1);
	
	glPointSize(1.0f);
}

