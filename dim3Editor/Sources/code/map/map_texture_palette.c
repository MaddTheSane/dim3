/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Texture Palette

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

extern map_type				map;
extern app_state_type		state;
extern app_pref_type		pref;

extern int					txt_palette_cur_page;
extern list_palette_type	file_palette,map_palette;

/* =======================================================

      Texture Palette Settings
      
======================================================= */

int map_texture_palette_per_page_count(void)
{
	return(pref.big_texture?16:32);
}

int map_texture_palette_page_list_count(void)
{
	return(max_map_texture/map_texture_palette_per_page_count());
}

int map_texture_palette_page_list_width(void)
{
	return((map_texture_palette_page_list_count()>>1)*16);
}

int map_texture_palette_pixel_size(void)
{
	int				lx,rx;
	d3rect			wbox;
	
	os_get_window_box(&wbox);
	
	lx=wbox.lx+list_palette_width(&file_palette);
	rx=wbox.rx-list_palette_width(&map_palette);
	
	return((rx-(lx+map_texture_palette_page_list_width()))/map_texture_palette_per_page_count());
}

void map_texture_palette_box(d3rect *box)
{
	int				pixel_sz;
	d3rect			wbox;
	
	pixel_sz=map_texture_palette_pixel_size();
	
	os_get_window_box(&wbox);
	
	box->lx=wbox.lx+list_palette_width(&file_palette);
	box->rx=wbox.rx-list_palette_width(&map_palette);
	box->ty=wbox.by-pixel_sz;
	box->by=(box->ty+pixel_sz)+1;
}

/* =======================================================

      Texture Palette Disabled
      
======================================================= */

bool map_texture_palette_get_disabled_state(void)
{
	return(!state.map.map_open);
}

/* =======================================================

      Get and Put Selection Texture
      
======================================================= */

int map_texture_palette_get_selected_texture(void)
{
	int					type,main_idx,poly_idx;
	
	if (select_count()!=1) return(-1);
	
	select_get(0,&type,&main_idx,&poly_idx);
	
	if (type==item_map_liquid) {
		if (view_get_uv_layer()==uv_layer_normal) return(map.liquid.liquids[main_idx].txt_idx);
		return(map.liquid.liquids[main_idx].lmap_txt_idx);
	}
	
	if (type==item_map_mesh) {
		if (view_get_uv_layer()==uv_layer_normal) return(map.mesh.meshes[main_idx].polys[poly_idx].txt_idx);
		return(map.mesh.meshes[main_idx].polys[poly_idx].lmap_txt_idx);
	}
	
	return(-1);
}

void map_texture_palette_put_selected_texture(int txt_idx)
{
	int					n,k,sel_count,type,main_idx,poly_idx;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
	
		select_get(n,&type,&main_idx,&poly_idx);
		
			// liquids
			
		if (type==item_map_liquid) {
			if (view_get_uv_layer()==uv_layer_normal) {
				map.liquid.liquids[main_idx].txt_idx=txt_idx;
			}
			else {
				map.liquid.liquids[main_idx].lmap_txt_idx=txt_idx;
			}
			continue;
		}
		
			// meshes
			
		if (type!=item_map_mesh) continue;
		
		mesh=&map.mesh.meshes[main_idx];
		
			// only set polygon
			
		if (state.map.select_mode==select_mode_polygon) {
			if (view_get_uv_layer()==uv_layer_normal) {
				mesh->polys[poly_idx].txt_idx=txt_idx;
			}
			else {
				mesh->polys[poly_idx].lmap_txt_idx=txt_idx;
			}
		}
		
			// set all mesh
			
		else {
			
			poly=mesh->polys;
			
			for (k=0;k!=mesh->npoly;k++) {
				if (view_get_uv_layer()==uv_layer_normal) {
					poly->txt_idx=txt_idx;
				}
				else {
					poly->lmap_txt_idx=txt_idx;
				}
				poly++;
			}
		
		}		
	}
}

/* =======================================================

      Reset Texture Palette
      
======================================================= */

void map_texture_palette_reset(void)
{
	int			sel;
	
		// get to proper texture page
		
	sel=map_texture_palette_get_selected_texture();
	if (sel!=-1) txt_palette_cur_page=sel/(pref.big_texture?16:32);
}


