/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Texture Frames

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

/* =======================================================

      Count Texture Frames
      
======================================================= */

int map_count_texture_frames(map_type *map,int txt)
{
	int					n;
	texture_type		*texture;
		
	texture=&map->textures[txt];
	
	for (n=0;n!=max_texture_frame;n++) {
		if (texture->frames[n].name[0]==0x0) return(n);
	}
	
	return(max_texture_frame);
}

/* =======================================================

      Set Current Frames
      
======================================================= */

void map_setup_animated_textures(map_type *map,int tick)
{
	bitmap_texture_setup_animation(map->textures,max_map_texture,tick);
}

/* =======================================================

      Texture Changing
      
======================================================= */

bool map_add_texture_frame(map_type *map,int txt,char *bitmap_name)
{
	int			count;
	
	count=map_count_texture_frames(map,txt);
	if (count==max_texture_frame) return(FALSE);
	
	map_textures_close_texture(map,txt);
	
	strcpy(map->textures[txt].frames[count].name,bitmap_name);
	
	map_textures_read_texture(map,txt);
	return(TRUE);
}

bool map_delete_texture_frame(map_type *map,int txt)
{
	int			count;
	
	count=map_count_texture_frames(map,txt);
	if (count==0) return(FALSE);
	
	map_textures_close_texture(map,txt);
	
	count--;
	
	map->textures[txt].frames[count].name[0]=0x0;
	
	bitmap_new(&map->textures[txt].frames[count].bitmap);
	bitmap_new(&map->textures[txt].frames[count].bumpmap);
	bitmap_new(&map->textures[txt].frames[count].specularmap);
	bitmap_new(&map->textures[txt].frames[count].glowmap);
	
	map_textures_read_texture(map,txt);
	return(TRUE);
}

bool map_replace_texture(map_type *map,int txt,char *bitmap_name)
{
	int					n;
	texture_type		*texture;

	map_textures_close_texture(map,txt);
	
	texture=&map->textures[txt];
	
	for (n=0;n!=max_texture_frame;n++) {
		texture->frames[n].name[0]=0x0;
	}
	
	strcpy(map->textures[txt].frames[0].name,bitmap_name);

	map_textures_read_texture(map,txt);
	return(TRUE);
}

bool map_delete_texture(map_type *map,int start_txt,int end_txt)
{
	int					n,k;
	texture_type		*texture;

	for (n=start_txt;n!=end_txt;n++) {
		map_textures_close_texture(map,n);

		texture=&map->textures[n];
	
		for (k=0;k!=max_texture_frame;k++) {
			texture->frames[k].name[0]=0x0;
		}
	
	}

	return(TRUE);
}


