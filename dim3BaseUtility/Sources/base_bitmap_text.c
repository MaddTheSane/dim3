/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Text to Textures

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

#if defined(D3_OS_MAC) || defined(D3_OS_IPHONE)
	extern bool cocoa_bitmap_text_font_exist(char *name);
#endif

#ifdef D3_OS_LINUX
	#include <ft2build.h>
	#include FT_FREETYPE_H
	#include <fontconfig/fontconfig.h>
#endif

#ifdef D3_OS_WINDOWS
	char			bitmap_enum_name[256];
	bool			bitmap_enum_ok;
#endif

/* =======================================================

      Create Font Texture OS X
      
======================================================= */

#if defined(D3_OS_MAC) || defined(D3_OS_IPHONE)

bool bitmap_text_font_exist(char *name)
{
	return(cocoa_bitmap_text_font_exist(name));
}

unsigned char* bitmap_text_size_internal(texture_font_size_type *d3_size_font,char *name,int size,int bitmap_wid,int bitmap_high)
{
	int					n,x,y,data_sz,row_add;
	char				ch;
	unsigned char		*bm_data,*txt_data,*sptr,*dptr;
	CGPoint				txt_pt;
	CGContextRef		bitmap_ctx;
	CGColorSpaceRef		color_space;
	CGAffineTransform	trans_flip;
	
		// data for bitmap

	row_add=bitmap_wid<<2;
	data_sz=row_add*bitmap_high;
	
	bm_data=malloc(data_sz);
	if (bm_data==NULL) return(NULL);
	
	bzero(bm_data,data_sz);
	
		// create bitmap context
		
	color_space=CGColorSpaceCreateDeviceRGB();
	bitmap_ctx=CGBitmapContextCreate(bm_data,bitmap_wid,bitmap_high,8,row_add,color_space,kCGImageAlphaPremultipliedLast);
	CGColorSpaceRelease(color_space);
	
	if (bitmap_ctx==NULL) {
		free(bm_data);
		return(NULL);
	}
	
	CGContextTranslateCTM(bitmap_ctx,0,bitmap_high);
    CGContextScaleCTM(bitmap_ctx,1.0f,-1.0f);
	
	CGContextClearRect(bitmap_ctx,CGRectMake(0,0,bitmap_wid,bitmap_high));
 
	CGContextSetAlpha(bitmap_ctx,1.0f);
	CGContextSetBlendMode(bitmap_ctx,kCGBlendModeNormal);

		// setup the correct font

	CGContextSelectFont(bitmap_ctx,name,size,kCGEncodingMacRoman);
	
		// setup text drawing
		
	CGContextSetTextDrawingMode(bitmap_ctx,kCGTextFill);
	CGContextSetRGBFillColor(bitmap_ctx,1.0f,1.0f,1.0f,1.0f);
	CGContextSetRGBStrokeColor(bitmap_ctx,1.0f,1.0f,1.0f,1.0f);

	trans_flip=CGAffineTransformMake(1.0f,0.0f,0.0f,-1.0f,0.0f,0.0f);
	CGContextSetTextMatrix(bitmap_ctx,trans_flip);
		
		// draw the characters

	for (n=0;n!=90;n++) {
	
			// draw the character

		ch=(n+'!');

		x=(n%d3_size_font->char_per_line)*d3_size_font->char_box_wid;
		y=((n/d3_size_font->char_per_line)*d3_size_font->char_box_high)+d3_size_font->char_baseline;

		CGContextShowTextAtPoint(bitmap_ctx,x,y,&ch,1);

			// get the spacing information
			
		txt_pt=CGContextGetTextPosition(bitmap_ctx);

		d3_size_font->char_size[n]=(txt_pt.x-(float)x)/(float)d3_size_font->char_box_wid;
	}

		// texture data
		
	txt_data=malloc(data_sz);
	if (txt_data==NULL) {
		CGContextRelease(bitmap_ctx);
		free(bm_data);
		return(NULL);
	}
	
		// create the texture
		// note the coordinate systems are flipped

	dptr=txt_data;
	sptr=bm_data;

	for (y=0;y!=bitmap_high;y++) {

		for (x=0;x!=bitmap_wid;x++) {

			*dptr++=0xFF;
			*dptr++=0xFF;
			*dptr++=0xFF;
			*dptr++=*sptr;		// use the anti-aliased font as the alpha mask (using the red component)
			
			sptr+=4;

		}
	}
	
		// dispose the bitmap context
		
	CGContextRelease(bitmap_ctx);
	free(bm_data);

	return(txt_data);
}

#endif

/* =======================================================

      Create Font Texture Linux
      
======================================================= */

#ifdef D3_OS_LINUX

// code from cyst, additional edit by brian
// not sure about my additions until some linux guy takes a run at it

bool bitmap_text_font_exist(char *name)
{
	int				error;
	char 			*font_filename;
	FcPattern 		*pat, *match;
	FcResult 		fc_result;
	FT_Library 		font_library;
	FT_Face 		face;

	pat = FcPatternCreate();
	FcPatternAddString(pat, FC_FAMILY, name);
	FcConfigSubstitute(NULL, pat, FcMatchPattern);
	FcDefaultSubstitute(pat);
	match = FcFontMatch(NULL, pat, &fc_result);
	FcPatternGetString(match, FC_FILE, 0, (FcChar8 **)&font_filename);

	FT_Init_FreeType(&font_library);
	return(FT_New_Face(font_library,font_filename,0,&face)==0);
}

unsigned char* bitmap_text_size_internal(texture_font_size_type *d3_size_font,char *name,int size,int bitmap_wid,int bitmap_high)
{
	int				n, x, y, font_x, font_y, txt_x, txt_y, error;
	unsigned char 	*data, *ptr;
	FcPattern 		*pat, *match;
	FcResult 		fc_result;
	char 			*font_filename;
	FT_Library 		font_library;
	FT_Face 		face;
	FT_Bitmap		bitmap;
	unsigned char	ch;
	
	data = malloc((bitmap_wid<<2)*bitmap_high);
	if (data==NULL) return(NULL);

	// find the font
	pat = FcPatternCreate();
	FcPatternAddString(pat, FC_FAMILY, name);
	FcConfigSubstitute(NULL, pat, FcMatchPattern);
	FcDefaultSubstitute(pat);
	match = FcFontMatch(NULL, pat, &fc_result);
	FcPatternGetString(match, FC_FILE, 0, (FcChar8 **)&font_filename);

	FT_Init_FreeType(&font_library);
	error = FT_New_Face(font_library,font_filename,0,&face);
	if (error) return(NULL);

	error = FT_Set_Char_Size(face,0,(size<<6),0,0);
	
	FcPatternDestroy(match);
	FcPatternDestroy(pat);
	
	for (n=0;n!=90;n++) {
		ch=(unsigned char)(n+'!');
		x=(n%d3_size_font->char_per_line)*d3_size_font->char_box_wid;
		y=((n/d3_size_font->char_per_line)*d3_size_font->char_box_high)+d3_size_font->char_baseline;

		// get the bitmap
		FT_Select_Charmap(face, ft_encoding_unicode);
		FT_UInt glyph_index = FT_Get_Char_Index(face,ch);

		error = FT_Load_Glyph(face,glyph_index,FT_LOAD_NO_HINTING);
		if (error) continue;
		
		error = FT_Render_Glyph(face->glyph,FT_RENDER_MODE_LIGHT);
		if (error) continue;

		bitmap = face->glyph->bitmap;
		
		// draw the bitmap
		for(font_y=0;font_y<bitmap.rows;font_y++){
			for(font_x=0;font_x<bitmap.width;font_x++){
				txt_x = x + font_x + face->glyph->bitmap_left;
				txt_y = y + font_y - face->glyph->bitmap_top-1;
				
				//bail if we are outside of the texture bounds
				if (
					(txt_x<0) || (txt_x>bitmap_wid)
					|| (txt_y<0) || (txt_y>bitmap_high)
					) continue;
				
				//set the pointer to the (x,y) position in the texture
				ptr = data + ((txt_y * bitmap_wid + txt_x) << 2);
				*ptr++=0xFF;
				*ptr++=0xFF;
				*ptr++=0xFF;
				*ptr++=(bitmap.buffer[font_y * bitmap.pitch + font_x]);
				d3_size_font->char_size[n]=(float)(1+(face->glyph->advance.x>>6))/(float)d3_size_font->char_box_wid;
			}
		}
	}

	return(data);
}

#endif

/* =======================================================

      Create Font Texture Windows
      
======================================================= */

#ifdef D3_OS_WINDOWS

int CALLBACK bitmap_text_font_callback(const LOGFONT *lfont,const TEXTMETRIC *ltme,DWORD font_type,LPARAM lparam)
{
	if (strcasecmp(lfont->lfFaceName,bitmap_enum_name)==0) {
		bitmap_enum_ok=TRUE;
		return(0);
	}

	return(1);
}

bool bitmap_text_font_exist(char *name)
{
	char			str[32];
	LOGFONT			lfont;

	strcpy(bitmap_enum_name,name);
	bitmap_enum_ok=FALSE;

	str[0]=0x0;

	lfont.lfCharSet=DEFAULT_CHARSET;
	lfont.lfFaceName[0]=0x0;
	lfont.lfPitchAndFamily=0;

	EnumFontFamiliesEx(GetDC(NULL),&lfont,bitmap_text_font_callback,0,0);

	return(bitmap_enum_ok);
}

unsigned char* bitmap_text_size_internal(texture_font_size_type *d3_size_font,char *name,int size,int bitmap_wid,int bitmap_high)
{
	int				n,x,y;
	unsigned char	ch;
	unsigned char	*data,*data_ptr,*bitmap_data,*bitmap_ptr;
	HDC				screen_dc,dc;
	BITMAPINFO		bmi;
	HBITMAP			bmp,old_bmp;
	HFONT			font,old_font;
	HBRUSH			brsh,old_brsh;
	RECT			box;
	ABC				ch_abc;

		// data for bitmap

	data=malloc((bitmap_wid<<2)*bitmap_high);
	if (data==NULL) return(NULL);

		// create bitmap

	screen_dc=GetDC(NULL);

	dc=CreateCompatibleDC(screen_dc);

	bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth=bitmap_wid;
	bmi.bmiHeader.biHeight=bitmap_high;
	bmi.bmiHeader.biPlanes=1;
	bmi.bmiHeader.biBitCount=32;
	bmi.bmiHeader.biCompression=BI_RGB;
	bmi.bmiHeader.biSizeImage=(4*bitmap_wid)*bitmap_high;

	bmp=CreateDIBSection(screen_dc,&bmi,DIB_RGB_COLORS,&bitmap_data,NULL,0);

	old_bmp=SelectObject(dc,bmp);

	SetMapMode(dc,MM_TEXT);
	SetMapperFlags(dc,1);
	SetTextAlign(dc,TA_BASELINE);
	SetBkMode(dc,TRANSPARENT);
	SetBkColor(dc,RGB(0,0,0));

		// clear bitmap

	brsh=CreateSolidBrush(RGB(255,255,255));
	old_brsh=SelectObject(dc,brsh);

	box.left=box.top=0;
	box.right=bitmap_wid;
	box.bottom=bitmap_high;

	FillRect(dc,&box,brsh);

	SelectObject(dc,old_brsh);
	DeleteObject(brsh);

		// draw the characters

	font=CreateFont(-size,0,0,0,FW_MEDIUM,0,0,0,0,OUT_OUTLINE_PRECIS,0,ANTIALIASED_QUALITY,0,name);
	if (font!=NULL) old_font=SelectObject(dc,font);
	
	SetTextColor(dc,RGB(0,0,0));

	for (n=0;n!=90;n++) {

			// draw the character

		ch=(unsigned char)(n+'!');

		x=(n%d3_size_font->char_per_line)*d3_size_font->char_box_wid;
		y=((n/d3_size_font->char_per_line)*d3_size_font->char_box_high)+d3_size_font->char_baseline;

		TextOut(dc,x,y,(char*)&ch,1);

			// get the spacing information

		GetCharABCWidths(dc,ch,ch,&ch_abc);
		d3_size_font->char_size[n]=((float)(ch_abc.abcA+ch_abc.abcB+ch_abc.abcC))/((float)d3_size_font->char_box_wid);
	}

	if (font!=NULL) {
		SelectObject(dc,old_font);
		DeleteObject(font);
	}
	
		// get the bitmap

	data_ptr=data;

	for (y=0;y!=bitmap_high;y++) {

		bitmap_ptr=bitmap_data+((4*bitmap_wid)*((bitmap_high-1)-y));

		for (x=0;x!=bitmap_wid;x++) {

			*data_ptr++=0xFF;
			*data_ptr++=0xFF;
			*data_ptr++=0xFF;

			*data_ptr++=0xFF-(*bitmap_ptr);		// use the anti-aliased font as the alpha mask

			bitmap_ptr+=4;
		}
	}

		// delete the bitmap

	SelectObject(dc,old_bmp);
	DeleteObject(bmp);
	DeleteDC(dc);

	return(data);
}

#endif

/* =======================================================

      Create Font Main Line
      
======================================================= */

unsigned char* bitmap_text_size_data(texture_font_size_type *d3_size_font,char *name,int size,int bitmap_wid,int bitmap_high)
{
	float			f_size;

	f_size=(float)size;

	d3_size_font->char_box_wid=(int)(f_size*1.05f);
	d3_size_font->char_box_high=(int)(f_size*1.5f);
	d3_size_font->char_real_high=(int)(f_size*1.15f);
	d3_size_font->char_baseline=(int)(f_size*0.82);
	d3_size_font->char_per_line=bitmap_wid/d3_size_font->char_box_wid;

	d3_size_font->gl_xoff=((float)d3_size_font->char_box_wid/(float)bitmap_wid);
	d3_size_font->gl_xadd=(d3_size_font->gl_xoff-0.005f);
	d3_size_font->gl_yoff=((float)d3_size_font->char_box_high/(float)bitmap_high);
	d3_size_font->gl_yadd=(((float)d3_size_font->char_real_high/(float)bitmap_high)-0.005f);

	return(bitmap_text_size_internal(d3_size_font,name,size,bitmap_wid,bitmap_high));
}

void bitmap_text_size(texture_font_size_type *d3_size_font,char *name,int size,int bitmap_wid,int bitmap_high)
{
	unsigned char	*data;

	data=bitmap_text_size_data(d3_size_font,name,size,bitmap_wid,bitmap_high);
	bitmap_data(&d3_size_font->bitmap,data,bitmap_wid,bitmap_high,TRUE,FALSE,FALSE,FALSE,FALSE);
	free(data);
}

void bitmap_text_initialize(texture_font_type *d3_font)
{
	int				n,idx;

		// determine which font exists, and use that

	idx=0;

	for (n=0;n!=max_iface_font_variant;n++) {
		if (bitmap_text_font_exist(d3_font->name[n])) {
			idx=n;
			break;
		}
	}

		// load the font

	bitmap_text_size(&d3_font->size_24,d3_font->name[idx],24,512,256);
	bitmap_text_size(&d3_font->size_48,d3_font->name[idx],48,1024,512);
}

void bitmap_text_shutdown(texture_font_type *d3_font)
{
	bitmap_close(&d3_font->size_24.bitmap);
	bitmap_close(&d3_font->size_48.bitmap);
}
