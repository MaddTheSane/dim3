/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View dim3RTL Material

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
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "ray_interface.h"

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

extern int					view_rtl_draw_scene_id;

/* =======================================================

      dim3RTL Create Material from Texture
      
======================================================= */

int view_dim3rtl_create_material_from_texture(char *sub_path,texture_type *texture,texture_frame_type *frame)
{
	int					material_id,wid,high,png_wid,png_high;
	bool				alpha_channel;
	char				name[256],path[1024];
	unsigned char		*png_data;

		// create material, use original
		// bitmap for wid/high
		
	file_paths_data(&file_path_setup,path,sub_path,frame->name,"png");
		
	png_data=png_utility_read(path,&wid,&high,&alpha_channel);
	if (png_data==NULL) return(-1);
	
	material_id=rtlMaterialAdd(wid,high,texture->rl_alpha_type,0);
	rtlMaterialAttachBufferData(material_id,RL_MATERIAL_TARGET_COLOR,(alpha_channel?RL_MATERIAL_FORMAT_32_RGBA:RL_MATERIAL_FORMAT_24_RGB),png_data);
	
	free(png_data);

	rtlMaterialSetRefractionFactor(material_id,texture->rl_refract_factor);
	
		// normal map
		
	sprintf(name,"%s_n",frame->name);
	file_paths_data(&file_path_setup,path,sub_path,name,"png");

	png_data=png_utility_read(path,&png_wid,&png_high,&alpha_channel);
	if (png_data!=NULL) {
		if ((png_wid!=wid) || (png_high!=high)) {
			fprintf(stdout,"%s: Not correct size for material\n",path);
		}
		else {
			rtlMaterialAttachBufferData(material_id,RL_MATERIAL_TARGET_NORMAL,(alpha_channel?RL_MATERIAL_FORMAT_32_RGBA:RL_MATERIAL_FORMAT_24_RGB),png_data);
		}
		free(png_data);
	}

		// specular map
		
	sprintf(name,"%s_s",frame->name);
	file_paths_data(&file_path_setup,path,sub_path,name,"png");

	png_data=png_utility_read(path,&png_wid,&png_high,&alpha_channel);
	if (png_data!=NULL) {
		if ((png_wid!=wid) || (png_high!=high)) {
			fprintf(stdout,"%s: Not correct size for material\n",path);
		}
		else {
			rtlMaterialAttachBufferData(material_id,RL_MATERIAL_TARGET_SPECULAR,(alpha_channel?RL_MATERIAL_FORMAT_32_RGBA:RL_MATERIAL_FORMAT_24_RGB),png_data);
		}
		free(png_data);
	}

	rtlMaterialSetShineFactor(material_id,texture->shine_factor);
	
		// glow map
		
	sprintf(name,"%s_g",frame->name);
	file_paths_data(&file_path_setup,path,sub_path,name,"png");

	png_data=png_utility_read(path,&png_wid,&png_high,&alpha_channel);
	if (png_data!=NULL) {
		if ((png_wid!=wid) || (png_high!=high)) {
			fprintf(stdout,"%s: Not correct size for material\n",path);
		}
		else {
			rtlMaterialAttachBufferData(material_id,RL_MATERIAL_TARGET_GLOW,(alpha_channel?RL_MATERIAL_FORMAT_32_RGBA:RL_MATERIAL_FORMAT_24_RGB),png_data);
		}
		free(png_data);
	}

	rtlMaterialSetGlowFactor(material_id,texture->glow.min);

		// mipmaps

	rtlMaterialBuildMipMaps(material_id);
	
	return(material_id);
}

/* =======================================================

      dim3RTL Create Material from Path
      
======================================================= */

int view_dim3rtl_create_material_from_path(char *path,int alpha_type)
{
	int					material_id,wid,high;
	bool				alpha_channel;
	unsigned char		*png_data;

		// create material directly
		// from PNG
		
	png_data=png_utility_read(path,&wid,&high,&alpha_channel);
	if (png_data==NULL) return(-1);
	
	material_id=rtlMaterialAdd(wid,high,alpha_type,0);
	rtlMaterialAttachBufferData(material_id,RL_MATERIAL_TARGET_COLOR,(alpha_channel?RL_MATERIAL_FORMAT_32_RGBA:RL_MATERIAL_FORMAT_24_RGB),png_data);
	
	free(png_data);

	return(material_id);
}

int view_dim3rtl_create_material_from_color(d3col *col)
{
	int				material_id;
	rtlColor		rcol;
	
	rcol.r=col->r;
	rcol.g=col->g;
	rcol.b=col->b;
	rcol.a=1.0f;

	material_id=rtlMaterialAdd(1,1,RL_MATERIAL_ALPHA_PASS_THROUGH,0);
	rtlMaterialAttachBufferColor(material_id,RL_MATERIAL_TARGET_COLOR,&rcol);

	return(material_id);
}

