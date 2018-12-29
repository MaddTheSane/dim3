/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Texture

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

#define kTexturePropertyAnimate				0
#define kTexturePropertyAlphaAdditive		1
#define kTexturePropertyPixelated			2
#define kTexturePropertyCompress			3

#define kTexturePropertyShader				5
#define kTexturePropertyGlowRate			6
#define kTexturePropertyGlowMin				7
#define kTexturePropertyGlowMax				8
#define kTexturePropertyShineFactor			9
#define kTextPropertyAlphaType				10
#define kTexturePropertyRefractFactor		11
#define kTexturePropertyMaterialName		12
#define kTexturePropertyScaleOffset			13
#define kTexturePropertyScaleSize			14

#define kTexturePropertyFrameWaitStart		20
#define kTexturePropertyFrameWaitEnd		(20+(max_texture_frame-1))

extern map_type					map;
extern iface_type				iface;

extern list_palette_type		map_palette;

char							texture_rtl_alpha_type_str[][32]={"Pass Through","Reflection","Refraction","Additive",""};

/* =======================================================

      Property Palette Fill Texture
      
======================================================= */

void map_palette_fill_texture(int texture_idx)
{
	int					n;
	texture_type		*texture;

	texture=&map.textures[texture_idx];

	list_palette_set_title(&map_palette,"Texture",texture->frames[0].name,NULL,NULL,NULL,NULL);

	list_palette_add_header(&map_palette,0,"Texture Settings");
	list_palette_add_checkbox(&map_palette,kTexturePropertyAnimate,"Animated",&texture->animate.on,FALSE);
	list_palette_add_checkbox(&map_palette,kTexturePropertyAlphaAdditive,"Alpha Additive",&texture->additive,FALSE);
	list_palette_add_checkbox(&map_palette,kTexturePropertyPixelated,"Pixelated",&texture->pixelated,FALSE);
	list_palette_add_checkbox(&map_palette,kTexturePropertyCompress,"Compressed",&texture->compress,FALSE);
	list_palette_add_checkbox(&map_palette,-1,"Flip Normals",&texture->flip_normal,FALSE);

	list_palette_add_header(&map_palette,0,"Texture Options");
	list_palette_add_picker_list_string(&map_palette,kTexturePropertyShader,"Shader Override",(char*)iface.shader_list.shaders,iface.shader_list.nshader,sizeof(iface_shader_type),(int)offsetof(iface_shader_type,name),TRUE,TRUE,texture->shader_name,FALSE);
	list_palette_add_int(&map_palette,kTexturePropertyGlowRate,"Glow Rate",&texture->glow.rate,FALSE);
	list_palette_add_float(&map_palette,kTexturePropertyGlowMin,"Glow Min",&texture->glow.min,FALSE);
	list_palette_add_float(&map_palette,kTexturePropertyGlowMax,"Glow Max",&texture->glow.max,FALSE);
	list_palette_add_float(&map_palette,kTexturePropertyShineFactor,"Shine Factor",&texture->shine_factor,FALSE);
	list_palette_add_picker_list_int(&map_palette,kTextPropertyAlphaType,"RTL Alpha Type",(char*)texture_rtl_alpha_type_str,-1,name_str_len,0,FALSE,FALSE,&texture->rl_alpha_type,FALSE);
	list_palette_add_float(&map_palette,kTexturePropertyRefractFactor,"RTL Refraction Factor",&texture->rl_refract_factor,FALSE);
	list_palette_add_string(&map_palette,kTexturePropertyMaterialName,"Material Name",texture->material_name,name_str_len,FALSE);

	list_palette_add_header(&map_palette,0,"Texture Frame Waits");
	for (n=0;n!=max_texture_frame;n++) {
		if (texture->frames[n].name[0]!=0x0) {
			list_palette_add_int(&map_palette,(kTexturePropertyFrameWaitStart+n),texture->frames[n].name,&texture->animate.wait[n],FALSE);
		}
		else {
			list_palette_add_int(&map_palette,(kTexturePropertyFrameWaitStart+n),"(empty)",&texture->animate.wait[n],FALSE);
		}
	}

	list_palette_add_header(&map_palette,0,"Texture UVs");
	list_palette_add_uv(&map_palette,kTexturePropertyScaleOffset,"Offset",&texture->scale.uv_offset,FALSE);
	list_palette_add_uv(&map_palette,kTexturePropertyScaleSize,"Scale (1000 units)",&texture->scale.uv_size,FALSE);
}

/* =======================================================

      Property Palette Click Texture
      
======================================================= */

void map_palette_click_texture(int texture_idx,int id,bool double_click)
{
}

