/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Text Routines

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

#include "video.h"

// supergumba -- move to defs file

#define font_bitmap_pixel_sz			256
#define font_bitmap_point				28
#define font_bitmap_char_wid			24
#define font_bitmap_char_high			28
#define font_bitmap_char_baseline		21

extern hud_type				hud;
extern setup_type			setup;

bool						font_small;
float						font_char_size[90]=
								{0.38f,0.42f,0.67f,0.67f,0.71f,0.75f,0.29f,0.50f,0.33f,0.54f,
								 0.83f,0.38f,0.40f,0.38f,0.58f,0.67f,0.67f,0.67f,0.67f,0.67f,
								 0.67f,0.67f,0.67f,0.67f,0.67f,0.38f,0.38f,0.83f,0.83f,0.83f,
								 0.46f,0.92f,0.75f,0.62f,0.70f,0.79f,0.67f,0.64f,0.79f,0.79f,
								 0.33f,0.50f,0.71f,0.58f,0.90f,0.79f,0.83f,0.58f,0.83f,0.67f,
								 0.67f,0.67f,0.75f,0.71f,0.96f,0.67f,0.67f,0.67f,0.50f,0.58f,
								 0.38f,0.67f,0.54f,0.67f,0.58f,0.62f,0.54f,0.67f,0.58f,0.42f,
								 0.67f,0.67f,0.33f,0.33f,0.62f,0.33f,0.90f,0.64f,0.65f,0.60f,
								 0.67f,0.46f,0.54f,0.42f,0.64f,0.60f,0.83f,0.60f,0.58f,0.62f};

bitmap_type					font_bitmap;

/* =======================================================

      Setup Text Drawing
      
======================================================= */

#ifdef D3_OS_WINDOWS

void gl_text_initialize(void)
{
	int				n,x,y,font_high;
	unsigned char	ch;
	unsigned char	*data,*ptr;
	HDC				screen_dc,dc;
	HBITMAP			bmp,*old_bmp;
	HFONT			font;
	COLORREF		col;

		// data for bitmap

	data=valloc((font_bitmap_pixel_sz<<2)*font_bitmap_pixel_sz);
	if (data==NULL) return;

		// create bitmap

	screen_dc=GetDC(NULL);

	dc=CreateCompatibleDC(screen_dc);
	bmp=CreateCompatibleBitmap(screen_dc,font_bitmap_pixel_sz,font_bitmap_pixel_sz);
	old_bmp=SelectObject(dc,bmp);

	SetMapMode(dc,MM_TEXT);
	SetMapperFlags(dc,1);
	SetTextAlign(dc,TA_BASELINE);
	SetBkMode(dc,OPAQUE);
	SetBkColor(dc,RGB(0,0,0));

		// draw the characters

	font_high=-MulDiv(font_bitmap_point,GetDeviceCaps(dc,LOGPIXELSY),72);
	font=CreateFont(-font_high,0,0,0,FW_NORMAL,0,0,0,0,OUT_OUTLINE_PRECIS,0,ANTIALIASED_QUALITY,0,"Arial");
	SelectObject(dc,font);
	SetTextColor(dc,RGB(255,255,255));

	for (n=0;n!=90;n++) {
		ch=(unsigned char)(n+'!');

		x=(n%10)*font_bitmap_char_wid;
		y=((n/10)*font_bitmap_char_high)+font_bitmap_char_baseline;

		TextOut(dc,x,y,(char*)&ch,1);
	}

	DeleteObject(font);

		// get the bitmap

	ptr=data;

	for (y=0;y!=font_bitmap_pixel_sz;y++) {

		for (x=0;x!=font_bitmap_pixel_sz;x++) {

			col=GetPixel(dc,x,y);

			*ptr++=0xFF;
			*ptr++=0xFF;
			*ptr++=0xFF;

			*ptr++=GetRValue(col);		// use the anti-aliased font as the alpha mask

		}
	}

	bitmap_data(&font_bitmap,"font",data,font_bitmap_pixel_sz,font_bitmap_pixel_sz,anisotropic_mode_none,mipmap_mode_none,FALSE);

	free(data);

		// delete the bitmap

	SelectObject(dc,old_bmp);
	DeleteObject(bmp);
	DeleteDC(dc);
}

#endif

#ifdef D3_OS_MAC

void gl_text_initialize(void)
{
	int					n,x,y,row_add;
	unsigned char		ch;
	unsigned char		*data,*sptr,*dptr;
	Rect				box;
	PixMapHandle		texturemap;
	GWorldPtr			gworld;
	CGrafPtr			org_gworld;
	GDHandle			org_gdhand;
	
		// data for bitmap

	data=valloc((font_bitmap_pixel_sz<<2)*font_bitmap_pixel_sz);
	if (data==NULL) return;

		// create bitmap
		
	SetRect(&box,0,0,font_bitmap_pixel_sz,font_bitmap_pixel_sz);
	NewGWorld(&gworld,32,&box,NULL,NULL,0);
	
	GetGWorld(&org_gworld,&org_gdhand);
	SetGWorld(gworld,NULL);
	
		// draw the characters

	TextFont(FMGetFontFamilyFromName("\pArial"));
	TextSize(font_bitmap_point);
	TextFace(0);
	
	SwapQDTextFlags(kQDUseCGTextRendering|kQDUseCGTextMetrics);
		
	for (n=0;n!=90;n++) {
		ch=(unsigned char)(n+'!');

		x=(n%10)*font_bitmap_char_wid;
		y=((n/10)*font_bitmap_char_high)+font_bitmap_char_baseline;

		MoveTo(x,y);
		DrawChar(ch);
	}

		// get the bitmap

	texturemap=GetGWorldPixMap(gworld);

	LockPixels(texturemap);
	sptr=(unsigned char*)GetPixBaseAddr(texturemap);
	row_add=GetPixRowBytes(texturemap)-(font_bitmap_pixel_sz<<2);
	
	dptr=data;

	for (y=0;y!=font_bitmap_pixel_sz;y++) {

		for (x=0;x!=font_bitmap_pixel_sz;x++) {

			*dptr++=0xFF;
			*dptr++=0xFF;
			*dptr++=0xFF;

			*dptr++=*sptr++;		// use the anti-aliased font as the alpha mask (using the red component)
			sptr+=3;

		}
		
		sptr+=row_add;
	}
	
	UnlockPixels(texturemap);
	
	bitmap_data(&font_bitmap,"font",data,font_bitmap_pixel_sz,font_bitmap_pixel_sz,anisotropic_mode_none,mipmap_mode_none,FALSE);

	free(data);

		// dispose the gworld
		
	SetGWorld(org_gworld,org_gdhand);
	DisposeGWorld(gworld);
}

#endif

void gl_text_shutdown(void)
{
	bitmap_close(&font_bitmap);
}

/* =======================================================

      Text Sizes
      
======================================================= */

inline int gl_text_get_char_width(bool small_text)
{
	if (small_text) return((int)(((float)hud.scale_x)*text_small_factor));
	return((int)(((float)hud.scale_x)*text_large_factor));
}

inline int gl_text_get_char_height(bool small_text)
{
	return((int)(((float)gl_text_get_char_width(small_text))*text_height_factor));
}

int gl_text_get_string_width(char *str,bool small_text)
{
	int			i,ch,x,wid,len;
	char		*c;
	
	wid=gl_text_get_char_width(small_text);
	
	c=str;
	len=strlen(str);
	
	x=0;
	
	for (i=0;i<len;i++) {
		ch=(int)*c++;

		if ((ch>='!') && (ch<='z')) {
			ch-='!';
			x+=(int)(wid*font_char_size[ch]);
		}
		else {
			x+=(wid/3);
		}
	}
	
	return(x);
}

/* =======================================================

      Start and End Text Drawing
      
======================================================= */

void gl_text_start(bool small_text)
{
	GLfloat				fct[4];

		// remember font size
		
	font_small=small_text;
	
		// setup texture
		
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,font_bitmap.gl_id);
	
		// texture combines
		
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);

	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);
	
		// alpha
		
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_ALPHA,GL_SRC_ALPHA);

	fct[0]=fct[1]=fct[2]=fct[3]=1.0f;	
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,fct);
	
		// no wrapping
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	
		// transparency blending
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void gl_text_end(void)
{
	glDisable(GL_BLEND);
	
		// restore wrapping
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    
	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Draw Line of Text
      
======================================================= */

void gl_text_draw_line(int x,int y,char *txt,int txtlen,bool vcenter)
{
	int			i,ch,
				lft,rgt,top,bot,xoff,yoff,wid,high;
	float		gx_lft,gx_rgt,gy_top,gy_bot;
	char		*c;
    
		// get width and height
		
	wid=gl_text_get_char_width(font_small);
	high=gl_text_get_char_height(font_small);
	
		// draw text
	
	glBegin(GL_QUADS);

	lft=x;
	bot=y;
	if (vcenter) bot+=((high>>1)+(high/8));		// add in middle + descender
	top=bot-high;
	
	c=txt;
	
	for (i=0;i<txtlen;i++) {
	
		ch=(int)*c++;

		if ((ch>='!') && (ch<='z')) {
		
			ch-='!';
			yoff=ch/10;
			xoff=ch-(yoff*10);

			gx_lft=((float)xoff)*0.09375f;
			gx_rgt=gx_lft+0.089f;
			gy_top=((float)yoff)*0.109375f;
			gy_bot=gy_top+0.108f;
			
			rgt=lft+wid;
            
			glTexCoord2f(gx_lft,gy_top);
			glVertex2i(lft,top);
			glTexCoord2f(gx_rgt,gy_top);
			glVertex2i(rgt,top);
			glTexCoord2f(gx_rgt,gy_bot);
			glVertex2i(rgt,bot);
			glTexCoord2f(gx_lft,gy_bot);
			glVertex2i(lft,bot);
			
			lft+=(int)(wid*font_char_size[ch]);
			
		}
		else {
			lft+=wid/3;
		}
	}

	glEnd();
}

void gl_text_draw(int x,int y,char *txt,int just,bool vcenter,d3col *col,float alpha)
{
	GLfloat				fct[4];

        // font justification
        
	switch (just) {
		case tx_center:
			x-=(gl_text_get_string_width(txt,font_small)>>1);
			break;
		case tx_right:
			x-=gl_text_get_string_width(txt,font_small);
			break;
	}
	
        // font color and alpha
        
	glColor3f(col->r,col->g,col->b);

	fct[0]=fct[1]=fct[2]=1.0f;
	fct[3]=alpha;
	
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,fct);

		// draw text line

	gl_text_draw_line(x,y,txt,strlen(txt),vcenter);
}
