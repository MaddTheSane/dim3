/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Read/Write/Check PNG Files

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
	#include "dim3baseutility.h"
#endif

/* =======================================================

      IO Replacements
      
======================================================= */

void png_utility_fread(png_structp png_ptr,png_bytep data,png_size_t length)
{
	FILE			*file;

	file=(FILE*)png_get_io_ptr(png_ptr);
	fread(data,1,length,file);
}

void png_utility_fwrite(png_structp png_ptr,png_bytep data,png_size_t length)
{
	FILE			*file;

	file=(FILE*)png_get_io_ptr(png_ptr);
	fwrite(data,1,length,file);
}

void png_utility_fflush(png_structp png_ptr)
{
	FILE			*file;

	file=(FILE*)png_get_io_ptr(png_ptr);
	fflush(file);
}

/* =======================================================

      Read PNGs
      
======================================================= */

unsigned char* png_utility_read(char *path,int *p_wid,int *p_high,bool *alpha_channel)
{
	int						x,y,bit_depth,
							psz,rowbytes,channels,wid,high;
	unsigned char			header[8];
	unsigned char			*data,*ptr;
	FILE					*file;
	png_structp				png_ptr;
	png_infop				info_ptr;
	png_bytep				*rptrs;

		// open file

	file=fopen(path,"rb");
	if (file==NULL) return(NULL);
	
	fread(header,1,8,file);
	if (png_sig_cmp(header,0,8)) return(NULL);
	
		// get info
		
	png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if (png_ptr==NULL) {
		fclose(file);
		return(NULL);
	}
	
	info_ptr=png_create_info_struct(png_ptr);
	if (info_ptr==NULL) {
		png_destroy_read_struct(&png_ptr,NULL,NULL);
		fclose(file);
		return(NULL);
	}
	
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		fclose(file);
		return(NULL);
	}
	
		// we have to force libPNG to call back
		// for fread and fwrite because changes
		// in the libraries in win32 cause all sorts of havoc

	png_set_read_fn(png_ptr,file,png_utility_fread);
	png_set_sig_bytes(png_ptr,8);
	
	png_read_info(png_ptr,info_ptr);
	
	wid=png_get_image_width(png_ptr,info_ptr);
	high=png_get_image_height(png_ptr,info_ptr);
	rowbytes=png_get_rowbytes(png_ptr,info_ptr);
	channels=png_get_channels(png_ptr,info_ptr);
	bit_depth=png_get_bit_depth(png_ptr,info_ptr)*channels;

		// create the bitmap
		
	if (bit_depth==32) {
		psz=(wid<<2)*high;
		*alpha_channel=TRUE;
	}
	else {
		psz=(wid*3)*high;
		*alpha_channel=FALSE;
	}
	
	data=malloc(psz);
	if (data==NULL) {
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		fclose(file);
		return(NULL);
	}
	
	if (p_wid!=NULL) {
		*p_wid=wid;
		*p_high=high;
	}

		// special check for non-24/32 bit
		// textures

	if ((bit_depth!=24) && (bit_depth!=32)) {

			// just make a white texture

		ptr=data;

		for (y=0;y!=high;y++) {
			for (x=0;x!=wid;x++) {
				*ptr++=0xFF;
				*ptr++=0xFF;
				*ptr++=0xFF;
			}
		}

		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		fclose(file);

		return(data);
	}

		// read the file

	png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr,info_ptr);
		
	rptrs=(png_bytep*)malloc(sizeof(png_bytep)*high);
	if (rptrs==NULL) {
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		fclose(file);
		return(NULL);
	}
	
	for (y=0;y!=high;y++) {
		rptrs[y]=(png_byte*)malloc(rowbytes);
		if (rptrs[y]==NULL) {
			png_destroy_read_struct(&png_ptr,&info_ptr,NULL);		// supergumba -- note -- this error state will leak -- need to fix in the future
			free(rptrs);
			fclose(file);
			return(NULL);
		}
	}
	
	png_read_image(png_ptr,rptrs);
	
		// translate
		
	ptr=data;
	
	for (y=0;y!=high;y++) {
		memmove(ptr,rptrs[y],rowbytes);
		ptr+=rowbytes;
	}

		// clean up
		
	for (y=0;y!=high;y++) {
		free(rptrs[y]);
	}
	
	free(rptrs);
		
	png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
	fclose(file);
	
	return(data);
}

/* =======================================================

      Write PNGs
      
======================================================= */

bool png_utility_write(unsigned char *data,int wid,int high,bool alpha_channel,char *path)
{
	int						y,rowbytes;
	unsigned char			*ptr;
	FILE					*file;
	png_structp				png_ptr;
	png_infop				info_ptr;
	png_bytep				*rptrs;
	
		// open file
		
	file=fopen(path,"wb");
	if (file==NULL) return(FALSE);
	
		// setup write
		
	png_ptr=png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if (png_ptr==NULL) {
		fclose(file);
		return(FALSE);
	}
	
	info_ptr=png_create_info_struct(png_ptr);
	if (info_ptr==NULL) {
		png_destroy_read_struct(&png_ptr,NULL,NULL);
		fclose(file);
		return(FALSE);
	}
	
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		fclose(file);
		return(FALSE);
	}

		// we have to force libPNG to call back
		// for fread and fwrite because changes
		// in the libraries in win32 cause all sorts of havoc

	png_set_write_fn(png_ptr,file,png_utility_fwrite,png_utility_fflush);
	
		// write the header
		
	png_set_IHDR(png_ptr,info_ptr,wid,high,8,(alpha_channel?PNG_COLOR_TYPE_RGB_ALPHA:PNG_COLOR_TYPE_RGB),PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_BASE,PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr,info_ptr);
	
		// setup row pointers
		
	rptrs=(png_bytep*)malloc(sizeof(png_bytep)*high);
	
	if (alpha_channel) {
		rowbytes=(wid<<2);
	}
	else {
		rowbytes=(wid*3);
	}
	
	ptr=data;
	
	for (y=0;y!=high;y++) {
		rptrs[y]=(png_byte*)malloc(rowbytes);
		memmove(rptrs[y],ptr,rowbytes);
		ptr+=rowbytes;
	}
		
		// write the image
		
	png_write_image(png_ptr,rptrs);
	
		// write the end
		
	png_write_end(png_ptr,NULL);
	
		// clean up
		
	for (y=0;y!=high;y++) {
		free(rptrs[y]);
	}
	
	free(rptrs);
		
	png_destroy_write_struct(&png_ptr,&info_ptr);
	fclose(file);
	
	return(TRUE);
}

/* =======================================================

      Check PNGs
      
======================================================= */

bool png_utility_check(char *path,char *err_str)
{
	int						x,y,bit_depth,color_type,channels,
							*v,values[]={2,4,8,16,32,64,128,256,512,1024,2048,4096,-1};
	bool					x_ok,y_ok;
	unsigned char			header[8];
	FILE					*file;
	png_structp				png_ptr;
	png_infop				info_ptr;
	
		// open file
		
	file=fopen(path,"rb");
	if (file==NULL) {
		strcpy(err_str,"Could not open PNG.");
		return(FALSE);
	}
	
	fread(header,1,8,file);
	if (png_sig_cmp(header,0,8)) {
		strcpy(err_str,"Not a proper PNG file.");
		return(FALSE);
	}
	
		// setup read
		
	png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if (png_ptr==NULL) {
		fclose(file);
		strcpy(err_str,"Could not read PNG.");
		return(FALSE);
	}
	
	info_ptr=png_create_info_struct(png_ptr);
	if (info_ptr==NULL) {
		png_destroy_read_struct(&png_ptr,NULL,NULL);
		fclose(file);
		strcpy(err_str,"Could not read PNG.");
		return(FALSE);
	}
	
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		fclose(file);
		strcpy(err_str,"Could not read PNG.");
		return(FALSE);
	}
	
		// we have to force libPNG to call back
		// for fread and fwrite because changes
		// in the libraries in win32 cause all sorts of havoc

	png_set_read_fn(png_ptr,file,png_utility_fread);
	png_set_sig_bytes(png_ptr,8);
	
	png_read_info(png_ptr,info_ptr);

	x=png_get_image_width(png_ptr,info_ptr);
	y=png_get_image_height(png_ptr,info_ptr);
	channels=png_get_channels(png_ptr,info_ptr);
	bit_depth=png_get_bit_depth(png_ptr,info_ptr);
	color_type=png_get_color_type(png_ptr,info_ptr);
	
	png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr,info_ptr);
	
		// clean up		

	png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
	fclose(file);
	
		// correct bit depth?
		
	if ((bit_depth!=8) || ((color_type!=PNG_COLOR_TYPE_RGB) && (color_type!=PNG_COLOR_TYPE_RGB_ALPHA))) {
		strcpy(err_str,"PNG color components need to have a bit depth of 8.");
		return(FALSE);
	}
	
		// correct channels
		
	if ((channels!=3) && (channels!=4)) {
		strcpy(err_str,"PNG must be either 3-component (RGB) or 4-component (RGBA).");
		return(FALSE);
	}
	
		// is it not a power of 2?
		
	x_ok=y_ok=FALSE;
	
	v=values;
	while (TRUE) {
		if (*v==-1) break;
		x_ok=x_ok||(x==*v);
		y_ok=y_ok||(y==*v);
		v++;
	}
	
	if ((!x_ok) || (!y_ok)) {
		strcpy(err_str,"Both the width and height of the texture must be a power of 2 (2, 4, 8, 16, 32, 64, 128, 256, etc).");
		return(FALSE);
	}

		// bitmap is OK
		
	return(TRUE);
}

/* =======================================================

      Copy PNGs
      
======================================================= */

bool png_utility_copy_compare_paths(char *srce_path,char *dest_path)
{
	int				n,len;
	char			ch1,ch2;

	len=strlen(srce_path);
	if (len!=strlen(dest_path)) return(FALSE);

		// we compare this way so we can
		// skip and / or \ reverses
		// and caps

	for (n=0;n!=len;n++) {
		ch1=srce_path[n];
		ch2=dest_path[n];

		if ((ch1=='\\') || (ch1=='/')) {
			if ((ch2!='\\') && (ch2!='/')) return(FALSE);
			continue;
		}

		if ((ch1>='A') && (ch1<='Z')) ch1+=('a'-'A');
		if ((ch2>='A') && (ch2<='Z')) ch2+=('a'-'A');

		if (ch1!=ch2) return(FALSE);
	}

	return(TRUE);
}

bool png_utility_copy(char *srce_path,char *dest_path)
{
	int				sz;
	unsigned char	c[1024];
	FILE			*srce,*dest;

		// compare the paths and
		// ignore if same file

	if (png_utility_copy_compare_paths(srce_path,dest_path)) return(TRUE);

		// open files
		
	srce=fopen(srce_path,"rb");
	if (srce==NULL) return(FALSE);
	
	dest=fopen(dest_path,"wb");
	if (dest==NULL) {
		fclose(srce);
		return(FALSE);
	}
	
		// copy
		
	while (TRUE) {
		sz=(int)fread(c,1,1024,srce);
		if (sz<=0) break;
		fwrite(c,1,sz,dest);
	}

		// close
		
	fclose(srce);
	fclose(dest);
	
	return(TRUE);
}

