/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Auto Generate Textures Utilities

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
	#include "dim3baseutility.h"
#endif

#define bitmap_ag_outline_darken_factor				0.7f

/* =======================================================

      AG Bitmap Random
      
======================================================= */

int bitmap_ag_random_int(int max)
{
	float			f;

	f=(float)random()/(float)RAND_MAX;
	return((int)(f*(float)max));
}

bool bitmap_ag_random_bool(void)
{
	return(bitmap_ag_random_int(100)>=50);
}

void bitmap_ag_random_color(d3col *col,int min_r,int min_g,int min_b,int max_r,int max_g,int max_b)
{
	col->r=((float)(min_r+(bitmap_ag_random_int(max_r-min_r))))/255.0f;
	col->g=((float)(min_g+(bitmap_ag_random_int(max_g-min_g))))/255.0f;
	col->b=((float)(min_b+(bitmap_ag_random_int(max_b-min_b))))/255.0f;
}

void bitmap_ag_random_color_lock(d3col *col,int min,int max)
{
	float		f;

	f=((float)(min+(bitmap_ag_random_int(max-min))))/255.0f;
	col->r=col->g=col->b=f;
}

void bitmap_ag_random_color_change(d3col *base_col,d3col *col,float fct)
{
	float		f;

	f=(((float)(100-bitmap_ag_random_int(200)))*0.01f)*fct;

	col->r=base_col->r+f;
	if (col->r<0.0f) col->r=0.0f;
	if (col->r>1.0f) col->r=1.0f;

	col->g=base_col->g+f;
	if (col->g<0.0f) col->g=0.0f;
	if (col->g>1.0f) col->g=1.0f;

	col->b=base_col->b+f;
	if (col->b<0.0f) col->b=0.0f;
	if (col->b>1.0f) col->b=1.0f;
}

/* =======================================================

      Bitmap Create/Save
      
======================================================= */

bool bitmap_ag_texture_create(bitmap_ag_type *ag_bitmap,bool has_glow)
{
	int				n,pixel_count,byte_len,kr,kg,kb;
	unsigned char	*p_ptr,*b_ptr,*s_ptr,*g_ptr;
	
	ag_bitmap->frame->bitmap.wid=ag_bitmap->frame->bitmap.high=ag_bitmap->pixel_sz;
	ag_bitmap->frame->bitmap.opaque=TRUE;

	ag_bitmap->frame->bumpmap.wid=ag_bitmap->frame->bumpmap.high=ag_bitmap->pixel_sz;
	ag_bitmap->frame->bumpmap.opaque=TRUE;

	ag_bitmap->frame->specularmap.wid=ag_bitmap->frame->specularmap.high=ag_bitmap->pixel_sz;
	ag_bitmap->frame->specularmap.opaque=TRUE;

	ag_bitmap->frame->glowmap.wid=ag_bitmap->frame->glowmap.high=ag_bitmap->pixel_sz;
	ag_bitmap->frame->glowmap.opaque=TRUE;
	
		// get the data

	pixel_count=ag_bitmap->frame->bitmap.wid*ag_bitmap->frame->bitmap.high;
	byte_len=pixel_count*3;

	ag_bitmap->png_data=malloc(byte_len);
	if (ag_bitmap->png_data==NULL) return(FALSE);

	ag_bitmap->bump_data=malloc(byte_len);
	if (ag_bitmap->bump_data==NULL) {
		free(ag_bitmap->png_data);
		return(FALSE);
	}

	ag_bitmap->spec_data=malloc(byte_len);
	if (ag_bitmap->spec_data==NULL) {
		free(ag_bitmap->bump_data);
		free(ag_bitmap->png_data);
		return(FALSE);
	}
	
	ag_bitmap->glow_data=NULL;
	
	if (has_glow) {
		ag_bitmap->glow_data=malloc(byte_len);
		if (ag_bitmap->glow_data==NULL) {
			free(ag_bitmap->glow_data);
			free(ag_bitmap->bump_data);
			free(ag_bitmap->png_data);
			return(FALSE);
		}
	}

		// fill the png

	kr=(int)(((float)0xFF)*ag_bitmap->back_col.r);
	kg=(int)(((float)0xFF)*ag_bitmap->back_col.g);
	kb=(int)(((float)0xFF)*ag_bitmap->back_col.b);
	
	p_ptr=ag_bitmap->png_data;
	b_ptr=ag_bitmap->bump_data;
	s_ptr=ag_bitmap->spec_data;
	if (has_glow) g_ptr=ag_bitmap->glow_data;

	for (n=0;n!=pixel_count;n++) {
		*p_ptr++=kr;
		*p_ptr++=kg;
		*p_ptr++=kb;
		*b_ptr++=0x0;
		*b_ptr++=0x0;
		*b_ptr++=0xFF;
		*s_ptr++=0x0;
		*s_ptr++=0x0;
		*s_ptr++=0x0;
		if (has_glow) {
			*g_ptr++=0x0;
			*g_ptr++=0x0;
			*g_ptr++=0x0;
		}
	}

		// default the clip

	ag_bitmap->clip_lx=0;
	ag_bitmap->clip_rx=ag_bitmap->pixel_sz;
	ag_bitmap->clip_ty=0;
	ag_bitmap->clip_by=ag_bitmap->pixel_sz;

	return(TRUE);
}

bool bitmap_ag_texture_finish(bitmap_ag_type *ag_bitmap,char *base_path)
{
	bool		ok;
	char		path[1024];

		// save file to disk
		
	sprintf(path,"%s/%s.png",base_path,ag_bitmap->frame->name);
	ok=bitmap_write_png_data(ag_bitmap->png_data,ag_bitmap->pixel_sz,ag_bitmap->pixel_sz,FALSE,path);

	if (!ag_bitmap->no_bump_spec) {
		if (ok) {
			sprintf(path,"%s/%s_n.png",base_path,ag_bitmap->frame->name);
			ok=bitmap_write_png_data(ag_bitmap->bump_data,ag_bitmap->pixel_sz,ag_bitmap->pixel_sz,FALSE,path);
		}

		if (ok) {
			sprintf(path,"%s/%s_s.png",base_path,ag_bitmap->frame->name);
			ok=bitmap_write_png_data(ag_bitmap->spec_data,ag_bitmap->pixel_sz,ag_bitmap->pixel_sz,FALSE,path);
		}
		
		if ((ok) && (ag_bitmap->glow_data!=NULL)) {
			sprintf(path,"%s/%s_g.png",base_path,ag_bitmap->frame->name);
			ok=bitmap_write_png_data(ag_bitmap->glow_data,ag_bitmap->pixel_sz,ag_bitmap->pixel_sz,FALSE,path);
		}
	}

		// free data

	free(ag_bitmap->png_data);
	free(ag_bitmap->bump_data);
	free(ag_bitmap->spec_data);
	free(ag_bitmap->glow_data);

	return(ok);
}

/* =======================================================

      Speculars
      
======================================================= */

void bitmap_ag_texture_make_spec(bitmap_ag_type *ag_bitmap,float spec_fct)
{
	int			px,py;
	float		f;
	d3col		col;

	for (py=0;py!=ag_bitmap->pixel_sz;py++) {
		for (px=0;px!=ag_bitmap->pixel_sz;px++) {

			bitmap_ag_texture_read_pixel(ag_bitmap,ag_bitmap->png_data,px,py,&col);

				// get max color

			f=col.r;
			if (col.g>f) f=col.g;
			if (col.b>f) f=col.b;

				// add in the contrast

			f=((f-0.5f)*spec_fct)+f;
			if (f<0.0f) f=0.0f;
			if (f>1.0f) f=1.0f;
			
			col.r=col.g=col.b=f;

			bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->spec_data,px,py,&col);
		}
	}
}

/* =======================================================

      Clips
      
======================================================= */

void bitmap_ag_set_clip(bitmap_ag_type *ag_bitmap,int x,int y,int wid,int high)
{
	ag_bitmap->clip_lx=x;
	ag_bitmap->clip_rx=x+wid;
	ag_bitmap->clip_ty=y;
	ag_bitmap->clip_by=y+high;
}

void bitmap_ag_clear_clip(bitmap_ag_type *ag_bitmap)
{
	ag_bitmap->clip_lx=0;
	ag_bitmap->clip_rx=ag_bitmap->pixel_sz;
	ag_bitmap->clip_ty=0;
	ag_bitmap->clip_by=ag_bitmap->pixel_sz;
}

/* =======================================================

      Read/Write Pixels
      
======================================================= */

void bitmap_ag_texture_read_pixel(bitmap_ag_type *ag_bitmap,unsigned char *data,int x,int y,d3col *col)
{
	unsigned char		*ptr;
	
	if (x<ag_bitmap->clip_lx) x+=ag_bitmap->clip_rx;
	if (x>=ag_bitmap->clip_rx) x-=(ag_bitmap->clip_rx-ag_bitmap->clip_lx);
	
	if (y<ag_bitmap->clip_ty) y+=ag_bitmap->clip_by;
	if (y>=ag_bitmap->clip_by) y-=(ag_bitmap->clip_by-ag_bitmap->clip_ty);

	ptr=data+((y*(ag_bitmap->pixel_sz*3))+(x*3));
	col->r=((float)*ptr++)/255.0f;
	col->g=((float)*ptr++)/255.0f;
	col->b=((float)*ptr)/255.0f;
}

void bitmap_ag_texture_write_pixel(bitmap_ag_type *ag_bitmap,unsigned char *data,int x,int y,d3col *col)
{
	unsigned char		*ptr;

	if (x<ag_bitmap->clip_lx) x+=ag_bitmap->clip_rx;
	if (x>=ag_bitmap->clip_rx) x-=(ag_bitmap->clip_rx-ag_bitmap->clip_lx);
	
	if (y<ag_bitmap->clip_ty) y+=ag_bitmap->clip_by;
	if (y>=ag_bitmap->clip_by) y-=(ag_bitmap->clip_by-ag_bitmap->clip_ty);

	ptr=data+((y*(ag_bitmap->pixel_sz*3))+(x*3));
	*ptr++=(int)(((float)0xFF)*col->r);
	*ptr++=(int)(((float)0xFF)*col->g);
	*ptr=(int)(((float)0xFF)*col->b);
}

void bitmap_ag_texture_read_normal(bitmap_ag_type *ag_bitmap,unsigned char *data,int x,int y,d3vct *normal)
{
	unsigned char		*ptr;

	if (x<ag_bitmap->clip_lx) x+=ag_bitmap->clip_rx;
	if (x>=ag_bitmap->clip_rx) x-=(ag_bitmap->clip_rx-ag_bitmap->clip_lx);
	
	if (y<ag_bitmap->clip_ty) y+=ag_bitmap->clip_by;
	if (y>=ag_bitmap->clip_by) y-=(ag_bitmap->clip_by-ag_bitmap->clip_ty);

	ptr=data+((y*(ag_bitmap->pixel_sz*3))+(x*3));
	normal->x=(((float)*ptr++)/128.0f)-1.0f;
	normal->y=(((float)*ptr++)/128.0f)-1.0f;
	normal->z=(((float)*ptr)/128.0f)-1.0f;
}

void bitmap_ag_texture_write_normal(bitmap_ag_type *ag_bitmap,unsigned char *data,int x,int y,d3vct *normal)
{
	unsigned char		*ptr;

	if (x<ag_bitmap->clip_lx) x+=ag_bitmap->clip_rx;
	if (x>=ag_bitmap->clip_rx) x-=(ag_bitmap->clip_rx-ag_bitmap->clip_lx);
	
	if (y<ag_bitmap->clip_ty) y+=ag_bitmap->clip_by;
	if (y>=ag_bitmap->clip_by) y-=(ag_bitmap->clip_by-ag_bitmap->clip_ty);

	vector_normalize(normal);		// normalize before writing

	ptr=data+((y*(ag_bitmap->pixel_sz*3))+(x*3));
	*ptr++=(int)((normal->x+1.0f)*128.0f);
	*ptr++=(int)((normal->y+1.0f)*128.0f);
	*ptr=(int)((normal->z+1.0f)*128.0f);
}

/* =======================================================

      Pixel Effects
      
======================================================= */

void bitmap_ag_texture_add_noise(bitmap_ag_type *ag_bitmap,int x,int y,int wid,int high,float base_darken,float percentage)
{
	int			px,py,per_ok;
	float		fct;
	d3col		col;

		// get precent where it's OK to
		// add noise

	per_ok=(int)(100.0f*percentage);

	for (py=y;py<(y+high);py++) {
		for (px=x;px<(x+wid);px++) {

			if (bitmap_ag_random_int(100)>per_ok) continue;

				// the bitmap noise

			bitmap_ag_texture_read_pixel(ag_bitmap,ag_bitmap->png_data,px,py,&col);

			fct=base_darken+(((float)bitmap_ag_random_int(100))*0.004f);

			col.r*=fct;
			if (col.r>1.0f) col.r=1.0f;
			col.g*=fct;
			if (col.g>1.0f) col.g=1.0f;
			col.b*=fct;
			if (col.b>1.0f) col.b=1.0f;

			bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,px,py,&col);
		}
	}
}

/* =======================================================

      Particle Effects
      
======================================================= */

void bitmap_ag_texture_add_particle(bitmap_ag_type *ag_bitmap,int x,int y,int wid,int high,float fct,bool flip_normal,int density)
{
	int			n,k,px,py,mx,my;
	float		rad,fx,fy,f_wid,f_high,
				start_f,factor_f;
	d3col		col;
	d3vct		normal;

	mx=x+(wid>>1);
	my=y+(high>>1);

	start_f=0.4;
	factor_f=0.1f;
	
	for (n=0;n!=5;n++) {
		
		for (k=0;k!=density;k++) {
			rad=(2.0f*TRIG_PI)*(((float)bitmap_ag_random_int(1000))/1000.0f);
			f_wid=start_f+(((float)bitmap_ag_random_int(wid))*factor_f);
			f_high=start_f+(((float)bitmap_ag_random_int(high))*factor_f);

			fx=sinf(rad);
			px=mx+(int)(f_wid*fx);

			fy=cosf(rad);
			py=my-(int)(f_high*fy);
		
				// add the mark pixels as random particles

			bitmap_ag_texture_read_pixel(ag_bitmap,ag_bitmap->png_data,px,py,&col);

			col.r*=fct;
			if (col.r>1.0f) col.r=1.0f;
			col.g*=fct;
			if (col.g>1.0f) col.g=1.0f;
			col.b*=fct;
			if (col.b>1.0f) col.b=1.0f;

			bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,px,py,&col);

				// get a normal for the pixel change

			bitmap_ag_texture_read_normal(ag_bitmap,ag_bitmap->bump_data,px,py,&normal);
			normal.x=flip_normal?-fx:fx;
			normal.y=flip_normal?-fy:fy;
			normal.z=0.5f;		// always force Z normal back to top
			bitmap_ag_texture_write_normal(ag_bitmap,ag_bitmap->bump_data,px,py,&normal);
		}
		
			// next ring

		start_f-=0.1f;
		factor_f+=0.1f;
	}
}

void bitmap_ag_texture_add_ridge_horizontal(bitmap_ag_type *ag_bitmap,int x,int y,int wid,int high,float min,float max)
{
	int			px,py,line_count,high_count;
	float		f;
	bool		ridge_up;
	d3col		col;
	d3vct		normal;

	line_count=0;
	high_count=0;

	ridge_up=bitmap_ag_random_bool();

	for (px=x;px<(x+wid);px++) {

			// move up or down

		if (line_count==0) {
			if (bitmap_ag_random_bool()) {
				y++;
			}
			else {
				y--;
			}
			line_count=1+bitmap_ag_random_int(15);
		}

			// change height

		if (high_count==0) {
			if (high==1) {
				high++;
			}
			else {
				if (high==5) {
					high--;
				}
				else {
					high+=(bitmap_ag_random_bool()?-1:1);
				}
			}
			high_count=1+bitmap_ag_random_int(15);
		}

			// draw the line

		for (py=y;py<(y+high);py++) {
			f=min+((max-min)*(((float)(y-py))/((float)high)));
			if (ridge_up) f=max-(f-min);
			
			bitmap_ag_texture_read_pixel(ag_bitmap,ag_bitmap->png_data,px,py,&col);

			col.r*=f;
			col.g*=f;
			col.b*=f;

			bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,px,py,&col);

				// get a normal for the pixel change

			normal.x=0.0f;
			normal.y=-((((float)(y-py))/((float)high))*0.5f);
			if (ridge_up) normal.y=-normal.y;
			normal.z=0.5f;		// always force Z normal back to top
			bitmap_ag_texture_write_normal(ag_bitmap,ag_bitmap->bump_data,px,py,&normal);
		}

		line_count--;
		high_count--;
	}
}

/* =======================================================

      Gradients
      
======================================================= */

void bitmap_ag_texture_gradient_overlay_vertical(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high,float min,float max)
{
	int			x,y;
	float		f;
	d3col		col;

		// draw the glow

	for (y=py;y<(py+high);y++) {
	
		f=min+((max-min)*(((float)(y-py))/((float)high)));
		
		for (x=px;x<(px+wid);x++) {
			bitmap_ag_texture_read_pixel(ag_bitmap,ag_bitmap->png_data,px,py,&col);
			col.r*=f;
			col.b*=f;
			col.g*=f;
			bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,x,y,&col);
		}
	}
}

/* =======================================================

      Random Color Stripes
      
======================================================= */

void bitmap_ag_texture_random_color_stripe_array(float fct,float *fct_array)
{
	int				n,count;
	float			f;

	count=0;

	for (n=0;n!=100;n++) {
	
		count--;
		
		if (count<=0) {
			f=(((float)(100-bitmap_ag_random_int(200)))*0.01f)*fct;
			count=1+bitmap_ag_random_int(3);
		}
		
		fct_array[n]=f;
	}
}

void bitmap_ag_texture_random_color_stripe_vertical(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high,d3col *base_col,float fct)
{
	int			x,y;
	float		f,fct_array[100];
	d3col		col;

	bitmap_ag_texture_random_color_stripe_array(fct,fct_array);

	for (y=py;y<(py+high);y++) {

		f=fct_array[y%100];
	
		col.r=base_col->r+f;
		if (col.r<0.0f) col.r=0.0f;
		if (col.r>1.0f) col.r=1.0f;
		
		col.g=base_col->g+f;
		if (col.g<0.0f) col.g=0.0f;
		if (col.g>1.0f) col.g=1.0f;
		
		col.b=base_col->b+f;
		if (col.b<0.0f) col.b=0.0f;
		if (col.b>1.0f) col.b=1.0f;
		
		for (x=px;x<(px+wid);x++) {
			bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,x,y,&col);
		}
	}
}

void bitmap_ag_texture_random_color_stripe_horizontal(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high,d3col *base_col,float fct)
{
	int			x,y;
	float		f,fct_array[100];
	d3col		col;

	bitmap_ag_texture_random_color_stripe_array(fct,fct_array);

	for (x=px;x<(px+wid);x++) {
	
		f=fct_array[x%100];
	
		col.r=base_col->r+f;
		if (col.r<0.0f) col.r=0.0f;
		if (col.r>1.0f) col.r=1.0f;
		
		col.g=base_col->g+f;
		if (col.g<0.0f) col.g=0.0f;
		if (col.g>1.0f) col.g=1.0f;
		
		col.b=base_col->b+f;
		if (col.b<0.0f) col.b=0.0f;
		if (col.b>1.0f) col.b=1.0f;
		
		for (y=py;y<(py+high);y++) {
			bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,x,y,&col);
		}
	}
}

void bitmap_ag_texture_random_color_stripe_slant(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high,d3col *base_col,float fct)
{
	int			x,y;
	float		f,fct_array[100];
	d3col		col;

	bitmap_ag_texture_random_color_stripe_array(fct,fct_array);

	for (y=py;y<(py+high);y++) {
		for (x=px;x<(px+wid);x++) {

			f=fct_array[(x+y)%100];
		
			col.r=base_col->r+f;
			if (col.r<0.0f) col.r=0.0f;
			if (col.r>1.0f) col.r=1.0f;
			
			col.g=base_col->g+f;
			if (col.g<0.0f) col.g=0.0f;
			if (col.g>1.0f) col.g=1.0f;
			
			col.b=base_col->b+f;
			if (col.b<0.0f) col.b=0.0f;
			if (col.b>1.0f) col.b=1.0f;
		
			bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,x,y,&col);
		}
	}
}

/* =======================================================

      Shapes
      
======================================================= */

void bitmap_ag_texture_draw_rectangle(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high,int lip_sz,bool flip_normal,d3col *col,d3col *edge_col)
{
	int			n,x,y,lx,rx,ty,by;
	float		f;
	d3col		dark_col,col2;
	d3vct		normal;

		// get darker color for outlines

	dark_col.r=col->r*bitmap_ag_outline_darken_factor;
	dark_col.g=col->g*bitmap_ag_outline_darken_factor;
	dark_col.b=col->b*bitmap_ag_outline_darken_factor;

	lx=px;
	rx=px+wid;
	ty=py;
	by=py+high;

	for (n=0;n!=lip_sz;n++) {

		if ((rx<=lx) || (by<=ty)) break;

			// get lip color

		f=(float)(n+1)/(float)lip_sz;
		col2.r=dark_col.r+((col->r-dark_col.r)*f);
		col2.g=dark_col.g+((col->g-dark_col.g)*f);
		col2.b=dark_col.b+((col->b-dark_col.b)*f);
		
		if (edge_col!=NULL) {
			if ((n==0) || (n==(lip_sz-2))) {
				col2.r=edge_col->r;
				col2.g=edge_col->g;
				col2.b=edge_col->b;
			}
		}

			// draw rectangle

		for (y=ty;y!=by;y++) {
			for (x=lx;x!=rx;x++) {
				bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,x,y,&col2);

				if (x==lx) {
					normal.x=flip_normal?0.89f:-0.89f;
					normal.y=0.0f;
					normal.z=0.44f;
					bitmap_ag_texture_write_normal(ag_bitmap,ag_bitmap->bump_data,x,y,&normal);
					continue;
				}

				if (x==(rx-1)) {
					normal.x=flip_normal?-0.89f:0.89f;
					normal.y=0.0f;
					normal.z=0.44f;
					bitmap_ag_texture_write_normal(ag_bitmap,ag_bitmap->bump_data,x,y,&normal);
					continue;
				}

				if (y==ty) {
					normal.x=0.0f;
					normal.y=flip_normal?0.89f:-0.89f;
					normal.z=0.44f;
					bitmap_ag_texture_write_normal(ag_bitmap,ag_bitmap->bump_data,x,y,&normal);
					continue;
				}

				if (y==(by-1)) {
					normal.x=0.0f;
					normal.y=flip_normal?-0.89f:0.89f;
					normal.z=0.44f;
					bitmap_ag_texture_write_normal(ag_bitmap,ag_bitmap->bump_data,x,y,&normal);
					continue;
				}

				normal.x=-1.0f;
				normal.y=-1.0f;
				normal.z=1.0f;
				bitmap_ag_texture_write_normal(ag_bitmap,ag_bitmap->bump_data,x,y,&normal);
			}
		}

		lx++;
		rx--;
		ty++;
		by--;
	}
}

void bitmap_ag_texture_draw_oval(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high,int border_sz,bool flip_normal,d3col *border_col,d3col *col)
{
	int				n,x,y,mx,my,border_count;
	float			f_wid,f_high,fx,fy,rad;
	d3vct			normal;

	mx=px+(wid>>1);
	my=py+(high>>1);

	border_count=border_sz;

	while ((wid>0) && (high>0)) {

		f_wid=((float)wid)*0.5f;
		f_high=((float)high)*0.5f;

		for (n=0;n!=1000;n++) {
			rad=((2.0f*TRIG_PI)*((float)n))/1000.0f;

			fx=sinf(rad);
			x=mx+(int)(f_wid*fx);

			fy=cosf(rad);
			y=my-(int)(f_high*fy);

				// the color pixel

			if (border_count>0) {
				bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,x,y,border_col);
			}
			else {
				bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,x,y,col);
			}

				// get a normal for the pixel change

			normal.x=flip_normal?-fx:fx;
			normal.y=flip_normal?fy:-fy;
			normal.z=0.5f;		// always force Z normal back to top
			bitmap_ag_texture_write_normal(ag_bitmap,ag_bitmap->bump_data,x,y,&normal);
		}

		if (border_count>0) border_count--;
		if ((border_count==0) && (col==NULL)) break;

		wid--;
		high--;
	}
}

void bitmap_ag_texture_draw_line_horizontal(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int lip_sz,bool flip_normal,d3col *col)
{
	int			n,x;
	float		f;
	d3col		dark_col,col2;
	d3vct		normal;

		// get darker color for outlines

	dark_col.r=col->r*bitmap_ag_outline_darken_factor;
	dark_col.g=col->g*bitmap_ag_outline_darken_factor;
	dark_col.b=col->b*bitmap_ag_outline_darken_factor;

		// top lip
	
	for (n=0;n!=lip_sz;n++) {

			// get lip color

		f=(float)(n+1)/(float)lip_sz;
		col2.r=dark_col.r+((col->r-dark_col.r)*f);
		col2.g=dark_col.g+((col->g-dark_col.g)*f);
		col2.b=dark_col.b+((col->b-dark_col.b)*f);

			// draw top lip line

		for (x=px;x<(px+wid);x++) {
			bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,x,py,&col2);

			normal.x=0;
			normal.y=flip_normal?0.89f:-0.89f;
			normal.z=0.44f;
			bitmap_ag_texture_write_normal(ag_bitmap,ag_bitmap->bump_data,x,py,&normal);
		}

		py++;
	}

		// line

	for (x=px;x<(px+wid);x++) {
		bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,x,py,&col2);
		normal.x=0;
		normal.y=0;
		normal.z=1.0f;
		bitmap_ag_texture_write_normal(ag_bitmap,ag_bitmap->bump_data,x,py,&normal);
	}

		// bottom lip
	
	for (n=0;n!=lip_sz;n++) {

			// get lip color

		f=1.0f-((float)(n+1)/(float)lip_sz);
		col2.r=dark_col.r+((col->r-dark_col.r)*f);
		col2.g=dark_col.g+((col->g-dark_col.g)*f);
		col2.b=dark_col.b+((col->b-dark_col.b)*f);

			// draw top lip line

		for (x=px;x<(px+wid);x++) {
			bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,x,py,&col2);

			normal.x=0;
			normal.y=flip_normal?0.89f:-0.89f;
			normal.z=0.44f;
			bitmap_ag_texture_write_normal(ag_bitmap,ag_bitmap->bump_data,x,py,&normal);
		}

		py++;
	}

}
