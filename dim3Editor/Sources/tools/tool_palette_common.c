/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Tool Palette

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

extern file_path_setup_type		file_path_setup;

/* =======================================================

      Tool Palette Initialize and Shutdown
      
======================================================= */

void tool_palette_initialize(tool_palette_type *tool_palette,int count,tool_palette_tool_type *tools)
{
	int				n;
	char			sub_path[1024],path[1024];

		// setup

	tool_palette->count=count;
	tool_palette->push_idx=-1;

	memmove(tool_palette->tools,tools,(sizeof(tool_palette_tool_type)*count));

		// load icons
		
	os_get_support_file_path(sub_path);
	strcat(sub_path,"/Tools");
		
	for (n=0;n!=tool_palette->count;n++) {
		tool_palette->state[n].disabled=FALSE;
		tool_palette->state[n].selected=FALSE;

		if (tool_palette->tools[n].file_name[0]!=0x0) {
			file_paths_app(&file_path_setup,path,sub_path,tool_palette->tools[n].file_name,"png");
			bitmap_open(&tool_palette->tools[n].bitmap,path,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE);
		}
	}
}

void tool_palette_shutdown(tool_palette_type *tool_palette)
{
	int				n;

	for (n=0;n!=tool_palette->count;n++) {
		if (tool_palette->tools[n].file_name[0]!=0x0) bitmap_close(&tool_palette->tools[n].bitmap);
	}
}

/* =======================================================

      Tool Palette Settings
      
======================================================= */

void tool_palette_box(d3rect *box)
{
	d3rect			wbox;
	
	os_get_window_box(&wbox);
	
	box->lx=wbox.lx;
	box->rx=wbox.rx;
	box->ty=wbox.ty;
	box->by=box->ty+(tool_button_size+1);
}

void tool_palette_set_state(tool_palette_type *tool_palette,int idx,bool selected,bool disabled)
{
	tool_palette->state[idx].selected=selected;
	tool_palette->state[idx].disabled=disabled;
}

/* =======================================================

      Tool Palette Drawing
      
======================================================= */

void tool_palette_draw_icon(int x,int y,unsigned long gl_id,bool is_highlight,bool is_disabled,bool is_pushed)
{
	float			rg_col,b_col,alpha;
	float			vertexes[8],colors[16],uvs[8]={0.0f,0.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1.0f};

		// background

	if (is_pushed) {
		rg_col=0.5f;
		b_col=0.6f;
	}
	else {
		if (!is_highlight) {
			rg_col=b_col=0.95f;
		}
		else {
			rg_col=0.8f;
			b_col=0.9f;
		}
	}

	alpha=is_disabled?0.4f:1.0f;

	vertexes[0]=vertexes[6]=(float)x;
	vertexes[2]=vertexes[4]=(float)(x+tool_button_size);
	vertexes[1]=vertexes[3]=(float)y;
	vertexes[5]=vertexes[7]=(float)(y+tool_button_size);

	colors[0]=colors[1]=colors[4]=colors[5]=rg_col;
	colors[2]=colors[6]=b_col;
	colors[8]=colors[9]=colors[12]=colors[13]=rg_col-0.2f;
	colors[10]=colors[14]=b_col-0.2f;

	colors[3]=colors[7]=colors[11]=colors[15]=alpha;
			
	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4,GL_FLOAT,0,colors);

	glDrawArrays(GL_QUADS,0,4);

	glDisableClientState(GL_COLOR_ARRAY);

		// bitmap

	glColor4f(1.0f,1.0f,1.0f,alpha);
	
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D,gl_id);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,uvs);

	glDrawArrays(GL_QUADS,0,4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_TEXTURE_2D);

		// outline

	vertexes[0]=vertexes[6]=(float)x;
	vertexes[2]=vertexes[4]=(float)(x+tool_button_size);
	vertexes[1]=vertexes[3]=(float)y;
	vertexes[5]=vertexes[7]=(float)(y+(tool_button_size+1));
			
	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(0.4f,0.4f,0.4f,alpha);
	glDrawArrays(GL_LINE_LOOP,0,4);
}

void tool_palette_draw(tool_palette_type *tool_palette)
{
	int				n,k,x;
	float			vertexes[8];
	d3rect			wbox,tbox;
	
	tool_palette_box(&tbox);

		// viewport setup
		
	os_get_window_box(&wbox);

	glEnable(GL_SCISSOR_TEST);
	glScissor(tbox.lx,(wbox.by-tbox.by),(tbox.rx-tbox.lx),(tbox.by-tbox.ty));

	glViewport(tbox.lx,(wbox.by-tbox.by),(tbox.rx-tbox.lx),(tbox.by-tbox.ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)tbox.lx,(GLdouble)tbox.rx,(GLdouble)tbox.by,(GLdouble)tbox.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		// draw setup

	glDisable(GL_DEPTH_TEST);

		// background

	vertexes[0]=vertexes[6]=(float)tbox.lx;
	vertexes[2]=vertexes[4]=(float)tbox.rx;
	vertexes[1]=vertexes[3]=(float)tbox.ty;
	vertexes[5]=vertexes[7]=(float)tbox.by;
			
	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(0.95f,0.95f,0.95f,1.0f);
	glDrawArrays(GL_QUADS,0,4);

		// tools

	x=tbox.lx;

	for (n=0;n!=tool_palette->count;n++) {
	
			// splitter?
			
		if (tool_palette->tools[n].file_name[0]==0x0) {
			x=tbox.rx;
			for (k=(n+1);k<tool_palette->count;k++) {
				x-=tool_button_size;
				if (tool_palette->tools[k].separator) x-=tool_palette_seperator_size;
			}
			continue;
		}
		
			// separator
			
		if (tool_palette->tools[n].separator) x+=tool_palette_seperator_size;
		
			// draw tool
			
		tool_palette_draw_icon(x,tbox.ty,tool_palette->tools[n].bitmap.gl_id,tool_palette->state[n].selected,tool_palette->state[n].disabled,(tool_palette->push_idx==n));
		x+=tool_button_size;
	}
	
		// border

	vertexes[0]=vertexes[4]=(float)tbox.lx;
	vertexes[2]=vertexes[6]=(float)tbox.rx;
#ifdef D3_OS_WINDOWS
	vertexes[1]=vertexes[3]=(float)(tbox.ty+1);		// win32 has white line under menu
#else
	vertexes[1]=vertexes[3]=(float)tbox.ty;
#endif
	vertexes[5]=vertexes[7]=(float)tbox.by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINES,0,4);
}

/* =======================================================

      Tool Palette Clicking
      
======================================================= */

bool tool_palette_click_mouse_down(tool_palette_type *tool_palette,int push_idx,int lx,int ty)
{
	d3pnt				pt;
	
	tool_palette->push_idx=push_idx;
	
	main_wind_draw();

	while (!os_track_mouse_location(&pt,NULL)) {
		if ((pt.x<lx) || (pt.x>=(lx+tool_button_size)) || (pt.y<ty) || (pt.y>=(ty+tool_button_size))) {
			tool_palette->push_idx=-1;
		}
		else {
			tool_palette->push_idx=push_idx;
		}

		main_wind_draw();
		
		usleep(10000);
	}

	if (tool_palette->push_idx==-1) return(FALSE);

	tool_palette->push_idx=-1;
	main_wind_draw();

	return(TRUE);
}

int tool_palette_click_find_index(tool_palette_type *tool_palette,d3pnt *pnt,int *px)
{
	int				n,k,x;
	d3rect			tbox;
	
	tool_palette_box(&tbox);

	x=tbox.lx;

	for (n=0;n!=tool_palette->count;n++) {
	
			// splitter?
			
		if (tool_palette->tools[n].file_name[0]==0x0) {
			x=tbox.rx;
			for (k=(n+1);k<tool_palette->count;k++) {
				x-=tool_button_size;
				if (tool_palette->tools[n].separator) x-=tool_palette_seperator_size;
			}
			continue;
		}
		
			// separator
			
		if (tool_palette->tools[n].separator) x+=tool_palette_seperator_size;

			// skip disabled

		if (tool_palette->state[n].disabled) {
			x+=tool_button_size;
			continue;
		}

			// check click
			
		if ((pnt->x>=x) && (pnt->x<(x+tool_button_size))) {
			*px=x;
			return(n);
		}
		
		x+=tool_button_size;
	}

	return(-1);
}

int tool_palette_click(tool_palette_type *tool_palette,d3pnt *pnt)
{
	int				idx,x;
	d3rect			tbox;

	idx=tool_palette_click_find_index(tool_palette,pnt,&x);
	if (idx==-1) return(-1);
	
	tool_palette_box(&tbox);
	
	if (tool_palette_click_mouse_down(tool_palette,idx,x,tbox.ty)) return(idx);

	return(-1);
}

/* =======================================================

      Tool Palette Mouse Over
      
======================================================= */

void tool_palette_mouse_move(tool_palette_type *tool_palette,d3pnt *pnt)
{
	int				idx,x,mx;
	d3pnt			tpnt;
	d3rect			tbox;
	
	tool_palette_box(&tbox);

	idx=-1;

	if ((pnt->y>=tbox.ty) && (pnt->y<=tbox.by)) {
		idx=tool_palette_click_find_index(tool_palette,pnt,&x);
	}

		// setup tool tip
		
	if (idx==-1) {
		tool_tip_clear();
	}
	else {
		tpnt.x=pnt->x;
		tpnt.y=tbox.by+15;
		
		mx=(tbox.lx+tbox.rx)>>1;
		tool_tip_setup(&tpnt,tool_palette->tools[idx].tool_tip,(x>mx));
	}
}

