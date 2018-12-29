/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Tool Tips

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

char							tool_tip_str[256];
d3rect							tool_tip_box;


/* =======================================================

      Setup Tool Tip
      
======================================================= */

void tool_tip_initialize(void)
{
	tool_tip_str[0]=0x0;
}

void tool_tip_setup(d3pnt *pnt,char *str,bool right)
{
	int				wid,high;

		// will we need a redraw?

	if (strcmp(str,tool_tip_str)==0) return;

		// left or right?
		
	strcpy(tool_tip_str,str);

	wid=text_width(tool_tip_font_size,str)+4;
	high=(int)(tool_tip_font_size+2);

	if (!right) {
		tool_tip_box.lx=pnt->x;
		tool_tip_box.rx=tool_tip_box.lx+(6+wid);
	}
	else {
		tool_tip_box.rx=pnt->x;
		tool_tip_box.lx=tool_tip_box.rx-(6+wid);
	}

	tool_tip_box.ty=pnt->y-((high>>1)+2);
	tool_tip_box.by=pnt->y+((high>>1)+2);

		// need redraw

	main_wind_draw();
}

void tool_tip_clear(void)
{
	bool			redraw;
	
	redraw=(tool_tip_str[0]!=0x0);
	
	tool_tip_str[0]=0x0;
	
	if (redraw) main_wind_draw();
}

/* =======================================================

      Tool Palette Drawing
      
======================================================= */

void tool_tip_draw(void)
{
	float			vertexes[8];
	d3rect			wbox;
	
	if (tool_tip_str[0]==0x0) return;

		// viewport setup
		
	os_get_window_box(&wbox);
	
	glEnable(GL_SCISSOR_TEST);
	glScissor(wbox.lx,(wbox.by-wbox.by),(wbox.rx-wbox.lx),(wbox.by-wbox.ty));

	glViewport(wbox.lx,(wbox.by-wbox.by),(wbox.rx-wbox.lx),(wbox.by-wbox.ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)wbox.lx,(GLdouble)wbox.rx,(GLdouble)wbox.by,(GLdouble)wbox.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		// draw setup

	glDisable(GL_DEPTH_TEST);

		// background and line

	vertexes[0]=vertexes[6]=(float)tool_tip_box.lx;
	vertexes[2]=vertexes[4]=(float)tool_tip_box.rx;
	vertexes[1]=vertexes[3]=(float)tool_tip_box.ty;
	vertexes[5]=vertexes[7]=(float)tool_tip_box.by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(1.0f,1.0f,0.5f,0.9f);
	glDrawArrays(GL_QUADS,0,4);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINE_LOOP,0,4);
	
		// the tip

	text_draw((tool_tip_box.lx+4),(tool_tip_box.by-2),tool_tip_font_size,NULL,tool_tip_str);
}

