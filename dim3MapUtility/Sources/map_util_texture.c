/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Texture Reading

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

extern file_path_setup_type	file_path_setup;

/* =======================================================

      New Map Textures
      
======================================================= */

void map_textures_new(map_type *map)
{
	int					n;
	texture_type		*texture;
	
	texture=map->textures;
	
	for (n=0;n!=max_map_texture;n++) {
		bitmap_texture_clear(texture);
		texture++;
	}
}

/* =======================================================

      Read Map Textures
      
======================================================= */

void map_textures_read_setup(map_type *map)
{
	int					n,k;
	texture_type		*texture;
	texture_frame_type	*frame;

		// clear all textures

	texture=map->textures;
	
	for (n=0;n!=max_map_texture;n++) {
			
		frame=texture->frames;
		
		for (k=0;k!=max_texture_frame;k++) {
			bitmap_new(&frame->bitmap);
			bitmap_new(&frame->bumpmap);
			bitmap_new(&frame->specularmap);
			bitmap_new(&frame->glowmap);
			
			frame++;
		}

		texture++;
	}
}

bool map_textures_read_texture_ok(map_type *map,int txt_idx)
{
	int					n,k;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	map_liquid_type		*liq;

		// check meshes

	mesh=map->mesh.meshes;
		
	for (n=0;n!=map->mesh.nmesh;n++) {
			
		poly=mesh->polys;
			
		for (k=0;k!=mesh->npoly;k++) {
			if ((poly->txt_idx==txt_idx) || (poly->lmap_txt_idx==txt_idx)) return(TRUE);
			poly++;
		}
			
		mesh++;
	}
		
		// check liquids

	liq=map->liquid.liquids;
		
	for (n=0;n!=map->liquid.nliquid;n++) {
		if ((liq->txt_idx==txt_idx) || (liq->lmap_txt_idx==txt_idx) || (liq->overlay.txt_idx==txt_idx)) return(TRUE);
		liq++;
	}
		
		// fogs, backgrounds, and sky

	if (map->fog.texture_idx==txt_idx) return(TRUE);
	if (map->background.on) {
		if (map->background.front.fill==txt_idx) return(TRUE);
		if (map->background.middle.fill==txt_idx) return(TRUE);
		if (map->background.back.fill==txt_idx) return(TRUE);
	}
	if (map->sky.on) {
		if (map->sky.fill==txt_idx) return(TRUE);
		if (map->sky.bottom_fill==txt_idx) return(TRUE);
		if (map->sky.north_fill==txt_idx) return(TRUE);
		if (map->sky.south_fill==txt_idx) return(TRUE);
		if (map->sky.east_fill==txt_idx) return(TRUE);
		if (map->sky.west_fill==txt_idx) return(TRUE);
	}

	return(FALSE);
}

void map_textures_read_texture(map_type *map,int txt_idx)
{
	int					n;
	char				path[1024],name[256];
	texture_type		*texture;
	texture_frame_type	*frame;
						
		// if in engine, then only load textures
		// directly hooked up to elements

#ifdef D3_ENGINE
	if (!map_textures_read_texture_ok(map,txt_idx)) return;
#endif

		// load textures
	
	texture=&map->textures[txt_idx];
			
		// load texture

	frame=texture->frames;
	
	for (n=0;n!=max_texture_frame;n++) {
	
		if (frame->name[0]!=0x0) {
				
				// bitmap
			
			file_paths_data(&file_path_setup,path,"Bitmaps/Textures",frame->name,"png");
			bitmap_open(&frame->bitmap,path,TRUE,texture->compress,texture->pixelated,FALSE,FALSE,FALSE);

				// bumpmap
				
			sprintf(name,"%s_n",frame->name);
			file_paths_data(&file_path_setup,path,"Bitmaps/Textures",name,"png");		// compresses messes up normals
			bitmap_open(&frame->bumpmap,path,TRUE,FALSE,texture->pixelated,FALSE,FALSE,texture->flip_normal);
							
				// specular map
				
			sprintf(name,"%s_s",frame->name);
			file_paths_data(&file_path_setup,path,"Bitmaps/Textures",name,"png");
			bitmap_open(&frame->specularmap,path,TRUE,texture->compress,texture->pixelated,FALSE,FALSE,FALSE);

				// glow map
				
			sprintf(name,"%s_g",frame->name);
			file_paths_data(&file_path_setup,path,"Bitmaps/Textures",name,"png");
			bitmap_open(&frame->glowmap,path,TRUE,texture->compress,texture->pixelated,FALSE,TRUE,FALSE);
		}
		
		frame++;
	}
}

void map_textures_read_complete(map_type *map)
{
	int				n;

	map_textures_read_setup(map);

	for (n=0;n!=max_map_texture;n++) {
		map_textures_read_texture(map,n);
	}
}

/* =======================================================

      Close Map Textures
      
======================================================= */

void map_textures_close_texture(map_type *map,int txt_idx)
{
	int					n;
	texture_type		*texture;
	texture_frame_type	*frame;
	
	texture=&map->textures[txt_idx];
	frame=texture->frames;
		
	for (n=0;n!=max_texture_frame;n++) {
		bitmap_close(&frame->bitmap);
		bitmap_close(&frame->bumpmap);
		bitmap_close(&frame->specularmap);
		bitmap_close(&frame->glowmap);
		
		frame++;
	}
}

void map_textures_close(map_type *map)
{
	int				n;
	
	for (n=0;n!=max_map_texture;n++) {
		map_textures_close_texture(map,n);
	}
}

