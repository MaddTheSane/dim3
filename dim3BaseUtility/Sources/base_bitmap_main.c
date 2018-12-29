/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Main Bitmap Functions
 
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

/* =======================================================

      New Bitmaps
      
======================================================= */

void bitmap_new(bitmap_type *bitmap)
{
	bitmap->wid=bitmap->high=0;
	bitmap->opaque=TRUE;
	bitmap->gl_id=-1;
}

/* =======================================================

      Bitmaps Utilities
      
======================================================= */

void bitmap_scrub_black_to_alpha(bitmap_type *bitmap,unsigned char *png_data)
{
	int					n,psz;
	unsigned char		*data;

	data=png_data;
	psz=(bitmap->wid<<2)*bitmap->high;

	for (n=0;n<psz;n+=4) {
		*(data+3)=*data;
		data+=4;
	}
}

void bitmap_set_alpha_mode(bitmap_type *bitmap,unsigned char *png_data)
{
	int					n,psz;
	unsigned char		*data;

		// assume it's opaque until we see different data

	bitmap->opaque=TRUE;
					
		// search for transparencies

	data=png_data+3;
	psz=(bitmap->wid<<2)*bitmap->high;
		
	for (n=0;n<psz;n+=4) {
			
		if (*data!=0xFF) {
			bitmap->opaque=FALSE;
			return;
		}

		data+=4;
	}
}

unsigned char* bitmap_strip_alpha(bitmap_type *bitmap,unsigned char *png_data)
{
	int					n,psz;
	unsigned char		*strip_data,*srce,*dest;

		// new data
		
	psz=(bitmap->wid*3)*bitmap->high;
	strip_data=(unsigned char*)malloc(psz);
	
		// strip out alphas
		
	srce=png_data;
	dest=strip_data;
	
	psz=(bitmap->wid<<2)*bitmap->high;
		
	for (n=0;n<psz;n+=4) {
		*dest++=*srce++;
		*dest++=*srce++;
		*dest++=*srce++;
		srce++;
	}
	
	return(strip_data);
}

int bitmap_find_nearest_power_2(int sz)
{
	int			n,p2;

		// all textures need to be
		// at least 8 pixels

	if (sz<8) return(8);

		// get closest power of 2,
		// from 8 to 2048

	p2=8;
	for (n=0;n!=9;n++) {
		if (p2>sz) {
			if ((sz-(p2/2))<(p2-sz)) return(p2/2);
			return(p2);
		}
		p2*=2;
	}

		// most be really big,
		// just use 1024

	return(1024);
}

unsigned char* bitmap_fix_power_2(bitmap_type *bitmap,bool has_alpha,unsigned char *png_data)
{
	int				wid,high,byte_sz,x,y,kx,ky,dsz,
					line_x,line_y,old_line_x,old_line_y,
					i_r,i_g,i_b,i_alpha;
	float			x_skip,y_skip;
	unsigned char	ub_r,ub_g,ub_b,ub_alpha;
	unsigned char	*data,*sptr,*dptr;

		// get closest power of 2 sizes

	wid=bitmap_find_nearest_power_2(bitmap->wid);
	high=bitmap_find_nearest_power_2(bitmap->high);

		// any changes?

	if ((wid==bitmap->wid) && (high==bitmap->high)) return(png_data);

		// is it 3 or 4 bytes?

	if (has_alpha) {
		byte_sz=4;
	}
	else {
		byte_sz=3;
	}

		// convert data

		// sometimes we can be making bigger widths
		// or heights so we check to see if the lines
		// have stayed the same and then create
		// the new pixel from the averages

	dsz=(wid*byte_sz)*high;

	data=(unsigned char*)malloc(dsz);
	if (data==NULL) return(png_data);

	x_skip=(float)bitmap->wid/(float)wid;
	y_skip=(float)bitmap->high/(float)high;

	old_line_y=-1;

	dptr=data;

	for (y=0;y!=high;y++) {

		line_y=(int)(y_skip*((float)y));

		old_line_x=-1;
	
		for (x=0;x!=wid;x++) {
			line_x=(int)(x_skip*((float)x));

				// get the regular pixel
				// this is the one we use if no blending

			sptr=png_data+(byte_sz*line_x)+((bitmap->wid*byte_sz)*line_y);

			ub_r=*sptr++;
			ub_g=*sptr++;
			ub_b=*sptr++;

			if (byte_sz==4) ub_alpha=*sptr;

				// check to see if this is a repeated
				// line, if so, we need to create it
				// by averaging

			if ((line_x==old_line_x) || (line_y==old_line_y)) {
				if ((line_x>0) && (line_x<(wid-1)) && (line_y>0) && (line_y<(high-1))) {

					i_r=i_g=i_b=i_alpha=0;

					for (ky=(line_y-1);ky<=(line_y+1);ky++) {
						for (kx=(line_x-1);kx<=(line_x+1);kx++) {
							if ((ky==line_y) && (kx==line_x)) continue;

							sptr=png_data+(byte_sz*line_x)+((bitmap->wid*byte_sz)*line_y);
							i_r+=(int)*sptr++;
							i_g+=(int)*sptr++;
							i_b+=(int)*sptr++;

							if (byte_sz==4) i_alpha+=(int)*sptr;
						}
					}

					ub_r=(unsigned char)(i_r>>3);
					ub_g=(unsigned char)(i_g>>3);
					ub_b=(unsigned char)(i_b>>3);
					if (byte_sz==4) ub_alpha=(unsigned char)(i_alpha>>3);
				}
			}

			*dptr++=ub_r;
			*dptr++=ub_g;
			*dptr++=ub_b;
			if (byte_sz==4) *dptr++=ub_alpha;

			old_line_x=line_x;
		}

		old_line_y=line_y;
	}

	free(png_data);

		// reset width and height

	bitmap->wid=wid;
	bitmap->high=high;

	return(data);
}

unsigned char* bitmap_setup_alpha(bitmap_type *bitmap,unsigned char *png_data,bool alpha_channel,bool scrub_black_to_alpha)
{
	unsigned char				*strip_data;
	
	bitmap->opaque=TRUE;

	if (alpha_channel) {

			// scrub blacks to alphas
			// this is mostly used for glow maps

		if (scrub_black_to_alpha) bitmap_scrub_black_to_alpha(bitmap,png_data);
		
			// find if bitmap has transparencies

		bitmap_set_alpha_mode(bitmap,png_data);
		
			// if alpha is actually all 0xFF, then strip it
			
		if (bitmap->opaque) {
			strip_data=bitmap_strip_alpha(bitmap,png_data);
			if (strip_data!=NULL) {
				free(png_data);
				png_data=strip_data;
			}
		}
	}
	
	return(png_data);
}

void bitmap_flip_normals(bitmap_type *bitmap,unsigned char *png_data)
{
	int					n,psz,byte_sz;
	unsigned char		*data;

		// is it 3 or 4 bytes?

	if (!bitmap->opaque) {
		byte_sz=4;
	}
	else {
		byte_sz=3;
	}

		// run through and flip
		// Y on normals so shaders
		// don't have to do it

	data=png_data;
	
	psz=bitmap->wid*bitmap->high;
		
	for (n=0;n!=psz;n++) {
		*(data+1)=(0xFF-(*(data+1)));
		data+=byte_sz;
	}
}

/* =======================================================

      Open Bitmaps
      
======================================================= */

bool bitmap_open(bitmap_type *bitmap,char *path,bool mipmap,bool compress,bool pixelated,bool npot,bool scrub_black_to_alpha,bool flip_normal)
{
	unsigned char		*png_data;
	bool				ok,alpha_channel;
	
	bitmap_new(bitmap);
	
		// read bitmap
	
	png_data=png_utility_read(path,&bitmap->wid,&bitmap->high,&alpha_channel);
	if (png_data==NULL) return(FALSE);

		// if not a non-power-of-two, fix size
		// if data is not a power of two

	if (!npot) png_data=bitmap_fix_power_2(bitmap,alpha_channel,png_data);

		// set alphas and scrubbing
		// and any normal flipping
		
	png_data=bitmap_setup_alpha(bitmap,png_data,alpha_channel,scrub_black_to_alpha);
	if (flip_normal) bitmap_flip_normals(bitmap,png_data);
		
		// get the texture
		
	ok=bitmap_texture_open(bitmap,png_data,mipmap,compress,pixelated,npot);
		
	free(png_data);
	
	return(ok);
}

/* =======================================================

      Color Bitmap
      
======================================================= */

bool bitmap_color(bitmap_type *bitmap,d3col *col)
{
	int				n,kr,kg,kb;
	unsigned char	*png_data,*dptr;
	bool			ok;
	
	bitmap->wid=bitmap->high=32;
	bitmap->opaque=TRUE;
	
	png_data=malloc((32*32)*3);
	if (png_data==NULL) return(FALSE);
	
	kr=(int)(0xFF*col->r);
	kg=(int)(0xFF*col->g);
	kb=(int)(0xFF*col->b);
	
	dptr=png_data;

	for (n=0;n!=(32*32);n++) {
		*dptr++=kr;
		*dptr++=kg;
		*dptr++=kb;
	}
	
	ok=bitmap_texture_open(bitmap,png_data,FALSE,FALSE,TRUE,FALSE);

	free(png_data);
	
	return(ok);
}

/* =======================================================

      Data Bitmap
      
======================================================= */

bool bitmap_data(bitmap_type *bitmap,unsigned char *data,int wid,int high,bool alpha_channel,bool mipmap,bool compress,bool pixelated,bool npot)
{
	bitmap->wid=wid;
	bitmap->high=high;

		// if not a non-power-of-two, fix size
		// if data is not a power of two

	if (!npot) data=bitmap_fix_power_2(bitmap,alpha_channel,data);
	
		// find if bitmap has transparencies
	
	bitmap->opaque=TRUE;
	if (alpha_channel) bitmap_set_alpha_mode(bitmap,data);
	
		// get the texture
		
	return(bitmap_texture_open(bitmap,data,mipmap,compress,pixelated,npot));
}

/* =======================================================

      Close Bitmaps
      
======================================================= */

void bitmap_close(bitmap_type *bitmap)
{
	if (bitmap->gl_id!=-1) bitmap_texture_close(bitmap);
	bitmap->gl_id=-1;
}

/* =======================================================

      Misc Bitmap Functions
      
======================================================= */

unsigned char* bitmap_load_png_data(char *path,int *p_wid,int *p_high,bool *alpha_channel)
{
	return(png_utility_read(path,p_wid,p_high,alpha_channel));
}

bool bitmap_write_png_data(unsigned char *data,int wid,int high,bool alpha_channel,char *path)
{
	return(png_utility_write(data,wid,high,alpha_channel,path));
}

bool bitmap_check(char *path,char *err_str)
{
	return(png_utility_check(path,err_str));
}

bool bitmap_copy(char *srce_path,char *dest_path)
{
	return(png_utility_copy(srce_path,dest_path));
}



