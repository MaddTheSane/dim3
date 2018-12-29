/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Primitives

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
	#include "dim3engine.h"
#endif

#include "interface.h"

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

/* =======================================================

      Screen Tint
      
======================================================= */

void view_primitive_2D_tint_screen(d3col *col,float alpha)
{
	float			*vp;
	
	view_bind_utility_vertex_object();
	vp=(float*)view_map_utility_vertex_object();

	*vp++=0.0f;
	*vp++=0.0f;
	*vp++=0.0f;
	*vp++=(float)view.screen.y_sz;
	*vp++=(float)view.screen.x_sz;
	*vp++=0.0f;
	*vp++=(float)view.screen.x_sz;
	*vp++=(float)view.screen.y_sz;

	view_unmap_utility_vertex_object();

	gl_shader_draw_execute_simple_color_start(2,0,col,alpha);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	gl_shader_draw_execute_simple_color_end();

	view_unbind_utility_vertex_object();
}

/* =======================================================

      2D Fills
      
======================================================= */

void view_primitive_2D_color_poly(int x0,int y0,d3col *col0,int x1,int y1,d3col *col1,int x2,int y2,d3col *col2,int x3,int y3,d3col *col3,float alpha)
{
	float			*vp;
	unsigned char	*cp,uc_alpha;

	view_bind_utility_vertex_object();
	vp=(float*)view_map_utility_vertex_object();

	uc_alpha=(unsigned char)(alpha*255.0f);
								
	*vp++=(float)x0;
	*vp++=(float)y0;
	*vp++=(float)x3;
	*vp++=(float)y3;
	*vp++=(float)x1;
	*vp++=(float)y1;
	*vp++=(float)x2;
	*vp++=(float)y2;

	cp=(unsigned char*)vp;
	*cp++=(unsigned char)(col0->r*255.0f);
	*cp++=(unsigned char)(col0->g*255.0f);
	*cp++=(unsigned char)(col0->b*255.0f);
	*cp++=uc_alpha;
	*cp++=(unsigned char)(col3->r*255.0f);
	*cp++=(unsigned char)(col3->g*255.0f);
	*cp++=(unsigned char)(col3->b*255.0f);
	*cp++=uc_alpha;
	*cp++=(unsigned char)(col1->r*255.0f);
	*cp++=(unsigned char)(col1->g*255.0f);
	*cp++=(unsigned char)(col1->b*255.0f);
	*cp++=uc_alpha;
	*cp++=(unsigned char)(col2->r*255.0f);
	*cp++=(unsigned char)(col2->g*255.0f);
	*cp++=(unsigned char)(col2->b*255.0f);
	*cp++=uc_alpha;

	view_unmap_utility_vertex_object();
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		// draw the polygon

	gl_shader_draw_execute_simple_gradient_start(2,0,((4*2)*sizeof(float)));
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	gl_shader_draw_execute_simple_gradient_end();
		
		// finish draw

	glDisable(GL_BLEND);

	view_unbind_utility_vertex_object();
}

void view_primitive_2D_color_quad(d3col *col,float alpha,int lft,int rgt,int top,int bot)
{
	view_primitive_2D_color_poly(lft,top,col,rgt,top,col,rgt,bot,col,lft,bot,col,alpha);
}

void view_primitive_2D_color_trig(d3col *col,float alpha,int lft,int rgt,int top,int bot,int dir)
{
	float			*vp;

	view_bind_utility_vertex_object();
	vp=(float*)view_map_utility_vertex_object();

	switch (dir) {

		case 0:
			*vp++=(float)lft;
			*vp++=(float)bot;
			*vp++=(float)rgt;
			*vp++=(float)bot;
			*vp++=((float)(lft+rgt))*0.5f;
			*vp++=(float)top;
			break;

		case 1:
			*vp++=(float)lft;
			*vp++=(float)top;
			*vp++=(float)rgt;
			*vp++=((float)(top+bot))*0.5f;
			*vp++=(float)lft;
			*vp++=(float)bot;
			break;

		case 2:
			*vp++=(float)lft;
			*vp++=(float)top;
			*vp++=(float)rgt;
			*vp++=(float)top;
			*vp++=((float)(lft+rgt))*0.5f;
			*vp++=(float)bot;
			break;

		case 3:
			*vp++=(float)rgt;
			*vp++=(float)top;
			*vp++=(float)lft;
			*vp++=((float)(top+bot))*0.5f;
			*vp++=(float)rgt;
			*vp++=(float)bot;
			break;
	}

	view_unmap_utility_vertex_object();

		// setup draw
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		// draw the trig
		
	gl_shader_draw_execute_simple_color_start(2,0,col,alpha);
	glDrawArrays(GL_TRIANGLES,0,3);
	gl_shader_draw_execute_simple_color_end();
	
		// finish draw

	glDisable(GL_BLEND);

	view_unbind_utility_vertex_object();
}

void view_primitive_2D_color_arc(d3col *out_col,d3col *in_col,float alpha,int lft,int rgt,int top,int bot,float start_perc,float end_perc)
{
	float			mx,my,fx,fy,start_rad,end_rad;
	float			*vp;
	unsigned char	*cp,uc_alpha;

		// get angles

	mx=(float)((lft+rgt)>>1);
	my=(float)((top+bot)>>1);

	fx=(float)((rgt-lft)>>1);
	fy=(float)((bot-top)>>1);

	start_rad=(TRIG_PI*2.0f)*start_perc;
	end_rad=(TRIG_PI*2.0f)*end_perc;

		// vertexes and colors

	view_bind_utility_vertex_object();
	vp=(float*)view_map_utility_vertex_object();

	uc_alpha=(unsigned char)(alpha*255.0f);

	*vp++=mx;
	*vp++=my;
	*vp++=mx+(fx*sinf(start_rad));
	*vp++=my-(fy*cosf(start_rad));
	*vp++=mx+(fx*sinf(end_rad));
	*vp++=my-(fy*cosf(end_rad));

	cp=(unsigned char*)vp;
	*cp++=(unsigned char)(in_col->r*255.0f);
	*cp++=(unsigned char)(in_col->g*255.0f);
	*cp++=(unsigned char)(in_col->b*255.0f);
	*cp++=uc_alpha;
	*cp++=(unsigned char)(out_col->r*255.0f);
	*cp++=(unsigned char)(out_col->g*255.0f);
	*cp++=(unsigned char)(out_col->b*255.0f);
	*cp++=uc_alpha;
	*cp++=(unsigned char)(out_col->r*255.0f);
	*cp++=(unsigned char)(out_col->g*255.0f);
	*cp++=(unsigned char)(out_col->b*255.0f);
	*cp++=uc_alpha;

	view_unmap_utility_vertex_object();

		// setup draw
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		// draw the trig
		
	gl_shader_draw_execute_simple_gradient_start(2,0,((3*2)*sizeof(float)));
	glDrawArrays(GL_TRIANGLES,0,3);
	gl_shader_draw_execute_simple_gradient_end();
	
		// finish draw

	glDisable(GL_BLEND);

	view_unbind_utility_vertex_object();
}

/* =======================================================

      2D Lines
      
======================================================= */

void view_primitive_2D_line(d3col *col,float alpha,int x0,int y0,int x1,int y1)
{
	float			*vp;

	view_bind_utility_vertex_object();
	vp=(float*)view_map_utility_vertex_object();

	*vp++=(float)x0;
	*vp++=(float)y0;
	*vp++=(float)x1;
	*vp++=(float)y1;

	view_unmap_utility_vertex_object();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		
	gl_shader_draw_execute_simple_color_start(2,0,col,alpha);
	glDrawArrays(GL_LINES,0,2);
	gl_shader_draw_execute_simple_color_end();

	glDisable(GL_BLEND);

	view_unbind_utility_vertex_object();
}

void view_primitive_2D_line_poly(d3col *col,float alpha,int x0,int y0,int x1,int y1,int x2,int y2,int x3,int y3)
{
	float			*vp;

	view_bind_utility_vertex_object();
	vp=(float*)view_map_utility_vertex_object();

	*vp++=(float)x0;
	*vp++=(float)y0;
	*vp++=(float)x1;
	*vp++=(float)y1;
	*vp++=(float)x2;
	*vp++=(float)y2;
	*vp++=(float)x3;
	*vp++=(float)y3;

	view_unmap_utility_vertex_object();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	gl_shader_draw_execute_simple_color_start(2,0,col,alpha);
	glDrawArrays(GL_LINE_LOOP,0,4);
	gl_shader_draw_execute_simple_color_end();

	glDisable(GL_BLEND);

	view_unbind_utility_vertex_object();
}

void view_primitive_2D_line_quad(d3col *col,float alpha,int lft,int rgt,int top,int bot)
{
	view_primitive_2D_line_poly(col,alpha,lft,top,rgt,top,rgt,bot,lft,bot);
}

void view_primitive_2D_line_trig(d3col *col,float alpha,int lft,int rgt,int top,int bot,int dir)
{
	float			*vp;
	
		// get the vertexes

	view_bind_utility_vertex_object();
	vp=(float*)view_map_utility_vertex_object();

	switch (dir) {

		case 0:
			*vp++=(float)lft;
			*vp++=(float)bot;
			*vp++=(float)rgt;
			*vp++=(float)bot;
			*vp++=((float)(lft+rgt))*0.5f;
			*vp++=(float)top;
			break;

		case 1:
			*vp++=(float)lft;
			*vp++=(float)top;
			*vp++=(float)rgt;
			*vp++=((float)(top+bot))*0.5f;
			*vp++=(float)lft;
			*vp++=(float)bot;
			break;

		case 2:
			*vp++=(float)lft;
			*vp++=(float)top;
			*vp++=(float)rgt;
			*vp++=(float)top;
			*vp++=((float)(lft+rgt))*0.5f;
			*vp++=(float)bot;
			break;

		case 3:
			*vp++=(float)rgt;
			*vp++=(float)top;
			*vp++=(float)lft;
			*vp++=((float)(top+bot))*0.5f;
			*vp++=(float)rgt;
			*vp++=(float)bot;
			break;
	}

	view_unmap_utility_vertex_object();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		
	gl_shader_draw_execute_simple_color_start(2,0,col,alpha);
	glDrawArrays(GL_LINE_LOOP,0,3);
	gl_shader_draw_execute_simple_color_end();

	glDisable(GL_BLEND);

	view_unbind_utility_vertex_object();
}

/* =======================================================

      3D Lines
      
======================================================= */

void view_primitive_3D_line(d3col *col,float alpha,int x0,int y0,int z0,int x1,int y1,int z1)
{
	float			*vp;

	view_bind_utility_vertex_object();
	vp=(float*)view_map_utility_vertex_object();

	*vp++=(float)x0;
	*vp++=(float)y0;
	*vp++=(float)z0;
	*vp++=(float)x1;
	*vp++=(float)y1;
	*vp++=(float)z1;

	view_unmap_utility_vertex_object();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		
	gl_shader_draw_execute_simple_color_start(3,0,col,alpha);
	glDrawArrays(GL_LINES,0,2);
	gl_shader_draw_execute_simple_color_end();

	glDisable(GL_BLEND);

	view_unbind_utility_vertex_object();
}

void view_primitive_3D_line_cube(d3col *col,float alpha,int *px,int *py,int *pz)
{
	int				n;
	unsigned short	cube_indexes[24]={0,1,1,2,2,3,3,0,4,5,5,6,6,7,7,4,0,4,1,5,2,6,3,7};
	float			*vp;

		// get the vertexes

	view_bind_utility_vertex_object();
	vp=(float*)view_map_utility_vertex_object();
	
	for (n=0;n!=8;n++) {
		*vp++=(float)px[n];
		*vp++=(float)py[n];
		*vp++=(float)pz[n];
	}

	view_unmap_utility_vertex_object();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		
	gl_shader_draw_execute_simple_color_start(3,0,col,alpha);
	glDrawElements(GL_LINES,24,GL_UNSIGNED_SHORT,(GLvoid*)cube_indexes);
	gl_shader_draw_execute_simple_color_end();

	glDisable(GL_BLEND);

	view_unbind_utility_vertex_object();
}

/* =======================================================

      Textured Fills
      
======================================================= */

void view_primitive_2D_texture_quad(GLuint gl_id,d3col *col,float alpha,int lft,int rgt,int top,int bot,float gx,float gx2,float gy,float gy2,bool clamp)
{
	float			*vp;
	d3col			col2,*col_ptr;

		// setup the data

	view_bind_utility_vertex_object();
	vp=(float*)view_map_utility_vertex_object();

	*vp++=(float)lft;
	*vp++=(float)top;
	*vp++=gx;
	*vp++=gy;

	*vp++=(float)lft;
	*vp++=(float)bot;
	*vp++=gx;
	*vp++=gy2;

	*vp++=(float)rgt;
	*vp++=(float)top;
	*vp++=gx2;
	*vp++=gy;

	*vp++=(float)rgt;
	*vp++=(float)bot;
	*vp++=gx2;
	*vp++=gy2;

	if (col==NULL) {
		col2.r=col2.g=col2.b=1.0f;
		col_ptr=&col2;
	}
	else {
		col_ptr=col;
	}

	view_unmap_utility_vertex_object();

		// setup texture draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	gl_shader_draw_execute_simple_bitmap_set_texture(gl_id);

	if (clamp) {
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	}

		// draw the quad

	gl_shader_draw_execute_simple_bitmap_start(2,0,(2*sizeof(float)),((2+2)*sizeof(float)),col_ptr,alpha);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

		// finish texture draw
		
	if (clamp) {
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	}
	
	gl_shader_draw_execute_simple_bitmap_end();

	glDisable(GL_BLEND);

	view_unbind_utility_vertex_object();
}

void view_primitive_2D_texture_quad_rot(GLuint gl_id,d3col *col,float alpha,int lft,int rgt,int top,int bot,float ang,float gx,float gx2,float gy,float gy2)
{
	int				px[4],py[4];
	float			*vp;
	d3col			col2,*col_ptr;
	
	px[0]=px[3]=lft;
	px[1]=px[2]=rgt;
	py[0]=py[1]=top;
	py[2]=py[3]=bot;

	if (ang!=0.0f) rotate_2D_polygon(4,px,py,((lft+rgt)>>1),((top+bot)>>1),ang);

		// setup the data

	view_bind_utility_vertex_object();
	vp=(float*)view_map_utility_vertex_object();

	*vp++=(float)px[0];
	*vp++=(float)py[0];
	*vp++=gx;
	*vp++=gy;

	*vp++=(float)px[3];
	*vp++=(float)py[3];
	*vp++=gx;
	*vp++=gy2;

	*vp++=(float)px[1];
	*vp++=(float)py[1];
	*vp++=gx2;
	*vp++=gy;

	*vp++=(float)px[2];
	*vp++=(float)py[2];
	*vp++=gx2;
	*vp++=gy2;

	if (col==NULL) {
		col2.r=col2.g=col2.b=1.0f;
		col_ptr=&col2;
	}
	else {
		col_ptr=col;
	}

	view_unmap_utility_vertex_object();

		// setup texture draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		// draw the quad

	gl_shader_draw_execute_simple_bitmap_set_texture(gl_id);
	gl_shader_draw_execute_simple_bitmap_start(2,0,(2*sizeof(float)),((2+2)*sizeof(float)),col_ptr,alpha);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	gl_shader_draw_execute_simple_bitmap_end();

		// finish texture draw

	glDisable(GL_BLEND);

	view_unbind_utility_vertex_object();
}

