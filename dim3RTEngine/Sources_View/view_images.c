/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Bitmaps

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
#include "objects.h"

extern server_type			server;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

int							remote_slow_image_idx,remote_talk_image_idx;
bitmap_type					empty_bitmap;

/* =======================================================

      Initialize Images
      
======================================================= */

void view_images_initialize(void)
{
	int				n;
	d3col			col;
	view_image_type	*image;
	
	image=view.images;
	
	for (n=0;n!=max_view_image;n++) {
		image->path[n]=0x0;
		image++;
	}
		
		// first bitmap is a empty bitmap
		// to be used when file is missing
		
	image=&view.images[0];
	
	col.r=col.b=col.g=1.0f;
	bitmap_color(&image->bitmap,&col);
	
	strcpy(image->path,"_dim3_core_empty_bitmap");
}

void view_images_shutdown(void)
{
	view_image_type	*image;

	image=&view.images[0];
	
	image->path[0]=0x0;
	bitmap_close(&image->bitmap);
}

/* =======================================================

      Load Single Image
      
======================================================= */

int view_images_find_path(char *path)
{
	int					n;
	view_image_type		*image;
	
	image=view.images;

	for (n=0;n!=max_view_image;n++) {
		if (image->path[0]!=0x0) {
			if (strcmp(image->path,path)==0) return(n);
		}
		image++;
	}
	
	return(-1);
}

int view_images_find_first_free(void)
{
	int					n;
	view_image_type		*image;
	
	image=view.images;

	for (n=0;n!=max_view_image;n++) {
		if (image->path[0]==0x0) return(n);
		image++;
	}
	
	return(-1);
}

/* =======================================================

      Load Single Image
      
======================================================= */

int view_images_load_single(char *path,bool npot,bool simple)
{
	int					idx;
	view_image_type		*image;
	
		// null paths get empty image
		
	if (path==NULL) return(0);

		// already loaded?

	idx=view_images_find_path(path);
	if (idx!=-1) return(idx);
	
		// it's a new image
		// if no free spots, return empty image
		
	idx=view_images_find_first_free();
	if (idx==-1) return(0);
		
	image=&view.images[idx];

	if (simple) {
		if (!bitmap_open(&image->bitmap,path,FALSE,FALSE,FALSE,npot,FALSE,FALSE)) return(0);
	}
	else {
		if (!bitmap_open(&image->bitmap,path,TRUE,FALSE,FALSE,npot,FALSE,FALSE)) return(0);
	}

	strcpy(image->path,path);
	
	return(idx);
}

void view_images_free_single(int idx)
{
	view_image_type		*image;
	
		// don't remove first item as it's always the empty one
		// or -1, which is an error state
		
	if (idx<=0) return;
	
	image=&view.images[idx];
	if (image->path[0]==0x0) return;
	
		// mark as free
		
	image->path[0]=0x0;
   
		// close the bitmap
	
	bitmap_close(&image->bitmap);
}

/* =======================================================

      Load and Free All Game Cached Images
      
======================================================= */

void view_images_cached_load(void)
{
	int							n;
	char						path[1024];
	iface_bitmap_type			*iface_bitmap;
	iface_radar_icon_type		*icon;
	iface_halo_type				*halo;
	iface_label_type			*label;
	iface_crosshair_type		*crosshair;
#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)
	iface_virtual_stick_type	*stick;
	iface_virtual_button_type	*button;
#endif

		// hud bitmaps

	iface_bitmap=iface.bitmap_list.bitmaps;
	
	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
		file_paths_data(&file_path_setup,path,"Bitmaps/Interface",iface_bitmap->filename,"png");
		iface_bitmap->image_idx=view_images_load_single(path,FALSE,TRUE);
		iface_bitmap++;
	}

		// radar background

	file_paths_data(&file_path_setup,path,"Bitmaps/Radar",iface.radar.background_bitmap_name,"png");
	iface.radar.background_image_idx=view_images_load_single(path,FALSE,TRUE);

		// radar icons
	
	icon=iface.radar.icons;
	
	for (n=0;n!=iface.radar.nicon;n++) {
		file_paths_data(&file_path_setup,path,"Bitmaps/Radar",icon->bitmap_name,"png");
		icon->image_idx=view_images_load_single(path,FALSE,TRUE);
		icon++;
	}
	
		// halos

	halo=iface.halo_list.halos;

	for (n=0;n!=iface.halo_list.nhalo;n++) {
		file_paths_data(&file_path_setup,path,"Bitmaps/Halos",halo->bitmap_name,"png");
		halo->image_idx=view_images_load_single(path,FALSE,FALSE);
		halo++;
	}

		// labels

	label=iface.label_list.labels;

	for (n=0;n!=iface.label_list.nlabel;n++) {
		file_paths_data(&file_path_setup,path,"Bitmaps/Labels",label->bitmap_name,"png");
		label->image_idx=view_images_load_single(path,FALSE,FALSE);
		label++;
	}

		// crosshairs

	crosshair=iface.crosshair_list.crosshairs;

	for (n=0;n!=iface.crosshair_list.ncrosshair;n++) {
		file_paths_data(&file_path_setup,path,"Bitmaps/Crosshairs",crosshair->bitmap_name,"png");
		crosshair->image_idx=view_images_load_single(path,FALSE,TRUE);
		crosshair++;
	}

		// remote bitmaps

	if (net_setup.mode!=net_mode_none) {
		file_paths_data(&file_path_setup,path,"Bitmaps/Network","slow","png");
		remote_slow_image_idx=view_images_load_single(path,FALSE,TRUE);
	
		file_paths_data(&file_path_setup,path,"Bitmaps/Network","talk","png");
		remote_talk_image_idx=view_images_load_single(path,FALSE,TRUE);
	}
	
		// virtual controls
		
#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)
	
	stick=iface.virtual_control.sticks;
	
	for (n=0;n!=max_virtual_stick;n++) {
		
		if ((stick->on) && (stick->use_bitmap)) {
			file_paths_data(&file_path_setup,path,"Bitmaps/Virtual",stick->outer_bitmap_name,"png");
			stick->outer_image_idx=view_images_load_single(path,FALSE,TRUE);
		
			file_paths_data(&file_path_setup,path,"Bitmaps/Virtual",stick->inner_bitmap_name,"png");
			stick->inner_image_idx=view_images_load_single(path,FALSE,TRUE);
		}
		
		stick++;
	}

	button=iface.virtual_control.buttons;
	
	for (n=0;n!=max_virtual_button;n++) {
	
		if ((button->on) && (button->use_bitmap)) {
			file_paths_data(&file_path_setup,path,"Bitmaps/Virtual",button->up_bitmap_name,"png");
			button->up_image_idx=view_images_load_single(path,FALSE,TRUE);
	
			file_paths_data(&file_path_setup,path,"Bitmaps/Virtual",button->down_bitmap_name,"png");
			button->down_image_idx=view_images_load_single(path,FALSE,TRUE);
		}
		
		button++;
	}

#endif
}

void view_images_cached_free(void)
{
	int							n;
	iface_bitmap_type			*iface_bitmap;
	iface_radar_icon_type		*icon;
  	iface_halo_type				*halo;
	iface_label_type			*label;
	iface_crosshair_type		*crosshair;
#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)
	iface_virtual_stick_type	*stick;
	iface_virtual_button_type	*button;
#endif

		// hud bitmaps

	iface_bitmap=iface.bitmap_list.bitmaps;
	
	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
		view_images_free_single(iface_bitmap->image_idx);
		iface_bitmap++;
	}

		// radar

	if (iface.radar.on) {
	
			// radar background

		view_images_free_single(iface.radar.background_image_idx);

			// radar icons
		
		icon=iface.radar.icons;
		
		for (n=0;n!=iface.radar.nicon;n++) {
			view_images_free_single(icon->image_idx);
			icon++;
		}
	}
	
		// halos

	halo=iface.halo_list.halos;

	for (n=0;n!=iface.halo_list.nhalo;n++) {
		view_images_free_single(halo->image_idx);
		halo++;
	}

		// labels

	label=iface.label_list.labels;

	for (n=0;n!=iface.label_list.nlabel;n++) {
		view_images_free_single(label->image_idx);
		label++;
	}

		// crosshairs

	crosshair=iface.crosshair_list.crosshairs;

	for (n=0;n!=iface.crosshair_list.ncrosshair;n++) {
		view_images_free_single(crosshair->image_idx);
		crosshair++;
	}

		// remote bitmaps

	if (net_setup.mode!=net_mode_none) {
		view_images_free_single(remote_slow_image_idx);
		view_images_free_single(remote_talk_image_idx);
	}
	
		// virtual controls
		
#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)

	stick=iface.virtual_control.sticks;
	
	for (n=0;n!=max_virtual_stick;n++) {
	
		if ((stick->on) && (stick->use_bitmap)) {
			view_images_free_single(stick->outer_image_idx);
			view_images_free_single(stick->inner_image_idx);
		}
		
		stick++;
	}

	button=iface.virtual_control.buttons;
	
	for (n=0;n!=max_virtual_button;n++) {
	
		if ((button->on) && (button->use_bitmap)) {
			view_images_free_single(button->up_image_idx);
			view_images_free_single(button->down_image_idx);
		}
		
		button++;
	}

#endif
}

/* =======================================================

      Get Images
      
======================================================= */

bool view_images_is_empty(int idx)
{
	return(idx==0);
}

bitmap_type* view_images_get_bitmap(int idx)
{
	view_image_type			*image;
	
		// sanity check for bad bitmaps
		
	if ((idx<0) || (idx>=max_view_image)) idx=0;
	
	image=&view.images[idx];
	return(&image->bitmap);
}

unsigned long view_images_get_gl_id(int idx)
{
	bitmap_type			*bitmap;
	
	bitmap=view_images_get_bitmap(idx);
	return(bitmap->gl_id);
}
