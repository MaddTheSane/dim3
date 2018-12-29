//
// dim3RTL
// by Brian Barnes (bbarnes@klinksoftware.com)
// free software offered as is for any use
//

#include "ray_os_defs.h"
#include "ray.h"
#include "ray_interface.h"

extern ray_global_type		ray_global;

/* =======================================================

      Float to ULong
      
======================================================= */

unsigned long ray_create_ulong_color_from_float(ray_color_type *col)
{
	int			int_r,int_g,int_b,int_a;

	int_r=(int)(col->r*255.0f);
	int_g=(int)(col->g*255.0f);
	int_b=(int)(col->b*255.0f);
	int_a=(int)(col->a*255.0f);
	
#ifdef __BIG_ENDIAN__
	return((int_r<<24)|(int_g<<16)|(int_b<<8)|a);
#else
	return((int_r)|(int_g<<8)|(int_b<<16)|(int_a<<24));
#endif
}

unsigned long ray_create_ulong_color_from_float_no_alpha(ray_color_type *col)
{
	int			int_r,int_g,int_b;

	int_r=(int)(col->r*255.0f);
	int_g=(int)(col->g*255.0f);
	int_b=(int)(col->b*255.0f);
	
#ifdef __BIG_ENDIAN__
	return((int_r<<24)|(int_g<<16)|(int_b<<8)|0x000000FF);
#else
	return((int_r)|(int_g<<8)|(int_b<<16)|0xFF000000);
#endif
}

unsigned long ray_create_ulong_color_from_float_no_alpha_clamp(ray_color_type *col)
{
	int			int_r,int_g,int_b;

	int_r=((int)(col->r*255.0f))&0xFF;
	int_g=((int)(col->g*255.0f))&0xFF;
	int_b=((int)(col->b*255.0f))&0xFF;
	
#ifdef __BIG_ENDIAN__
	return((int_r<<24)|(int_g<<16)|(int_b<<8)|0x000000FF);
#else
	return((int_r)|(int_g<<8)|(int_b<<16)|0xFF000000);
#endif
}

/* =======================================================

      ULong to Float
      
======================================================= */

void ray_create_float_color_from_ulong(unsigned long ul,ray_color_type *col)
{
#ifdef __BIG_ENDIAN__
	col->r=((float)((ul>>24)&0xFF))*(1.0f/255.0f);
	col->g=((float)((ul>>16)&0xFF))*(1.0f/255.0f);
	col->b=((float)((ul>>8)&0xFF))*(1.0f/255.0f);
	col->a=((float)(ul&0xFF))*(1.0f/255.0f);
#else
	col->r=((float)(ul&0xFF))*(1.0f/255.0f);
	col->g=((float)((ul>>8)&0xFF))*(1.0f/255.0f);
	col->b=((float)((ul>>16)&0xFF))*(1.0f/255.0f);
	col->a=((float)((ul>>24)&0xFF))*(1.0f/255.0f);
#endif
}

void ray_create_float_color_from_ulong_no_alpha(unsigned long ul,ray_color_type *col)
{
#ifdef __BIG_ENDIAN__
	col->r=((float)((ul>>24)&0xFF))*(1.0f/255.0f);
	col->g=((float)((ul>>16)&0xFF))*(1.0f/255.0f);
	col->b=((float)((ul>>8)&0xFF))*(1.0f/255.0f);
#else
	col->r=((float)(ul&0xFF))*(1.0f/255.0f);
	col->g=((float)((ul>>8)&0xFF))*(1.0f/255.0f);
	col->b=((float)((ul>>16)&0xFF))*(1.0f/255.0f);
#endif
	col->a=1.0f;
}

/* =======================================================

      Black ULong
      
======================================================= */

unsigned long ray_create_ulong_black(void)
{
#ifdef __BIG_ENDIAN__
	return(0x000000FF);
#else
	return(0xFF000000);
#endif
}


