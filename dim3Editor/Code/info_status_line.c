/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Info Status Line

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "interface.h"
#include "dialog.h"
#include "common_view.h"

extern int				cx,cy,cz;
extern map_type			map;

extern WindowRef		mainwind;

GrafPtr					info_status_saveport;

/* =======================================================

      Info Status Draw Utility
      
======================================================= */

void info_status_line_draw_start(void)
{
	Rect				wbox;
	RGBColor			blackcolor={0x0,0x0,0x0};
	
	GetPort(&info_status_saveport);
	SetPort(GetWindowPort(mainwind));
	
		// erase
		
 	GetWindowPortBounds(mainwind,&wbox);
	wbox.top=wbox.bottom-info_high;
	EraseRect(&wbox);
	
		// setup text drawing
		
	RGBForeColor(&blackcolor);
	TextSize(10);
	
		// draw the line
		
	MoveTo(wbox.left,wbox.top);
	LineTo(wbox.right,wbox.top);
}

void info_status_line_draw_end(void)
{
	SetPort(info_status_saveport);
}

/* =======================================================

      Info Status Line Drawing
      
======================================================= */

void info_status_line_draw(void)
{
	int				type,main_idx,sub_idx,len;
	char				txt[256];
	Rect				wbox;
	
	info_status_line_draw_start();
	
 	GetWindowPortBounds(mainwind,&wbox);

		// left info
	
	sprintf(txt,"(%d,%d,%d)",cx,cy,cz);
	
	MoveTo((wbox.left+4),(wbox.bottom-4));
	DrawText(txt,0,strlen(txt));
		
		// right info
		
	txt[0]=0x0;
		
	if (select_count()!=1) {
		strcpy(txt,"No piece selected");
	}
	else {
	
		select_get(0,&type,&main_idx,&sub_idx);

		switch (type) {
		
		// supergumba -- need work here
		
			case mesh_piece:
				sprintf(txt,"Mesh: %d",main_idx);
				break;
		
			case liquid_piece:
				sprintf(txt,"Liquid: %d",main_idx);
				break;
		
			case node_piece:
				sprintf(txt,"Node: %s (%d,%d,%d)",map.nodes[main_idx].name,map.nodes[main_idx].pnt.x,map.nodes[main_idx].pnt.y,map.nodes[main_idx].pnt.z);
				break;
				
			case spot_piece:
				sprintf(txt,"Spot: %s (%d,%d,%d)",map.spots[main_idx].name,map.spots[main_idx].pnt.x,map.spots[main_idx].pnt.y,map.spots[main_idx].pnt.z);
				break;
				
			case scenery_piece:
				sprintf(txt,"Scenery: %s (%d,%d,%d)",map.sceneries[main_idx].model_name,map.sceneries[main_idx].pnt.x,map.sceneries[main_idx].pnt.y,map.sceneries[main_idx].pnt.z);
				break;
				
			case light_piece:
				sprintf(txt,"Light: (%d,%d,%d)",map.lights[main_idx].pnt.x,map.lights[main_idx].pnt.y,map.lights[main_idx].pnt.z);
				break;
				
			case sound_piece:
				sprintf(txt,"Sound: %s (%d,%d,%d)",map.sounds[main_idx].name,map.sounds[main_idx].pnt.x,map.sounds[main_idx].pnt.y,map.sounds[main_idx].pnt.z);
				break;
				
			case particle_piece:
				sprintf(txt,"Particle: %s (%d,%d,%d)",map.particles[main_idx].name,map.particles[main_idx].pnt.x,map.particles[main_idx].pnt.y,map.particles[main_idx].pnt.z);
				break;
		}
	}
	
	len=TextWidth(txt,0,strlen(txt));
		
	MoveTo((wbox.right-(len+20)),(wbox.bottom-4));
	DrawText(txt,0,strlen(txt));

	info_status_line_draw_end();
}

/* =======================================================

      Info Status Activity Drawing
      
======================================================= */

void info_status_line_activity(char *str)
{
	Rect				wbox;
	
	info_status_line_draw_start();
	
	if (str!=NULL) {
		GetWindowPortBounds(mainwind,&wbox);
	
		MoveTo((wbox.left+4),(wbox.bottom-4));
		DrawText(str,0,strlen(str));
	}	

	info_status_line_draw_end();
}
