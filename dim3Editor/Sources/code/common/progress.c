/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Progress Indicator

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

int					progress_count,progress_pos;
char				progress_title[256];

/* =======================================================

      Progress Drawing
      
======================================================= */

void progress_draw(void)
{
	int					lx,rx,mx,ty,by,my;
	float				vertexes[8*2],colors[8*4];
	d3rect				wbox;

		// draw the main window

	main_wind_draw_no_swap();

		// draw the progress

	os_get_window_box(&wbox);

	glEnable(GL_SCISSOR_TEST);
	glScissor(wbox.lx,0,(wbox.rx-wbox.lx),(wbox.by-wbox.ty));

	glViewport(wbox.lx,0,(wbox.rx-wbox.lx),(wbox.by-wbox.ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)wbox.lx,(GLdouble)wbox.rx,(GLdouble)wbox.by,(GLdouble)wbox.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		// draw setup

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		// background

	vertexes[0]=vertexes[6]=(float)wbox.lx;
	vertexes[2]=vertexes[4]=(float)wbox.rx;
	vertexes[1]=vertexes[3]=(float)wbox.ty;
	vertexes[5]=vertexes[7]=(float)wbox.by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(0.0f,0.0f,0.0f,0.5f);
	glDrawArrays(GL_QUADS,0,4);

		// progress box

	lx=wbox.lx+50;
	rx=wbox.rx-50;
	ty=((wbox.ty+wbox.by)>>1)-30;
	by=ty+60;

	vertexes[0]=vertexes[6]=(float)lx;
	vertexes[2]=vertexes[4]=(float)rx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glDrawArrays(GL_QUADS,0,4);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINE_LOOP,0,4);

		// progress bar background

	glEnableClientState(GL_COLOR_ARRAY);

	lx+=10;
	rx-=10;
	by-=10;
	ty=by-15;

	mx=lx+((rx-lx)*progress_pos)/progress_count;
	my=(ty+by)>>1;

	vertexes[0]=vertexes[6]=vertexes[8]=vertexes[14]=(float)lx;
	vertexes[2]=vertexes[4]=vertexes[10]=vertexes[12]=(float)rx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=vertexes[13]=vertexes[9]=vertexes[11]=(float)my;
	vertexes[13]=vertexes[15]=(float)by;

	colors[0]=colors[4]=colors[24]=colors[28]=0.7f;
	colors[8]=colors[12]=colors[16]=colors[20]=0.5f;
	colors[1]=colors[5]=colors[25]=colors[29]=0.7f;
	colors[9]=colors[13]=colors[17]=colors[21]=0.5f;
	colors[2]=colors[6]=colors[26]=colors[30]=0.7f;
	colors[10]=colors[14]=colors[18]=colors[22]=0.5f;
	colors[3]=colors[7]=colors[11]=colors[15]=colors[19]=colors[23]=colors[27]=colors[31]=1.0f;

	glVertexPointer(2,GL_FLOAT,0,vertexes);
	glColorPointer(4,GL_FLOAT,0,colors);

	glDrawArrays(GL_QUADS,0,8);

		// progress bar foreground

	vertexes[0]=vertexes[6]=vertexes[8]=vertexes[14]=(float)lx;
	vertexes[2]=vertexes[4]=vertexes[10]=vertexes[12]=(float)mx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=vertexes[13]=vertexes[9]=vertexes[11]=(float)my;
	vertexes[13]=vertexes[15]=(float)by;

	colors[0]=colors[4]=colors[24]=colors[28]=0.0f;
	colors[8]=colors[12]=colors[16]=colors[20]=0.0f;
	colors[1]=colors[5]=colors[25]=colors[29]=0.75f;
	colors[9]=colors[13]=colors[17]=colors[21]=1.0f;
	colors[2]=colors[6]=colors[26]=colors[30]=0.0f;
	colors[10]=colors[14]=colors[18]=colors[22]=0.0f;
	colors[3]=colors[7]=colors[11]=colors[15]=colors[19]=colors[23]=colors[27]=colors[31]=1.0f;

	glVertexPointer(2,GL_FLOAT,0,vertexes);
	glColorPointer(4,GL_FLOAT,0,colors);

	glDrawArrays(GL_QUADS,0,8);

	glDisableClientState(GL_COLOR_ARRAY);

		// bar borders

	glColor4f(0.0f,0.0f,0.0f,1.0f);

	vertexes[0]=vertexes[6]=(float)lx;
	vertexes[2]=vertexes[4]=(float)mx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINE_LOOP,0,4);

	vertexes[0]=vertexes[6]=(float)lx;
	vertexes[2]=vertexes[4]=(float)rx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINE_LOOP,0,4);

		// title

	text_draw_center(((lx+rx)>>1),(ty-5),20.0f,NULL,progress_title);

	os_swap_gl_buffer();
}

/* =======================================================

      Progress Running
      
======================================================= */

void progress_start(char *title,int count)
{
	progress_count=count;
	progress_pos=0;

	strcpy(progress_title,title);

	progress_draw();
}

void progress_end(void)
{
	main_wind_draw();
}

void progress_next(void)
{
	progress_pos++;
	if (progress_pos>progress_count) progress_pos=progress_count;

	progress_draw();
}

void progress_next_title(char *title)
{
	strcpy(progress_title,title);
	progress_next();
}
