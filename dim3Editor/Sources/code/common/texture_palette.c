/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Texture Palette

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
extern model_type			model;
extern app_state_type		state;

int							txt_palette_cur_page;

/* =======================================================

      Texture Palette Drawing
      
======================================================= */

void texture_palette_draw(void)
{
	int					n,k,x,ty,by,sel,idx,
						page_list_count,per_page_count,page_list_width,pixel_sz;
	float				vertexes[8],uvs[8]={0.0f,0.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1.0f};
	bool				has_texture,is_disabled;
	d3rect				wbox,tbox;
	texture_type		*txt_list;
	texture_type		*texture;
	
		// settings
		
	if (state.mode==app_mode_map) {
		txt_list=map.textures;
		page_list_count=map_texture_palette_page_list_count();
		per_page_count=map_texture_palette_per_page_count();
		page_list_width=map_texture_palette_page_list_width();
		pixel_sz=map_texture_palette_pixel_size();
		sel=map_texture_palette_get_selected_texture();
		is_disabled=map_texture_palette_get_disabled_state();
		map_texture_palette_box(&tbox);
	}
	else {
		txt_list=model.textures;
		page_list_count=model_texture_palette_page_list_count();
		per_page_count=model_texture_palette_per_page_count();
		page_list_width=model_texture_palette_page_list_width();
		pixel_sz=model_texture_palette_pixel_size();
		sel=model_texture_palette_get_selected_texture();
		is_disabled=model_texture_palette_get_disabled_state();
		model_texture_palette_box(&tbox);
	}
	
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
	
		// texture page switch

	if (page_list_width!=0) {
	
		x=tbox.lx;
		ty=tbox.ty;
		by=ty+(pixel_sz>>1);

		for (n=0;n!=page_list_count;n++) {
		
				// time to move down?
				
			if (n==(page_list_count>>1)) {
				x=tbox.lx;
				ty=by;
				by=tbox.ty+pixel_sz;
			}
			
				// draw page

			vertexes[0]=vertexes[6]=(float)x;
			vertexes[2]=vertexes[4]=(float)(x+16);
			vertexes[1]=vertexes[3]=(float)ty;
			vertexes[5]=vertexes[7]=(float)by;
			
			glVertexPointer(2,GL_FLOAT,0,vertexes);
			
			if (txt_palette_cur_page==n) {
				glColor4f(0.3f,0.3f,1.0f,1.0f);
			}
			else {
				glColor4f(0.75f,0.75f,0.75f,1.0f);
			}

			glDrawArrays(GL_QUADS,0,4);

				// outline

			glColor4f(0.0f,0.0f,0.0f,1.0f);
			glDrawArrays(GL_LINE_LOOP,0,4);
			
				// mark if there are textures
				
			has_texture=FALSE;
			idx=n*per_page_count;

			if (!is_disabled) {
				for (k=idx;k!=(idx+per_page_count);k++) {
					if (txt_list[k].frames[0].bitmap.gl_id!=-1) {
						has_texture=TRUE;
						break;
					}
				}
			}
			
			if (has_texture) {
				vertexes[0]=vertexes[6]=(float)(x+11);
				vertexes[2]=vertexes[4]=(float)(x+14);
				vertexes[1]=vertexes[3]=(float)(by-5);
				vertexes[5]=vertexes[7]=(float)(by-2);
			
				glVertexPointer(2,GL_FLOAT,0,vertexes);
				
				glColor4f(0.0f,0.0f,0.0f,1.0f);
				glDrawArrays(GL_QUADS,0,4);
			}
			
			x+=16;
		}
	}

		// textures
		
	ty=tbox.ty+1;
	by=(ty+pixel_sz)-1;

	if (!is_disabled) {
		
		x=tbox.lx+page_list_width;
		
		glColor4f(1.0f,1.0f,1.0f,1.0f);

		glEnable(GL_TEXTURE_2D);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			
		for (n=0;n!=per_page_count;n++) {
			texture=&txt_list[n+(txt_palette_cur_page*per_page_count)];

				// the textures
	
			if (texture->frames[0].bitmap.gl_id!=-1) {
				glBindTexture(GL_TEXTURE_2D,texture->frames[0].bitmap.gl_id);

				vertexes[0]=vertexes[6]=(float)x;
				vertexes[2]=vertexes[4]=(float)(x+pixel_sz);
				vertexes[1]=vertexes[3]=(float)ty;
				vertexes[5]=vertexes[7]=(float)by;

				glVertexPointer(2,GL_FLOAT,0,vertexes);
				glTexCoordPointer(2,GL_FLOAT,0,uvs);

				glDrawArrays(GL_QUADS,0,4);
			}

			x+=pixel_sz;
		}

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		
		glDisable(GL_TEXTURE_2D);
	}

		// right fill
	
	x=(tbox.lx+page_list_width)+(per_page_count*pixel_sz);

	vertexes[0]=vertexes[6]=(float)x;
	vertexes[2]=vertexes[4]=(float)tbox.rx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);
		
	glColor4f(0.75f,0.75f,0.75f,1.0f);
	glDrawArrays(GL_QUADS,0,4);
	
		// lines
		
	x=tbox.lx+page_list_width;
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	for (n=0;n!=per_page_count;n++) {
		idx=n+(txt_palette_cur_page*per_page_count);
	
		vertexes[0]=vertexes[6]=(float)x;
		vertexes[2]=vertexes[4]=(float)(x+pixel_sz);
		vertexes[1]=vertexes[3]=(float)ty;
		vertexes[5]=vertexes[7]=(float)by;

		glVertexPointer(2,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINE_LOOP,0,4);
		
		x+=pixel_sz;
	}

	vertexes[0]=(float)tbox.lx;
	vertexes[2]=(float)tbox.rx;
	vertexes[1]=vertexes[3]=(float)ty;
	
	glVertexPointer(2,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINES,0,2);
	
		// selection
	
	if (sel==-1) return;
	if ((sel<(txt_palette_cur_page*per_page_count)) || (sel>=((txt_palette_cur_page+1)*per_page_count))) return;
	
	x=((sel-(txt_palette_cur_page*per_page_count))*pixel_sz)+(tbox.lx+page_list_width);
	
	vertexes[0]=vertexes[6]=(float)x;
	vertexes[2]=vertexes[4]=(float)(x+pixel_sz);
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(1.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINE_LOOP,0,4);
}

/* =======================================================

      Texture Palette Clicking
      
======================================================= */

void texture_palette_click(d3pnt *pnt,bool double_click)
{
	int				nsel,page,
					page_list_count,per_page_count,page_list_width,pixel_sz;
	d3rect			tbox;
	texture_type	*txt_list;

		// settings
		
	if (state.mode==app_mode_map) {
		if (map_texture_palette_get_disabled_state()) return;

		txt_list=map.textures;
		page_list_count=map_texture_palette_page_list_count();
		per_page_count=map_texture_palette_per_page_count();
		page_list_width=map_texture_palette_page_list_width();
		pixel_sz=map_texture_palette_pixel_size();
		map_texture_palette_box(&tbox);
	}
	else {
		if (model_texture_palette_get_disabled_state()) return;

		txt_list=model.textures;
		page_list_count=model_texture_palette_page_list_count();
		per_page_count=model_texture_palette_per_page_count();
		page_list_width=model_texture_palette_page_list_width();
		pixel_sz=model_texture_palette_pixel_size();
		model_texture_palette_box(&tbox);
	}
	
		// move within palette
		
	pnt->x-=tbox.lx;
	pnt->y-=tbox.ty;
	
		// texture page change
		
	if (pnt->x<page_list_width) {
		page=((pnt->y/(pixel_sz>>1))*(page_list_count>>1))+(pnt->x/16);
		if (txt_palette_cur_page!=page) {
			txt_palette_cur_page=page;
			main_wind_draw();
		}
		return;
	}
	
		// find clicked texture
		
	nsel=(pnt->x-page_list_width)/pixel_sz;
	nsel+=(txt_palette_cur_page*per_page_count);
		
		// double clicks
		
	if (double_click) {
		texture_edit_switch(nsel);
		main_wind_draw();
		return;
	}
    
		// single clicks
		
	if (txt_list[nsel].frames[0].bitmap.gl_id==-1) return;
	
	if (state.mode==app_mode_map) {
		map_texture_palette_put_selected_texture(nsel);
	}
	else {
		model_texture_palette_put_selected_texture(nsel);
	}

	main_wind_draw();
}

