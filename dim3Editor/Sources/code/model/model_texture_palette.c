/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Texture Palette

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

extern bool						list_palette_open;

extern model_type				model;
extern app_state_type			state;

extern list_palette_type		file_palette,model_palette;

/* =======================================================

      Texture Palette Settings
      
======================================================= */

int model_texture_palette_per_page_count(void)
{
	return(max_model_texture);
}

int model_texture_palette_page_list_count(void)
{
	return(1);
}

int model_texture_palette_page_list_width(void)
{
	return(0);
}

int model_texture_palette_pixel_size(void)
{
	int				lx,rx;
	d3rect			wbox;
	
	os_get_window_box(&wbox);
	
	lx=wbox.lx+list_palette_width(&file_palette);
	rx=wbox.rx-list_palette_width(&model_palette);

	return((rx-(lx+model_texture_palette_page_list_width()))/model_texture_palette_per_page_count());
}

void model_texture_palette_box(d3rect *box)
{
	int				pixel_sz;
	d3rect			wbox;
	
	pixel_sz=model_texture_palette_pixel_size();
	
	os_get_window_box(&wbox);
	
	box->lx=wbox.lx+list_palette_width(&file_palette);
	box->rx=wbox.rx-list_palette_width(&model_palette);
	box->ty=wbox.by-pixel_sz;
	box->by=(box->ty+pixel_sz)+1;
}

/* =======================================================

      Texture Palette Disabled
      
======================================================= */

bool model_texture_palette_get_disabled_state(void)
{
	return(!state.model.model_open);
}

/* =======================================================

      Get and Set Textures
      
======================================================= */

int model_texture_palette_get_selected_texture(void)
{
	int				poly_idx;

		// only get texture if we are in polygon mode

	if (state.model.select_mode!=select_mode_polygon) return(-1);

	poly_idx=model_poly_mask_get_first_sel(state.model.cur_mesh_idx);
	if (poly_idx==-1) return(-1);

	return(model.meshes[state.model.cur_mesh_idx].polys[poly_idx].txt_idx);
}

void model_texture_palette_put_selected_texture(int txt_idx)
{
		// if vertex mode, select vertexes
		// if poly mode, replace textures

	switch (state.model.select_mode) {
		
		case select_mode_vertex:
			model_vertex_mask_set_sel_texture(state.model.cur_mesh_idx,txt_idx);
			break;

		case select_mode_polygon:
			model_poly_mask_set_sel_texture(state.model.cur_mesh_idx,txt_idx);
			break;

	}
}

