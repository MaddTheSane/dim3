/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Portal Vertex Lists

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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "lights.h"

extern map_type			map;
extern view_type		view;
extern setup_type		setup;

/* =======================================================

      Compile Portal OpenGL Lists
      
======================================================= */

void portal_compile_gl_lists(int tick,int rn)
{
	int						n,nvlist;
	float					fx,fy,fz;
	float					*pv,*pp,*pc;
	portal_type				*portal;
	portal_vertex_list_type	*vl;
	
	portal=&map.portals[rn];

		// the arrays

	vl=portal->vertexes.vertex_list;
	nvlist=portal->vertexes.nvlist;

	pv=(float*)portal->vertexes.pvert;
	pp=(float*)portal->vertexes.pcoord;
	pc=(float*)portal->vertexes.pcolor;

		// the eye offset

	fx=(float)view.camera.pos.x;
	fy=(float)view.camera.pos.y;
	fz=(float)view.camera.pos.z;

		// check if lights or vertex data has changed in
		// room.  If not, only run through the vertexes
		// instead of recreating the lights

	if (!map_portal_light_check_changes(portal)) {

		for (n=0;n!=nvlist;n++) {
			*pv++=(vl->x-fx);
			*pv++=(vl->y-fy);
			*pv++=(fz-vl->z);
			*pp++=vl->gx;
			*pp++=vl->gy;

			vl++;
		}
		
		return;
	}
	
		// run ray-traced lighting if option is
		// turned on
		
	if (setup.ray_trace_lighting) {

		for (n=0;n!=nvlist;n++) {
			light_trace_calculate_light_color(portal,vl->x,vl->y,vl->z,pc);
			pc+=3;

			*pv++=(vl->x-fx);
			*pv++=(vl->y-fy);
			*pv++=(fz-vl->z);
			*pp++=vl->gx;
			*pp++=vl->gy;

			vl++;
		}
		
		return;
	}

		// run regular lighting

	for (n=0;n!=nvlist;n++) {
		map_portal_calculate_light_color(portal,(double)vl->x,(double)vl->y,(double)vl->z,pc);
		pc+=3;

		*pv++=(vl->x-fx);
		*pv++=(vl->y-fy);
		*pv++=(fz-vl->z);
		*pp++=vl->gx;
		*pp++=vl->gy;

		vl++;
	}
}

/* =======================================================

      Use Portal OpenGL Lists
      
======================================================= */

void portal_compile_gl_list_attach(int rn,int txt_unit_count)
{
	int						nvlist,sz;
	portal_type				*portal;
	
	portal=&map.portals[rn];
	nvlist=portal->vertexes.nvlist;
	
	sz=(nvlist*3)*sizeof(float);

		// vertexes
		
#ifdef D3_OS_MAC
	glVertexArrayRangeAPPLE(sz,portal->vertexes.pvert);
	glEnableClientState(GL_VERTEX_ARRAY_RANGE_APPLE);
#endif

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,portal->vertexes.pvert);

		// uvs
		
	if (txt_unit_count>=3) {
		glClientActiveTexture(GL_TEXTURE2);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,0,portal->vertexes.pcoord);
	}

	if (txt_unit_count>=2) {
		glClientActiveTexture(GL_TEXTURE1);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,0,portal->vertexes.pcoord);
	}
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,portal->vertexes.pcoord);

		// color array

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,portal->vertexes.pcolor);

#ifdef D3_OS_MAC
	glFlushVertexArrayRangeAPPLE(sz,portal->vertexes.pvert);
#endif
}

void portal_compile_gl_list_dettach(void)
{
	glDisableClientState(GL_COLOR_ARRAY);

	glClientActiveTexture(GL_TEXTURE2);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_VERTEX_ARRAY);

#ifdef D3_OS_MAC
	glDisableClientState(GL_VERTEX_ARRAY_RANGE_APPLE);
#endif
}
